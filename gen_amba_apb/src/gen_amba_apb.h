#ifndef GEN_AMBA_APB_H
#define GEN_AMBA_APB_H
//--------------------------------------------------------
// Copyright (c) 2018 by Ando Ki (andoki@gmail.com)
// The contents and codes along with it are prepared      
// in the hope that it will be useful to understand
// Ando Ki's work, but WITHOUT ANY WARRANTY.              
// The design and code are not guaranteed to work on all systems.   
// While there are no known issues with using the design and code, 
// no technical support will be provided for problems     
// that might arise.                                      
//--------------------------------------------------------

extern int gen_ahb2apb       ( unsigned int num, char* module, char* prefix, FILE* fo );
extern int gen_ahb2apb_bridge( char *prefix, FILE* fo );
extern int gen_axi2apb       ( unsigned int num, char* module, char* prefi, int axi4, FILE* fox );
extern int gen_axi2apb_bridge( char *prefix, int axi4, FILE* fo );
extern int gen_apb_amba      ( unsigned int numS, char *module, char *prefix, int bridge, FILE* fo );
extern int gen_apb_amba_core ( unsigned int num, char *module, char* prefix, int bridge, FILE* fo );
extern int gen_apb_decoder   ( unsigned int num, char* prefix, FILE* fo );
extern int gen_apb_mux       ( unsigned int num, char* prefix, FILE* fo );

//--------------------------------------------------------
// Revision history
//
// 2018.06.06: Released
// 2016.03.26: Startd by Ando Ki.
//--------------------------------------------------------
#endif
