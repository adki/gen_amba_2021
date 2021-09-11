@ECHO OFF

IF EXIST gen_amba_axi.exe DEL   /Q    gen_amba_axi.exe
IF EXIST gen_amba_axi     DEL   /Q    gen_amba_axi
IF EXIST obj              RMDIR /S/Q  obj

@FOR /D %%I in (*) DO @(
	@IF EXIST %%I\Clean.bat (
		@PUSHD %%I
		@CALL .\Clean.bat
		@POPD
	)
)
