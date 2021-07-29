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
CC              = ${GCC}
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
TARGET		= gen_amba_ahb

SRC	=\
	main.c\
	arg_parser.c\
	gen_ahb_amba.c\
	gen_ahb_arbiter.c\
	gen_ahb_m2s.c\
	gen_ahb_lite.c\
	gen_ahb_decoder.c\
	gen_ahb_s2m.c\
	gen_ahb_default_slave.c
		
OBJS	= $(SRC:.c=.o)

#-------------------------------------------------------------
H_DIR		= src
C_DIR		= src

vpath %.h	$(H_DIR)
vpath %.c	$(C_DIR)

#-------------------------------------------------------------
ifndef OBJECTDIR
  OBJECTDIR = obj
endif
ifeq (${wildcard $(OBJECTDIR)},)
  DUMMY := ${shell mkdir $(OBJECTDIR)}
endif

#-------------------------------------------------------------
$(OBJECTDIR)/%.o: %.c
	$(CC) -c $(CCFLAGS) $(INCDIRS) -o $@ $<

#-------------------------------------------------------------
all: $(TARGET)

$(TARGET): $(addprefix $(OBJECTDIR)/, $(OBJS))
	$(CC) -g -o $(TARGET) $^ $(LIBS)

run: $(TARGET)
	./$(TARGET) --master=3 --slave=4 --output=amba_ahb_m3s4.v

#-------------------------------------------------------------
install: $(TARGET)
	@if [ ! -d $(INSTD) ]; then mkdir -p $(INSTD); fi
	cp $(TARGET) $(INSTD)/$(TARGET)
	chmod +x $(INSTD)/$(TARGET)

#-------------------------------------------------------------
clean:
	-rm -fr $(OBJECTDIR)
	-rm -fr ./Debug
	-rm -f *.stackdump
	-rm -f compile.log
	-rm -f *.v

cleanup clobber: clean
	-rm -f $(TARGET) $(TARGET).exe

cleanupall distclean: cleanup
