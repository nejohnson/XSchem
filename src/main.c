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
#define __USE_GNU
#include <fenv.h>
#include <sys/wait.h>


void clean_up_child_process(int signal_num)
{
  /* remove child process */
  // int status;
 
  //// putting a wait here is dangerous, in some cases
  //// the program may deadlock....
  ////
  if(debug_var>=1) fprintf(errfp, "clean_up_child_process()\n");
  // wait (&status);
}

void sig_handler(int s){
  char emergency_dir[4096];

  // 20150410
  if(s==SIGINT) {
    fprintf(errfp, "Use 'exit' to close the program\n");
    return;
  }
  if(s==SIGCHLD) {
    fprintf(errfp, "SIGCHLD received\n");
    return;
  }
  my_snprintf(emergency_dir, S(emergency_dir), "%s/xschem_emergencysave_%s_XXXXXX", 
           getenv("HOME"),
           skip_dir(schematic[currentsch]));
  if( !mkdtemp(emergency_dir) ) {
    if(debug_var>=1) fprintf(errfp, "xinit(): problems creating emergency save dir\n");
    Tcl_Eval(interp, "exit");
  }

  rename(undo_dirname, emergency_dir);
  fprintf(errfp, "\nFATAL: signal %d\n", s);
  fprintf(errfp, "while editing: %s\n", skip_dir(schematic[currentsch]));
  fprintf(errfp, "EMERGENCY SAVE DIR: %s\n", emergency_dir);
  // /20150410
  // Tcl_Eval(interp, "exit");
  Tcl_EvalEx(interp, "exit", -1, TCL_EVAL_GLOBAL);
  // exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

  // sig handler that traps CTRL-C
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = sig_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  // sigIntHandler.sa_flags = SA_NOCLDWAIT;
  
  // trap signals // 20150410
  sigaction(SIGINT, &sigIntHandler, NULL);  // ctrl-C
  sigaction(SIGFPE, &sigIntHandler, NULL);
  sigaction(SIGILL, &sigIntHandler, NULL);
  sigaction(SIGSEGV, &sigIntHandler, NULL);
  sigaction(SIGTERM, &sigIntHandler, NULL);

  sigIntHandler.sa_handler = clean_up_child_process;
  sigaction(SIGCHLD, &sigIntHandler, NULL);

  //// enable all fpe exceptions
  // feenableexcept(FE_ALL_EXCEPT); // enable FPE exceptions
  feenableexcept( FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW ); // 20150410 enable FPE exceptions


/*
   // 20090916
   //// avoid zombies, set SA_NOCLDWAIT flag
   struct sigaction act;

   act.sa_handler = SIG_DFL;
   sigemptyset(&act.sa_mask);
   act.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &act, NULL);
   // /20090916
*/

   errfp=stderr; 
   process_options(argc, argv);
   if(has_x) Tk_Main(1, argv, Tcl_AppInit);
   else     Tcl_Main(1, argv, Tcl_AppInit);
   return 0;
}

