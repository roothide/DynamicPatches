ARCHS = arm64 arm64e
TARGET := iphone:clang:latest:15.0

THEOS_PACKAGE_SCHEME = roothide
FINALPACK=1
DEBUG=0

include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = AutoPatches

$(LIBRARY_NAME)_FILES = Patches.cpp Patches.m
$(LIBRARY_NAME)_CFLAGS = -fvisibility=hidden -fobjc-arc
$(LIBRARY_NAME)_CCFLAGS = -fvisibility=hidden -std=c++11
$(LIBRARY_NAME)_LDFLAGS = -L./ -ldobby #elleki may crash when be loaded in image_load_notiry -lsubstrate
$(LIBRARY_NAME)_INSTALL_PATH = /usr/lib/DynamicPatches

include $(THEOS_MAKE_PATH)/library.mk

clean::
	rm -rf ./packages/*
