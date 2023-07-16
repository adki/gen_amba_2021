//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Arbiter for AMBA AXI
//--------------------------------------------------------
#include <stdio.h>

static char *code[] = {
 "`ifndef AXI_FIFO_SYNC_V"
,"`define AXI_FIFO_SYNC_V"
,"//----------------------------------------------------------------"
,"// axi_fifo_sync.v"
,"//----------------------------------------------------------------"
,"// Synchronous FIFO"
,"//----------------------------------------------------------------"
,"// MACROS and PARAMETERS"
,"//     FDW: bit-width of data"
,"//     FAW: num of entries in power of 2"
,"//----------------------------------------------------------------"
,"// Features"
,"//    * ready-valid handshake protocol"
,"//    * lookahead full and empty -- see fullN and emptyN"
,"//    * First-Word Fall-Through, but rd_vld indicates its validity"
,"//----------------------------------------------------------------"
,"//    * data moves when both ready(rdy) and valid(vld) is high."
,"//    * ready(rdy) means the receiver is ready to accept data."
,"//    * valid(vld) means the data is valid on 'data'."
,"//----------------------------------------------------------------"
,"//"
,"//               __     ___     ___     ___     ___"
,"//   clk           |___|   |___|   |___|   |___|"
,"//               _______________________________"
,"//   wr_rdy     "
,"//                      _______________ "
,"//   wr_vld      ______|       ||      |___________  "
,"//                      _______  ______"
,"//   wr_din      XXXXXXX__D0___XX__D1__XXXX"
,"//               ______________                         ___"
,"//   empty                     |_______________________|"
,"//                                      _______________ "
,"//   rd_rdy      ______________________|               |___"
,"//                                      _______________"
,"//   rd_vld      ______________________|       ||      |___"
,"//                                      ________ _______"
,"//   rd_dout     XXXXXXXXXXXXXXXXXXXXXXX__D0____X__D1___XXXX"
,"//"
,"//   full        __________________________________________"
,"//"
,"//----------------------------------------------------------------"
,""
,"module axi_fifo_sync"
,"     #(parameter FDW =32, // fifo data width"
,"                 FAW =5,  // num of entries in 2 to the power FAW"
,"                 FULN=4)  // lookahead-full"
,"("
,"       input   wire           rstn// asynchronous reset (active low)"
,"     , input   wire           clr // synchronous reset (active high)"
,"     , input   wire           clk"
,"     , output  wire           wr_rdy"
,"     , input   wire           wr_vld"
,"     , input   wire [FDW-1:0] wr_din"
,"     , input   wire           rd_rdy"
,"     , output  wire           rd_vld"
,"     , output  wire [FDW-1:0] rd_dout"
,"     , output  wire           full"
,"     , output  wire           empty"
,"     , output  wire           fullN  // lookahead full"
,"     , output  wire           emptyN // lookahead empty"
,"     , output  reg  [FAW:0]   item_cnt // num of elements in the FIFO to be read"
,"     , output  wire [FAW:0]   room_cnt // num of rooms in the FIFO to be written"
,");"
,"   //---------------------------------------------------"
,"   localparam FDT = 1<<FAW;"
,"   //---------------------------------------------------"
,"   reg  [FAW:0]   fifo_head; // where data to be read"
,"   reg  [FAW:0]   fifo_tail; // where data to be written"
,"   reg  [FAW:0]   next_tail;"
,"   reg  [FAW:0]   next_head;"
,"   wire [FAW-1:0] read_addr = (rd_vld&rd_rdy) ? next_head[FAW-1:0] : fifo_head[FAW-1:0];"
,"   //---------------------------------------------------"
,"   // synopsys translate_off"
,"   initial fifo_head = 'h0;"
,"   initial fifo_tail = 'h0;"
,"   initial next_head = 'h0;"
,"   initial next_tail = 'h0;"
,"   // synopsys translate_on"
,"   //---------------------------------------------------"
,"   // accept input"
,"   // push data item into the entry pointed by fifo_tail"
,"   //"
,"   always @(posedge clk or negedge rstn) begin"
,"      if (rstn==1'b0) begin"
,"          fifo_tail <= 0;"
,"          next_tail <= 1;"
,"      end else if (clr) begin"
,"          fifo_tail <= 0;"
,"          next_tail <= 1;"
,"      end else begin"
,"          if (!full && wr_vld) begin"
,"              fifo_tail <= next_tail;"
,"              next_tail <= next_tail + 1;"
,"          end "
,"      end"
,"   end"
,"   //---------------------------------------------------"
,"   // provide output"
,"   // pop data item from the entry pointed by fifo_head"
,"   //"
,"   always @(posedge clk or negedge rstn) begin"
,"      if (rstn==1'b0) begin"
,"          fifo_head <= 0;"
,"          next_head <= 1;"
,"      end else if (clr) begin"
,"          fifo_head <= 0;"
,"          next_head <= 1;"
,"      end else begin"
,"          if (!empty && rd_rdy) begin"
,"              fifo_head <= next_head;"
,"              next_head <= next_head + 1;"
,"          end"
,"      end"
,"   end"
,"   //---------------------------------------------------"
,"   // how many items in the FIFO"
,"   //"
,"   assign  room_cnt = FDT-item_cnt;"
,"   always @(posedge clk or negedge rstn) begin"
,"      if (rstn==1'b0) begin"
,"         item_cnt <= 0;"
,"      end else if (clr) begin"
,"         item_cnt <= 0;"
,"      end else begin"
,"         if (wr_vld&&!full&&(!rd_rdy||(rd_rdy&&empty))) begin"
,"             item_cnt <= item_cnt + 1;"
,"         end else"
,"         if (rd_rdy&&!empty&&(!wr_vld||(wr_vld&&full))) begin"
,"             item_cnt <= item_cnt - 1;"
,"         end"
,"      end"
,"   end"
,"   "
,"   //---------------------------------------------------"
,"   assign rd_vld = ~empty;"
,"   assign wr_rdy = ~full;"
,"   assign empty  = (fifo_head == fifo_tail);"
,"   assign full   = (item_cnt>=FDT);"
,"   //---------------------------------------------------"
,"   assign fullN  = (item_cnt>=(FDT-FULN));"
,"   assign emptyN = (item_cnt<=FULN);"
,"   //---------------------------------------------------"
,"   // synopsys translate_off"
,"   `ifdef RIGOR"
,"   always @(negedge clk or negedge rstn) begin"
,"      if (rstn&&!clr) begin"
,"          if ((item_cnt==0)&&(!empty))"
,"             $display(\"%0t %m: empty flag mis-match: %d\", $time, item_cnt);"
,"          if ((item_cnt==FDT)&&(!full))"
,"             $display(\"%0t %m: full flag mis-match: %d\", $time, item_cnt);"
,"          if (item_cnt>FDT)"
,"             $display(\"%0t %m: fifo handling error: item_cnt>FDT %d:%d\", $time, item_cnt, FDT);"
,"          if ((item_cnt+room_cnt)!=FDT)"
,"             $display(\"%0t %m: count mis-match: item_cnt:room_cnt %d:%d\", $time, item_cnt, room_cnt);"
,"      end"
,"   end"
,"   `endif"
,"   // synopsys translate_on"
,"   //---------------------------------------------------"
,"   reg [FDW-1:0] Mem [0:FDT-1];"
,"   assign rd_dout  = Mem[fifo_head[FAW-1:0]];"
,"   always @(posedge clk) begin"
,"       if (!full && wr_vld) begin"
,"           Mem[fifo_tail[FAW-1:0]] <= wr_din;"
,"       end"
,"   end"
,"   //---------------------------------------------------"
,"endmodule"
,"`endif"
, NULL
};

