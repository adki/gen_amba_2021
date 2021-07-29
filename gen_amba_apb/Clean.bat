@ECHO OFF

IF EXIST gen_amba_apb.exe DEL   /Q    gen_amba_apb.exe
IF EXIST gen_amba_apb     DEL   /Q    gen_amba_apb
IF EXIST obj              RMDIR /S/Q  obj
