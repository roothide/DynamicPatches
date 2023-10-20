#include "Patches.h"
#include <sys/fcntl.h>
#include <mach-o/loader.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <map>

using namespace std;

#include <sys/syslog.h>
#define LOG(...)   //{openlog("AutoPatches",LOG_PID,LOG_AUTH);syslog(LOG_DEBUG, "AutoPatches: " __VA_ARGS__);closelog();}

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"

class PATCH {
public:
    int reg;
    string str;
};

map<uint64_t,PATCH*> gPatchesRuntime;
static pthread_mutex_t gPatchesLock=PTHREAD_MUTEX_INITIALIZER;

void patches_handler(void *address, DobbyRegisterContext *ctx)
{
    pthread_mutex_lock(&gPatchesLock);
    PATCH* patch = gPatchesRuntime[(uint64_t)address];
    pthread_mutex_unlock(&gPatchesLock);
    assert(patch != NULL);

    LOG("handle %p %s", address, patch->str.c_str());

    const char* oldpath = patch->str.c_str();
    const char* newpath = jbroot(oldpath);
    LOG("oldpath=%s", oldpath);
    LOG("newpath=%s", newpath);
    ctx->general.x[patch->reg] = (uint64_t)newpath;
}

void add_codepatch(void* addr, int reg, char* str)
{
    PATCH* patch = new PATCH;

    patch->reg = reg;
    patch->str = string(str);

    pthread_mutex_lock(&gPatchesLock);
    gPatchesRuntime[(uint64_t)addr] = patch;
    pthread_mutex_unlock(&gPatchesLock);

    DobbyInstrument((void*)addr, patches_handler);
}

map<uint64_t,string> scan_cstrings(uint64_t addr, uint64_t size)
{
    map<uint64_t,string> strings;

    char* p = (char*)addr;
    while(p < (char*)(addr+size))
    {
        int l = strlen(p);
        if(l) {
            //LOG("cstring %s @ %p", p, p);
            if(strstr(p, "/var/jb")) {
                strings[(uint64_t)p] = string(p);
            }
        }

        p += l+1;
    }

    return strings;
}

void patch_cfstrings(map<uint64_t,string>& cstrings, uint64_t addr, uint64_t size)
{
    for(int i=0; i<(size/sizeof(struct __CFString)); i++)
    {
        struct __CFString cfstr = ((struct __CFString*)addr)[i];
        //LOG("cfstr: %s %d", cfstr.buffer, cfstr.length);
        auto it = cstrings.find((uint64_t)cfstr.buffer);
        if(it != cstrings.end())
        {
            LOG("new cfstr: %s %d", cfstr.buffer, cfstr.length);

            cfstr.buffer = jbroot(cfstr.buffer);
            cfstr.length = strlen(cfstr.buffer);

            DobbyCodePatch((void*)&((struct __CFString*)addr)[i], (uint8_t*)&cfstr, sizeof(cfstr));
        }
    }
}

void patch_pointers(map<uint64_t,string>& cstrings, uint64_t addr, uint64_t size)
{
    for(int i=0; i<(size/sizeof(char*)); i++)
    {
        char** p = &((char**)addr)[i];
        auto it = cstrings.find((uint64_t)*p);
        if(it != cstrings.end()) {
            LOG("string pointer: %p, %p, %s", p, *p, *p);
        }
    }
}

// borrow from gdb, refer: binutils-gdb/gdb/arch/arm.h
#define submask(x) ((1L << ((x) + 1)) - 1)
#define bits(obj, st, fn) (((obj) >> (st)) & submask((fn) - (st)))
#define bit(obj, st) (((obj) >> (st)) & 1)
#define sbits(obj, st, fn) ((long)(bits(obj, st, fn) | ((long)bit(obj, fn) * ~submask(fn - st))))

static inline int decode_rd(uint32_t instr) {
  return bits(instr, 0, 4);
}

static inline int decode_rn(uint32_t instr) {
  return bits(instr, 5, 4);
}

static inline int64_t SignExtend(unsigned long x, int M, int N) {
#if 1
  char sign_bit = bit(x, M - 1);
  unsigned long sign_mask = 0 - sign_bit;
  x |= ((sign_mask >> M) << M);
#else
  x = (long)((long)x << (N - M)) >> (N - M);
#endif
  return (int64_t)x;
}

static inline int64_t decode_adr_imm_value(uint32_t instr) {
  typedef uint32_t instr_t;
  struct {
    instr_t Rd : 5;      // Destination register
    instr_t immhi : 19;  // 19-bit upper immediate
    instr_t dummy_0 : 5; // Must be 10000 == 0x10
    instr_t immlo : 2;   // 2-bit lower immediate
    instr_t op : 1;      // 0 = ADR, 1 = ADRP
  } instr_decode;

  *(instr_t *)&instr_decode = instr;

  int64_t imm = instr_decode.immlo + (instr_decode.immhi << 2);
  imm = SignExtend(imm, 2 + 19, 64);
  return imm;
}

