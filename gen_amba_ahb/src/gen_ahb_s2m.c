//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Slave-to-Master mux for AMBA AHB
//--------------------------------------------------------
#include <stdio.h>

//--------------------------------------------------------
int gen_ahb_s2m( unsigned int num // num of slaves
               , char* prefix
               , FILE* fo)
{
    int i;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %sahb_s2m_s%d\n", prefix, num);
fprintf(fo, "(\n");
fprintf(fo, "       input   wire         HRESETn\n");
fprintf(fo, "     , input   wire         HCLK\n");
fprintf(fo, "     , input   wire         HSELd\n");
     for (i=0; i<num; i++) {
fprintf(fo, "     , input   wire         HSEL%d\n", i);
     }
fprintf(fo, "     , output  reg   [31:0] HRDATA\n");
fprintf(fo, "     , output  reg   [ 1:0] HRESP\n");
fprintf(fo, "     , output  reg          HREADY\n");
     for (i=0; i<num; i++) {
fprintf(fo, "     , input   wire  [31:0] HRDATA%d\n", i);
fprintf(fo, "     , input   wire  [ 1:0] HRESP%d\n", i);
fprintf(fo, "     , input   wire         HREADY%d\n", i);
     }
fprintf(fo, "     , input   wire  [31:0] HRDATAd\n");
fprintf(fo, "     , input   wire  [ 1:0] HRESPd\n");
fprintf(fo, "     , input   wire         HREADYd\n");
fprintf(fo, ");\n");
     for (i=0; i<num; i++) {
fprintf(fo, "  localparam D_HSEL%d = %d'h%X;\n", i, num+1, 1<<i);
     }
fprintf(fo, "  localparam D_HSELd = %d'h%X;\n", num+1, 1<<num);
fprintf(fo, "  wire [%d:0] _hsel = {HSELd", num);
     for (i=num-1; i>=0; i--) {
fprintf(fo, ",HSEL%d", i);
     }
fprintf(fo, "};\n");
fprintf(fo, "  reg  [%d:0] _hsel_reg;\n", num);
fprintf(fo, "  always @ (posedge HCLK or negedge HRESETn) begin\n");
fprintf(fo, "    if (HRESETn==1'b0)   _hsel_reg <= %d'h0;\n", num+1);
fprintf(fo, "    else if(HREADY) _hsel_reg <= _hsel; // default HREADY must be 1'b1\n");
fprintf(fo, "  end\n");
fprintf(fo, "  always @ (_hsel_reg or HREADYd");
     for (i=0; i<num; i++) {
fprintf(fo, " or HREADY%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "    case(_hsel_reg)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "      D_HSEL%d: HREADY = HREADY%d;\n", i, i);
     }
fprintf(fo, "      D_HSELd: HREADY = HREADYd;\n");
fprintf(fo, "      default: HREADY = 1'b1;\n");
fprintf(fo, "    endcase\n");
fprintf(fo, "  end\n");
fprintf(fo, "  always @ (_hsel_reg or HRDATAd");
     for (i=0; i<num; i++) {
fprintf(fo, " or HRDATA%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "    case(_hsel_reg)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "      D_HSEL%d: HRDATA = HRDATA%d;\n", i, i);
     }
fprintf(fo, "      D_HSELd: HRDATA = HRDATAd;\n");
fprintf(fo, "      default: HRDATA = 32'b0;\n");
fprintf(fo, "    endcase\n");
fprintf(fo, "  end\n");
fprintf(fo, "  always @ (_hsel_reg or HRESPd");
     for (i=0; i<num; i++) {
fprintf(fo, " or HRESP%d", i);
     }
fprintf(fo, ") begin\n");
fprintf(fo, "    case(_hsel_reg)\n");
     for (i=0; i<num; i++) {
fprintf(fo, "      D_HSEL%d: HRESP = HRESP%d;\n", i, i);
     }
fprintf(fo, "      D_HSELd: HRESP = HRESPd;\n");
fprintf(fo, "      default: HRESP = 2'b01; //`HRESP_ERROR;\n");
fprintf(fo, "    endcase\n");
fprintf(fo, "  end\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
