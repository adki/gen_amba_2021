`ifndef _SIM_DEFINE_V_
`define _SIM_DEFNE_V_
//-----------------------------------------------------------------------
`define SIM      // define this for simulation case if you are not sure
`define VCD       // define this for VCD waveform dump
`undef  DEBUG
`define RIGOR
`undef  LOW_POWER
//-----------------------------------------------------------------------
`define BUS_DELAY // use empty bus-dealy for icarus iverilog
`define MEM_DELAY  0
//-----------------------------------------------------------------------
`define SINGLE_TEST
`define BURST_TEST
//-----------------------------------------------------------------------
`endif
