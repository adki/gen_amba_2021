@ECHO OFF

cmd.exe /c ../../../gen_amba_aix --master=3 --slave=3 --output=amba_axi_m3s3.v
cmd.exe /c xvlog -i ../../design -i ../../ip\
        	./sim_define.v\
        	../../ip/axi_tester.v\
        	../../ip/mem_axi.v\
        	../../design/amba_axi_m3s3.v\
        	../../design/top.v
cmd.exe /c xelab -prj xsim.prj -debug typical top -s top

IF "%~1"=="" GOTO :CMD
IF /I "%1"=="GUI" GOTO :GUI

:CMD
    cmd.exe /c xsim top -t xsim_run.tcl
    GOTO :END

:GUI
    # select "Window-->Waveform"
    # run all
    xsim top -gui
    GOTO :END

:END
   PAUSE -1
