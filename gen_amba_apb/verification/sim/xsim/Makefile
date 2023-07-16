#------------------------------------------------------------------
# Copyright (c) 2021 by Ando Ki.
# All right reserved.
#
# This code is distributed in the hope that it will
# be useful to understand Ando Ki's work,
# but WITHOUT ANY WARRANTY.
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
MST    ?= axi
SLV    ?= 2
SIZE   ?= 1024
GEN_BUS = ../../../gen_amba_apb
GEN_TOP = ../../gen_apb_top.sh
BUS_V   = ../../design/$(MST)_to_apb_s$(SLV).v
TOP_V   = ../../design/top.v
#--------------------------------------------------------
all: run

run: make_bus make_top xlib compile elab sim

make_bus:
	@if [ ! -x $(GEN_BUS) ]; then make -C ../../..; fi
	@if [ ! -x $(GEN_BUS) ]; then echo "Something wrong"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	$(GEN_BUS) --$(MST) --slave=$(SLV) --output=$(BUS_V)

make_top:
	@if [ ! -x $(GEN_TOP) ]; then echo "Cannot find $(GEN_TOP)"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	$(GEN_TOP) -$(MST) -slv $(SLV) -siz $(SIZE) -out $(TOP_V)
	@if [ ! -r $(TOP_V) ]; then echo "Couldn't generate top.v"; fi

xlib:
	@if [ -d $(WORK) ]; then /bin/rm -rf $(WORK); fi

compile:
	xvlog -sv -i ../../design -i ../../ip\
        	./sim_define_${MST}.v\
        	../../ip/${MST}_tester.v\
        	../../ip/mem_apb.v\
        	$(BUS_V)\
        	$(TOP_V)

elab:
	xelab -debug typical top -s top

sim: elab
	if [ "$(GUI)" = "1" ]; then\
		xsim top -gui;\
	else\
		xsim top -t xsim_run.tcl;\
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
	/bin/rm -f $(TOP_V)

cleanupall: cleanup
	/bin/rm -f ../../design/amba_ahb_m*s*.v

#--------------------------------------------------------
