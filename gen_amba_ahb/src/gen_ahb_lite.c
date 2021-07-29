//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// AMBA AHB-lite
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "gen_amba_ahb.h"

int gen_ahb_lite ( unsigned int lite // AMBA AHB-lite or not
                 , unsigned int num // num of slaves
                 , char *module
                 , char *prefix
                 , FILE *fo)
{
    int ret=0;

    if ((num<2)||(module==NULL)||(prefix==NULL)) return 1;

    ret += gen_ahb_lite_core( lite, num, module, prefix, fo );
    ret += gen_ahb_decoder( num, prefix, fo );
    ret += gen_ahb_s2m( num, prefix, fo );
    ret += gen_ahb_default_slave( prefix, fo );

    return ret;
}

//--------------------------------------------------------
int gen_ahb_lite_core ( unsigned int lite
                      , unsigned int num // num of slaves
                      , char *module
                      , char *prefix
                      , FILE *fo)
{
    int i;
    unsigned int start=0x00000000;
    unsigned int size =0x00010000;

    if ((num<2)||(module==NULL)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
if (lite) fprintf(fo, "module %s\n", (module==0) ? "ahb_lite" : module);
else      fprintf(fo, "module %sahb_lite_s%d\n", prefix, num);
fprintf(fo, "     #(parameter P_NUM=%d // num of slaves\n", num);
    for (i=0; i<num; i++) {
fprintf(fo, "               , P_HSEL%d_START=32'h%08X, P_HSEL%d_SIZE=32'h%08X\n", i, start, i, size);
         start += (num>16) ? 0x01000000 : 0x10000000;
    }
fprintf(fo, "               )\n");
fprintf(fo, "(\n");
fprintf(fo, "        input   wire         HRESETn\n");
fprintf(fo, "      , input   wire         HCLK\n");
fprintf(fo, "      , input   wire  [31:0] M_HADDR\n");
fprintf(fo, "      , input   wire  [ 1:0] M_HTRANS\n");
fprintf(fo, "      , input   wire         M_HWRITE\n");
fprintf(fo, "      , input   wire  [ 2:0] M_HSIZE\n");
fprintf(fo, "      , input   wire  [ 2:0] M_HBURST\n");
fprintf(fo, "      , input   wire  [ 3:0] M_HPROT\n");
fprintf(fo, "      , input   wire  [31:0] M_HWDATA\n");
fprintf(fo, "      , output  wire  [31:0] M_HRDATA\n");
fprintf(fo, "      , output  wire  [ 1:0] M_HRESP\n");
fprintf(fo, "      , output  wire         M_HREADY\n");
fprintf(fo, "      , output  wire  [31:0] S_HADDR\n");
fprintf(fo, "      , output  wire  [ 1:0] S_HTRANS\n");
fprintf(fo, "      , output  wire  [ 2:0] S_HSIZE\n");
fprintf(fo, "      , output  wire  [ 2:0] S_HBURST\n");
fprintf(fo, "      , output  wire  [ 3:0] S_HPROT\n");
fprintf(fo, "      , output  wire         S_HWRITE\n");
fprintf(fo, "      , output  wire  [31:0] S_HWDATA\n");
fprintf(fo, "      , output  wire         S_HREADY\n");
    for (i=0; i<num; i++) {
fprintf(fo, "      , output  wire         S%d_HSEL\n", i);
fprintf(fo, "      , input   wire         S%d_HREADY\n", i);
fprintf(fo, "      , input   wire  [ 1:0] S%d_HRESP\n", i);
fprintf(fo, "      , input   wire  [31:0] S%d_HRDATA\n", i);
    }
fprintf(fo, "      , input   wire         REMAP\n");
fprintf(fo, ");\n");
fprintf(fo, "   wire        HSELd; // default slave\n");
fprintf(fo, "   wire [31:0] HRDATAd;\n");
fprintf(fo, "   wire [ 1:0] HRESPd;\n");
fprintf(fo, "   wire        HREADYd;\n");
fprintf(fo, "   assign S_HADDR  = M_HADDR;\n");
fprintf(fo, "   assign S_HTRANS = M_HTRANS;\n");
fprintf(fo, "   assign S_HSIZE  = M_HSIZE;\n");
fprintf(fo, "   assign S_HBURST = M_HBURST;\n");
fprintf(fo, "   assign S_HWRITE = M_HWRITE;\n");
fprintf(fo, "   assign S_HPROT  = M_HPROT;\n");
fprintf(fo, "   assign S_HWDATA = M_HWDATA;\n");
fprintf(fo, "   assign S_HREADY = M_HREADY;\n");
fprintf(fo, "   %sahb_decoder_s%d\n", prefix, num);
fprintf(fo, "      #(.P_NUM(%d)\n", num);
    for (i=0; i<num; i++) {
fprintf(fo, "       ,.P_HSEL%d_START(P_HSEL%d_START)", i, i);
fprintf(fo, ",.P_HSEL%d_SIZE(P_HSEL%d_SIZE)\n", i, i);
    }
fprintf(fo, "       )\n");
fprintf(fo, "   u_ahb_decoder (\n");
fprintf(fo, "                 .HADDR(M_HADDR)\n");
fprintf(fo, "                ,.HSELd(HSELd)\n");
    for (i=0; i<num; i++) {
fprintf(fo, "                ,.HSEL%d(S%d_HSEL)\n", i, i);
    }
fprintf(fo, "                ,.REMAP(REMAP));\n");
fprintf(fo, "   %sahb_s2m_s%d u_ahb_s2m (\n", prefix, num);
fprintf(fo, "                 .HRESETn(HRESETn)\n");
fprintf(fo, "                ,.HCLK   (HCLK   )\n");
fprintf(fo, "                ,.HRDATA (M_HRDATA)\n");
fprintf(fo, "                ,.HRESP  (M_HRESP )\n");
fprintf(fo, "                ,.HREADY (M_HREADY)\n");
    for (i=0; i<num; i++) {
fprintf(fo, "                ,.HSEL%d  (S%d_HSEL)\n", i, i);
fprintf(fo, "                ,.HRDATA%d(S%d_HRDATA)\n", i, i);
fprintf(fo, "                ,.HRESP%d (S%d_HRESP)\n", i, i);
fprintf(fo, "                ,.HREADY%d(S%d_HREADY)\n", i, i);
    }
fprintf(fo, "                ,.HSELd  (HSELd  )\n");
fprintf(fo, "                ,.HRDATAd(HRDATAd)\n");
fprintf(fo, "                ,.HRESPd (HRESPd )\n");
fprintf(fo, "                ,.HREADYd(HREADYd));\n");
fprintf(fo, "   %sahb_default_slave u_ahb_default_slave (\n", prefix);
fprintf(fo, "                 .HRESETn  (HRESETn  )\n");
fprintf(fo, "                ,.HCLK     (HCLK     )\n");
fprintf(fo, "                ,.HSEL     (HSELd    )\n");
fprintf(fo, "                ,.HADDR    (S_HADDR  )\n");
fprintf(fo, "                ,.HTRANS   (S_HTRANS )\n");
fprintf(fo, "                ,.HWRITE   (S_HWRITE )\n");
fprintf(fo, "                ,.HSIZE    (S_HSIZE  )\n");
fprintf(fo, "                ,.HBURST   (S_HBURST )\n");
fprintf(fo, "                ,.HWDATA   (S_HWDATA )\n");
fprintf(fo, "                ,.HRDATA   (HRDATAd  )\n");
fprintf(fo, "                ,.HRESP    (HRESPd   )\n");
fprintf(fo, "                ,.HREADYin (S_HREADY )\n");
fprintf(fo, "                ,.HREADYout(HREADYd  ));\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
