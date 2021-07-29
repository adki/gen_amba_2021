//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Master-to-Slave mux for AMBA AHB
//--------------------------------------------------------
#include <stdio.h>

//--------------------------------------------------------
int gen_ahb_m2s( unsigned int num // num of masters
               , char *prefix
               , FILE *fo)
{
    int i;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %sahb_m2s_m%d\n", prefix, num);
fprintf(fo, "(\n");
fprintf(fo, "       input   wire          HRESETn\n");
fprintf(fo, "     , input   wire          HCLK\n");
fprintf(fo, "     , input   wire          HREADY\n");
fprintf(fo, "     , input   wire  [ 3:0]  HMASTER\n");
fprintf(fo, "     , output  reg   [31:0]  HADDR\n");
fprintf(fo, "     , output  reg   [ 3:0]  HPROT\n");
fprintf(fo, "     , output  reg   [ 1:0]  HTRANS\n");
fprintf(fo, "     , output  reg           HWRITE\n");
fprintf(fo, "     , output  reg   [ 2:0]  HSIZE\n");
fprintf(fo, "     , output  reg   [ 2:0]  HBURST\n");
fprintf(fo, "     , output  reg   [31:0]  HWDATA\n");
     for (i=0; i<num; i++) {
fprintf(fo, "     , input   wire  [31:0]  HADDR%d\n", i);
fprintf(fo, "     , input   wire  [ 3:0]  HPROT%d\n", i);
fprintf(fo, "     , input   wire  [ 1:0]  HTRANS%d\n", i);
fprintf(fo, "     , input   wire          HWRITE%d\n", i);
fprintf(fo, "     , input   wire  [ 2:0]  HSIZE%d\n", i);
fprintf(fo, "     , input   wire  [ 2:0]  HBURST%d\n", i);
fprintf(fo, "     , input   wire  [31:0]  HWDATA%d\n", i);
     }
fprintf(fo, ");\n");
fprintf(fo, "       reg [3:0] hmaster_delay=4'h0;\n");
fprintf(fo, "       always @ (posedge HCLK or negedge HRESETn) begin\n");
fprintf(fo, "           if (HRESETn==1'b0) begin\n");
fprintf(fo, "                hmaster_delay <= 4'b0;\n");
fprintf(fo, "           end else begin\n");
fprintf(fo, "                if (HREADY) begin\n");
fprintf(fo, "                   hmaster_delay <= HMASTER;\n");
fprintf(fo, "                end\n");
fprintf(fo, "           end\n");
fprintf(fo, "       end\n");
fprintf(fo, "       always @ (HMASTER");
     for (i=0; i<num; i++) {
fprintf(fo, " or HADDR%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (HMASTER)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HADDR = HADDR%d;\n", i, i);
     }
fprintf(fo, "           default: HADDR = ~32'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "       always @ (HMASTER");
     for (i=0; i<num; i++) {
fprintf(fo, " or HPROT%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (HMASTER)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HPROT = HPROT%d;\n", i, i);
     }
fprintf(fo, "           default: HPROT = 4'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "       always @ (HMASTER");
     for (i=0; i<num; i++) {
fprintf(fo, " or HTRANS%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (HMASTER)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HTRANS = HTRANS%d;\n", i, i);
     }
fprintf(fo, "           default: HTRANS = 2'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "       always @ (HMASTER");
     for (i=0; i<num; i++) {
fprintf(fo, " or HWRITE%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (HMASTER)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HWRITE = HWRITE%d;\n", i, i);
     }
fprintf(fo, "           default: HWRITE = 1'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "       always @ (HMASTER");
     for (i=0; i<num; i++) {
fprintf(fo, " or HSIZE%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (HMASTER)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HSIZE = HSIZE%d;\n", i, i);
     }
fprintf(fo, "           default: HSIZE = 3'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "       always @ (HMASTER");
     for (i=0; i<num; i++) {
fprintf(fo, " or HBURST%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (HMASTER)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HBURST = HBURST%d;\n", i, i);
     }
fprintf(fo, "           default: HBURST = 3'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "       always @ (hmaster_delay");
     for (i=0; i<num; i++) {
fprintf(fo, " or HWDATA%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "           case (hmaster_delay)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "           4'h%X: HWDATA = HWDATA%d;\n", i, i);
     }
fprintf(fo, "           default: HWDATA = 3'b0;\n");
fprintf(fo, "           endcase\n");
fprintf(fo, "        end\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
