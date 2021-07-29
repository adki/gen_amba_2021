`ifndef AXI_MASTER_TASKS_V
`define AXI_MASTER_TASKS_V
//----------------------------------------------------------------
//  Copyright (c) 2010 by Ando Ki.
//  All right reserved.
//  http://www.future-ds.com
//  All rights are reserved by Ando Ki.
//  Do not use in any means or/and methods without Ando Ki's permission.
//----------------------------------------------------------------
// axi_master_tasks.v
//----------------------------------------------------------------
// VERSION: 2011.01.11.
//----------------------------------------------------------------

localparam LD=1; // local delay

//----------------------------------------------------------------
// It reads bleng*size bytes from addr and stores the data in to dataR[]
// read(addr+bnum*0, bnum);
// read(addr+bnum*1, bnum);
// read(addr+bnum*2, bnum);
//  ... ...
// read(addr+size*(blen-1), size);
task axi_master_read;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
     input                delay; // 0:don't use delay
     reg   [WIDTH_ID-1:0] ARID_reg;
     integer idx;
     integer rdelay;
begin
     ARID_reg <= 'h0;
     @ (posedge ACLK); //-------------------------------------------
     fork
     begin // AR-phase
       ARID_reg <=   (ARID + 1);
       ARID     <= #LD ARID + 1;
       ARADDR   <= #LD addr;
       ARLEN    <= #LD (bleng - 1); // 0, 1, ...
       ARLOCK   <= #LD lock;
       ARSIZE   <= #LD get_size(bnum);
       ARBURST  <= #LD burst;
       ARVALID  <= #LD 1'b1;
       @ (posedge ACLK); //-----------------------------------------
       while (ARREADY==1'b0) @ (posedge ACLK);
       //ARID     <= #LD 0;
       ARADDR   <= #LD ~0;
       ARLEN    <= #LD 0;
       ARLOCK   <= #LD 0;
       ARSIZE   <= #LD 0;
       ARBURST  <= #LD 0;
       ARVALID  <= #LD 1'b0;
     end // AR-phase
     //-------------------------------------------------------------
     begin // R-phase
       RREADY   <= #LD 1; 
       for (idx=0; idx<bleng; idx=idx+1) begin
            @ (posedge ACLK); //-----------------------------------
            while (RVALID==1'b0) @ (posedge ACLK);
            dataR[idx] = RDATA; // it simply store the read-data; should be blocking
            if (RRESP!=2'b00) begin
                $display($time,,"%m ERROR RD RRESP no-ok 0x%02x", RRESP);
            end
            if (RID!=ARID_reg) begin
                $display($time,,"%m ERROR RD RID mis-match 0x%4x:0x%04x", RID, ARID_reg);
            end
            if (idx==(bleng-1)) begin
                if (RLAST==1'b0) begin
                    $display($time,,"%m ERROR RD RLAST not driven");
                end
            end else begin
                if (RLAST==1'b1) begin
                    $display($time,,"%m ERROR RD RLAST not expected");
                end
                if (delay) begin
                   rdelay = {$random}%5;
                   if (rdelay>0) begin
                       RREADY <= #LD 0;
                       repeat (rdelay) @ (posedge ACLK); //------------
                       RREADY <= #LD 1;
                   end
                end
            end
       end
       RREADY   <= #LD 0; 
     end // R-phase
     join
end
endtask
//----------------------------------------------------------------
// It write bleng*bnum bytes to addr.
// write(addr+bnum*0, bnum);
// write(addr+bnum*1, bnum);
// write(addr+bnum*2, bnum);
//  ... ...
// write(addr+bnum*(blen-1), bnum);
task axi_master_write;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
     input                delay; // 0:don't use delay
     reg   [WIDTH_ID-1:0] AWID_reg;
     reg   [WIDTH_AD-1:0] addr_reg;
     integer idx;
     integer wdelay;
begin
     AWID_reg <= AWID + 1;
     addr_reg <= addr;
     @ (posedge ACLK); //-------------------------------------------
     fork
     begin // AW-phase
       AWID     <= #LD AWID_reg;
       AWADDR   <= #LD addr;
       AWLEN    <= #LD bleng - 1;
       AWLOCK   <= #LD lock;
       AWSIZE   <= #LD get_size(bnum);
       AWBURST  <= #LD burst;
       AWVALID  <= #LD 1'b1;
       @ (posedge ACLK); //-----------------------------------------
       while (AWREADY==1'b0) @ (posedge ACLK);
       //AWID     <= #LD 0;
       AWADDR   <= #LD ~0;
       AWLEN    <= #LD 0;
       AWLOCK   <= #LD 0;
       AWSIZE   <= #LD 0;
       AWBURST  <= #LD 0;
       AWVALID  <= #LD 1'b0;
     end // AW-phase
     begin // W&B-phase
       for (idx=0; idx<bleng; idx=idx+1) begin
            `ifndef AMBA_AXI4
            WID      <= #LD AWID_reg;
            `endif
            WDATA    <= #LD dataW[idx];
            WSTRB    <= #LD get_strb(addr_reg,bnum);
            if (idx==(bleng-1)) WLAST    <= #LD 1;
            else                WLAST    <= #LD 0;
            WVALID   <= #LD 1'b1;
            @ (posedge ACLK); //------------------------------------
            while (WREADY==1'b0) @ (posedge ACLK);
            addr_reg = get_next_addr(addr_reg,bnum,bleng,burst);
            if (delay) begin
               wdelay = {$random}%5;
               if (wdelay>0) begin
                   WVALID <= #LD 1'b0;
                   //WID    <= #LD 0;
                   WLAST  <= #LD 1'b0;
                   WDATA  <= #LD ~0;
                   repeat (wdelay) @ (posedge ACLK);
               end
            end
        end
        WVALID   <= #LD 1'b0;
        //WID      <= #LD 0;
        WDATA    <= #LD ~0;
        WLAST    <= #LD 1'b0;
        BREADY   <= #LD 1'b1;
        @ (posedge ACLK); //----------------------------------------
        while (BVALID==1'b0) @ (posedge ACLK);
        BREADY   <= #LD 0;
        if (BRESP!=2'b00) begin
            $display($time,,"%m ERROR WR BRESP no-ok 0x%02x", BRESP);
        end
        if (BID!=AWID_reg) begin
            $display($time,,"%m ERROR WR BID mis-match 0x%4x:0x%04x", BID, AWID_reg);
        end
     end // W&B-phase
     join
end
endtask
//----------------------------------------------------------------
// It performs a Read-Modify-Write.
// The first read should be locked, but the following write should
// not be locked in order to release the locked-state.
task axi_master_rmw;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte
     input                delay; // 0:don't use delay
begin
     axi_master_read (addr, bnum, 1, 1, 2'b10, delay);
     axi_master_write(addr, bnum, 1, 1, 2'b00, delay);// should not be locked
end
endtask
//----------------------------------------------------------------
function [2:0] get_size;
    input [15:0] bnum; // num of byte
begin
    case (bnum)
    1:    get_size = 3'h0;
    2:    get_size = 3'h1;
    4:    get_size = 3'h2;
    8:    get_size = 3'h3;
    16:   get_size = 3'h4;
    32:   get_size = 3'h5;
    64:   get_size = 3'h6;
    128:  get_size = 3'h7;
    default: $display($time,,"%m ERROR un-supported size: %d", bnum);
    endcase
end
endfunction
//----------------------------------------------------------------
function [WIDTH_DS-1:0] get_strb;
   input [WIDTH_AD-1:0] addr ;
   input [15:0]         bnum ; // num of bytes 
   integer              offset;
begin
   offset = addr[WIDTH_DSB-1:0]; //offset = addr%WIDTH_DS;
   case (bnum)
     1:   get_strb = {  1{1'b1}}<<offset;
     2:   get_strb = {  2{1'b1}}<<offset;
     4:   get_strb = {  4{1'b1}}<<offset;
     8:   get_strb = {  8{1'b1}}<<offset;
    16:   get_strb = { 16{1'b1}}<<offset;
    32:   get_strb = { 32{1'b1}}<<offset;
    64:   get_strb = { 64{1'b1}}<<offset;
   128:   get_strb = {128{1'b1}}<<offset;
   default: $display($time,,"%m ERROR un-supported size: %d", bnum);
   endcase
end
endfunction
//-----------------------------------------------------------
function [WIDTH_AD-1:0] get_next_addr;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // byte num
     input [15:0]         bleng; // burst length
     input [ 1:0]         burst; // burst type
     reg   [WIDTH_AD-1:0] mask ;
begin
     case (burst)
     2'b00: get_next_addr = addr;
     2'b01:
         if (bnum<WIDTH_DS) begin
             get_next_addr = addr + bnum;
         end else begin
             get_next_addr[WIDTH_DSB-1:0] = 0;
             get_next_addr[WIDTH_AD-1:WIDTH_DSB] = addr[WIDTH_AD-1:WIDTH_DSB]+1;
         end
     2'b10: begin
            mask          = get_wrap_mask(bnum,bleng);
            get_next_addr = (addr&~mask)
                          | (((addr&mask)+(1<<(bnum-1)))&mask);
            end
     2'b11: begin
            get_next_addr = addr;
            // synopsys translate_off
            $display($time,,"%m ERROR un-defined BURST %01x", burst);
            // synopsys translate_on
            end
     endcase
end
endfunction
//-----------------------------------------------------------
function [WIDTH_AD-1:0] get_wrap_mask;
     input [15:0]      bnum ;
     input [15:0]      bleng; // burst length
begin
     case (bnum)
       1: get_wrap_mask = (    bleng)-1;
       2: get_wrap_mask = (  2*bleng)-1;
       4: get_wrap_mask = (  4*bleng)-1;
       8: get_wrap_mask = (  8*bleng)-1;
      16: get_wrap_mask = ( 16*bleng)-1;
      32: get_wrap_mask = ( 32*bleng)-1;
      64: get_wrap_mask = ( 64*bleng)-1;
     128: get_wrap_mask = (128*bleng)-1;
     endcase
end
endfunction
//----------------------------------------------------------------
// Revision History
//
// 2011.01.10: Started by Ando Ki (adki@future-ds.com)
//----------------------------------------------------------------
`endif
