//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki.
// All right reserved.
//
// adki@future-ds.com
// andoki@gmail.com
//--------------------------------------------------------
// VERSION: 2016.03.26.
//--------------------------------------------------------
// AMBA AXI utils
//--------------------------------------------------------
#include <stdio.h>
#include "gen_axi_utils.h"

//--------------------------------------------------------
int gen_axi_mport( char* prefix  // prefix (M_, M0_, )
                 , char* otype // output type (wire or reg)
                 , int   axi4
                 , FILE* fo)
{
    int ret=0;

    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , input   wire  [WIDTH_CID-1:0]     %sMID\n", prefix);
    ret += gen_axi_mport_aw( prefix, otype, axi4, fo );
    ret += gen_axi_mport_w ( prefix, otype, axi4, fo );
    ret += gen_axi_mport_b ( prefix, otype, fo );
    ret += gen_axi_mport_ar( prefix, otype, axi4, fo );
    ret += gen_axi_mport_r ( prefix, otype, fo );

    return ret;
}

//--------------------------------------------------------
int gen_axi_mport_aw( char* prefix  // prefix (M_, M0_, )
                    , char* otype // output type (wire or reg)
                    , int   axi4
                    , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , input   wire  [WIDTH_ID-1:0]      %sAWID\n", prefix);
fprintf(fo, "     , input   wire  [WIDTH_AD-1:0]      %sAWADDR\n", prefix);
if (axi4) {
fprintf(fo, "     , input   wire  [ 7:0]              %sAWLEN\n", prefix);
fprintf(fo, "     , input   wire                      %sAWLOCK\n", prefix);
} else {
fprintf(fo, "     , input   wire  [ 3:0]              %sAWLEN\n", prefix);
fprintf(fo, "     , input   wire  [ 1:0]              %sAWLOCK\n", prefix);
}
fprintf(fo, "     , input   wire  [ 2:0]              %sAWSIZE\n", prefix);
fprintf(fo, "     , input   wire  [ 1:0]              %sAWBURST\n", prefix);
fprintf(fo, "     `ifdef  AMBA_AXI_CACHE\n");
fprintf(fo, "     , input   wire  [ 3:0]              %sAWCACHE\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     , input   wire  [ 2:0]              %sAWPROT\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     , input   wire                      %sAWVALID\n", prefix);
fprintf(fo, "     , output  %-4s                      %sAWREADY\n", otype, prefix);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     , input   wire  [ 3:0]              %sAWQOS\n", prefix);
fprintf(fo, "     , input   wire  [ 3:0]              %sAWREGION\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "     , input   wire  [WIDTH_AWUSER-1:0]  %sAWUSER\n", prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_mport_w( char* prefix  // prefix (M_, M0_, )
                   , char* otype // output type (wire or reg)
                   , int   axi4
                   , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

if (axi4==0) {
fprintf(fo, "     , input   wire  [WIDTH_ID-1:0]      %sWID\n", prefix);
}
fprintf(fo, "     , input   wire  [WIDTH_DA-1:0]      %sWDATA\n", prefix);
fprintf(fo, "     , input   wire  [WIDTH_DS-1:0]      %sWSTRB\n", prefix);
fprintf(fo, "     , input   wire                      %sWLAST\n", prefix);
fprintf(fo, "     , input   wire                      %sWVALID\n", prefix);
fprintf(fo, "     , output  %-4s                      %sWREADY\n", otype, prefix);
fprintf(fo, "     `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "     , input   wire  [WIDTH_WUSER-1:0]   %sWUSER\n", prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_mport_ar( char* prefix  // prefix (M_, M0_, )
                    , char* otype // output type (wire or reg)
                    , int   axi4
                    , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , input   wire  [WIDTH_ID-1:0]      %sARID\n", prefix);
fprintf(fo, "     , input   wire  [WIDTH_AD-1:0]      %sARADDR\n", prefix);
if (axi4) {
fprintf(fo, "     , input   wire  [ 7:0]              %sARLEN\n", prefix);
fprintf(fo, "     , input   wire                      %sARLOCK\n", prefix);
} else {
fprintf(fo, "     , input   wire  [ 3:0]              %sARLEN\n", prefix);
fprintf(fo, "     , input   wire  [ 1:0]              %sARLOCK\n", prefix);
}
fprintf(fo, "     , input   wire  [ 2:0]              %sARSIZE\n", prefix);
fprintf(fo, "     , input   wire  [ 1:0]              %sARBURST\n", prefix);
fprintf(fo, "     `ifdef  AMBA_AXI_CACHE\n");
fprintf(fo, "     , input   wire  [ 3:0]              %sARCACHE\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     , input   wire  [ 2:0]              %sARPROT\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     , input   wire                      %sARVALID\n", prefix);
fprintf(fo, "     , output  %-4s                      %sARREADY\n", otype, prefix);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     , input   wire  [ 3:0]              %sARQOS\n", prefix);
fprintf(fo, "     , input   wire  [ 3:0]              %sARREGION\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "     , input   wire  [WIDTH_ARUSER-1:0]  %sARUSER\n", prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_mport_b( char* prefix  // prefix (M_, M0_, )
                   , char* otype // output type (wire or reg)
                   , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , output  %-4s  [WIDTH_ID-1:0]      %sBID\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [ 1:0]              %sBRESP\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sBVALID\n", otype, prefix);
fprintf(fo, "     , input   wire                      %sBREADY\n", prefix);
fprintf(fo, "     `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "     , output  %-4s  [WIDTH_BUSER-1:0]   %sBUSER\n", otype, prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_mport_r( char* prefix  // prefix (M_, M0_, )
                   , char* otype // output type (wire or reg)
                   , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , output  %-4s  [WIDTH_ID-1:0]      %sRID\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [WIDTH_DA-1:0]      %sRDATA\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [ 1:0]              %sRRESP\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sRLAST\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sRVALID\n", otype, prefix);
fprintf(fo, "     , input   wire                      %sRREADY\n", prefix);
fprintf(fo, "     `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "     , output  %-4s  [WIDTH_RUSER-1:0]   %sRUSER\n", otype, prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_sport( char* prefix  // prefix (S_, S0_, )
                 , char* otype // output type (wire or reg)
                 , int   axi4
                 , FILE* fo)
{
    int ret=0;

    if ((prefix==NULL)||(otype==NULL)) return 1;

    ret += gen_axi_sport_aw( prefix, otype, axi4, fo );
    ret += gen_axi_sport_w ( prefix, otype, axi4, fo );
    ret += gen_axi_sport_b ( prefix, otype, fo );
    ret += gen_axi_sport_ar( prefix, otype, axi4, fo );
    ret += gen_axi_sport_r ( prefix, otype, fo );

    return 0;
}

//--------------------------------------------------------
int gen_axi_sport_aw( char* prefix
                    , char* otype // output type (wire or reg)
                    , int   axi4
                    , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , output  %-4s  [WIDTH_SID-1:0]     %sAWID\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [WIDTH_AD-1:0]      %sAWADDR\n", otype, prefix);
if (axi4) {
fprintf(fo, "     , output  %-4s  [ 7:0]              %sAWLEN\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sAWLOCK\n", otype, prefix);
} else {
fprintf(fo, "     , output  %-4s  [ 3:0]              %sAWLEN\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [ 1:0]              %sAWLOCK\n", otype, prefix);
}
fprintf(fo, "     , output  %-4s  [ 2:0]              %sAWSIZE\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [ 1:0]              %sAWBURST\n", otype, prefix);
fprintf(fo, "     `ifdef  AMBA_AXI_CACHE\n");
fprintf(fo, "     , output  %-4s  [ 3:0]              %sAWCACHE\n", otype, prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     , output  %-4s  [ 2:0]              %sAWPROT\n", otype, prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     , output  %-4s                      %sAWVALID\n", otype, prefix);
fprintf(fo, "     , input   wire                      %sAWREADY\n", prefix);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     , output  %-4s  [ 3:0]              %sAWQOS\n", otype, prefix);
fprintf(fo, "     , output  %-4s  [ 3:0]              %sAWREGION\n", otype, prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "     , output  %-4s  [WIDTH_AWUSER-1:0]  %sAWUSER\n", otype, prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_sport_w( char* prefix
                   , char* otype // output type (wire or reg)
                   , int   axi4
                   , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

if (axi4==0) {
fprintf(fo, "     , output  %-4s   [WIDTH_SID-1:0]    %sWID\n", otype, prefix);
}
fprintf(fo, "     , output  %-4s   [WIDTH_DA-1:0]     %sWDATA\n", otype, prefix);
fprintf(fo, "     , output  %-4s   [WIDTH_DS-1:0]     %sWSTRB\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sWLAST\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sWVALID\n", otype, prefix);
fprintf(fo, "     , input   wire                      %sWREADY\n", prefix);
fprintf(fo, "     `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "     , output  %-4s   [WIDTH_WUSER-1:0]  %sWUSER\n", otype, prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_sport_ar( char* prefix
                    , char* otype // output type (wire or reg)
                    , int   axi4
                    , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , output  %-4s   [WIDTH_SID-1:0]    %sARID\n", otype, prefix);
fprintf(fo, "     , output  %-4s   [WIDTH_AD-1:0]     %sARADDR\n", otype, prefix);
if (axi4) {
fprintf(fo, "     , output  %-4s   [ 7:0]             %sARLEN\n", otype, prefix);
fprintf(fo, "     , output  %-4s                      %sARLOCK\n", otype, prefix);
} else {
fprintf(fo, "     , output  %-4s   [ 3:0]             %sARLEN\n", otype, prefix);
fprintf(fo, "     , output  %-4s   [ 1:0]             %sARLOCK\n", otype, prefix);
}
fprintf(fo, "     , output  %-4s   [ 2:0]             %sARSIZE\n", otype, prefix);
fprintf(fo, "     , output  %-4s   [ 1:0]             %sARBURST\n", otype, prefix);
fprintf(fo, "     `ifdef  AMBA_AXI_CACHE\n");
fprintf(fo, "     , output  %-4s   [ 3:0]             %sARCACHE\n", otype, prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     , output  %-4s   [ 2:0]             %sARPROT\n", otype, prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     , output  %-4s                      %sARVALID\n", otype, prefix);
fprintf(fo, "     , input   wire                      %sARREADY\n", prefix);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     , output  %-4s   [ 3:0]             %sARQOS\n", otype, prefix);
fprintf(fo, "     , output  %-4s   [ 3:0]             %sARREGION\n", otype, prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "     , output  %-4s   [WIDTH_ARUSER-1:0] %sARUSER\n", otype, prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_sport_b( char* prefix
                   , char* otype // output type (wire or reg)
                   , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , input   wire   [WIDTH_SID-1:0]    %sBID\n", prefix);
fprintf(fo, "     , input   wire   [ 1:0]             %sBRESP\n", prefix);
fprintf(fo, "     , input   wire                      %sBVALID\n", prefix);
fprintf(fo, "     , output  %-4s                      %sBREADY\n", otype, prefix);
fprintf(fo, "     `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "     , input   wire   [WIDTH_BUSER-1:0]  %sBUSER\n", prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_sport_r( char* prefix
                   , char* otype // output type (wire or reg)
                   , FILE* fo)
{
    if ((prefix==NULL)||(otype==NULL)) return 1;

fprintf(fo, "     , input   wire   [WIDTH_SID-1:0]    %sRID\n", prefix);
fprintf(fo, "     , input   wire   [WIDTH_DA-1:0]     %sRDATA\n", prefix);
fprintf(fo, "     , input   wire   [ 1:0]             %sRRESP\n", prefix);
fprintf(fo, "     , input   wire                      %sRLAST\n", prefix);
fprintf(fo, "     , input   wire                      %sRVALID\n", prefix);
fprintf(fo, "     , output  %-4s                      %sRREADY\n", otype, prefix);
fprintf(fo, "     `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "     , input   wire   [WIDTH_RUSER-1:0]  %sRUSER\n", prefix);
fprintf(fo, "     `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_signal( char* prefix
                  , int   axi4
                  , FILE* fo )
{
    if (prefix==NULL) return 1;

fprintf(fo, "     wire  [WIDTH_SID-1:0]     %sAWID     ;\n", prefix);
fprintf(fo, "     wire  [WIDTH_AD-1:0]      %sAWADDR   ;\n", prefix);
if (axi4) {
fprintf(fo, "     wire  [ 7:0]              %sAWLEN    ;\n", prefix);
fprintf(fo, "     wire                      %sAWLOCK   ;\n", prefix);
} else {
fprintf(fo, "     wire  [ 3:0]              %sAWLEN    ;\n", prefix);
fprintf(fo, "     wire  [ 1:0]              %sAWLOCK   ;\n", prefix);
}
fprintf(fo, "     wire  [ 2:0]              %sAWSIZE   ;\n", prefix);
fprintf(fo, "     wire  [ 1:0]              %sAWBURST  ;\n", prefix);
fprintf(fo, "     `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "     wire  [ 3:0]              %sAWCACHE  ;\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     wire  [ 2:0]              %sAWPROT   ;\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     wire                      %sAWVALID  ;\n", prefix);
fprintf(fo, "     wire                      %sAWREADY  ;\n", prefix);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     wire  [ 3:0]              %sAWQOS    ;\n", prefix);
fprintf(fo, "     wire  [ 3:0]              %sAWREGION ;\n", prefix);
fprintf(fo, "     `endif\n");
if (axi4==0) {
fprintf(fo, "     wire  [WIDTH_SID-1:0]     %sWID      ;\n", prefix);
}
fprintf(fo, "     wire  [WIDTH_DA-1:0]      %sWDATA    ;\n", prefix);
fprintf(fo, "     wire  [WIDTH_DS-1:0]      %sWSTRB    ;\n", prefix);
fprintf(fo, "     wire                      %sWLAST    ;\n", prefix);
fprintf(fo, "     wire                      %sWVALID   ;\n", prefix);
fprintf(fo, "     wire                      %sWREADY   ;\n", prefix);
fprintf(fo, "     wire  [WIDTH_SID-1:0]     %sBID      ;\n", prefix);
fprintf(fo, "     wire  [ 1:0]              %sBRESP    ;\n", prefix);
fprintf(fo, "     wire                      %sBVALID   ;\n", prefix);
fprintf(fo, "     wire                      %sBREADY   ;\n", prefix);
fprintf(fo, "     wire  [WIDTH_SID-1:0]     %sARID     ;\n", prefix);
fprintf(fo, "     wire  [WIDTH_AD-1:0]      %sARADDR   ;\n", prefix);
if (axi4) {
fprintf(fo, "     wire  [ 7:0]              %sARLEN    ;\n", prefix);
fprintf(fo, "     wire                      %sARLOCK   ;\n", prefix);
} else {
fprintf(fo, "     wire  [ 3:0]              %sARLEN    ;\n", prefix);
fprintf(fo, "     wire  [ 1:0]              %sARLOCK   ;\n", prefix);
}
fprintf(fo, "     wire  [ 2:0]              %sARSIZE   ;\n", prefix);
fprintf(fo, "     wire  [ 1:0]              %sARBURST  ;\n", prefix);
fprintf(fo, "     `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "     wire  [ 3:0]              %sARCACHE  ;\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "     wire  [ 2:0]              %sARPROT   ;\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     wire                      %sARVALID  ;\n", prefix);
fprintf(fo, "     wire                      %sARREADY  ;\n", prefix);
fprintf(fo, "     `ifdef AMBA_QOS\n");
fprintf(fo, "     wire  [ 3:0]              %sARQOS    ;\n", prefix);
fprintf(fo, "     wire  [ 3:0]              %sARREGION ;\n", prefix);
fprintf(fo, "     `endif\n");
fprintf(fo, "     wire  [WIDTH_SID-1:0]     %sRID      ;\n", prefix);
fprintf(fo, "     wire  [WIDTH_DA-1:0]      %sRDATA    ;\n", prefix);
fprintf(fo, "     wire  [ 1:0]              %sRRESP    ;\n", prefix);
fprintf(fo, "     wire                      %sRLAST    ;\n", prefix);
fprintf(fo, "     wire                      %sRVALID   ;\n", prefix);
fprintf(fo, "     wire                      %sRREADY   ;\n", prefix);

    return 0;
}

//--------------------------------------------------------
int gen_axi_m2s_mcon_aw( char* prefixA, char* prefixB, char* postfix, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)||(postfix==NULL)) return 1;

fprintf(fo, "                              , .%sMID               (%sMID       )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWID              (%sAWID      )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWADDR            (%sAWADDR    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWLEN             (%sAWLEN     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWLOCK            (%sAWLOCK    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWSIZE            (%sAWSIZE    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWBURST           (%sAWBURST   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "                              , .%sAWCACHE           (%sAWCACHE   )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "                              , .%sAWPROT            (%sAWPROT    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "                              , .%sAWVALID           (%sAWVALID   )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWREADY           (%sAWREADY%s)\n", prefixA, prefixB, postfix);
fprintf(fo, "         `ifdef AMBA_QOS\n");
fprintf(fo, "                              , .%sAWQOS             (%sAWQOS     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sAWREGION          (%sAWREGION  )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "                              , .%sAWUSER            (%sAWUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_m2s_mcon_w( char* prefixA, char* prefixB, char* postfix, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)||(postfix==NULL)) return 1;

fprintf(fo, "                              , .%sWID               (%sWID       )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sWDATA             (%sWDATA     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sWSTRB             (%sWSTRB     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sWLAST             (%sWLAST     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sWVALID            (%sWVALID    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sWREADY            (%sWREADY%s )\n", prefixA, prefixB, postfix);
fprintf(fo, "         `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "                              , .%sWUSER             (%sWUSER     )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_m2s_mcon_ar( char* prefixA, char* prefixB, char* postfix, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)||(postfix==NULL)) return 1;

fprintf(fo, "                              , .%sARID              (%sARID      )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARADDR            (%sARADDR    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARLEN             (%sARLEN     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARLOCK            (%sARLOCK    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARSIZE            (%sARSIZE    )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARBURST           (%sARBURST   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "                              , .%sARCACHE           (%sARCACHE   )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "                              , .%sARPROT            (%sARPROT    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "                              , .%sARVALID           (%sARVALID   )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARREADY           (%sARREADY%s)\n", prefixA, prefixB, postfix);
fprintf(fo, "         `ifdef AMBA_QOS\n");
fprintf(fo, "                              , .%sARQOS             (%sARQOS     )\n", prefixA, prefixB);
fprintf(fo, "                              , .%sARREGION          (%sARREGION  )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "                              , .%sARUSER            (%sARUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_m2s_scon_aw( char* prefixA, char* prefixB, FILE* fo)
{
    if ((prefixA==NULL)||(prefixB==NULL)) return 1;

fprintf(fo, "         , .%sAWID               (%sAWID      )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWADDR             (%sAWADDR    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWLEN              (%sAWLEN     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWLOCK             (%sAWLOCK    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWSIZE             (%sAWSIZE    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWBURST            (%sAWBURST   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "         , .%sAWCACHE            (%sAWCACHE   )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "         , .%sAWPROT             (%sAWPROT    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         , .%sAWVALID            (%sAWVALID   )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWREADY            (%sAWREADY   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_QOS\n");
fprintf(fo, "         , .%sAWQOS              (%sAWQOS     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sAWREGION           (%sAWREGION  )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_AWUSER\n");
fprintf(fo, "         , .%sAWUSER             (%sAWUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_m2s_scon_w ( char* prefixA, char* prefixB, FILE* fo)
{
    if ((prefixA==NULL)||(prefixB==NULL)) return 1;

fprintf(fo, "         , .%sWID                (%sWID       )\n", prefixA, prefixB);
fprintf(fo, "         , .%sWDATA              (%sWDATA     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sWSTRB              (%sWSTRB     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sWLAST              (%sWLAST     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sWVALID             (%sWVALID    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sWREADY             (%sWREADY    )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_WUSER\n");
fprintf(fo, "         , .%sWUSER              (%sWUSER     )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_m2s_scon_ar( char* prefixA, char* prefixB, FILE* fo)
{
    if ((prefixA==NULL)||(prefixB==NULL)) return 1;

fprintf(fo, "         , .%sARID               (%sARID      )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARADDR             (%sARADDR    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARLEN              (%sARLEN     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARLOCK             (%sARLOCK    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARSIZE             (%sARSIZE    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARBURST            (%sARBURST   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_CACHE\n");
fprintf(fo, "         , .%sARCACHE            (%sARCACHE   )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_PROT\n");
fprintf(fo, "         , .%sARPROT             (%sARPROT    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         , .%sARVALID            (%sARVALID   )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARREADY            (%sARREADY   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_QOS\n");
fprintf(fo, "         , .%sARQOS              (%sARQOS     )\n", prefixA, prefixB);
fprintf(fo, "         , .%sARREGION           (%sARREGION  )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");
fprintf(fo, "         `ifdef AMBA_AXI_ARUSER\n");
fprintf(fo, "         , .%sARUSER             (%sARUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_s2m_mcon_b( char* prefixA, char* prefixB, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)) return 1;

fprintf(fo, "         , .%sMID                (%sMID      )\n", prefixA, prefixB);
fprintf(fo, "         , .%sBID                (%sBID      )\n", prefixA, prefixB);
fprintf(fo, "         , .%sBRESP              (%sBRESP    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sBVALID             (%sBVALID   )\n", prefixA, prefixB);
fprintf(fo, "         , .%sBREADY             (%sBREADY   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "         , .%sBUSER              (%sBUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_s2m_mcon_r( char* prefixA, char* prefixB, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)) return 1;

fprintf(fo, "         , .%sRID                (%sRID      )\n", prefixA, prefixB);
fprintf(fo, "         , .%sRDATA              (%sRDATA    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sRRESP              (%sRRESP    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sRLAST              (%sRLAST    )\n", prefixA, prefixB);
fprintf(fo, "         , .%sRVALID             (%sRVALID   )\n", prefixA, prefixB);
fprintf(fo, "         , .%sRREADY             (%sRREADY   )\n", prefixA, prefixB);
fprintf(fo, "         `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "         , .%sRUSER              (%sRUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_s2m_scon_b( char* prefixA, char* prefixB, char* postfix, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)||(postfix==NULL)) return 1;

fprintf(fo, "                           , .%sBID               (%sBID      )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sBRESP             (%sBRESP    )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sBVALID            (%sBVALID   )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sBREADY            (%sBREADY%s)\n", prefixA, prefixB, postfix);
fprintf(fo, "         `ifdef AMBA_AXI_BUSER\n");
fprintf(fo, "                           , .%sBUSER             (%sBUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
int gen_axi_s2m_scon_r( char* prefixA, char* prefixB, char* postfix, FILE* fo )
{
    if ((prefixA==NULL)||(prefixB==NULL)||(postfix==NULL)) return 1;

fprintf(fo, "                           , .%sRID               (%sRID      )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sRDATA             (%sRDATA    )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sRRESP             (%sRRESP    )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sRLAST             (%sRLAST    )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sRVALID            (%sRVALID   )\n", prefixA, prefixB);
fprintf(fo, "                           , .%sRREADY            (%sRREADY%s)\n", prefixA, prefixB, postfix);
fprintf(fo, "         `ifdef AMBA_AXI_RUSER\n");
fprintf(fo, "                           , .%sRUSER             (%sRUSER    )\n", prefixA, prefixB);
fprintf(fo, "         `endif\n");

    return 0;
}

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
