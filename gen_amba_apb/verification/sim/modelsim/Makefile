#------------------------------------------------------------------
# Copyright (c) 2018 by Ando Ki.
# All right reserved.
#
# This code is distributed in the hope that it will
# be useful to understand Ando Ki's work,
# but WITHOUT ANY WARRANTY.
#------------------------------------------------------------------
SHELL		= /bin/sh
MAKEFILE	= Makefile
#------------------------------------------------------------------
unexport PLIOBJS
#--------------------------------------------------------
ifeq ($(MAKECMDGOALS),$(findstring $(MAKECMDGOALS), "" "all" "vlib" "compile" "simulate"))
VLIB	= $(shell which vlib)
VLOG	= $(shell which vlog)
VSIM	= $(shell which vsim)
STR	= $(shell $(VSIM) -version)
#--------------------------------------------------------
VVER	= $(shell for S in $(STR); do\
		if [ "$${NN}" = "vsim" ]; then\
			echo $$S;\
		fi;\
		NN=$$S;\
	done)
#--------------------------------------------------------
MAJOR	= $(firstword $(subst ., , $(VVER)))
#--------------------------------------------------------
ifeq ($(shell test $(MAJOR) -ge 10; echo $$?),0)
	VOPT=-novopt -l transcript
else
	VOPT=-l transcript
endif
endif
#--------------------------------------------------------
WORK	 = work
PLIOBJS	 =
#--------------------------------------------------------
TOP	 = top
#--------------------------------------------------------
MST     ?=axi
SLV     ?=2
SIZE    ?=1024
BIT_AXI_ADDR ?= 32
BIT_AXI_DATA ?= 256
GEN_BUS  =../../../gen_amba_apb
GEN_TOP  =../../gen_apb_top.sh
BUS_V    =../../design/$(MST)_to_apb_s$(SLV).v
TOP_V    =../../design/top.v
#--------------------------------------------------------
all: make_bus make_top vlib compile simulate

make_bus:
	@if [ ! -x $(GEN_BUS) ]; then make -C ../../..; fi
	@if [ ! -x $(GEN_BUS) ]; then echo "Something wrong"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	$(GEN_BUS) --$(MST) --slv=$(SLV) --out=$(BUS_V)

make_top:
	@if [ ! -x $(GEN_TOP) ]; then echo "Cannot find $(GEN_TOP)"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	@if [ "$(MST)" = "axi" ]; then\
		$(GEN_TOP) -$(MST) -axiad $(BIT_AXI_ADDR) -axida $(BIT_AXI_DATA) -slv $(SLV) -siz $(SIZE) -out $(TOP_V);\
	else\
		$(GEN_TOP) -$(MST) -slv $(SLV) -siz $(SIZE) -out $(TOP_V);\
	fi
	@if [ ! -r $(TOP_V) ]; then echo "Couldn't generate top.v"; fi

vlib:
	@if [ -d $(WORK) ]; then /bin/rm -rf $(WORK); fi
	($(VLIB) $(WORK) || exit -1) 2>&1 | tee compile.log

compile:
	($(VLOG) -lint -work $(WORK)\
		+incdir+../../design+../../ip\
	        ./sim_define_$(MST).v\
	        ../../ip/$(MST)_tester.v\
	        ../../ip/mem_apb.v\
	        $(BUS_V)\
	        $(TOP_V)\
		|| exit -1) 2>&1 | tee compile.log

simulate: compile
	$(VSIM) $(VOPT) -c -do "run -all; quit"\
		$(WORK).$(TOP)

#--------------------------------------------------------
clean:
	-@/bin/rm -rf $(WORK)
	-@/bin/rm -f  transcript
	-@/bin/rm -f  wave.vcd dump.vcd
	-@/bin/rm -f  vsim.wlf
	-@/bin/rm -f  vish_stacktrace.vstf
	-@/bin/rm -f  wlft*
	-@/bin/rm -f  compile.log

cleanup clobber: clean
	-@/bin/rm -f $(BUS_V) $(TOP_V)

cleanupall: cleanup
	-@/bin/rm -f ../../design/amba_ahb_m*s*.v

#--------------------------------------------------------