static inline int64_t decode_adrp_imm_value(uint32_t instr) {
  int64_t imm = decode_adr_imm_value(instr);
  imm = imm << 12;
  return imm;
}

//https://developer.arm.com/documentation/ddi0596/2021-12/Base-Instructions/ADD--immediate---Add--immediate--?lang=en
static inline int64_t decode_add_imm12_value(uint32_t instr)
{
    int32_t imm12 = (instr & 0x3FFC00) >> 10;
    uint8_t shift = (instr>>22) & 1;
    int64_t offset = shift ? (imm12 << 12) : imm12;
    return offset;
}

void patch_textcode(map<uint64_t,string>& cstrings, uint64_t addr, uint64_t size)
{
    uint32_t* p = (uint32_t*)addr;
    while(p < (uint32_t*)(addr+size))
    {
        uint32_t code = *p;

        // is ADR/ADRP
        if( (code&0x1F000000)==0x10000000 )
        {
            // is ADR reg,cstring ?
            if((code&0x9F000000)==0x10000000)
            {
                int rd = decode_rd(code);
                int64_t offset = decode_adr_imm_value(code);
                uint64_t addr = (uint64_t)p + offset;
                
                //LOG("%p:ADR rd=%d, offset=%llx, addr=%llx", p, rd, offset, addr);

                auto it = cstrings.find(addr);
                if(it != cstrings.end()) 
                {
                    LOG("string ref ADR: %p %llx, %s", p, addr, (char*)addr);
                    uint32_t nop = 0xD503201F;
                    if(DobbyCodePatch((void*)p, (uint8_t*)&nop, sizeof(nop)) == 0)
                        add_codepatch((void*)p, rd, (char*)addr);
                }
            }
            // is ADRP reg, cstring@PAGE ?
            else if((code&0x9F000000)==0x90000000)
            {
                uint32_t code2 = *(p+1);
                // is ADD reg, cstring@PAGE_OFF ?
                if ((code2&0xFF800000)==0x91000000)
                {
                    int rd = decode_rd(code);
                    int64_t page = decode_adrp_imm_value(code);
                    uint64_t addr = ((uint64_t)p & ~(PAGE_SIZE-1)) + page;

                    //LOG("%p:ADRP rd=%d, page=%llx, addr=%llx", p, rd, page, addr);
                    
                    int rd2 = decode_rd(code2);
                    int rn2 = decode_rn(code2);
                    
                    int64_t offset = decode_add_imm12_value(code2);
                    
                    addr += offset;
                    
                    //LOG("%p:ADD rd=%d, rs=%d, offset=%llx, addr=%llx", p+1, rd2, rn2, offset, addr);
                    
                    if(rd==rd2 && rd==rn2)
                    {
                        auto it = cstrings.find(addr);
                        if(it != cstrings.end()) 
                        {
                            LOG("string ref ADRL: %p %llx, %s", p, addr, (char*)addr);
                            uint64_t nops = 0xD503201FD503201F;
                            if(DobbyCodePatch((void*)p, (uint8_t*)&nops, sizeof(nops)) == 0)
                                add_codepatch((void*)p, rd, (char*)addr);
                        }
                    }

                    p++; //goto next
                }
            }
        }

        p++;
    }
}

