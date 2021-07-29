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
CC		= $(GCC)
DEFS		=
INCDIRS		= -Isrc
LIBS		=
ifeq ($(PLATFORM),cygwin)
DEFS		+= -DWIN32
LIBS		+=
else ifeq ($(PLATFORM),mingw)
DEFS		+= -DWIN32
LIBS		+=
endif
OPT		= -g
CCFLAGS		= $(DEFS) $(OPT)

#-------------------------------------------------------------
TARGET		= gen_amba_apb

SRC	=\
	main.c\
	arg_parser.c\
	gen_ahb2apb.c\
	gen_ahb2apb_bridge.c\
	gen_axi2apb.c\
	gen_axi2apb_bridge.c\
	gen_apb_amba.c\
	gen_apb_decoder.c\
	gen_apb_mux.c
		
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
	$(CC) -o $(TARGET) $^ $(LIBS)

#-------------------------------------------------------------
run: $(TARGET)
	./$(TARGET) --ahb --slave=3 --output=ahb_to_apb_s3.v
	./$(TARGET) --axi4 --slave=3 --output=axi_to_apb_s3.v
	./$(TARGET) --axi3 --slave=4 --output=axi_to_apb_s4.v

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
