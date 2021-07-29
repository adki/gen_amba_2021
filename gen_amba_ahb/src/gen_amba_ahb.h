#ifndef GEN_AMBA_AHB_H
#define GEN_AMBA_AHB_H
//--------------------------------------------------------
// Copyright (c) 2016 by Ando Ki (adki@future-ds.com / andoki@gmail.com)
// The contents and codes along with it are prepared      
// in the hope that it will be useful to understand
// Ando Ki's work, but WITHOUT ANY WARRANTY.              
// The design and code are not guaranteed to work on all systems.   
// While there are no known issues with using the design and code, 
// no technical support will be provided for problems     
// that might arise.                                      
//--------------------------------------------------------

extern int gen_ahb_amba( unsigned int numM // num of masters
                       , unsigned int numS // num of slaves
                       , char *module
                       , char *prefix
                       , FILE *fo);
extern int gen_ahb_amba_core( unsigned int numM // num of masters
                            , unsigned int numS // num of slaves
                            , char *module
                            , char *prefix
                            , FILE *fo);
extern int gen_ahb_arbiter  ( unsigned int numM
                            , unsigned int numS
                            , char* prefix
                            , FILE *fo);
extern int gen_ahb_m2s      ( unsigned int numM
                            , char* prefix
                            , FILE *fo);
extern int gen_ahb_lite     ( unsigned int lite 
                            , unsigned int numS
                            , char* module
                            , char* prefix
                            , FILE *fo);
extern int gen_ahb_lite_core( unsigned int lite
                            , unsigned int num
                            , char* module
                            , char *prefix
                            , FILE *fo);
extern int gen_ahb_decoder  ( unsigned int num
                            , char* prefix
                            , FILE *fo);
extern int gen_ahb_s2m      ( unsigned int num
                            , char* prefix
                            , FILE *fo);
extern int gen_ahb_default_slave( char* prefix
                                , FILE *fo);

//--------------------------------------------------------
// Revision history
//
// 2016.03.26: Startd by Ando Ki.
//--------------------------------------------------------
#endif
