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
#-------------------------------------------------------------------------------
ifeq ($(MAKECMDGOALS),$(findstring $(MAKECMDGOALS), "" "all" "vlib" "compile" "simulate"))
    export VLIB = $(shell which vlib)
    export VLOG = $(shell which vlog)
    export VSIM = $(shell which vsim)
    export STR  = $(shell $(VSIM) -version)
    export VVER = $(shell for S in $(STR); do\
                  if [ "$${NN}" = "vsim" ]; then\
                       echo $$S;\
                  fi;\
                  NN=$$S;\
                  done)
    export MAJOR = $(firstword $(subst ., , $(VVER)))
    ifeq ($(shell test $(MAJOR) -ge 10; echo $$?),0)
        VOPT=-novopt -l transcript
    else
        VOPT=-l transcript
    endif
endif

#--------------------------------------------------------
WORK    = work
PLIOBJS =
#--------------------------------------------------------
TOP     = top
#--------------------------------------------------------
MST?=2
SLV?=2
SIZE?=1024
GEN_BUS=../../../gen_amba_axi
GEN_TOP=../../gen_axi_top.sh
BUS_V=../../design/amba_axi_m$(MST)s$(SLV).v
TOP_V=../../design/top.v
#--------------------------------------------------------
all: make_bus make_top vlib compile simulate

make_bus:
	@if [ ! -x $(GEN_BUS) ]; then make -C ../../..; fi
	@if [ ! -x $(GEN_BUS) ]; then echo "Something wrong"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	$(GEN_BUS) --master=$(MST) --slave=$(SLV) --output=$(BUS_V)

make_top:
	@if [ ! -x $(GEN_TOP) ]; then echo "Cannot find $(GEN_TOP)"; exit -1; fi
	@if [ ! -d ../../design ]; then mkdir ../../design; fi
	$(GEN_TOP) -mst $(MST) -slv $(SLV) -siz $(SIZE) -out $(TOP_V)
	@if [ ! -r $(TOP_V) ]; then echo "Couldn't generate top.v"; fi

vlib:
	@if [ -d $(WORK) ]; then /bin/rm -rf $(WORK); fi
	($(VLIB) $(WORK) || exit -1) 2>&1 | tee compile.log

compile:
	($(VLOG) -lint -work $(WORK)\
		+incdir+../../design+../../ip\
	        ./sim_define.v\
	        ../../ip/axi_tester.v\
	        ../../ip/mem_axi.v\
	        $(BUS_V)\
	        $(TOP_V)\
		|| exit -1) 2>&1 | tee compile.log

simulate: compile
	$(VSIM) $(VOPT) -c -do "run -all; quit"\
		+SINGLE_TEST=1 +BURST_TEST=1\
                +SINGLE_TEST_MEM=0 +BURST_TEST_MEM=1\
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
	-@/bin/rm -f $(TOP_V)

cleanupall: cleanup
	-@/bin/rm -f ../../design/amba_ahb_m*s*.v

#--------------------------------------------------------
