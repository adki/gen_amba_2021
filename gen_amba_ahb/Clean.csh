#!/bin/csh -f

if ( -e gen_amba_ahb.exe ) \rm -f   gen_amba_ahb.exe endif
if ( -e gen_amba_ahb     ) \rm -f   gen_amba_ahb     endif
if ( -d obj              ) \rm -rf  obj              endif
