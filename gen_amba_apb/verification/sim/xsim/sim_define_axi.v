`ifndef _SIM_DEFINE_V_
`define _SIM_DEFNE_V_
//-----------------------------------------------------------------------
`define SIM      // define this for simulation case if you are not sure
`define VCD       // define this for VCD waveform dump
`undef  DEBUG
`define RIGOR
`undef  LOW_POWER
//-----------------------------------------------------------------------
`define BUS_DELAY
`define MEM_DELAY  0
//-----------------------------------------------------------------------
`define AMBA_APB3
`define AMBA_APB4
//-----------------------------------------------------------------------
`define WIDTH_AD   32 // address width
`define WIDTH_DA   32 // data width
`define AMBA_AXI4
`undef  AMBA_AXI_CACHE
`undef  AMBA_AXI_PROT
`define BURST_TYPE_WRAPP_ENABLED
`ifdef AMBA_AXI4
`define AMBA_QOS
`else
`undef AMBA_QOS
`endif
//-----------------------------------------------------------------------
`endif
