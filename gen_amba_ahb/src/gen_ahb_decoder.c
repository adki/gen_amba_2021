//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Address decoder for AMBA AHB
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//--------------------------------------------------------
int gen_ahb_decoder( unsigned int num, char* prefix, FILE* fo )
{
    int i, j;
    unsigned int start=0x00000000;
    unsigned int size =0x00010000;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %sahb_decoder_s%d\n", prefix, num);
fprintf(fo, "     #(parameter P_NUM        =%d\n", num);
    for (i=0; i<num; i++) {
fprintf(fo, "               , P_HSEL%d_START=32'h%08X, P_HSEL%d_SIZE=32'h%08X\n", i, start, i, size);
fprintf(fo, "               , P_HSEL%d_END  =P_HSEL%d_START+P_HSEL%d_SIZE\n", i, i, i);
        start += (num>16) ? 0x01000000: 0x10000000;
    }
fprintf(fo, "               )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input   wire [31:0] HADDR\n");
fprintf(fo, "     , output  wire        HSELd // default slave\n");
    for (i=0; i<num; i++) {
fprintf(fo, "     , output  wire        HSEL%d\n", i);
    }
fprintf(fo, "     , input   wire        REMAP\n");
fprintf(fo, ");\n");
fprintf(fo, "   wire [%d:0] ihsel;\n", num-1);
fprintf(fo, "   wire       ihseld = ~|ihsel;\n");
fprintf(fo, "   assign HSELd = ihseld;\n");
fprintf(fo, "   assign HSEL0 = (REMAP) ? ihsel[1] : ihsel[0];\n");
fprintf(fo, "   assign HSEL1 = (REMAP) ? ihsel[0] : ihsel[1];\n");
    for (i=2; i<num; i++) {
fprintf(fo, "   assign HSEL%d = ihsel[%d];\n", i, i);
    }
    for (i=0; i<num; i++) {
fprintf(fo, "   assign ihsel[%d] = ((P_NUM>%d)&&(HADDR>=P_HSEL%d_START)&&(HADDR<P_HSEL%d_END)) ? 1'b1 : 1'b0;\n", i, i, i, i);
    }
fprintf(fo, "   `ifdef RIGOR\n");
fprintf(fo, "   // synthesis translate_off\n");
fprintf(fo, "   initial begin\n");
for (i=0; i<num; i++) {
//fprintf(fo, "       if ((P_HSEL%d_START>0)&&(P_HSEL%d_START<32'h00010000)) ", i, i);
//fprintf(fo, "$display(\"%%m ERROR P_HSEL%d_START should be positive 32-bit\");\n", i);
fprintf(fo, "       if (P_HSEL%d_SIZE==0) ", i);
fprintf(fo, "$display(\"%%m ERROR P_HSEL%d_SIZE should be positive 32-bit\");\n", i);
}
for (i=0; i<num; i++) {
for (j=0; j<num; j++) {
if (i!=j) {
fprintf(fo, "       if ((P_HSEL%d_END>P_HSEL%d_START)&&\n", i, j);
fprintf(fo, "           (P_HSEL%d_END<=P_HSEL%d_END)) ", i, j);
fprintf(fo, "$display(\"%%m ERROR address range overlapped %d:%d\");\n", i, j);
}}}
fprintf(fo, "   end\n");
fprintf(fo, "   // synthesis translate_on\n");
fprintf(fo, "   `endif\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
