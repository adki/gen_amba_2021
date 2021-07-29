`ifndef AHB_TASKS_V
`define AHB_TASKS_V
//------------------------------------------------------------------------------
// Copyright (c) 2018 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//------------------------------------------------------------------------------
// ahb_tasks.h
//------------------------------------------------------------------------------
// VERSION = 2018.07.19.
//------------------------------------------------------------------------------
// [NOTE]
// - Data passed through argument is in justified fashion.
//   So, the data should be re-positioned according to address and size.
//--------------------------------------------------------
reg [31:0] data_burst_read [0:1023]; // its index is not address, but beat
reg [31:0] data_burst_write[0:1023]; // its index is not address, but beat

//------------------------------------------------------------------------------
// 'ahb_write' generates AMBA AHB wirte transaction.
//
// [input]
//  addr: 32-bit address to be driven on HADDR[31:0].
//  data: 32-bit width data to be driven on HWDATA[31:0].
//        Partial bytes are in justified fashion.
//  size: Num of valid bytes in 'data' starting from addr%4.
//        It can be 1, 2, and 4.
// [note]
//------------------------------------------------------------------------------
task ahb_write;
     input  [31:0] addr; // byte-wise adddress
     input  [31:0] data; // justifed data
     input  [ 2:0] size; // num of bytes
     output [ 1:0] status; // 0 for OK
begin
       ahb_write_core( addr
                     , data
                     , size
                     , 4'b0001 // non-cacheable,non-bufferable,user,data
                     , 1'b0 // hlock
                     , status);
end
endtask

//------------------------------------------------------------------------------
task ahb_write_core;
     input  [31:0] addr; // byte-wise address
     input  [31:0] data; // justified data
     input  [ 2:0] size; // num of bytes
     input  [ 3:0] hprot;
     input         lock;
     output [ 1:0] status; // 0 for OK, 1 for error, 2 for retry, 3 for split
begin
    status = 0;
    @ (posedge HCLK);
    HBUSREQ <= 1;
    HLOCK   <= (lock) ? 1'b1 : HLOCK;
    @ (posedge HCLK);
    while ((HGRANT!==1'b1)||(HREADY!==1'b1)) @ (posedge HCLK);
    HBUSREQ <= 1'b0;
    HLOCK   <= (lock) ? 1'b1 : HLOCK;
    HADDR   <= addr;
    HPROT   <= hprot;   //`HPROT_DATA
    HTRANS  <= 2'b10;   //`HTRANS_NONSEQ;
    HBURST  <= 3'b000;  //`HBURST_SINGLE;
    HWRITE  <= 1'b1;    //`HWRITE_WRITE;
    case (size)
    1:  HSIZE <= 3'b000; //`HSIZE_BYTE;
    2:  HSIZE <= 3'b001; //`HSIZE_HWORD;
    4:  HSIZE <= 3'b010; //`HSIZE_WORD;
    default: begin
             status = 1;
             $display("%04d %m ERROR: unsupported transfer size: %d-byte", $time, size);
             end
    endcase
    @ (posedge HCLK);
    while (HREADY!==1) @ (posedge HCLK);
    `ifdef ENDIAN_BIG
    HWDATA <= data<<(24-(8*addr[1:0])); // for big-endian
    `else
    HWDATA <= data<<(8*addr[1:0]); // for little-endian
    `endif
    HTRANS <= 2'b0;
    @ (posedge HCLK);
    while (HREADY===0) @ (posedge HCLK);
    case (HRESP)
    2'b00: status = 0; // OK
    2'b01: status = 1; // ERROR
    2'b10: status = 2; // RETRY
    2'b11: status = 3; // SPLIT
    endcase
    if (HRESP!=2'b00) begin //if (HRESP!=`HRESP_OKAY)
         status = 1;
         $display("%04d %m ERROR: non OK response write", $time);
    end
end
endtask

//------------------------------------------------------------------------------
// 'ahb_read' generates AMBA AHB read transaction.
//
// [input]
//  addr: 32-bit address to be driven on HADDR[31:0].
//  size: Num of valid bytes in 'data' starting from addr%4.
//        It can be 1, 2, and 4.
// [output]
//  data: 32-bit data has been read.
//        Partial bytes are in justified fashion.
// [note]
//------------------------------------------------------------------------------
task ahb_read;
     input  [31:0] addr; // byte-wise address
     output [31:0] data; // justified data
     input  [ 2:0] size; // num of bytes
     output [ 1:0] status; // 0 for OK
