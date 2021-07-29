//--------------------------------------------------------
// Copyright (c) 2018 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2018.09.20.
//--------------------------------------------------------
// Slave-to-Master mux for AMBA AXI
//--------------------------------------------------------
#include <stdio.h>
#include "gen_axi_utils.h"

//--------------------------------------------------------
int gen_axi_stom( unsigned int num // num of slaves
                , char* prefix // num of slaves for the module
                , FILE* fo)
{
    int i;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %saxi_stom_s%d\n", prefix, num);
fprintf(fo, "       #(parameter MASTER_ID   = 0 // for reference\n");
fprintf(fo, "                 , WIDTH_CID   = 4 // Channel ID width in bits\n");
fprintf(fo, "                 , WIDTH_ID    = 4 // ID width in bits\n");
fprintf(fo, "                 , WIDTH_AD    =32 // address width\n");
fprintf(fo, "                 , WIDTH_DA    =32 // data width\n");
fprintf(fo, "                 , WIDTH_DS    =(WIDTH_DA/8)  // data strobe width\n");
fprintf(fo, "                 , WIDTH_SID   =WIDTH_CID+WIDTH_ID // ID for slave\n");
fprintf(fo, "                 `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "                 , WIDTH_BUSER = 1 // Write-response user path\n");
fprintf(fo, "                 `endif\n");
fprintf(fo, "                 `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "                 , WIDTH_RUSER = 1 // read-data user path\n");
fprintf(fo, "                 `endif\n");
fprintf(fo, "        )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input   wire                      ARESETn\n");
fprintf(fo, "     , input   wire                      ACLK\n");
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     , input   wire  [WIDTH_CID-1:0]     M_MID\n");
gen_axi_mport_b("M_", "reg", fo);
gen_axi_mport_r("M_", "reg", fo);
for (i=0; i<num; i++) {
char sp[4]; sprintf(sp, "S%d_", i);
fprintf(fo, "     //--------------------------------------------------------------\n");
gen_axi_sport_b(sp, "wire", fo);
gen_axi_sport_r(sp, "wire", fo);
}
fprintf(fo, "     //--------------------------------------------------------------\n");
gen_axi_sport_b("SD_", "wire", fo);
gen_axi_sport_r("SD_", "wire", fo);
fprintf(fo, ");\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     localparam NUM=%d;\n", num);
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire [NUM:0] BSELECT, RSELECT;\n");
fprintf(fo, "     wire [NUM:0] BGRANT , RGRANT ;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign BSELECT[%d]   = (S%d_BID[WIDTH_SID-1:WIDTH_ID]==M_MID);\n", i, i);
fprintf(fo, "     assign BSELECT[%d]   = (SD_BID[WIDTH_SID-1:WIDTH_ID]==M_MID);\n", i);
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign RSELECT[%d]   = (S%d_RID[WIDTH_SID-1:WIDTH_ID]==M_MID);\n", i, i);
fprintf(fo, "     assign RSELECT[%d]   = (SD_RID[WIDTH_SID-1:WIDTH_ID]==M_MID);\n", i);
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     %saxi_arbiter_stom_s%d #(.NUM(NUM))\n", prefix, num);
fprintf(fo, "     u_axi_arbiter_stom_s%d (\n", num);
fprintf(fo, "           .ARESETn  (ARESETn)\n");
fprintf(fo, "         , .ACLK     (ACLK   )\n");
fprintf(fo, "         , .BSELECT  (BSELECT)\n");
fprintf(fo, "         , .BVALID   ({SD_BVALID");
for (i=num-1; i>=0; i--) fprintf(fo, ",S%d_BVALID", i); fprintf(fo, "})\n");
fprintf(fo, "         , .BREADY   ({SD_BREADY");
for (i=num-1; i>=0; i--) fprintf(fo, ",S%d_BREADY", i); fprintf(fo, "})\n");
fprintf(fo, "         , .BGRANT   (BGRANT )\n");
fprintf(fo, "         , .RSELECT  (RSELECT)\n");
fprintf(fo, "         , .RVALID   ({SD_RVALID");
for (i=num-1; i>=0; i--) fprintf(fo, ",S%d_RVALID", i); fprintf(fo, "})\n");
fprintf(fo, "         , .RREADY   ({SD_RREADY");
for (i=num-1; i>=0; i--) fprintf(fo, ",S%d_RREADY", i); fprintf(fo, "})\n");
fprintf(fo, "         , .RLAST    ({SD_RLAST");
for (i=num-1; i>=0; i--) fprintf(fo, ",S%d_RLAST", i); fprintf(fo, "})\n");
fprintf(fo, "         , .RGRANT   (RGRANT )\n");
fprintf(fo, "     );\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     localparam NUM_B_WIDTH = 0\n");
fprintf(fo, "                    + WIDTH_ID           //M_BID\n");
fprintf(fo, "                    +  2                 //M_BRESP\n");
fprintf(fo, "                    +  1                 //M_BVALID\n");
fprintf(fo, "                      `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "                    + WIDTH_BUSER        //M_BUSER\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                    ;\n");
fprintf(fo, "     localparam NUM_R_WIDTH = 0\n");
fprintf(fo, "                    + WIDTH_ID           //M_RID\n");
fprintf(fo, "                    + WIDTH_DA           //M_RDATA\n");
fprintf(fo, "                    +  2                 //M_RRESP\n");
fprintf(fo, "                    +  1                 //M_RLAST\n");
fprintf(fo, "                    +  1                 //M_RVALID\n");
fprintf(fo, "                      `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "                    + WIDTH_RUSER        //M_RUSER\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                    ;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire [NUM_B_WIDTH-1:0] bus_b[0:NUM];\n");
fprintf(fo, "     wire [NUM_R_WIDTH-1:0] bus_r[0:NUM];\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++) {
fprintf(fo, "     assign bus_b[%d] = {  S%d_BID[WIDTH_ID-1:0]\n", i, i);
fprintf(fo, "                         ,S%d_BRESP\n", i);
fprintf(fo, "                         ,S%d_BVALID\n", i);
fprintf(fo, "     `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "                         ,S%d_BUSER\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                       };\n");
}
fprintf(fo, "     assign bus_b[NUM] = {SD_BID[WIDTH_ID-1:0]\n");
fprintf(fo, "                         ,SD_BRESP\n");
fprintf(fo, "                         ,SD_BVALID\n");
fprintf(fo, "     `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "                         ,SD_BUSER\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "                       };\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++) {
fprintf(fo, "     assign bus_r[%d] = {  S%d_RID[WIDTH_ID-1:0]\n", i, i);
fprintf(fo, "                         ,S%d_RDATA\n", i);
fprintf(fo, "                         ,S%d_RRESP\n", i);
fprintf(fo, "                         ,S%d_RLAST\n", i);
fprintf(fo, "                         ,S%d_RVALID\n", i);
fprintf(fo, "     `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "                         ,S%d_RUSER\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                       };\n");
}
fprintf(fo, "     assign bus_r[NUM] = {SD_RID[WIDTH_ID-1:0]\n");
fprintf(fo, "                         ,SD_RDATA\n");
fprintf(fo, "                         ,SD_RRESP\n");
fprintf(fo, "                         ,SD_RLAST\n");
fprintf(fo, "                         ,SD_RVALID\n");
fprintf(fo, "     `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "                         ,SD_RUSER\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "                       };\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     `define M_BBUS {M_BID[WIDTH_ID-1:0]\\\n");
fprintf(fo, "                    ,M_BRESP\\\n");
fprintf(fo, "                    ,M_BVALID\\\n");
fprintf(fo, "                    `ifdef AMBA_AXI_BUSER\\\n");
fprintf(fo, "                    ,M_BUSER\\\n");
fprintf(fo, "                    `endif\\\n");
fprintf(fo, "                    }\n");
fprintf(fo, "     always @ ( BGRANT");
for (i=0; i<num; i++) fprintf(fo, ", bus_b[%d]", i); fprintf(fo, ", bus_b[NUM] ) begin\n");
fprintf(fo, "            `ifdef AMBA_AXI_MUX\n");
fprintf(fo, "            case (BGRANT)\n");
for (i=0; i<num; i++)
fprintf(fo, "            %d'h%X: `M_BBUS = bus_b[%d];\n", (num+1), (1<<i), i);
fprintf(fo, "            %d'h%X: `M_BBUS = bus_b[NUM];\n", (num+1), (1<<num));
fprintf(fo, "            default:    `M_BBUS = 'h0;\n");
fprintf(fo, "            endcase\n");
fprintf(fo, "            `else\n");
fprintf(fo, "            `M_BBUS = ({NUM_B_WIDTH{BGRANT[0]}}&bus_b[0])\n");
for (i=1; i<num; i++)
fprintf(fo, "                    | ({NUM_B_WIDTH{BGRANT[%d]}}&bus_b[%d])\n", i, i);
fprintf(fo, "                    | ({NUM_B_WIDTH{BGRANT[NUM]}}&bus_b[NUM]);\n");
fprintf(fo, "            `endif\n");
fprintf(fo, "     end\n");
fprintf(fo, "     `define M_RBUS {M_RID[WIDTH_ID-1:0]\\\n");
fprintf(fo, "                    ,M_RDATA\\\n");
fprintf(fo, "                    ,M_RRESP\\\n");
fprintf(fo, "                    ,M_RLAST\\\n");
fprintf(fo, "                    ,M_RVALID\\\n");
fprintf(fo, "                    `ifdef AMBA_AXI_RUSER\\\n");
fprintf(fo, "                    ,M_RUSER\\\n");
fprintf(fo, "                    `endif\\\n");
fprintf(fo, "                    }\n");
fprintf(fo, "     always @ ( RGRANT");
for (i=0; i<num; i++) fprintf(fo, ", bus_r[%d]", i); fprintf(fo, ", bus_r[NUM] ) begin\n");
fprintf(fo, "            `ifdef AMBA_AXI_MUX\n");
fprintf(fo, "            case (RGRANT)\n");
//case (RGRANT)
//5'h1:    `M_RBUS = (S0_RID[WIDTH_SID-1:WIDTH_ID]==MASTER_ID[WIDTH_CID-1:0]) ? bus_r[0]   : 'h0;
//5'h2:    `M_RBUS = (S1_RID[WIDTH_SID-1:WIDTH_ID]==MASTER_ID[WIDTH_CID-1:0]) ? bus_r[1]   : 'h0;
//5'h4:    `M_RBUS = (S2_RID[WIDTH_SID-1:WIDTH_ID]==MASTER_ID[WIDTH_CID-1:0]) ? bus_r[2]   : 'h0;
//5'h8:    `M_RBUS = (S3_RID[WIDTH_SID-1:WIDTH_ID]==MASTER_ID[WIDTH_CID-1:0]) ? bus_r[3]   : 'h0;
//5'h10:   `M_RBUS =                                                            bus_r[NUM];
//default: `M_RBUS = 'h0;
//endcase
for (i=0; i<num; i++)
fprintf(fo, "            %d'h%X: `M_RBUS = bus_r[%d];\n", num+1, 1<<i, i);
fprintf(fo, "            %d'h%X: `M_RBUS = bus_r[NUM];\n", num+1, 1<<num);
fprintf(fo, "            default:    `M_RBUS = 'h0;\n");
fprintf(fo, "            endcase\n");
fprintf(fo, "            `else\n");
fprintf(fo, "            `M_RBUS = ({NUM_R_WIDTH{RGRANT[0]}}&bus_r[0])\n");
for (i=1; i<num; i++)
fprintf(fo, "                    | ({NUM_R_WIDTH{RGRANT[%d]}}&bus_r[%d])\n", i, i);
fprintf(fo, "                    | ({NUM_R_WIDTH{RGRANT[NUM]}}&bus_r[NUM]);\n");
fprintf(fo, "            `endif\n");
fprintf(fo, "     end\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign S%d_BREADY = BGRANT[%d]&M_BREADY;\n", i, i);
fprintf(fo, "     assign SD_BREADY = BGRANT[NUM]&M_BREADY;\n");
fprintf(fo, "\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign S%d_RREADY = RGRANT[%d]&M_RREADY;\n", i, i);
fprintf(fo, "     assign SD_RREADY = RGRANT[NUM]&M_RREADY;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");
    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2018.09.20: 'WIDTH_BUSER/RUSER'
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
