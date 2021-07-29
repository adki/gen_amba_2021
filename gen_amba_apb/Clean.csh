#!/bin/csh -f

if ( -e gen_amba_apb.exe ) \rm -f   gen_amba_apb.exe endif
if ( -e gen_amba_apb     ) \rm -f   gen_amba_apb     endif
if ( -d obj              ) \rm -rf  obj              endif