begin
       ahb_read_core( addr
                    , data
                    , size
                    , 4'b0001 // non-cacheable,non-bufferable,user,data
                    , 1'b0 // lock
                    , status);
end
endtask

//------------------------------------------------------------------------------
task ahb_read_core;
     input  [31:0] addr; // byte-wise address
     output [31:0] data; // justifed data
     input  [ 2:0] size; // num of bytes
     input  [ 3:0] hprot;
     input         lock ;
     output [ 1:0] status;
begin
    status = 0;
    @ (posedge HCLK);
    HBUSREQ <= 1'b1;
    HLOCK   <= (lock) ? 1'b1 : HLOCK;
    @ (posedge HCLK);
    while ((HGRANT!==1'b1)||(HREADY!==1'b1)) @ (posedge HCLK);
    HBUSREQ <= 1'b0;
    HLOCK   <= (lock) ? 1'b1 : 1'b0;
    HADDR   <= addr;
    HPROT   <= hprot; //`HPROT_DATA
    HTRANS  <= 2'b10;  //`HTRANS_NONSEQ;
    HBURST  <= 3'b000; //`HBURST_SINGLE;
    HWRITE  <= 1'b0;   //`HWRITE_READ;
    case (size)
    1:  HSIZE <= 3'b000; //`HSIZE_BYTE;
    2:  HSIZE <= 3'b001; //`HSIZE_HWORD;
    4:  HSIZE <= 3'b010; //`HSIZE_WORD;
    default: begin
             status = 1;
             $display("%04d %m ERROR: unsupported transfer size: %d-byte", $time, size);
             end
    endcase
    @ (posedge HCLK);
    while (HREADY!==1'b1) @ (posedge HCLK);
    HTRANS <= 2'b0;
    @ (posedge HCLK);
    while (HREADY===0) @ (posedge HCLK);
    `ifdef ENDIAN_BIG
    data = HRDATA>>(24-(8*addr[1:0])); // must be blocking
    `else
    data = HRDATA>>(8*addr[1:0]); // must be blocking
    `endif
    case (HRESP)
    2'b00: status = 0; // OK
    2'b01: status = 1; // ERROR
    2'b10: status = 2; // RETRY
    2'b11: status = 3; // SPLIT
    endcase
    if (HRESP!=2'b00) begin //if (HRESP!=`HRESP_OKAY)
        status = 1;
        $display("%04d %m ERROR: non OK response for read", $time);
    end
end
endtask

//------------------------------------------------------------------------------
// Bursts must not cross a 1kB address boundary.
task ahb_read_burst;
     input  [31:0] addr; // byte-wise address
     input  [ 2:0] hburst; // type of burst
     output [ 1:0] status; // 0 for OK
begin
     ahb_read_burst_core( addr
                        , 4
                        , hburst
                        , 4'b0001 // hprot
                        , status);
end
endtask

//------------------------------------------------------------------------------
task ahb_read_burst_core;
     input  [31:0] addr;
     input  [ 2:0] size; // num of byte
     input  [ 2:0] hburst; // HBURST
     input  [ 3:0] hprot;// HPROT
     output [ 1:0] status; // 0 for OK
     reg    [31:0] data;
begin
     case (hburst)
     3'b000: begin ahb_read_core(addr,data,size,hprot,1'b0, status); data_burst_read[0] = data; end // SINGLE
     3'b001: ahb_read_burst_inc (addr,size, 1,hprot,status); // INCR
     3'b010: ahb_read_burst_wrap(addr,size, 4,hprot,status); // WRAP4
     3'b011: ahb_read_burst_inc (addr,size, 4,hprot,status); // INCR4
     3'b100: ahb_read_burst_wrap(addr,size, 8,hprot,status); // WRAP8
     3'b101: ahb_read_burst_inc (addr,size, 8,hprot,status); // INCR8
     3'b110: ahb_read_burst_wrap(addr,size,16,hprot,status); // WRAP16
     3'b111: ahb_read_burst_inc (addr,size,16,hprot,status); // INCR16
     endcase
end
endtask

