/* File: options.c
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2019 Stefan Frederik Schippers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "xschem.h"
#define SHORT 1
#define LONG 2

void check_opt(char *opt, char *optval, int type)
{
    if( (type == SHORT && *opt == 'd') || (type == LONG && !strcmp("debug", opt)) ) {
        if(optval) debug_var=atoi(optval);
        else debug_var = 0;

    } else if( (type == SHORT && *opt == 'S') || (type == LONG && !strcmp("simulate", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will do simulation\n");
        do_simulation=1;

    } else if( (type == SHORT && *opt == 'W') || (type == LONG && !strcmp("waves", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will show waves\n");
        do_waves=1;

    } else if( (type == SHORT && *opt == 'n') || (type == LONG && !strcmp("netlist", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will do netlist\n");
        do_netlist=1;

    } else if( (type == SHORT && *opt == 'f') || (type == LONG && !strcmp("flat_netlist", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set flat netlist\n");
        flat_netlist=1;

    } else if( (type == SHORT && *opt == 'r') || (type == LONG && !strcmp("no_readline", opt)) ) {
        no_readline=1;

    } else if( (type == SHORT && *opt == 'p') || (type == LONG && !strcmp("postscript", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will print postscript/pdf\n");
        do_print=1;

    } else if( (type == LONG && !strcmp("pdf", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will print postscript/pdf\n");
        do_print=1;

    } else if( (type == LONG && !strcmp("plotfile", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): user plotfile specified: %s\n", optval ? optval : "NULL");
        if(optval) my_strncpy(plotfile, optval, S(plotfile));

    } else if( (type == LONG && !strcmp("events", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): event reporting on stdout enabled\n");
        event_reporting = 1;
        no_readline = 1;

    } else if( (type == LONG && !strcmp("rcfile", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): user rcfile specified: %s\n", optval ? optval : "NULL");
        if(optval) my_strncpy(rcfile, optval, S(rcfile));

    } else if( (type == LONG && !strcmp("png", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will print png\n");
        do_print=2;

    } else if( (type == LONG && !strcmp("tcl_command", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): passing tcl command to interpreter: %s\n", optval);
        my_strdup(110, &tcl_command, optval);

    } else if( (type == LONG && !strcmp("svg", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): will print png\n");
        do_print=3;

    } else if( (type == SHORT && *opt == 'c') || (type == LONG && !strcmp("color_ps", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set color postscript\n");
        color_ps=1;

    } else if( (type == SHORT && *opt == '3') || (type == LONG && !strcmp("a3page", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set A3 page size\n");
        a3page=1;

    } else if( (type == SHORT && *opt == 'i') || (type == LONG && !strcmp("no_rcload", opt)) ) {
        load_initfile=0;

    } else if( (type == SHORT && *opt == 'l') || (type == LONG && !strcmp("log", opt)) ) {
        if(optval) errfp = fopen(optval, "w");

    } else if( (type == SHORT && *opt == 'o') || (type == LONG && !strcmp("netlist_path", opt)) ) {
        if(optval) {
          my_strdup(48, &netlist_dir, optval);
        }

    } else if( (type == SHORT && *opt == 's') || (type == LONG && !strcmp("spice", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to spice\n");
        netlist_type=CAD_SPICE_NETLIST;

    } else if( (type == SHORT && *opt == 'V') || (type == LONG && !strcmp("vhdl", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to vhdl\n");
        netlist_type=CAD_VHDL_NETLIST;

    } else if( (type == SHORT && *opt == 'w') || (type == LONG && !strcmp("verilog", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to verilog\n");
        netlist_type=CAD_VERILOG_NETLIST;

    } else if( (type == SHORT && *opt == 'v') || (type == LONG && !strcmp("version", opt)) ) {
        print_version();

    } else if( (type == SHORT && *opt == 't') || (type == LONG && !strcmp("tedax", opt)) ) {
        if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to tEDAx\n");
        netlist_type=CAD_TEDAX_NETLIST;

    } else if( (type == SHORT && *opt == 'q') || (type == LONG && !strcmp("quit", opt)) ) {
        quit=1;

    } else if( (type == SHORT && *opt == 'x') || (type == LONG && !strcmp("no_x", opt)) ) {
        has_x=0;

    } else if( (type == SHORT && *opt == 'z') || (type == LONG && !strcmp("rainbow", opt)) ) {
        rainbow_colors=1;

    } else if( (type == SHORT && *opt == 'h') || (type == LONG && !strcmp("help", opt)) ) {
        help=1;

    } else {
        fprintf(errfp, "Unknown option: %s\n", opt);
    }
}

int process_options(int argc, char *argv[])
{
  int i, arg_cnt;
  char *opt, *optval;

  for(arg_cnt = i = 1; i < argc; i++) {
    opt = argv[i];
    if(*opt == '-') { /* options */
      opt++;
      if(opt && *opt=='-') { /* long options */
        opt++;
        if(*opt) {
          optval = strchr(opt, '=');
          if(optval) {
            *optval = '\0';
            optval++;
          }
          if(!optval && i < argc-1 && argv[i+1][0] != '-') {
            /* options requiring arguments are listed here */
            if(!strcmp("debug", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("tcl_command", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("log", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("netlist_path", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("rcfile", opt)) {
              optval = argv[++i];
            }
            else if(!strcmp("plotfile", opt)) {
              optval = argv[++i];
            }
          }
          check_opt(opt, optval, LONG);
          /* printf("long opt:%s, value: %s\n", opt, optval ? optval : "no value"); */
        }
      }
      else { /* short options */
        while(*opt) {
          optval = NULL;
          /* options requiring arguments are listed here */
          if(*opt == 'l') {
            optval = argv[++i];
          }
          else if(*opt == 'd') {
            optval = argv[++i];
          }
          else if(*opt == 'o') {
            optval = argv[++i];
          }
          check_opt(opt, optval, SHORT);
          /* printf("opt: %c, value: %s\n", *opt, optval ? optval : "no value"); */
          opt++;
        }
      }
    } else { /* arguments */
      /* printf("argument: %s\n", opt); */
      argv[arg_cnt++] = opt;
    }
  }
  if (arg_cnt>=2) {
    if(debug_var>=1) fprintf(errfp, "process_option(): file name given: %s\n",argv[1]);
    my_strdup(291, &filename, argv[1]);
  }
  return arg_cnt;
}

