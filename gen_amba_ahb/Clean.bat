@ECHO OFF

IF EXIST gen_amba_ahb.exe DEL   /Q    gen_amba_ahb.exe
IF EXIST gen_amba_ahb     DEL   /Q    gen_amba_ahb
IF EXIST obj              RMDIR /S/Q  obj
