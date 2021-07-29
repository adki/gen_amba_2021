//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Address decoder for AMBA APB
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//--------------------------------------------------------
int gen_apb_decoder( unsigned int num // num of slaves
                   , char* prefix
                   , FILE* fo)
{
    int i, j;
    unsigned int start=0xC0000000;
    unsigned int size =0x00001000;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %sapb_decoder_s%d\n", prefix, num);
fprintf(fo, "     #(parameter P_NUM=%d // num of slaves\n", num);
    for (i=0; i<num; i++) {
fprintf(fo, "               , P_PSEL%d_START=32'h%08X, P_PSEL%d_SIZE=32'h%08X\n", i, start, i, size);
fprintf(fo, "               , P_PSEL%d_END  =P_PSEL%d_START+P_PSEL%d_SIZE\n", i, i, i);
        start += 0x00001000;
    }
fprintf(fo, "               )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input   wire        PSEL\n");
fprintf(fo, "     , input   wire [31:0] PADDR\n");
    for (i=0; i<num; i++) {
fprintf(fo, "     , output  wire        PSEL%d\n", i);
    }
fprintf(fo, ");\n");
    for (i=0; i<num; i++) {
fprintf(fo, "   assign PSEL%d = ((PSEL==1'b1)&&(P_NUM>%d)&&(PADDR>=P_PSEL%d_START)&&(PADDR<P_PSEL%d_END)) ? 1'b1 : 1'b0;\n", i, i, i, i);
    }
fprintf(fo, "   `ifdef RIGOR\n");
fprintf(fo, "   // synthesis translate_off\n");
fprintf(fo, "   initial begin\n");
for (i=0; i<num; i++) {
//fprintf(fo, "       if ((P_PSEL%d_START!=0)&&(P_PSEL%d_START<32'h10000)) $display(\"%%m ERROR P_PSEL%d_START should be positive 32-bit\");\n", i, i, i);
fprintf(fo, "       if (P_PSEL%d_SIZE==0) $display(\"%%m ERROR P_PSEL%d_SIZE should be positive 32-bit\");\n", i, i);
}
for (i=0; i<num; i++) {
for (j=0; j<num; j++) {
if (i!=j) {
fprintf(fo, "       if ((P_PSEL%d_END>P_PSEL%d_START)&&\n", i, j);
fprintf(fo, "           (P_PSEL%d_END<=P_PSEL%d_END)) $display(\"%%m ERROR address range overlapped %d:%d\");\n", i, j, i, j);
//printf("       $display(\"P_PSEL%d_END:0x%%X P_PSEL%d_START:0x%%X\", P_PSEL%d_END,P_PSEL%d_START);\n", i, j, i, j);
//printf("       $display(\"P_PSEL%d_END:0x%%X P_PSEL%d_END  :0x%%X\", P_PSEL%d_END,P_PSEL%d_END);\n", i, j, i, j);
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
