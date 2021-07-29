//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Generate AMBA AHB
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "gen_amba_ahb.h"

//--------------------------------------------------------
int gen_ahb_amba(unsigned int numM // num of masters
                , unsigned int numS // num of slaves
                , char *module
                , char *prefix
                , FILE *fo)
{
    int ret=0;

    if ((numM<2)||(numS<2)||(module==NULL)||(prefix==NULL)) return 1;

    ret += gen_ahb_amba_core ( numM, numS, module, prefix, fo );
    ret += gen_ahb_arbiter( numM, numS, prefix, fo );
    ret += gen_ahb_m2s( numM, prefix, fo );
    ret += gen_ahb_lite( 0, numS, module, prefix, fo );

    return ret;
}

//--------------------------------------------------------
int gen_ahb_amba_core( unsigned int numM // num of masters
                     , unsigned int numS // num of slaves
                     , char *module
                     , char *prefix
                     , FILE *fo)
{
    int i;
    unsigned int start=0x00000000;
    unsigned int size =0x00010000;

    if ((numM<2)||(numS<2)||(module==NULL)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %s\n", module);
fprintf(fo, "     #(parameter P_NUMM=%d // num of masters\n", numM);
fprintf(fo, "               , P_NUMS=%d // num of slaves\n", numS);
    for (i=0; i<numS; i++) {
fprintf(fo, "               , P_HSEL%d_START=32'h%08X, P_HSEL%d_SIZE=32'h%08X\n", i, start, i, size);
         start += (numS>16) ? 0x01000000 : 0x10000000;
    }
fprintf(fo, "               )\n");
fprintf(fo, "(\n");
fprintf(fo, "        input   wire         HRESETn\n");
fprintf(fo, "      , input   wire         HCLK\n");
    for (i=0; i<numM; i++) {
fprintf(fo, "      , input   wire         M%d_HBUSREQ\n", i);
fprintf(fo, "      , output  wire         M%d_HGRANT\n", i);
fprintf(fo, "      , input   wire  [31:0] M%d_HADDR\n", i);
fprintf(fo, "      , input   wire  [ 1:0] M%d_HTRANS\n", i);
fprintf(fo, "      , input   wire  [ 2:0] M%d_HSIZE\n", i);
fprintf(fo, "      , input   wire  [ 2:0] M%d_HBURST\n", i);
fprintf(fo, "      , input   wire  [ 3:0] M%d_HPROT\n", i);
fprintf(fo, "      , input   wire         M%d_HLOCK\n", i);
fprintf(fo, "      , input   wire         M%d_HWRITE\n", i);
fprintf(fo, "      , input   wire  [31:0] M%d_HWDATA\n", i);
    }
fprintf(fo, "      , output  wire  [31:0] M_HRDATA\n");
fprintf(fo, "      , output  wire  [ 1:0] M_HRESP\n");
fprintf(fo, "      , output  wire         M_HREADY\n");
fprintf(fo, "      , output  wire  [31:0] S_HADDR\n");
fprintf(fo, "      , output  wire         S_HWRITE\n");
fprintf(fo, "      , output  wire  [ 1:0] S_HTRANS\n");
fprintf(fo, "      , output  wire  [ 2:0] S_HSIZE\n");
fprintf(fo, "      , output  wire  [ 2:0] S_HBURST\n");
fprintf(fo, "      , output  wire  [31:0] S_HWDATA\n");
fprintf(fo, "      , output  wire  [ 3:0] S_HPROT\n");
fprintf(fo, "      , output  wire         S_HREADY\n");
fprintf(fo, "      , output  wire  [ 3:0] S_HMASTER\n");
fprintf(fo, "      , output  wire         S_HMASTLOCK\n");
    for (i=0; i<numS; i++) {
fprintf(fo, "      , output  wire         S%d_HSEL\n", i);
fprintf(fo, "      , input   wire         S%d_HREADY\n", i);
fprintf(fo, "      , input   wire  [ 1:0] S%d_HRESP\n", i);
fprintf(fo, "      , input   wire  [31:0] S%d_HRDATA\n", i);
fprintf(fo, "      , input   wire  [15:0] S%d_HSPLIT\n", i);
    }
fprintf(fo, "      , input   wire         REMAP\n");
fprintf(fo, ");\n");
fprintf(fo, "  %sahb_arbiter #(.NUMM(%d),.NUMS(%d))\n", prefix, numM, numS);
fprintf(fo, "  u_ahb_arbiter (\n");
fprintf(fo, "         .HRESETn   (HRESETn    )\n");
fprintf(fo, "       , .HCLK      (HCLK       )\n");
fprintf(fo, "       , .HBUSREQ   ({M%d_HBUSREQ", numM-1);
    for (i=numM-2; i>=0; i--) {
fprintf(fo, ",M%d_HBUSREQ", i);
    }
fprintf(fo, "})\n");
fprintf(fo, "       , .HGRANT    ({M%d_HGRANT", numM-1);
    for (i=numM-2; i>=0; i--) {
fprintf(fo, ",M%d_HGRANT", i);
    }
fprintf(fo, "})\n");
fprintf(fo, "       , .HMASTER   (S_HMASTER  )\n");
fprintf(fo, "       , .HLOCK     ({M%d_HLOCK", numM-1);
    for (i=numM-2; i>=0; i--) {
fprintf(fo, ",M%d_HLOCK", i);
    }
fprintf(fo, "})\n");
fprintf(fo, "       , .HREADY    (M_HREADY   )\n");
fprintf(fo, "       , .HMASTLOCK (S_HMASTLOCK)\n");
fprintf(fo, "       , .HSPLIT    ({S%d_HSPLIT", numS-1);
    for (i=numS-2; i>=0; i--) {
fprintf(fo, ",S%d_HSPLIT", i);
    }
fprintf(fo, "})\n");
fprintf(fo, "  );\n");
fprintf(fo, "  wire  [31:0] M_HADDR;\n");
fprintf(fo, "  wire  [ 1:0] M_HTRANS;\n");
fprintf(fo, "  wire  [ 2:0] M_HSIZE;\n");
fprintf(fo, "  wire  [ 2:0] M_HBURST;\n");
fprintf(fo, "  wire  [ 3:0] M_HPROT;\n");
fprintf(fo, "  wire         M_HWRITE;\n");
fprintf(fo, "  wire  [31:0] M_HWDATA;\n");
fprintf(fo, "  %sahb_m2s_m%d u_ahb_m2s (\n", prefix, numM);
fprintf(fo, "       .HRESETn  (HRESETn    )\n");
fprintf(fo, "     , .HCLK     (HCLK       )\n");
fprintf(fo, "     , .HREADY   (M_HREADY   )\n");
fprintf(fo, "     , .HMASTER  (S_HMASTER  )\n");
fprintf(fo, "     , .HADDR    (M_HADDR    )\n");
fprintf(fo, "     , .HPROT    (M_HPROT    )\n");
fprintf(fo, "     , .HTRANS   (M_HTRANS   )\n");
fprintf(fo, "     , .HWRITE   (M_HWRITE   )\n");
fprintf(fo, "     , .HSIZE    (M_HSIZE    )\n");
fprintf(fo, "     , .HBURST   (M_HBURST   )\n");
fprintf(fo, "     , .HWDATA   (M_HWDATA   )\n");
    for (i=0; i<numM; i++) {
fprintf(fo, "     , .HADDR%d   (M%d_HADDR  )\n", i, i);
fprintf(fo, "     , .HPROT%d   (M%d_HPROT  )\n", i, i);
fprintf(fo, "     , .HTRANS%d  (M%d_HTRANS )\n", i, i);
fprintf(fo, "     , .HWRITE%d  (M%d_HWRITE )\n", i, i);
fprintf(fo, "     , .HSIZE%d   (M%d_HSIZE  )\n", i, i);
fprintf(fo, "     , .HBURST%d  (M%d_HBURST )\n", i, i);
fprintf(fo, "     , .HWDATA%d  (M%d_HWDATA )\n", i, i);
    }
fprintf(fo, "  );\n");
fprintf(fo, "  %sahb_lite_s%d #(.P_NUM(%d)\n", prefix, numS, numS);
    for (i=0; i<numS; i++) {
fprintf(fo, "               ,.P_HSEL%d_START(P_HSEL%d_START), .P_HSEL%d_SIZE(P_HSEL%d_SIZE)\n", i, i, i, i);
    }
fprintf(fo, "               )\n");
fprintf(fo, "  u_ahb_lite (\n");
fprintf(fo, "       .HRESETn   (HRESETn  )\n");
fprintf(fo, "     , .HCLK      (HCLK     )\n");
fprintf(fo, "     , .M_HADDR   (M_HADDR  )\n");
fprintf(fo, "     , .M_HTRANS  (M_HTRANS )\n");
fprintf(fo, "     , .M_HWRITE  (M_HWRITE )\n");
fprintf(fo, "     , .M_HSIZE   (M_HSIZE  )\n");
fprintf(fo, "     , .M_HBURST  (M_HBURST )\n");
fprintf(fo, "     , .M_HPROT   (M_HPROT  )\n");
fprintf(fo, "     , .M_HWDATA  (M_HWDATA )\n");
fprintf(fo, "     , .M_HRDATA  (M_HRDATA )\n");
fprintf(fo, "     , .M_HRESP   (M_HRESP  )\n");
fprintf(fo, "     , .M_HREADY  (M_HREADY )\n");
fprintf(fo, "     , .S_HADDR   (S_HADDR  )\n");
fprintf(fo, "     , .S_HTRANS  (S_HTRANS )\n");
fprintf(fo, "     , .S_HSIZE   (S_HSIZE  )\n");
fprintf(fo, "     , .S_HBURST  (S_HBURST )\n");
fprintf(fo, "     , .S_HWRITE  (S_HWRITE )\n");
fprintf(fo, "     , .S_HPROT   (S_HPROT  )\n");
fprintf(fo, "     , .S_HWDATA  (S_HWDATA )\n");
fprintf(fo, "     , .S_HREADY  (S_HREADY )\n");
    for (i=0; i<numS; i++) {
fprintf(fo, "     , .S%d_HSEL   (S%d_HSEL  )\n", i, i);
fprintf(fo, "     , .S%d_HRDATA (S%d_HRDATA)\n", i, i);
fprintf(fo, "     , .S%d_HRESP  (S%d_HRESP )\n", i, i);
fprintf(fo, "     , .S%d_HREADY (S%d_HREADY)\n", i, i);
    }
fprintf(fo, "     , .REMAP     (REMAP    )\n");
fprintf(fo, "  );\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
