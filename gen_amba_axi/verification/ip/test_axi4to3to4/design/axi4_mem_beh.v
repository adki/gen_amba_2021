//------------------------------------------------------------------------------
// Copyright (c) 2021 by Ando Ki.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//------------------------------------------------------------------------------
// axi4_mem.v
//------------------------------------------------------------------------------
// VERSION: 2021.07.01.
//------------------------------------------------------------------------------
// MACROS:
//    BURST_TYPE_WRAPP_ENABLED   - Burst wrapping type enabled
// PARAMETERS:
//    P_SIZE_IN_BYTES - size of memory in bytes
//    P_DELAY_WRITE_SETUP - delay to the first data from address cycle.
//    P_DELAY_WRITE_BURST - delay between consecutive data cycles.
//    P_DELAY_READ_SETUP  - delay to the first data from address cycle.
//    P_DELAY_READ_BURST  - delay between consecutive data cycles.
// Note:
//    read-first (not write-through)
//------------------------------------------------------------------------------

module axi4_mem_beh
     #(parameter AXI_WIDTH_ID =4
               , AXI_WIDTH_AD =32 // address width
               , AXI_WIDTH_DA =32 // data width
               , AXI_WIDTH_DS =(AXI_WIDTH_DA/8)  // data strobe width
               , AXI_WIDTH_DSB=$clog2(AXI_WIDTH_DS) // data strobe width
               , AXI_WIDTH_SID=AXI_WIDTH_ID
               , P_SIZE_IN_BYTES=1024
               , P_DELAY_WRITE_SETUP=0
               , P_DELAY_WRITE_BURST=0
               , P_DELAY_READ_SETUP=0
               , P_DELAY_READ_BURST=0
               )
