#------------------------------------------------------------------
SHELL		= /bin/sh
MAKEFILE	= Makefile
#--------------------------------------------------------
VLOG	= iverilog
VSIM	= vvp
#--------------------------------------------------------
TOP	= top
#--------------------------------------------------------
MST    ?= 2
SLV    ?= 2
SIZE   ?= 1024
GEN_BUS = ../../../gen_amba_ahb
GEN_TOP = ../../gen_ahb_top.sh
BUS_V   = ../../design/amba_ahb_m$(MST)s$(SLV).v
TOP_V   = ../../design/top.v
#--------------------------------------------------------
all: make_bus make_top compile simulate

make_bus:
	@if [ ! -x $(GEN_BUS) ]; then make -C ../../..; fi
	@if [ ! -x $(GEN_BUS) ]; then echo "Something wrong"; exit -1; fi
	$(GEN_BUS) --master=$(MST) --slave=$(SLV) --output=$(BUS_V)

make_top:
	@if [ ! -x $(GEN_TOP) ]; then echo "Cannot find $(GEN_TOP)"; exit -1; fi
	$(GEN_TOP) -mst $(MST) -slv $(SLV) -siz $(SIZE) -out ../../design/top.v
	@if [ ! -r $(TOP_V) ]; then echo "Couldn't generate top.v"; fi


compile:
	($(VLOG) -g2012 -o $(TOP).vvp -s $(TOP)\
                -I../../design -I../../ip\
                ./sim_define.v\
                ../../ip/ahb_tester.v\
                ../../ip/mem_ahb.v\
                $(BUS_V)\
                $(TOP_V)\
		|| exit -1) 2>&1 | tee compile.log

simulate: compile
	$(VSIM) -l vvp.log $(TOP).vvp

#--------------------------------------------------------
clean:
	/bin/rm -f  $(TOP).vvp
	/bin/rm -f  compile.log
	/bin/rm -f  wave.vcd
	/bin/rm -f  vvp.log
	/bin/rm -f  result.bmp

cleanup clobber: clean

cleanupall distclean: cleanup
#--------------------------------------------------------
