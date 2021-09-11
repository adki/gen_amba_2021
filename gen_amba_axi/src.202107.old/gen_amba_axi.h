#ifndef GEN_AMBA_AXI_H
#define GEN_AMBA_AXI_H
//--------------------------------------------------------
// Copyright (c) 2016-2021 by Ando Ki (adki@future-ds.com / andoki@gmail.com)
// The contents and codes along with it are prepared      
// in the hope that it will be useful to understand
// Ando Ki's work, but WITHOUT ANY WARRANTY.              
// The design and code are not guaranteed to work on all systems.   
// While there are no known issues with using the design and code, 
// no technical support will be provided for problems     
// that might arise.                                      
//--------------------------------------------------------

extern int gen_axi_amba( unsigned int numM // num of masters
                       , unsigned int numS // num of slaves
                       , char *module
                       , char *prefix
                       , int   axi4 // AXI4 if 1
                       , FILE *fo);
extern int gen_axi_amba_core( unsigned int numM // num of masters
                            , unsigned int numS // num of slaves
                            , char *module
                            , char *prefix
                            , int   axi4 // AXI4 if 1
                            , FILE *fo);
extern int gen_axi_mtos( unsigned int num // num of masters
                       , char *prefix
                       , int   axi4 // AXI4 if 1
                       , FILE *fo);
extern int gen_axi_stom( unsigned int num // num of slaves
                       , char *prefix
                       , FILE *fo);
extern int gen_axi_arbiter_mtos( unsigned int num // num of masters
                               , char *prefix
                               , FILE *fo);
extern int gen_axi_arbiter_stom( unsigned int num // num of slaves
                               , char *prefix
                               , FILE *fo);
extern int gen_axi_default_slave( char* prefix
                                , int   axi4 // AXI4 if 1
                                , FILE* fo );

extern int gen_axi_wid( char* prefix
                      , FILE* fo );

//--------------------------------------------------------
// Revision history
//
// 2021.06.01: 'axi4' argument for 'gen_aix_amba()'.
// 2016.03.26: Startd by Ando Ki.
//--------------------------------------------------------
#endif
