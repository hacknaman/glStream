ifneq ($(ARCH), WIN_NT)
ifneq ($(ARCH), WIN_98)
ARCH=$(shell uname | sed -e 's/-//g')
endif
endif

ifeq ($(ARCH),CYGWIN_NT5.0)
ARCH=WIN_NT
endif

ECHO := echo

include $(TOP)/config/$(ARCH).mk

###########################
# LEAVE THESE THINGS ALONE!
###########################

ifdef PROGRAM
BUILDDIR := $(TOP)/built/$(PROGRAM)
else
ifdef LIBRARY
BUILDDIR := $(TOP)/built/$(LIBRARY)
else
BUILDDIR := dummy_builddir
endif
endif

ifdef MPI
CR_CC = $(MPI_CC)
CR_CXX = $(MPI_CXX)
MPI_STRING = (MPI)
LDFLAGS += $(MPI_LDFLAGS)
else
CR_CC = $(CC)
CR_CXX = $(CXX)
endif

ifdef VTK
VTK_STRING = (VTK)
ifdef WINDOWS
CXXFLAGS += -Ic:/vtk31/graphics -Ic:/vtk31/common -Ic:/vtk31/imaging -Ic:/vtk31/local
LDFLAGS += c:/vtkbin/Debug/lib/vtkCommon.lib c:/vtkbin/Debug/lib/vtkContrib.lib c:/vtkbin/Debug/lib/vtkGraphics0.lib c:/vtkbin/Debug/lib/vtkGraphics1.lib c:/vtkbin/Debug/lib/vtkGraphics2.lib c:/vtkbin/Debug/lib/vtkGraphics3.lib c:/vtkbin/Debug/lib/vtkGraphics4.lib c:/vtkbin/Debug/lib/vtkImaging.lib 
else
error VTK on non-windows platform?
endif
endif

ifdef GLUT
ifdef WINDOWS
LDFLAGS += glut32.lib
else
LDFLAGS += -L/usr/X11R6/lib -lglut
endif
OPENGL = 1
endif

ifdef OPENGL
ifdef WINDOWS
LDFLAGS += glu32.lib opengl32.lib
else
LDFLAGS += -L/usr/X11R6/lib -lGLU -lGL -lXmu -lXi -lX11
endif
endif


OBJDIR := $(BUILDDIR)/$(ARCH)
DEPDIR := $(BUILDDIR)/$(ARCH)/dependencies
BINDIR := $(TOP)/bin/$(ARCH)
ifdef WINDOWS
DSO_DIR := $(BINDIR)
else
DSO_DIR := $(TOP)/lib/$(ARCH)
endif

define MAKE_OBJDIR
	if test ! -d $(OBJDIR); then $(MKDIR) $(OBJDIR); fi
endef

define MAKE_BINDIR
	if test ! -d $(BINDIR); then $(MKDIR) $(BINDIR); fi
endef

define MAKE_DEPDIR
	if test ! -d $(DEPDIR); then $(MKDIR) $(DEPDIR); fi
endef

define MAKE_DSODIR
	if test ! -d $(DSO_DIR); then $(MKDIR) $(DSO_DIR); fi
endef

ifdef TEST
FILES := $(TEST)
endif

DEPS    := $(addprefix $(DEPDIR)/, $(FILES))
DEPS    := $(addsuffix .depend, $(DEPS))
OBJS    := $(addprefix $(OBJDIR)/, $(FILES))
OBJS    := $(addsuffix $(OBJSUFFIX), $(OBJS))
ifdef LIBRARY
ifdef SHARED
LIBNAME := $(addprefix $(OBJDIR)/, $(LIBPREFIX)$(LIBRARY)$(DLLSUFFIX))
else
LIBNAME := $(addprefix $(OBJDIR)/, $(LIBPREFIX)$(LIBRARY)$(LIBSUFFIX))
endif
else
LIBNAME := dummy_libname
endif

TEMPFILES := *~ \\\#*\\\#

ifdef PROGRAM
PROG_TARGET := $(BINDIR)/$(PROGRAM)
TARGET := $(PROGRAM)
else
PROG_TARGET := dummy_prog_target
endif

ifdef LIBRARY
ifdef SHARED
TARGET := $(LIBPREFIX)$(LIBRARY)$(DLLSUFFIX)
else
TARGET := $(LIBPREFIX)$(LIBRARY)$(LIBSUFFIX)
endif
endif

ifndef TARGET
TARGET := NOTHING
endif

ifeq ($(INCLUDEDEPS), 1)
ifneq ($(DEPS)HACK, HACK)
include $(DEPS)
endif
endif

INCLUDE_DIRS += -I$(TOP)/include -I.

PRINT_COMMAND := lpr

ifeq ($(HUMPER_AT_STANFORD),1)
INCLUDE_DIRS += -I/usr/graphics/include -I/usr/common/include
LDFLAGS += -L/usr/common/lib32 -L/usr/graphics/lib32
endif

