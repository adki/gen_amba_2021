//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// Arbiter for AMBA AXI
//--------------------------------------------------------
#include <stdio.h>

static char *code[] = {
 "("
,"       input  wire           ARESETn"
,"     , input  wire           ACLK"
,"     //-----------------------------------------------------------"
,"     , input  wire  [NUM:0]  BSELECT  // selected by comparing trans_id"
,"     , input  wire  [NUM:0]  BVALID"
,"     , input  wire  [NUM:0]  BREADY"
,"     , output wire  [NUM:0]  BGRANT"
,"     //-----------------------------------------------------------"
,"     , input  wire  [NUM:0]  RSELECT  // selected by comparing trans_id"
,"     , input  wire  [NUM:0]  RVALID"
,"     , input  wire  [NUM:0]  RREADY"
,"     , input  wire  [NUM:0]  RLAST"
,"     , output wire  [NUM:0]  RGRANT"
,");"
,"     //-----------------------------------------------------------"
,"     // read-data arbiter"
,"     //-----------------------------------------------------------"
,"     reg [NUM:0] rgrant_reg;"
,"     //-----------------------------------------------------------"
,"     localparam STR_RUN    = 'h0,"
,"                STR_WAIT   = 'h1;"
,"     reg stateR=STR_RUN;"
,"     always @ (posedge ACLK or negedge ARESETn) begin"
,"           if (ARESETn==1'b0) begin"
,"               rgrant_reg  <= 'h0;"
,"               stateR      <= STR_RUN;"
,"           end else begin"
,"               case (stateR)"
,"               STR_RUN: begin"
,"                    if (|RGRANT) begin"
,"                       if (~|(RGRANT&RREADY&RLAST)) begin"
,"                           rgrant_reg <= RGRANT;"
,"                           stateR     <= STR_WAIT;"
,"                       end"
,"                    end"
,"                    end // STR_RUN"
,"               STR_WAIT: begin"
,"                    if (|(RGRANT&RVALID&RREADY&RLAST)) begin"
,"                        rgrant_reg <= 'h0;"
,"                        stateR     <= STR_RUN;"
,"                    end"
,"                    end // STR_WAIT"
,"               endcase"
,"           end"
,"     end"
,"     //-----------------------------------------------------------"
,"     assign RGRANT = (stateR==STR_RUN) ? priority_sel(RSELECT&RVALID)"
,"                                       : rgrant_reg;"
,"     //-----------------------------------------------------------"
,"     // write-response arbiter"
,"     //-----------------------------------------------------------"
,"     reg [NUM:0] bgrant_reg;"
,"     localparam STB_RUN    = 'h0,"
,"                STB_WAIT   = 'h1;"
,"     reg stateB=STB_RUN;"
,"     always @ (posedge ACLK or negedge ARESETn) begin"
,"           if (ARESETn==1'b0) begin"
,"               bgrant_reg  <= 'h0;"
,"               stateB      <= STB_RUN;"
,"           end else begin"
,"               case (stateB)"
,"               STB_RUN: begin"
,"                    if (|BGRANT) begin"
,"                       if (~|(BGRANT&BREADY)) begin"
,"                           bgrant_reg <= BGRANT;"
,"                           stateB     <= STB_WAIT;"
,"                       end"
,"                    end"
,"                    end // STB_RUN"
,"               STB_WAIT: begin"
,"                    if (|(BGRANT&BVALID&BREADY)) begin"
,"                        bgrant_reg <= 'h0;"
,"                        stateB     <= STB_RUN;"
,"                    end"
,"                    end // STB_WAIT"
,"               endcase"
,"           end"
,"     end"
,"     //-----------------------------------------------------------"
,"     assign BGRANT = (stateB==STB_RUN) ? priority_sel(BSELECT&BVALID)"
,"                                       : bgrant_reg;"
,"     //-----------------------------------------------------------"
,NULL
};

//--------------------------------------------------------
int gen_axi_arbiter_stom( unsigned int num // num of slave
                        , char *prefix
                        , FILE *fo)
{
    int i, j;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %saxi_arbiter_stom_s%d\n", prefix, num);
fprintf(fo, "     #(parameter NUM=%d) // num of slaves\n", num);
    i=0;
    while (code[i] != NULL) {
         fprintf(fo, "%s\n", code[i]);
         i++;
    }
fprintf(fo, "     function [NUM:0] priority_sel;\n");
fprintf(fo, "        input [NUM:0] request;\n");
fprintf(fo, "     begin\n");
fprintf(fo, "          casex (request)\n");
for (i=0; i<=num; i++) {
fprintf(fo, "          %d'b", num+1); for (j=0; j<(num-i); j++) fprintf(fo, "%c", 'x');
                                      fprintf(fo, "1"); for (j=0; j<i; j++) fprintf(fo, "0");
                                      fprintf(fo, ": priority_sel = %d'h%X;\n", num+1, 1<<i);
}
fprintf(fo, "          default: priority_sel = %d'h%X;\n", num+1, 0);
fprintf(fo, "          endcase\n");
fprintf(fo, "     end\n");
fprintf(fo, "     endfunction\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}
//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
