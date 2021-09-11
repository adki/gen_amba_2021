`ifndef AXI_MASTER_TASKS_V
`define AXI_MASTER_TASKS_V
//----------------------------------------------------------------
// Copyright (c) 2010-2020-2021 by Ando Ki.
// All right reserved.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//----------------------------------------------------------------
// axi_master_tasks.v
//----------------------------------------------------------------
// VERSION: 2021.07.10.
//----------------------------------------------------------------
localparam LD=1; // local delay

integer seed_tread=5;
integer seed_twrite=5;

integer num_of_reads ; initial num_of_reads =0;
integer num_of_writes; initial num_of_writes=0;

//----------------------------------------------------------------
task axi_statistics;
    input integer id;
begin
    $display("Master[%2d] reads=%5d writes=%5d", id, num_of_reads, num_of_writes);
end
endtask

//----------------------------------------------------------------
integer seed_arid=99;
//----------------------------------------------------------------
// It reads bleng*size bytes from addr and stores the data in to dataR[]
// read(addr+bnum*0, bnum);
// read(addr+bnum*1, bnum);
// read(addr+bnum*2, bnum);
//  ... ...
// read(addr+size*(blen-1), size);
task axi_master_read;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
     input                delay; // 0:don't use delay
     reg   [WIDTH_ID-1:0] arid;
begin
     if (bnum!=1<<$clog2(bnum)) $display($time,,"%m ERROR size in bytes not supported %d", bnum);
     arid <= $random(seed_arid);
     @ (posedge ACLK); //-------------------------------------------
     fork axi_master_read_ar( arid
                            , addr 
                            , bnum 
                            , bleng
                            , burst
                            , lock);
          axi_master_read_r( arid
                           , bleng
                           , delay);
     join
     num_of_reads = num_of_reads + 1;
end
endtask

task axi_master_read_ar;
     input [WIDTH_ID-1:0] arid ;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
