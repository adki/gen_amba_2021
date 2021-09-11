`ifndef _SIM_DEFINE_V_
`define _SIM_DEFNE_V_
//-----------------------------------------------------------------------
`define SIM      // define this for simulation case if you are not sure
`define VCD       // define this for VCD waveform dump
`undef  DEBUG
`define RIGOR
`undef  LOW_POWER
//-----------------------------------------------------------------------
//`define BUS_DELAY // use empty bus-dealy for icarus iverilog
`define MEM_DELAY  0
//-----------------------------------------------------------------------
`define WIDTH_AD   64 // address width
`define WIDTH_DA   128 // data width
`define AMBA_AXI4
`undef  AMBA_AXI_CACHE
`undef  AMBA_AXI_PROT
`undef  AMBA_AXI_QOS
`define BURST_TYPE_WRAPP_ENABLED
//-----------------------------------------------------------------------
`define SINGLE_TEST
`undef  BURST_TEST
//-----------------------------------------------------------------------
`endif
