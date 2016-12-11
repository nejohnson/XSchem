/* File: options.c
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2016 Stefan Frederik Schippers
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
void
 process_options(int argc, char **argv) {
     int c;
     
     while (1) {
         c = getopt (argc, argv, "3cpxzl:d:nfhrsvwq");
         if (c == -1)
             break;
 
         switch (c) {
 
         case 'n':
             if(debug_var>=1) fprintf(errfp, "process_options(): will do netlist\n");
             do_netlist=1;
             break;
 
         case 'f':
             if(debug_var>=1) fprintf(errfp, "process_options(): set flat netlist\n");
             flat_netlist=1;
             break;
 
         case 'r':
             no_readline=1;
             break;

         case 'p':
             if(debug_var>=1) fprintf(errfp, "process_options(): will print postscript\n");
             do_print=1;
             break;
 
         case 'c':
             if(debug_var>=1) fprintf(errfp, "process_options(): set color postscript\n");
             color_ps=1;
             break;
 
         case '3':
             if(debug_var>=1) fprintf(errfp, "process_options(): set A3 page size\n");
             a3page=1;
             break;
 
         case 'd':
             debug_var=atoi(optarg);
             break;
 
         case 'l':
             errfp = fopen(optarg, "w");
             if(debug_var>=1) fprintf(errfp, "process_options(): opening log file: %s\n", optarg);
             break;
 
         case 's':
             if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to spice\n");
             netlist_type=CAD_SPICE_NETLIST;
             break;
 
         case 'v':
             if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to vhdl\n");
             netlist_type=CAD_VHDL_NETLIST;
             break;
         case 'w':
             if(debug_var>=1) fprintf(errfp, "process_options(): set netlist type to verilog\n");
             netlist_type=CAD_VERILOG_NETLIST;
             break;
 
         case 'q':
             quit=1;
             break;
 
         case 'x':
             has_x=0;
             break;
 
         case 'z':
             rainbow_colors=1;
             break;
 
         case 'h':
             help=1;
             break;
 
         case '?':
             printf("what ???????\n");
             break;
 
         default:
             printf ("?? getopt returned character code 0%o ??\n", c);
         }
     }
 
     if (optind < argc) {
          if(debug_var>=1) fprintf(errfp, "process_option(): file name given: %s\n",argv[optind]);
          my_strdup(&filename, argv[optind]);
     }
     
 }
 

