//----------------------------------------------------------------
// Copyright (c) 2021 by Ando Ki.
// All right reserved.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//----------------------------------------------------------------
// axi_tester.v
//----------------------------------------------------------------
// VERSION: 2021.09.05.
//----------------------------------------------------------------
`timescale 1ns/1ns

module axi4_tester
    #(parameter WIDTH_ID =4         // ID width in bits
              , WIDTH_AD =32        // address width
              , WIDTH_DA =32        // data width
              , WIDTH_DS =(WIDTH_DA/8) // data strobe width
              , WIDTH_DSB=$clog2(WIDTH_DS) // data strobe width
              , P_SIZE_IN_BYTES=2*1024*1024
              )
(
       input  wire                 ARESETn
     , input  wire                 ACLK
     //-----------------------------------------------------------
     , output reg  [WIDTH_ID-1:0]  AWID
     , output reg  [WIDTH_AD-1:0]  AWADDR
     , output reg  [ 7:0]          AWLEN
     , output reg                  AWLOCK
     , output reg  [ 2:0]          AWSIZE
     , output reg  [ 1:0]          AWBURST
     `ifdef AMBA_AXI_CACHE
     , output reg  [ 3:0]          AWCACHE=4'h0
     `endif
     `ifdef AMBA_AXI_PROT
     , output reg  [ 2:0]          AWPROT=3'h0
     `endif
     , output reg                  AWVALID
     , input  wire                 AWREADY
     //-----------------------------------------------------------
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
     , output reg  [ 7:0]          ARLEN
     , output reg                  ARLOCK
     , output reg  [ 2:0]          ARSIZE
     , output reg  [ 1:0]          ARBURST
     `ifdef AMBA_AXI_CACHE
     , output reg  [ 3:0]          ARCACHE=4'h0
     `endif
     `ifdef AMBA_AXI_PROT
     , output reg  [ 2:0]          ARPROT=3'h0
     `endif
     , output reg                  ARVALID
     , input  wire                 ARREADY
     //-----------------------------------------------------------
     , input  wire [WIDTH_ID-1:0]  RID
     , input  wire [WIDTH_DA-1:0]  RDATA
     , input  wire [ 1:0]          RRESP
     , input  wire                 RLAST
     , input  wire                 RVALID
     , output reg                  RREADY
     //-----------------------------------------------------------
);
     //-----------------------------------------------------------
     localparam SIZE_IN_BYTES=(P_SIZE_IN_BYTES/WIDTH_DS)*WIDTH_DS;
     localparam ADDR_LENGTH=$clog2(SIZE_IN_BYTES);
     //-----------------------------------------------------------
     reg done=1'b0;
     //-----------------------------------------------------------
     reg [WIDTH_DA-1:0] dataR[0:1023]; // non-justified data
     reg [WIDTH_DA-1:0] dataW[0:1023]; // non-justified data
     `include "axi4_master_tasks.v"
     `include "mem_test_tasks.v"
     //-----------------------------------------------------------
     integer arg; // for commandline test plus arguments
     reg [15:0]         bnum ; initial bnum  = 0;
     reg [15:0]         blen ; initial blen  = 0;
     reg                delay; initial delay = 0;
     reg [WIDTH_AD-1:0] saddr, depth;
     integer    idx;
     //-----------------------------------------------------------
     initial begin
           AWID        = 0;
           AWADDR      = ~0;
           AWLEN       = 0;
           AWLOCK      = 0;
           AWSIZE      = 0;
           AWBURST     = 0;
           AWVALID     = 0;
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
           ARVALID     = 0;
           RREADY      = 0; 
           wait (ARESETn==1'b0);
           wait (ARESETn==1'b1);
           repeat (5) @ (posedge ACLK);
           //-----------------------------------------------------
if ($value$plusargs("SINGLE_TEST=%d", arg) && arg) begin
           $display("SINGLE_TEST================");
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID;
           for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
               test_single(saddr, bnum, delay);
           end
           @ (posedge ACLK);
end
if ($value$plusargs("BURST_TEST=%d", arg) && arg) begin
           $display("BURST_TEST================");
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID; //saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
           for (blen=1; blen<=33; blen=blen*2) begin
              for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                test_burst (saddr, bnum, blen, 2'b01, delay, 0);
               end
           end
           for (blen=1; blen<=256; blen=blen*2+bnum) begin
              for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                test_burst (saddr, bnum, blen, 2'b01, delay, 0);
               end
           end
           @ (posedge ACLK);
end
if ($value$plusargs("BURST_RANDOM_TEST=%d", arg) && arg) begin
           $display("BURST_RANDOM_TEST================");
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID; //saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
           for (blen=1; blen<=256; blen=blen*2) begin
               for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                test_burst (saddr, bnum, blen, 2'b01, delay, arg+1);
               end
           end
           for (blen=1; blen<=256; blen=blen*2+bnum) begin
               for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                test_burst (saddr, bnum, blen, 2'b01, delay, arg+1);
               end
           end
           @ (posedge ACLK);
end
if ($value$plusargs("BURST_MISALIGNED_TEST=%d", arg) && arg) begin
           $display("BURST_MISALIGNED_TEST================");
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID; //saddr = ((P_MST_ID-1)<<ADDR_LENGTH);
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
           @ (posedge ACLK);
end
if ($value$plusargs("SINGLE_TEST_MEM=%d", arg) && arg) begin
           $display("SINGLE_TEST_MEM %d ================", arg);
           depth = 'h10;
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID;
           mem_test(saddr, saddr + depth-1, 1, delay);
           @ (posedge ACLK);
end
if ($value$plusargs("SINGLE_TEST_MEM=%d", arg) && arg) begin
           $display("SINGLE_TEST_MEM %d ================", arg);
           depth = 'h10;
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID;
           for (bnum=1; bnum<=WIDTH_DS; bnum=bnum*2) begin
                mem_test(saddr, saddr + depth-1, bnum, delay);
           end
           @ (posedge ACLK);
end
if ($value$plusargs("BURST_TEST_MEM=%d", arg) && arg) begin
           $display("BURST_TEST_MEM %d ================", arg);
           depth = 4*16*4;
           saddr = 0; //SIZE_IN_BYTES*P_MST_ID;
           for (bnum=4; bnum<=WIDTH_DS; bnum=bnum*2) begin
               for (blen=1; blen<=16; blen=blen+1) begin
                   mem_test_burst(saddr, saddr + depth - 1, bnum, blen, delay);
               end
           end
           @ (posedge ACLK);
           repeat (5) @ (posedge ACLK);
end
           //-----------------------------------------------------
           repeat (5) @ (posedge ACLK);
           //-----------------------------------------------------
           done = 1'b1;
     end
     //-----------------------------------------------------------
endmodule
//----------------------------------------------------------------
// Revision History
//
// 2021.09.05: Started by Ando Ki (adki@future-ds.com)
//----------------------------------------------------------------