(
       input  wire                     ARESETn
     , input  wire                     ACLK
     , input  wire [AXI_WIDTH_SID-1:0] AWID
     , input  wire [AXI_WIDTH_AD-1:0]  AWADDR
     , input  wire [ 7:0]              AWLEN
     , input  wire                     AWLOCK
     , input  wire [ 2:0]              AWSIZE
     , input  wire [ 1:0]              AWBURST
     , input  wire                     AWVALID
     , output reg                      AWREADY
     , input  wire [AXI_WIDTH_DA-1:0]  WDATA
     , input  wire [AXI_WIDTH_DS-1:0]  WSTRB
     , input  wire                     WLAST
     , input  wire                     WVALID
     , output reg                      WREADY
     , output reg  [AXI_WIDTH_SID-1:0] BID
     , output reg  [ 1:0]              BRESP
     , output reg                      BVALID
     , input  wire                     BREADY
     , input  wire [AXI_WIDTH_SID-1:0] ARID
     , input  wire [AXI_WIDTH_AD-1:0]  ARADDR
     , input  wire [ 7:0]              ARLEN
     , input  wire                     ARLOCK
     , input  wire [ 2:0]              ARSIZE
     , input  wire [ 1:0]              ARBURST
     , input  wire                     ARVALID
     , output reg                      ARREADY
     , output reg  [AXI_WIDTH_SID-1:0] RID
     , output reg  [AXI_WIDTH_DA-1:0]  RDATA
     , output reg  [ 1:0]              RRESP
     , output reg                      RLAST
     , output reg                      RVALID
     , input  wire                     RREADY
);
     //-------------------------------------------------------------------------
     localparam ADDR_LENGTH=$clog2(P_SIZE_IN_BYTES)
              , DEPTH = (1<<(ADDR_LENGTH-AXI_WIDTH_DSB));
     //-------------------------------------------------------------------------
     reg [AXI_WIDTH_DA-1:0] mem[0:DEPTH-1]; // synthesis syn_ramstyle="block_ram"; 
     //-------------------------------------------------------------------------
     // write case
     //-------------------------------------------------------------------------
     reg  [AXI_WIDTH_SID-1:0] AWID_reg   ='h0;
     reg  [AXI_WIDTH_AD-1:0]  AWADDR_reg ='h0;
     reg  [ 7:0]              AWLEN_reg  ='h0;
     reg  [ 2:0]              AWSIZE_reg ='h0;
     reg  [ 1:0]              AWBURST_reg='h0;
     //-------------------------------------------------------------------------
     reg  [ADDR_LENGTH-1:0] addrW='h0; // address of each transfer within a burst
     reg  [ 7:0]            beatW=8'h0; // keeps num of transfers within a burst
     reg  [ 7:0]            delayW=8'h0;
     integer idz;
     //-------------------------------------------------------------------------
     localparam STW_READY= 'h0
              , STW_DELAY= 'h1
              , STW_WRITE= 'h2
              , STW_RSP  = 'h3;
     reg [1:0] stateW=STW_READY;
     always @ (posedge ACLK or negedge ARESETn) begin
     if (ARESETn==1'b0) begin
         AWID_reg    <= 'h0;
         AWADDR_reg  <= 'h0;
         AWLEN_reg   <= 'h0;
         AWSIZE_reg  <= 'b0;
         AWBURST_reg <= 'b0;
         AWREADY     <= 1'b0;
         WREADY      <= 1'b0;
         BID         <=  'h0;
         BRESP       <= 2'b10; // SLAVE ERROR
         BVALID      <= 1'b0;
         addrW       <=  'h0;
         beatW       <=  'h0;
         delayW      <=  'h0;
         stateW      <= STW_READY;
     end else begin
         case (stateW)
         STW_READY: begin
             if ((AWVALID==1'b1)&&(AWREADY==1'b1)) begin
                  AWID_reg    <= AWID   ;
                  AWADDR_reg  <= AWADDR ;
                  AWLEN_reg   <= AWLEN  ;
                  AWSIZE_reg  <= AWSIZE ;
                  AWBURST_reg <= AWBURST;
                  AWREADY     <= 1'b0;
                  BRESP       <= 2'b00; // OKAY
                  addrW       <= AWADDR[ADDR_LENGTH-1:0];
                  beatW       <=  'h0;
                  if (P_DELAY_WRITE_SETUP==0) begin
                      WREADY <= 1'b1;
                      delayW <= P_DELAY_WRITE_BURST;
                      stateW <= STW_WRITE;
                  end else begin
                      WREADY <= 1'b0;
                      delayW <= P_DELAY_WRITE_SETUP-1;
                      stateW <= STW_DELAY;
                  end
             end else begin
                  AWREADY <= 1'b1;
             end
             end // STW_READY
         STW_DELAY: begin
             delayW <= delayW - 1;
             if (delayW==0) begin
                  WREADY <= 1'b1;
                  stateW <= STW_WRITE;
             end
             end // STW_DELAY
         STW_WRITE: begin
             if (WVALID==1'b1) begin
                 for (idz=0; idz<AXI_WIDTH_DS; idz=idz+1) begin
                      if (WSTRB[idz]) mem[addrW[ADDR_LENGTH-1:AXI_WIDTH_DSB]][8*idz +: 8]
                                      <= WDATA[8*idz +: 8];
                 end
                 beatW  <= beatW + 1;
                 addrW  <= get_next_addr_wr(addrW,AWSIZE_reg
                                           ,AWBURST_reg,AWLEN_reg);
                 if (beatW>=AWLEN_reg) begin
                     WREADY <= 1'b0;
                     BVALID <= 1'b1;
                     BID    <= AWID_reg;
                     if (WLAST==1'b0) BRESP <= 2'b10; // SLVERR - missing last
                     stateW <= STW_RSP;
                 end else begin
                     if (P_DELAY_WRITE_BURST!=0) begin
                         WREADY <= 1'b0;
                         delayW <= P_DELAY_WRITE_BURST-1;
                         stateW <= STW_DELAY;
                     end
                 end
             end
             end // STW_WRITE
         STW_RSP: begin
             if (BREADY==1'b1) begin
                 BVALID  <= 1'b0;
                 AWREADY <= 1'b1;
                 beatW   <=  'h0;
                 stateW  <= STW_READY;
             end
             end // STW_RSP
         endcase
     end // if
     end // always
     //-------------------------------------------------------------------------
     // synthesis translate_off
     reg  [8*10-1:0] stateW_ascii="READY";
     always @ (stateW) begin
     case (stateW)
     STW_READY: stateW_ascii="READY  ";
     STW_DELAY: stateW_ascii="DELAY  ";
     STW_WRITE: stateW_ascii="WRITE  ";
     STW_RSP  : stateW_ascii="RSP    ";
     default  : stateW_ascii="UNKNOWN";
     endcase
     end
     // synthesis translate_on
     //-------------------------------------------------------------------------
     // read case
     //-------------------------------------------------------------------------
     reg  [AXI_WIDTH_AD-1:0]  ARADDR_reg ='h0;
     reg  [ 7:0]              ARLEN_reg  ='h0;
     reg  [ 2:0]              ARSIZE_reg ='h0;
     reg  [ 1:0]              ARBURST_reg='h0;
     //-------------------------------------------------------------------------
     reg  [AXI_WIDTH_DA-1:0]  dataR='h0;
     reg  [ADDR_LENGTH-1:0]   addrR='h0; // address of each transfer within a burst
     reg  [ 7:0]              beatR='h0; // keeps num of transfers within a burst
     reg  [ 7:0]              delayR='h0;
     //-------------------------------------------------------------------------
     localparam STR_READY= 'h0
              , STR_DELAY= 'h1
              , STR_READ = 'h2
              , STR_END  = 'h3;
     reg [1:0] stateR=STR_READY;
     always @ (posedge ACLK or negedge ARESETn) begin
     if (ARESETn==1'b0) begin
         ARADDR_reg  <= 'h0;
         ARLEN_reg   <= 'h0;
         ARSIZE_reg  <= 'b0;
         ARBURST_reg <= 'b0;
         ARREADY     <= 1'b0;
         RID         <=  'h0;
         RLAST       <= 1'b0;
         RRESP       <= 2'b10; // SLAERROR
         RDATA       <=  'h0;
         RVALID      <= 1'b0;
         dataR       <=  'h0;
         addrR       <=  'h0;
         beatR       <=  'h0;
         delayR      <=  'h0;
         stateR      <= STR_READY;
     end else begin
         case (stateR)
         STR_READY: begin
             if ((ARVALID==1'b1)&&(ARREADY==1'b1)) begin
                  ARADDR_reg  <= ARADDR ;
                  ARLEN_reg   <= ARLEN  ;
                  ARSIZE_reg  <= ARSIZE ;
                  ARBURST_reg <= ARBURST;
                  ARREADY     <= 1'b0;
                  RID         <= ARID;
                  RRESP       <= 2'b00;
                  addrR       <= ARADDR;
                  if (P_DELAY_READ_SETUP==0) begin
                      RDATA  <= mem[ARADDR[ADDR_LENGTH-1:AXI_WIDTH_DSB]];
                      RVALID <= 1'b1;
                      RLAST  <= (ARLEN=='h0) ? 1'b1 : 1'b0;
                      beatR  <=  'h1;
                      addrR  <= get_next_addr_rd(ARADDR[ADDR_LENGTH-1:0]
                                                ,ARSIZE,ARBURST,ARLEN);
                      delayR <= P_DELAY_READ_BURST;
                      stateR <= (ARLEN=='h0) ? STR_END : STR_READ;
                  end else begin
                      beatR  <=  'h0;
                      delayR <= P_DELAY_READ_SETUP-1;
                      RVALID <= 1'b0;
                      stateR <= STR_DELAY;
                  end
             end else begin
                 ARREADY <= 1'b1;
             end
             end // STR_READY
         STR_DELAY: begin
             delayR <= delayR - 1;
             if (delayR==0) begin
                 RDATA  <= mem[addrR[ADDR_LENGTH-1:AXI_WIDTH_DSB]];
                 RVALID <= 1'b1;
                 beatR  <= beatR + 1;
                 addrR  <= get_next_addr_rd(addrR[ADDR_LENGTH-1:0]
                                           ,ARSIZE_reg,ARBURST_reg,ARLEN_reg);
                 if (beatR==ARLEN_reg) begin
                     RLAST  <= 1'b1;
                     stateR <= STR_END;
                 end else begin
                     stateR <= STR_READ;
                 end
             end
             end // STR_DELAY
         STR_READ: begin // address only
             if (RREADY==1'b1) begin
                  if (P_DELAY_READ_BURST==0) begin
                     RDATA  <= mem[addrR[ADDR_LENGTH-1:AXI_WIDTH_DSB]];
                     RVALID <= 1'b1;
                     beatR  <= beatR + 1;
                     addrR  <= get_next_addr_rd(addrR[ADDR_LENGTH-1:0]
                                               ,ARSIZE_reg,ARBURST_reg,ARLEN_reg);
                     if (beatR==ARLEN_reg) begin
                         RLAST  <= 1'b1;
                         stateR <= STR_END;
                     end
                  end else begin
                      RVALID <= 1'b0;
                      delayR <= P_DELAY_READ_BURST-1;
                      stateR <= STR_DELAY;
                  end
             end
             end // STR_READ
         STR_END: begin // data only
             if (RREADY==1'b1) begin
                 RDATA   <=  'h0;
                 RRESP   <= 2'b10; // SLVERR
                 RLAST   <= 1'b0;
                 RVALID  <= 1'b0;
                 ARREADY <= 1'b1;
                 delayR  <=  'h0;
                 beatR   <=  'h0;
                 stateR  <= STR_READY;
             end
             end // STR_END
         endcase
     end // if
     end // always
     //-------------------------------------------------------------------------
     // synthesis translate_off
     reg  [8*10-1:0] stateR_ascii="READY";
     always @ (stateR) begin
     case (stateR)
     STR_READY: stateR_ascii="READY  ";
     STR_DELAY: stateR_ascii="DELAY  ";
     STR_READ : stateR_ascii="READ   ";
     STR_END  : stateR_ascii="END    ";
     default  : stateR_ascii="UNKNOWN";
     endcase
     end
     // synthesis translate_on
     //-------------------------------------------------------------------------
     function [ADDR_LENGTH-1:0] get_next_addr_wr;
     input [ADDR_LENGTH-1:0] addr ;
     input [ 2:0]            size ;
     input [ 1:0]            burst; // burst type
     input [ 7:0]            len  ; // burst length
     reg   [ADDR_LENGTH-AXI_WIDTH_DSB-1:0] naddr;
     reg   [ADDR_LENGTH-1:0] mask ;
     begin
          case (burst)
          2'b00: get_next_addr_wr = addr;
          2'b01: begin
                 if ((1<<size)<AXI_WIDTH_DS) begin
                    get_next_addr_wr = addr + (1<<size);
                 end else begin
                     naddr = addr[ADDR_LENGTH-1:AXI_WIDTH_DSB];
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
     //-------------------------------------------------------------------------
     function [ADDR_LENGTH-1:0] get_next_addr_rd;
     input [ADDR_LENGTH-1:0] addr ;
     input [ 2:0]            size ;
     input [ 1:0]            burst; // burst type
     input [ 7:0]            len  ; // burst length
     reg   [ADDR_LENGTH-AXI_WIDTH_DSB-1:0] naddr;
     reg   [ADDR_LENGTH-1:0] mask ;
     begin
          case (burst)
          2'b00: get_next_addr_rd = addr;
          2'b01: begin
                 if ((1<<size)<AXI_WIDTH_DS) begin
                    get_next_addr_rd = addr + (1<<size);
                 end else begin
                     naddr = addr[ADDR_LENGTH-1:AXI_WIDTH_DSB];
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
     //-------------------------------------------------------------------------
     `ifdef BURST_TYPE_WRAPP_ENABLED
     function [ADDR_LENGTH-1:0] get_wrap_mask;
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
     //-------------------------------------------------------------------------
     // synthesis translate_off
     function [AXI_WIDTH_DA-1:0] read_word;
         input  [AXI_WIDTH_AD-1:0] addr;
     begin
         read_word = mem[addr[ADDR_LENGTH-1:AXI_WIDTH_DSB]];
     end
     endfunction
     function write_word;
         input  [AXI_WIDTH_AD-1:0] addr;
         input  [AXI_WIDTH_DA-1:0] data; // justified data
     begin
         mem[addr[ADDR_LENGTH-1:AXI_WIDTH_DSB]] = data;
     end
     endfunction
     // synthesis translate_on
endmodule
//------------------------------------------------------------------------------
// Revision History
//
// 2021.07.01: read_word()/write_word() added.
// 2018.05.01: Started by Ando Ki (adki@future-ds.com)
//------------------------------------------------------------------------------
