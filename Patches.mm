#include "Patches.h"

#define LOG(...) NSLog(@"FilzaPatches: " __VA_ARGS__)

NSDictionary* gPatchesConfig = @{

    @"/Applications/Filza.app/Filza" : @[

        // "/var/lib/filza/sconf.plist"
        @{@"vaddr": @0x0100713418, @"type":@"__CFString", @"action":@"jbroot"},
        // "/var/mobile/Library/Filza/scripts"
        @{@"vaddr": @0x0100700D78, @"type":@"__CFString", @"action":@"jbroot"},
        // "/var/mobile/Library/Filza/tmp"
        @{@"vaddr": @0x0100701818, @"type":@"__CFString", @"action":@"jbroot"},
        // "/var/mobile/Library/Filza"
        @{@"vaddr": @0x01007121B8, @"type":@"__CFString", @"action":@"jbroot"},
        //"/var/root/.ssh/id_rsa"
        @{@"vaddr": @0x010070A4D8, @"type":@"__CFString", @"action":@"jbroot"},
        // "/var/mobile/Library/Preferences/com.tigisoftware.Filza.plist"
        @{@"vaddr": @0x0100713B98, @"type":@"__CFString", @"action":@"jbroot"},
        //"/var/mobile/Library/Filza/.Trash"
        @{@"vaddr": @0x0100713C38, @"type":@"__CFString", @"action":@"jbroot"},

        // "/var/mobile/Documents"
        @{@"vaddr": @0x0100707A38, @"type":@"__CFString", @"action":@"jbroot"},
        // "/var/mobile/Downloads"
        @{@"vaddr": @0x0100713598, @"type":@"__CFString", @"action":@"jbroot"},
        // "/var/tmp"
        @{@"vaddr": @0x010070ACD8, @"type":@"__CFString", @"action":@"jbroot"},

/////////////////////////////////////////////////////////////////////////////////////////////////

        //[TGPreferences defaultPath]
        @{@"vaddr": @0x01002689C0, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"jbroot"},
        //[TGPreferences downloadDirectory]
        @{@"vaddr": @0x01002661F4, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"jbroot"},
        //[TGPreferences tempDirectory]
        @{@"vaddr": @0x01002661F4, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"jbroot"},


        //ScriptsExecutor_ExecuteScriptWithReturnString_fileItems_parentController_userInputs_completionHandler___block_invoke
        @{@"vaddr": @0x01002516D4, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x0100251738, @"regs":@[ @(21) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x0100251A74, @"regs":@[ @(21) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x0100251AFC, @"regs":@[ @(21) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x0100251AB8, @"regs":@[ @(21) ], @"type":@"NSString", @"action":@"rootfs"},

        //[TGPreferences createHttpServer]
        @{@"vaddr": @0x0100267844, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x0100267A04, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},

        //[AddLinkViewController viewDidLoad]
        @{@"vaddr": @0x01002A1F8C, @"regs":@[ @(2) ], @"type":@"NSString", @"action":@"rootfs"},

        //[FavoritesTableViewController tableView:didSelectRowAtIndexPath:]
        @{@"vaddr": @0x010017EE18, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"jbroot"},
        @{@"vaddr": @0x010017F098, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"jbroot"},

        //-[LeftPanelTableViewController tableView:didSelectRowAtIndexPath:]
        @{@"vaddr": @0x0100140C2C, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"jbroot"},
        
        //_TGDatabaseInterface_libDirectoryPath__block_invoke
        @{@"vaddr": @0x01002329F8, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x0100232A10, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x0100232A58, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x0100232A6C, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},

        //InitPaths
        @{@"vaddr": @0x01001C4094, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},

        //[TGPreferences init]
        @{@"vaddr": @0x01002640D0, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x01002643F0, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x0100264408, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},


        //_TGSettingsRestoreController_showInController_fromRect_animated___block_invoke
        @{@"vaddr": @0x0100117608, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},

        //_copyExistPath for rootful
        @{@"vaddr": @0x1001C4DC4, @"regs":@[ @(3) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x1001C4DF4, @"regs":@[ @(3) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x1001C4E24, @"regs":@[ @(3) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x1001C4E50, @"regs":@[ @(3) ], @"type":@"cstring", @"action":@"jbroot"},

        //_dpkgInfo
        @{@"vaddr": @0x10020DA64, @"regs":@[ @(19) ], @"type":@"cstring", @"action":@"rootfs"},

        //[TGRootFileManager dpkgInfo:]
        @{@"vaddr": @0x100106D3C, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"rootfs"},

        //[Zipper createDEB:toFilePath:]
        @{@"vaddr": @0x1001DD820,  @"regs":@[ @(0), @(19) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x1001DD834,  @"regs":@[ @(19) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x19

        //[InstallerViewController install:]
        @{@"vaddr": @0x01000C830C, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01000C85D0, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01000C852C, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},

        //[Zipper unDEBFile:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001DF560, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DF570, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DF584, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20

        @{@"vaddr": @0x01001DF754, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DF764, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DF778, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20

        @{@"vaddr": @0x01001DF8D8, @"regs":@[ @(0), @(25) ], @"type":@"NSString", @"action":@"rootfs"},

        // TGSystem
        @{@"vaddr": @0x010020E308, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"pathenv_jbroot"},

        //InstallerViewController_respring__block_invoke
        @{@"vaddr": @0x01000CAC20, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},

        //[Zipper unZipFile:toPath:currentDirectory:outMessage:]
         @{@"vaddr": @0x01001DE330, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
         @{@"vaddr": @0x01001DE0B0, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},

        //[Zipper unZipFile:toPath:currentDirectory:withPassword:outMessage:]
         @{@"vaddr": @0x01001DE6CC, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
         @{@"vaddr": @0x01001DE93C, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
         //using 7z
         @{@"vaddr": @0x01001DEAA0, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
         @{@"vaddr": @0x01001DEAA8, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"},
         @{@"vaddr": @0x01001DEB2C, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
         @{@"vaddr": @0x01001DED50, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
         @{@"vaddr": @0x01001DED58, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"},
         @{@"vaddr": @0x01001DEDE0, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
         

        //[Zipper ZipFiles:toFilePath:currentDirectory:]
        @{@"vaddr": @0x01001DDA68, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DDA8C, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20

        //[Zipper unTarFile:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001E123C, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E1248, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
        @{@"vaddr": @0x01001E1254, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},

        @{@"vaddr": @0x01001E1498, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E14A4, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
        @{@"vaddr": @0x01001E14B0, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},

        // [Zipper isProtectedRarFile:signatureOK:]
        @{@"vaddr": @0x01001DD198, @"regs":@[ @(19) ], @"type":@"NSString", @"action":@"rootfs"},
    
        //[Zipper unRarFile:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001DFADC, @"regs":@[ @(0), @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DFB24, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
        @{@"vaddr": @0x01001DFD38, @"regs":@[ @(0), @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DFD80, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
         
        //[Zipper unRarFile:toPath:currentDirectory:withPassword:outMessage:]
        @{@"vaddr": @0x01001E0058, @"regs":@[ @(0), @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E00A0, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20
        @{@"vaddr": @0x01001E02EC, @"regs":@[ @(0), @(20) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E0334, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"}, //revert x20

        //[Zipper unGzipFile:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001E174C, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E1A54, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        
        //[Zipper unBzip2File:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001E0608, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E0910, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        
        //[Zipper unXZipFile:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001E0C18, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001E0F10, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        
        //[Zipper un7ZipFile:toPath:currentDirectory:outMessage:]
        @{@"vaddr": @0x01001DF04C, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DF054, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"}, //revert x20
        @{@"vaddr": @0x01001DF0B8, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"},

        @{@"vaddr": @0x01001DF2DC, @"regs":@[ @(0) ], @"type":@"NSString", @"action":@"rootfs"},
        @{@"vaddr": @0x01001DF2E4, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"rootfs"}, //revert x20
        @{@"vaddr": @0x01001DF344, @"regs":@[ @(20) ], @"type":@"NSString", @"action":@"jbroot"},
        
    ],

    @"/usr/libexec/filza/FilzaHelper" : @[
        // TGSystem
        @{@"vaddr": @0x0100005CC4, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"pathenv_jbroot"},

        //CopyExistPath
        @{@"vaddr": @0x100005520, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x100005548, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x100005570, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x100005598, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},
        @{@"vaddr": @0x1000055C0, @"regs":@[ @(2) ], @"type":@"cstring", @"action":@"jbroot"},
    ],

    @"/usr/libexec/filza/FilzaWebDAVServer" : @[
        //"/var/lib/filza/sconf.plist"
        @{@"vaddr": @0x010002A898, @"type":@"__CFString", @"action":@"jbroot"},

        //HTTPCCreate:appBundlePath
        @{@"vaddr": @0x010000B478, @"regs":@[ @(4) ], @"type":@"NSString", @"action":@"jbroot"},
    ],

    @"/Applications/Filza.app/PlugIns/Sharing.appex/Sharing" : @[
    ],

    
    @"/usr/libexec/filza/Filza" : @[
        //hacky way to skip check installed path
        @{@"vaddr": @0x1000075C0, @"regs":@[ @(21) ], @"type":@"cstring", @"action":@"rootfs"},

        @{@"vaddr": @0x100007228, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"pathenv_jbroot"},
    ],
};

NSLock* gPatchesLock = nil;
NSMutableDictionary* gPatchesRuntime = nil;

const char* convertPath(NSDictionary* patch, const char* path)
{
    const char* newpath=NULL;

    if([patch[@"action"] isEqualToString:@"jbroot"]) {
        newpath = jbroot(path);
    } else if([patch[@"action"] isEqualToString:@"rootfs"]) {
        newpath = rootfs(path);
    } else if([patch[@"action"] isEqualToString:@"pathenv_jbroot"]) {

        NSArray* paths = [@(path) componentsSeparatedByString:@":"];
        NSMutableArray* newpaths = [[NSMutableArray alloc] init];
        [paths enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            [newpaths addObject:jbroot((NSString*)obj)];
        }];

        NSString* newpathenv = [newpaths componentsJoinedByString:@":"];
        newpath = newpathenv.UTF8String;

    } else if([patch[@"action"] isEqualToString:@"pathenv_rootfs"]) {

        NSArray* paths = [@(path) componentsSeparatedByString:@":"];
        NSMutableArray* newpaths = [[NSMutableArray alloc] init];
        [paths enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            [newpaths addObject:rootfs((NSString*)obj)];
        }];

        NSString* newpathenv = [newpaths componentsJoinedByString:@":"];
        newpath = newpathenv.UTF8String;

    } else {
        abort();
    }
    return newpath;
}

__attribute__((overloadable))
NSString* convertPath(NSDictionary* patch, NSString* path)
{
    NSString* newpath=NULL;

    if([patch[@"action"] isEqualToString:@"jbroot"]) {
        newpath = jbroot(path);
    } else if([patch[@"action"] isEqualToString:@"rootfs"]) {
        newpath = rootfs(path);
    } else if([patch[@"action"] isEqualToString:@"pathenv"]) {
        
    } else if([patch[@"action"] isEqualToString:@"pathenv_jbroot"]) {

        NSArray* paths = [path componentsSeparatedByString:@":"];
        NSMutableArray* newpaths = [[NSMutableArray alloc] init];
        [paths enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            [newpaths addObject:jbroot((NSString*)obj)];
        }];

        NSString* newpathenv = [newpaths componentsJoinedByString:@":"];
        newpath = newpathenv;

    } else if([patch[@"action"] isEqualToString:@"pathenv_rootfs"]) {

        NSArray* paths = [path componentsSeparatedByString:@":"];
        NSMutableArray* newpaths = [[NSMutableArray alloc] init];
        [paths enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            [newpaths addObject:rootfs((NSString*)obj)];
        }];

        NSString* newpathenv = [newpaths componentsJoinedByString:@":"];
        newpath = newpathenv;

    } else {
        abort();
    }
    return newpath;
}

void patches_handler(void *address, DobbyRegisterContext *ctx)
{
    [gPatchesLock lock];
    NSDictionary* patch = gPatchesRuntime[@((uint64_t)address)];
    [gPatchesLock unlock];

    LOG(@"handle %p %@", address, patch);

    assert(patch != NULL);

    void (^block)(DobbyRegisterContext*,NSDictionary*) = patch[@"block"];
    if(block) {
        block(ctx, patch);
        return;
    }

    if([patch[@"type"] isEqualToString:@"cstring"])
    {
        for(NSNumber* item in patch[@"regs"])
        {
            int reg = [item intValue];
            const char* oldpath = (const char*)(ctx->general.x[reg]);
            const char* newpath = convertPath(patch, oldpath);
            LOG(@"oldpath=%s", oldpath);
            LOG(@"newpath=%s", newpath);
            ctx->general.x[reg] = (uint64_t)newpath;
        }
    }
    else if([patch[@"type"] isEqualToString:@"NSString"])
    {
        for(NSNumber* item in patch[@"regs"])
        {
            int reg = [item intValue];
            NSString* oldpath = (__bridge NSString*)(void*)(ctx->general.x[reg]);
            NSString* newpath = convertPath(patch, oldpath);
            LOG(@"oldpath=%@, newpath=%@", oldpath, newpath);
            ctx->general.x[reg] = (uint64_t)(void*)CFBridgingRetain(newpath);
        }
    }
    else {
        abort();
    }
}


BOOL pathFileEqual(const char* path1, const char* path2)
{
	if(!path1 || !path2) return NO;

	struct stat st1;
	if(stat(path1, &st1) != 0)
		return NO;

	struct stat st2;
	if(stat(path2, &st2) != 0)
		return NO;
	
	if(st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino) 
		return NO;

	return YES;
}

void (*orig_InitPaths)()=NULL;
void new_InitPaths()
{
    LOG(@"InitPaths");

    orig_InitPaths();

    const char* pathvars[] = {
        "_FilzaReloadPath", "_FilzaCmdPath", 
        //"_WebDAVPath", //write to daemon plist handle by launchctl
        "_LaunchDaemonWD",
        "_BashPath", "_ZshPath",
        "_ApplicationsPrefix", "_APrefix", "_UICachePrefix",
        "_p7zipPath", //required bcz /usr/bin/7z is a shell wrapper
    };

    const char* FilzaPath = jbroot("/Applications/Filza.app/Filza");

    for(int i=0; i<sizeof(pathvars)/sizeof(pathvars[0]); i++) {
        char* _var = (char*)DobbySymbolResolver(FilzaPath, pathvars[i]);
        LOG(@"pathvar[%d] %s %s", i, pathvars[i], _var);
        assert(_var != NULL);
        strcpy(_var, jbroot(_var[0]?_var:"/"));
    }

    //fix the bug that webdavserver cannot be actively started in settings of filza
    char* _LaunchStartWD = (char*)DobbySymbolResolver(FilzaPath, "_LaunchStartWD");
    strcpy(_LaunchStartWD, "launchctl start com.tigisoftware.filza.webdavserver");
}

#include <spawn.h>

int (*orig_posix_spawn)(pid_t * pid, const char * path, const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t * attrp, char *const argv[], char *const envp[])=NULL;
int new_posix_spawn(pid_t * pid, const char * path, const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t * attrp, char *const argv[], char *const envp[])
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    LOG(@"cwd=%s", cwd);
    LOG(@"posix_spawn=%s %p %p", path, attrp, file_actions);
    if(argv) for(int i=0; argv[i]; i++) {
        LOG(@"argv[%d]: %s", i, argv[i]);
    }
    if(envp) for(int i=0; envp[i]; i++) {
        LOG(@"envp[%d]: %s", i, envp[i]);
    }


    // int fds[2]={0};
    // assert(pipe(fds)==0);
    
    // posix_spawn_file_actions_t child_fd_actions;
    // posix_spawn_file_actions_init(&child_fd_actions);
    // posix_spawn_file_actions_adddup2(&child_fd_actions, fds[1], STDOUT_FILENO);
    // posix_spawn_file_actions_adddup2(&child_fd_actions, fds[1], STDERR_FILENO);
    // posix_spawn_file_actions_addclose(&child_fd_actions, fds[0]);
    // posix_spawn_file_actions_addclose(&child_fd_actions, fds[1]);

    // file_actions = &child_fd_actions;


    int ret = orig_posix_spawn(pid, path, file_actions, attrp, argv, envp);
    LOG(@"posix_spawn ret=%d pid=%d", ret, *pid);


    // char buffer[1024];
    // close(fds[1]);
    // while(read(fds[0], buffer, sizeof(buffer)))
    //     LOG(@"spawnOutput: %s", buffer);

    return ret;
}

void (*orig_importSettingsFromNoContainerFilza)()=NULL;
void new_importSettingsFromNoContainerFilza()
{
    LOG(@"Skip importSettingsFromNoContainerFilza");
}

extern "C"
__attribute__ ((visibility ("default"))) //invoke by RootHide PatchLoader
void InitPatches(const char* path, void* header, uint64_t slide)
{
    LOG(@"load %p,%p,%s", header, (void*)slide, path);

    if(!gPatchesLock) {
        gPatchesLock = [[NSLock alloc] init];
    }
    if(!gPatchesRuntime) {
        gPatchesRuntime = [[NSMutableDictionary alloc] init];
    }

    const char* FilzaPath = jbroot("/Applications/Filza.app/Filza");
    if(pathFileEqual(path, FilzaPath))
    {
        void* _InitPaths = DobbySymbolResolver(FilzaPath, "_InitPaths");
        assert(_InitPaths != NULL);
        DobbyHook((void*)_InitPaths, (void*)new_InitPaths, (void**)&orig_InitPaths);

        void* importSettingsFromNoContainerFilza = DobbySymbolResolver(FilzaPath, "_objc_msgSend$importSettingsFromNoContainerFilza");
        assert(importSettingsFromNoContainerFilza != NULL);
        DobbyHook((void*)importSettingsFromNoContainerFilza, (void*)new_importSettingsFromNoContainerFilza, (void**)&orig_importSettingsFromNoContainerFilza);

        //skip symlinks when creating zip file
        __CFString* ZipFileOptions = (__CFString*)(0x01007105F8 + slide);
        ZipFileOptions->buffer = "-rqy";
        ZipFileOptions->length = 4;
    }

    //test
    //DobbyHook((void*)posix_spawn, (void*)new_posix_spawn, (void**)&orig_posix_spawn);

    dobby_enable_near_branch_trampoline();

    for(NSString* keypath in gPatchesConfig)
    {
        NSArray* patches = gPatchesConfig[keypath];

        if(!pathFileEqual(path, jbroot(keypath).UTF8String))
        {
            continue;
        }

        for(NSDictionary* patch in patches)
        {
            uint64_t addr = [patch[@"vaddr"] unsignedLongValue] + slide;
            LOG(@"load %p %@", (void*)addr, patch);

            if([patch[@"type"] isEqualToString:@"__CFString"]) {
                __CFString* str = (__CFString*)addr;
                const char* newpath = convertPath(patch, str->buffer);
                LOG(@"__CFString %s => %s", str->buffer, newpath);
                str->buffer = newpath;
                str->length = strlen(newpath);
                continue;
            }
            

            [gPatchesLock lock];
            gPatchesRuntime[@(addr)] = patch;
            [gPatchesLock unlock];

            DobbyInstrument((void*)addr, patches_handler);
        }

        break;

    }

}
