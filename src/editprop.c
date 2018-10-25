/* File: editprop.c
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
#include <stdarg.h>

static int rot = 0, flip = 0;          

size_t my_strdup(char **dest, const char *src) // empty source string --> dest=NULL
{
 size_t len;
 if(*dest!=NULL) { 
   if(debug_var>=3) fprintf(errfp,"  my_strdup:  calling my_free %lx\n", (unsigned long)*dest);
   my_free(dest);
 }
 if(src!=NULL && src[0]!='\0')  
 {
  // 20180923
  len = strlen(src)+1;
  *dest = my_malloc(len);
  memcpy(*dest, src, len);
  // *dest=strdup(src);
  if(debug_var>=3) fprintf(errfp,"my_strdup(): duplicated %lx string %s\n",(unsigned long)*dest, src);
  return len-1;
 }
 return 0;
}

// 20171004 copy at most n chars, adding a null char at end
void my_strndup(char **dest, const char *src, int n) // empty source string --> dest=NULL

{
 if(*dest!=NULL) {
   if(debug_var>=3) fprintf(errfp,"  my_strdup:  calling my_free %lx\n", (unsigned long)*dest);
   my_free(dest);
 }
 if(src!=NULL && src[0]!='\0')
 {
  // 20180924 replace strndup()
  char *p = memchr(src, '\0', n);
  if(p) n = p - src;
  *dest = my_malloc(n+1);
  if(*dest) {
    memcpy(*dest, src, n);
    (*dest)[n] = '\0';
  }
  // *dest=strndup(src, n);

  if(debug_var>=3) fprintf(errfp,"my_strndup(): duplicated %lx string %s\n",(unsigned long)*dest, src);
 }
}


int my_snprintf(char *str, int size, const char *fmt, ...) // 20161124
{
  int  size_of_print;
  char s[200];

  va_list args;
  va_start(args, fmt);
  size_of_print = vsnprintf(str, size, fmt, args);

  if(has_x && size_of_print >=size) { // output was truncated 
    snprintf(s, S(s), "alert_ { Warning: overflow in my_snprintf print size=%d, buffer size=%d} {}",
             size_of_print, size);
    tcleval(s);
  }
  va_end(args);
  return size_of_print;
}


size_t my_strdup2(char **dest, const char *src) // 20150409 duplicates also empty string 
{
 size_t len;
 if(*dest!=NULL) {
   if(debug_var>=3) fprintf(errfp,"  my_strdup:  calling my_free %lx\n", (unsigned long)*dest);
   my_free(dest);
 }
 if(src!=NULL)
 {
  // 20180923
  len = strlen(src)+1;
  *dest = my_malloc(len);
  memcpy(*dest, src, len);
  // *dest=strdup(src);
  if(debug_var>=3) fprintf(errfp,"my_strdup2(): duplicated %lx string %s\n",(unsigned long)*dest, src);
  return len-1;
 }
 return 0;
}

size_t my_strcat(char **str, const char *append_str)
{
 size_t s, a;
 if(debug_var>=3) fprintf(errfp,"my_strcat(): str=%s  append_str=%s\n", *str, append_str);
 if( *str != NULL)
 {
  s = strlen(*str);
  if(append_str == NULL || append_str[0]=='\0') return s;
  a = strlen(append_str)+1;
  my_realloc(str, s + a );
  memcpy(*str+s, append_str, a); // 20180923
  if(debug_var>=3) fprintf(errfp,"my_strcat(): reallocated %lx, string %s\n",(unsigned long)*str, *str);
  return s + a -1;
 }
 else
 {
  if(append_str == NULL || append_str[0]=='\0') return 0;
  a = strlen(append_str) + 1;
  *str = my_malloc( a );
  memcpy(*str, append_str, a); // 20180923
  if(debug_var>=3) fprintf(errfp,"my_strcat(): allocated %lx, string %s\n",(unsigned long)*str, *str);
  return a -1;
 }
}


void *my_calloc(size_t nmemb, size_t size)
{
   void *ptr;
   if(size*nmemb > 0) {
     ptr=calloc(nmemb, size);
     if(debug_var>=3) 
       fprintf(errfp, "my_calloc(): allocating %lx ,  %lu bytes\n",
               (unsigned long)ptr, (unsigned long) (size*nmemb));
   }
   else ptr = NULL;
   return ptr;
}

void *my_malloc(size_t size) 
{
 void *ptr;
 if(size>0) {
   ptr=malloc(size);
   if(debug_var>=3) fprintf(errfp, "my_malloc(): allocating %lx , %lu bytes\n",
     (unsigned long)ptr, (unsigned long) size);
 }
 else ptr=NULL;
 return ptr;
}

void my_realloc(void *ptr,size_t size)
{
 unsigned long a;
 a = (unsigned long) *(void **)ptr;
 *(void **)ptr=realloc(*(void **)ptr,size);
 if(debug_var>=3) 
   fprintf(errfp, "my_realloc(): reallocating %lx --> %lx to %lu bytes\n",
           a, (unsigned long) *(void **)ptr,(unsigned long) size);

} 

void my_free(void *ptr)
{
 if(debug_var>=3) fprintf(errfp, "my_free():  freeing %lx\n",(unsigned long)*(void **)ptr);
 free(*(void **)ptr);
 *(void **)ptr=NULL;
}

void my_strncpy(char *d, const char *s, int n)
{
 int i=0;
 n-=1;
 if(debug_var>=3) 
   fprintf(errfp, "my_strncpy():  copying %s to %lu\n", s, (unsigned long)d);
 while( (d[i]=s[i]) )
 {
  if(i==n) { d[i]='\0'; return; }
  i++;
 }
}

void set_inst_prop(int i)
{// set inst prop from symbol template string
  char *ptr;

  ptr = (inst_ptr[i].ptr+instdef)->templ; //20150409

  if(get_tok_value(ptr,"name",0)[0]==0)  // if does not have a name field just copy 
					    // the entire template string 03102001
  {
   my_strdup(&inst_ptr[i].prop_ptr, ptr);
   my_strdup2(&inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name", 0)); // 20150409
   return;
  }


   if(debug_var>=2) fprintf(errfp, "set_inst_prop(): lookin for format string: %s\n",  ptr);
  if(ptr!=NULL) {// 03-02-2000
    new_prop_string(&inst_ptr[i].prop_ptr, ptr,0);
  } else {
    my_strdup( &inst_ptr[i].prop_ptr,NULL);
  }
  my_strdup2(&inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name",0)); // 20150409
}

// x=0 use text widget   x=1 use vim editor
void edit_text_property(int x)
{
   #ifdef HAS_CAIRO
   int customfont;
   #endif
   int sel, k, text_changed; 
   int c,l;
   double xx1,yy1,xx2,yy2;
   double pcx,pcy;	// pin center 20070317
   char property[1024];// used for float 2 string conv (xscale  and yscale) overflow safe
   char *strlayer;

   if(debug_var>=1) fprintf(errfp, "edit_text_property(): entering\n");
   sel = selectedgroup[0].n;
   if(textelement[sel].prop_ptr !=NULL)
      tclsetvar("props",textelement[sel].prop_ptr); 
   else
      tclsetvar("props",""); // 20171112

   tclsetvar("txt",textelement[sel].txt_ptr);
   tclsetvar("retval",textelement[sel].txt_ptr); // for viewdata
   my_snprintf(property, S(property), "%.16g",textelement[sel].yscale); 
   tclsetvar("vsize",property);
   my_snprintf(property, S(property), "%.16g",textelement[sel].xscale); 
   tclsetvar("hsize",property);
   if(x==0) tcleval("enter_text {text:}");
   else if(x==2) tcleval("viewdata $::retval");
   else if(x==1) tcleval("edit_vi_prop {Text:}");
   else {
     fprintf(errfp, "edit_text_property() : unknown parameter x=%d\n",x); exit(EXIT_FAILURE);
   }

   text_changed=0;
   if(x==0) {
     if( strcmp(textelement[sel].txt_ptr, tclgetvar("txt") ) ) {
       if(debug_var>=1) fprintf(errfp, "edit_text_property(): x=0, text_changed=1\n");
       text_changed=1;
     } else {
       if(debug_var>=1) fprintf(errfp, "edit_text_property(): x=0, text_changed=0\n");
       text_changed=0;
     }
   } else if(x==1) {

   // 20080804
     if( strcmp(textelement[sel].txt_ptr, tclgetvar("retval") ) ) {
       if(debug_var>=1) fprintf(errfp, "edit_text_property(): x=1, text_changed=1\n");
       text_changed=1;
     } else {
       if(debug_var>=1) fprintf(errfp, "edit_text_property(): x=1, text_changed=0\n");
       text_changed=0;
     }
   }
   if(strcmp(tclgetvar("rcode"),"") )
   {
     if(debug_var>=1) fprintf(errfp, "edit_text_property(): rcode !=\"\"\n");
     modified=1; push_undo(); // 20150327
     bbox(BEGIN,0.0,0.0,0.0,0.0);
     for(k=0;k<lastselected;k++)
     {
       if(selectedgroup[k].type!=TEXT) continue;
       sel=selectedgroup[k].n;

       rot = textelement[sel].rot;	// calculate bbox, some cleanup needed here
       flip = textelement[sel].flip;
       #ifdef HAS_CAIRO
       customfont = set_text_custom_font(&textelement[sel]);
       #endif
       text_bbox(textelement[sel].txt_ptr, textelement[sel].xscale,
                 textelement[sel].yscale, rot, flip, 
                 textelement[sel].x0, textelement[sel].y0,
                 &xx1,&yy1,&xx2,&yy2);
       #ifdef HAS_CAIRO
       if(customfont) cairo_restore(ctx);
       #endif

       bbox(ADD, xx1, yy1, xx2, yy2 );        
    
       if(debug_var>=1) fprintf(errfp, "edit_property(): text props: props=%s  text=%s\n",
         tclgetvar("props"),
         tclgetvar("txt") );
       if(text_changed) {
         if(current_type==SYMBOL) {
           c = lastrect[PINLAYER];
           for(l=0;l<c;l++) {
             if(!strcmp( (get_tok_value(rect[PINLAYER][l].prop_ptr, "name",0)),
                          textelement[sel].txt_ptr) ) {
               #ifdef HAS_CAIRO
               customfont = set_text_custom_font(&textelement[sel]);
               #endif
               text_bbox(textelement[sel].txt_ptr, textelement[sel].xscale,
               textelement[sel].yscale, rot, flip,
               textelement[sel].x0, textelement[sel].y0,
               &xx1,&yy1,&xx2,&yy2);
               #ifdef HAS_CAIRO
               if(customfont) cairo_restore(ctx);
               #endif

	       pcx = (rect[PINLAYER][l].x1+rect[PINLAYER][l].x2)/2.0;
	       pcy = (rect[PINLAYER][l].y1+rect[PINLAYER][l].y2)/2.0;

               if(
                   // 20171206 20171221
                   (fabs( (yy1+yy2)/2 - pcy) < CADGRID/2 && 
                   (fabs(xx1 - pcx) < CADGRID*6 || fabs(xx2 - pcx) < CADGRID*6) )
                   || 
                   (fabs( (xx1+xx2)/2 - pcx) < CADGRID/2 && 
                   (fabs(yy1 - pcy) < CADGRID*6 || fabs(yy2 - pcy) < CADGRID*6) )
               ) {
                 if(x==0)  // 20080804
                   my_strdup(&rect[PINLAYER][l].prop_ptr, 
                     subst_token(rect[PINLAYER][l].prop_ptr, "name", 
                     (char *) tclgetvar("txt")) );
                 else
                   my_strdup(&rect[PINLAYER][l].prop_ptr, 
                     subst_token(rect[PINLAYER][l].prop_ptr, "name", 
                     (char *) tclgetvar("retval")) );
               }
             }
           } 
         }
         if(x==0)  // 20080804
           my_strdup(&textelement[sel].txt_ptr, (char *) tclgetvar("txt"));
         else // 20080804
           my_strdup(&textelement[sel].txt_ptr, (char *) tclgetvar("retval"));
         
       }
       if(x==0) {
       my_strdup(&textelement[sel].prop_ptr,(char *) tclgetvar("props"));
       my_strdup(&textelement[sel].font, get_tok_value(textelement[sel].prop_ptr, "font", 0));//20171206
       strlayer = get_tok_value(textelement[sel].prop_ptr, "layer", 0); // 20171206
       if(strlayer[0]) textelement[sel].layer = atoi(strlayer);
       else textelement[sel].layer=-1;
       textelement[sel].xscale=atof(tclgetvar("hsize"));
       textelement[sel].yscale=atof(tclgetvar("vsize"));
       }
    
       				// calculate bbox, some cleanup needed here
       #ifdef HAS_CAIRO
       customfont = set_text_custom_font(&textelement[sel]);
       #endif
       text_bbox(textelement[sel].txt_ptr, textelement[sel].xscale,
                 textelement[sel].yscale, rot, flip, 
                 textelement[sel].x0, textelement[sel].y0,
                 &xx1,&yy1,&xx2,&yy2);
       #ifdef HAS_CAIRO
       if(customfont) cairo_restore(ctx);
       #endif

       bbox(ADD, xx1, yy1, xx2, yy2 );        
    
     }
     bbox(SET,0.0,0.0,0.0,0.0);
     draw();
     bbox(END,0.0,0.0,0.0,0.0);
   }
}

static char *old_prop=NULL;
static int i=-1;
static int netlist_commands;

// x=0 use text widget   x=1 use vim editor
void edit_symbol_property(int x)
{
 static char *result=NULL;

   i=selectedgroup[0].n;
   netlist_commands =  !strcmp(		// 20070318
                         // get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr,"type",0), // 20150409
                         (inst_ptr[i].ptr+instdef)->type, // 20150409
                         "netlist_commands"
                       );

   fill_symbol_editprop_form( x);

   if(x==0) {
     tcleval("edit_prop {Input property:}");
     my_strdup(&result, Tcl_GetStringResult(interp));
   }
   else {
     if(netlist_commands && x==1)    tcleval("edit_vi_netlist_prop {Input property:}");
     else if(x==1)    tcleval("edit_vi_prop {Input property:}");
     else if(x==2)    tcleval("viewdata $::retval");
     my_strdup(&result, Tcl_GetStringResult(interp));
   }
   if(debug_var>=1) fprintf(errfp, "edit_symbol_property(): before update_symbol, modified=%d\n", modified);
   update_symbol(result, x);
   if(debug_var>=1) fprintf(errfp, "edit_symbol_property(): done update_symbol, modified=%d\n", modified);
   i=-1; // 20160423
}



// x=0 use text widget   x=1 use vim editor
void update_symbol(char *result, int x)
{
 int k, sym_number;
 int no_change_props=0;
 int only_different=0;
 int copy_cell=0; // 20150911
 int prefix=0;
 static char *name=NULL,*ptr=NULL, *template=NULL;
 char symbol[PATH_MAX];
 static char *new_prop=NULL;
 char *type;
 int cond;
 int pushed=0; // 20150327

   i=selectedgroup[0].n; // 20110413
   if(!result) 
   {
    if(debug_var>=1) fprintf(errfp, "update_symbol(): edit symbol prop aborted\n");
    return;
   }

   if(netlist_commands && x==1) {
   // 20070318
     my_strdup( &new_prop,
       subst_token(old_prop, "value", (char *) tclgetvar("retval") )
     );
   }
   else {
     my_strdup(&new_prop, (char *) tclgetvar("retval"));
     if(debug_var>=1) fprintf(errfp, "update_symbol(): new_prop=%s\n", new_prop);
   }

   my_strncpy(symbol, (char *) tclgetvar("symbol") , S(symbol));
   no_change_props=atoi(tclgetvar("rbutton1") );
   only_different=atoi(tclgetvar("rbutton2") );
   copy_cell=atoi(tclgetvar("user_wants_copy_cell") ); // 20150911


  if(copy_cell) { // 20150911
    remove_symbols();
    link_symbols_to_instances();
  }

   prefix=0;

   // 20150911
   //   |
   if(copy_cell || (strcmp(symbol, inst_ptr[i].name)) ) // user wants to change symbol ; added strcmp 30102003
   {
    if(debug_var>=1) fprintf(errfp, "update_symbol(): changing symbol: %s --> %s\n", symbol, inst_ptr[i].name);

     my_strncpy(symbol, rel_sym_path(symbol), S(symbol));

     
    // 20150911
    //     |
    if(strcmp(symbol, inst_ptr[i].name)) {
      modified=1;
      prepared_hash_objects=0; // 20171224
      prepared_netlist_structs=0;
      prepared_hilight_structs=0;
    }
    sym_number=match_symbol(symbol); // check if exist
    if(sym_number>=0)
    {
     // my_strdup(&template, get_tok_value((instdef+sym_number)->prop_ptr, "template",2)); // 20150409
     my_strdup(&template, (instdef+sym_number)->templ); // 20150409
     prefix=(get_tok_value(template, "name",0))[0]; // get new symbol prefix 
    }
   }
   else sym_number=-1;

   bbox(BEGIN,0.0,0.0,0.0,0.0);

   for(k=0;k<lastselected;k++)
   {
    if(debug_var>=1) fprintf(errfp, "update_symbol(): for k loop: k=%d\n", k);
    if(selectedgroup[k].type!=ELEMENT) continue;
    i=selectedgroup[k].n;
    if(!pushed) { push_undo(); pushed=1;} // 20150327 push_undo

    // 20171220 calculate bbox before changes to correctly redraw areas
    // must be recalculated as cairo text extents vary with zoom factor.
    symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2);

    if(sym_number>=0) // changing symbol !
    {
     delete_inst_node(i); // 20180208 fix crashing bug: delete node info if changing symbol
                          // if number of pins is different we must delete these data *before*
                          // changing ysmbol, otherwise i might end up deleting non allocated data.
     my_strdup(&inst_ptr[i].name,symbol);
     inst_ptr[i].ptr=sym_number;
    }


    bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);

    hash_proplist(inst_ptr[i].prop_ptr , 1); // remove old props from hash table
    // update property string from tcl dialog
    if(!no_change_props)
    {
     if(debug_var>=1) fprintf(errfp, "update_symbol(): no_change_props=%d\n", no_change_props);
     if(only_different) {
           if( set_different_token(&inst_ptr[i].prop_ptr, new_prop, old_prop) ) {
             my_strdup2(&inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name", 0)); // 20160308
												// allow change name
             modified=1;
             prepared_hash_objects=0; // 20171224
             prepared_netlist_structs=0;
             prepared_hilight_structs=0;
           }
     }
     else {
       if(new_prop) {  // 20111205

         // .................... <-- 20111205 20160308 changed from if(inst_ptr... && strcmp...)
         if(!inst_ptr[i].prop_ptr || strcmp(inst_ptr[i].prop_ptr, new_prop)) {
           if(debug_var>=1) fprintf(errfp, "update_symbol(): changing prop: |%s| -> |%s|\n", inst_ptr[i].prop_ptr, new_prop);
           my_strdup(&inst_ptr[i].prop_ptr, new_prop);
           my_strdup2(&inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name", 0)); // 20150409
           modified=1;
           prepared_hash_objects=0; // 20171224
           prepared_netlist_structs=0;
           prepared_hilight_structs=0;
         }
       }  else {  // 20111205
         my_strdup(&inst_ptr[i].prop_ptr, "");
         my_strdup2(&inst_ptr[i].instname, ""); // 20150409
       }
     }
    }
    // my_strdup(&name, get_tok_value(inst_ptr[i].prop_ptr,"name",0) ); // 20150409
    my_strdup(&name, inst_ptr[i].instname); // 20150409
    if(name && name[0] )  // 30102003
    {  
     if(debug_var>=1) fprintf(errfp, "update_symbol(): prefix!='\\0', name=%s\n", name);

     // 20110325 only modify prefix if prefix not NUL
     if(prefix) name[0]=prefix; // change prefix if changing symbol type;

     if(debug_var>=1) fprintf(errfp, "update_symbol(): name=%s\n", name);
     my_strdup(&ptr,subst_token(inst_ptr[i].prop_ptr, "name", name) );
                    // set name of current inst
     new_prop_string(&inst_ptr[i].prop_ptr, ptr,0); // set new prop_ptr
     my_strdup2(&inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name",0)); // 20150409
 
     // type=get_tok_value(instdef[inst_ptr[i].ptr].prop_ptr,"type",0); // 20150409
     type=instdef[inst_ptr[i].ptr].type; // 20150409
     cond= strcmp(type,"label") && strcmp(type,"ipin") &&
           strcmp(type,"opin") &&  strcmp(type,"iopin");
     if(cond) inst_ptr[i].flags|=2;
     else inst_ptr[i].flags &=~2;
    }
    hash_proplist(inst_ptr[i].prop_ptr , 0); // put new props in hash table
    // new symbol bbox after prop changes (may change due to text length)
    symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2);
 
    bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
   } 
   // redraw symbol with new props
   bbox(SET,0.0,0.0,0.0,0.0);
   if(debug_var>=1) fprintf(errfp, "update_symbol(): redrawing inst_ptr.txtprop string\n");
   draw();
   bbox(END,0.0,0.0,0.0,0.0);
   //// 20160308 added if(), leave edited objects selected after updating properties
   //// unless i am clicking another element with edit property dialog box open
   //// in this latter case the last pointed element remains selected.
   if( !strcmp(tclgetvar("editprop_semaphore"), "2")) {
     unselect_all();
     select_object(mousex,mousey,SELECTED);
   }
   rebuild_selected_array();
}

void fill_symbol_editprop_form(int x) 
{
 // int i; // 20160423 commented


   // 20160423 if no more stuff selected close editprop toplevel form
   if(lastselected==0 || selectedgroup[0].type!=ELEMENT) Tcl_GlobalEval(interp, "set editprop_semaphore 0");

   if(!lastselected) return;
   i=selectedgroup[0].n;

   // 20160423
   if(selectedgroup[0].type!=ELEMENT) return;
  
   if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): element %d property=%s\n",i,inst_ptr[i].prop_ptr);
   if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): modified=%d\n", modified);
   if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): symbol=%s\n", inst_ptr[i].name);
   if(inst_ptr[i].prop_ptr!=NULL)
   {
    if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): element %d property=%s\n",i,inst_ptr[i].prop_ptr);
 
    if(netlist_commands && x==1) {
    // 20070318
      tclsetvar("retval",get_tok_value( inst_ptr[i].prop_ptr,"value",0));
    } else {
      tclsetvar("retval",inst_ptr[i].prop_ptr);
    }
   }
   else
   {
     set_inst_prop(i);
     if(inst_ptr[i].prop_ptr!=NULL) {
      if(netlist_commands && x==1) {
      // 20070318
        tclsetvar("retval",get_tok_value( inst_ptr[i].prop_ptr,"value",0));
      } else {
        tclsetvar("retval",inst_ptr[i].prop_ptr);
      }
     } else {
      if(netlist_commands && x==1) {
      // 20070318
        tclsetvar("retval","");
      } else {
        tclsetvar("retval","");
      }
     }
   }

   // moved here from update_symbol() 20110413
   if(netlist_commands && x==1) {
     my_strdup(&old_prop, inst_ptr[i].prop_ptr);
   } else {
     my_strdup(&old_prop, (char *) tclgetvar("retval"));
   }
   // /20110413


   // tcleval("puts \"retval= $retval\"");  // ??????
   tclsetvar("symbol",inst_ptr[i].name);
}

void change_elem_order(void)
{
   Instance tmp;
   Box tmpbox;
   char tmp_txt[50]; // overflow safe
   int c, new_n;

    rebuild_selected_array();
    if(lastselected==1)
    {
     my_snprintf(tmp_txt, S(tmp_txt), "%d",selectedgroup[0].n);
     tclsetvar("retval",tmp_txt);
     tcleval("text_line {Input number} 0");
     if(strcmp(tclgetvar("rcode"),"") )
     {
      push_undo(); // 20150327
      modified=1;
      prepared_hash_objects=0; // 20171224
      prepared_netlist_structs=0;
      prepared_hilight_structs=0;
     }
     sscanf(tclgetvar("retval"), "%d",&new_n);

     if(selectedgroup[0].type==ELEMENT)
     {
      if(new_n>=lastinst) new_n=lastinst-1;
      tmp=inst_ptr[new_n];
      inst_ptr[new_n]=inst_ptr[selectedgroup[0].n];
      inst_ptr[selectedgroup[0].n]=tmp;
      if(debug_var>=1) fprintf(errfp, "change_elem_order(): selected element %d\n", selectedgroup[0].n);
     }
     else if(selectedgroup[0].type==RECT)
     {
      c=selectedgroup[0].col;
      if(new_n>=lastrect[c]) new_n=lastrect[c]-1;
      tmpbox=rect[c][new_n];
      rect[c][new_n]=rect[c][selectedgroup[0].n];
      rect[c][selectedgroup[0].n]=tmpbox;
      if(debug_var>=1) fprintf(errfp, "change_elem_order(): selected element %d\n", selectedgroup[0].n);
     }
    }
}

// x=0 use tcl text widget  x=1 use vim editor  x=2 only view data
void edit_property(int x)
{
 int j;
 if(!has_x) return;
 rebuild_selected_array(); // from the .sel field in objects build
 if(lastselected==0 )      // the array of selected objs
 {
   if(netlist_type==CAD_VERILOG_NETLIST && current_type==SCHEMATIC) {
    if(schverilogprop!=NULL)	//09112003
      tclsetvar("retval",schverilogprop);
    else
      tclsetvar("retval","");
   }
   else if(netlist_type==CAD_SPICE_NETLIST && current_type==SCHEMATIC) { // 20100217
    if(schprop!=NULL) 
      tclsetvar("retval",schprop);
    else
      tclsetvar("retval","");
   }
   else if(netlist_type==CAD_TEDAX_NETLIST && current_type==SCHEMATIC) { // 20100217
    if(schtedaxprop!=NULL) 
      tclsetvar("retval",schtedaxprop);
    else
      tclsetvar("retval","");
   }
   else { // this is used for symbol global props also
    if(schvhdlprop!=NULL)
      tclsetvar("retval",schvhdlprop);
    else
      tclsetvar("retval","");
   }

   if(x==0)         tcleval("text_line {Global schematic property:} 0");          
   else if(x==1) {
      if(debug_var>=1) fprintf(errfp, "edit_property(): executing edit_vi_prop\n");
      tcleval("edit_vi_prop {Global schematic property:}");
   }
   else if(x==2)    tcleval("viewdata $::retval");
   if(debug_var>=1) fprintf(errfp, "edit_property(): done executing edit_vi_prop, result=%s\n",Tcl_GetStringResult(interp));
   if(debug_var>=1) fprintf(errfp, "edit_property(): rcode=%s\n",tclgetvar("rcode") );
   if(strcmp(tclgetvar("rcode"),"") )
   {
     if(current_type==SYMBOL && // 20120404 added case for symbol editing, use schvhdlprop regardless of netlisting mode
        (!schvhdlprop || strcmp(schvhdlprop, tclgetvar("retval") ) ) ) { // symbol edit
        modified=1; push_undo(); // 20150327
        my_strdup(&schvhdlprop, (char *) tclgetvar("retval"));
     } else if(netlist_type==CAD_VERILOG_NETLIST && current_type==SCHEMATIC && // 20120228 check if schverilogprop NULL
        (!schverilogprop || strcmp(schverilogprop, tclgetvar("retval") ) ) ) { // 20120209
        modified=1; push_undo(); // 20150327
        my_strdup(&schverilogprop, (char *) tclgetvar("retval")); //09112003
    
     } else if(netlist_type==CAD_SPICE_NETLIST && current_type==SCHEMATIC && // 20120228 check if schprop NULL
        (!schprop || strcmp(schprop, tclgetvar("retval") ) ) ) { // 20120209
        modified=1; push_undo(); // 20150327
        my_strdup(&schprop, (char *) tclgetvar("retval")); //09112003 

     } else if(netlist_type==CAD_TEDAX_NETLIST && current_type==SCHEMATIC && // 20120228 check if schprop NULL
        (!schtedaxprop || strcmp(schtedaxprop, tclgetvar("retval") ) ) ) { // 20120209
        modified=1; push_undo(); // 20150327
        my_strdup(&schtedaxprop, (char *) tclgetvar("retval")); //09112003 

     } else if(netlist_type==CAD_VHDL_NETLIST && current_type==SCHEMATIC && // 20120228 check if schvhdlprop NULL
        (!schvhdlprop || strcmp(schvhdlprop, tclgetvar("retval") ) ) ) { // netlist_type==CAD_VHDL_NETLIST // 20120209
        modified=1; push_undo(); // 20150327
        my_strdup(&schvhdlprop, (char *) tclgetvar("retval"));
     }
   }

   // update the bounding box of vhdl "architecture" instances that embed
   // the schvhdlprop string. 04102001
   for(j=0;j<lastinst;j++)
   {
    // if( !strcmp( get_tok_value( (inst_ptr[j].ptr+instdef)->prop_ptr, "type",0), "architecture") ) // 20150409
    if( !strcmp( (inst_ptr[j].ptr+instdef)->type, "architecture") ) // 20150409
    {
      if(debug_var>=1) fprintf(errfp, "edit_property(): updating vhdl architecture\n");
      symbol_bbox(j, &inst_ptr[j].x1, &inst_ptr[j].y1,
                        &inst_ptr[j].x2, &inst_ptr[j].y2);
    } 
   } // end for(j
   return;
 }

 switch(selectedgroup[0].type)
 {
  case ELEMENT:
   edit_symbol_property(x);
   break;
  case ARC:
   if(debug_var>=1) fprintf(errfp, "edit_property(), modified=%d\n", modified);
   if(arc[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
     tclsetvar("retval",arc[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
   } else { // 20161208
     tclsetvar("retval","");
   }
   tcleval("text_line {Input property:} 0");
   if(strcmp(tclgetvar("rcode"),"") )
   {
    modified=1; push_undo(); // 20150327
    my_strdup(&arc[selectedgroup[0].col][selectedgroup[0].n].prop_ptr,
     (char *) tclgetvar("retval"));
   }
   break;

  case RECT:
   if(debug_var>=1) fprintf(errfp, "edit_property(), modified=%d\n", modified);
   if(rect[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
     tclsetvar("retval",rect[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
   } else { // 20161208
     tclsetvar("retval","");
   }
   tcleval("text_line {Input property:} 0");
   if(strcmp(tclgetvar("rcode"),"") )
   {
    modified=1; push_undo(); // 20150327
    my_strdup(&rect[selectedgroup[0].col][selectedgroup[0].n].prop_ptr,
	   (char *) tclgetvar("retval"));
   }
   break;
  case WIRE:
   if(debug_var>=1) fprintf(errfp, "edit_property(): input property:\n");
   if(wire[selectedgroup[0].n].prop_ptr!=NULL) {
     tclsetvar("retval",wire[selectedgroup[0].n].prop_ptr);
   } else { // 20161208
     tclsetvar("retval","");
   }
   tcleval("text_line {Input property:} 0");
   if(strcmp(tclgetvar("rcode"),"") )
   {
    prepared_hash_wires=0; // 20181025
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
    modified=1; push_undo(); // 20150327
    my_strdup(&wire[selectedgroup[0].n].prop_ptr,(char *) tclgetvar("retval"));
    if(get_tok_value(wire[selectedgroup[0].n].prop_ptr,"bus",0)[0]) wire[selectedgroup[0].n].bus=1; // 20171201
    else wire[selectedgroup[0].n].bus=0;
   }
   break;
  case POLYGON: // 20171115
   if(debug_var>=1) fprintf(errfp, "edit_property(): input property:\n");
   if(polygon[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
     tclsetvar("retval",polygon[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
   } else { // 20161208
     tclsetvar("retval","");
   }
   tcleval("text_line {Input property:} 0");
   if(strcmp(tclgetvar("rcode"),"") )
   {
    int old_fill; // 20180914
    int k;
    double x1=0., y1=0., x2=0., y2=0.;
    int c, i;

    c = selectedgroup[0].col;
    i = selectedgroup[0].n;
    modified=1; push_undo(); // 20150327
    my_strdup(&polygon[c][i].prop_ptr,
        (char *) tclgetvar("retval"));
    // 20180914
    old_fill = polygon[c][i].fill;
    if( !strcmp(get_tok_value(polygon[c][i].prop_ptr,"fill",0),"true") )
      polygon[c][i].fill =1;
    else 
      polygon[c][i].fill =0;
    if(old_fill != polygon[c][i].fill) {
      bbox(BEGIN,0.0,0.0,0.0,0.0);
      for(k=0; k<polygon[c][i].points; k++) {
        if(k==0 || polygon[c][i].x[k] < x1) x1 = polygon[c][i].x[k];
        if(k==0 || polygon[c][i].y[k] < y1) y1 = polygon[c][i].y[k];
        if(k==0 || polygon[c][i].x[k] > x2) x2 = polygon[c][i].x[k];
        if(k==0 || polygon[c][i].y[k] > y2) y2 = polygon[c][i].y[k];
      }
      bbox(ADD, x1, y1, x2, y2);
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
      draw();
      bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    }
   }
   break;

  case LINE:
   if(debug_var>=1) fprintf(errfp, "edit_property(): input property:\n");
   if(line[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
     tclsetvar("retval",line[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
   } else { // 20161208
     tclsetvar("retval","");
   }
   tcleval("text_line {Input property:} 0");
   if(strcmp(tclgetvar("rcode"),"") )
   {
    modified=1; push_undo(); // 20150327
    my_strdup(&line[selectedgroup[0].col][selectedgroup[0].n].prop_ptr, (char *) tclgetvar("retval"));
   }
   break;
  case TEXT:
   edit_text_property(x);
   break;             
 }

}