CFLAGS += -D$(ARCH) $(INCLUDE_DIRS)
CXXFLAGS += -D$(ARCH) $(INCLUDE_DIRS)

ifdef LESSWARN
WARN_STRING = (NOWARN)
else
CFLAGS += $(FULLWARN)
endif

ifdef RELEASE
CFLAGS += $(RELEASEFLAGS)
CXXFLAGS += $(RELEASEFLAGS)
LDFLAGS += $(RELEASEFLAGS)
RELEASE_STRING = (RELEASE)
RELEASE_FLAGS = "RELEASE=1"
else
ifdef PROFILE
CFLAGS += $(DEBUGFLAGS) $(PROFILEFLAGS)
CXXFLAGS += $(DEBUGFLAGS) $(PROFILEFLAGS)
LDFLAGS += $(DEBUGFLAGS) $(PROFILEFLAGS)
RELEASE_STRING = (PROFILE)
else
CFLAGS += $(DEBUGFLAGS)
CXXFLAGS += $(DEBUGFLAGS)
LDFLAGS += $(DEBUGFLAGS)
RELEASE_STRING = (DEBUG)
endif
endif

ifdef WINDOWS
LDFLAGS += /link /incremental:no /pdb:none
ifndef RELEASE
LDFLAGS += /debug
endif
endif

ifndef SUBDIRS
all: arch $(PRECOMP) dep
recurse: $(PROG_TARGET) $(LIBNAME) done
else
SUBDIRS_ALL = $(foreach dir, $(SUBDIRS), $(dir).subdir)

subdirs: $(SUBDIRS_ALL)

$(SUBDIRS_ALL):
	@$(MAKE) -C $(basename $@) $(RELEASE_FLAGS)
endif

release:
	@$(MAKE) RELEASE=1

profile:
	@$(MAKE) PROFILE=1

done:
	@$(ECHO) "  Done!"
	@$(ECHO) ""

arch: 
	@$(ECHO) "-------------------------------------------------------------------------------"
ifdef BANNER
	@$(ECHO) "              $(BANNER)"
else
ifdef PROGRAM
	@$(ECHO) "              Building $(TARGET) for $(ARCH) $(RELEASE_STRING) $(MPI_STRING) $(VTK_STRING) $(WARN_STRING)"
endif
ifdef LIBRARY
	@$(ECHO) "              Building $(TARGET) for $(ARCH) $(RELEASE_STRING) $(MPI_STRING) $(VTK_STRING) $(WARN_STRING)"
endif
endif
	@$(ECHO) "-------------------------------------------------------------------------------"
ifneq ($(BUILDDIR), dummy_builddir)
	@$(MAKE_BINDIR)
	@$(MAKE_OBJDIR)
	@$(MAKE_DEPDIR)
	@$(MAKE_DSODIR)
endif

ifdef WINDOWS

LIBRARIES := $(foreach lib,$(LIBRARIES),$(TOP)/built/$(lib)/$(ARCH)/$(LIBPREFIX)$(lib)$(LIBSUFFIX))
#LIBRARIES := $(LIBRARIES:$(DLLSUFFIX)=$(LIBSUFFIX))

else

LDFLAGS += -L$(DSO_DIR)
LIBRARIES := $(foreach lib,$(LIBRARIES),-l$(lib))

endif


dep: $(DEPS)
	@$(MAKE) $(PARALLELMAKEFLAGS) recurse INCLUDEDEPS=1

$(PROG_TARGET): $(OBJS)
ifdef PROGRAM
	@$(ECHO) "Linking $(PROGRAM) for $(ARCH)"
ifdef WINDOWS
	@$(CR_CXX) $(OBJS) /Fe$(PROG_TARGET)$(EXESUFFIX) $(LIBRARIES) $(LDFLAGS)
else
	@$(CR_CXX) $(OBJS) -o $(PROG_TARGET)$(EXESUFFIX) $(LDFLAGS) $(LIBRARIES)
endif

endif

$(LIBNAME): $(OBJS) $(LIB_DEFS)
ifdef LIBRARY
	@$(ECHO) "Linking $@"
ifdef WINDOWS
ifdef SHARED
	@$(LD) $(SHARED_LDFLAGS) /Fe$(LIBNAME) $(OBJS) $(LIBRARIES) $(LIB_DEFS) $(LDFLAGS)
else
	@$(LD) $(OBJS) $(LIBRARIES) $(LDFLAGS)
endif #shared
else
ifdef SHARED
	@$(LD) $(SHARED_LDFLAGS) -o $(LIBNAME) $(OBJS) $(LDFLAGS) $(LIBRARIES)
else
	@$(ECHO) How do I make static libraries on $(ARCH)?
endif #shared
endif

ifndef DONT_COPY_DLL
	@$(CP) $(LIBNAME) $(DSO_DIR)
endif
endif

