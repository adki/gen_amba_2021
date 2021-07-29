#-------------------------------------------------------------------------------
# VIVADO TCL Script
#-------------------------------------------------------------------------------
# Copyright (c) 2018 by Future Design Systems
# All right reserved.
#-------------------------------------------------------------------------------
# VERSION: 2018.06.01.
#---------------------------------------------------------
if {[info exists env(VIVADO_VER)] == 0} { 
     set VIVADO_VER vivado.2019.2
} else { 
     set VIVADO_VER $::env(VIVADO_VER)
}
if {[info exists env(FPGA_TYPE)] == 0} { 
     set FPGA_TYPE  ZYNQ7000
     set DEVICE     xc7z020-clg484-1
     set BOARD_TYPE ZEDBOARD
} else { 
     set FPGA_TYPE $::env(FPGA_TYPE)
     if {${FPGA_TYPE}=="ZYNQ7000"} { 
          set DEVICE     xc7z020-clg484-1
          set BOARD_TYPE ZEDBOARD
     } else {
          puts "${FPGA_TYPE} not supported"
          exit 1
     }
}
if {[info exists env(MODULE)] == 0} { 
     set MODULE axi_to_apb_s2
} else { 
     set MODULE $::env(MODULE)
}
if {[info exists env(WORK)] == 0} { 
     set WORK work
} else { 
     set WORK $::env(WORK)
}
if {[info exists env(RIGOR)] == 0} { 
     set RIGOR 1
} else { 
     set RIGOR $::env(RIGOR)
}

#---------------------------------------------------------
set_part ${DEVICE}
set_property part ${DEVICE} [current_project]
file mkdir ${WORK}

set out_dir    ${WORK}
set part       ${DEVICE}
set module     ${MODULE}
set rigor      ${RIGOR}

#------------------------------------------------------------------------------
# Assemble the design source files
#proc proc_read { {out_dir ${WORK}} {part ${DEVICE}} {module ${MODULE}} { rigor 0 } } {
     set DIR_RTL        .

     #-------------------------------------------------------------------------
     set_property verilog_dir " ${DIR_RTL}
                              " [current_fileset]
     puts [get_property verilog_dir [current_fileset]]

     #-------------------------------------------------------------------------
     set VLG_LIST "syn_define.v
                   ${DIR_RTL}/${MODULE}.v
                  "
     puts ${VLG_LIST}
     read_verilog  ${VLG_LIST}

     #-------------------------------------------------------------------------

#     return 0
#}

#---------------------------------------------------------
# Run synthesis and implementation
#proc proc_synth { out_dir {part ${DEVICE}} {module ${MODULE}} { rigor 0 } } {
     #proc_read ${out_dir} ${part} ${module} ${rigor}
     synth_design -top ${module} -part ${part}\
                  -mode out_of_context\
                  -flatten_hierarchy rebuilt\
                  -keep_equivalent_registers\
                  -directive RunTimeOptimized\
                  -verilog_define SYN=1\
                  -verilog_define VIVADO=1\
                  -verilog_define ${FPGA_TYPE}=1
    #write_verilog -force -mode synth_stub ${module_stu}
    #puts "${module_stu} has been written"
    #write_verilog -force -mode funcsim ${module_net}
    #puts "${module_net} has been written"
    #write_edif -force ${module_edn}
    #puts "${module_edn} has been written"
     write_checkpoint -force ${out_dir}/post_synth
     write_checkpoint -force ${MODULE}
     if { ${rigor} == 1} {
        report_timing_summary -file ${out_dir}/post_synth_timing_summary.rpt
        report_timing -sort_by group -max_paths 5 -path_type summary -file ${out_dir}/post_synth_timing.rpt
        report_power -file ${out_dir}/post_synth_power.rpt
        report_utilization -file ${out_dir}/post_synth_util.rpt
     }
#     return 0
#}

#---------------------------------------------------------
if {[info exists env(GUI)] == 0} {
     exit
} else {
    if { $::env(GUI) == 0} exit
}

#-------------------------------------------------------------------------------
# Revision History:
#
# 2017.01.17: 'BOARD_TYPE' added by Ando Ki.
#-------------------------------------------------------------------------------
