/* File: callback.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2020 Stefan Frederik Schippers
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
static int mx_save, my_save;
static int last_command=0;

void start_line(double mx, double my)
{
    last_command = STARTLINE;
    if(ui_state & STARTLINE) {
      if(!vertical_move) {
        mx_save = mx;
        mx_double_save=mousex_snap;
      }
      if(!horizontal_move) {
        my_save = my;
        my_double_save=mousey_snap;
      }
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
    } else {
      mx_save = mx; my_save = my;
      mx_double_save=mousex_snap;
      my_double_save=mousey_snap;
    }
    new_line(PLACE);
}

void start_wire(double mx, double my)
{
     last_command = STARTWIRE;
     if(ui_state & STARTWIRE) {
       if(!vertical_move) {
         mx_save = mx;
         mx_double_save=mousex_snap;
       }
       if(!horizontal_move) {
         my_save = my;
         my_double_save=mousey_snap;
       }
       if(horizontal_move) mousey_snap = my_double_save;
       if(vertical_move) mousex_snap = mx_double_save;
     } else {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
     }
     new_wire(PLACE,mousex_snap, mousey_snap);

}
/* main window callback */
/* mx and my are set to the mouse coord. relative to window  */
int callback(int event, int mx, int my, KeySym key,
                 int button, int aux, int state)
{
 char str[PATH_MAX + 100]; /* overflow safe 20161122 */
 static char sel_or_clip[PATH_MAX] = "";/* overflow safe 20161122 */
 struct stat buf;
 unsigned short sel;
 static int capslock = 0;
 static int numlock = 0;
#ifndef __unix__
  short cstate = GetKeyState(VK_CAPITAL);
  short nstate = GetKeyState(VK_NUMLOCK);
  if (capslock==0 && (cstate&0x0001)) {
    tcleval(".statusbar.8 configure -state active -text {CAPS LOCK SET! }");
    capslock = 1;
  }
  if (capslock==1 && !(cstate&0x0001)) {
    if (numlock) tcleval(".statusbar.8 configure -state active -text {NUM LOCK SET! }");
    else tcleval(".statusbar.8 configure -state  normal -text {}");
    capslock = 0;
  }
 if(numlock==0 && (nstate&0x0001)) {
   tcleval(".statusbar.8 configure -state active -text {NUM LOCK SET! }");
   numlock = 1;
 }
 if (numlock==1 && !(nstate&0x0001)) {
   if(capslock) tcleval(".statusbar.8 configure -state active -text {CAPS LOCK SET! }");
   else tcleval(".statusbar.8 configure -state  normal -text {}");
   numlock = 0;
 }
#else
 XKeyboardState kbdstate;
 XGetKeyboardControl(display, &kbdstate);
 if(capslock == 0 && (kbdstate.led_mask & 1)) {
     tcleval(".statusbar.8 configure -state active -text {CAPS LOCK SET! }");
     capslock = 1;
 }
 if(capslock == 1 && !(kbdstate.led_mask & 1)) {
     if(numlock) tcleval(".statusbar.8 configure -state active -text {NUM LOCK SET! }");
     else tcleval(".statusbar.8 configure -state  normal -text {}");
     capslock = 0;
 }
 if(numlock == 0 && (kbdstate.led_mask & 2)) {
     tcleval(".statusbar.8 configure -state active -text {NUM LOCK SET! }");
     numlock = 1;
 }
 if(numlock == 1 && !(kbdstate.led_mask & 2)) {
     if(capslock) tcleval(".statusbar.8 configure -state active -text {CAPS LOCK SET! }");
     else tcleval(".statusbar.8 configure -state  normal -text {}");
     numlock = 0;
 }
#endif
 state &=~Mod2Mask; /* 20170511 filter out NumLock status */
 if(semaphore)
 {
   if(debug_var>=2)
     if(event != MotionNotify) fprintf(errfp, "callback(): reentrant call of callback(), semaphore=%d\n", semaphore);
   /* if(event==Expose) {
    *   XCopyArea(display, save_pixmap, window, gctiled, mx,my,button,aux,mx,my);
    *
    * }
    */
   /* return 0; */
 }
 semaphore++;           /* used to debug Tcl-Tk frontend */
 mousex=X_TO_XSCHEM(mx);
 mousey=Y_TO_XSCHEM(my);
 mousex_snap=ROUND(mousex / cadsnap) * cadsnap;
 mousey_snap=ROUND(mousey / cadsnap) * cadsnap;
 my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d path: %s",
   mousex_snap, mousey_snap, lastselected, xctx->sch_path[xctx->currsch] );
 statusmsg(str,1);
 switch(event)
 {
  case EnterNotify:
    if(!sel_or_clip[0]) my_snprintf(sel_or_clip, S(sel_or_clip), "%s/%s", user_conf_dir, ".selection.sch");

    /* xschem window *sending* selected objects
       when the pointer comes back in abort copy operation since it has been done
       in another xschem window; STARTCOPY set and selection file does not exist any more */
    if( stat(sel_or_clip, &buf)  && (ui_state & STARTCOPY) )
    {
      copy_objects(ABORT); /* also unlinks sel_or_flip file */
      unselect_all();
    }
    /* xschem window *receiving* selected objects */
    /* no selected objects and selection file exists */
    if(lastselected == 0  && !stat(sel_or_clip, &buf)) {
      dbg(2, "callback(): Enter event\n");
      mousex_snap = 490;
      mousey_snap = -340;
      merge_file(1, ".sch");
      xunlink(sel_or_clip);
    }
    break;

  case Expose:
    XCopyArea(display, save_pixmap, window, gctiled, mx,my,button,aux,mx,my);
    {
      XRectangle xr[1];
      xr[0].x=mx;
      xr[0].y=my;
      xr[0].width=button;
      xr[0].height=aux;
      /* redraw selection on expose, needed if no backing store available on the server 20171112 */
      XSetClipRectangles(display, gc[SELLAYER], 0,0, xr, 1, Unsorted);
      rebuild_selected_array();
      draw_selection(gc[SELLAYER],0);
      XSetClipMask(display, gc[SELLAYER], None);
    }
    dbg(1, "callback(): Expose\n");
    break;
  case ConfigureNotify:
    resetwin(1, 1, 0);
    draw();
    break;

  case MotionNotify:
    if(semaphore >= 2) break;
#ifndef __unix__
    if ((ui_state & STARTWIRE) || (ui_state & STARTARC) || (ui_state & STARTLINE) || (ui_state & STARTMOVE) ||
      (ui_state & STARTCOPY) || (ui_state & STARTRECT) || (ui_state & STARTPOLYGON) ||
      (ui_state & STARTPAN2) || (ui_state & STARTPAN) || (ui_state & STARTSELECT)) {
      XCopyArea(display, save_pixmap, window, gctiled, xrect[0].x, xrect[0].y,
        xrect[0].width, xrect[0].height, xrect[0].x, xrect[0].y);
    }
#endif
    if(ui_state & STARTPAN2)   pan2(RUBBER, mx, my); /* 20121123 -  20160425 moved up */
    if(ui_state) {
      #ifdef TURBOX_FIX
      /* fix Exceed TurboX bugs when drawing with pixmap tiled fill pattern */
      /* *NOT* a solution but at least makes the program useable. 20171130 */
      XSetClipRectangles(display, gctiled, 0,0, xrect, 1, Unsorted);
      #endif
      my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d w=%.16g h=%.16g",
        mousex_snap, mousey_snap,
        lastselected ,
        mousex_snap-mx_double_save, mousey_snap-my_double_save
      );
      statusmsg(str,1);
    }
    if(ui_state & STARTPAN)    pan(RUBBER);
    if(ui_state & STARTZOOM)   zoom_box(RUBBER);
    if(ui_state & STARTSELECT && !(ui_state & PLACE_SYMBOL) && !(ui_state & STARTPAN2)) {
      if( (state & Button1Mask)  && (state & Mod1Mask)) { /* 20171026 added unselect by area  */
          select_rect(RUBBER,0);
      } else if(state & Button1Mask) {
          select_rect(RUBBER,1);
      }
    }
    if(ui_state & STARTWIRE) {
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
      new_wire(RUBBER, mousex_snap, mousey_snap);
    }
    if(ui_state & STARTARC) {
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
      new_arc(RUBBER, 0);
    }
    if(ui_state & STARTLINE) {
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
      new_line(RUBBER);
    }
    if(ui_state & STARTMOVE) {
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
      move_objects(RUBBER,0,0,0);
    }
    if(ui_state & STARTCOPY) {
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
      copy_objects(RUBBER);
    }
    if(ui_state & STARTRECT) new_rect(RUBBER);
    if(ui_state & STARTPOLYGON) {
      if(horizontal_move) mousey_snap = my_double_save;
      if(vertical_move) mousex_snap = mx_double_save;
      new_polygon(RUBBER);
    }
    if(!(ui_state & STARTPOLYGON) && (state&Button1Mask) && !(ui_state & STARTWIRE) && !(ui_state & STARTPAN2) &&
         !(state & Mod1Mask) && !(state & ShiftMask) && !(ui_state & PLACE_SYMBOL)) /* start of a mouse area select */
    {
      static int onetime=0;
      if(mx != mx_save || my != my_save) {
        if( !(ui_state & STARTSELECT)) {
          select_rect(START,1);
          onetime=1;
        }
        if(abs(mx-mx_save) > 8 || abs(my-my_save) > 8 ) { /* set some reasonable threshold before unselecting */
          if(onetime) {
            unselect_all(); /* 20171026 avoid multiple calls of unselect_all() */
            onetime=0;
          }
          ui_state|=STARTSELECT; /* set it again cause unselect_all() clears it... 20121123 */
        }
      }
    }
    if((state & Button1Mask)  && (state & Mod1Mask) && !(state & ShiftMask) &&
       !(ui_state & STARTPAN2) && !(ui_state & PLACE_SYMBOL)) { /* 20150927 unselect area */
      if( !(ui_state & STARTSELECT)) {
        select_rect(START,0);
      }
    }
    else if((state&Button1Mask) && (state & ShiftMask) && !(ui_state & PLACE_SYMBOL) &&
             !(ui_state & STARTPAN2) ) {
      if(mx != mx_save || my != my_save) {
        if( !(ui_state & STARTSELECT)) {
          select_rect(START,1);
        }
        if(abs(mx-mx_save) > 8 || abs(my-my_save) > 8 ) {  /* set some reasonable threshold before unselecting */
          select_object(X_TO_XSCHEM(mx_save), Y_TO_XSCHEM(my_save), 0, 0); /* remove near object if dragging */
          rebuild_selected_array();
        }
      }
    }
    break;
  case KeyRelease:
    break;
  case KeyPress:
   if(key==' ') {
     if(ui_state & STARTWIRE) { /*  & instead of == 20190409 */
       new_wire(RUBBER|CLEAR, mousex_snap, mousey_snap);
       manhattan_lines++;
       manhattan_lines %=3;
       new_wire(RUBBER, mousex_snap, mousey_snap);

     } else if(ui_state==STARTLINE) {
       new_line(RUBBER|CLEAR);
       manhattan_lines++;
       manhattan_lines %=3;
       new_line(RUBBER);
     } else {
       if(semaphore<2) {
         rebuild_selected_array();
         if(lastselected==0) ui_state &=~SELECTION;
       }
       pan2(START, mx, my);
       ui_state |= STARTPAN2;
     }
     break;
   }
   if(key == '_' )              /* toggle change line width */
   {
    change_lw =!change_lw;
    if(change_lw) {
        tcleval("alert_ { enabling change line width} {}");
        tclsetvar("change_lw","1");
    }
    else {
        tcleval("alert_ { disabling change line width} {}");
        tclsetvar("change_lw","0");
    }
    break;
   }
   if(key == 'b' && state==ControlMask)         /* toggle show text in symbol */
   {
    sym_txt =!sym_txt;
    if(sym_txt) {
        /* tcleval("alert_ { enabling text in symbol} {}"); */
        tclsetvar("sym_txt","1");
        draw();
    }
    else {
        /* tcleval("alert_ { disabling text in symbol} {}"); */
        tclsetvar("sym_txt","0");
        draw();
    }
    break;
   }
   if(key == '%' )              /* toggle draw grid */
   {
    draw_grid =!draw_grid;
    if(draw_grid) {
        /* tcleval("alert_ { enabling draw grid} {}"); */
        tclsetvar("draw_grid","1");
        draw();
    }
    else {
        /* tcleval("alert_ { disabling draw grid} {}"); */
        tclsetvar("draw_grid","0");
        draw();
    }
    break;
   }
   if(key == 'j'  && state==0 )                 /* print list of highlight nets */
   {
     if(semaphore >= 2) break;
     print_hilight_net(1);
     break;
   }
   if(key == 'j'  && state==ControlMask)        /* create ipins from highlight nets */
   {
     if(semaphore >= 2) break;
     print_hilight_net(0);
     break;
   }
   if(key == 'j'  && state==Mod1Mask)   /* create labels without i prefix from hilight nets */
   {
     if(semaphore >= 2) break;
     print_hilight_net(4);
     break;
   }
   if(key == 'J'  && state==(Mod1Mask | ShiftMask) )    /* create labels with i prefix from hilight nets 20120913 */
   {
     if(semaphore >= 2) break;
     print_hilight_net(2);
     break;
   }
   if(key == 'h'  && state==ControlMask )       /* 20161102 go to http link */
   {
     launcher();
     break;
   }
   if(key == 'h'  && state==Mod1Mask)   /* create symbol pins from schematic pins 20171208 */
   {
     tcleval("schpins_to_sympins");
     break;
   }
   if(key == 'h' && state == 0) {
     /* horizontally constrained drag 20171023 */
     if ( horizontal_move ) {
       tcleval("set horizontal_move 0" );
     } else {
       tcleval("set horizontal_move 1" );
       tcleval("xschem set horizontal_move");
     }
     if(ui_state & STARTWIRE) {
       if(horizontal_move) mousey_snap = my_double_save;
       if(vertical_move) mousex_snap = mx_double_save;
       new_wire(RUBBER, mousex_snap, mousey_snap);
     }
     if(ui_state & STARTLINE) {
       if(horizontal_move) mousey_snap = my_double_save;
       if(vertical_move) mousex_snap = mx_double_save;
       new_line(RUBBER);
     }
     break;
   }
   if(key=='H' && state==ShiftMask) {           /* attach labels to selected instances */
    attach_labels_to_inst();
    break;
   }
   if(key == 'v' && state==0) {
     /* vertically constrained drag 20171023 */
     if ( vertical_move ) {
       tcleval("set vertical_move 0" );
     } else {
       tcleval("set vertical_move 1" );
       tcleval("xschem set vertical_move");
     }
     if(ui_state & STARTWIRE) {
       if(horizontal_move) mousey_snap = my_double_save;
       if(vertical_move) mousex_snap = mx_double_save;
       new_wire(RUBBER, mousex_snap, mousey_snap);
     }
     if(ui_state & STARTLINE) {
       if(horizontal_move) mousey_snap = my_double_save;
       if(vertical_move) mousex_snap = mx_double_save;
       new_line(RUBBER);
     }
     break;
   }
   if(key == 'j'  && state == (ControlMask | Mod1Mask) )  /* print list of highlight net with label expansion */
   {
     print_hilight_net(3);
     break;
   }
   if(key == 'J' && state==ShiftMask) {
    create_plot_cmd(NGSPICE);
    break;
   }
   if(key == '$'  && ( state == ShiftMask) )            /* toggle pixmap  saving */
   {
    draw_pixmap =!draw_pixmap;
    if(draw_pixmap) tcleval("alert_ { enabling draw pixmap} {}");
    else tcleval("alert_ { disabling draw pixmap} {}");
    break;
   }
   if(key == '$' && (state &ControlMask) )              /* toggle window  drawing */
   {
    draw_window =!draw_window;
    if(draw_window) {
      tcleval("alert_ { enabling draw window} {}");
      tclsetvar("draw_window","1");
    } else {
      tcleval("alert_ { disabling draw window} {}");
      tclsetvar("draw_window","0");
    }
    break;
   }
   if(key == '='  && (state &ControlMask))              /* toggle fill rectangles */
   {
    int x;
    fill++;
    if(fill==3) fill=0;

    if(fill==1) {
     tcleval("alert_ { Stippled pattern fill} {}");
     for(x=0;x<cadlayers;x++) {
       if(fill_type[x]==1) XSetFillStyle(display,gcstipple[x],FillSolid);
       else XSetFillStyle(display,gcstipple[x],FillStippled);
     }
    }
    else if(fill==2) {
     tcleval("alert_ { solid pattern fill} {}");
     for(x=0;x<cadlayers;x++)
      XSetFillStyle(display,gcstipple[x],FillSolid);
    }
    else  {
     tcleval("alert_ { No pattern fill} {}");
     for(x=0;x<cadlayers;x++)
      XSetFillStyle(display,gcstipple[x],FillStippled);
    }

    draw();
    break;
   }
   if(key == '+'  && state&ControlMask)         /* change line width */
   {
    xctx->lw+=0.1;
    change_linewidth(xctx->lw);
    draw();
    break;
   }

   if(key == '-'  && state&ControlMask)         /* change line width */
   {
    xctx->lw-=0.1;if(xctx->lw<0.0) xctx->lw=0.0;
    change_linewidth(xctx->lw);
    draw();
    break;
   }
   if(key == 'X' && state == ShiftMask) /* highlight discrepanciens between selected instance pin and net names */
   {
     hilight_net_pin_mismatches();
     break;
   }
   if(key== 'W' && state == ShiftMask) {  /* create wire snapping to closest instance pin */
     double x, y;
     int xx, yy;
     if(semaphore >= 2) break;
     if(!(ui_state & STARTWIRE)){
       find_closest_net_or_symbol_pin(mousex, mousey, &x, &y);
       xx = X_TO_SCREEN(x);
       yy = Y_TO_SCREEN(y);
       mx_save = xx; my_save = yy;
       mx_double_save = ROUND(x / cadsnap) * cadsnap;
       my_double_save = ROUND(y / cadsnap) * cadsnap;
       new_wire(PLACE, x, y);
     }
     else {
       find_closest_net_or_symbol_pin(mousex, mousey, &x, &y);
       new_wire(RUBBER, x, y);
       new_wire(PLACE|END, x, y);
       horizontal_move = vertical_move=0;
       tcleval("set vertical_move 0; set horizontal_move 0" );
     }
     break;
   }
   if(key == 'w'&& state==0)    /* place wire. */
   {
     if(semaphore >= 2) break;
     start_wire(mx, my);
     break;
   }
   if(key == XK_Return && (state == 0 ) && ui_state & STARTPOLYGON) { /* close polygon */
    new_polygon(ADD|END);
    break;
   }
   if(key == XK_Escape )                        /* abort & redraw */
   {
    no_draw = 0;
    if(semaphore >= 2) break;
    tcleval("set vertical_move 0; set horizontal_move 0" );
    last_command=0;
    manhattan_lines = 0;
    horizontal_move = vertical_move = 0;
    dbg(1, "callback(): Escape: ui_state=%ld\n", ui_state);
    if(ui_state & STARTMOVE)
    {
     move_objects(ABORT,0,0,0);
     if(ui_state & START_SYMPIN) {
       delete();
       ui_state &= ~START_SYMPIN;
     }
     break;
    }
    if(ui_state & STARTCOPY)
    {
     copy_objects(ABORT);
     break;
    }
    if(ui_state & STARTMERGE) {
      delete();
      set_modify(0); /* aborted merge: no change, so reset modify flag set by delete() */
    }

    ui_state = 0;
    unselect_all();
    draw();
    break;
   }
   if(key=='z' && state == 0)                   /* zoom box */
   {
    dbg(1, "callback(): zoom_box call\n");
    zoom_box(START);break;
   }
   if(key=='Z' && state == ShiftMask)                   /* zoom in */
   {
    view_zoom(0.0); break;
   }
   if(key=='p' && state == Mod1Mask)                           /* add symbol pin */
   {
    unselect_all();
    storeobject(-1, mousex_snap-2.5, mousey_snap-2.5, mousex_snap+2.5, mousey_snap+2.5,
                xRECT, PINLAYER, SELECTED, "name=XXX\ndir=inout");
    need_rebuild_selected_array=1;
    rebuild_selected_array();
    move_objects(START,0,0,0);
    ui_state |= START_SYMPIN;
    break;
   }
   if(key=='w' && !ui_state && state==ControlMask)              /* start polygon, 20171115 */
   {
     if(semaphore >= 2) break;
     dbg(1, "callback(): start polygon\n");
     mx_save = mx; my_save = my;
     mx_double_save=mousex_snap;
     my_double_save=mousey_snap;
     last_command = 0;
     new_polygon(PLACE);
     break;
   }
   if(key=='p' && state == ControlMask)                         /* pan */
   {
    pan(START);break;
   }
   if(key=='P' && state == ShiftMask)                   /* pan, other way to. */
   {
    xctx->xorigin=-mousex_snap+areaw*xctx->zoom/2.0;xctx->yorigin=-mousey_snap+areah*xctx->zoom/2.0;
    draw();
    break;
   }
   if(key=='5' && state == 0) { /* 20110112 display only probes */
    toggle_only_probes();
    break;
   }  /* /20110112 */
   if(key<='9' && key >='0' && state==ControlMask)              /* choose layer */
   {
    char n[30];
    rectcolor = key - '0'+4;
    my_snprintf(n, S(n), "%d", rectcolor);
    Tcl_VarEval(interp, "xschem set rectcolor ", n, "; reconfigure_layers_button", NULL);
    dbg(1, "callback(): new color: %d\n",color_index[rectcolor]);
    break;
   }
   if(key==XK_Delete && (ui_state & SELECTION) )        /* delete objects */
   {
     if(semaphore >= 2) break;
     delete();break;
   }
   if(key==XK_Right)                    /* left */
   {
    xctx->xorigin+=-CADMOVESTEP*xctx->zoom;
    draw();
    break;
   }
   if(key==XK_Left)                     /* right */
   {
    xctx->xorigin-=-CADMOVESTEP*xctx->zoom;
    draw();
    break;
   }
   if(key==XK_Down)                     /* down */
   {
    xctx->yorigin+=-CADMOVESTEP*xctx->zoom;
    draw();
    break;
   }
   if(key==XK_Up)                       /* up */
   {
    xctx->yorigin-=-CADMOVESTEP*xctx->zoom;
    draw();
    break;
   }
   if(key=='q' && state == ControlMask) /* exit */
   {
     if(semaphore >= 2) break;
     if(modified) {
       tcleval("tk_messageBox -type okcancel -message {UNSAVED data: want to exit?}");
       if(strcmp(tclresult(),"ok")==0) {
         tcleval( "exit");
       }
     }
     else {
       tcleval( "exit");
     }
     break;
   }
   if(key=='t' && state == 0)                        /* place text */
   {
     if(semaphore >= 2) break;
     last_command = 0;
     place_text(1, mousex_snap, mousey_snap); /* 1 = draw text 24122002 */
     break;
   }
   if(key=='r' && !ui_state && state==0)              /* start rect */
   {
    dbg(1, "callback(): start rect\n");
    mx_save = mx; my_save = my;
    mx_double_save=mousex_snap;
    my_double_save=mousey_snap;
    last_command = 0;
    new_rect(PLACE);
    break;
   }
   if(key=='V' && state == ShiftMask)                           /* toggle spice/vhdl netlist  */
   {
    netlist_type++; if(netlist_type==6) netlist_type=1;
    if(netlist_type == CAD_VHDL_NETLIST)
    {
     tclsetvar("netlist_type","vhdl");
    }
    else if(netlist_type == CAD_SPICE_NETLIST)
    {
     tclsetvar("netlist_type","spice");
    }
    else if(netlist_type == CAD_VERILOG_NETLIST)
    {
     tclsetvar("netlist_type","verilog");
    }
    else if(netlist_type == CAD_TEDAX_NETLIST)
    {
     tclsetvar("netlist_type","tedax");
    }
    else if(netlist_type == CAD_SYMBOL_ATTRS)
    {
     tclsetvar("netlist_type","symbol");
    }
    break;
   }

   if(key=='s' && (state == ControlMask) )      /* save 20121201 */
   {
     if(semaphore >= 2) break;
     /* check if unnamed schematic, use saveas in this case */
     if(!strcmp(xctx->sch[xctx->currsch],"") || strstr(xctx->sch[xctx->currsch], "untitled")) {
       saveas(NULL, SCHEMATIC);
     } else {
       save(1);
     }
     break;
   }
   if(key=='s' && state == (ControlMask | Mod1Mask) )           /* save as symbol */
   {
     if(semaphore >= 2) break;
     saveas(NULL, SYMBOL);
     break;
   }
   if(key=='S' && state == (ShiftMask | ControlMask)) /* save as schematic */
   {
     if(semaphore >= 2) break;
     saveas(NULL, SCHEMATIC);
     break;
   }
   if(key=='e' && state == 0)           /* descend to schematic */
   {
    if(semaphore >= 2) break;
    descend_schematic(0);break;
   }
   if(key=='e' && state == Mod1Mask)            /* edit schematic in new window */
   {
    schematic_in_new_window();
    break;
   }
   if(key=='i' && state == Mod1Mask)            /* edit symbol in new window */
   {
    symbol_in_new_window();
    break;
   }
   if( (key=='e' && state == ControlMask) || (key==XK_BackSpace))  /* back */
   {
    if(semaphore >= 2) break;
    go_back(1);break;
   }

   if(key=='a' && state == 0)   /* make symbol */
   {
    if(semaphore >= 2) break;
    tcleval("tk_messageBox -type okcancel -message {do you want to make symbol view ?}");
    if(strcmp(tclresult(),"ok")==0)
    {
     save_schematic(xctx->sch[xctx->currsch]);
     make_symbol();
    }
    break;
   }
   if(key=='a' && state == ControlMask)         /* select all */
   {
    select_all();
    break;
   }
   if(key=='y' && state == 0)                           /* toggle stretching */
   {
    enable_stretch=!enable_stretch;

    if(enable_stretch) {
        tcleval("alert_ { enabling stretch mode } {}");
        tclsetvar("enable_stretch","1");
    }
    else {
        tcleval("alert_ { disabling stretch mode } {}");
        tclsetvar("enable_stretch","0");
    }
    break;
   }
   if(key=='x' && state == ControlMask) /* cut into clipboard */
   {
    if(semaphore >= 2) break;
    rebuild_selected_array();
    if(lastselected) {  /* 20071203 check if something selected */
      save_selection(2);
      delete();
    }
    break;
   }
   if(key=='c' && state == ControlMask)   /* save clipboard */
   {
     if(semaphore >= 2) break;
     rebuild_selected_array();
     if(lastselected) {  /* 20071203 check if something selected */
       save_selection(2);
     }
    break;
   }
   if(key=='C' && state == ShiftMask)   /* place arc */
   {
     if(semaphore >= 2) break;
     mx_save = mx; my_save = my;
     mx_double_save=mousex_snap;
     my_double_save=mousey_snap;
     last_command = 0;
     new_arc(PLACE, 180.);
     break;
   }
   if(key=='C' && state == (ControlMask|ShiftMask))   /* place circle */
   {
     if(semaphore >= 2) break;
     mx_save = mx; my_save = my;
     mx_double_save=mousex_snap;
     my_double_save=mousey_snap;
     last_command = 0;
     new_arc(PLACE, 360.);
     break;
   }
   if(key=='O' && state == ShiftMask)   /* Toggle light/dark colorscheme 20171113 */
   {
     dark_colorscheme=!dark_colorscheme;
     tclsetvar("dark_colorscheme", dark_colorscheme ? "1" : "0");
     color_dim=0.0;
     build_colors(color_dim);
     draw();
     break;
   }
   if(key=='v' && state == ControlMask)   /* load clipboard */
   {
    if(semaphore >= 2) break;
    merge_file(2,".sch");
    break;
   }
   if(key=='Q' && state == (ControlMask | ShiftMask) ) /* view prop */
   {
    edit_property(2);break;
   }
   if(key=='q' && state==0)                     /* edit prop */
   {
    if(semaphore >= 2) break;
    edit_property(0);
    break;
   }
   if(key=='q' && state==Mod1Mask)                      /* edit .sch file (DANGER!!) */
   {
    if(semaphore >= 2) break;
    rebuild_selected_array();
    if(lastselected==0 ) {
      my_snprintf(str, S(str), "edit_file {%s}", abs_sym_path(xctx->sch[xctx->currsch], ""));
      tcleval(str);
    }
    else if(selectedgroup[0].type==ELEMENT) {
      my_snprintf(str, S(str), "edit_file {%s}",
         abs_sym_path(xctx->inst[selectedgroup[0].n].name, ""));
      tcleval(str);

    }
    break;
   }
   if(key=='Q' && state == ShiftMask)                           /* edit prop with vim */
   {
    if(semaphore >= 2) break;
    edit_property(1);break;
   }
   if(key=='i' && state==0)                     /* descend to  symbol */
   {
    if(semaphore >= 2) break;
    descend_symbol();break;
   }
   if(key==XK_Insert || (key == 'I' && state == ShiftMask) ) /* insert sym */
   {
    if(semaphore >= 2) break;
    last_command = 0;
    unselect_all();

    /* place_symbol(-1,NULL,mousex_snap, mousey_snap, 0, 0, NULL,3, 1);*/
    mx_save = mx; my_save = my;
    mx_double_save = mousex_snap;
    my_double_save = mousey_snap;
    if(place_symbol(-1,NULL,mousex_snap, mousey_snap, 0, 0, NULL, 4, 1) ) {
      mousey_snap = my_double_save;
      mousex_snap = mx_double_save;
      move_objects(START,0,0,0);
      ui_state |= PLACE_SYMBOL;
    }
    break;
   }
   if(key=='s' && state & Mod1Mask)                     /* reload */
   {
    if(semaphore >= 2) break;
     tcleval("tk_messageBox -type okcancel -message {Are you sure you want to reload from disk?}");
     if(strcmp(tclresult(),"ok")==0) {
        char filename[PATH_MAX];
        unselect_all();
        remove_symbols();
        my_strncpy(filename, abs_sym_path(xctx->sch[xctx->currsch], ""), S(filename));
        load_schematic(1, filename, 1);
        draw();
     }
     break;
   }
   if(key=='o' && state == ControlMask)   /* load */
   {

    if(semaphore >= 2) break;
    tcleval("catch { ngspice::resetdata }");
    ask_new_file();
    break;
   }
   if(key=='S' && state == ShiftMask)   /* change element order */
   {
    if(semaphore >= 2) break;
    change_elem_order();
    break;
   }
   if(key=='k' && state==Mod1Mask)        /* select whole net (all attached wires/labels/pins) */
   {
     select_hilight_net();
     break;
   }
   if(key=='k' && state==ControlMask)                           /* unhilight net */
   {
    xRect boundbox;
    int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
    if(semaphore >= 2) break;
    if(!big) calc_drawing_bbox(&boundbox, 2);
    unhilight_net();
    /* undraw_hilight_net(1); */
    if(!big) {
      bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
      bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    }
    draw();
    if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    break;
   }
   if(key=='K' && state==(ControlMask|ShiftMask))       /* hilight net drilling thru elements  */
                                                        /* with 'propagate_to' prop set on pins */
   {
    if(semaphore >= 2) break;
    enable_drill=1;
    hilight_net(0);
    redraw_hilights();
    /* draw_hilight_net(1); */
    break;
   }
   if(key=='k' && state==0)                             /* hilight net */
   {
    if(semaphore >= 2) break;
    enable_drill=0;
    hilight_net(0);
    redraw_hilights();
    /* draw_hilight_net(1); */
    break;
   }
   if(key=='K' && state == ShiftMask)                           /* delete hilighted nets */
   {
    xRect boundbox;
    int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
    if(semaphore >= 2) break;
    enable_drill=0;
    if(!big) calc_drawing_bbox(&boundbox, 2);
    delete_hilight_net();
    /* undraw_hilight_net(1); */
    if(!big) {
      bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
      bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    }
    draw();
    if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    break;
   }
   if(key=='g' && state==Mod1Mask) { /* highlight net and send to gaw viewer */
     if(semaphore >= 2) break;
     enable_drill=0;
     hilight_net(GAW);
     redraw_hilights();
     break;
   }
   if(key=='g' && state==0)                         /* half snap factor */
   {
    set_snap(cadsnap / 2.0);
    break;
   }
   if(key=='g' && state==ControlMask)              /* set snap factor 20161212 */
   {
    my_snprintf(str, S(str),
     "input_line {Enter snap value (default: %.16g current: %.16g)}  {xschem set cadsnap} {%g} 10",
     CADSNAP, cadsnap, cadsnap);
    tcleval(str);
    break;
   }
   if(key=='G' && state==ShiftMask)                                    /* double snap factor */
   {
    set_snap(cadsnap * 2.0);
    break;
   }
   if(key=='*' && state==(Mod1Mask|ShiftMask) )         /* svg print , 20121108 */
   {
    if(semaphore >= 2) break;
    svg_draw();
    break;
   }
   if(key=='*' && state==ShiftMask )                    /* postscript print */
   {
    if(semaphore >= 2) break;
    ps_draw();
    break;
   }
   if(key=='*' && state==(ControlMask|ShiftMask) )      /* xpm print */
   {
    if(semaphore >= 2) break;
    print_image();
    break;
   }
   if(key=='u' && state==Mod1Mask)                      /* align to grid */
   {
    if(semaphore >= 2) break;
    push_undo();
    round_schematic_to_grid(cadsnap);
    set_modify(1);
    prepared_hash_instances=0;
    prepared_hash_wires=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;

    draw();
    break;
   }
   if(0 && (key=='u') && (state==ControlMask))                   /* testmode */
   {
    int mult;
    remove_symbol(2);
    link_symbols_to_instances();
    expandlabel("/RST", &mult);
    expandlabel("/CCC[3:0]", &mult);
    expandlabel("CCC[AA:BB:DD]", &mult);
    expandlabel("CCC[9:1:2]", &mult);
    expandlabel("CCC[10:BB:DD]", &mult);
    expandlabel("CCC[10..BB..DD]", &mult);
    expandlabel("CCC[10..0..2]", &mult);
    expandlabel("123", &mult);
    expandlabel("123AA", &mult);
    break;
   }
   if(key=='u' && state==0)                             /* undo */
   {
    if(semaphore >= 2) break;
    pop_undo(0);
    draw();
    break;
   }
   if(key=='U' && state==ShiftMask)                     /* redo */
   {
    if(semaphore >= 2) break;
    pop_undo(1);
    draw();
    break;
   }
   if(key=='&')                         /* check wire connectivity */
   {
    if(semaphore >= 2) break;
    push_undo();
    trim_wires();
    draw();
    break;
   }
   if(key=='l' && state == ControlMask) { /* create schematic from selected symbol 20171004 */

     if(semaphore >= 2) break;
     create_sch_from_sym();
     break;
   }
   if(key=='l' && state == 0) /* start line */
   {
    start_line(mx, my);
    break;
   }
   if(key=='l' && state == Mod1Mask) {                         /* add pin label*/
    place_net_label(1);
    break;
   }
   if(key=='L' && state == (Mod1Mask | ShiftMask)) {                         /* add pin label*/
    place_net_label(0);
    break;
   }
   if(key=='F' && state==ShiftMask)                     /* Flip */
   {
    if(ui_state & STARTMOVE) move_objects(FLIP,0,0,0);
    else if(ui_state & STARTCOPY) copy_objects(FLIP);
    else {
      rebuild_selected_array();
      mx_save = mx; my_save = my;
      mx_double_save=mousex_snap;
      my_double_save=mousey_snap;
      move_objects(START,0,0,0);
      if(lastselected>1) move_objects(FLIP,0,0,0);
      else               move_objects(FLIP|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    break;
   }
   if(key=='\\' && state==0)          /* Fullscreen */
   {
    dbg(1, "callback(): toggle fullscreen\n");
    toggle_fullscreen();
    break;
   }
   if(key=='f' && state==Mod1Mask)              /* flip objects around their anchor points 20171208 */
   {
    if(ui_state & STARTMOVE) move_objects(FLIP|ROTATELOCAL,0,0,0);
    else if(ui_state & STARTCOPY) copy_objects(FLIP|ROTATELOCAL);
    else {
      rebuild_selected_array();
      mx_save = mx; my_save = my;
      mx_double_save=mousex_snap;
      my_double_save=mousey_snap;
      move_objects(START,0,0,0);
      move_objects(FLIP|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    break;
   }
   if(key=='R' && state==ShiftMask)             /* Rotate */
   {
    if(ui_state & STARTMOVE) move_objects(ROTATE,0,0,0);
    else if(ui_state & STARTCOPY) copy_objects(ROTATE);
    else {
      rebuild_selected_array();
      mx_save = mx; my_save = my;
      mx_double_save=mousex_snap;
      my_double_save=mousey_snap;
      move_objects(START,0,0,0);
      if(lastselected>1) move_objects(ROTATE,0,0,0);
      else               move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }

    break;
   }
   if(key=='r' && state==Mod1Mask)              /* Rotate objects around their anchor points 20171208 */
   {
    if(ui_state & STARTMOVE) move_objects(ROTATE|ROTATELOCAL,0,0,0);
    else if(ui_state & STARTCOPY) copy_objects(ROTATE|ROTATELOCAL);
    else {
      rebuild_selected_array();
      mx_save = mx; my_save = my;
      mx_double_save=mousex_snap;
      my_double_save=mousey_snap;
      move_objects(START,0,0,0);
      move_objects(ROTATE|ROTATELOCAL,0,0,0);
      move_objects(END,0,0,0);
    }
    break;
   }
   if(key=='m' && state==0 && !(ui_state & (STARTMOVE | STARTCOPY)))/* move selected obj. */
   {
    mx_save = mx; my_save = my;
    mx_double_save=mousex_snap;
    my_double_save=mousey_snap;
    move_objects(START,0,0,0);
    break;
   }

   if(key=='c' && state==0 &&           /* copy selected obj.  */
     !(ui_state & (STARTMOVE | STARTCOPY)))
   {
    if(semaphore >= 2) break;
    mx_save = mx; my_save = my;
    mx_double_save=mousex_snap;
    my_double_save=mousey_snap;
    copy_objects(START);
    break;
   }
   if(key=='n' && state==ControlMask)              /* New schematic */
   {
     if(semaphore >= 2) break;
     tcleval("xschem clear SCHEMATIC");
   }
   if(key=='N' && state==(ShiftMask|ControlMask) )    /* New symbol */
   {
     if(semaphore >= 2) break;
     tcleval("xschem clear SYMBOL");
   }
   if(key=='N' && state==ShiftMask)              /* hierarchical netlist */
   {
    yyparse_error = 0;
    if(semaphore >= 2) break;
    unselect_all();
    if(set_netlist_dir(0, NULL)) {
      dbg(1, "callback(): -------------\n");
      if(netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(1);
      else if(netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(1);
      else if(netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(1);
      else if(netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(1);
      else
        if(has_x) tcleval("tk_messageBox -type ok -message {Please Set netlisting mode (Options menu)}");

      dbg(1, "callback(): -------------\n");
    }
    break;
   }
   if(key=='n' && state==0)              /* netlist */
   {
    yyparse_error = 0;
    if(semaphore >= 2) break;
    unselect_all();
    if( set_netlist_dir(0, NULL) ) {
      dbg(1, "callback(): -------------\n");
      if(netlist_type == CAD_SPICE_NETLIST)
        global_spice_netlist(0);
      else if(netlist_type == CAD_VHDL_NETLIST)
        global_vhdl_netlist(0);
      else if(netlist_type == CAD_VERILOG_NETLIST)
        global_verilog_netlist(0);
      else if(netlist_type == CAD_TEDAX_NETLIST)
        global_tedax_netlist(0);
      else
        if(has_x) tcleval("tk_messageBox -type ok -message {Please Set netlisting mode (Options menu)}");
      dbg(1, "callback(): -------------\n");
    }
    break;
   }
   if(key=='A' && state==ShiftMask)                             /* toggle show netlist */
   {
    netlist_show = !netlist_show;
    if(netlist_show) {
        tcleval("alert_ { enabling show netlist window} {}");
        tclsetvar("netlist_show","1");
    }
    else {
        tcleval("alert_ { disabling show netlist window } {}");
        tclsetvar("netlist_show","0");
    }
    break;
   }
   if(key=='>') {
     if(semaphore >= 2) break;
     if(draw_single_layer< cadlayers-1) draw_single_layer++;
     draw();
     break;
   }
   if(key=='<') {
     if(semaphore >= 2) break;
     if(draw_single_layer>=0 ) draw_single_layer--;
     draw();
     break;
   }
   if(key==':')                         /* toggle flat netlist (only spice)  */
   {
    flat_netlist = !flat_netlist;
    if(flat_netlist) {
        tcleval("alert_ { enabling flat netlist} {}");
        tclsetvar("flat_netlist","1");
    }
    else {
        tcleval("alert_ { set hierarchical netlist } {}");
        tclsetvar("flat_netlist","0");
    }
    break;
   }
   if(key=='b' && state==0)                     /* merge schematic */
   {
    if(semaphore >= 2) break;
    merge_file(0, ""); /* 2nd parameter not used any more for merge 25122002 */
    break;
   }
   if(key=='b' && state==Mod1Mask)                     /* hide/show instance details */
   {
    if(semaphore >= 2) break;
    hide_symbols++;
    if(hide_symbols >= 3) hide_symbols = 0;
    tclsetvar("hide_symbols", hide_symbols == 2 ? "2" : hide_symbols == 1 ? "1" : "0");
    draw();
    break;
   }
   if(key=='D' && state==ShiftMask)                     /* delete files */
   {
    if(semaphore >= 2) break;
    delete_files();
    break;
   }
   if(key=='x' && state == 0 )                  /* new cad session */
   {
    char * tmp;
    tmp = (char *) tclgetvar("XSCHEM_START_WINDOW");
    if(tmp && tmp[0]) new_window(abs_sym_path(tmp, "") ,0);
    else new_window(NULL, 0);
    break;
   }
   if((key=='#') && !(state & ControlMask) )
   {
    check_unique_names(0);
    break;
   }
   if((key=='#') && (state & ControlMask) )
   {
    check_unique_names(1);
    break;
   }
   if( 0 && (key==';') && (state & ControlMask) )    /* testmode:  for performance testing */
   {
    draw_stuff();
    draw();
    break;
   }
   if(0 && key=='~' && (state & ControlMask)) {  /* testmode */
    break;
   }
   if(0 && key=='|' && !(state & ControlMask)) {            /* testmode */
    break;
   }
   if(0 && key=='|' && (state & ControlMask))      /* testmode */
   {
    double w, x1, y1, x2, y2;
    int i;
    int snap=100;

    clear_drawing();
    for(i=0;i<5000; i++) {
      w = (1+ROUND(rand()%1200/snap))*snap;
      x1 = 40000+ROUND(rand()%30000/snap)*snap;
      y1 = 40000+ROUND(rand()%30000/snap)*snap;
      x2=x1+w;
      y2=y1;
      ORDER(x1, y1, x2, y2);
      storeobject(-1, x1, y1, x2, y2 ,WIRE,0,0,NULL);
    }
    for(i=0;i<5000; i++) {
      w = (1+ROUND(rand()%1200/snap))*snap;
      x1 = 40000+ROUND(rand()%30000/snap)*snap;
      y1 = 40000+ROUND(rand()%30000/snap)*snap;
      x2=x1;
      y2=y1+w;
      ORDER(x1, y1, x2, y2);
      storeobject(-1, x1, y1, x2, y2, WIRE,0,0,NULL);
    }
    prepared_netlist_structs = 0;
    prepared_hilight_structs = 0;
    prepared_hash_wires = 0;
    zoom_full(1, 0);
    break;
   }

   if(key=='f' && state == ControlMask)         /* search */
   {
    if(semaphore >= 2) break;
    tcleval("property_search");
    break;
   }
   if(key=='f' && state == 0 )                  /* full zoom */
   {
    zoom_full(1, 0);
    break;
   }
   if((key=='z' && state==ControlMask))                         /* zoom out */
   {
     view_unzoom(0.0);
     break;
   }
   if(key=='!')
   {
     if(semaphore >= 2) break;
     break_wires_at_pins();
     break;
   }
   break;

  case ButtonPress:                     /* end operation */
   dbg(1, "callback(): ButtonPress  ui_state=%ld state=%d\n",ui_state,state);
   if(ui_state & STARTPAN2) {
     ui_state &=~STARTPAN2;
     mx_save = mx; my_save = my;
     mx_double_save=mousex_snap;
     my_double_save=mousey_snap;

     break;
   }
   if(button==Button5 && state == 0 ) view_unzoom(CADZOOMSTEP);
   else if(button == Button3 && semaphore <2) {
     if(!(ui_state & STARTPOLYGON) && !(state & Mod1Mask) ) {
       last_command = 0;
       unselect_all();
       select_object(mousex,mousey,SELECTED, 1);
       rebuild_selected_array();
       if(state & ShiftMask) {
         edit_property(1);
       } else {
         edit_property(0);
       }
     }

   }
   else if(button==Button4 && state == 0 ) view_zoom(CADZOOMSTEP);

   else if(button==Button4 && (state & ShiftMask) && !(state & Button2Mask)) {
    xctx->xorigin+=-CADMOVESTEP*xctx->zoom/2.;
    draw();
   }
   else if(button==Button5 && (state & ShiftMask) && !(state & Button2Mask)) {
    xctx->xorigin-=-CADMOVESTEP*xctx->zoom/2.;
    draw();
   }
   else if(button==Button4 && (state & ControlMask) && !(state & Button2Mask)) {
    xctx->yorigin+=-CADMOVESTEP*xctx->zoom/2.;
    draw();
   }
   else if(button==Button5 && (state & ControlMask) && !(state & Button2Mask)) {
    xctx->yorigin-=-CADMOVESTEP*xctx->zoom/2.;
    draw();
   }
   else if(button==Button1 && (state & Mod1Mask) ) {
     last_command = 0;
     mx_save = mx; my_save = my;
     mx_double_save=mousex_snap;
     my_double_save=mousey_snap;

     /* useless code ? 20200905 */
     /* if(semaphore<2) {
       rebuild_selected_array();
       if(lastselected==0) ui_state &=~SELECTION;
     } */

     select_object(mousex, mousey, 0, 0);
     rebuild_selected_array();
     if(lastselected==0) ui_state &=~SELECTION;
   }
   else if(button==Button2 && (state == 0)) {
     pan2(START, mx, my);
     ui_state |= STARTPAN2;
     break;
   }
   else if(semaphore >= 2) { /* button1 click to select another instance while edit prop dialog open */
     if(button==Button1 && state==0 && tclgetvar("edit_symbol_prop_new_sel")[0]) {
       tcleval("set edit_symbol_prop_new_sel 1; .dialog.f1.b1 invoke"); /* invoke 'OK' of edit prop dialog */
     } else if(button==Button1 && (state & ShiftMask) && tclgetvar("edit_symbol_prop_new_sel")[0]) {
       select_object(mousex, mousey, SELECTED, 0);
       rebuild_selected_array();
     }
     break;
   }
   else if(button==Button1)
   {
     if(persistent_command && last_command) {
       if(last_command == STARTLINE)  start_line(mx, my);
       if(last_command == STARTWIRE)  start_wire(mx, my);
       break;
     }
     if(!(ui_state & STARTPOLYGON) && !(ui_state & STARTWIRE) && !(ui_state & STARTLINE) ) {
       horizontal_move = vertical_move=0;
       tcleval("set vertical_move 0; set horizontal_move 0" );
     }
     if(ui_state & MENUSTARTTEXT) {
       place_text(1, mousex_snap, mousey_snap);
       ui_state &=~MENUSTARTTEXT;
       break;
     }
     if(ui_state & MENUSTARTWIRE) {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       new_wire(PLACE, mousex_snap, mousey_snap);
       ui_state &=~MENUSTARTWIRE;
       break;
     }
     if(ui_state & MENUSTARTSNAPWIRE) {
       double x, y;
       int xx, yy;

       find_closest_net_or_symbol_pin(mousex, mousey, &x, &y);
       xx = X_TO_SCREEN(x);
       yy = Y_TO_SCREEN(y);
       mx_save = xx; my_save = yy;
       mx_double_save = ROUND(x / cadsnap) * cadsnap;
       my_double_save = ROUND(y / cadsnap) * cadsnap;

       new_wire(PLACE, x, y);
       ui_state &=~MENUSTARTSNAPWIRE;
       break;
     }
     if(ui_state & MENUSTARTLINE) {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       new_line(PLACE);
       ui_state &=~MENUSTARTLINE;
       break;
     }
     if(ui_state & MENUSTARTRECT) {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       new_rect(PLACE);
       ui_state &=~MENUSTARTRECT;
       break;
     }
     if(ui_state & MENUSTARTPOLYGON) {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       new_polygon(PLACE);
       ui_state &=~MENUSTARTPOLYGON;
       break;
     }
     if(ui_state & MENUSTARTARC) {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       new_arc(PLACE, 180.);
       ui_state &=~MENUSTARTARC;
       break;
     }
     if(ui_state & MENUSTARTCIRCLE) {
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       new_arc(PLACE, 360.);
       ui_state &=~MENUSTARTCIRCLE;
       break;
     }
     if(ui_state & MENUSTARTZOOM) {
       zoom_box(START);
       ui_state &=~MENUSTARTZOOM;
       break;
     }
     if(ui_state & STARTPAN) {
       pan(END);
       break;
     }
     if(ui_state & STARTZOOM) {
       zoom_box(END);
       break;
     }
     if(ui_state & STARTWIRE) {
       if(persistent_command) {
         if(!vertical_move) {
           mx_save = mx;
           mx_double_save=mousex_snap;
         }
         if(!horizontal_move) {
           my_save = my;
           my_double_save=mousey_snap;
         }
         if(horizontal_move) mousey_snap = my_double_save;
         if(vertical_move) mousex_snap = mx_double_save;
         new_wire(PLACE, mousex_snap, mousey_snap);

       } else {
         new_wire(PLACE|END, mousex_snap, mousey_snap);
       }
       break;
     }
     if(ui_state & STARTARC) {
       new_arc(SET, 0);
       break;
     }
     if(ui_state & STARTLINE) {
       if(persistent_command) {
         if(!vertical_move) {
           mx_save = mx;
           mx_double_save=mousex_snap;
         }
         if(!horizontal_move) {
           my_save = my;
           my_double_save=mousey_snap;
         }
         if(horizontal_move) mousey_snap = my_double_save;
         if(vertical_move) mousex_snap = mx_double_save;
         new_line(PLACE);
       } else {
         new_line(PLACE|END);
       }
       break;
     }
     if(ui_state & STARTRECT) {
       new_rect(PLACE|END);
       break;
     }
     if(ui_state & STARTPOLYGON) {
       if(horizontal_move) mousey_snap = my_double_save;
       if(vertical_move) mousex_snap = mx_double_save;
       new_polygon(ADD);
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       horizontal_move = vertical_move=0;
       tcleval("set vertical_move 0; set horizontal_move 0" );
       break;
     }
     if(ui_state & STARTMOVE) {
       move_objects(END,0,0,0);
       ui_state &=~START_SYMPIN;
       break;
     }
     if(ui_state & STARTCOPY) {
       copy_objects(END);
       break;
     }
     if( !(ui_state & STARTSELECT) && !(ui_state & STARTWIRE) && !(ui_state & STARTLINE)  ) {
       int prev_last_sel = lastselected;
       mx_save = mx; my_save = my;
       mx_double_save=mousex_snap;
       my_double_save=mousey_snap;
       if( !(state & ShiftMask) && !(state & Mod1Mask) ) {
         unselect_all();
#ifndef __unix__
         XCopyArea(display, save_pixmap, window, gctiled, xrect[0].x, xrect[0].y,
           xrect[0].width, xrect[0].height, xrect[0].x, xrect[0].y);
#endif
       }
       sel = select_object(mousex, mousey, SELECTED, 0);
       rebuild_selected_array();
#ifndef __unix__
       draw_selection(gc[SELLAYER], 0); /* 20181009 moved outside of cadlayers loop */
#endif
       if(sel && state == ControlMask) {
         launcher();
       }
       if( !(state & ShiftMask) )  {
         xRect boundbox;
         if(auto_hilight && hilight_nets && sel == 0 ) { /* 20160413 20160503 */
           if(!prev_last_sel) {
             int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
             if(!big) calc_drawing_bbox(&boundbox, 2);
             delete_hilight_net();
             /* undraw_hilight_net(1); */
             if(!big) {
               bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
               bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
               bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
             }
             draw();
             if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
           }
         }
       }
       if(auto_hilight) {
         hilight_net(0);
         if(lastselected) {
           redraw_hilights();
           /* draw_hilight_net(1); */
         }
       }
       break;
     }
   } /* button==Button1 */
   break;
  case ButtonRelease:
   if(ui_state & STARTPAN2) {
     ui_state &=~STARTPAN2;
     mx_save = mx; my_save = my;
     mx_double_save=mousex_snap;
     my_double_save=mousey_snap;

     break;
   }
   dbg(1, "callback(): ButtonRelease  ui_state=%ld state=%d\n",ui_state,state);
   if(semaphore >= 2) break;
   if(ui_state & STARTSELECT) {
     if(state & ControlMask) {
       enable_stretch=1;
       select_rect(END,-1);
       enable_stretch=0;
       break;
     } else {
       /* 20150927 filter out button4 and button5 events */
       if(!(state&(Button4Mask|Button5Mask) ) ) select_rect(END,-1);
     }
     rebuild_selected_array();
     my_snprintf(str, S(str), "mouse = %.16g %.16g - selected: %d path: %s",
       mousex_snap, mousey_snap, lastselected, xctx->sch_path[xctx->currsch] );
     statusmsg(str,1);

   }
   break;
  case -3:  /* double click  : edit prop */
   if(semaphore >= 2) break;
   dbg(1, "callback(): DoubleClick  ui_state=%ld state=%d\n",ui_state,state);
   if(button==Button1) {
     if(ui_state == STARTWIRE) {
       ui_state &= ~STARTWIRE;
     }
     if(ui_state == STARTLINE) {
       ui_state &= ~STARTLINE;
     }
     if( (ui_state & STARTPOLYGON) && (state ==0 ) ) {
       new_polygon(SET);
     }
   }
#ifndef __unix__
  case MOUSE_WHEEL_UP:  /* windows do not use button4 and button5 like X */
  {
    xctx->xorigin += -CADMOVESTEP * xctx->zoom / 2.;
    draw();
  }
#endif
   break;

  default:
   dbg(1, "callback(): Event:%d\n",event);
   break;
 }

 semaphore--;
 return 0;
}

