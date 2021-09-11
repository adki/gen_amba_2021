//--------------------------------------------------------
// Copyright (c) 2018 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2018.09.20.
//--------------------------------------------------------
// Master-to-Slave mux for AMBA AXI
//--------------------------------------------------------
#include <stdio.h>
#include "gen_axi_utils.h"

//--------------------------------------------------------
int gen_axi_mtos( unsigned int num // num of masters
                , char *prefix
                , int   axi4
                , FILE *fo)
{
    int i;

    if ((num<2)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %saxi_mtos_m%d\n", prefix, num);
fprintf(fo, "       #(parameter SLAVE_ID     =0    // for reference\n");
fprintf(fo, "                 , SLAVE_EN     =1'b1 // the slave is available when 1\n");
fprintf(fo, "                 , WIDTH_CID    =4  // Channel ID width in bits\n");
fprintf(fo, "                 , WIDTH_ID     =4  // ID width in bits\n");
fprintf(fo, "                 , WIDTH_AD     =32 // address width\n");
fprintf(fo, "                 , WIDTH_DA     =32 // data width\n");
fprintf(fo, "                 , WIDTH_DS     =(WIDTH_DA/8)  // data strobe width\n");
fprintf(fo, "                 , WIDTH_SID    =WIDTH_CID+WIDTH_ID // ID for slave\n");
fprintf(fo, "                 `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "                 , WIDTH_AWUSER =1 // Write-address user path\n");
fprintf(fo, "                 `endif\n");
fprintf(fo, "                 `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "                 , WIDTH_WUSER  =1 // Write-data user path\n");
fprintf(fo, "                 `endif\n");
fprintf(fo, "                 `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "                 , WIDTH_ARUSER =1 // read-address user path\n");
fprintf(fo, "                 `endif\n");
fprintf(fo, "                 , NUM_MASTER   =%d    // number of master\n", num);
fprintf(fo, "                 , SLAVE_DEFAULT=1'b0  // default-salve when 1\n");
fprintf(fo, "                 , ADDR_LENGTH  =12 // effective addre bits\n");
fprintf(fo, "        ,parameter [WIDTH_AD-1:0] ADDR_BASE={WIDTH_AD{1'b0}}\n");
fprintf(fo, "        )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input   wire                      ARESETn\n");
fprintf(fo, "     , input   wire                      ACLK\n");
for (i=0; i<num; i++) {
char mp[4]; sprintf(mp, "M%d_", i);
fprintf(fo, "     //--------------------------------------------------------------\n");
fprintf(fo, "     , input   wire  [WIDTH_CID-1:0]     %sMID\n", mp);
gen_axi_mport_aw(mp, "wire", axi4, fo);
gen_axi_mport_w (mp, "wire", 0, fo);
gen_axi_mport_ar(mp, "wire", axi4, fo);
}
fprintf(fo, "     //--------------------------------------------------------------\n");
gen_axi_sport_aw("S_", "reg", axi4, fo);
gen_axi_sport_w ("S_", "reg", 0, fo);
gen_axi_sport_ar("S_", "reg", axi4, fo);
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     , output  wire  [NUM_MASTER-1:0]    AWSELECT_OUT\n");
fprintf(fo, "     , output  wire  [NUM_MASTER-1:0]    ARSELECT_OUT\n");
fprintf(fo, "     , input   wire  [NUM_MASTER-1:0]    AWSELECT_IN\n");
fprintf(fo, "     , input   wire  [NUM_MASTER-1:0]    ARSELECT_IN\n");
fprintf(fo, ");\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     reg  [NUM_MASTER-1:0] AWSELECT;\n");
fprintf(fo, "     reg  [NUM_MASTER-1:0] ARSELECT;\n");
fprintf(fo, "     wire [NUM_MASTER-1:0] AWGRANT, WGRANT, ARGRANT;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     assign  AWSELECT_OUT = AWSELECT;\n");
fprintf(fo, "     assign  ARSELECT_OUT = ARSELECT;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     always @ ( * ) begin\n");
fprintf(fo, "        if (SLAVE_DEFAULT=='h0) begin\n");
for (i=0; i<num; i++)
fprintf(fo, "            AWSELECT[%d] = SLAVE_EN[0]&(M%d_AWADDR[WIDTH_AD-1:ADDR_LENGTH]==ADDR_BASE[WIDTH_AD-1:ADDR_LENGTH]);\n", i, i);
fprintf(fo, "\n");
for (i=0; i<num; i++)
fprintf(fo, "            ARSELECT[%d] = SLAVE_EN[0]&(M%d_ARADDR[WIDTH_AD-1:ADDR_LENGTH]==ADDR_BASE[WIDTH_AD-1:ADDR_LENGTH]);\n", i, i);
fprintf(fo, "        end else begin\n");
fprintf(fo, "            AWSELECT = ~AWSELECT_IN & {M%d_AWVALID", num-1);
for (i=num-2; i>=0; i--) fprintf(fo, ",M%d_AWVALID", i); fprintf(fo, "};\n");
fprintf(fo, "            ARSELECT = ~ARSELECT_IN & {M%d_ARVALID", num-1);
for (i=num-2; i>=0; i--) fprintf(fo, ",M%d_ARVALID", i); fprintf(fo, "};\n");
fprintf(fo, "        end\n");
fprintf(fo, "     end\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
#define allX(A,B,N)\
fprintf(fo, "     wire [NUM_MASTER-1:0] %s_ALL = {M%d_%s", (A), (N)-1, (B));\
for (i=num-2; i>=0; i--) fprintf(fo, ",M%d_%s", i, (B)); fprintf(fo, "};\n");

allX("AWVALID", "AWVALID", num)
allX("AWREADY", "AWREADY", num)
if (axi4) {
allX("AWLOCK", "AWLOCK", num)
} else {
allX("AWLOCK", "AWLOCK[1]", num)
}
allX("ARVALID", "ARVALID", num)
allX("ARREADY", "ARREADY", num)
if (axi4) {
allX("ARLOCK", "ARLOCK", num)
} else {
allX("ARLOCK", "ARLOCK[1]", num)
}
allX("WVALID", "WVALID", num)
allX("WREADY", "WREADY", num)
allX("WLAST", "WLAST", num)
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     %saxi_arbiter_mtos_m%d #(.WIDTH_CID(WIDTH_CID) // Channel ID width in bits\n", prefix, num);
fprintf(fo, "                          ,.WIDTH_ID (WIDTH_ID ) // Transaction ID\n");
fprintf(fo, "                          )\n");
fprintf(fo, "     u_axi_arbiter_mtos_m%d (\n", num);
fprintf(fo, "           .ARESETn  (ARESETn           )\n");
fprintf(fo, "         , .ACLK     (ACLK              )\n");
fprintf(fo, "         , .AWSELECT (AWSELECT          )\n");
fprintf(fo, "         , .AWVALID  (AWVALID_ALL       )\n");
fprintf(fo, "         , .AWREADY  (AWREADY_ALL       )\n");
fprintf(fo, "         , .AWLOCK   (AWLOCK_ALL        )\n");
fprintf(fo, "         , .AWGRANT  (AWGRANT           )\n");
for (i=0; i<num; i++)
fprintf(fo, "         , .AWSID%d  ({M%d_MID,M%d_AWID}  )\n", i, i, i);
fprintf(fo, "         , .WVALID   (WVALID_ALL        )\n");
fprintf(fo, "         , .WREADY   (WREADY_ALL        )\n");
fprintf(fo, "         , .WLAST    (WLAST_ALL         )\n");
fprintf(fo, "         , .WGRANT   (WGRANT            )\n");
for (i=0; i<num; i++)
fprintf(fo, "         , .WSID%d    ({M%d_MID,M%d_WID}   )\n", i, i, i);
fprintf(fo, "         , .ARSELECT (ARSELECT          )\n");
fprintf(fo, "         , .ARVALID  (ARVALID_ALL       )\n");
fprintf(fo, "         , .ARREADY  (ARREADY_ALL       )\n");
fprintf(fo, "         , .ARLOCK   (ARLOCK_ALL        )\n");
fprintf(fo, "         , .ARGRANT  (ARGRANT           )\n");
for (i=0; i<num; i++)
fprintf(fo, "         , .ARSID%d   ({M%d_MID,M%d_ARID}  )\n", i, i, i);
for (i=0; i<num; i++)
fprintf(fo, "         , .MID%d     (M%d_MID            )\n", i, i);
fprintf(fo, "     );\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     localparam NUM_AW_WIDTH = 0\n");
fprintf(fo, "                    + WIDTH_SID          //S_AWID\n");
fprintf(fo, "                    + WIDTH_AD           //S_AWADDR\n");
if (axi4) {
fprintf(fo, "                    +  8                 //S_AWLEN\n");
fprintf(fo, "                    +  1                 //S_AWLOCK\n");
} else {
fprintf(fo, "                    +  4                 //S_AWLEN\n");
fprintf(fo, "                    +  2                 //S_AWLOCK\n");
}
fprintf(fo, "                    +  3                 //S_AWSIZE\n");
fprintf(fo, "                    +  2                 //S_AWBURST\n");
fprintf(fo, "                       `ifdef  AMBA_AXI_CACHE\n");
fprintf(fo, "                    +  4                 //S_AWCACHE\n");
fprintf(fo, "                       `endif\n");
fprintf(fo, "                       `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "                    +  3                 //S_AWPROT\n");
fprintf(fo, "                       `endif\n");
fprintf(fo, "                    +  1                 //S_AWVALID\n");
fprintf(fo, "                      `ifdef AMBA_QOS\n");
fprintf(fo, "                    +  4                 //S_AWQOS\n");
fprintf(fo, "                    +  4                 //S_AWREGION\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                      `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "                    + WIDTH_AWUSER       //S_AWUSER\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                    ;\n");
fprintf(fo, "     localparam NUM_W_WIDTH = 0\n");
fprintf(fo, "                    + WIDTH_SID          //S_WID\n");
fprintf(fo, "                    + WIDTH_DA           //S_WDATA\n");
fprintf(fo, "                    + WIDTH_DS           //S_WSTRB\n");
fprintf(fo, "                    + 1                  //S_WLAST\n");
fprintf(fo, "                    + 1                  //S_WVALID\n");
fprintf(fo, "                      `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "                    + WIDTH_WUSER        //S_WUSER\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                    ;\n");
fprintf(fo, "     localparam NUM_AR_WIDTH = 0\n");
fprintf(fo, "                    + WIDTH_SID          //S_ARID\n");
fprintf(fo, "                    + WIDTH_AD           //S_ARADDR\n");
if (axi4) {
fprintf(fo, "                    +  8                 //S_ARLEN\n");
fprintf(fo, "                    +  1                 //S_ARLOCK\n");
} else {
fprintf(fo, "                    +  4                 //S_ARLEN\n");
fprintf(fo, "                    +  2                 //S_ARLOCK\n");
}
fprintf(fo, "                    +  3                 //S_ARSIZE\n");
fprintf(fo, "                    +  2                 //S_ARBURST\n");
fprintf(fo, "                       `ifdef  AMBA_AXI_CACHE\n");
fprintf(fo, "                    +  4                 //S_ARCACHE\n");
fprintf(fo, "                       `endif\n");
fprintf(fo, "                       `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "                    +  3                 //S_ARPROT\n");
fprintf(fo, "                       `endif\n");
fprintf(fo, "                    +  1                 //S_ARVALID\n");
fprintf(fo, "                      `ifdef AMBA_QOS\n");
fprintf(fo, "                    +  4                 //S_ARQOS\n");
fprintf(fo, "                    +  4                 //S_ARREGION\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                      `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "                    + WIDTH_ARUSER       //S_ARUSER\n");
fprintf(fo, "                      `endif\n");
fprintf(fo, "                    ;\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire [NUM_AW_WIDTH-1:0] bus_aw[0:NUM_MASTER-1];\n");
fprintf(fo, "     wire [NUM_W_WIDTH-1 :0] bus_w [0:NUM_MASTER-1];\n");
fprintf(fo, "     wire [NUM_AR_WIDTH-1:0] bus_ar[0:NUM_MASTER-1];\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign M%d_AWREADY = AWGRANT[%d]&S_AWREADY;\n", i, i);
fprintf(fo, "\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign M%d_WREADY  = WGRANT [%d]&S_WREADY;\n", i, i);
fprintf(fo, "\n");
for (i=0; i<num; i++)
fprintf(fo, "     assign M%d_ARREADY = ARGRANT[%d]&S_ARREADY;\n", i, i);
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++) {
fprintf(fo, "     assign bus_aw[%d] = {  M%d_MID // master %d master channel id\n", i, i, i);
fprintf(fo, "                          ,M%d_AWID\n", i);
fprintf(fo, "                          ,M%d_AWADDR\n", i);
fprintf(fo, "                          ,M%d_AWLEN\n", i);
fprintf(fo, "                          ,M%d_AWLOCK\n", i);
fprintf(fo, "                          ,M%d_AWSIZE\n", i);
fprintf(fo, "                          ,M%d_AWBURST\n", i);
fprintf(fo, "     `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "                          ,M%d_AWCACHE\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "                          ,M%d_AWPROT\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                          ,M%d_AWVALID\n", i);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "                          ,M%d_AWQOS\n", i);
fprintf(fo, "                          ,M%d_AWREGION\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "                          ,M%d_AWUSER\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                        };\n");
}
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++) {
fprintf(fo, "     assign bus_w[%d]  = {  M%d_MID\n", i, i);
fprintf(fo, "                          ,M%d_WID\n", i);
fprintf(fo, "                          ,M%d_WDATA\n", i);
fprintf(fo, "                          ,M%d_WSTRB\n", i);
fprintf(fo, "                          ,M%d_WLAST\n", i);
fprintf(fo, "                          ,M%d_WVALID\n", i);
fprintf(fo, "     `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "                          ,M%d_WUSER\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                        };\n");
}
fprintf(fo, "     //-----------------------------------------------------------\n");
for (i=0; i<num; i++) {
fprintf(fo, "     assign bus_ar[%d] = {  M%d_MID\n", i, i);
fprintf(fo, "                          ,M%d_ARID\n", i);
fprintf(fo, "                          ,M%d_ARADDR\n", i);
fprintf(fo, "                          ,M%d_ARLEN\n", i);
fprintf(fo, "                          ,M%d_ARLOCK\n", i);
fprintf(fo, "                          ,M%d_ARSIZE\n", i);
fprintf(fo, "                          ,M%d_ARBURST\n", i);
fprintf(fo, "     `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "                          ,M%d_ARCACHE\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "                          ,M%d_ARPROT\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                          ,M%d_ARVALID\n", i);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "                          ,M%d_ARQOS\n", i);
fprintf(fo, "                          ,M%d_ARREGION\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "                          ,M%d_ARUSER\n", i);
fprintf(fo, "     `endif\n");
fprintf(fo, "                        };\n");
}
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     `define S_AWBUS {S_AWID\\\n");
fprintf(fo, "                     ,S_AWADDR\\\n");
fprintf(fo, "                     ,S_AWLEN\\\n");
fprintf(fo, "                     ,S_AWLOCK\\\n");
fprintf(fo, "                     ,S_AWSIZE\\\n");
fprintf(fo, "                     ,S_AWBURST\\\n");
fprintf(fo, "                     `ifdef AMBA_AXI_CACHE\\\n");
fprintf(fo, "                     ,S_AWCACHE\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     `ifdef AMBA_AXI_PROT\\\n");
fprintf(fo, "                     ,S_AWPROT\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     ,S_AWVALID\\\n");
fprintf(fo, "                     `ifdef AMBA_QOS\\\n");
fprintf(fo, "                     ,S_AWQOS\\\n");
fprintf(fo, "                     ,S_AWREGION\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     `ifdef AMBA_AXI_AWUSER\\\n");
fprintf(fo, "                     ,S_AWUSER\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     }\n");
fprintf(fo, "     always @ ( AWGRANT");
for (i=0; i<num; i++) fprintf(fo, ", bus_aw[%d]", i); fprintf(fo, " ) begin\n");
fprintf(fo, "            `ifdef AMBA_AXI_MUX\n");
fprintf(fo, "            case (AWGRANT)\n");
for (i=0; i<num; i++)
fprintf(fo, "            %d'h%X:  `S_AWBUS = bus_aw[%d];\n", num, 1<<i, i);
fprintf(fo, "            default:    `S_AWBUS = 'h0;\n");
fprintf(fo, "            endcase\n");
fprintf(fo, "            `else\n");
fprintf(fo, "            `S_AWBUS = ({NUM_AW_WIDTH{AWGRANT[0]}}&bus_aw[0])\n");
for (i=1; i<num; i++)
fprintf(fo, "                     | ({NUM_AW_WIDTH{AWGRANT[%d]}}&bus_aw[%d])\n", i, i);
fprintf(fo, "                     ;\n");
fprintf(fo, "            `endif\n");
fprintf(fo, "     end\n");
fprintf(fo, "     `define S_WBUS {S_WID\\\n");
fprintf(fo, "                    ,S_WDATA\\\n");
fprintf(fo, "                    ,S_WSTRB\\\n");
fprintf(fo, "                    ,S_WLAST\\\n");
fprintf(fo, "                    ,S_WVALID\\\n");
fprintf(fo, "                    `ifdef AMBA_AXI_WUSER\\\n");
fprintf(fo, "                    ,S_WUSER\\\n");
fprintf(fo, "                    `endif\\\n");
fprintf(fo, "                    }\n");
fprintf(fo, "     always @ ( WGRANT");
for (i=0; i<num; i++) fprintf(fo, ", bus_w[%d]", i); fprintf(fo, " ) begin\n");
fprintf(fo, "            `ifdef AMBA_AXI_MUX\n");
fprintf(fo, "            case (WGRANT)\n");
for (i=0; i<num; i++)
fprintf(fo, "            %d'h%X:  `S_WBUS = bus_w[%d];\n", num, 1<<i, i);
fprintf(fo, "            default:    `S_WBUS = 'h0;\n");
fprintf(fo, "            endcase\n");
fprintf(fo, "            `else\n");
fprintf(fo, "            `S_WBUS = ({NUM_W_WIDTH{WGRANT[0]}}&bus_w[0])\n");
for (i=1; i<num; i++)
fprintf(fo, "                    | ({NUM_W_WIDTH{WGRANT[%d]}}&bus_w[%d])\n", i, i);
fprintf(fo, "                    ;\n");
fprintf(fo, "            `endif\n");
fprintf(fo, "     end\n");
fprintf(fo, "     `define S_ARBUS {S_ARID\\\n");
fprintf(fo, "                     ,S_ARADDR\\\n");
fprintf(fo, "                     ,S_ARLEN\\\n");
fprintf(fo, "                     ,S_ARLOCK\\\n");
fprintf(fo, "                     ,S_ARSIZE\\\n");
fprintf(fo, "                     ,S_ARBURST\\\n");
fprintf(fo, "                     `ifdef AMBA_AXI_CACHE\\\n");
fprintf(fo, "                     ,S_ARCACHE\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     `ifdef AMBA_AXI_PROT\\\n");
fprintf(fo, "                     ,S_ARPROT\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     ,S_ARVALID\\\n");
fprintf(fo, "                     `ifdef AMBA_QOS\\\n");
fprintf(fo, "                     ,S_ARQOS\\\n");
fprintf(fo, "                     ,S_ARREGION\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     `ifdef AMBA_AXI_ARUSER\\\n");
fprintf(fo, "                     ,S_ARUSER\\\n");
fprintf(fo, "                     `endif\\\n");
fprintf(fo, "                     }\n");
fprintf(fo, "     always @ ( ARGRANT");
for (i=0; i<num; i++) fprintf(fo, ", bus_ar[%d]", i); fprintf(fo, " ) begin\n");
fprintf(fo, "            `ifdef AMBA_AXI_MUX\n");
fprintf(fo, "            case (ARGRANT)\n");
for (i=0; i<num; i++)
fprintf(fo, "            %d'h%X:  `S_ARBUS = bus_ar[%d];\n", num, 1<<i, i);
fprintf(fo, "            default:    `S_ARBUS = 'h0;\n");
fprintf(fo, "            endcase\n");
fprintf(fo, "            `else\n");
fprintf(fo, "            `S_ARBUS = ({NUM_AR_WIDTH{ARGRANT[0]}}&bus_ar[0])\n");
for (i=1; i<num; i++)
fprintf(fo, "                     | ({NUM_AR_WIDTH{ARGRANT[%d]}}&bus_ar[%d])\n", i, i);
fprintf(fo, "                     ;\n");
fprintf(fo, "            `endif\n");
fprintf(fo, "     end\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "endmodule\n");
fprintf(fo, "//---------------------------------------------------------------------------\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2018.09.20: 'WIDTH_ARUSER/WUSER/ARUSER'
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
