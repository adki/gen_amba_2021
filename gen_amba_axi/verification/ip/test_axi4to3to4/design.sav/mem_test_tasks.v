`ifndef MEM_TEST_TASKS_V
`define MEM_TEST_TASKS_V
//----------------------------------------------------------------
// Copyright (c) 2018-2021 by Ando Ki.
// All right reserved.
// This is licensed with the 2-clause BSD license to make the program and
// library useful in open and closed source products independent of their
// licensing scheme.
//----------------------------------------------------------------
// axi_tester.v
//----------------------------------------------------------------
// VERSION: 2021.09.05.
//----------------------------------------------------------------
reg error_flag=0;
always @ ( * ) begin
   if (error_flag==1) begin
        repeat (50) @ (posedge ACLK);
        $finish(2);
   end
end

integer seed_mread=9;
integer seed_mwrite=11;
//-----------------------------------------------------------
task test_single;
     input [WIDTH_AD-1:0] addr;
     input [15:0]         bnum; // num of bytes
     input                delay;
begin
     dataW[0] = {(2*WIDTH_DS){4'b01}}+1;
     axi_master_write(addr, bnum, 1, 1, 2'b00, delay);
     axi_master_read (addr, bnum, 1, 1, 2'b00, delay);
     axi_master_rmw  (addr, bnum,              delay);
end
endtask
//-----------------------------------------------------------
task test_burst;
     input [WIDTH_AD-1:0] addr;
     input [15:0]         bnum; // num of bytes for a beat
     input [15:0]         blen; // burst length: 1 ~ 16
     input [ 1:0]         burst; // type
     input                delay;
     input integer        random;
     integer offset, ind, seed, w, error;
     reg [WIDTH_AD-1:0] loc;
     reg [WIDTH_DA-1:0] patt, mask;
begin
     loc = addr;
     seed = random;
     for (ind=0; ind<blen; ind=ind+1) begin
         dataW[ind] = 'hX; // make unknown
         offset = loc[WIDTH_DSB-1:0];
         if (random==0) begin
             dataW[ind] = {(2*WIDTH_DS){4'b10}} + ind;
         end else begin
             for (w=0; w<bnum; w=w+1) begin
                  dataW[ind][8*w+:8] = $random(seed)&'hFF;
             end
         end
         if (offset>0) dataW[ind] = dataW[ind]<<(8*offset);
         loc = loc + bnum;
     end
     axi_master_write(addr, bnum, blen, burst, 2'b00, delay);
     axi_master_read (addr, bnum, blen, burst, 2'b00, delay);
     patt = (1<<(8*bnum))-1;
     loc  = addr;
     error = 0;
     for (ind=0; ind<blen; ind=ind+1) begin
         offset = loc[WIDTH_DSB-1:0];
         mask = patt<<(8*offset);
         if ((dataW[ind]&mask)!==(dataR[ind]&mask)) begin
$display("%0t %m mismatch A:0x%x D:0x%x, but 0x%x expected %0d-byte:%0d-leng",
          $time, addr, dataR[ind]&mask, dataW[ind]&mask, bnum, blen);
             error = error + 1;
         end
         loc = loc + bnum;
     end
     if (error==0) $display($time,,"%m test_burst %0d-byte:%0d-leng OK", bnum, blen);
     else          $display($time,,"%m test_burst %0d-byte:%0d-leng error %0d", bnum, blen, error);
end
endtask
//-----------------------------------------------------------
     task test_error;
          input [WIDTH_AD-1:0] addr;
          input                delay;
          integer ind;
     begin
          for (ind=1; ind<=WIDTH_DS; ind=ind*2) begin
              axi_master_write(addr, ind, 1, 1, 2'b00, delay);
              axi_master_read (addr, ind, 1, 1, 2'b00, delay);
              axi_master_rmw  (addr, ind,              delay);
          end
          for (ind=1; ind<=16; ind=ind+1) begin
              axi_master_write(addr, 4, ind, 2'b01, 2'b00, delay);
              axi_master_read (addr, 4, ind, 2'b01, 2'b00, delay);
          end
     end
     endtask
     //-----------------------------------------------------------
     task mem_test;
          input [WIDTH_AD-1:0] startA;
          input [WIDTH_AD-1:0] endA;
          input [15:0]         bnum;
          input                delay; // 1 for delay added
          reg   [WIDTH_DA-1:0] addr;
          reg   [WIDTH_DA-1:0] data;
          reg   [WIDTH_DA-1:0] store[0:1023];
          integer idx, error;
     begin
          if ((endA-startA)>1023) begin
             $display($time,,"%m out-of-range 0x%x: should be smaller than 1024",
                              (endA-startA));
          end
          idx = 0;
          for (addr=startA; (addr+bnum-1)<=endA; addr=addr+bnum) begin
               dataW[0]   = get_data(0)&get_mask(addr, bnum);
               store[idx] = dataW[0];
               axi_master_write(addr, bnum, 1, 1, 2'b00, delay);
               idx = idx + 1;
          end
          idx = 0;
          error = 0;
          for (addr=startA; (addr+bnum-1)<=endA; addr=addr+bnum) begin
               axi_master_read(addr, bnum, 1, 1, 2'b00, delay);
               data = dataR[0]&get_mask(addr, bnum);
               if (data!==store[idx]) begin
                   error = error + 1;
                   $display($time,,"%m mismatch A:0x%x D:0x%x, but 0x%x expected",
                                    addr, data, store[idx]);
error_flag=1;
               end
               idx = idx + 1;
          end
          if (error==0)
          $display($time,,"%m mem_test OK for %02d-byte from 0x%x to 0x%x",
                           bnum, startA, endA);
     end
     endtask
     //-----------------------------------------------------------
     task mem_test_burst;
          input [WIDTH_AD-1:0] startA;
          input [WIDTH_AD-1:0] endA;
          input [15:0]         bnum; // byte num:  1,2,4,8,16
          input [15:0]         blen; // burst len: 1-16
          input                delay;
          reg   [WIDTH_DA-1:0] addr;
          reg   [WIDTH_DA-1:0] data;
          reg   [WIDTH_DA-1:0] store[0:1023];
          integer idy;
          integer error, a, b;
     begin
          if ((endA-startA)>1023) begin
             $display($time,,"%m out-of-range 0x%x: should be smaller than 1024",
                              (endA-startA));
          end
          idy = 0;
          for (addr=startA; (addr+bnum*blen-1)<=endA; addr=addr+bnum*blen) begin
               a = addr;
               for (b=0; b<blen; b=b+1) begin
if (0) begin
                   dataW[b]     = get_data(0)&get_mask(a, bnum);
end else begin
                   dataW[b]     = addr&get_mask(a, bnum);
end
                   store[idy+b] = dataW[b];
                   a = get_next_addrX(a, bnum);
               end
               axi_master_write(addr, bnum, blen, 1, 2'b00, delay);
               idy = idy + blen;
          end
          error = 0;
          idy   = 0;
          for (addr=startA; (addr+bnum*blen-1)<=endA; addr=addr+bnum*blen) begin
               axi_master_read(addr, bnum, blen, 1, 2'b00, delay);
               a = addr;
               for (b=0; b<blen; b=b+1) begin
                   data = dataR[b]&get_mask(a, bnum);
                   if (data!==store[idy+b]) begin
                       error = error + 1;
                       $display($time,,"%m mismatch A:0x%x D:0x%x, but 0x%x expected",
                                        addr, data, store[idy]);
error_flag=1;
                   end
                   a = get_next_addrX(a, bnum);
               end
               idy = idy + blen;
          end
          if (error==0)
          $display($time,,"%m mem_test_burst OK for %02d-byte %02d-length from 0x%x to 0x%x",
                           bnum, blen, startA, endA);
     end
     endtask
     //-----------------------------------------------------------
     // It calculates mask for data.
     function [WIDTH_DA-1:0] get_mask;
        input [WIDTH_AD-1:0] addr;
        input [ 15:0]        bnum; // byte-number: 1,2,4,8,16
        reg   [127:0]        mask; // for the case of 16-byte (128-bit)
        reg   [  3:0]        offset;
     begin
          case (bnum)
           1: mask = 'hFF;
           2: mask = 'hFFFF;
           4: mask = 'hFFFF_FFFF;
           8: mask = 'hFFFF_FFFF_FFFF_FFFF;
          16: mask = 'hFFFF_FFFF_FFFF_FFFF_FFFF_FFFF_FFFF_FFFF;
          endcase
          offset = addr%WIDTH_DS;
          if ((offset%bnum)!=0) begin
              $display($time,,"%m WARN offset:bnum mismatch A:0x%x W:%02d BN:%02d OFFSET:%02d",
                               addr, WIDTH_DS, bnum, offset);
          end
          get_mask = mask<<(offset*8);
     end
     endfunction
     //-----------------------------------------------------------
     // It generates data according to WIDTH_DA.
     function [WIDTH_DA-1:0] get_data;
          input [31:0] dummy;
          integer xx;
     begin
          for (xx=0; xx<WIDTH_DA; xx=xx+32) begin
               get_data = (get_data<<32)|{$random(seed_mwrite)};
          end
     end
     endfunction
     //-----------------------------------------------------------
     function [WIDTH_AD-1:0] get_next_addrX;
           input [WIDTH_AD-1:0] addr;
           input [15:0]         bnum;
     begin
           if (bnum<WIDTH_DS) begin
               get_next_addrX = addr + bnum;
           end else begin 
               get_next_addrX[WIDTH_DSB-1:0] = 0;
               get_next_addrX[WIDTH_AD-1:WIDTH_DSB] = addr[WIDTH_AD-1:WIDTH_DSB] + 1;
           end
     end
     endfunction
     //-----------------------------------------------------------
//----------------------------------------------------------------
// Revision History
//
// 2021.09.05: "task test_burst" updated.
// 2021.07.10: "task test_burst" added 'random' argument.
// 2018.07.19: Started by Ando Ki (adki@future-ds.com)
//----------------------------------------------------------------
`endif
