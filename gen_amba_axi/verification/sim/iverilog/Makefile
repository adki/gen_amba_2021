#------------------------------------------------------------------
SHELL		= /bin/sh
MAKEFILE	= Makefile
#--------------------------------------------------------
IVERILOG_HOME = /usr/local
ILOG	= $(IVERILOG_HOME)/bin/iverilog
ISIM	= $(IVERILOG_HOME)/bin/vvp
#--------------------------------------------------------
TOP	= top
#--------------------------------------------------------
MST    ?= 3
SLV    ?= 2
SIZE   ?= "(4*1024*1024)"
GEN_BUS = ../../../gen_amba_axi
GEN_TOP = ../../gen_axi_top.sh
BUS_V   = ../../design/amba_axi_m$(MST)s$(SLV).v
#--------------------------------------------------------
all: make_bus make_top compile simulate

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
	@if [   -f ../../design/$(TOP).v ]; then /bin/rm -f ../../design/$(TOP).v; fi
	$(GEN_TOP) -mst $(MST) -slv $(SLV) -siz $(SIZE) -out ../../design/top.v
	@if [ ! -r ../../design/$(TOP).v ]; then echo "Couldn't generate ../../design/$(TOP).v"; fi

compile:
	($(ILOG) -g2012 -o $(TOP).vvp -s $(TOP)\
                -I../../design -I../../ip\
                ./sim_define.v\
                ../../ip/axi_tester.v\
                ../../ip/mem_axi.v\
                $(BUS_V)\
                ../../design/$(TOP).v\
		|| exit -1) 2>&1 | tee compile.log

sim simulate: compile
	$(ISIM) -l vvp.log $(TOP).vvp\
		+SINGLE_TEST=1\
		+BURST_TEST=1\
		+BURST_RANDOM_TEST=1\
		+BURST_MISALIGNED_TEST=1\
		+SINGLE_TEST_MEM=1

#+SINGLE_TEST=1\
#+BURST_TEST=1\
#+BURST_RANDOM_TEST=1\
#+BURST_MISALIGNED_TEST=1\
#+SINGLE_TEST_MEM=1\
#+BURST_TEST_MEM=1\
##+MULTIPLE_OUTSTANDING=1;\

#--------------------------------------------------------
clean:
	/bin/rm -f  $(TOP).vvp
	/bin/rm -f  compile.log
	/bin/rm -f  wave.vcd
	/bin/rm -f  vvp.log
	/bin/rm -f  result.bmp

cleanup clobber: clean
	/bin/rm -f ../../design/$(TOP).v

cleanupall distclean: cleanup
	/bin/rm -f $(BUS_V)
#--------------------------------------------------------