begin
    ARID     <= #LD arid;
    ARADDR   <= #LD addr;
    ARLEN    <= #LD (bleng - 1); // 0, 1, ...
    `ifdef AMBA_AXI4
    ARLOCK   <= #LD lock[0];
    `else
    ARLOCK   <= #LD lock;
    `endif
    ARSIZE   <= #LD get_size(bnum);
    ARBURST  <= #LD burst;
    ARVALID  <= #LD 1'b1;
    @ (posedge ACLK); //-----------------------------------------
    while (ARREADY==1'b0) @ (posedge ACLK);
    `ifndef LOW_POWER
    ARID     <= #LD 0;
    ARADDR   <= #LD ~0;
    ARLEN    <= #LD 0;
    ARLOCK   <= #LD 0;
    ARSIZE   <= #LD 0;
    ARBURST  <= #LD 0;
    `endif
    ARVALID  <= #LD 1'b0;
end
endtask

task axi_master_read_r;
     input [WIDTH_ID-1:0] arid ;
     input [15:0]         bleng; // burst length: 1, 2, ...
     input                delay; // 0:don't use delay
     integer idx;
     integer rdelay;
begin
    RREADY   <= #LD 1; 
    for (idx=0; idx<bleng; idx=idx+1) begin
         @ (posedge ACLK); //-----------------------------------
         while (RVALID==1'b0) @ (posedge ACLK);
         dataR[idx] = RDATA; // it simply store the read-data; should be blocking
         if (RRESP!=2'b00) begin
             $display($time,,"%m ERROR RD RRESP no-ok 0x%02x", RRESP);
         end
         if (RID!=arid) begin
             $display($time,,"%m ERROR RD RID mis-match 0x%4x:0x%04x", RID, arid);
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
                rdelay = {$random(seed_tread)}%5;
                if (rdelay>0) begin
                    RREADY <= #LD 0;
                    repeat (rdelay) @ (posedge ACLK); //------------
                    RREADY <= #LD 1;
                end
             end
         end
    end
    RREADY   <= #LD 0; 
end
endtask

task axi_master_read_multiple_outstanding;
     input integer        arnum; // num of multiple outstanding
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
     input                delay; // 0:don't use delay
     reg   [WIDTH_AD-1:0] reg_addr[0:15];
     reg   [WIDTH_ID-1:0] reg_arid[0:15];
     integer idx, idy;
begin
     if (bnum!=1<<$clog2(bnum)) $display($time,,"%m ERROR size in bytes not supported %d", bnum);
     for (idx=0; idx<arnum; idx=idx+1) begin
          reg_arid[idx] = $random(seed_arid);
          reg_addr[idx] = addr + bleng*bnum*idx;
     end // for
     @ (posedge ACLK); //-------------------------------------------
     fork begin
          for (idx=0; idx<arnum; idx=idx+1) begin
              axi_master_read_ar( reg_arid[idx]
                                , reg_addr[idx]
                                , bnum 
                                , bleng
                                , burst
                                , lock);
          end // for
          end
          begin
          for (idy=0; idy<arnum; idy=idy+1) begin
              axi_master_read_r( reg_arid[idy]
                               , bleng
                               , delay);
          end // for
          end
     join
     num_of_reads = num_of_reads + arnum;
end
endtask
//----------------------------------------------------------------
integer seed_awid=9;
//----------------------------------------------------------------
// It write bleng*bnum bytes to addr.
// write(addr+bnum*0, bnum);
// write(addr+bnum*1, bnum);
// write(addr+bnum*2, bnum);
//  ... ...
// write(addr+bnum*(blen-1), bnum);
task axi_master_write;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
     input                delay; // 0:don't use delay
     reg   [WIDTH_ID-1:0] awid;
begin
     if (bnum!=1<<$clog2(bnum)) $display($time,,"%m ERROR size in bytes not supported %d", bnum);
     awid <= $random(seed_awid);
     @ (posedge ACLK); //-------------------------------------------
     fork axi_master_write_aw( awid
                             , addr
                             , bnum
                             , bleng
                             , burst
                             , lock);
          axi_master_write_w( awid
                            , addr
                            , bnum
                            , bleng
                            , burst
                            , delay);
          axi_master_write_b( awid);
     join
     num_of_writes = num_of_writes + 1;
end
endtask

task axi_master_write_aw;
     input [WIDTH_ID-1:0] awid ;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
begin
    AWID     <= #LD awid;
    AWADDR   <= #LD addr;
    AWLEN    <= #LD bleng - 1;
    `ifdef AMBA_AXI4
    AWLOCK   <= #LD lock[0];
    `else
    AWLOCK   <= #LD lock;
    `endif
    AWSIZE   <= #LD get_size(bnum);
    AWBURST  <= #LD burst;
    AWVALID  <= #LD 1'b1;
    @ (posedge ACLK); //-----------------------------------------
    while (AWREADY==1'b0) @ (posedge ACLK);
    `ifndef LOW_POWER
    AWID     <= #LD 0;
    AWADDR   <= #LD ~0;
    AWLEN    <= #LD 0;
    AWLOCK   <= #LD 0;
    AWSIZE   <= #LD 0;
    AWBURST  <= #LD 0;
    `endif
    AWVALID  <= #LD 1'b0;
end
endtask

task axi_master_write_w;
     input [WIDTH_ID-1:0] awid ;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input                delay; // 0:don't use delay
     reg   [WIDTH_AD-1:0] addr_reg;
     integer idx;
     integer wdelay;
begin
    addr_reg = addr;
    for (idx=0; idx<bleng; idx=idx+1) begin
        `ifndef AMBA_AXI4
        WID      <= #LD awid;
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
           wdelay = {$random(seed_twrite)}%5;
           if (wdelay>0) begin
               `ifndef LOW_POWER
               `ifndef AMBA_AXI4
               WID    <= #LD 0;
               `endif
               WDATA  <= #LD ~0;
               WLAST  <= #LD 1'b0;
               `endif
               WVALID <= #LD 1'b0;
               repeat (wdelay) @ (posedge ACLK);
           end
        end
    end
    `ifndef LOW_POWER
    `ifndef AMBA_AXI4
    WID      <= #LD 0;
    `endif
    WDATA    <= #LD ~0;
    WLAST    <= #LD 1'b0;
    `endif
    WVALID   <= #LD 1'b0;
end
endtask

task axi_master_write_b;
     input [WIDTH_ID-1:0] awid ;
begin
    BREADY   <= #LD 1'b1;
    @ (posedge ACLK); //----------------------------------------
    while (BVALID==1'b0) @ (posedge ACLK);
    BREADY   <= #LD 0;
    if (BRESP!=2'b00) begin
        $display($time,,"%m ERROR WR BRESP no-ok 0x%02x", BRESP);
    end
    if (BID!=awid) begin
        $display($time,,"%m ERROR WR BID mis-match 0x%4x:0x%04x", BID, awid);
    end
end
endtask

task axi_master_write_multiple_outstanding;
     input integer        awnum; // num of multiple outstanding
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input [15:0]         bleng; // burst length: 1, 2, ...
     input [ 1:0]         burst; // 0:fixed, 1:incr, 2:wrap
     input [ 1:0]         lock ; // 2'b10:lock, 2'b01:excl
     input                delay; // 0:don't use delay
     reg   [WIDTH_AD-1:0] reg_addr[0:15];
     reg   [WIDTH_ID-1:0] reg_awid[0:15];
     integer idx, idy, idz;
begin
     if (bnum!=1<<$clog2(bnum)) $display($time,,"%m ERROR size in bytes not supported %d", bnum);
     for (idx=0; idx<awnum; idx=idx+1) begin
          reg_awid[idx] = $random(seed_awid);
          reg_addr[idx] = addr + bleng*bnum*idx;
     end // for
     @ (posedge ACLK); //-------------------------------------------
     fork begin
          for (idx=0; idx<awnum; idx=idx+1) begin
              axi_master_write_aw( reg_awid[idx]
                                 , reg_addr[idx]
                                 , bnum 
                                 , bleng
                                 , burst
                                 , lock);
          end // for
          end
          begin
          for (idy=0; idy<awnum; idy=idy+1) begin
              axi_master_write_w( reg_awid[idy]
                                , reg_addr[idx]
                                , bnum
                                , bleng
                                , burst
                                , delay);
          end // for
          end
          begin
          for (idz=0; idz<awnum; idz=idz+1) begin
              axi_master_write_b( reg_awid[idz] );
          end // for
          end
     join
     num_of_writes = num_of_writes + awnum;
end
endtask
//----------------------------------------------------------------
// It performs a Read-Modify-Write.
// The first read should be locked, but the following write should
// not be locked in order to release the locked-state.
task axi_master_rmw;
     input [WIDTH_AD-1:0] addr ;
     input [15:0]         bnum ; // num of byte for a beat of burst
     input                delay; // 0:don't use delay
begin
     axi_master_read (addr, bnum, 1, 1, 2'b10, delay);
     axi_master_write(addr, bnum, 1, 1, 2'b00, delay);// should not be locked
end
endtask
//----------------------------------------------------------------
function [2:0] get_size;
    input [15:0] bnum ; // num of byte for a beat of burst
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
    default: begin $display($time,,"%m ERROR un-supported size: %d", bnum);
             end
    endcase
end
endfunction
//----------------------------------------------------------------
function [WIDTH_DS-1:0] get_strb;
   input [WIDTH_AD-1:0] addr ;
   input [15:0]         bnum ; // num of byte for a beat of burst
   reg   [WIDTH_DS-1:0] offset;
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
     input [15:0]         bnum ; // num of byte for a beat of burst
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
     input [15:0]      bnum ; // num of byte for a beat of burst
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
// 2021.07.10: axi_master_read_multiple_outstanding/axi_master_write_multiple_outstanding added.
// 2020.01.08: checking validity of axsize
//             if (bnum!=1<<clogb2(bnum)) $display($time,,"%m ERROR size in bytes not supported %d", bnum);
// 2011.01.10: Started by Ando Ki (adki@future-ds.com)
//----------------------------------------------------------------
`endif
