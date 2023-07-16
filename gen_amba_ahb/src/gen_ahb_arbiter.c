//--------------------------------------------------------
// Copyright (c) 2016-2023 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2023.07.16
//--------------------------------------------------------
// Arbiter for AMBA AHB
//--------------------------------------------------------
#include <stdio.h>

static char *code[] = {
 "("
,"       input   wire               HRESETn"
,"     , input   wire               HCLK"
,"     , input   wire [NUMM-1:0]    HBUSREQ // 0: highest priority"
,"     , output  reg  [NUMM-1:0]    HGRANT"
,"     , output  reg  [     3:0]    HMASTER"
,"     , input   wire [NUMM-1:0]    HLOCK"
,"     , input   wire               HREADY"
,"     , output  reg                HMASTLOCK"
,"     , input   wire [16*NUMS-1:0] HSPLIT"
,");"
,"   reg  [NUMM-1:0] hmask={NUMM{1'b0}}; // 1=mask-out"
,"   wire [     3:0] id=encoder(HGRANT);"
,"   localparam ST_READY='h0"
,"            , ST_STAY ='h1;"
,"   reg state=ST_READY;"
,"   always @ (posedge HCLK or negedge HRESETn) begin"
,"   if (HRESETn==1'b0) begin"
,"       HGRANT    <=  'h0;"
,"       HMASTER   <= 4'h0;"
,"       HMASTLOCK <= 1'b0;"
,"       hmask     <=  'h0;"
,"       state     <= ST_READY;"
,"   end else if (HREADY==1'b1) begin"
,"       HMASTER   <= id;"
,"       HMASTLOCK <= HLOCK[id];"
,"       case (state)"
,"       ST_READY: begin"
,"          if (HBUSREQ!=0) begin"
,"              HGRANT  <= func_priority(HBUSREQ);"
,"              hmask   <= 'h0;"
,"              state   <= ST_STAY;"
,"          end"
,"          end // ST_READY"
,"       ST_STAY: begin"
,"          if (HBUSREQ=='b0) begin"
,"              HGRANT <= 'h0;"
,"              hmask  <= 'h0;"
,"              state  <= ST_READY;"
,"          end else if (HBUSREQ[id]==1'b0) begin"
,"              if ((HBUSREQ&~hmask)=='b0) begin"
,"                  HGRANT <= func_priority(HBUSREQ);"
,"                  hmask  <= 'h0;"
,"              end else begin"
,"                  HGRANT    <= func_priority(HBUSREQ&~hmask);"
,"                  hmask[id] <= 1'b1;"
,"              end"
,"          end"
,"          end // ST_STAY"
,"       default: begin"
,"                HGRANT <= 'h0;"
,"                state  <= ST_READY;"
,"                end"
,"       endcase"
,"   end // if"
,"   end // always"
,"   function [NUMM-1:0] func_priority;"
,"     input  [NUMM-1:0] req;"
,"     reg    [15:0] val;"
,"   begin"
,"     casex ({{16-NUMM{1'b0}},req})"
,"     16'bxxxx_xxxx_xxxx_xxx1: val = 'h0001;"
,"     16'bxxxx_xxxx_xxxx_xx10: val = 'h0002;"
,"     16'bxxxx_xxxx_xxxx_x100: val = 'h0004;"
,"     16'bxxxx_xxxx_xxxx_1000: val = 'h0008;"
,"     16'bxxxx_xxxx_xxx1_0000: val = 'h0010;"
,"     16'bxxxx_xxxx_xx10_0000: val = 'h0020;"
,"     16'bxxxx_xxxx_x100_0000: val = 'h0040;"
,"     16'bxxxx_xxxx_1000_0000: val = 'h0080;"
,"     16'bxxxx_xxx1_0000_0000: val = 'h0100;"
,"     16'bxxxx_xx10_0000_0000: val = 'h0200;"
,"     16'bxxxx_x100_0000_0000: val = 'h0400;"
,"     16'bxxxx_1000_0000_0000: val = 'h0800;"
,"     16'bxxx1_0000_0000_0000: val = 'h1000;"
,"     16'bxx10_0000_0000_0000: val = 'h2000;"
,"     16'bx100_0000_0000_0000: val = 'h4000;"
,"     16'b1000_0000_0000_0000: val = 'h8000;"
,"     default: val = 'h0000;"
,"     endcase"
,"     func_priority = val[NUMM-1:0];"
,"   end"
,"   endfunction // func_priority"
,"   function [3:0] encoder;"
,"     input  [NUMM-1:0] req;"
,"   begin"
,"     casex ({{16-NUMM{1'b0}},req})"
,"     16'bxxxx_xxxx_xxxx_xxx1: encoder = 'h0;"
,"     16'bxxxx_xxxx_xxxx_xx10: encoder = 'h1;"
,"     16'bxxxx_xxxx_xxxx_x100: encoder = 'h2;"
,"     16'bxxxx_xxxx_xxxx_1000: encoder = 'h3;"
,"     16'bxxxx_xxxx_xxx1_0000: encoder = 'h4;"
,"     16'bxxxx_xxxx_xx10_0000: encoder = 'h5;"
,"     16'bxxxx_xxxx_x100_0000: encoder = 'h6;"
,"     16'bxxxx_xxxx_1000_0000: encoder = 'h7;"
,"     16'bxxxx_xxx1_0000_0000: encoder = 'h8;"
,"     16'bxxxx_xx10_0000_0000: encoder = 'h9;"
,"     16'bxxxx_x100_0000_0000: encoder = 'hA;"
,"     16'bxxxx_1000_0000_0000: encoder = 'hB;"
,"     16'bxxx1_0000_0000_0000: encoder = 'hC;"
,"     16'bxx10_0000_0000_0000: encoder = 'hD;"
,"     16'bx100_0000_0000_0000: encoder = 'hE;"
,"     16'b1000_0000_0000_0000: encoder = 'hF;"
,"     default: encoder = 'h0;"
,"     endcase"
,"   end"
,"   endfunction // encoder"
,"   `ifdef RIGOR"
,"   // synthesis translate_off"
,"   integer idx, idy;"
,"   always @ ( posedge HCLK or negedge HRESETn) begin"
,"   if (HRESETn==1'b0) begin"
,"   end else begin"
,"       if (|HGRANT) begin"
,"           idy = 0;"
,"           for (idx=0; idx<NUMM; idx=idx+1) if (HGRANT[idx]) idy = idy + 1;"
,"           if (idy>1) $display(\"%04d %m ERROR AHB arbitration more than one granted\", $time);"
,"       end"
,"   end // if"
,"   end // always"
,"   // synthesis translate_on"
,"   `endif"
,"endmodule"
,NULL
};

//--------------------------------------------------------
int gen_ahb_arbiter( unsigned int numM // num of masters
                   , unsigned int numS // num of slaves
                   , char *prefix
                   , FILE *fo)
{
    if ((numM<2)||(numS<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %sahb_arbiter\n", prefix);
fprintf(fo, "     #(parameter NUMM=%d // num of masters\n", numM);
fprintf(fo, "               , NUMS=%d)// num of slaves\n", numS);
    int i=0;
    while (code[i] != NULL) {
         fprintf(fo, "%s\n", code[i]);
         i++;
    }
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}
//--------------------------------------------------------
// Revision history:
//
// 2023.07.16: 'priority' --> 'func_priority' due to SystemVerilog keyword.
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
