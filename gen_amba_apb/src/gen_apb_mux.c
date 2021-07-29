//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Read-Data mux for AMBA APB
//--------------------------------------------------------
#include <stdio.h>

//--------------------------------------------------------
int gen_apb_mux( unsigned int num // num of masters
               , char *prefix
               , FILE *fo)
{
    int i;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %sapb_mux_s%d\n", prefix, num);
fprintf(fo, "     #(parameter P_NUM=%d) // num of slaves\n", num);
fprintf(fo, "(\n");
fprintf(fo, "       input   wire  [P_NUM-1:0]  PSEL\n");
fprintf(fo, "     , output  reg   [31:0]       PRDATA\n");
for (i=0; i<num; i++) {
fprintf(fo, "     , input   wire  [31:0]       PRDATA%d\n", i);
}
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , output  reg                PREADY\n");
fprintf(fo, "     , output  reg                PSLVERR\n");
for (i=0; i<num; i++) {
fprintf(fo, "     , input   wire               PREADY%d\n", i);
fprintf(fo, "     , input   wire               PSLVERR%d\n", i);
}
fprintf(fo, "     `endif\n");
fprintf(fo, ");\n");
fprintf(fo, "     always @ ( PSEL");
for (i=0; i<num; i++) {
fprintf(fo, " or PRDATA%d", i);
}
fprintf(fo, ") begin\n");
fprintf(fo, "     case (PSEL)\n");
for (i=0; i<num; i++) {
fprintf(fo, "     %d'h%X: PRDATA = PRDATA%d;\n", num, (1<<i), i);
}
fprintf(fo, "     default: PRDATA = 32'h0;\n");
fprintf(fo, "     endcase\n");
fprintf(fo, "     end // always\n");
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     always @ ( PSEL");
for (i=0; i<num; i++) {
fprintf(fo, " or PREADY%d", i);
}
fprintf(fo, ") begin\n");
fprintf(fo, "     case (PSEL)\n");
for (i=0; i<num; i++) {
fprintf(fo, "     %d'h%X: PREADY = PREADY%d;\n", num, (1<<i), i);
}
fprintf(fo, "     default: PREADY = 1'b1; // make ready by default\n");
fprintf(fo, "     endcase\n");
fprintf(fo, "     end // always\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     always @ ( PSEL");
for (i=0; i<num; i++) {
fprintf(fo, " or PSLVERR%d", i);
}
fprintf(fo, ") begin\n");
fprintf(fo, "     case (PSEL)\n");
for (i=0; i<num; i++) {
fprintf(fo, "     %d'h%X: PSLVERR = PSLVERR%d;\n", num, (1<<i), i);
}
fprintf(fo, "     default: PSLVERR = 1'b1; // make error by default\n");
fprintf(fo, "     endcase\n");
fprintf(fo, "     end // always\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}
//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