//--------------------------------------------------------
int gen_axi_arbiter_mtos( unsigned int num // num of master
                        , char *prefix
                        , FILE *fo)
{
    int i, j;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %saxi_arbiter_mtos_m%d\n", prefix, num);
fprintf(fo, "     #(parameter WIDTH_CID=4 // Channel ID width in bits\n");
fprintf(fo, "               , WIDTH_ID =4 // Transaction ID\n");
fprintf(fo, "               , WIDTH_SID=(WIDTH_CID+WIDTH_ID)\n");
fprintf(fo, "               , NUM      =%d // num of masters\n", num);
fprintf(fo, "               )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input  wire                  ARESETn\n");
fprintf(fo, "     , input  wire                  ACLK\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       AWSELECT  // selected by address decoder\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       AWVALID\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       AWREADY\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       AWLOCK    // lock-bit only not exclusive-bit\n");
fprintf(fo, "     , output wire  [NUM-1:0]       AWGRANT\n");
for (i=0; i<num; i++)
fprintf(fo, "     , input  wire  [WIDTH_SID-1:0] AWSID%d   // {master_id,trans_id}\n", i);
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       WVALID\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       WLAST\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       WREADY\n");
fprintf(fo, "     , output reg   [NUM-1:0]       WGRANT\n");
for (i=0; i<num; i++)
fprintf(fo, "     , input  wire  [WIDTH_SID-1:0] WSID%d  // {master_id,trans_id}\n", i);
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       ARSELECT  // selected by address decoder\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       ARVALID\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       ARLOCK    // lock-bit only not exclusive-bit\n");
fprintf(fo, "     , input  wire  [NUM-1:0]       ARREADY\n");
fprintf(fo, "     , output wire  [NUM-1:0]       ARGRANT\n");
for (i=0; i<num; i++)
fprintf(fo, "     , input  wire  [WIDTH_SID-1:0] ARSID%d   // {master_id,trans_id}\n", i);
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++)
fprintf(fo, "     , input  wire  [WIDTH_CID-1:0] MID%d // master%d id\n", i, i);
fprintf(fo, ");\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     reg                  locked, unlock;\n");
fprintf(fo, "     reg  [WIDTH_SID-1:0] locksid; // {master_id,trans_id}\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire [WIDTH_SID-1:0] granted_arsid = ({WIDTH_SID{ARGRANT[0]}}&ARSID0)\n");
for (i=1; i<num; i++)
fprintf(fo, "                                        | ({WIDTH_SID{ARGRANT[%d]}}&ARSID%d)\n", i, i);
fprintf(fo, "                                        ;\n");
fprintf(fo, "     reg  [NUM-1:0] argrant_reg;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     // The atomic access should consist of a read followed by a write.\n");
fprintf(fo, "     // The atomic access should be a single burst transfer.\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     // read-address arbiter\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     localparam STAR_RUN    = 'h0,\n");
fprintf(fo, "                STAR_WAIT   = 'h1,\n");
fprintf(fo, "                STAR_LOCK   = 'h2;\n");
fprintf(fo, "     reg [1:0] stateAR=STAR_RUN;\n");
fprintf(fo, "     always @ (posedge ACLK or negedge ARESETn) begin\n");
fprintf(fo, "           if (ARESETn==1'b0) begin\n");
fprintf(fo, "               locked      <= 1'b0;\n");
fprintf(fo, "               locksid     <=  'h0;\n");
fprintf(fo, "               argrant_reg <= 'h0;\n");
fprintf(fo, "               stateAR     <= STAR_RUN;\n");
fprintf(fo, "           end else begin\n");
fprintf(fo, "               case (stateAR)\n");
fprintf(fo, "               STAR_RUN: begin\n");
fprintf(fo, "                    if (|(ARGRANT&ARLOCK)) begin\n");
fprintf(fo, "                        // note that ARLOCK={M1_ARLOCK[1],M1_ALOCK[1]}\n");
fprintf(fo, "                        locked      <= 1'b1;\n");
fprintf(fo, "                        locksid     <= granted_arsid;\n");
fprintf(fo, "                        argrant_reg <= ARGRANT;\n");
fprintf(fo, "                        stateAR     <= STAR_LOCK;\n");
fprintf(fo, "                    end else begin\n");
fprintf(fo, "                        if (|ARGRANT) begin\n");
fprintf(fo, "                           // prevent the case that\n");
fprintf(fo, "                           // the granted-one is not completed due to ~ARREADY\n");
fprintf(fo, "                           // and new higher-priority-one joined,\n");
fprintf(fo, "                           // then things can go wrong.\n");
fprintf(fo, "                           if (~|(ARGRANT&ARREADY)) begin\n");
fprintf(fo, "                               argrant_reg <= ARGRANT;\n");
fprintf(fo, "                               stateAR     <= STAR_WAIT;\n");
fprintf(fo, "                           end\n");
fprintf(fo, "                        end\n");
fprintf(fo, "                    end\n");
fprintf(fo, "                    end // STAR_RUN\n");
fprintf(fo, "               STAR_WAIT: begin\n");
fprintf(fo, "                    if (|(ARGRANT&ARVALID&ARREADY)) begin\n");
fprintf(fo, "                        stateAR <= STAR_RUN;\n");
fprintf(fo, "                    end\n");
fprintf(fo, "                    end // STAR_WAIT\n");
fprintf(fo, "               STAR_LOCK: begin\n");
fprintf(fo, "                    if (unlock) begin\n");
fprintf(fo, "                        locked      <= 1'b0;\n");
fprintf(fo, "                        locksid     <=  'h0;\n");
fprintf(fo, "                        argrant_reg <=  'h0;\n");
fprintf(fo, "                        stateAR     <= STAR_RUN;\n");
fprintf(fo, "                    end\n");
fprintf(fo, "                    end // STAR_LOCK\n");
fprintf(fo, "               endcase\n");
fprintf(fo, "           end\n");
fprintf(fo, "     end\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     assign ARGRANT = (stateAR==STAR_RUN) ? priority_sel(ARSELECT&ARVALID)\n");
fprintf(fo, "                                          : argrant_reg;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     // write-address arbiter\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire [WIDTH_SID-1:0] fifo_push_din   = (AWGRANT[0]==1'b1) ? AWSID0\n");
for (i=1; i<num; i++)
fprintf(fo, "                                          : (AWGRANT[%d]==1'b1) ? AWSID%d\n", i, i);
fprintf(fo, "                                          : 'h0;\n");
fprintf(fo, "     wire                 fifo_push_valid = |(AWGRANT&AWREADY);\n");
fprintf(fo, "     wire                 fifo_pop_ready;\n");
fprintf(fo, "     wire                 fifo_pop_valid;\n");
fprintf(fo, "     wire [WIDTH_SID-1:0] fifo_pop_dout ;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     reg [NUM-1:0] awgrant_reg;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     localparam STAW_RUN    = 'h0,\n");
fprintf(fo, "                STAW_WAIT   = 'h1,\n");
fprintf(fo, "                STAW_LOCK   = 'h2;\n");
fprintf(fo, "     reg [1:0] stateAW=STAW_RUN;\n");
fprintf(fo, "     always @ (posedge ACLK or negedge ARESETn) begin\n");
fprintf(fo, "     if (ARESETn==1'b0) begin\n");
fprintf(fo, "         awgrant_reg <=  'h0;\n");
fprintf(fo, "         unlock      <= 1'b0;\n");
fprintf(fo, "         stateAW     <= STAW_RUN;\n");
fprintf(fo, "     end else begin\n");
fprintf(fo, "     case (stateAW)\n");
fprintf(fo, "     STAW_RUN: begin\n");
fprintf(fo, "          if (~locked) begin\n");
fprintf(fo, "              if (|AWGRANT) begin\n");
fprintf(fo, "                  if (~|(AWGRANT&AWREADY)) begin\n");
fprintf(fo, "                     awgrant_reg <= AWGRANT;\n");
fprintf(fo, "                     stateAW     <= STAW_WAIT;\n");
fprintf(fo, "                  end\n");
fprintf(fo, "              end\n");
fprintf(fo, "          end else begin\n");
fprintf(fo, "              if (locksid[WIDTH_SID-1:WIDTH_ID]==MID0) begin\n");
fprintf(fo, "                 if (AWSELECT[0]&AWVALID[0]) begin\n");
fprintf(fo, "                     if (locksid[WIDTH_SID-1:WIDTH_ID]==\n");
fprintf(fo, "                         AWSID0[WIDTH_SID-1:WIDTH_ID]) begin\n");
fprintf(fo, "                         awgrant_reg <= 6'b000001;\n");
fprintf(fo, "                         if (~AWLOCK[0]) unlock <= 1'b1;\n");
fprintf(fo, "                         else            unlock <= 1'b0;\n");
fprintf(fo, "                         stateAW <= STAW_LOCK;\n");
fprintf(fo, "                     end else begin\n");
fprintf(fo, "                         // synopsys translate_off\n");
fprintf(fo, "                         `ifdef RIGOR\n");
fprintf(fo, "                         $display(\"%%0t %%m ERROR un-expected write-request during lock AWID(0x%%2x) from MID(%%3d)\",\n");
fprintf(fo, "                                          $time, AWSID0, MID0);\n");
fprintf(fo, "                         `endif\n");
fprintf(fo, "                         // synopsys translate_on\n");
fprintf(fo, "                     end\n");
fprintf(fo, "                 end\n");
for (i=1; i<num; i++) {
fprintf(fo, "              end else if (locksid[WIDTH_SID-1:WIDTH_ID]==MID%d) begin\n", i);
fprintf(fo, "                 if (AWSELECT[%d]&AWVALID[%d]) begin\n", i, i);
fprintf(fo, "                     if (locksid[WIDTH_SID-1:WIDTH_ID]==\n");
fprintf(fo, "                         AWSID%d[WIDTH_SID-1:WIDTH_ID]) begin\n", i);
fprintf(fo, "                         awgrant_reg <= %d'h%X;\n", num, 1<<i);
fprintf(fo, "                         if (~AWLOCK[%d]) unlock <= 1'b1;\n", i);
fprintf(fo, "                         else            unlock <= 1'b0;\n");
fprintf(fo, "                         stateAW <= STAW_LOCK;\n");
fprintf(fo, "                     end else begin\n");
fprintf(fo, "                         // synopsys translate_off\n");
fprintf(fo, "                         `ifdef RIGOR\n");
fprintf(fo, "                         $display(\"%%0t %%m ERROR un-expected write-request during lock AWID(0x%%2x) from MID(%%3d)\",\n");
fprintf(fo, "                                          $time, AWSID%d, MID%d);\n", i, i);
fprintf(fo, "                         `endif\n");
fprintf(fo, "                         // synopsys translate_on\n");
fprintf(fo, "                     end\n");
fprintf(fo, "                 end\n");
}
fprintf(fo, "              end\n");
fprintf(fo, "              // synopsys translate_off\n");
fprintf(fo, "              `ifdef RIGOR\n");
fprintf(fo, "              else begin\n");
fprintf(fo, "                   $display(\"%%0t %%m ERROR un-expected MID for lock 0x%%x\",\n");
fprintf(fo, "                                    $time, locksid[WIDTH_SID-1:WIDTH_ID]);\n");
fprintf(fo, "              end\n");
fprintf(fo, "              `endif\n");
fprintf(fo, "              // synopsys translate_on\n");
fprintf(fo, "          end\n");
fprintf(fo, "          end // STAW_RUN\n");
fprintf(fo, "     STAW_WAIT: begin\n");
fprintf(fo, "          if (|(AWGRANT&AWVALID&AWREADY)) begin\n");
fprintf(fo, "             awgrant_reg <= 'h0;\n");
fprintf(fo, "             stateAW     <= STAW_RUN;\n");
fprintf(fo, "          end\n");
fprintf(fo, "          end // STAW_WAIT\n");
fprintf(fo, "     STAW_LOCK: begin\n");
fprintf(fo, "          if (|(AWGRANT&AWVALID&AWREADY)) begin\n");
fprintf(fo, "             awgrant_reg <=  'h0;\n");
fprintf(fo, "             unlock      <= 1'b0;\n");
fprintf(fo, "             stateAW     <= STAW_RUN;\n");
fprintf(fo, "          end\n");
fprintf(fo, "          end // STAW_LOCK\n");
fprintf(fo, "     default: begin\n");
fprintf(fo, "          awgrant_reg <=  'h0;\n");
fprintf(fo, "          unlock      <= 1'b0;\n");
fprintf(fo, "          stateAW     <= STAW_RUN;\n");
fprintf(fo, "              end\n");
fprintf(fo, "     endcase\n");
fprintf(fo, "     end // if\n");
fprintf(fo, "     end // always\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     assign AWGRANT = ((stateAW==STAW_RUN)&~locked) ? priority_sel(AWSELECT&AWVALID)\n");
fprintf(fo, "                                                    : awgrant_reg;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     axi_fifo_sync #(.FDW(WIDTH_SID), .FAW(4))\n");
fprintf(fo, "     u_axi_fifo_sync\n");
fprintf(fo, "     (\n");
fprintf(fo, "           .rstn     (ARESETn)\n");
fprintf(fo, "         , .clr      (1'b0   )\n");
fprintf(fo, "         , .clk      (ACLK   )\n");
fprintf(fo, "         , .wr_rdy   (               )\n");
fprintf(fo, "         , .wr_vld   (fifo_push_valid)\n");
fprintf(fo, "         , .wr_din   (fifo_push_din  )\n");
fprintf(fo, "         , .rd_rdy   (fifo_pop_ready )\n");
fprintf(fo, "         , .rd_vld   (fifo_pop_valid )\n");
fprintf(fo, "         , .rd_dout  (fifo_pop_dout  )\n");
fprintf(fo, "         , .full     (               )\n");
fprintf(fo, "         , .empty    ()\n");
fprintf(fo, "         , .fullN    ()\n");
fprintf(fo, "         , .emptyN   ()\n");
fprintf(fo, "         , .item_cnt ()\n");
fprintf(fo, "         , .room_cnt ()\n");
fprintf(fo, "     );\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire active_wvalid = |(WGRANT&WVALID);\n");
fprintf(fo, "     wire active_wready = |WREADY;\n");
fprintf(fo, "     wire active_wlast  = |(WGRANT&WLAST);\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     assign fifo_pop_ready = fifo_pop_valid\n");
fprintf(fo, "                           & active_wvalid\n");
fprintf(fo, "                           & active_wready\n");
fprintf(fo, "                           & active_wlast;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     always @ ( * ) begin\n");
fprintf(fo, "     if (~fifo_pop_valid) begin\n");
fprintf(fo, "         WGRANT = %d'h0;\n", num);
fprintf(fo, "     end else begin\n");
fprintf(fo, "              if (fifo_pop_dout[WIDTH_SID-1:WIDTH_ID]==MID0) WGRANT = %d'h%X;\n", num, 1);
for (i=1; i<num; i++) {
fprintf(fo, "         else if (fifo_pop_dout[WIDTH_SID-1:WIDTH_ID]==MID%d) WGRANT = %d'h%X;\n", i, num, 1<<i);
}
fprintf(fo, "         else WGRANT = %d'h0;\n", num);
fprintf(fo, "     end // if\n");
fprintf(fo, "     end // always\n");
fprintf(fo, "     // synopsys translate_off\n");
fprintf(fo, "     `ifdef RIGOR\n");
fprintf(fo, "     always @ (negedge ACLK or negedge ARESETn) begin\n");
fprintf(fo, "          if (ARESETn==1'b1) begin\n");
fprintf(fo, "              if (fifo_pop_valid&~|WGRANT) begin\n");
fprintf(fo, "                  $display(\"%%0t %%m ERROR FIFO valid, but none granted WGRANT\", $time);\n");
fprintf(fo, "              end\n");
fprintf(fo, "          end\n");
fprintf(fo, "     end\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     // synopsys translate_on\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     function [NUM-1:0] priority_sel;\n");
fprintf(fo, "     input    [NUM-1:0] request;\n");
fprintf(fo, "     begin\n");
fprintf(fo, "          casex (request)\n");
for (i=0; i<num; i++) {
fprintf(fo, "          %d'b", num); for (j=0; j<(num-i-1); j++) fprintf(fo, "x");
                                      fprintf(fo, "1"); for (j=0; j<i; j++) fprintf(fo, "0");
                                      fprintf(fo, ": priority_sel = %d'h%X;\n", num, 1<<i);
}
fprintf(fo, "          default:   priority_sel = %d'h0;\n", num);
fprintf(fo, "          endcase\n");
fprintf(fo, "     end\n");
fprintf(fo, "     endfunction\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    i = 0;
    while (code[i] != NULL) {
         fprintf(fo, "%s\n", code[i]);
         i++;
    }
    return 0;
}
//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