//------------------------------------------------------------------------------
// 'ahb_read_burst_inc' performs a burst read transaction.
//  - each beat carries 'size' bytes in non-justified data in HRDATA[31:0]
//  - each beat stores 'size' bytes in justified data in 'data_burst_read[n]',
//    where 'n' represents the num of beats starting from 0.
task ahb_read_burst_inc;
     input  [31:0] addr;
     input  [ 2:0] size; // num of byte
     input  [31:0] leng; // num of beat
     input  [ 3:0] hprot;// HPROT
     output [ 1:0] status; // 0 for OK
     reg    [31:0] loc ; // keep current address
     reg    [31:0] beat; // keep beat num.
     integer       i, ln, k;
begin
    status = 0;
    k = 0;
    @ (posedge HCLK);
    HBUSREQ <= 1'b1;
    @ (posedge HCLK);
    while ((HGRANT!==1'b1)||(HREADY!==1'b1)) @ (posedge HCLK);
    HPROT  <= hprot;
    HADDR  <= addr ;
    HTRANS <= 2'b10; //`HTRANS_NONSEQ;
    if (leng==16)     begin HBURST <= 3'b111; ln=16; end //`HBURST_INCR16;
    else if (leng==8) begin HBURST <= 3'b101; ln= 8; end //`HBURST_INCR8;
    else if (leng==4) begin HBURST <= 3'b011; ln= 4; end //`HBURST_INCR4;
    else              begin HBURST <= 3'b001; ln=leng; end //`HBURST_INCR;
    HWRITE <= 1'b0; //`HWRITE_READ;
    case (size)
    1:  HSIZE <= 3'b000; //`HSIZE_BYTE;
    2:  HSIZE <= 3'b001; //`HSIZE_HWORD;
    4:  HSIZE <= 3'b010; //`HSIZE_WORD;
    default: begin
             status = 1;
             $display("%04d %m ERROR: unsupported transfer size: %d-byte", $time, size);
             end
    endcase
    loc  = addr;
    beat = 0;
    @ (posedge HCLK);
    while (HREADY==1'b0) @ (posedge HCLK);
    while (leng>0) begin
       for (i=0; i<ln-1; i=i+1) begin
           if (i>=(ln-3)) HBUSREQ <= 1'b0;
           HADDR  <= HADDR + size;
           HTRANS <= 2'b11; //`HTRANS_SEQ;
           @ (posedge HCLK);
           while (HREADY==1'b0) @ (posedge HCLK);
           `ifdef ENDIAN_BIG
           data_burst_read[beat] <= HRDATA>>(24-(8*loc[1:0])); // big-endian
           `else
           data_burst_read[beat] <= HRDATA>>(8*loc[1:0]); // little-endian
           `endif
           k = k+1;
           loc = loc + size;
           beat = beat + 1;
       end
       leng = leng - ln;
       if (leng==0) begin
          HTRANS  <= 0;
          HBUSREQ <= 1'b0;
       end else begin
          HADDR  <= HADDR + size;
          HTRANS <= 2'b10; //`HTRANS_NONSEQ;
          if (leng>=16)     begin HBURST <= 3'b111; ln=16; end //`HBURST_INCR16;
          else if (leng>=8) begin HBURST <= 3'b101; ln= 8; end //`HBURST_INCR8;
          else if (leng>=4) begin HBURST <= 3'b011; ln= 4; end //`HBURST_INCR4;
          else              begin HBURST <= 3'b001; ln=leng; end //`HBURST_INCR;
          @ (posedge HCLK);
          while (HREADY==0) @ (posedge HCLK);
          `ifdef ENDIAN_BIG
          data_burst_read[beat] = HRDATA>>(24-(8*loc[1:0]));
          `else
          data_burst_read[beat] = HRDATA>>(8*loc[1:0]);
          `endif
          if (HRESP!=2'b00) begin //if (HRESP!=`HRESP_OKAY)
              status = 1;
              $display("%04d %m ERROR: non OK response for read", $time);
          end
          k = k+1;
          loc = loc + size;
          beat = beat + 1;
       end
    end
    @ (posedge HCLK);
    while (HREADY==0) @ (posedge HCLK);
    data_burst_read[beat] = HRDATA>>(8*loc[1:0]); // must be blocking
end
endtask

//------------------------------------------------------------------------------
task ahb_read_burst_wrap;
     input  [31:0] addr; // starting address
     input  [ 2:0] size; // num of bytes for each beat
     input  [31:0] leng; // num of beats
     input  [ 3:0] hprot;// HPROT
     output [ 1:0] status; // 0 for OK
     reg    [31:0] loc ; // keep current address
     reg    [31:0] beat; // keep beat num.
     integer       i, ln, k;
begin
    status = 0;
    k = 0;
    @ (posedge HCLK);
    HBUSREQ <= 1'b1;
    @ (posedge HCLK);
    while ((HGRANT!==1'b1)||(HREADY!==1'b1)) @ (posedge HCLK);
    HPROT  <= hprot;
    HADDR  <= addr ;
    HTRANS <= 2'b10; //`HTRANS_NONSEQ;
    if (leng==16)     begin HBURST <= 3'b111; ln=16; end //`HBURST_INCR16;
    else if (leng==8) begin HBURST <= 3'b101; ln= 8; end //`HBURST_INCR8;
    else if (leng==4) begin HBURST <= 3'b011; ln= 4; end //`HBURST_INCR4;
    else              begin 
                      status = 1;
                      $display("%04d %m ERROR: unsupported wrap-burst: %d", $time, leng);
                      end
    HWRITE <= 1'b0; //`HWRITE_READ;
    case (size)
    1:  HSIZE <= 3'b000; //`HSIZE_BYTE;
    2:  HSIZE <= 3'b001; //`HSIZE_HWORD;
    4:  HSIZE <= 3'b010; //`HSIZE_WORD;
    default: begin
             status = 1;
             $display("%04d %m ERROR: unsupported transfer size: %d-byte", $time, size);
             end
    endcase
    loc  = addr;
    beat = 0;
    @ (posedge HCLK);
    while (HREADY==1'b0) @ (posedge HCLK);
    while (leng>0) begin
       for (i=0; i<ln-1; i=i+1) begin
           if (i>=(ln-3)) HBUSREQ <= 1'b0;
           HADDR  <= get_next_haddr_wrap(HBURST, HADDR, size);
           HTRANS <= 2'b11; //`HTRANS_SEQ;
           @ (posedge HCLK);
           while (HREADY==1'b0) @ (posedge HCLK);
           `ifdef ENDIAN_BIG
           data_burst_read[beat] <= HRDATA>>(24-(8*loc[1:0])); // big-endian
           `else
           data_burst_read[beat] <= HRDATA>>(8*loc[1:0]); // little-endian
           `endif
           if (HRESP!=2'b00) begin //if (HRESP!=`HRESP_OKAY)
               status = 1;
               $display("%04d %m ERROR: non OK response for read", $time);
           end
           k = k+1;
           loc = loc + size;
           beat = beat + 1;
       end
       leng = leng - ln;
       if (leng==0) begin
          HTRANS  <= 0;
          HBUSREQ <= 1'b0;
       end else begin
           status = 1;
           $display("%04d not supported wrap burst", $time);
       end
    end
    @ (posedge HCLK);
    while (HREADY==0) @ (posedge HCLK);
    data_burst_read[beat] = HRDATA>>(8*loc[1:0]); // must be blocking
end
endtask

//------------------------------------------------------------------------------
task ahb_write_burst;
     input  [31:0] addr;
     input  [ 2:0] hburst;
     output [ 1:0] status; // 0 for OK
begin
     ahb_write_burst_core( addr
                         , 4
                         , hburst
                         , 4'b0001
                         , status);
end
endtask

//------------------------------------------------------------------------------
task ahb_write_burst_core;
     input  [31:0] addr;
     input  [ 2:0] size;
     input  [ 2:0] hburst; // HBURST
     input  [ 3:0] hprot;// HPROT
     output [ 1:0] status; // 0 for OK
begin
     case (hburst)
     3'b000: ahb_write_core(addr,data_burst_write[0],size,hprot,1'b0,status); // SINGLE
     3'b001: ahb_write_burst_inc (addr,size, 1,hprot,status); // INCR
     3'b010: ahb_write_burst_wrap(addr,size, 4,hprot,status); // WRAP4
     3'b011: ahb_write_burst_inc (addr,size, 4,hprot,status); // INCR4
     3'b100: ahb_write_burst_wrap(addr,size, 8,hprot,status); // WRAP8
     3'b101: ahb_write_burst_inc (addr,size, 8,hprot,status); // INCR8
     3'b110: ahb_write_burst_wrap(addr,size,16,hprot,status); // WRAP16
     3'b111: ahb_write_burst_inc (addr,size,16,hprot,status); // INCR16
     endcase
end
endtask

//------------------------------------------------------------------------------
// Bursts must not cross a 1kB address boundary.
task ahb_write_burst_inc;
     input  [31:0] addr;
     input  [ 2:0] size;
     input  [31:0] leng;
     input  [ 3:0] hprot;// HPROT
     output [ 1:0] status; // 0 for OK
     reg    [31:0] loc ; // keep current address
     reg    [31:0] beat; // keep beat num.
     integer       i, j, ln;
begin
    status = 0;
    j = 0;
    ln = 0;
    @ (posedge HCLK);
    HBUSREQ <= 1'b1;
    @ (posedge HCLK);
    while (leng>0) begin
       while ((HGRANT!==1'b1)||(HREADY!==1'b1)) @ (posedge HCLK);
       HPROT  <= hprot;
       HADDR  <= addr; addr = addr + 4;
       HTRANS <= 2'b10; //`HTRANS_NONSEQ;
       if (leng>=16)     begin HBURST <= 3'b111; ln=16; end//`HBURST_INCR16;
       else if (leng>=8) begin HBURST <= 3'b101; ln= 8; end//`HBURST_INCR8;
       else if (leng>=4) begin HBURST <= 3'b011; ln= 4; end//`HBURST_INCR4;
       else              begin HBURST <= 3'b001; ln=leng; end//`HBURST_INCR;
       HWRITE <= 1'b1; //`HWRITE_WRITE;
       case (size)
       1:  HSIZE <= 3'b000; //`HSIZE_BYTE;
       2:  HSIZE <= 3'b001; //`HSIZE_HWORD;
       4:  HSIZE <= 3'b010; //`HSIZE_WORD;
       default: begin
                status = 1;
                $display("%04d %m ERROR: unsupported transfer size: %d-byte", $time, size);
                end
       endcase
       loc  = addr;
       beat = 0;
       for (i=0; i<ln-1; i=i+1) begin
           @ (posedge HCLK);
           while (HREADY==1'b0) @ (posedge HCLK);
           if (i>=(ln-3)) HBUSREQ <= 1'b0;
           `ifdef ENDIAN_BIG
           HWDATA <= data_burst_write[beat]<<(24-(8*loc[1:0])); // big-endian
           `else
           HWDATA <= data_burst_write[beat]<<(8*loc[1:0]); // little-endian
           `endif
           HADDR  <= HADDR + size;
           HTRANS <= 2'b11; //`HTRANS_SEQ;
           loc = loc + size;
           beat = beat + 1;
           while (HREADY==1'b0) @ (posedge HCLK);
           if (HRESP!=2'b00) begin //`HRESP_OKAY
               status = 1;
               $display("%04d %m ERROR: non OK response write", $time);
           end
       end
       @ (posedge HCLK);
       while (HREADY==0) @ (posedge HCLK);
       `ifdef ENDIAN_BIG
       HWDATA <= data_burst_write[beat]<<(24-(8*loc[1:0]));
       `else
       HWDATA <= data_burst_write[beat]<<(8*loc[1:0]);
       `endif
       if (HRESP!=2'b00) begin //`HRESP_OKAY
           status = 1;
           $display("%04d %m ERROR: non OK response write", $time);
       end
       if (ln==leng) begin
           HTRANS  <= 0;
           HBUSREQ <= 1'b0;
       end
       leng = leng - ln;
       j = j+ln;
    end
    @ (posedge HCLK);
    while (HREADY==0) @ (posedge HCLK);
    if (HRESP!=2'b00) begin //`HRESP_OKAY
        status = 1;
        $display("%04d %m ERROR: non OK response write", $time);
    end
end
endtask

//------------------------------------------------------------------------------
task ahb_write_burst_wrap;
     input  [31:0] addr;
     input  [ 2:0] size;
     input  [31:0] leng;
     input  [ 3:0] hprot;// HPROT
     output [ 1:0] status; // 0 for OK
     reg    [31:0] loc ; // keep current address
     reg    [31:0] beat; // keep beat num.
     integer       i, j, ln;
begin
    status = 0;
    j = 0;
    ln = 0;
    @ (posedge HCLK);
    HBUSREQ <= 1'b1;
    @ (posedge HCLK);
    while (leng>0) begin
       while ((HGRANT!==1'b1)||(HREADY!==1'b1)) @ (posedge HCLK);
       HPROT  <= hprot;
       HADDR  <= addr;
       HTRANS <= 2'b10; //`HTRANS_NONSEQ;
       if (leng==16)     begin HBURST <= 3'b111; ln=16; end//`HBURST_INCR16;
       else if (leng==8) begin HBURST <= 3'b101; ln= 8; end//`HBURST_INCR8;
       else if (leng==4) begin HBURST <= 3'b011; ln= 4; end//`HBURST_INCR4;
       else              begin
                         status = 1;
                         $display("%04d un-defined burst for wrap: %d", $time, leng);
                         end
       HWRITE <= 1'b1; //`HWRITE_WRITE;
       case (size)
       1:  HSIZE <= 3'b000; //`HSIZE_BYTE;
       2:  HSIZE <= 3'b001; //`HSIZE_HWORD;
       4:  HSIZE <= 3'b010; //`HSIZE_WORD;
       default: begin
                status = 1;
                $display("%04d %m ERROR: unsupported transfer size: %d-byte", $time, size);
                end
       endcase
       loc  = addr;
       beat = 0;
       for (i=0; i<ln-1; i=i+1) begin
           @ (posedge HCLK);
           while (HREADY==1'b0) @ (posedge HCLK);
           if (i>=(ln-3)) HBUSREQ <= 1'b0;
           `ifdef ENDIAN_BIG
           HWDATA <= data_burst_write[beat]<<(24-(8*loc[1:0])); // big-endian
           `else
           HWDATA <= data_burst_write[beat]<<(8*loc[1:0]); // little-endian
           `endif
           HADDR  <= get_next_haddr_wrap(HBURST, HADDR, size);
           HTRANS <= 2'b11; //`HTRANS_SEQ;
           loc = loc + size;
           beat = beat + 1;
           while (HREADY==1'b0) @ (posedge HCLK);
           if (HRESP!=2'b00) begin //`HRESP_OKAY
               status = 1;
               $display("%04d %m ERROR: non OK response write", $time);
           end
       end
       @ (posedge HCLK);
       while (HREADY==0) @ (posedge HCLK);
       `ifdef ENDIAN_BIG
       HWDATA <= data_burst_write[beat]<<(24-(8*loc[1:0]));
       `else
       HWDATA <= data_burst_write[beat]<<(8*loc[1:0]);
       `endif
       if (HRESP!=2'b00) begin //`HRESP_OKAY
           status = 1;
           $display("%04d %m ERROR: non OK response write", $time);
       end
       if (ln==leng) begin
           HTRANS  <= 0;
           HBUSREQ <= 1'b0;
       end
       leng = leng - ln;
       j = j+ln;
    end
    @ (posedge HCLK);
    while (HREADY==0) @ (posedge HCLK);
    if (HRESP!=2'b00) begin //`HRESP_OKAY
        status = 1;
        $display("%04d %m ERROR: non OK response write", $time);
    end
end
endtask

//------------------------------------------------------------------------------
// Get next wrapping address
function [31:0] get_next_haddr_wrap;
     input [ 2:0] hburst;
     input [31:0] haddr;
     input [ 2:0] size; // num of bytes for each beat
begin
    get_next_haddr_wrap = haddr;
    case (hburst)
    3'b111: get_next_haddr_wrap[5:0] = haddr[5:0] + size; //`HBURST_INCR16;
    3'b101: get_next_haddr_wrap[4:0] = haddr[4:0] + size; //`HBURST_INCR8;
    3'b011: get_next_haddr_wrap[3:0] = haddr[3:0] + size; //`HBURST_INCR4;
    default: $display("%04d undefined burst: %d", $time, hburst);
    endcase
end
endfunction

//------------------------------------------------------------------------------
// Revision history
//
// 2018.07.19: Rewritten by Ando Ki (andoki@gmail.com)
//-------------------------------------------------------------------------
`endif