.SUFFIXES: .cpp .c .cxx .cc .C .s .l

%.cpp: %.l
	@$(ECHO) "Creating $@"
	@$(LEX) $< > $@

%.cpp: %.y
	@$(ECHO) "Creating $@"
	@$(YACC) $<
	@$(MV) y.tab.c $@

$(DEPDIR)/%.depend: %.cpp
	@$(MAKE_DEPDIR)
	@$(ECHO) "Rebuilding dependencies for $<"
	@$(PERL) $(TOP)/scripts/fastdep.pl $(INCLUDE_DIRS) --obj-prefix='$(OBJDIR)/' --extra-target=$@ $< > $@

$(DEPDIR)/%.depend: %.cxx
	@$(MAKE_DEPDIR)
	@$(ECHO) "Rebuilding dependencies for $<"
	@$(PERL) $(TOP)/scripts/fastdep.pl $(INCLUDE_DIRS) --obj-prefix='$(OBJDIR)/' --extra-target=$@ $< > $@

$(DEPDIR)/%.depend: %.cc
	@$(MAKE_DEPDIR)
	@$(ECHO) "Rebuilding dependencies for $<"
	@$(PERL) $(TOP)/scripts/fastdep.pl $(INCLUDE_DIRS) --obj-prefix='$(OBJDIR)/' --extra-target=$@ $< > $@

$(DEPDIR)/%.depend: %.C
	@$(MAKE_DEPDIR)
	@$(ECHO) "Rebuilding dependencies for $<"
	@$(PERL) $(TOP)/scripts/fastdep.pl $(INCLUDE_DIRS) --obj-prefix='$(OBJDIR)/' --extra-target=$@ $< > $@

$(DEPDIR)/%.depend: %.c
	@$(MAKE_DEPDIR)
	@$(ECHO) "Rebuilding dependencies for $<"
	@$(PERL) $(TOP)/scripts/fastdep.pl $(INCLUDE_DIRS) --obj-prefix='$(OBJDIR)/' --extra-target=$@ $< > $@

$(DEPDIR)/%.depend: %.s
	@$(MAKE_DEPDIR)
	@$(ECHO) "Rebuilding dependencies for $<"
	@$(PERL) $(TOP)/scripts/fastdep.pl $(INCLUDE_DIRS) --obj-prefix='$(OBJDIR)/' --extra-target=$@ $< > $@

$(OBJDIR)/%.obj: %.cpp Makefile
	@$(ECHO) -n "Compiling "
	@$(CR_CXX) /Fo$@ /c $(CXXFLAGS) $<

$(OBJDIR)/%.obj: %.c Makefile
	@$(ECHO) -n "Compiling "
	@$(CR_CC) /Fo$@ /c $(CFLAGS) $<

$(OBJDIR)/%.o: %.cpp Makefile
	@$(ECHO) "Compiling $<"
	@$(CR_CXX) -o $@ -c $(CXXFLAGS) $<

$(OBJDIR)/%.o: %.cxx Makefile
	@$(ECHO) "Compiling $<"
	@$(CR_CXX) -o $@ -c $(CXXFLAGS) $<

$(OBJDIR)/%.o: %.cc Makefile
	@$(ECHO) "Compiling $<"
	@$(CR_CXX) -o $@ -c $(CXXFLAGS) $<

$(OBJDIR)/%.o: %.C Makefile
	@$(ECHO) "Compiling $<"
	@$(CR_CXX) -o $@ -c $(CXXFLAGS) $<

$(OBJDIR)/%.o: %.c Makefile
	@$(ECHO) "Compiling $<"
	@$(CR_CC) -o $@ -c $(CFLAGS) $<

$(OBJDIR)/%.o: %.s Makefile
	@$(ECHO) "Assembling $<"
	@$(AS) -o $@ $<


###############
# Other targets
###############

clean:
ifdef SUBDIRS
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
else
ifdef LIBRARY
	@$(ECHO) "Removing all $(ARCH) object files for $(TARGET)."
else
ifdef PROGRAM
	@$(ECHO) "Removing all $(ARCH) object files for $(PROGRAM)."
endif
endif
endif
	@$(RM) $(OBJS) $(TEMPFILES)
ifneq ($(SLOP)HACK, HACK)
	@$(ECHO) "Also blowing away:    $(SLOP)"
	@$(RM) $(SLOP)
endif


ifdef SUBDIRS
clobber:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clobber; done
else
clobber: clean
ifdef LIBRARY
	@$(ECHO) "Removing $(LIBNAME) for $(ARCH)."
	@$(RM) $(LIBNAME)
else
ifdef PROGRAM
	@$(ECHO) "Removing $(PROGRAM) for $(ARCH)."
	@$(RM) $(PROGRAM)
	@$(RM) $(BINDIR)/$(PROGRAM)
endif
endif
	@$(ECHO) "Removing dependency files (if any)"
	@$(RM) $(DEPDIR)/*.depend
endif
