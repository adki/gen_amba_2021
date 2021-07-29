//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// AXI-to-APB bridge
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//--------------------------------------------------------
int gen_axi2apb( unsigned int num, char* module, char* prefix, int axi4, FILE* fo )
{
    int i=0;
    unsigned int start=0xC0000000;

    if ((num<2)||(module==NULL)||(prefix==NULL)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "module %s\n", module);
fprintf(fo, "     #(parameter AXI_WIDTH_SID = 4 // ID width in bits\n");
fprintf(fo, "               , AXI_WIDTH_AD  =32 // address width\n");
fprintf(fo, "               , AXI_WIDTH_DA  =32 // data width\n");
fprintf(fo, "               , AXI_WIDTH_DS  =(AXI_WIDTH_DA/8) // data strobe width\n");
fprintf(fo, "               , NUM_PSLAVE    =%d\n", num);
fprintf(fo, "               , WIDTH_PAD     =32 // address width\n");
fprintf(fo, "               , WIDTH_PDA     =32 // data width\n");
fprintf(fo, "               , WIDTH_PDS     =(WIDTH_PDA/8) // data strobe width\n");
for (i=0; i<num; i++) {
fprintf(fo, "               , ADDR_PBASE%d  =32'h%08X, ADDR_PLENGTH%d=16\n", i, start, i);
start += 0x1000;
}
fprintf(fo, "               , CLOCK_RATIO  =2'b00// 0=1:1, 3=async\n");
fprintf(fo, "      )\n");
fprintf(fo, "(\n");
fprintf(fo, "       input  wire                     ARESETn\n");
fprintf(fo, "     , input  wire                     ACLK\n");
fprintf(fo, "     , input  wire [AXI_WIDTH_SID-1:0] AWID\n");
fprintf(fo, "     , input  wire [AXI_WIDTH_AD-1:0]  AWADDR\n");
if (axi4) {
fprintf(fo, "     , input  wire [ 7:0]              AWLEN\n");
fprintf(fo, "     , input  wire                     AWLOCK\n");
} else {
fprintf(fo, "     , input  wire [ 3:0]              AWLEN\n");
fprintf(fo, "     , input  wire [ 1:0]              AWLOCK\n");
}
fprintf(fo, "     , input  wire [ 2:0]              AWSIZE\n");
fprintf(fo, "     , input  wire [ 1:0]              AWBURST\n");
fprintf(fo, "     `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "     , input  wire [ 3:0]              AWCACHE\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     , input  wire [ 2:0]              AWPROT\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     , input  wire                     AWVALID\n");
fprintf(fo, "     , output wire                     AWREADY\n");
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     , input  wire [ 3:0]              AWQOS\n");
fprintf(fo, "     , input  wire [ 3:0]              AWREGION\n");
fprintf(fo, "     `endif\n");
if (axi4==0) {
fprintf(fo, "     , input  wire [AXI_WIDTH_SID-1:0] WID\n");
}
fprintf(fo, "     , input  wire [AXI_WIDTH_DA-1:0]  WDATA\n");
fprintf(fo, "     , input  wire [AXI_WIDTH_DS-1:0]  WSTRB\n");
fprintf(fo, "     , input  wire                     WLAST\n");
fprintf(fo, "     , input  wire                     WVALID\n");
fprintf(fo, "     , output wire                     WREADY\n");
fprintf(fo, "     , output wire [AXI_WIDTH_SID-1:0] BID\n");
fprintf(fo, "     , output wire [ 1:0]              BRESP\n");
fprintf(fo, "     , output wire                     BVALID\n");
fprintf(fo, "     , input  wire                     BREADY\n");
fprintf(fo, "     , input  wire [AXI_WIDTH_SID-1:0] ARID\n");
fprintf(fo, "     , input  wire [AXI_WIDTH_AD-1:0]  ARADDR\n");
if (axi4) {
fprintf(fo, "     , input  wire [ 7:0]              ARLEN\n");
fprintf(fo, "     , input  wire                     ARLOCK\n");
} else {
fprintf(fo, "     , input  wire [ 3:0]              ARLEN\n");
fprintf(fo, "     , input  wire [ 1:0]              ARLOCK\n");
}
fprintf(fo, "     , input  wire [ 2:0]              ARSIZE\n");
fprintf(fo, "     , input  wire [ 1:0]              ARBURST\n");
fprintf(fo, "     `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "     , input  wire [ 3:0]              ARCACHE\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     , input  wire [ 2:0]              ARPROT\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     , input  wire                     ARVALID\n");
fprintf(fo, "     , output wire                     ARREADY\n");
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     , input  wire [ 3:0]              ARQOS\n");
fprintf(fo, "     , input  wire [ 3:0]              ARREGION\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     , output wire [AXI_WIDTH_SID-1:0] RID\n");
fprintf(fo, "     , output wire [AXI_WIDTH_DA-1:0]  RDATA\n");
fprintf(fo, "     , output wire [ 1:0]              RRESP\n");
fprintf(fo, "     , output wire                     RLAST\n");
fprintf(fo, "     , output wire                     RVALID\n");
fprintf(fo, "     , input  wire                     RREADY\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     , input  wire                     PRESETn\n");
fprintf(fo, "     , input  wire                     PCLK\n");
fprintf(fo, "     , output wire [WIDTH_PAD-1:0]     S_PADDR\n");
fprintf(fo, "     , output wire                     S_PENABLE\n");
fprintf(fo, "     , output wire                     S_PWRITE\n");
fprintf(fo, "     , output wire [WIDTH_PDA-1:0]     S_PWDATA\n");
for (i=0; i<num; i++) {
fprintf(fo, "     , output wire                     S%d_PSEL\n", i);
fprintf(fo, "     , input  wire [WIDTH_PDA-1:0]     S%d_PRDATA\n", i);
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     , input  wire                     S%d_PREADY\n", i);
fprintf(fo, "     , input  wire                     S%d_PSLVERR\n", i);
fprintf(fo, "     `endif\n");
}
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     , output wire [WIDTH_PDS-1:0]     S_PSTRB\n");
fprintf(fo, "     , output wire [ 2:0]              S_PPROT\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, ");\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     wire        M_PSEL   ;\n");
fprintf(fo, "     wire        M_PENABLE;\n");
fprintf(fo, "     wire [31:0] M_PADDR  ;\n");
fprintf(fo, "     wire        M_PWRITE ;\n");
fprintf(fo, "     wire [31:0] M_PRDATA ;\n");
fprintf(fo, "     wire [31:0] M_PWDATA ;\n");
fprintf(fo, "     `ifdef AMBA_APB3\n");
fprintf(fo, "     wire        M_PREADY ;\n");
fprintf(fo, "     wire        M_PSLVERR;\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_APB4\n");
fprintf(fo, "     wire [ 2:0] M_PPROT  ;\n");
fprintf(fo, "     wire [ 3:0] M_PSTRB  ;\n");
fprintf(fo, "     `endif\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     %saxi_to_apb_bridge #(.AXI_WIDTH_SID(AXI_WIDTH_SID)// ID width in bits\n", prefix);
fprintf(fo, "                        ,.AXI_WIDTH_AD (AXI_WIDTH_AD )// address width\n");
fprintf(fo, "                        ,.AXI_WIDTH_DA (AXI_WIDTH_DA )// data width\n");
fprintf(fo, "                        ,.APB_WIDTH_PAD(WIDTH_PAD    )// APB address width\n");
fprintf(fo, "                        ,.CLOCK_RATIO  (CLOCK_RATIO  )\n");
fprintf(fo, "                        )\n");
fprintf(fo, "     u_bridge (\n");
fprintf(fo, "            .ARESETn            (ARESETn    )\n");
fprintf(fo, "          , .ACLK               (ACLK       )\n");
fprintf(fo, "          , .AWID               (AWID       )\n");
fprintf(fo, "          , .AWADDR             (AWADDR     )\n");
fprintf(fo, "          , .AWLEN              (AWLEN      )\n");
fprintf(fo, "          , .AWLOCK             (AWLOCK     )\n");
fprintf(fo, "          , .AWSIZE             (AWSIZE     )\n");
fprintf(fo, "          , .AWBURST            (AWBURST    )\n");
fprintf(fo, "          `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "          , .AWCACHE            (AWCACHE    )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "          `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "          , .AWPROT             (AWPROT     )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "          , .AWVALID            (AWVALID    )\n");
fprintf(fo, "          , .AWREADY            (AWREADY    )\n");
fprintf(fo, "          `ifdef AMBA_QOS\n");
fprintf(fo, "          , .AWQOS              (AWQOS      )\n");
fprintf(fo, "          , .AWREGION           (AWREGION   )\n");
fprintf(fo, "          `endif\n");
if (axi4=0) {
fprintf(fo, "          , .WID                (WID        )\n");
}
fprintf(fo, "          , .WDATA              (WDATA      )\n");
fprintf(fo, "          , .WSTRB              (WSTRB      )\n");
fprintf(fo, "          , .WLAST              (WLAST      )\n");
fprintf(fo, "          , .WVALID             (WVALID     )\n");
fprintf(fo, "          , .WREADY             (WREADY     )\n");
fprintf(fo, "          , .BID                (BID        )\n");
fprintf(fo, "          , .BRESP              (BRESP      )\n");
fprintf(fo, "          , .BVALID             (BVALID     )\n");
fprintf(fo, "          , .BREADY             (BREADY     )\n");
fprintf(fo, "          , .ARID               (ARID       )\n");
fprintf(fo, "          , .ARADDR             (ARADDR     )\n");
fprintf(fo, "          , .ARLEN              (ARLEN      )\n");
fprintf(fo, "          , .ARLOCK             (ARLOCK     )\n");
fprintf(fo, "          , .ARSIZE             (ARSIZE     )\n");
fprintf(fo, "          , .ARBURST            (ARBURST    )\n");
fprintf(fo, "          `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "          , .ARCACHE            (ARCACHE    )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "          `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "          , .ARPROT             (ARPROT     )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "          , .ARVALID            (ARVALID    )\n");
fprintf(fo, "          , .ARREADY            (ARREADY    )\n");
fprintf(fo, "          `ifdef AMBA_QOS\n");
fprintf(fo, "          , .ARQOS              (ARQOS      )\n");
fprintf(fo, "          , .ARREGION           (ARREGION   )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "          , .RID                (RID        )\n");
fprintf(fo, "          , .RDATA              (RDATA      )\n");
fprintf(fo, "          , .RRESP              (RRESP      )\n");
fprintf(fo, "          , .RLAST              (RLAST      )\n");
fprintf(fo, "          , .RVALID             (RVALID     )\n");
fprintf(fo, "          , .RREADY             (RREADY     )\n");
fprintf(fo, "\n");
fprintf(fo, "          , .PCLK               (PCLK       )\n");
fprintf(fo, "          , .PRESETn            (PRESETn    )\n");
fprintf(fo, "          , .M_PSEL             (M_PSEL     )\n");
fprintf(fo, "          , .M_PENABLE          (M_PENABLE  )\n");
fprintf(fo, "          , .M_PADDR            (M_PADDR    )\n");
fprintf(fo, "          , .M_PWRITE           (M_PWRITE   )\n");
fprintf(fo, "          , .M_PRDATA           (M_PRDATA   )\n");
fprintf(fo, "          , .M_PWDATA           (M_PWDATA   )\n");
fprintf(fo, "          `ifdef AMBA_APB3\n");
fprintf(fo, "          , .M_PREADY           (M_PREADY   )\n");
fprintf(fo, "          , .M_PSLVERR          (M_PSLVERR  )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "          `ifdef AMBA_APB4\n");
fprintf(fo, "          , .M_PPROT            (M_PPROT    )\n");
fprintf(fo, "          , .M_PSTRB            (M_PSTRB    )\n");
fprintf(fo, "          `endif\n");
fprintf(fo, "\n");
fprintf(fo, "     );\n");
fprintf(fo, "     //-----------------------------------------------------------\n");
fprintf(fo, "     %samba_apb_s%d #(.P_NUM(%d)// num of slaves\n", prefix, num, num);
for (i=0; i<num; i++) {
fprintf(fo, "                  ,.P_PSEL%d_START(ADDR_PBASE%d),.P_PSEL%d_SIZE(1<<ADDR_PLENGTH%d)\n", i, i, i, i);
}
fprintf(fo, "                  )\n");
fprintf(fo, "     u_amba_apb_s%d (\n", num);
fprintf(fo, "         .PRESETn       (PRESETn    )\n");
fprintf(fo, "       , .PCLK          (PCLK       )\n");
fprintf(fo, "       , .M_PSEL        (M_PSEL     )\n");
fprintf(fo, "       , .M_PENABLE     (M_PENABLE  )\n");
fprintf(fo, "       , .M_PADDR       (M_PADDR    )\n");
fprintf(fo, "       , .M_PWRITE      (M_PWRITE   )\n");
fprintf(fo, "       , .M_PWDATA      (M_PWDATA   )\n");
fprintf(fo, "       , .M_PRDATA      (M_PRDATA   )\n");
fprintf(fo, "       `ifdef AMBA_APB3\n");
fprintf(fo, "       , .M_PREADY      (M_PREADY   )\n");
fprintf(fo, "       , .M_PSLVERR     (M_PSLVERR  )\n");
fprintf(fo, "       `endif\n");
fprintf(fo, "       `ifdef AMBA_APB4\n");
fprintf(fo, "       , .M_PPROT       (M_PPROT    )\n");
fprintf(fo, "       , .M_PSTRB       (M_PSTRB    )\n");
fprintf(fo, "       `endif\n");
fprintf(fo, "       , .S_PADDR       (S_PADDR      )\n");
fprintf(fo, "       , .S_PENABLE     (S_PENABLE    )\n");
fprintf(fo, "       , .S_PWRITE      (S_PWRITE     )\n");
fprintf(fo, "       , .S_PWDATA      (S_PWDATA     )\n");
fprintf(fo, "       `ifdef AMBA_APB4\n");
fprintf(fo, "       , .S_PPROT       (S_PPROT      )\n");
fprintf(fo, "       , .S_PSTRB       (S_PSTRB      )\n");
fprintf(fo, "       `endif\n");
for (i=0; i<num; i++) {
fprintf(fo, "       , .S_PSEL%d      (S%d_PSEL     )\n", i, i);
fprintf(fo, "       , .S_PRDATA%d    (S%d_PRDATA   )\n", i, i);
fprintf(fo, "       `ifdef AMBA_APB3\n");
fprintf(fo, "       , .S_PREADY%d    (S%d_PREADY   )\n", i, i);
fprintf(fo, "       , .S_PSLVERR%d   (S%d_PSLVERR  )\n", i, i);
fprintf(fo, "       `endif\n");
}
fprintf(fo, "     );\n");
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
