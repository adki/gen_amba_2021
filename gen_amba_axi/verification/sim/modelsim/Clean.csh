#!/bin/csh -f

if ( -d work                 )  \rm -rf work    endif
if ( -d lib                  )  \rm -rf lib     endif
if ( -e transcript           )  \rm -f transcript    endif
if ( -e compile.log          )  \rm -f compile.log    endif
if ( -e wave.vcd             )  \rm -f wave.vcd    endif
if ( -e vish_stacktrace.vstf )  \rm -f vish_stacktrace.vstf    endif
if ( -e ethernet_log.txt     )  \rm -f ethernet_log.txt    endif
if ( -e mm.v                 )  \rm -f mm.v    endif
if ( -e m.v                  )  \rm -f m.v    endif
if ( -e xx.v                 )  \rm -f xx.v    endif
if ( -e x.v                  )  \rm -f x.v    endif
if ( -e vsim.wlf             )  \rm -f vsim.wlf    endif
