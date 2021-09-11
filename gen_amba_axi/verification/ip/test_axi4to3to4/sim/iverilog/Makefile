#------------------------------------------------------------------
SHELL		= /bin/sh
MAKEFILE	= Makefile
#--------------------------------------------------------
ILOG	= iverilog
ISIM	= vvp
#--------------------------------------------------------
TOP	= top
#--------------------------------------------------------
OPTIONS ?= +SINGLE_TEST=1\
           +BURST_TEST=1\
           +BURST_RANDOM_TEST=1\
           +BURST_MISALIGNED_TEST=1\
           +SINGLE_TEST_MEM=1\
           +BURST_TEST_MEM=1
#--------------------------------------------------------
all: compile simulate

compile:
	($(ILOG) -o $(TOP).vvp -s $(TOP)\
                -I../../design\
                ./sim_define.v\
                ../../design/top.v\
                ../../../axi4to3.v\
                ../../../axi3to4.v\
                ../../design/axi4_mem_beh.v\
                ../../design/axi4_tester.v\
		|| exit -1) 2>&1 | tee compile.log

sim simulate: compile
	$(ISIM) -l vvp.log $(TOP).vvp $(OPTIONS)

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
