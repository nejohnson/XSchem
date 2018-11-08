/* File: main.c
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
#include <sys/wait.h>

void sig_handler(int s){
  char emergency_prefix[PATH_MAX];
  const char *emergency_dir;

  /* 20150410 */
  if(s==SIGINT) {
    fprintf(errfp, "Use 'exit' to close the program\n");
    return;
  }
  /* 20180923 no more mkdtemp */
  my_snprintf(emergency_prefix, S(emergency_prefix), "xschem_emergencysave_%s_", 
           skip_dir(schematic[currentsch]));
  if( !(emergency_dir = create_tmpdir(emergency_prefix)) ) {
    fprintf(errfp, "xinit(): problems creating emergency save dir\n");
    /* tcleval( "exit"); */
    tcleval("exit");
  }

  if(rename(undo_dirname, emergency_dir)) {
    fprintf(errfp, "rename dir %s to %s failed\n", undo_dirname, emergency_dir);
  }
  fprintf(errfp, "\nFATAL: signal %d\n", s);
  fprintf(errfp, "while editing: %s\n", skip_dir(schematic[currentsch]));
  fprintf(errfp, "EMERGENCY SAVE DIR: %s\n", emergency_dir);
  exit(EXIT_FAILURE);
}

void child_handler(int signum) 
{ 
    /* fprintf(errfp, "SIGCHLD received\n"); */
    wait(NULL); 
} 

int main(int argc, char **argv)
{
  xschem_executable = argv[0];
  signal(SIGINT, sig_handler);
  signal(SIGSEGV, sig_handler);
  signal(SIGILL, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGFPE, sig_handler);
  /* signal(SIGCHLD, child_handler); */  /* avoid zombies 20180925 --> conflicts with tcl exec */

  errfp=stderr; 
  /* 20181013 check for empty or non existing DISPLAY *before* calling Tk_Main or Tcl_Main */
  if(!getenv("DISPLAY") || !getenv("DISPLAY")[0]) has_x=0;
  process_options(argc, argv);
  if(debug_var>=1 && !has_x) 
    fprintf(errfp, "main(): no DISPLAY set, assuming no X available\n");

  if(has_x) Tk_Main(1, argv, Tcl_AppInit);
  else     Tcl_Main(1, argv, Tcl_AppInit);
  return 0;
}

