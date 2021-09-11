//------------------------------------------------------------------------------
// Copyright (c) 2021 by Ando Ki.
// All right reserved.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//------------------------------------------------------------------------------
// axi4to3.v
//------------------------------------------------------------------------------
// It converts AMBA AXI4 to AXI3 by spliting 16-burst transactions.
//------------------------------------------------------------------------------
// VERSION: 2021.08.04.
//------------------------------------------------------------------------------
//  [MACROS]
//------------------------------------------------------------------------------
//  [PARAMETER]
//------------------------------------------------------------------------------

module axi4to3
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
     , input  wire [ 7:0]              S_AWLEN
     , input  wire                     S_AWLOCK
     , input  wire [ 2:0]              S_AWSIZE
     , input  wire [ 1:0]              S_AWBURST
     `ifdef AMBA_AXI_CACHE
     , input  wire [ 3:0]              S_AWCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , input  wire [ 2:0]              S_AWPROT
     `endif
     , input  wire                     S_AWVALID
     , output reg                      S_AWREADY=1'b0
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
     , input  wire [ 7:0]              S_ARLEN
     , input  wire                     S_ARLOCK
     , input  wire [ 2:0]              S_ARSIZE
     , input  wire [ 1:0]              S_ARBURST
     `ifdef AMBA_AXI_CACHE
     , input  wire [ 3:0]              S_ARCACHE
     `endif
     `ifdef AMBA_AXI_PROT
     , input  wire [ 2:0]              S_ARPROT
     `endif
     , input  wire                     S_ARVALID
     , output reg                      S_ARREADY=1'b0
     , output wire [AXI_WIDTH_SID-1:0] S_RID
     , output wire [AXI_WIDTH_DA-1:0]  S_RDATA
     , output wire [ 1:0]              S_RRESP
     , output wire                     S_RLAST
     , output wire                     S_RVALID
     , input  wire                     S_RREADY
     //-------------------------------------------------------------------------
     , output reg  [AXI_WIDTH_ID-1:0]  M_AWID='h0
     , output reg  [AXI_WIDTH_AD-1:0]  M_AWADDR='h0
     , output reg  [ 3:0]              M_AWLEN=4'h0
     , output reg  [ 1:0]              M_AWLOCK=2'h0
     , output reg  [ 2:0]              M_AWSIZE=3'h0
     , output reg  [ 1:0]              M_AWBURST=2'h0
     `ifdef AMBA_AXI_CACHE
     , output reg  [ 3:0]              M_AWCACHE=4'h0
     `endif
     `ifdef AMBA_AXI_PROT
     , output reg  [ 2:0]              M_AWPROT=3'h0
     `endif
     , output reg                      M_AWVALID=1'b0
     , input  wire                     M_AWREADY
     , output wire [AXI_WIDTH_SID-1:0] M_WID
     , output wire [AXI_WIDTH_DA-1:0]  M_WDATA
     , output wire [AXI_WIDTH_DS-1:0]  M_WSTRB
     , output wire                     M_WLAST
     , output wire                     M_WVALID
     , input  wire                     M_WREADY
     , input  wire [AXI_WIDTH_ID-1:0]  M_BID
     , input  wire [ 1:0]              M_BRESP
     , input  wire                     M_BVALID
     , output wire                     M_BREADY
     , output reg  [AXI_WIDTH_ID-1:0]  M_ARID='h0
     , output reg  [AXI_WIDTH_AD-1:0]  M_ARADDR='h0
     , output reg  [ 3:0]              M_ARLEN=4'h0
     , output reg  [ 1:0]              M_ARLOCK=2'b0
     , output reg  [ 2:0]              M_ARSIZE=3'h0
     , output reg  [ 1:0]              M_ARBURST=2'b0
     `ifdef AMBA_AXI_CACHE
     , output reg  [ 3:0]              M_ARCACHE=4'h0
     `endif
     `ifdef AMBA_AXI_PROT
     , output reg  [ 2:0]              M_ARPROT=3'h0
     `endif
     , output reg                      M_ARVALID=1'b0
     , input  wire                     M_ARREADY
     , input  wire [AXI_WIDTH_ID-1:0]  M_RID
     , input  wire [AXI_WIDTH_DA-1:0]  M_RDATA
     , input  wire [ 1:0]              M_RRESP
     , input  wire                     M_RLAST
     , input  wire                     M_RVALID
     , output wire                     M_RREADY
);
     //-----------------------------------------------------------
     // write case
     //-----------------------------------------------------------
     reg  [AXI_WIDTH_SID-1:0] AWID_reg   ='h0;
     reg  [AXI_WIDTH_AD-1:0]  AWADDR_reg ='h0;
     reg  [ 7:0]              AWLEN_reg  ='h0;
     reg                      AWLOCK_reg ='h0;
     reg  [ 2:0]              AWSIZE_reg ='h0;
     reg  [ 1:0]              AWBURST_reg='h0;
     reg  [ 3:0]              AWCACHE_reg='h0;
     reg  [ 2:0]              AWPROT_reg ='h0;
     //-----------------------------------------------------------
     reg  [AXI_WIDTH_AD-1:0]  addrW='h0; // address of each transfer within a burst
     reg  [ 7:0]              beatW='h0; // keeps num of transfers within a burst
     //-----------------------------------------------------------
     localparam STW_READY = 'h0
              , STW_ADDR  = 'h1
              , STW_DATA  = 'h2
              , STW_RSP   = 'h3
              , STW_END   = 'h4;
     reg [2:0] stateW=STW_READY;
     always @ (posedge ACLK or negedge ARESETn) begin
         if (ARESETn==1'b0) begin
             AWID_reg    <=  'h0;
             AWADDR_reg  <=  'h0;
             AWLEN_reg   <=  'h0;
             AWSIZE_reg  <=  'b0;
             AWBURST_reg <=  'b0;
             AWLOCK_reg  <=  'b0;
             AWCACHE_reg <=  'h0;
             AWPROT_reg  <=  'b0;
             S_AWREADY   <= 1'b0;
             addrW       <=  'h0;
             beatW       <=  'h0;
             M_AWID      <=  'h0;
             M_AWADDR    <=  'h0;
             M_AWLEN     <= 4'h0;
             M_AWLOCK    <= 2'h0;
             M_AWSIZE    <= 3'h0;
             M_AWBURST   <= 2'h0;
             `ifdef AMBA_AXI_CACHE
             M_AWCACHE   <= 4'h0;
             `endif
             `ifdef AMBA_AXI_PROT
             M_AWPROT    <= 3'h0;
             `endif
             M_AWVALID   <= 1'b0;
             stateW      <= STW_READY;
         end else begin
             case (stateW)
             STW_READY: begin
                 if ((S_AWVALID==1'b1)&&(S_AWREADY==1'b1)) begin
                      AWID_reg    <= S_AWID   ;
                      AWADDR_reg  <= S_AWADDR ;
                      AWLEN_reg   <= S_AWLEN  ;
                      AWSIZE_reg  <= S_AWSIZE ;
                      AWBURST_reg <= S_AWBURST;
                      AWLOCK_reg  <= S_AWLOCK ;
                      `ifdef AMBA_AXI_CACHE
                      AWCACHE_reg <= S_AWCACHE;
                      `else
                      AWCACHE_reg <= 'h0;
                      `endif
                      `ifdef AMBA_AXI_PROT
                      AWPROT_reg  <= S_AWPROT ;
                      `else
                      AWPROT_reg  <= 'h0;
                      `endif
                      addrW       <= S_AWADDR;
                      beatW       <= 'h0;
                      stateW      <= STW_ADDR;
                      S_AWREADY   <= 1'b0;
                      M_AWID      <= S_AWID;
                      M_AWADDR    <= S_AWADDR;
                      M_AWLEN     <=(S_AWLEN[7:4]) ? 4'hF : S_AWLEN[3:0];
                      M_AWLOCK    <= {1'b0,S_AWLOCK};
                      M_AWSIZE    <= S_AWSIZE ;
                      M_AWBURST   <= S_AWBURST;
                      `ifdef AMBA_AXI_CACHE
                      M_AWCACHE   <= S_AWCACHE;
                      `endif
                      `ifdef AMBA_AXI_PROT
                      M_AWPROT    <= M_AWPROT;
                      `endif
                      M_AWVALID   <= 1'b1;
                 end else begin
                      S_AWREADY <= 1'b1;
                 end
                 end // STW_READY
             STW_ADDR: begin
                 if (M_AWREADY==1'b1) begin
                      M_AWVALID <= 1'b0;
                      stateW    <= STW_DATA;
                 end
                 end // STW_ADDR
             STW_DATA: begin
                 if ((S_WVALID==1'b1)&&(S_WREADY==1'b1)&&
                     (M_WVALID==1'b1)&&(M_WREADY==1'b1)) begin
                     beatW  <= beatW + 1;
                     addrW  <= get_next_addr_wr(addrW,AWSIZE_reg,AWBURST_reg,AWLEN_reg);
                     if (M_WLAST==1'b1) begin
                         if (beatW==AWLEN_reg) begin
                             stateW   <= STW_END;
                         end else begin
                             stateW   <= STW_RSP;
                         end
                     end
                 end
                 end // STW_WRITE0
             STW_RSP: begin
                 if (M_BVALID==1'b1) begin
                     M_AWADDR  <= addrW;
                     M_AWLEN   <= ((AWLEN_reg-beatW)>'hF) ? 'hF : AWLEN_reg-beatW;
                     M_AWVALID <= 1'b1;
                     stateW    <= STW_ADDR;
                 end
                 end // STW_RSP
             STW_END: begin
                 if ((S_BVALID==1'b1)&&(S_BREADY==1'b1)&&
                     (M_BVALID==1'b1)&&(M_BREADY==1'b1)) begin
                     S_AWREADY <= 1'b0;
                     stateW    <= STW_READY;
                 end
                 end // STW_END
             endcase
         end
     end
    //--------------------------------------------------------------------------
    assign S_WREADY = (stateW==STW_DATA) ? M_WREADY : 1'b0;
    assign S_BID    = M_BID  ;
    assign S_BRESP  = M_BRESP;
    assign S_BVALID = (stateW==STW_END ) ? M_BVALID : 1'b0;
    assign M_WID    = (stateW==STW_DATA) ? AWID_reg :  'h0;
    assign M_WDATA  = (stateW==STW_DATA) ? S_WDATA  :  'h0;
    assign M_WSTRB  = (stateW==STW_DATA) ? S_WSTRB  :  'h0;
    assign M_WLAST  = (stateW==STW_DATA) ? S_WLAST|(beatW[3:0]==4'hF) : 1'b0;
    assign M_WVALID = (stateW==STW_DATA) ? S_WVALID : 1'b0;
    assign M_BREADY = (stateW==STW_DATA)||(stateW==STW_RSP)||(stateW==STW_END);
    //--------------------------------------------------------------------------
    // synthesis translate_off
    reg  [8*10-1:0] stateW_ascii="READY";
    always @ (stateW) begin
    case (stateW)
    STW_READY: stateW_ascii="READY  ";
    STW_ADDR : stateW_ascii="ADDR   ";
    STW_DATA : stateW_ascii="DATA   ";
    STW_RSP  : stateW_ascii="RSP    ";
    STW_END  : stateW_ascii="END    ";
    default  : stateW_ascii="UNKNOWN";
    endcase
    end
    // synthesis translate_on
    //--------------------------------------------------------------------------
     function [AXI_WIDTH_AD-1:0] get_next_addr_wr;
          input [AXI_WIDTH_AD-1:0] addr ;
          input [ 2:0]             size ;
          input [ 1:0]             burst; // burst type
          input [ 7:0]             len  ; // burst length
          reg   [AXI_WIDTH_AD-AXI_WIDTH_DSB-1:0] naddr;
          reg   [AXI_WIDTH_AD-1:0] mask ;
     begin
          case (burst)
          2'b00: get_next_addr_wr = addr;
          2'b01: begin
                 if ((1<<size)<AXI_WIDTH_DS) begin
                    get_next_addr_wr = addr + (1<<size);
                 end else begin
                     naddr = addr[AXI_WIDTH_AD-1:AXI_WIDTH_DSB];
                     naddr = naddr + 1;
                     get_next_addr_wr = {naddr,{AXI_WIDTH_DSB{1'b0}}};
                 end
                 end
          2'b10: begin
                 `ifdef BURST_TYPE_WRAPP_ENABLED
                 mask          = get_wrap_mask(size,len);
                 get_next_addr_wr = (addr&~mask)
                               | (((addr&mask)+(1<<size))&mask);
                 `else
                 // synopsys translate_off
                 $display($time,,"%m ERROR BURST WRAP not supported");
                 // synopsys translate_on
                 `endif
                 end
          2'b11: begin
                 get_next_addr_wr = addr;
                 // synopsys translate_off
                 $display($time,,"%m ERROR un-defined BURST %01x", burst);
                 // synopsys translate_on
                 end
          endcase
     end
     endfunction
    //--------------------------------------------------------------------------
     `ifdef BURST_TYPE_WRAPP_ENABLED
     function [AXI_WIDTH_AD-1:0] get_wrap_mask;
          input [ 2:0]      size ;
          input [ 7:0]      len  ; // burst length
     begin
          case (size)
          3'b000: get_wrap_mask = (    len)-1;
          3'b001: get_wrap_mask = (  2*len)-1;
          3'b010: get_wrap_mask = (  4*len)-1;
          3'b011: get_wrap_mask = (  8*len)-1;
          3'b100: get_wrap_mask = ( 16*len)-1;
          3'b101: get_wrap_mask = ( 32*len)-1;
          3'b110: get_wrap_mask = ( 64*len)-1;
          3'b111: get_wrap_mask = (128*len)-1;
          endcase
     end
     endfunction
     `endif
     //-----------------------------------------------------------
     // read case
     //-----------------------------------------------------------
     reg  [AXI_WIDTH_AD-1:0] ARADDR_reg ='h0;
     reg  [ 7:0]             ARLEN_reg  ='h0;
     reg                     ARLOCK_reg ='h0;
     reg  [ 2:0]             ARSIZE_reg ='h0;
     reg  [ 1:0]             ARBURST_reg='h0;
     reg  [ 3:0]             ARCACHE_reg='h0;
     reg  [ 2:0]             ARPROT_reg ='h0;
     //-----------------------------------------------------------
     reg  [AXI_WIDTH_AD-1:0] addrR='h0; // address of each transfer within a burst
     reg  [ 7:0]             beatR='h0; // keeps num of transfers within a burst
     //-----------------------------------------------------------
     localparam STR_READY  = 'h0
              , STR_ADDR   = 'h1
              , STR_DATA   = 'h2;
     reg [1:0] stateR=STR_READY;
     always @ (posedge ACLK or negedge ARESETn) begin
         if (ARESETn==1'b0) begin
             ARADDR_reg  <=  'h0;
             ARLEN_reg   <=  'h0;
             ARLOCK_reg  <=  'b0;
             ARSIZE_reg  <=  'b0;
             ARBURST_reg <=  'b0;
             ARCACHE_reg <=  'h0;
             ARPROT_reg  <=  'b0;
             S_ARREADY   <= 1'b0;
             M_ARID      <=  'h0;
             M_ARADDR    <=  'h0;
             M_ARLEN     <=  'h0;
             M_ARSIZE    <=  'h0;
             M_ARBURST   <=  'h0;
             M_ARLOCK    <=  'h0;
             `ifdef AMBA_AXI_CACHE
             M_ARCACHE   <=  'h0;
             `endif
             `ifdef AMBA_AXI_PROT
             M_ARPROT    <=  'h0;
             `endif
             M_ARVALID   <= 1'b0;
             addrR       <=  'h0;
             beatR       <=  'h0;
             stateR      <= STR_READY;
         end else begin
             case (stateR)
             STR_READY: begin
                 if ((S_ARVALID==1'b1)&&(S_ARREADY==1'b1)) begin
                      ARADDR_reg  <= S_ARADDR ;
                      ARLEN_reg   <= S_ARLEN  ;
                      ARSIZE_reg  <= S_ARSIZE ;
                      ARBURST_reg <= S_ARBURST;
                      ARLOCK_reg  <= S_ARLOCK ;
                      `ifdef AMBA_AXI_CACHE
                      ARCACHE_reg <= S_ARCACHE;
                      `else
                      ARCACHE_reg <= 'h0;
                      `endif
                      `ifdef AMBA_AXI_PROT
                      ARPROT_reg  <= S_ARPROT ;
                      `else
                      ARPROT_reg  <= 'h0;
                      `endif
                      S_ARREADY   <= 1'b0;
                      M_ARID      <= S_ARID;
                      M_ARADDR    <= S_ARADDR ;
                      M_ARLEN     <=(S_ARLEN[7:4]) ? 4'hF : S_ARLEN[3:0];
                      M_ARSIZE    <= S_ARSIZE ;
                      M_ARBURST   <= S_ARBURST;
                      M_ARLOCK    <= S_ARLOCK ;
                      `ifdef AMBA_AXI_CACHE
                      M_ARCACHE   <= S_ARCACHE;
                      `endif
                      `ifdef AMBA_AXI_PROT
                      M_ARPROT    <= S_ARPROT ;
                      `endif
                      M_ARVALID   <= 1'b1;
                      addrR       <= get_next_addr_rd(S_ARADDR,S_ARSIZE,S_ARBURST,S_ARLEN);
                      beatR       <= 'h0;
                      stateR      <= STR_ADDR;
                 end else begin
                     S_ARREADY <= 1'b1;
                 end
                 end // STR_READY
             STR_ADDR: begin
                 if (M_ARREADY==1'b1) begin
                      M_ARVALID <= 1'b0;
                      stateR    <= STR_DATA;
                 end
                 end // STR_ADDR
             STR_DATA: begin
                 if ((S_RVALID==1'b1)&&(S_RREADY==1'b1)&&
                     (M_RVALID==1'b1)&&(M_RREADY==1'b1)) begin
                     beatR  <= beatR + 1;
                     addrR  <= get_next_addr_rd(addrR,ARSIZE_reg,ARBURST_reg,ARLEN_reg);
                     if (beatR>=ARLEN_reg) begin
                        S_ARREADY <= 1'b1;
                        stateR    <= STR_READY;
                     end else if (beatR[3:0]=='hF) begin
                        M_ARADDR  <= addrR;
                        M_ARLEN   <=((ARLEN_reg-beatR)>='hF) ? 4'hF : ARLEN_reg-beatR-1;
                        M_ARVALID <= 1'b1;
                        stateR    <= STR_ADDR;
                     end
                 end
                 end // STR_DATA
             endcase
         end
     end
    //--------------------------------------------------------------------------
    assign M_RREADY = (stateR==STR_DATA) ? S_RREADY : 1'b0;
    assign S_RID    = M_RID;
    assign S_RDATA  = M_RDATA;
    assign S_RRESP  = M_RRESP;
    assign S_RLAST  = (stateR==STR_DATA)&&(beatR==ARLEN_reg);
    assign S_RVALID = (stateR==STR_DATA) ? M_RVALID : 1'b0;
    //--------------------------------------------------------------------------
    // synthesis translate_off
    reg  [8*10-1:0] stateR_ascii="READY";
    always @ (stateR) begin
    case (stateR)
    STR_READY: stateR_ascii="READY  ";
    STR_ADDR : stateR_ascii="ADDR   ";
    STR_DATA : stateR_ascii="DATA   ";
    default  : stateR_ascii="UNKNOWN";
    endcase
    end
    // synthesis translate_on
    //--------------------------------------------------------------------------
     function [AXI_WIDTH_AD-1:0] get_next_addr_rd;
          input [AXI_WIDTH_AD-1:0] addr ;
          input [ 2:0]             size ;
          input [ 1:0]             burst; // burst type
          input [ 7:0]             len  ; // burst length
          reg   [AXI_WIDTH_AD-AXI_WIDTH_DSB-1:0] naddr;
          reg   [AXI_WIDTH_AD-1:0] mask ;
     begin
          case (burst)
          2'b00: get_next_addr_rd = addr;
          2'b01: begin
                 if ((1<<size)<AXI_WIDTH_DS) begin
                    get_next_addr_rd = addr + (1<<size);
                 end else begin
                     naddr = addr[AXI_WIDTH_AD-1:AXI_WIDTH_DSB];
                     naddr = naddr + 1;
                     get_next_addr_rd = {naddr,{AXI_WIDTH_DSB{1'b0}}};
                 end
                 end
          2'b10: begin
                 `ifdef BURST_TYPE_WRAPP_ENABLED
                 mask          = get_wrap_mask(size,len);
                 get_next_addr_rd = (addr&~mask)
                               | (((addr&mask)+(1<<size))&mask);
                 `else
                 // synopsys translate_off
                 $display($time,,"%m ERROR BURST WRAP not supported");
                 // synopsys translate_on
                 `endif
                 end
          2'b11: begin
                 get_next_addr_rd = addr;
                 // synopsys translate_off
                 $display($time,,"%m ERROR un-defined BURST %01x", burst);
                 // synopsys translate_on
                 end
          endcase
     end
     endfunction
    //--------------------------------------------------------------------------
endmodule

//------------------------------------------------------------------------------
// Revision History
//
// 2021.09.04: Started by Ando Ki (adki@future-ds.com)
//------------------------------------------------------------------------------
