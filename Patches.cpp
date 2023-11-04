#include "Patches.h"
#include <sys/fcntl.h>
#include <mach-o/loader.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <map>
//#include <CoreFoundation/CoreFoundation.h>

using namespace std;

#include <sys/syslog.h>
//don't reboot userspace when enabled this
#define LOG(...)    //    {openlog("AutoPatches",LOG_PID,LOG_AUTH);syslog(LOG_DEBUG, "AutoPatches: " __VA_ARGS__);closelog();}

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



#define arm64_trunc_page(x) ((x) & (~(0x1000 - 1)))
#define arm64_round_page(x) trunc_page((x) + (0x1000 - 1))

// borrow from gdb, refer: binutils-gdb/gdb/arch/arm.h
#define submask(x) ((1L << ((x) + 1)) - 1)
#define bits(obj, st, fn) (((obj) >> (st)) & submask((fn) - (st)))
#define bit(obj, st) (((obj) >> (st)) & 1)
#define sbits(obj, st, fn) ((long)(bits(obj, st, fn) | ((long)bit(obj, fn) * ~submask(fn - st))))

static inline int decode_rd(uint32_t instr) {
  return bits(instr, 0, 4);
}

static inline int decode_rn(uint32_t instr) {
  return bits(instr, 5, 9);
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

void patch_textcode(map<uint64_t,string>& cstrings, void* header, uint64_t addresss, uint64_t size)
{
    uint32_t* p = (uint32_t*)addresss;
    while(p < (uint32_t*)(addresss+size))
    {
        uint32_t code = *p;
        uint64_t module_offset = (uint64_t)p - (uint64_t)header;

        // is ADR/ADRP
        if( (code&0x1F000000)==0x10000000 )
        {
            // is ADR reg,cstring ?
            if((code&0x9F000000)==0x10000000)
            {
                int rd = decode_rd(code);
                int64_t offset = decode_adr_imm_value(code);
                uint64_t addr = (uint64_t)p + offset;
                
                //LOG("%p/%llX:ADR rd=%d, offset=%llx, addr=%llx", p, module_offset, rd, offset, addr);

                auto it = cstrings.find(addr);
                if(it != cstrings.end()) 
                {
                    LOG("string ref ADR: %p/%llX %llx, %s", p, module_offset, addr, (char*)addr);
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
                    uint64_t addr = arm64_trunc_page((uint64_t)p + page);

                    //LOG("%p/%llX:ADRP rd=%d, page=%llx, addr=%llx", p, module_offset, rd, page, addr);
                    
                    int rd2 = decode_rd(code2);
                    int rn2 = decode_rn(code2);
                    
                    int64_t offset = decode_add_imm12_value(code2);
                    
                    addr += offset;
                    
                    //LOG("%p/%llX:ADD rd=%d, rn=%d, offset=%llx, addr=%llx", p, module_offset+4, rd2, rn2, offset, addr);
                    
                    if(rd==rd2 && rd==rn2)
                    {
                        auto it = cstrings.find(addr);
                        if(it != cstrings.end()) 
                        {
                            LOG("string ref ADRL: %p/%llX %llx, %s", p, module_offset, addr, (char*)addr);
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
    LOG("cstring_section=%llx, %llx", cstring_section->addr, cstring_section->size);
    map<uint64_t,string> cstrings = scan_cstrings(cstring_addr, cstring_section->size);
    LOG("cstrings count=%ld", cstrings.size());

    if(cstrings.size()==0)
        return;

    uint64_t text_addr = ((uint64_t)slide + text_section->addr);
    LOG("text_section=%llx, %llx", text_section->addr, text_section->size);
    patch_textcode(cstrings, header, text_addr, text_section->size);

    if(cfstring_section) {
    LOG("cfstring_section=%llx, %llx", cfstring_section->addr, cfstring_section->size);
        uint64_t cfstring_addr = ((uint64_t)slide + cfstring_section->addr);
        patch_cfstrings(cstrings, cfstring_addr, cfstring_section->size);
    }

    if(data_section) {
        LOG("data_section=%llx, %llx", data_section->addr, data_section->size);
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

    if(strncmp(path, "/var/jb/", sizeof("/var/jb/")-1) == 0)
    {
        path = jbroot(path);
    }

    return fopen(path, mode);
}

FILE * my_freopen( const char *path, const char *mode, FILE *stream )
{
    LOG("freopen=%s, %s, %p", path, mode, stream);

    return freopen(path,mode,stream);
}


void* my_opendir(char* path)
{
    LOG("opendir=%s", path);
    return opendir(path);
}

int my_open(const char* path, int mode, int flag)
{
    LOG("open %s %x %x", path, mode, flag);
    return open(path, mode, flag);
}

int my_stat(char* path, struct stat* st)
{
    LOG("stat=%s", path);
    return stat(path, st);
}


extern "C" {
void hook_$sSS6appendyySSF(uint64_t p)
{
    uint64_t a1 = *(uint64_t*)(p + 0);
    uint64_t a2 = *(uint64_t*)(p + 8);
    const char* str = NULL;

    const char* varjb = "/var/jb";
    const char* path = "/var/mobile/Library/Preferences/";

    if(a1 == (0xd000000000000000+strlen(path)) 
        && (a2 & 0x8000000000000000) != 0 )
    {
        // str = (const char*)( (a2 & ~0x8000000000000000) + 0x20);
        // if(strncmp(str, path, strlen(path)) == 0)
        // {
        //     const char* newpath = jbroot(str);
        //     uint64_t new_a2 = ( (uint64_t)newpath | 0x8000000000000000 ) - 0x20;
        //     *(uint64_t*)(p + 8) = new_a2;
        //     *(uint64_t*)(p + 0) = 0xd000000000000000+strlen(newpath);
        // }
    }
    else if(a1==*(uint64_t*)varjb &&  a2==0xE700000000000000) {
        str = varjb;
        const char* newpath = jbroot(str);
        uint64_t new_a2 = ( (uint64_t)newpath | 0x8000000000000000 ) - 0x20;
        *(uint64_t*)(p + 8) = new_a2;
        *(uint64_t*)(p + 0) = 0xd000000000000000+strlen(newpath);
    }

    LOG("hook_sSS6appendyySSF %p %p %s", (void*)a1, (void*)a2, str);
}

void* (*orig_$sSS6appendyySSF)()=NULL;
__attribute__((naked)) void* my_$sSS6appendyySSF()
{
  __asm(
    "sub sp,sp,0x100 \n"
    "stp x0,x1,[sp,0] \n"
    "stp x2,x3,[sp,0x10] \n"
    "stp x4,x5,[sp,0x20] \n"
    "stp x6,x7,[sp,0x30] \n"
    "stp x8,x9,[sp,0x40] \n"
    "stp x10,x11,[sp,0x50] \n"
    "stp x12,x13,[sp,0x60] \n"
    "stp x14,x15,[sp,0x70] \n"
    "stp x29,x30,[sp,0x80] \n"
    "mov x0, x20 \n"
    "bl _hook_$sSS6appendyySSF\n"
    "ldp x0,x1,[sp,0] \n"
    "ldp x2,x3,[sp,0x10] \n"
    "ldp x4,x5,[sp,0x20] \n"
    "ldp x6,x7,[sp,0x30] \n"
    "ldp x8,x9,[sp,0x40] \n"
    "ldp x10,x11,[sp,0x50] \n"
    "ldp x12,x13,[sp,0x60] \n"
    "ldp x14,x15,[sp,0x70] \n"
    "ldp x29,x30,[sp,0x80] \n"
    "add sp,sp,0x100 \n"
    "adrp x16, _orig_$sSS6appendyySSF@PAGE \n"
	"ldr x16, [x16, _orig_$sSS6appendyySSF@PAGEOFF] \n"
    "br x16 \n"
    );
}
}

// CFStringRef my_CFStringCreateWithCString(CFAllocatorRef alloc, const char *cStr, CFStringEncoding encoding)
// {
//     LOG("CFStringCreateWithCString: %s", cStr);
//     return CFStringCreateWithCString(alloc, cStr, encoding);
// }

ssize_t  my_readlink(const char * path, char * buf, size_t bufsize)
{
    LOG("readlink %s", path);
    if(strncmp(path, "/var/jb", sizeof("/var/jb")-1)==0) {
        const char* newpath = jbroot(path);
        LOG("newpath %s", path);
        snprintf(buf,bufsize,"%s", newpath);
        return strlen(newpath);
    }

    return readlink(path,buf,bufsize);
}

#include <spawn.h>
int my_posix_spawn(pid_t * pidp, const char * path, const posix_spawn_file_actions_t *fap, const posix_spawnattr_t * attrp, char *const __argv[], char *const __envp[]) 
{
    if(strncmp(path, "/var/jb", sizeof("/var/jb")-1)==0) {
        path = jbroot(path);
    }

    return posix_spawn(pidp,path,fap,attrp, __argv,__envp);
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

void hook_api_symbole(const char* path, const char* sym, void* func, void** orig=NULL)
{
    LOG("hook %s for %s", sym, rootfs(path));
    dobby_dummy_func_t tmp;
    DobbyImportTableReplace((char*)path, (char*)sym, (dobby_dummy_func_t)func, (dobby_dummy_func_t*)&tmp);
    if(orig) *orig = (void*)tmp;
}

extern "C"
__attribute__ ((visibility ("default"))) //invoke by RootHide PatchLoader
void InitPatches(const char* path, void* header, uint64_t slide)
{
    hash<string> h;
    const char* jbpath = rootfs(path);
    size_t pathval = h(string(jbpath));
    LOG("**load %p, %p, %s, %lx", header, (void*)slide, jbpath, pathval);

    dobby_enable_near_branch_trampoline();

    bool autopatch=true;

    switch(pathval)
    {
        case 0x4b4ec095aaeca265:
            hook_api_symbole(path, "access", (void*)my_access);
            break;
        
        case 0x52da202616603413:
        case 0xc5a37085e8684d22:
            hook_api_symbole(path, "fopen", (void*)my_fopen);
            break;

        case 0xd1701f0d57c79a90:
            autopatch = false;
            // hook Swift function/api ?
            hook_api_symbole(path, "$sSS6appendyySSF", (void*)my_$sSS6appendyySSF, (void**)&orig_$sSS6appendyySSF);
            //hook_api_symbole(path, "CFStringCreateWithCString", (void*)my_CFStringCreateWithCString);
            hook_api_symbole(path, "readlink", (void*)my_readlink);
            break;

        case 0x9fa30a20e83aa366:
            hook_api_symbole(path, "open", (void*)my_open);
            hook_api_symbole(path, "fopen", (void*)my_fopen);
            hook_api_symbole(path, "freopen", (void*)my_freopen);
            hook_api_symbole(path, "access", (void*)my_access);
            break;

        case 0x2ea4e9938f958ba5:
            hook_api_symbole(path, "access", (void*)my_access);
            hook_api_symbole(path, "posix_spawn", (void*)my_posix_spawn);
    }

   if(autopatch) auto_patch_machO((struct mach_header_64*)header, slide);

}
