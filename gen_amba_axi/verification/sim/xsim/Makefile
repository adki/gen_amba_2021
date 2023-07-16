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
WORK	= work
#--------------------------------------------------------
TOP	= top
#--------------------------------------------------------
GUI    ?= 0
MST    ?= 3
SLV    ?= 2
SIZE   ?= "(4*1024*1024)"
GEN_BUS = ../../../gen_amba_axi
GEN_TOP = ../../gen_axi_top.sh
BUS_V   = ../../design/amba_axi_m$(MST)s$(SLV).v
TOP_V   = ../../design/top.v
#--------------------------------------------------------
all: run

run: make_bus make_top xlib compile elab sim

make_bus:
	@if [ ! -x $(GEN_BUS) ]; then make -C ../../..; fi
	@if [ ! -x $(GEN_BUS) ]; then echo "Something wrong"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	@if [   -f $(BUS_V) ]; then /bin/rm -f $(BUS_V); fi
	$(GEN_BUS) --master=$(MST) --slave=$(SLV) --output=$(BUS_V)
	@if [ ! -r $(BUS_V) ]; then echo "Couldn't generate $(BUS_V)"; fi

make_top:
	@if [ ! -x $(GEN_TOP) ]; then echo "Cannot find $(GEN_TOP)"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	@if [   -f $(TOP_V) ]; then /bin/rm -f $(TOP_V); fi
	$(GEN_TOP) -mst $(MST) -slv $(SLV) -siz $(SIZE) -out $(TOP_V)
	@if [ ! -r $(TOP_V) ]; then echo "Couldn't generate $(TOP_V)"; fi

xlib:
	@if [ -d $(WORK) ]; then /bin/rm -rf $(WORK); fi

compile:
	xvlog -sv -i ../../design -i ../../ip\
        	./sim_define.v\
        	../../ip/axi_tester.v\
        	../../ip/mem_axi.v\
        	$(BUS_V)\
        	$(TOP_V)

elab: compile
	xelab -debug typical top -s top

sim: elab
	if [ "$(GUI)" = "1" ]; then\
		xsim top -gui\
			-testplusarg SINGLE_TEST=1\
			-testplusarg BURST_TEST=1\
		  	-testplusarg BURST_RANDOM_TEST=2\
			-testplusarg BURST_MISALIGNED_TEST=1\
			-testplusarg SINGLE_TEST_MEM=1\
			-testplusarg BURST_TEST_MEM=1\
			-testplusarg MULTIPLE_OUTSTANDING=1;\
	else\
		xsim top -t xsim_run.tcl\
			-testplusarg SINGLE_TEST=1\
			-testplusarg BURST_TEST=1\
			-testplusarg BURST_RANDOM_TEST=2\
			-testplusarg BURST_MISALIGNED_TEST=1\
			-testplusarg SINGLE_TEST_MEM=1\
			-testplusarg BURST_TEST_MEM=1\
			;\
	fi
#-testplusarg SINGLE_TEST=1\
#-testplusarg BURST_TEST=1\
#-testplusarg BURST_RANDOM_TEST=2\
#-testplusarg BURST_MISALIGNED_TEST=1\
#-testplusarg SINGLE_TEST_MEM=1\
#-testplusarg BURST_TEST_MEM=1\
#-testplusarg MULTIPLE_OUTSTANDING=1;\

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
	/bin/rm -f $(TOP_V)

cleanupall: cleanup
	/bin/rm -f $(BUS_V)

#--------------------------------------------------------
