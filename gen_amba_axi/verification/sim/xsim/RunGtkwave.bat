@ECHO OFF

IF EXIST wave.vcd (
    gtkwave wave.vcd
) ELSE (
    ECHO "Cannot find wave.vcd."
    PAUSE
)
