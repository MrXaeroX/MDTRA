#
# MDTRA Lua Makefile for x86 Linux
#

DLLNAME=lua

ARCH=i686
OPTIMIZE=2

#make sure this is the correct compiler for your system
CC=gcc
CP=cp
STRIP=strip -x

DLL_SRCDIR=../../src/lua
DLL_DSTDIR=../../bin
DLL_OBJDIR=./obj-$(ARCH)

SHARED_SRCDIR=../../src/shared

BASE_CFLAGS= -D_LINUX -DNDEBUG -DLUA_USE_LINUX

#full optimization
CFLAGS=$(BASE_CFLAGS) -m32 -Wall -O$(OPTIMIZE) -mtune=$(ARCH) -march=pentium -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -fno-strict-aliasing

INCLUDEDIRS=-I. -I$(SHARED_SRCDIR)

LDFLAGS=-lm -Wl,-E -ldl
LDVFLAGS=

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

$(DLL_OBJDIR)/%.o: $(DLL_SRCDIR)/%.c
	$(DO_CC)

OBJ = \
	$(DLL_OBJDIR)/lapi.o \
	$(DLL_OBJDIR)/lauxlib.o \
	$(DLL_OBJDIR)/lbaselib.o \
	$(DLL_OBJDIR)/lcode.o \
	$(DLL_OBJDIR)/lcompiler.o \
	$(DLL_OBJDIR)/ldblib.o \
	$(DLL_OBJDIR)/ldebug.o \
	$(DLL_OBJDIR)/ldo.o \
	$(DLL_OBJDIR)/ldump.o \
	$(DLL_OBJDIR)/lfunc.o \
	$(DLL_OBJDIR)/lgc.o \
	$(DLL_OBJDIR)/linit.o \
	$(DLL_OBJDIR)/liolib.o \
	$(DLL_OBJDIR)/llex.o \
	$(DLL_OBJDIR)/lmathlib.o \
	$(DLL_OBJDIR)/lmem.o \
	$(DLL_OBJDIR)/loadlib.o \
	$(DLL_OBJDIR)/lobject.o \
	$(DLL_OBJDIR)/lopcodes.o \
	$(DLL_OBJDIR)/loslib.o \
	$(DLL_OBJDIR)/lparser.o \
	$(DLL_OBJDIR)/lpreprocessor.o \
	$(DLL_OBJDIR)/lscriptlib.o \
	$(DLL_OBJDIR)/lstate.o \
	$(DLL_OBJDIR)/lstring.o \
	$(DLL_OBJDIR)/lstrlib.o \
	$(DLL_OBJDIR)/ltable.o \
	$(DLL_OBJDIR)/ltablib.o \
	$(DLL_OBJDIR)/ltm.o \
	$(DLL_OBJDIR)/lundump.o \
	$(DLL_OBJDIR)/lvm.o \
	$(DLL_OBJDIR)/lzio.o \
	$(DLL_OBJDIR)/print.o

$(DLLNAME).$(SHLIBEXT) : neat $(OBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDVFLAGS)
	$(STRIP) $(DLLNAME).$(SHLIBEXT)
	$(CP) $(DLLNAME).$(SHLIBEXT) $(DLL_DSTDIR)/$(DLLNAME).$(SHLIBEXT)
	$(CP) $(DLLNAME).$(SHLIBEXT) ../$(DLLNAME).$(SHLIBEXT)

neat:
	-mkdir -p $(DLL_OBJDIR)

clean:
	-rm -f $(OBJ)
	-rm -f $(DLLNAME).$(SHLIBEXT)
spotless: clean
	-rm -r $(DLL_OBJDIR)
