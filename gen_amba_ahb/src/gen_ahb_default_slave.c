//--------------------------------------------------------
// Copyright (c) 2016-2023 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2023.07.16.
//--------------------------------------------------------
// default slave for AMBA AHB
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static char *code[] = {
 "       input   wire         HRESETn"
,"     , input   wire         HCLK"
,"     , input   wire         HSEL"
,"     , input   wire  [31:0] HADDR"
,"     , input   wire  [ 1:0] HTRANS"
,"     , input   wire         HWRITE"
,"     , input   wire  [ 2:0] HSIZE"
,"     , input   wire  [ 2:0] HBURST"
,"     , input   wire  [31:0] HWDATA"
,"     , output  wire  [31:0] HRDATA"
,"     , output  reg   [ 1:0] HRESP"
,"     , input   wire         HREADYin"
,"     , output  reg          HREADYout"
,");"
,"   assign HRDATA = 32'h0;"
,"   localparam STH_IDLE   = 2'h0"
,"            , STH_WRITE  = 2'h1"
,"            , STH_READ0  = 2'h2;"
,"   reg [1:0] state=STH_IDLE;"
,"   always @ (posedge HCLK or negedge HRESETn) begin"
,"       if (HRESETn==0) begin"
,"           HRESP     <= 2'b00; //`HRESP_OKAY;"
,"           HREADYout <= 1'b1;"
,"           state     <= STH_IDLE;"
,"       end else begin // if (HRESETn==0) begin"
,"           case (state)"
,"           STH_IDLE: begin"
,"                if (HSEL && HREADYin) begin"
,"                   case (HTRANS)"
,"                   //`HTRANS_IDLE, `HTRANS_BUSY: begin"
,"                   2'b00, 2'b01: begin"
,"                          HREADYout <= 1'b1;"
,"                          HRESP     <= 2'b00; //`HRESP_OKAY;"
,"                          state     <= STH_IDLE;"
,"                    end // HTRANS_IDLE or HTRANS_BUSY"
,"                   //`HTRANS_NONSEQ, `HTRANS_SEQ: begin"
,"                   2'b10, 2'b11: begin"
,"                          HREADYout <= 1'b0;"
,"                          HRESP     <= 2'b01; //`HRESP_ERROR;"
,"                          if (HWRITE) begin"
,"                              state <= STH_WRITE;"
,"                          end else begin"
,"                              state <= STH_READ0;"
,"                          end"
,"                    end // HTRANS_NONSEQ or HTRANS_SEQ"
,"                   endcase // HTRANS"
,"                end else begin// if (HSEL && HREADYin)"
,"                    HREADYout <= 1'b1;"
,"                    HRESP     <= 2'b00; //`HRESP_OKAY;"
,"                end"
,"                end // STH_IDLE"
,"           STH_WRITE: begin"
,"                     HREADYout <= 1'b1;"
,"                     HRESP     <= 2'b01; //`HRESP_ERROR;"
,"                     state     <= STH_IDLE;"
,"                end // STH_WRITE"
,"           STH_READ0: begin"
,"                    HREADYout <= 1'b1;"
,"                    HRESP     <= 2'b01; //`HRESP_ERROR;"
,"                    state     <= STH_IDLE;"
,"                end // STH_READ0"
,"           endcase // state"
,"       end // if (HRESETn==0)"
,"   end // always"
,"endmodule"
, NULL
};

//--------------------------------------------------------
int gen_ahb_default_slave( char* prefix, FILE* fo )
{
    int i=0;

    if (prefix==NULL) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "`ifndef %sAHB_DEFAULT_SLAVE_V\n", prefix);
fprintf(fo, "`define %sAHB_DEFAULT_SLAVE_V\n", prefix);
fprintf(fo, "module %sahb_default_slave\n", prefix);
fprintf(fo, "(\n");
    while (code[i] != NULL) {
         fprintf(fo, "%s\n", code[i]);
         i++;
    }
fprintf(fo, "`endif\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2023.07.16: default value for 'output reg' removed.
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
