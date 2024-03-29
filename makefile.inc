# makefile.inc (oilChecker)
# This file contains all of the environment definitions
# common to each of the makefiles within the project. 
#

# Include directories that are not already on the path
# DO NOT include the -I prefix to these paths - it will
# be added automatically
INCDIRS_TEMP = \
	$(CURDIR)/src \
	$(CURDIR)/src/utilities

INCDIRS = $(addprefix -I,$(INCDIRS_TEMP))

# For 3rd party files - will be included as system files so warinings
# are ignored.  These macros must be defined in your bashrc file.
LIB_INCDIRS_TEMP = \
	$(EIGEN)

LIB_INCDIRS = $(addprefix -isystem,$(LIB_INCDIRS_TEMP))

# Library directories that are not already on the path
# DO NOT include the -L prefix to these paths - it will
# be added automatically
LIBDIRS_TEMP = \
	$(RPI)/lib

LIBDIRS = $(addprefix -L,$(LIBDIRS_TEMP))

# Libraries to link against
# DO NOT include the -l prefix to these libraries - it
# will be added automatically
LIBS_TEMP = \
	wiringPi \
	rt

LIBS = $(addprefix -l,$(LIBS_TEMP))

# Static libraries to be build before the executable
# MUST be listed in order of dependence (i.e. first
# library must not be needed by other libraries and
# it must need information contained in the following
# libraries).
PSLIB = 

# Compiler to use
#CC = arm-linux-gnueabihf-g++
CC = g++

# Archiver to use
AR = ar rcs
RANLIB = ranlib

# Compiler flags
CFLAGS = -Wall -Wextra -Wno-psabi $(LIB_INCDIRS) -std=c++17  $(INCDIRS) `pkg-config --cflags libcurl openssl`
CFLAGS_RELEASE = $(CFLAGS) -O2
CFLAGS_DEBUG = $(CFLAGS) -g

# Linker flags
LDFLAGS = $(LIBDIRS) $(LIBS) `pkg-config --libs libcurl openssl` -pthread -lstdc++fs
LDFLAGS_DEBUG = $(LDFLAGS)
LDFLAGS_RELEASE = $(LDFLAGS)

# Object file output directory
OBJDIR = $(CURDIR)/.obj/
OBJDIR_DEBUG = $(OBJDIR)debug/
OBJDIR_RELEASE = $(OBJDIR)release/

# Binary file output directory
BINDIR = $(CURDIR)/bin/

# Library output directory
LIBOUTDIR = $(CURDIR)/.lib/

# Method for creating directories
MKDIR = mkdir -p

# Method for removing files
RM = rm -f
