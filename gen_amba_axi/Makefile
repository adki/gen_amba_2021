#-----------------------------------------------------------------------------
SHELL		= /bin/sh
MAKEFILE	= Makefile

#-------------------------------------------------------------
ARCH	= $(shell uname)
MACH	= $(shell uname -m)
ifeq ($(ARCH),Linux)
  PLATFORM= linux
  GCC=gcc
  ifeq ($(MACH),x86_64)
        PLATFORMX= linux64
        INSTD=../../bin/linux64
  else
        PLATFORMX= linux32
        INSTD=../../bin/linux32
  endif
else ifeq ($(findstring CYGWIN_NT,$(ARCH)),CYGWIN_NT)
  PLATFORM= cygwin
  GCC=i686-w64-mingw32-gcc
  ifeq ($(MACH),x86_64)
        PLATFORMX= cygwin64
        INSTD=../../bin/win64
  else
        PLATFORMX= cygwin32
        INSTD=../../bin/win32
  endif
else ifeq ($(findstring MINGW,$(ARCH)),MINGW)
  PLATFORM= mingw
  GCC=gcc
  ifeq ($(MACH),x86_64)
        PLATFORMX= mingw64
        INSTD=../../bin/win64
  else
        PLATFORMX= mingw32
        INSTD=../../bin/win32
  endif
else
  $(info un-supported platform $(ARCH))
endif

#-------------------------------------------------------------
CC              = gcc
DEFS            =
INCDIRS         = -Isrc
LIBS            =
ifeq ($(PLATFORM),cygwin)
DEFS            += -DWIN32
LIBS            +=
else ifeq ($(PLATFORM),mingw)
DEFS            += -DWIN32
LIBS            +=
endif
OPT             = -g
CCFLAGS         = $(DEFS) $(OPT)

#-------------------------------------------------------------
TARGET		= gen_amba_axi

SRC	=\
	main.c\
	arg_parser.c\
	gen_axi_amba.c\
	gen_axi_utils.c\
	gen_axi_mtos.c\
	gen_axi_stom.c\
	gen_axi_arbiter_mtos.c\
	gen_axi_arbiter_stom.c\
	gen_axi_default_slave.c\
        gen_axi_wid.c
		
OBJS	= $(SRC:.c=.o)

#-------------------------------------------------------------
H_DIR		= src
C_DIR		= src

vpath %.h	$(H_DIR)
vpath %.c	$(C_DIR)

#-------------------------------------------------------------
OBJECTDIR = obj
DUMMY      := $(shell [ -d $(OBJECTDIR) ] || mkdir $(OBJECTDIR) )

#-------------------------------------------------------------
$(OBJECTDIR)/%.o: %.c
	$(CC) -c $(CCFLAGS) $(INCDIRS) -o $@ $<

#-------------------------------------------------------------
all: $(TARGET)

$(TARGET): $(addprefix $(OBJECTDIR)/, $(OBJS))
	$(CC) -o $(TARGET) $^ $(LIBS)

run: $(TARGET)
	./$(TARGET) --master=2 --slave=3 --output=amba_axi_m2s3.v

#-------------------------------------------------------------
install: $(TARGET)
	@if [ ! -d $(INSTD) ]; then mkdir -p $(INSTD); fi
	cp $(TARGET) $(INSTD)/$(TARGET)
	chmod +x $(INSTD)/$(TARGET)

#-------------------------------------------------------------
DIRS	= $(subst /,, $(dir $(wildcard */Makefile)))

clean:
	-rm -fr $(OBJECTDIR)
	-rm -fr ./Debug
	-rm -f *.stackdump
	-rm -f compile.log
	-rm -f *.v
	for D in $(DIRS); do\
		if [ -f $$D/Makefile ] ; then \
			echo "make -C $$D -s $@";\
			make -C $$D -s $@;\
		fi;\
	done

cleanup clobber: clean
	-rm -f $(TARGET) $(TARGET).exe
	for D in $(DIRS); do\
		if [ -f $$D/Makefile ] ; then \
			echo "make -C $$D -s $@";\
			make -C $$D -s $@;\
		fi;\
	done

cleanupall distclean: cleanup
	for D in $(DIRS); do\
		if [ -f $$D/Makefile ] ; then \
			echo "make -C $$D -s $@";\
			make -C $$D -s $@;\
		fi;\
	done
