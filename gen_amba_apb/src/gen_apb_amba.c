//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Gnerate AMBA APB
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "gen_amba_apb.h"

//--------------------------------------------------------
int gen_apb_amba ( unsigned int num // num of slaves
                 , char *module
                 , char *prefix
                 , int   bridge // bridge if non zero 
                 , FILE *fo)
{
    int ret = 0;

    if ((num<2)||(prefix==NULL)) return 1;

    ret += gen_apb_amba_core( num, module, prefix, bridge, fo );
    ret += gen_apb_decoder  ( num, prefix, fo );
    ret += gen_apb_mux      ( num, prefix, fo );
    return ret;
}

//--------------------------------------------------------
int gen_apb_amba_core ( unsigned int num // num of slaves
                      , char *module // used when 'bridge' is 0
                      , char *prefix
                      , int   bridge // bridge if non zero 
                      , FILE *fo)
{
    int i;
    unsigned int start=0xC0000000;
    unsigned int size =0x00001000;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
if (bridge) fprintf(fo, "module %samba_apb_s%d\n", prefix, num);
else        fprintf(fo, "module %s\n", module);
fprintf(fo, "     #(parameter P_NUM=%d // num of slaves\n", num);
for (i=0; i<num; i++) {
fprintf(fo, "               , P_PSEL%d_START=32'h%08X, P_PSEL%d_SIZE=32'h%08X\n", i, start, i, size);
     start += 0x00001000;
}
fprintf(fo, "               )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input   wire          PRESETn\n");
fprintf(fo, "     , input   wire          PCLK\n");
fprintf(fo, "     , input   wire          M_PSEL\n");
fprintf(fo, "     , input   wire          M_PENABLE\n");
fprintf(fo, "     , input   wire  [31:0]  M_PADDR\n");
fprintf(fo, "     , input   wire          M_PWRITE\n");
fprintf(fo, "     , input   wire  [31:0]  M_PWDATA\n");
fprintf(fo, "     , output  wire  [31:0]  M_PRDATA\n");
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , output  wire          M_PREADY\n");
fprintf(fo, "     , output  wire          M_PSLVERR\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     , input   wire  [ 2:0]  M_PPROT\n");
fprintf(fo, "     , input   wire  [ 3:0]  M_PSTRB\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     , output  wire  [31:0]  S_PADDR\n");
fprintf(fo, "     , output  wire          S_PENABLE\n");
fprintf(fo, "     , output  wire          S_PWRITE\n");
fprintf(fo, "     , output  wire  [31:0]  S_PWDATA\n");
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     , output  wire  [ 2:0]  S_PPROT\n");
fprintf(fo, "     , output  wire  [ 3:0]  S_PSTRB\n");
fprintf(fo, "     `endif\n");
for (i=0; i<num; i++) {
fprintf(fo, "     , output  wire          S_PSEL%d\n", i);
fprintf(fo, "     , input   wire  [31:0]  S_PRDATA%d\n", i);
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , input   wire          S_PREADY%d\n", i);
fprintf(fo, "     , input   wire          S_PSLVERR%d\n", i);
fprintf(fo, "     `endif\n");
}
fprintf(fo, ");\n");
fprintf(fo, "     assign S_PADDR   = M_PADDR;\n");
fprintf(fo, "     assign S_PENABLE = M_PENABLE;\n");
fprintf(fo, "     assign S_PWRITE  = M_PWRITE;\n");
fprintf(fo, "     assign S_PWDATA  = M_PWDATA;\n");
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     assign S_PPROT   = M_PPROT;\n");
fprintf(fo, "     assign S_PSTRB   = M_PSTRB;\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     %sapb_decoder_s%d #(.P_NUM(%d)\n", prefix, num, num);
for (i=0; i<num; i++) {
fprintf(fo, "                  ,.P_PSEL%d_START(P_PSEL%d_START),.P_PSEL%d_SIZE(P_PSEL%d_SIZE)\n", i, i, i, i);
}
fprintf(fo, "                  )\n");
fprintf(fo, "     u_apb_decoder (\n");
fprintf(fo, "           .PSEL  ( M_PSEL  )\n");
fprintf(fo, "         , .PADDR ( M_PADDR )\n");
for (i=0; i<num; i++) {
fprintf(fo, "         , .PSEL%d ( S_PSEL%d )\n", i, i);
}
fprintf(fo, "     );\n");
fprintf(fo, "     %sapb_mux_s%d #(.P_NUM(%d))\n", prefix, num, num);
fprintf(fo, "     u_apb_mux (\n");
fprintf(fo, "           .PSEL     ({S_PSEL%d", num-1);
for (i=num-2; i>=0; i--) {
fprintf(fo, ",S_PSEL%d", i);
}
fprintf(fo, "})\n");
fprintf(fo, "         , .PRDATA   (M_PRDATA )\n");
for (i=0; i<num; i++) {
fprintf(fo, "         , .PRDATA%d  (S_PRDATA%d)\n", i, i);
}
fprintf(fo, "         `ifdef AMBA_APB3\n");
fprintf(fo, "         , .PREADY   (M_PREADY  )\n");
fprintf(fo, "         , .PSLVERR  (M_PSLVERR )\n");
for (i=0; i<num; i++) {
fprintf(fo, "         , .PREADY%d  (S_PREADY%d)\n", i, i);
fprintf(fo, "         , .PSLVERR%d (S_PSLVERR%d)\n", i, i);
}
fprintf(fo, "         `endif\n");
fprintf(fo, "     );\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
