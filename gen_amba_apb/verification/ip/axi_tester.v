//----------------------------------------------------------------
//  Copyright (c) 2018-2020 by Ando Ki.
//  All right reserved.
//  http://www.future-ds.com
//  All rights are reserved by Ando Ki.
//  Do not use in any means or/and methods without Ando Ki's permission.
//----------------------------------------------------------------
// axi_tester.v
//----------------------------------------------------------------
// VERSION: 2020.01.29.
//----------------------------------------------------------------
//  [MACROS]
//    AMBA_AXI4                  - AMBA AXI4
//----------------------------------------------------------------
`timescale 1ns/1ns

module axi_tester
    #(parameter P_MST_ID =0         // Master ID
              , P_NUM_MST=2
              , P_NUM_SLV=2
              , P_SIZE_IN_BYTES=1024
              , WIDTH_ID =4         // ID width in bits
              , WIDTH_AD =32        // address width
              , WIDTH_DA =32        // data width
              , WIDTH_DS =(WIDTH_DA/8) // data strobe width
              , WIDTH_DSB=clogb2(WIDTH_DS) // data strobe width
              , EN       =1
              , ADDR_LENGTH=clogb2(P_SIZE_IN_BYTES)
              )
(
       input  wire                 ARESETn
     , input  wire                 ACLK
     //-----------------------------------------------------------
     , output reg  [WIDTH_ID-1:0]  AWID
     , output reg  [WIDTH_AD-1:0]  AWADDR
     `ifdef AMBA_AXI4
     , output reg  [ 7:0]          AWLEN
     , output reg                  AWLOCK
     `else
     , output reg  [ 3:0]          AWLEN
     , output reg  [ 1:0]          AWLOCK
     `endif
     , output reg  [ 2:0]          AWSIZE
     , output reg  [ 1:0]          AWBURST
     `ifdef AMBA_AXI_CACHE
     , output reg  [ 3:0]          AWCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , output reg  [ 2:0]          AWPROT
     `endif
     , output reg                  AWVALID
     , input  wire                 AWREADY
     `ifdef AMBA_AXI4
     , output reg  [ 3:0]          AWQOS
     , output reg  [ 3:0]          AWREGION
     `endif
     //-----------------------------------------------------------
     `ifndef AMBA_AXI4
     , output reg  [WIDTH_ID-1:0]  WID
     `endif
     , output reg  [WIDTH_DA-1:0]  WDATA
     , output reg  [WIDTH_DS-1:0]  WSTRB
     , output reg                  WLAST
     , output reg                  WVALID
     , input  wire                 WREADY
     //-----------------------------------------------------------
     , input  wire [WIDTH_ID-1:0]  BID
     , input  wire [ 1:0]          BRESP
     , input  wire                 BVALID
     , output reg                  BREADY
     //-----------------------------------------------------------
     , output reg  [WIDTH_ID-1:0]  ARID
     , output reg  [WIDTH_AD-1:0]  ARADDR
     `ifdef AMBA_AXI4
     , output reg  [ 7:0]          ARLEN
     , output reg                  ARLOCK
     `else
     , output reg  [ 3:0]          ARLEN
     , output reg  [ 1:0]          ARLOCK
     `endif
     , output reg  [ 2:0]          ARSIZE
     , output reg  [ 1:0]          ARBURST
     `ifdef AMBA_AXI_CACHE
     , output reg  [ 3:0]          ARCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , output reg  [ 2:0]          ARPROT
     `endif
     , output reg                  ARVALID
     , input  wire                 ARREADY
     `ifdef AMBA_AXI4
     , output reg  [ 3:0]          ARQOS
     , output reg  [ 3:0]          ARREGION
     `endif
     //-----------------------------------------------------------
     , input  wire [WIDTH_ID-1:0]  RID
     , input  wire [WIDTH_DA-1:0]  RDATA
     , input  wire [ 1:0]          RRESP
     , input  wire                 RLAST
     , input  wire                 RVALID
     , output reg                  RREADY
     //-----------------------------------------------------------
     , input  wire                 CSYSREQ
     , output reg                  CSYSACK
     , output reg                  CACTIVE
);
     //-----------------------------------------------------------
     reg done=1'b0;
     //-----------------------------------------------------------
     reg [WIDTH_DA-1:0] dataR[0:1023];
     reg [WIDTH_DA-1:0] dataW[0:1023];
     `include "axi_master_tasks.v"
     `include "mem_test_axi_tasks.v"
     //-----------------------------------------------------------
     reg [15:0] bnum ; //initial bnum  = 0;
     reg [15:0] blen ; //initial blen  = 0;
     reg        delay; //initial delay = 0;
     reg [31:0] saddr, depth;
     integer    nm, ns;
     //-----------------------------------------------------------
     initial begin
           bnum  = 0;
           blen  = 0;
           delay = 1;
           CSYSACK     = 1'b0;
           CACTIVE     = 1'b1;
           AWID        = 0;
           AWADDR      = ~0;
           AWLEN       = 0;
           AWLOCK      = 0;
           AWSIZE      = 0;
           AWBURST     = 0;
     `ifdef AMBA_AXI_CACHE
           AWCACHE     = 0;
     `endif
     `ifdef AMBA_AXI_PROT
           AWPROT      = 0;
     `endif
           AWVALID     = 0;
           `ifdef AMBA_AXI4
           AWQOS       = 0;
           AWREGION    = 0;
           `endif
           `ifndef AMBA_AXI4
           WID         = 0;
           `endif
           WDATA       = ~0;
           WSTRB       = 0;
           WLAST       = 0;
           WVALID      = 0;
           BREADY      = 0;
           ARID        = 0;
           ARADDR      = ~0;
           ARLEN       = 0;
           ARLOCK      = 0;
           ARSIZE      = 0;
           ARBURST     = 0;
     `ifdef AMBA_AXI_CACHE
           ARCACHE     = 0;
     `endif
     `ifdef AMBA_AXI_PROT
           ARPROT      = 0;
     `endif
           ARVALID     = 0;
           `ifdef AMBA_AXI4
           ARQOS       = 0;
           ARREGION    = 0;
           `endif
           RREADY      = 0; 
           wait (ARESETn==1'b0);
           wait (ARESETn==1'b1);
           repeat (5) @ (posedge ACLK);
           //-----------------------------------------------------
if (EN==1) begin
if (0) begin
           for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
               test_single('h100*P_MST_ID+'h10  , bnum, delay);
               test_single('h200*P_MST_ID+'h1000, bnum, delay);
           end
end
if (0) begin
           saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
           for (blen=1; blen<=16; blen=blen+1) begin
                test_burst (saddr, 4, blen, 2'b01, delay);
           end
end
if (0) begin
           //test_error('h1000_0000, delay);
end
if (0) begin
           for (ns=0; ns<P_NUM_SLV; ns=ns+1) begin
                depth = WIDTH_DS*2;
                saddr = (ns<<ADDR_LENGTH)+depth*P_MST_ID;
                mem_test(saddr, saddr + depth-1, 1, delay);
           end
end
if (1) begin
           for (ns=0; ns<P_NUM_SLV; ns=ns+1) begin
                depth = WIDTH_DS*2;
                saddr = (ns<<ADDR_LENGTH)+depth*P_MST_ID;
                for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                    mem_test(saddr, saddr + depth-1, bnum, delay);
                end
           end
end
if (1) begin
           for (ns=0; ns<P_NUM_SLV; ns=ns+1) begin
                depth = WIDTH_DS*16*2;
                saddr = (ns<<ADDR_LENGTH)+depth*P_MST_ID;
                //for (bnum=4; bnum<=WIDTH_DS; bnum=bnum*2) begin
                for (blen=1; blen<=16; blen=blen+1) begin
                    mem_test_burst(saddr, saddr + depth - 1, bnum, blen, delay);
                end
                //end
           end
end
end
           //-----------------------------------------------------
           repeat (5) @ (posedge ACLK);
           CACTIVE = 1'b0;
           //-----------------------------------------------------
           done = 1'b1;
     end
     //-----------------------------------------------------------
     function integer clogb2;
     input [31:0] value;
     begin
        value = value - 1;
        for (clogb2 = 0; value > 0; clogb2 = clogb2 + 1)
           value = value >> 1;
        end
     endfunction
     //-----------------------------------------------------------
endmodule
//----------------------------------------------------------------
// Revision History
//
// 2020.01.29: 'depth' bug-fixed.
// 2018.12.30: Started by Ando Ki (adki@future-ds.com)
//----------------------------------------------------------------
