# Dynamic Patches
 RootHide Dynamic Patches, a tweak for tweaks.

# Filza Patches

Filza should be the most complex single jailbreak software.

the current branch is Filza Patches, which is also a patches template. 

you can check out the different branches of this repository.

# How It Works

each roothide dynamic patches module is loaded by [PatchLoader](https://github.com/RootHide/PatchLoader) when a mach-o module is loaded into memory by dyld (before actual execution).
and PatchLoader is similar to TweakLoader, but it is loaded before TweakLoader, so it can patch tweaks.

# Packages

we need to create a specific separate patches package for a specific jailbreak app/tweak package (of course the patches package can also be merged with the original package into one package).

the original package needs to be processed as follows:

1. repackage according to roothide specifications.
2. modify the ```Version``` field in the control file and add the ```-rootide``` suffix
3. add ```Pre-Depends: patches-PackageID(= PackageVersion-roothide)``` to control file

usually these are handled automatically by the RootHidePatcher app.

and the control file of the patches package needs to add the following fields:
```
Section: RootHide Patches
Depends: com.roothide.patchloader
Provides: patches-PackageID(= PackageVersion-roothide)
Recommends: PackageID(= PackageVersion-roothide)
Conflicts: PackageID(>> PackageVersion-roothide), PackageID(<< PackageVersion-roothide)
```

**all PackageIDs and PackageVersion above need to be consistent with the original package.**

# Patches Trigger

just like we need to specify the executables/bundles/classes  in plist to load the tweak, for dynamic patches package we need to specify which mach-o files in the original package need to apply patches.

in the roothide dynamic patching framework we do not use plist for configuration, since the mach-o we need to patch are all in the jailbreak file system, so we use a simpler binding method, a suffixed symbolic link, we create a symbolic link with ```.roothidepatch``` suffix for each mach-o that needs to be patched, and link to our patches module (dylib). in this template project, you only need to add the paths to PATCH_FILES in the Makefile.

# Create Patches

usually tweaks are written in a mix of C/Objective-C, which means they mainly use char/NSString strings to write paths. we can easily find all path strings in the binary and the code that uses them through the tool IDA, then we can make a patch list of all instruction addresses and registers. 

for the NSString constant, it is compiled into a __CFString structure, which contains a pointer to a char string. if this char string is not used separately in C language code, then we can directly patch the __CFString constant, rather than caring about the code that uses them, this can simplify some processes.

# Examples

we can easily find all path strings in IDA:
<img width="1103" alt="image" src="https://github.com/RootHide/DynamicPatches/assets/134120506/5182be4c-0022-4be9-bccd-690d2547a5ac">

### patch path string of C language in code
we can use IDA's xref(cross-reference) to find all code that uses this path string:
<img width="892" alt="image" src="https://github.com/RootHide/DynamicPatches/assets/134120506/928b9a5f-0238-4b73-819f-1f13464a9b4a">

here we can see that this path string is put into the X19 register, 
so we patch it in the next-line (0x01001C3C58) and convert the path to the jailbreak-directory (jbroot):
<img width="925" alt="image" src="https://github.com/RootHide/DynamicPatches/assets/134120506/dc61bf18-6e36-4996-b173-b71330fd2a26">

so in the end the patch we got is as follows:
```
@{@"vaddr": @0x01001C3C58, @"regs":@[ @(19) ], @"type":@"cstring", @"action":@"jbroot"}
```

### patch NSString path constants

while a cstring is referenced by __CFString, we can directly patch the __CFString without continuing to care about the code that uses this __CFString. 
but if other codes still reference this cstring, we still need to create separate cstring patches for them.

for example The path string below is also referenced by __CFString and other code as a cstring:
<img width="839" alt="image" src="https://github.com/RootHide/DynamicPatches/assets/134120506/9b6e8eb3-7221-4b29-a401-87b912ddf0c8">

the cstring path referenced by __CFString in 0x0100700D78
![image](https://github.com/RootHide/DynamicPatches/assets/134120506/26bdc9d9-950d-49fd-af79-17e615907f10)

the cstring path referenced by code here:
![image](https://github.com/RootHide/DynamicPatches/assets/134120506/8581e384-7501-473b-ae46-f3b7489d2111)

the cstring path referenced by another code here:
![image](https://github.com/RootHide/DynamicPatches/assets/134120506/e935bc8d-1fb8-4e68-a575-5e1bb249568a)


so in the end the patches we got is as follows:
```
@{@"vaddr": @0x0100700D78, @"type":@"__CFString", @"action":@"jbroot"},
@{@"vaddr": @0x0100117604, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
@{@"vaddr": @0x01002640CC, @"regs":@[ @(0) ], @"type":@"cstring", @"action":@"jbroot"},
```

## Notice

1. we can also use ```@"type":@"NSString"``` to patch some path string variables returned by Objective-C methods.
   
2. some jailbreak apps/tweaks may execute command line programs of bootstrap, which only accept jbroot-based paths as arguments.
   we can use ```@:"action":@"rootfs"``` to convert the rootfs-based paths to jbroot-based paths in code.
   and if a path needs to be saved to the config/plist file, we also need to perform this conversion, 
   since the randomized path converted by jbroot will be invalid in the next time we jailbreak.
   
3. after a path string are put into a register, this register may be used in multiple places in subsequent code.
   usually we patch it in the first place where it is used, but we may need to convert it back to original path in the next place of code.
   we can use ```action``` of ```jbroot```<=>```rootfs``` to convert path strings back and forth.

4. usually a patch will only replace one line of code, but in some cases a patch may also require space for three lines of code,
   so it is recommended that the address of 2 code patches should be separated by 3x4=12 bytes.
   
