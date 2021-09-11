#!/bin/csh -f

if ( -e gen_amba_axi.exe ) \rm -f   gen_amba_axi.exe endif
if ( -e gen_amba_axi     ) \rm -f   gen_amba_axi     endif
if ( -d obj              ) \rm -rf  obj              endif

foreach F ( * )
    if ( -d $F ) then
    if ( -e $F/Clean.csh ) then
       ( cd $F; ./Clean.csh )
    endif
    endif
end
