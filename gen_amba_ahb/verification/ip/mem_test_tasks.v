`ifndef MEM_TEST_TASKS_V
`define MEM_TEST_TASKS_V
//--------------------------------------------------------
// Copyright (c) 2018 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//--------------------------------------------------------
// mem_test_tasks.v
//--------------------------------------------------------
// VERSION = 2018.07.19.
//--------------------------------------------------------
   //-----------------------------------------------------
   // Memory test using a single accesse through AMBA AHB
   task memory_test;
        input [31:0] start;  // start address
        input [31:0] finish; // end address
        input [ 2:0] size;   // data size: 1, 2, 4
	//------------------
        reg [1:0] status;
        integer i, error, seed;
        reg [31:0] mask, dataW, dataR, expectX;
   begin
            error = 0;
            seed = 7;
            mask = get_mask(size);
            for (i=start; i<(finish-size+1); i=i+size) begin
                dataW = {$random(seed)}&mask; // seed will be updated
                ahb_write(i, dataW, size, status); // dataW is justified
                ahb_read(i, dataR, size, status); // dataR is justified
                dataR = dataR&mask;
                if (dataW!==dataR) begin
                   $display("[%04d] %m A:%x D:%x, but %x expectXed", $time, i, dataR, dataW);
                   error = error+1;
                end
            end
            if (error==0)
                   $display("[%04d] %m   RAW %x-%x %d-byte test OK", $time, start, finish, size);
            //-------------------------------------------------------------
            error = 0;
            seed = 1;
            mask = get_mask(size);
            for (i=start; i<(finish-size+1); i=i+size) begin
                dataW = {$random(seed)}&mask; // seed will be updated
                ahb_write(i, dataW, size, status); // dataW is justified
            end
            seed = 1;
            for (i=start; i<(finish-size+1); i=i+size) begin
                ahb_read(i, dataR, size, status); // dataR is justified
                dataR = dataR&mask;
                expectX = {$random(seed)}&mask; // seed will be updated
                if (dataR!==expectX) begin
                   $display("[%04d] %m A:%x D:%x, but %x expectXed", $time, i, dataR, expectX);
                   error = error+1;
                end
            end
            if (error==0)
                   $display("[%04d] %m RAAWA %x-%x %d-byte test OK", $time, start, finish, size);
   end
   endtask

   //-----------------------------------------------------
   // Memory test using a burst accesse through AMBA AHB
   // only for 4-byte granulity
   task memory_test_burst;
        input [31:0] start; // start address
        input [31:0] finish;// end address
        input [ 7:0] leng;  // burst length
        integer i, j, error, seed;
        reg [ 1:0] status;
        reg [31:0] expectX;
        reg [ 2:0] hburst;
   begin
          case (leng)
           4: hburst = 3'b011;
           8: hburst = 3'b101;
          16: hburst = 3'b111;
          endcase
          error = 0;
          if (finish>(start+leng*4)) begin
             seed  = 111;
             for (i=start; i<(finish-(leng*4)+1); i=i+leng*4) begin
                 for (j=0; j<leng; j=j+1) begin
                     data_burst_write[j] = $random(seed);
                 end
                 @ (posedge HCLK);
                 ahb_write_burst(i, hburst, status);
             end
             seed  = 111;
             for (i=start; i<(finish-(leng*4)+1); i=i+leng*4) begin
                 @ (posedge HCLK);
                 ahb_read_burst(i, hburst, status);
                 for (j=0; j<leng; j=j+1) begin
                     expectX = $random(seed);
                     if (data_burst_read[j] != expectX) begin
                        error = error+1;
                        $display("%m A=%hh D=%hh, but %hh expectXed",
                                i+j*leng, data_burst_read[j], expectX);
                     end
                 end
                 @ (posedge HCLK);
             end
             if (error==0)
                 $display("%m %d-length burst RAW OK: from %hh to %hh",
                           leng, start, finish);
          end else begin
              $display("%m %d-length burst read-after-write from %hh to %hh ???",
                           leng, start, finish);
          end
   end
   endtask
   //-----------------------------------------------------
   function [31:0] get_mask;
   input [2:0] size;
   begin
         case (size)
         3'd1: get_mask = 32'h0000_00FF;
         3'd2: get_mask = 32'h0000_FFFF;
         3'd4: get_mask = 32'hFFFF_FFFF;
         endcase
   end
   endfunction
//--------------------------------------------------------
// Revision history
//
// 2018.07.19: Started by Ando Ki (andoki@gmail.com)
//--------------------------------------------------------
`endif
