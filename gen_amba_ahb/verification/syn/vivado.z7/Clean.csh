#!/bin/csh -f

if ( -e xst             )  \rm -rf xst
if ( -e _xmsgs          )  \rm -rf _xmsgs
if ( -e compile.log     )  \rm -f compile.log
if ( -e ngc2edif.log    )  \rm -f ngc2edif.log
if ( -e compile.ngc     )  \rm -f compile.ngc
if ( -e xilinx.log      )  \rm -f xilinx.edif
if ( -e *.edif          )  \rm -f *.edif
if ( -e *.xncf          )  \rm -f *.xncf
if ( -e *.lso           )  \rm -f *.lso
#if ( -e *.ngc           )  \rm -f *.ngc
if ( -e *.ngr           )  \rm -f *.ngr
if ( -e *.nlf           )  \rm -f *.nlf
if ( -e *.log           )  \rm -f *.log
if ( -e *.blc           )  \rm -f *.blc
if ( -e *.xrpt          )  \rm -f *.xrpt
if ( -e ../*.ngc        )  \rm -f ../*.ngc
if ( -e ../*.edif       )  \rm -f ../*.edif
if ( -e xlnx_auto_0_xdb )  \rm -rf xlnx_auto_0_xdb
if ( -e xlnx_auto_0.ise )  \rm -f xlnx_auto_0.ise
if ( -e x_list.txt      )  \rm -f x_list.txt
if ( -e xx_list.txt     )  \rm -f xx_list.txt
if ( -e fifo32x1024.ngo )  \rm -f fifo32x1024.ngo
if ( -e fifo32x512.ngo  )  \rm -f fifo32x512.ngo
if ( -e hif.ngo         )  \rm -f hif.ngo
if ( -e usbhif8core.ngo )  \rm -f usbhif8core.ngo
