//--------------------------------------------------------
// Copyright (c) 2021 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2021.07.01.
//--------------------------------------------------------
// WID generator to support AXI4
//--------------------------------------------------------
#include <stdio.h>

//--------------------------------------------------------
int gen_axi_wid( char *prefix
               , FILE *fo)
{
fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %saxi_wid\n", prefix);
fprintf(fo, "      #(parameter WIDTH_ID= 4)\n");
fprintf(fo, "(\n");
fprintf(fo, "       input   wire                      ARESETn\n");
fprintf(fo, "     , input   wire                      ACLK\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     , input   wire  [WIDTH_ID-1:0]      M_AWID\n");
fprintf(fo, "     , input   wire  [ 7:0]              M_AWLEN\n");
fprintf(fo, "     , input   wire                      M_AWVALID\n");
fprintf(fo, "     , input   wire                      M_AWREADY\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     , output  wire   [WIDTH_ID-1:0]     M_WID\n");
fprintf(fo, "     , input   wire                      M_WLAST\n");
fprintf(fo, "     , input   wire                      M_WVALID\n");
fprintf(fo, "     , input   wire                      M_WREADY\n");
fprintf(fo, ");\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     wire                  fifo_push_ready; // not used\n");
fprintf(fo, "     wire                  fifo_push_valid;\n");
fprintf(fo, "     wire  [WIDTH_ID-1:0]  fifo_push_din  = M_AWID;\n");
fprintf(fo, "     wire                  fifo_pop_ready ;\n");
fprintf(fo, "     wire                  fifo_pop_valid ; // not used\n");
fprintf(fo, "     wire  [WIDTH_ID-1:0]  fifo_pop_dout  ;\n");
fprintf(fo, "     wire                  fifo_empty     ;\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     axi_fifo_sync #(.FDW(WIDTH_ID), .FAW(WIDTH_ID))\n");
fprintf(fo, "     u_axi_wid_fifo_sync\n");
fprintf(fo, "     (\n");
fprintf(fo, "           .rstn     (ARESETn)\n");
fprintf(fo, "         , .clr      (1'b0   )\n");
fprintf(fo, "         , .clk      (ACLK   )\n");
fprintf(fo, "         , .wr_rdy   (fifo_push_ready)\n");
fprintf(fo, "         , .wr_vld   (fifo_push_valid)\n");
fprintf(fo, "         , .wr_din   (fifo_push_din  )\n");
fprintf(fo, "         , .rd_rdy   (fifo_pop_ready )\n");
fprintf(fo, "         , .rd_vld   (fifo_pop_valid )\n");
fprintf(fo, "         , .rd_dout  (fifo_pop_dout  )\n");
fprintf(fo, "         , .full     (               )\n");
fprintf(fo, "         , .empty    (fifo_empty     )\n");
fprintf(fo, "         , .fullN    ()\n");
fprintf(fo, "         , .emptyN   ()\n");
fprintf(fo, "         , .item_cnt ()\n");
fprintf(fo, "         , .room_cnt ()\n");
fprintf(fo, "     );\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     assign fifo_push_din = M_AWID;\n");
fprintf(fo, "     assign fifo_push_valid = ((fifo_empty==1'b0)&&(M_AWVALID==1'b1)&&(M_AWREADY==1'b1))\n");
fprintf(fo, "                            ||((fifo_empty==1'b1)&&(M_AWVALID==1'b1)&&(M_AWREADY==1'b1)&&(M_WVALID==1'b0))\n");
fprintf(fo, "                            ||((fifo_empty==1'b1)&&(M_AWVALID==1'b1)&&(M_AWREADY==1'b1)&&(M_WVALID==1'b1)&&((M_WREADY==1'b0)||(M_WLAST==1'b0)));\n");
fprintf(fo, "     assign M_WID = ((fifo_empty==1'b1)&&\n");
fprintf(fo, "                     (M_AWVALID==1'b1)&&(M_AWREADY==1'b1)&&\n");
fprintf(fo, "                     (M_WVALID==1'b1)) ? M_AWID : fifo_pop_dout;\n");
fprintf(fo, "     assign fifo_pop_ready = ((fifo_empty==1'b0)&&(M_WVALID==1'b1)&&(M_WREADY==1'b1)&&(M_WLAST==1'b1));\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "endmodule\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2021.07.01: Started by Ando Ki.
//--------------------------------------------------------
