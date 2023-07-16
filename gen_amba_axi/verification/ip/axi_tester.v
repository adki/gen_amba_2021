//----------------------------------------------------------------
// Copyright (c) 2018-2021 by Ando Ki.
// All right reserved.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//----------------------------------------------------------------
// axi_tester.v
//----------------------------------------------------------------
// VERSION: 2021.07.10.
//----------------------------------------------------------------
//  [MACROS]
//    AMBA_AXI4                  - AMBA AXI4
//----------------------------------------------------------------
`timescale 1ns/1ns

module axi_tester
    #(parameter P_MST_ID =0         // Master ID
              , P_NUM_MST=2
              , P_NUM_SLV=2
              , WIDTH_CID=4
              , WIDTH_ID =4         // ID width in bits
              , WIDTH_AD =32        // address width
              , WIDTH_DA =32        // data width
              , WIDTH_DS =(WIDTH_DA/8) // data strobe width
              , WIDTH_DSB=clogb2(WIDTH_DS) // data strobe width
              , EN       =1
              , P_SIZE_IN_BYTES=1024
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
     `ifdef AMBA_QOS
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
     `ifdef AMBA_QOS
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
     //-----------------------------------------------------------
     , output reg                  busy_out
     , input  wire                 busy_in
);
     //-----------------------------------------------------------
     localparam SIZE_IN_BYTES=(P_SIZE_IN_BYTES/WIDTH_DS)*WIDTH_DS;
     localparam ADDR_LENGTH=clogb2(SIZE_IN_BYTES);
     //-----------------------------------------------------------
     wire [WIDTH_CID-1:0] MID = P_MST_ID;
     //-----------------------------------------------------------
     reg done=1'b0;
     //-----------------------------------------------------------
     reg [WIDTH_DA-1:0] dataR[0:1023]; // non-justified data
     reg [WIDTH_DA-1:0] dataW[0:1023]; // non-justified data
     `include "axi_master_tasks.v"
     `include "mem_test_tasks.v"
     //-----------------------------------------------------------
     integer arg; // for commandline test plus arguments
     reg [15:0]         bnum ; initial bnum  = 0;
     reg [15:0]         blen ; initial blen  = 0;
     reg                delay; initial delay = 0;
     reg [WIDTH_AD-1:0] saddr, depth;
     integer    stage;
     integer    idx;
     //-----------------------------------------------------------
     initial begin
           busy_out    = 1'b0;
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
           `ifdef AMBA_QOS
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
           `ifdef AMBA_QOS
           ARQOS       = 0;
           ARREGION    = 0;
           `endif
           RREADY      = 0; 
           stage = 0;
           wait (ARESETn==1'b0);
           wait (ARESETn==1'b1);
           repeat (5) @ (posedge ACLK);
           //-----------------------------------------------------
if (EN==1) begin
if ($value$plusargs("SINGLE_TEST=%d", arg) && arg) begin
           $display("SINGLE_TEST================");
           stage = 1;
           busy_out = 1'b1;
           saddr = SIZE_IN_BYTES*P_MST_ID;
           for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
               test_single(saddr, bnum, delay);
           end
           busy_out = 1'b0;
           @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if ($value$plusargs("BURST_TEST=%d", arg) && arg) begin
           $display("BURST_TEST================");
           stage = 2;
           busy_out = 1'b1;
           saddr = SIZE_IN_BYTES*P_MST_ID; //saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
           `ifdef AMBA_AXI4
           for (blen=1; blen<=256; blen=blen*2) begin
           `else
           for (blen=1; blen<=16; blen=blen+1) begin
           `endif
              for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                test_burst (saddr, bnum, blen, 2'b01, delay, 0);
               end
           end
           busy_out = 1'b0;
           @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if ($value$plusargs("BURST_RANDOM_TEST=%d", arg) && arg) begin
           $display("BURST_RANDOM_TEST================");
           stage = 2;
           busy_out = 1'b1;
           saddr = SIZE_IN_BYTES*P_MST_ID; //saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
           `ifdef AMBA_AXI4
           for (blen=1; blen<=256; blen=blen*2) begin
           `else
           for (blen=1; blen<=16; blen=blen+1) begin
           `endif
               for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                test_burst (saddr, bnum, blen, 2'b01, delay, arg+1);
               end
           end
           busy_out = 1'b0;
           @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if ($value$plusargs("BURST_MISALIGNED_TEST=%d", arg) && arg) begin
           $display("BURST_MISALIGNED_TEST================");
           stage = 3;
           busy_out = 1'b1;
           saddr = SIZE_IN_BYTES*P_MST_ID; //saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
           for (idx=0; idx<4; idx=idx+1) begin
               for (blen=1; blen<=16; blen=blen+1) begin
                    test_burst (saddr+idx, 4, blen, 2'b01, delay, 0);
               end
           end
           for (idx=0; idx<4; idx=idx+1) begin
               for (blen=1; blen<=16; blen=blen+1) begin
                    test_burst (saddr+idx, 4, blen, 2'b01, delay, 1);
               end
           end
           busy_out = 1'b0;
           @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if (0) begin
           //test_error('h1000_0000, delay);
end
if ($value$plusargs("SINGLE_TEST_MEM=%d", arg) && arg) begin
           $display("SINGLE_TEST_MEM %d ================", arg);
           stage = 4;
           busy_out = 1'b1;
           depth = 'h10;
           saddr = SIZE_IN_BYTES*P_MST_ID;
           mem_test(saddr, saddr + depth-1, 1, delay);
           busy_out = 1'b0;
           @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if ($value$plusargs("SINGLE_TEST_MEM=%d", arg) && arg) begin
           $display("SINGLE_TEST_MEM %d ================", arg);
           stage = 5;
           busy_out = 1'b1;
           depth = 'h10;
           saddr = SIZE_IN_BYTES*P_MST_ID;
           for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                mem_test(saddr, saddr + depth-1, bnum, delay);
           end
           busy_out = 1'b0;
           @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if ($value$plusargs("BURST_TEST_MEM=%d", arg) && arg) begin
           $display("BURST_TEST_MEM %d ================", arg);
           stage = 6;
           busy_out = 1'b1;
           depth = 4*16*4;
           saddr = SIZE_IN_BYTES*P_MST_ID;
           for (bnum=4; bnum<=WIDTH_DS; bnum=bnum*2) begin
               for (blen=1; blen<=16; blen=blen+1) begin
                   mem_test_burst(saddr, saddr + depth - 1, bnum, blen, delay);
               end
           end
           busy_out = 1'b0;
           @ (posedge ACLK);
           repeat (5) @ (posedge ACLK);
           while (busy_in==1'b1) @ (posedge ACLK);
end
if ($value$plusargs("MULTIPLE_OUTSTANDING=%d", arg) && arg) begin
     $display("MULTIPLE_OUTSTANDING %d ================", arg);
     busy_out = 1'b1;
     saddr = SIZE_IN_BYTES*P_MST_ID;
     axi_master_write_multiple_outstanding(
                 4 //awnum  // num of multiple outstanding
               , saddr
               , WIDTH_DS //bnum   // num of byte
               , 16 //bleng  // burst length: 1, 2, ...
               , 1 //burst  // 0:fixed, 1:incr, 2:wrap
               , 0 //lock   // 2'b10:lock, 2'b01:excl
               , 0 //delay  // 0:don't use delay
     );
     axi_master_read_multiple_outstanding(
                 4 //arnum  // num of multiple outstanding
               , saddr
               , WIDTH_DS //bnum   // num of byte
               , 16 //bleng  // burst length: 1, 2, ...
               , 1 //burst  // 0:fixed, 1:incr, 2:wrap
               , 0 //lock   // 2'b10:lock, 2'b01:excl
               , 0 //delay  // 0:don't use delay
     );
     busy_out = 1'b0;
     @ (posedge ACLK);
     repeat (5) @ (posedge ACLK);
     while (busy_in==1'b1) @ (posedge ACLK);
end
end // if (EN==1
           stage = 0;
           //-----------------------------------------------------
           repeat (5) @ (posedge ACLK);
           CACTIVE = 1'b0;
           //-----------------------------------------------------
           done = 1'b1;
           //$display("AXI statistics ===========================");
           axi_statistics(P_MST_ID);
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
// 2021.07.10: "$value$plusargs("BURST_MISALIGNED_TEST=%d", ..)" added
//              to test mis-aligned burst cases.
// 2010.12.30: Started by Ando Ki (adki@future-ds.com)
//----------------------------------------------------------------
