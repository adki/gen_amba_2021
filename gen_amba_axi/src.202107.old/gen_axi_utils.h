#ifndef GEN_AXI_UTILS_H
#define GEN_AXI_UTILS_H
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

extern int gen_axi_mport   ( char* prefix    // prefix M_, M0_, M1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_mport_aw( char* prefix    // prefix M_, M0_, M1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_mport_w ( char* prefix    // prefix M_, M0_, M1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_mport_b ( char* prefix    // prefix M_, M0_, M1_, ..
                           , char* otype     // output type (wire or reg)
                           , FILE* fo);
extern int gen_axi_mport_ar( char* prefix    // prefix M_, M0_, M1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_mport_r ( char* prefix    // prefix M_, M0_, M1_, ..
                           , char* otype     // output type (wire or reg)
                           , FILE* fo);

extern int gen_axi_sport   ( char* prefix    // prefix S_, S0_, S1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_sport_aw( char* prefix    // prefix S_, S0_, S1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_sport_w ( char* prefix    // prefix S_, S0_, S1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_sport_b ( char* prefix    // prefix S_, S0_, S1_, ..
                           , char* otype     // output type (wire or reg)
                           , FILE* fo);
extern int gen_axi_sport_ar( char* prefix    // prefix S_, S0_, S1_, ..
                           , char* otype     // output type (wire or reg)
                           , int   axi4
                           , FILE* fo);
extern int gen_axi_sport_r ( char* prefix    // prefix S_, S0_, S1_, ..
                           , char* otype     // output type (wire or reg)
                           , FILE* fo);

extern int gen_axi_signal( char* prefix
                         , int   axi4
                         , FILE* fo);

extern int gen_axi_m2s_mcon_aw( char* prefixA, char* prefixB, char* postfix, FILE* fo );
extern int gen_axi_m2s_mcon_w ( char* prefixA, char* prefixB, char* postfix, FILE* fo );
extern int gen_axi_m2s_mcon_ar( char* prefixA, char* prefixB, char* postfix, FILE* fo );
extern int gen_axi_m2s_scon_aw( char* prefixA, char* prefixB, FILE* fo );
extern int gen_axi_m2s_scon_w ( char* prefixA, char* prefixB, FILE* fo );
extern int gen_axi_m2s_scon_ar( char* prefixA, char* prefixB, FILE* fo );

extern int gen_axi_s2m_mcon_b( char* prefixA, char* prefixB, FILE* fo );
extern int gen_axi_s2m_mcon_r( char* prefixA, char* prefixB, FILE* fo );
extern int gen_axi_s2m_scon_b( char* prefixA, char* prefixB, char* postfix, FILE* fo );
extern int gen_axi_s2m_scon_r( char* prefixA, char* prefixB, char* postfix, FILE* fo );

//--------------------------------------------------------
// Revision history:
//
// 2016.03.26: Started by Ando Ki.
//--------------------------------------------------------
#endif
