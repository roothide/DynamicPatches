ARCHS = arm64 arm64e
TARGET := iphone:clang:latest:15.0

THEOS_PACKAGE_SCHEME = roothide

include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = FilzaPatches

$(LIBRARY_NAME)_FILES = Patches.mm
$(LIBRARY_NAME)_CFLAGS = -fobjc-arc
$(LIBRARY_NAME)_LDFLAGS = -L./ -ldobby
$(LIBRARY_NAME)_INSTALL_PATH = /usr/lib/DynamicPatches

include $(THEOS_MAKE_PATH)/library.mk


PATCH_FILES = \
	"/Applications/Filza.app/Filza" \
	"/Applications/Filza.app/PlugIns/Sharing.appex/Sharing" \
	"/usr/libexec/filza/Filza" \
	"/usr/libexec/filza/FilzaHelper" \
	"/usr/libexec/filza/FilzaWebDAVServer"


before-package::
	for file in $(PATCH_FILES); do \
		echo add patch file $$file at $$(dirname "$$file"); \
		dir=$$(dirname "$$file"); \
		mkdir -p "$(THEOS_STAGING_DIR)/$$dir" ; \
		ln -s "/usr/lib/DynamicPatches/$(LIBRARY_NAME).dylib" "$(THEOS_STAGING_DIR)/$$file.roothidepatch"; \
	done;

clean::
	rm -rf ./packages/*
	