void auto_patch_machO(struct mach_header_64* header, uint64_t slide)
{
    struct section_64* text_section=NULL;
    struct section_64* data_section=NULL;
    struct section_64* cstring_section=NULL;
    struct section_64* cfstring_section=NULL;

    struct load_command* lc = (struct load_command*)((uint64_t)header + sizeof(*header));
    for (int i = 0; i < header->ncmds; i++) {
        
        if (lc->cmd == LC_SEGMENT_64)
        {
            struct segment_command_64 * seg = (struct segment_command_64 *) lc;
            
            LOG("segment: %s file=%llx:%llx vm=%llx:%llx\n", seg->segname, seg->fileoff, seg->filesize, seg->vmaddr, seg->vmsize);
            
            struct section_64* sec = (struct section_64*)((uint64_t)seg+sizeof(*seg));
            for(int j=0; j<seg->nsects; j++)
            {
                //LOG("section[%d] = %s/%s offset=%x vm=%llx:%llx", j, sec[j].segname, sec[j].sectname, sec[j].offset, sec[j].addr, sec[j].size);

                if(strcmp(seg->segname, SEG_TEXT)==0) 
                {
                    if(strcmp(sec[j].sectname, SECT_TEXT)==0) {
                        text_section = &sec[j];
                    }
                    if(strcmp(sec[j].sectname, "__cstring")==0) {
                        cstring_section = &sec[j];
                    }
                }

                if(strcmp(seg->segname, SEG_DATA)==0 || strcmp(seg->segname, "__DATA_CONST")==0) 
                {
                    if(strcmp(sec[j].sectname, SECT_DATA)==0) {
                        data_section = &sec[j];
                    }
                    if(strcmp(sec[j].sectname, "__cfstring")==0) {
                        cfstring_section = &sec[j];
                    }
                }
            }
        }
        
        lc = (struct load_command *) ((char *)lc + lc->cmdsize);
    }

    LOG("sections: %p %p %p %p", 
    text_section, cstring_section, cfstring_section, data_section
    );

    if(!text_section || !cstring_section) {
        LOG("parse sections error!");
        return;
    }

    uint64_t cstring_addr = ((uint64_t)slide + cstring_section->addr);
    LOG("header=%p cstring=%llx,%llx", header, cstring_section->addr, cstring_section->size);
    map<uint64_t,string> cstrings = scan_cstrings(cstring_addr, cstring_section->size);
    LOG("cstrings count=%ld", cstrings.size());

    if(cstrings.size()==0)
        return;

    uint64_t text_addr = ((uint64_t)slide + text_section->addr);
    patch_textcode(cstrings, text_addr, text_section->size);

    if(cfstring_section) {
        uint64_t cfstring_addr = ((uint64_t)slide + cfstring_section->addr);
        patch_cfstrings(cstrings, cfstring_addr, cfstring_section->size);
    }

    if(data_section) {
        uint64_t data_addr = ((uint64_t)slide + data_section->addr);
        patch_pointers(cstrings, data_addr, data_section->size);
    }

}


int my_access(char* path, int mode)
{
    LOG("access=%s, %d", path, mode);
    if(strncmp(path, "/var/jb", sizeof("/var/jb")-1)==0) 
        path = (char*)jbroot(path);
    return access(path,mode);
}

void* my_fopen(const char* path, const char* mode)
{
    LOG("fopen=%s, %s", path, mode);

    const char* jbfile = rootfs(path);

    char mirrorfile[PATH_MAX];
    snprintf(mirrorfile,sizeof(mirrorfile),"%s/%s", jbroot("/var/mobile/Library/pkgmirror"), jbfile);

    LOG("check mirror file for %s => %s", jbfile, mirrorfile);

    if(access(mirrorfile, F_OK)==0)
    {
        LOG("exist mirror file for %s", jbfile);
        int fd=open(mirrorfile, 0);
        if(fd >= 0) {
            uint32_t magic=0;
            read(fd, &magic, sizeof(magic));
            close(fd);

            if(magic==0xBEBAFECA||magic==0xCAFEBABE||magic==0xFEEDFACF)
            {
                LOG("use mirror file for %s", jbfile);
                path = mirrorfile;
            }
        }
    }

    return fopen(path, mode);
}

void* my_opendir(char* path)
{
    LOG("opendir=%s", path);
    return opendir(path);
}

int my_stat(char* path, struct stat* st)
{
    LOG("stat=%s", path);
    return stat(path, st);
}

bool pathFileEqual(const char* path1, const char* path2)
{
	if(!path1 || !path2) return false;

	struct stat st1;
	if(stat(path1, &st1) != 0)
		return false;

	struct stat st2;
	if(stat(path2, &st2) != 0)
		return false;
	
	if(st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino) 
		return false;

	return true;
}

void hook_api_symbole(const char* path, const char* sym, void* func)
{
    LOG("hook %s for %s", sym, rootfs(path));
    dobby_dummy_func_t tmp;
    DobbyImportTableReplace((char*)path, (char*)sym, (dobby_dummy_func_t)func, (dobby_dummy_func_t*)&tmp);
}

extern "C"
__attribute__ ((visibility ("default"))) //invoke by RootHide PatchLoader
void InitPatches(const char* path, void* header, uint64_t slide)
{
    hash<string> h;
    const char* jbpath = rootfs(path);
    size_t pathval = h(string(jbpath));
    LOG("**load %p,%p,%s,%lx", header, (void*)slide, jbpath, pathval);

    dobby_enable_near_branch_trampoline();

    auto_patch_machO((struct mach_header_64*)header, slide);

    switch(pathval)
    {
        case 0x4b4ec095aaeca265:
            hook_api_symbole(path, "access", (void*)my_access);
            break;
        
        case 0x52da202616603413:
        case 0xc5a37085e8684d22:
            hook_api_symbole(path, "fopen", (void*)my_fopen);
            break;
    }

}
