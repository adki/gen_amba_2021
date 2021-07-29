//--------------------------------------------------------
// Copyright (c) 2016-2017 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2017.04.13.
//--------------------------------------------------------
// AHB-to-APB bridge
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//--------------------------------------------------------
int gen_ahb2apb( unsigned int num, char* module, char* prefix, FILE* fo )
{
    int i=0;
    unsigned int start=0xC0000000;

    if ((num<2)||(module==NULL)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %s\n", module);
fprintf(fo, "     #(parameter P_NUM=%d\n", num);
fprintf(fo, "               , P_PSEL0_START=32'h%08X, P_PSEL0_SIZE=32'h00001000", start);
start += 0x00001000;
for (i=1; i<num; i++) {
fprintf(fo, "\n");
fprintf(fo, "               , P_PSEL%d_START=32'h%08X, P_PSEL%d_SIZE=32'h00001000", i, start, i);
}
fprintf(fo, "\n");
fprintf(fo, "               , CLOCK_RATIO=2'b00 // 0=1:1, 3=async\n");
fprintf(fo, "       )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input  wire        HRESETn\n");
fprintf(fo, "     , input  wire        HCLK\n");
fprintf(fo, "     , input  wire        HSEL\n");
fprintf(fo, "     , input  wire [31:0] HADDR\n");
fprintf(fo, "     , input  wire [ 1:0] HTRANS\n");
fprintf(fo, "     , input  wire [ 3:0] HPROT\n");
fprintf(fo, "     , input  wire        HLOCK\n");
fprintf(fo, "     , input  wire        HWRITE\n");
fprintf(fo, "     , input  wire [ 2:0] HSIZE\n");
fprintf(fo, "     , input  wire [ 2:0] HBURST\n");
fprintf(fo, "     , input  wire [31:0] HWDATA\n");
fprintf(fo, "     , output wire [31:0] HRDATA\n");
fprintf(fo, "     , output wire [ 1:0] HRESP\n");
fprintf(fo, "     , input  wire        HREADYin\n");
fprintf(fo, "     , output wire        HREADYout\n");
fprintf(fo, "     , input  wire        PCLK\n");
fprintf(fo, "     , input  wire        PRESETn\n");
fprintf(fo, "     , output wire        S_PENABLE\n");
fprintf(fo, "     , output wire [31:0] S_PADDR\n");
fprintf(fo, "     , output wire        S_PWRITE\n");
fprintf(fo, "     , output wire [31:0] S_PWDATA\n");
for (i=0; i<num; i++) {
fprintf(fo, "     , output wire        S%d_PSEL\n", i);
fprintf(fo, "     , input  wire [31:0] S%d_PRDATA\n", i);
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , input  wire        S%d_PREADY\n", i);
fprintf(fo, "     , input  wire        S%d_PSLVERR\n", i);
fprintf(fo, "     `endif\n");
}
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     , output wire [ 2:0] S_PPROT\n");
fprintf(fo, "     , output wire [ 3:0] S_PSTRB\n");
fprintf(fo, "     `endif\n");
fprintf(fo, ");\n");
fprintf(fo, "   //-----------------------------------------------------\n");
fprintf(fo, "   wire        M_PSEL   ;\n");
fprintf(fo, "   wire        M_PENABLE;\n");
fprintf(fo, "   wire [31:0] M_PADDR  ;\n");
fprintf(fo, "   wire        M_PWRITE ;\n");
fprintf(fo, "   wire [31:0] M_PRDATA ;\n");
fprintf(fo, "   wire [31:0] M_PWDATA ;\n");
fprintf(fo, "   `ifdef AMBA_APB3\n");
fprintf(fo, "   wire        M_PREADY ;\n");
fprintf(fo, "   wire        M_PSLVERR;\n");
fprintf(fo, "   `endif\n");
fprintf(fo, "   `ifdef AMBA_APB4\n");
fprintf(fo, "   wire [ 2:0] M_PPROT  ;\n");
fprintf(fo, "   wire [ 3:0] M_PSTRB  ;\n");
fprintf(fo, "   `endif\n");
fprintf(fo, "   //-----------------------------------------------------\n");
fprintf(fo, "    %sahb_to_apb_bridge #(.CLOCK_RATIO(CLOCK_RATIO))\n", prefix);
fprintf(fo, "    u_ahb_to_apb_bridge (\n");
fprintf(fo, "          .HRESETn   (HRESETn)\n");
fprintf(fo, "        , .HCLK      (HCLK)\n");
fprintf(fo, "        , .HSEL      (HSEL)\n");
fprintf(fo, "        , .HADDR     (HADDR)\n");
fprintf(fo, "        , .HTRANS    (HTRANS)\n");
fprintf(fo, "        , .HPROT     (HPROT)\n");
fprintf(fo, "        , .HWRITE    (HWRITE)\n");
fprintf(fo, "        , .HSIZE     (HSIZE)\n");
fprintf(fo, "        , .HBURST    (HBURST)\n");
fprintf(fo, "        , .HWDATA    (HWDATA)\n");
fprintf(fo, "        , .HRDATA    (HRDATA)\n");
fprintf(fo, "        , .HRESP     (HRESP)\n");
fprintf(fo, "        , .HREADYin  (HREADYin)\n");
fprintf(fo, "        , .HREADYout (HREADYout)\n");
fprintf(fo, "\n");
fprintf(fo, "        , .PCLK      (PCLK)\n");
fprintf(fo, "        , .PRESETn   (PRESETn)\n");
fprintf(fo, "        , .M_PSEL    (M_PSEL)\n");
fprintf(fo, "        , .M_PENABLE (M_PENABLE)\n");
fprintf(fo, "        , .M_PADDR   (M_PADDR)\n");
fprintf(fo, "        , .M_PWRITE  (M_PWRITE)\n");
fprintf(fo, "        , .M_PRDATA  (M_PRDATA)\n");
fprintf(fo, "        , .M_PWDATA  (M_PWDATA)\n");
fprintf(fo, "\n");
fprintf(fo, "        `ifdef AMBA_APB3\n");
fprintf(fo, "        , .M_PREADY      (M_PREADY )\n");
fprintf(fo, "        , .M_PSLVERR     (M_PSLVERR)\n");
fprintf(fo, "        `endif\n");
fprintf(fo, "        `ifdef AMBA_APB4\n");
fprintf(fo, "        , .M_PPROT       (M_PPROT)\n");
fprintf(fo, "        , .M_PSTRB       (M_PSTRB)\n");
fprintf(fo, "        `endif\n");
fprintf(fo, "\n");
fprintf(fo, "   );\n");
fprintf(fo, "   //-----------------------------------------------------\n");
fprintf(fo, "   %samba_apb_s%d #(.P_NUM(%d)// num of slaves\n", prefix, num, num);
for (i=0; i<num; i++) {
fprintf(fo, "                ,.P_PSEL%d_START(P_PSEL%d_START),.P_PSEL%d_SIZE(P_PSEL%d_SIZE)\n", i, i, i, i);
}
fprintf(fo, "                )\n");
fprintf(fo, "   u_amba_apb_s%d (\n", num);
fprintf(fo, "       .PRESETn       (PRESETn    )\n");
fprintf(fo, "     , .PCLK          (PCLK       )\n");
fprintf(fo, "     , .M_PSEL        (M_PSEL     )\n");
fprintf(fo, "     , .M_PENABLE     (M_PENABLE  )\n");
fprintf(fo, "     , .M_PADDR       (M_PADDR    )\n");
fprintf(fo, "     , .M_PWRITE      (M_PWRITE   )\n");
fprintf(fo, "     , .M_PWDATA      (M_PWDATA   )\n");
fprintf(fo, "     , .M_PRDATA      (M_PRDATA   )\n");
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , .M_PREADY      (M_PREADY   )\n");
fprintf(fo, "     , .M_PSLVERR     (M_PSLVERR  )\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     , .M_PPROT       (M_PPROT    )\n");
fprintf(fo, "     , .M_PSTRB       (M_PSTRB    )\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     , .S_PADDR       (S_PADDR      )\n");
fprintf(fo, "     , .S_PENABLE     (S_PENABLE    )\n");
fprintf(fo, "     , .S_PWRITE      (S_PWRITE     )\n");
fprintf(fo, "     , .S_PWDATA      (S_PWDATA     )\n");
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     , .S_PPROT       (S_PPROT      )\n");
fprintf(fo, "     , .S_PSTRB       (S_PSTRB      )\n");
fprintf(fo, "     `endif\n");
for (i=0; i<num; i++) {
fprintf(fo, "     , .S_PSEL%d      (S%d_PSEL     )\n", i, i);
fprintf(fo, "     , .S_PRDATA%d    (S%d_PRDATA   )\n", i, i);
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , .S_PREADY%d    (S%d_PREADY   )\n", i, i);
fprintf(fo, "     , .S_PSLVERR%d   (S%d_PSLVERR  )\n", i, i);
fprintf(fo, "     `endif\n");
}
fprintf(fo, "   );\n");
fprintf(fo, "   //-----------------------------------------------------\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2017.04.13: Parameter bug fixed by Ando Ki.
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
