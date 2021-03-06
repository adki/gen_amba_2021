//--------------------------------------------------------
// Copyright (c) 2018 by Ando Ki (andoki@gmail.com)                         
// The contents and codes along with it are prepared      
// in the hope that it will be useful to understand       
// Ando Ki's work, but WITHOUT ANY WARRANTY.              
// The design is not guaranteed to work on all systems.   
// While there are no known issues with using the design, 
// no technical support will be provided for problems     
// that might arise.                                      
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#endif
#include "gen_amba_apb.h"

int main(int argc, char *argv[]) {
  extern void sig_handle(int);
  extern int  arg_parser(int, char **);
  extern const char program[];
  extern const unsigned int version;
  extern const char license[];
  extern unsigned int numS;
  extern char module[];
  extern char prefix[];
  extern FILE* fo;
  extern int   ahb, axi3, axi4;

  if ((signal(SIGINT, sig_handle)==SIG_ERR)
#ifndef WIN32
	  ||(signal(SIGQUIT, sig_handle)==SIG_ERR)
#endif
	  ) {
        fprintf(stderr, "Error: signal error\n");
        exit(1);
  }

  if (arg_parser(argc, argv)) return 1;

fprintf(fo, "//---------------------------------------------------------------------------\n");
fprintf(fo, "// gen_amba_apb.v generated by \"%s (%X)\"\n", program, version);
fprintf(fo, "//---------------------------------------------------------------------------\n");

  if (ahb) {
      if (gen_ahb2apb( numS, module, prefix, fo )) return 1;
      if (gen_ahb2apb_bridge( prefix, fo )) return 1;
  } else if (axi3|axi4) {
      if (gen_axi2apb( numS, module, prefix, axi4, fo )) return 1;
      if (gen_axi2apb_bridge( prefix, axi4, fo )) return 1;
  }
  if (gen_apb_amba( numS, module, prefix, axi3|axi4|ahb, fo )) return 1;

fprintf(fo, "/* %s\n*/\n", license);

  return(0);
}
//--------------------------------------------------------
void sig_handle(int sig) {
  extern void cleanup();
  switch (sig) {
  case SIGINT:
#ifndef WIN32
  case SIGQUIT:
#endif
       exit(0);
       break;
  }
}
//--------------------------------------------------------
// Revision history
//
// 2018.07.19: Minor modification by Ando Ki.
// 2016.03.26: Startd by Ando Ki.
//--------------------------------------------------------
