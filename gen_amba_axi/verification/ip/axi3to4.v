//------------------------------------------------------------------------------
// Copyright (c) 2021 by Ando Ki.
// All right reserved.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//------------------------------------------------------------------------------
// axi3to4.v
//------------------------------------------------------------------------------
// It converts AMBA AXI3 to AXI4.
//------------------------------------------------------------------------------
// VERSION: 2021.08.04.
//------------------------------------------------------------------------------
//  [MACROS]
//------------------------------------------------------------------------------
//  [PARAMETER]
//------------------------------------------------------------------------------

module axi3to4
     #(parameter AXI_WIDTH_ID = 4 // ID width in bits
               , AXI_WIDTH_AD =32 // address width
               , AXI_WIDTH_DA =32 // data width
               , AXI_WIDTH_DS =(AXI_WIDTH_DA/8)  // data strobe width
               , AXI_WIDTH_DSB=$clog2(AXI_WIDTH_DS) // data strobe width
               , AXI_WIDTH_SID=AXI_WIDTH_ID)
(
       input  wire                     ARESETn
     , input  wire                     ACLK
     , input  wire [AXI_WIDTH_SID-1:0] S_AWID
     , input  wire [AXI_WIDTH_AD-1:0]  S_AWADDR
     , input  wire [ 3:0]              S_AWLEN
     , input  wire [ 1:0]              S_AWLOCK
     , input  wire [ 2:0]              S_AWSIZE
     , input  wire [ 1:0]              S_AWBURST
     `ifdef AMBA_AXI_CACHE
     , input  wire [ 3:0]              S_AWCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , input  wire [ 2:0]              S_AWPROT
     `endif
     , input  wire                     S_AWVALID
     , output wire                     S_AWREADY
     , input  wire [AXI_WIDTH_SID-1:0] S_WID
     , input  wire [AXI_WIDTH_DA-1:0]  S_WDATA
     , input  wire [AXI_WIDTH_DS-1:0]  S_WSTRB
     , input  wire                     S_WLAST
     , input  wire                     S_WVALID
     , output wire                     S_WREADY
     , output wire [AXI_WIDTH_SID-1:0] S_BID
     , output wire [ 1:0]              S_BRESP
     , output wire                     S_BVALID
     , input  wire                     S_BREADY
     , input  wire [AXI_WIDTH_SID-1:0] S_ARID
     , input  wire [AXI_WIDTH_AD-1:0]  S_ARADDR
     , input  wire [ 3:0]              S_ARLEN
     , input  wire [ 1:0]              S_ARLOCK
     , input  wire [ 2:0]              S_ARSIZE
     , input  wire [ 1:0]              S_ARBURST
     `ifdef AMBA_AXI_CACHE
     , input  wire [ 3:0]              S_ARCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , input  wire [ 2:0]              S_ARPROT
     `endif
     , input  wire                     S_ARVALID
     , output wire                     S_ARREADY
     , output wire [AXI_WIDTH_SID-1:0] S_RID
     , output wire [AXI_WIDTH_DA-1:0]  S_RDATA
     , output wire [ 1:0]              S_RRESP
     , output wire                     S_RLAST
     , output wire                     S_RVALID
     , input  wire                     S_RREADY
     //-------------------------------------------------------------------------
     , output wire [AXI_WIDTH_ID-1:0]  M_AWID
     , output wire [AXI_WIDTH_AD-1:0]  M_AWADDR
     , output wire [ 7:0]              M_AWLEN
     , output wire                     M_AWLOCK
     , output wire [ 2:0]              M_AWSIZE
     , output wire [ 1:0]              M_AWBURST
     `ifdef AMBA_AXI_CACHE
     , output wire [ 3:0]              M_AWCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , output wire [ 2:0]              M_AWPROT
     `endif
     , output wire                     M_AWVALID
     , input  wire                     M_AWREADY
     , output wire [AXI_WIDTH_DA-1:0]  M_WDATA
     , output wire [AXI_WIDTH_DS-1:0]  M_WSTRB
     , output wire                     M_WLAST
     , output wire                     M_WVALID
     , input  wire                     M_WREADY
     , input  wire [AXI_WIDTH_ID-1:0]  M_BID
     , input  wire [ 1:0]              M_BRESP
     , input  wire                     M_BVALID
     , output wire                     M_BREADY
     , output wire [AXI_WIDTH_ID-1:0]  M_ARID
     , output wire [AXI_WIDTH_AD-1:0]  M_ARADDR
     , output wire [ 7:0]              M_ARLEN
     , output wire                     M_ARLOCK
     , output wire [ 2:0]              M_ARSIZE
     , output wire [ 1:0]              M_ARBURST
     `ifdef AMBA_AXI_CACHE
     , output wire [ 3:0]              M_ARCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , output wire [ 2:0]              M_ARPROT
     `endif
     , output wire                     M_ARVALID
     , input  wire                     M_ARREADY
     , input  wire [AXI_WIDTH_ID-1:0]  M_RID
     , input  wire [AXI_WIDTH_DA-1:0]  M_RDATA
     , input  wire [ 1:0]              M_RRESP
     , input  wire                     M_RLAST
     , input  wire                     M_RVALID
     , output wire                     M_RREADY
);
    //--------------------------------------------------------------------------
    assign S_AWREADY = M_AWREADY;
    assign S_WREADY  = M_WREADY ;
    assign S_BID     = M_BID    ;
    assign S_BRESP   = M_BRESP  ;
    assign S_BVALID  = M_BVALID ;
    assign S_ARREADY = M_ARREADY;
    assign S_RID     = M_RID    ;
    assign S_RDATA   = M_RDATA  ;
    assign S_RRESP   = M_RRESP  ;
    assign S_RLAST   = M_RLAST  ;
    assign S_RVALID  = M_RVALID ;
    //--------------------------------------------------------------------------
    assign M_AWID     = S_AWID    ;
    assign M_AWADDR   = S_AWADDR  ;
    assign M_AWLEN    ={4'h0,S_AWLEN};
    assign M_AWLOCK   = S_AWLOCK  [0];
    assign M_AWSIZE   = S_AWSIZE  ;
    assign M_AWBURST  = S_AWBURST ;
    `ifdef AMBA_AXI_CACHE
    assign M_AWCACHE  = S_AWCACHE ;
    `endif
    `ifdef AMBA_AXI_PROT
    assign M_AWPROT   = S_AWPROT  ;
    `endif
    assign M_AWVALID  = S_AWVALID ;
    assign M_WDATA    = S_WDATA   ;
    assign M_WSTRB    = S_WSTRB   ;
    assign M_WLAST    = S_WLAST   ;
    assign M_WVALID   = S_WVALID  ;
    assign M_BREADY   = S_BREADY  ;
    assign M_ARID     = S_ARID    ;
    assign M_ARADDR   = S_ARADDR  ;
    assign M_ARLEN    ={4'h0,S_ARLEN};
    assign M_ARLOCK   = S_ARLOCK  [0];
    assign M_ARSIZE   = S_ARSIZE  ;
    assign M_ARBURST  = S_ARBURST ;
    `ifdef AMBA_AXI_CACHE
    assign M_ARCACHE  = S_ARCACHE ;
    `endif
    `ifdef AMBA_AXI_PROT
    assign M_ARPROT   = S_ARPROT  ;
    `endif
    assign M_ARVALID  = S_ARVALID ;
    assign M_RREADY   = S_RREADY  ;
    //--------------------------------------------------------------------------
endmodule

//------------------------------------------------------------------------------
// Legacy considerations
// In an AXI4 environment, any AXI3 locked transaction is converted as follows:
// • AWLOCK[1:0] = 0b10 is converted to a normal write transaction, AWLOCK = 0b0
// • ARLOCK[1:0] = 0b10 is converted to a normal read transaction, ARLOCK = 0b0.
// This specification recommends that any component performing such a conversion,
//  typically an interconnect, includes an optional mechanism to detect and flag
// that such a translation has occurred.
// Any component that cannot operate correctly if this translation is performed
// cannot be used in an AXI4 environment.
// Note
// For many legacy cases that use locked transactions, such as the execution of
// a SWP instruction, a software change might be required to prevent the use of
// any instruction that forces a locked transaction.
//------------------------------------------------------------------------------
// Revision History
//
// 2021.09.04: Started by Ando Ki (adki@future-ds.com)
//------------------------------------------------------------------------------
