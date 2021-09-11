#!/usr/bin/env bash

/bin/rm -fr obj
/bin/rm -fr ./Debug
/bin/rm -f *.stackdump
/bin/rm -f compile.log
/bin/rm -f *.v
/bin/rm -f gen_amba_axi
/bin/rm -f gen_amba_axi.exe

for F in *; do
    if [[ -d "${F}" && ! -L "${F}" ]]; then
    if [ -f ${F}/Clean.sh ]; then
       ( cd ${F}; ./Clean.sh )
    fi
    fi
done
