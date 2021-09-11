#------------------------------------------------------------------
# Copyright (c) 2021 by Ando Ki.
# All right reserved.
# This is licensed with the 2-clause BSD license to make the program and
# library useful in open and closed source products independent of their
# licensing scheme.
#------------------------------------------------------------------
SHELL		= /bin/sh
MAKEFILE	= Makefile
#------------------------------------------------------------------
ifeq ($(MAKECMDGOALS),$(findstring $(MAKECMDGOALS), "" "all" "elab" "sim"))
    ifndef XILINX_VIVADO
       $(error XILINX_VIVADO environment variable not defined)
    endif
endif
#--------------------------------------------------------
WORK = work
#--------------------------------------------------------
TOP = top
#--------------------------------------------------------
GUI     ?= 0
OPTIONS ?= -testplusarg SINGLE_TEST=0\
           -testplusarg BURST_TEST=1\
           -testplusarg BURST_RANDOM_TEST=1\
           -testplusarg BURST_MISALIGNED_TEST=1\
           -testplusarg SINGLE_TEST_MEM=1\
           -testplusarg BURST_TEST_MEM=1
#--------------------------------------------------------
all: run

run: xlib compile elab sim

xlib:
	@if [ -d $(WORK) ]; then /bin/rm -rf $(WORK); fi

compile:
	xvlog -i ../../design\
        	./sim_define.v\
        	../../design/top.v\
        	../../../axi4to3.v\
        	../../../axi3to4.v\
        	../../design/axi4_mem_beh.v\
        	../../design/axi4_tester.v

elab: compile
	xelab -debug typical top -s top

sim: elab
	if [ "$(GUI)" = "1" ]; then\
		xsim top -gui $(OPTIONS);\
	else\
		xsim top -t xsim_run.tcl $(OPTIONS);\
	fi

#--------------------------------------------------------
clean:
	/bin/rm -rf $(WORK)
	/bin/rm -f  compile.log
	/bin/rm -f  top.wdb
	/bin/rm -f  wave.vcd
	/bin/rm -f  webtalk_*.backup.jou
	/bin/rm -f  webtalk_*.backup.log
	/bin/rm -f  webtalk.jou
	/bin/rm -f  webtalk.log
	/bin/rm -f  xelab.log
	/bin/rm -f  xelab.pb
	/bin/rm -fr .Xil
	/bin/rm -f  xsim_*.backup.jou
	/bin/rm -f  xsim_*.backup.log
	/bin/rm -fr xsim.dir
	/bin/rm -f  xsim.jou
	/bin/rm -f  xsim.log
	/bin/rm -f  xvlog.log
	/bin/rm -f  xvlog.pb

cleanup clobber: clean

cleanupall: cleanup

#--------------------------------------------------------
