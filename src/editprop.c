/* File: editprop.c
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

#include <stdarg.h>
#include "xschem.h"

static int rot = 0, flip = 0;          

char *my_strtok_r(char *str, const char *delim, char **saveptr)
{
  char *tok;
  if(str) { /* 1st call */
    *saveptr = str;
  }
  while(**saveptr && strchr(delim, **saveptr) ) { /* skip separators */
    ++(*saveptr);
  }
  tok = *saveptr; /* start of token */
  while(**saveptr && !strchr(delim, **saveptr) ) { /* look for separator marking end of current token */
    ++(*saveptr);
  }
  if(**saveptr) {
    **saveptr = '\0'; /* mark end of token */
    ++(*saveptr);     /* if not at end of string advance one char for next iteration */
  }
  if(tok[0]) return tok; /* return token */
  else return NULL; /* no more tokens */
}

size_t my_strdup(int id, char **dest, const char *src) /* empty source string --> dest=NULL */
{
 size_t len;
 if(*dest!=NULL) { 
   if(debug_var>=3) fprintf(errfp,"my_strdup():  calling my_free\n");
   my_free(dest);
 }
 if(src!=NULL && src[0]!='\0')  
 {
  /* 20180923 */
  len = strlen(src)+1;
  *dest = my_malloc(id, len);
  memcpy(*dest, src, len);
  /* *dest=strdup(src); */
  if(debug_var>=3) fprintf(errfp,"my_strdup(%d,): duplicated string %s\n", id, src);
  return len-1;
 }
 return 0;
}

/* 20171004 copy at most n chars, adding a null char at end */
void my_strndup(int id, char **dest, const char *src, int n) /* empty source string --> dest=NULL */

{
 if(*dest!=NULL) {
   if(debug_var>=3) fprintf(errfp,"  my_strndup:  calling my_free\n");
   my_free(dest);
 }
 if(src!=NULL && src[0]!='\0')
 {
  /* 20180924 replace strndup() */
  char *p = memchr(src, '\0', n);
  if(p) n = p - src;
  *dest = my_malloc(id, n+1);
  if(*dest) {
    memcpy(*dest, src, n);
    (*dest)[n] = '\0';
  }
  /* *dest=strndup(src, n); */

  if(debug_var>=3) fprintf(errfp,"my_strndup(%d,): duplicated string %s\n", id, src);
 }
}

#ifdef HAS_SNPRINTF
int my_snprintf(char *str, int size, const char *fmt, ...) /* 20161124 */
{
  int  size_of_print;
  char s[200];

  va_list args;
  va_start(args, fmt);
  size_of_print = vsnprintf(str, size, fmt, args);

  if(has_x && size_of_print >=size) { /* output was truncated  */
    snprintf(s, S(s), "alert_ { Warning: overflow in my_snprintf print size=%d, buffer size=%d} {}",
             size_of_print, size);
    tcleval(s);
  }
  va_end(args);
  return size_of_print;
}
#else

/*
   this is a replacement for snprintf(), **however** it implements only
   the bare minimum set of formatting used by XSCHEM
*/
int my_snprintf(char *string, int size, const char *format, ...)
{
  va_list args;
  const char *f, *fmt = NULL, *prev;
  int overflow, format_spec, l, n = 0;

  va_start(args, format);

  /* fprintf(errfp, "my_snprintf(): size=%d, format=%s\n", size, format); */
  prev = format;
  format_spec = 0;
  overflow = 0;
  for(f = format; *f; f++) {
    if(*f == '%') {
      format_spec = 1;
      fmt = f;
    }
    if(*f == 's' && format_spec) {
      char *sptr;
      sptr = va_arg(args, char *);
      l = fmt - prev;
      if(n+l > size) {
        overflow = 1;
        break;
      }
      memcpy(string + n, prev, l);
      string[n+l] = '\0';
      n += l;
      l = strlen(sptr);
      if(n+l+1 > size) {
        overflow = 1;
        break;
      }
      memcpy(string + n, sptr, l+1);
      n += l;
      format_spec = 0;
      prev = f + 1;
    }
    else if(format_spec && (*f == 'd' || *f == 'x' || *f == 'c') ) {
      char nfmt[50], nstr[50];
      int i, nlen;
      i = va_arg(args, int);
      l = f - fmt+1;
      strncpy(nfmt, fmt, l);
      nfmt[l] = '\0';
      l = fmt - prev;
      if(n+l > size) break;
      memcpy(string + n, prev, l);
      string[n+l] = '\0';
      n += l;
      nlen = sprintf(nstr, nfmt, i);
      if(n + nlen + 1 > size) {
        overflow = 1;
        break;
      }
      memcpy(string +n, nstr, nlen+1);
      n += nlen;
      format_spec = 0;
      prev = f + 1;
    }
    else if(format_spec && *f == 'g') {
      char nfmt[50], nstr[50];
      double i;
      int nlen;
      i = va_arg(args, double);
      l = f - fmt+1;
      strncpy(nfmt, fmt, l);
      nfmt[l] = '\0';
      l = fmt - prev;
      if(n+l > size) {
        overflow = 1;
        break;
      }
      memcpy(string + n, prev, l);
      string[n+l] = '\0';
      n += l;
      nlen = sprintf(nstr, nfmt, i);
      if(n + nlen + 1 > size) {
        overflow = 1;
        break;
      }
      memcpy(string +n, nstr, nlen+1);
      n += nlen;
      format_spec = 0;
      prev = f + 1;
    }
  }
  l = f - prev;
  if(!overflow && n+l+1 <= size) {
    memcpy(string + n, prev, l+1);
    n += l;
  } else {
    if(debug_var>=1) fprintf(errfp, "my_snprintf(): overflow, target size=%d, format=%s\n", size, format);
  }
  
  va_end(args);
  /* fprintf(errfp, "my_snprintf(): returning: |%s|\n", string); */
  return n;
}
#endif /* HAS_SNPRINTF */

size_t my_strdup2(int id, char **dest, const char *src) /* 20150409 duplicates also empty string  */
{
 size_t len;
 if(*dest!=NULL) {
   if(debug_var>=3) fprintf(errfp,"  my_strdup2():  calling my_free\n");
   my_free(dest);
 }
 if(src!=NULL)
 {
  /* 20180923 */
  len = strlen(src)+1;
  *dest = my_malloc(id, len);
  memcpy(*dest, src, len);
  /* *dest=strdup(src); */
  if(debug_var>=3) fprintf(errfp,"my_strdup2(%d,): duplicated string %s\n", id, src);
  return len-1;
 }
 return 0;
}

size_t my_strcat(int id, char **str, const char *append_str)
{
  size_t s, a;
  if(debug_var>=3) fprintf(errfp,"my_strcat(%d,): str=%s  append_str=%s\n", id, *str, append_str);
  if( *str != NULL)
  {
    s = strlen(*str);
    if(append_str == NULL || append_str[0]=='\0') return s;
    a = strlen(append_str) + 1;
    my_realloc(id, str, s + a );
    memcpy(*str + s, append_str, a); /* 20180923 */
    if(debug_var >= 3) fprintf(errfp,"my_strcat(%d,): reallocated string %s\n", id, *str);
    return s + a - 1;
  } else {
    if(append_str == NULL || append_str[0] == '\0') return 0;
    a = strlen(append_str) + 1;
    *str = my_malloc(id, a);
    memcpy(*str, append_str, a); /* 20180923 */
    if(debug_var>=3) fprintf(errfp,"my_strcat(%d,): allocated string %s\n", id, *str);
    return a - 1;
  }
}

size_t my_strncat(int id, char **str, size_t n, const char *append_str)
{
 size_t s, a;
 if(debug_var>=3) fprintf(errfp,"my_strncat(%d,): str=%s  append_str=%s\n", id, *str, append_str);
 a = strlen(append_str)+1;
 if(a>n+1) a=n+1;
 if( *str != NULL)
 {
  s = strlen(*str);
  if(append_str == NULL || append_str[0]=='\0') return s;
  my_realloc(id, str, s + a );
  memcpy(*str+s, append_str, a); /* 20180923 */
  *(*str+s+a) = '\0';
  if(debug_var>=3) fprintf(errfp,"my_strncat(%d,): reallocated string %s\n", id, *str);
  return s + a -1;
 }
 else
 {
  if(append_str == NULL || append_str[0]=='\0') return 0;
  *str = my_malloc(id,  a );
  memcpy(*str, append_str, a); /* 20180923 */
  *(*str+a) = '\0';
  if(debug_var>=3) fprintf(errfp,"my_strncat(%d,): allocated string %s\n", id, *str);
  return a -1;
 }
}


void *my_calloc(int id, size_t nmemb, size_t size)
{
   void *ptr;
   if(size*nmemb > 0) {
     ptr=calloc(nmemb, size);
     if(ptr == NULL) fprintf(errfp,"my_calloc(%d,): allocation failure\n", id);
     if(debug_var>=3) 
       fprintf(errfp, "my_calloc(%d,): allocating %p , %lu bytes\n",
               id, ptr, (unsigned long) (size*nmemb));
   }
   else ptr = NULL;
   return ptr;
}

void *my_malloc(int id, size_t size) 
{
 void *ptr;
 if(size>0) {
   ptr=malloc(size);
  if(ptr == NULL) fprintf(errfp,"my_malloc(%d,): allocation failure\n", id);
   if(debug_var>=3) fprintf(errfp, "my_malloc(%d,): allocating %p , %lu bytes\n",
     id, ptr, (unsigned long) size);
 }
 else ptr=NULL;
 return ptr;
}

void my_realloc(int id, void *ptr,size_t size)
{
 void *a;
 a = *(void **)ptr;
 if(size == 0) {
   free(*(void **)ptr);
   *(void **)ptr=NULL;
   if(debug_var>=3) fprintf(errfp, "my_free():  my_realloc_freeing %p\n",*(void **)ptr);
 } else {
   *(void **)ptr=realloc(*(void **)ptr,size);
    if(*(void **)ptr == NULL) fprintf(errfp,"my_realloc(%d,): allocation failure\n", id);
   if(debug_var>=3) 
     fprintf(errfp, "my_realloc(%d,): reallocating %p --> %p to %lu bytes\n",
           id, a, *(void **)ptr,(unsigned long) size);
 }

} 

void my_free(void *ptr)
{
 if(debug_var>=3) fprintf(errfp, "my_free():  freeing %p\n",*(void **)ptr);
 free(*(void **)ptr);
 *(void **)ptr=NULL;
}

/* n characters at most are copied, *d will be always NUL terminated if *s does
 *   not fit(d[n-1]='\0')
 * return # of copied characters
 */
int my_strncpy(char *d, const char *s, int n)
{
  int i = 0;
  n -= 1;
  if(debug_var>=3) 
    fprintf(errfp, "my_strncpy():  copying %s to %lu\n", s, (unsigned long)d);
  while( (d[i] = s[i]) )
  {
    if(i == n) { 
      if(s[i] != '\0') if(debug_var>=1)  fprintf(errfp, "my_strncpy(): overflow, n=%d, s=%s\n", n+1, s);
      d[i] = '\0';
      return i; 
    }
    i++;
  }
  return i;
}


void set_inst_prop(int i)
{
  char *ptr;
  char *tmp = NULL;

  ptr = (inst_ptr[i].ptr+instdef)->templ; /*20150409 */
  if(debug_var>=1) fprintf(errfp, "set_inst_prop(): i=%d, name=%s, prop_ptr = %s, template=%s\n", 
     i, inst_ptr[i].name, inst_ptr[i].prop_ptr, ptr);
  my_strdup(69, &inst_ptr[i].prop_ptr, ptr);
  my_strdup2(70, &inst_ptr[i].instname, get_tok_value(ptr, "name",0)); /* 20150409 */
  if(inst_ptr[i].instname[0]) {
    my_strdup(101, &tmp, inst_ptr[i].prop_ptr);
    new_prop_string(i, tmp, 0, disable_unique_names);
    my_free(&tmp);
  }
}

void edit_rect_property(void) 
{
  int i;
  int preserve;
  char *oldprop=NULL;
  if(rect[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
    my_strdup(67, &oldprop, rect[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
    tclsetvar("retval",oldprop);
  } else { /* 20161208 */
    tclsetvar("retval","");
  }

  tcleval("text_line {Input property:} 0 normal");
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {
    push_undo();
    set_modify(1);
    for(i=0; i<lastselected; i++) {
      if(selectedgroup[i].type != RECT) continue;

      if(preserve == 1) {
        set_different_token(&rect[selectedgroup[i].col][selectedgroup[i].n].prop_ptr, 
               (char *) tclgetvar("retval"), oldprop, 0, 0);
      } else {
        my_strdup(99, &rect[selectedgroup[i].col][selectedgroup[i].n].prop_ptr,
               (char *) tclgetvar("retval"));
      }
    }
  }
  my_free(&oldprop);
}


void edit_line_property(void)
{
  int i;
  int preserve;
  char *oldprop=NULL;
  if(line[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
    my_strdup(46, &oldprop, line[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
    tclsetvar("retval", oldprop);
  } else { /* 20161208 */
    tclsetvar("retval","");
  }
  tcleval("text_line {Input property:} 0 normal");
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {
    push_undo();
    set_modify(1);
    for(i=0; i<lastselected; i++) {
      if(selectedgroup[i].type != LINE) continue;

      if(preserve == 1) {
        set_different_token(&line[selectedgroup[i].col][selectedgroup[i].n].prop_ptr, 
               (char *) tclgetvar("retval"), oldprop, 0, 0);
      } else {
        my_strdup(102, &line[selectedgroup[i].col][selectedgroup[i].n].prop_ptr,
               (char *) tclgetvar("retval"));
      }
    }
  }
  my_free(&oldprop);
}


void edit_wire_property(void)
{
  int i;
  int preserve;
  char *oldprop=NULL;
  char *bus_ptr;

  if(wire[selectedgroup[0].n].prop_ptr!=NULL) {
    my_strdup(47, &oldprop, wire[selectedgroup[0].n].prop_ptr);
    tclsetvar("retval", oldprop);
  } else { /* 20161208 */
    tclsetvar("retval","");
  }
  tcleval("text_line {Input property:} 0 normal");
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {
    push_undo(); /* 20150327 */
    set_modify(1); 
    for(i=0; i<lastselected; i++) {
      int oldbus=0;
      int k = selectedgroup[i].n;
      if(selectedgroup[i].type != WIRE) continue; 
      prepared_hash_wires=0; /* 20181025 */
      prepared_netlist_structs=0;
      prepared_hilight_structs=0;
      oldbus = wire[k].bus;
      if(preserve == 1) {
        set_different_token(&wire[k].prop_ptr, 
               (char *) tclgetvar("retval"), oldprop, 0, 0);
      } else {
        my_strdup(100, &wire[k].prop_ptr,(char *) tclgetvar("retval"));
      }
      bus_ptr = get_tok_value(wire[k].prop_ptr,"bus",0);
      if(!strcmp(bus_ptr, "true")) {
        int ov, y1, y2;
        bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
        ov = bus_width > cadhalfdotsize ? bus_width : CADHALFDOTSIZE;
        if(wire[k].y1 < wire[k].y2) { y1 = wire[k].y1-ov; y2 = wire[k].y2+ov; }
        else                        { y1 = wire[k].y1+ov; y2 = wire[k].y2-ov; }
        bbox(ADD, wire[k].x1-ov, y1 , wire[k].x2+ov , y2 );
        wire[k].bus=1;
        bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
        draw();
        bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
      } else {
        if(oldbus){ /* 20171201 */
          int ov, y1, y2;
          bbox(BEGIN, 0.0 , 0.0 , 0.0 , 0.0);
          ov = bus_width> cadhalfdotsize ? bus_width : CADHALFDOTSIZE;
          if(wire[k].y1 < wire[k].y2) { y1 = wire[k].y1-ov; y2 = wire[k].y2+ov; }
          else                        { y1 = wire[k].y1+ov; y2 = wire[k].y2-ov; }
          bbox(ADD, wire[k].x1-ov, y1 , wire[k].x2+ov , y2 );
          wire[k].bus=0;
          bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
          draw();
          bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
        }
      }
    }
  }
  my_free(&oldprop);
}


void edit_polygon_property(void)
{
  int old_fill; /* 20180914 */
  int k;
  double x1=0., y1=0., x2=0., y2=0.;
  int c, i, ii;
  char *oldprop = NULL;
  int preserve;

  if(debug_var>=1) fprintf(errfp, "edit_property(): input property:\n");
  if(polygon[selectedgroup[0].col][selectedgroup[0].n].prop_ptr!=NULL) {
    my_strdup(112, &oldprop, polygon[selectedgroup[0].col][selectedgroup[0].n].prop_ptr);
    tclsetvar("retval", oldprop);
  } else { /* 20161208 */
    tclsetvar("retval","");
  }
  tcleval("text_line {Input property:} 0 normal");
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {

   set_modify(1); push_undo(); /* 20150327 */
   for(ii=0; ii<lastselected; ii++) {
     if(selectedgroup[ii].type != POLYGON) continue;
   
     i = selectedgroup[ii].n;
     c = selectedgroup[ii].col;

     if(preserve == 1) {
        set_different_token(&polygon[c][i].prop_ptr, (char *) tclgetvar("retval"), oldprop, 0, 0);

     } else {
        my_strdup(113, &polygon[c][i].prop_ptr, (char *) tclgetvar("retval"));
     }
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
  }
}


/* x=0 use text widget   x=1 use vim editor */
void edit_text_property(int x)
{
   #ifdef HAS_CAIRO
   int customfont;
   #endif
   int sel, k, text_changed; 
   int c,l;
   double xx1,yy1,xx2,yy2;
   double pcx,pcy;      /* pin center 20070317 */
   char property[1024];/* used for float 2 string conv (xscale  and yscale) overflow safe */
   char *strlayer;

   if(debug_var>=1) fprintf(errfp, "edit_text_property(): entering\n");
   sel = selectedgroup[0].n;
   if(textelement[sel].prop_ptr !=NULL)
      tclsetvar("props",textelement[sel].prop_ptr); 
   else
      tclsetvar("props",""); /* 20171112 */

   tclsetvar("txt",textelement[sel].txt_ptr);
   tclsetvar("retval",textelement[sel].txt_ptr); /* for viewdata */
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

   /* 20080804 */
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
     set_modify(1); push_undo(); /* 20150327 */
     bbox(BEGIN,0.0,0.0,0.0,0.0);
     for(k=0;k<lastselected;k++)
     {
       if(selectedgroup[k].type!=TEXT) continue;
       sel=selectedgroup[k].n;

       rot = textelement[sel].rot;      /* calculate bbox, some cleanup needed here */
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
                   /* 20171206 20171221 */
                   (fabs( (yy1+yy2)/2 - pcy) < cadgrid/2 && 
                   (fabs(xx1 - pcx) < cadgrid*6 || fabs(xx2 - pcx) < cadgrid*6) )
                   || 
                   (fabs( (xx1+xx2)/2 - pcx) < cadgrid/2 && 
                   (fabs(yy1 - pcy) < cadgrid*6 || fabs(yy2 - pcy) < cadgrid*6) )
               ) {
                 if(x==0)  /* 20080804 */
                   my_strdup(71, &rect[PINLAYER][l].prop_ptr, 
                     subst_token(rect[PINLAYER][l].prop_ptr, "name", 
                     (char *) tclgetvar("txt")) );
                 else
                   my_strdup(72, &rect[PINLAYER][l].prop_ptr, 
                     subst_token(rect[PINLAYER][l].prop_ptr, "name", 
                     (char *) tclgetvar("retval")) );
               }
             }
           } 
         }
         if(x==0)  /* 20080804 */
           my_strdup(73, &textelement[sel].txt_ptr, (char *) tclgetvar("txt"));
         else /* 20080804 */
           my_strdup(74, &textelement[sel].txt_ptr, (char *) tclgetvar("retval"));
         
       }
       if(x==0) {
       my_strdup(75, &textelement[sel].prop_ptr,(char *) tclgetvar("props"));
       my_strdup(76, &textelement[sel].font, get_tok_value(textelement[sel].prop_ptr, "font", 0));/*20171206 */
       strlayer = get_tok_value(textelement[sel].prop_ptr, "layer", 0); /* 20171206 */
       if(strlayer[0]) textelement[sel].layer = atoi(strlayer);
       else textelement[sel].layer=-1;
       textelement[sel].xscale=atof(tclgetvar("hsize"));
       textelement[sel].yscale=atof(tclgetvar("vsize"));
       }
    
                                /* calculate bbox, some cleanup needed here */
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

/* x=0 use text widget   x=1 use vim editor */
void edit_symbol_property(int x)
{
   char *result=NULL;

   i=selectedgroup[0].n;
   netlist_commands =  !strcmp(         /* 20070318 */
                         /* get_tok_value( (inst_ptr[i].ptr+instdef)->prop_ptr,"type",0), */
                         (inst_ptr[i].ptr+instdef)->type,
                         "netlist_commands"
                       );

   fill_symbol_editprop_form( x);

   if(x==0) {
     tcleval("edit_prop {Input property:}");
     my_strdup(77, &result, Tcl_GetStringResult(interp));
   }
   else {
     if(netlist_commands && x==1)    tcleval("edit_vi_netlist_prop {Input property:}");
     else if(x==1)    tcleval("edit_vi_prop {Input property:}");
     else if(x==2)    tcleval("viewdata $::retval");
     my_strdup(78, &result, Tcl_GetStringResult(interp));
   }
   if(debug_var>=1) fprintf(errfp, "edit_symbol_property(): before update_symbol, modified=%d\n", modified);
   update_symbol(result, x);
   my_free(&result);
   if(debug_var>=1) fprintf(errfp, "edit_symbol_property(): done update_symbol, modified=%d\n", modified);
   i=-1; /* 20160423 */
}



/* x=0 use text widget   x=1 use vim editor */
void update_symbol(const char *result, int x)
{
  int k, sym_number;
  int no_change_props=0;
  int only_different=0;
  int copy_cell=0; /* 20150911 */
  int prefix=0;
  static char *name=NULL,*ptr=NULL, *template=NULL;
  char symbol[PATH_MAX];
  static char *new_prop=NULL;
  char *type;
  int cond;
  int pushed=0; /* 20150327 */

  i=selectedgroup[0].n; /* 20110413 */
  if(!result) 
  {
   if(debug_var>=1) fprintf(errfp, "update_symbol(): edit symbol prop aborted\n");
   return;
  }

  if(netlist_commands && x==1) {
  /* 20070318 */
    my_strdup(79,  &new_prop,
      subst_token(old_prop, "value", (char *) tclgetvar("retval") )
    );
  }
  else {
    my_strdup(80, &new_prop, (char *) tclgetvar("retval"));
    if(debug_var>=1) fprintf(errfp, "update_symbol(): new_prop=%s\n", new_prop);
  }

  my_strncpy(symbol, (char *) tclgetvar("symbol") , S(symbol));
  if(debug_var>=1) fprintf(errfp, "update_symbol(): symbol=%s\n", symbol);
  no_change_props=atoi(tclgetvar("no_change_attrs") );
  only_different=atoi(tclgetvar("preserve_unchanged_attrs") );
  copy_cell=atoi(tclgetvar("user_wants_copy_cell") ); /* 20150911 */

  /* 20191227 necessary? */
  if(copy_cell) { /* 20150911 */
   remove_symbols();
   link_symbols_to_instances();
  }
  
  prefix=0;
  sym_number = -1;
  if(strcmp(symbol, inst_ptr[i].name)) {
    set_modify(1);
    prepared_hash_instances=0; /* 20171224 */
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
    sym_number=match_symbol(symbol); /* check if exist */
    if(sym_number>=0) {
      my_strdup(81, &template, (instdef+sym_number)->templ); /* 20150409 */
      prefix=(get_tok_value(template, "name",0))[0]; /* get new symbol prefix  */
    }
  }
  bbox(BEGIN,0.0,0.0,0.0,0.0);

  for(k=0;k<lastselected;k++)
  {
   if(debug_var>=1) fprintf(errfp, "update_symbol(): for k loop: k=%d\n", k);
   if(selectedgroup[k].type!=ELEMENT) continue;
   i=selectedgroup[k].n;

   /* 20171220 calculate bbox before changes to correctly redraw areas */
   /* must be recalculated as cairo text extents vary with zoom factor. */
   symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2);

   if(sym_number>=0) /* changing symbol ! */
   {
     if(!pushed) { push_undo(); pushed=1;}
     delete_inst_node(i); /* 20180208 fix crashing bug: delete node info if changing symbol */
                          /* if number of pins is different we must delete these data *before* */
                          /* changing ysmbol, otherwise i might end up deleting non allocated data. */
     my_strdup(82, &inst_ptr[i].name, rel_sym_path(symbol));
     if(event_reporting) {
       char n1[PATH_MAX];
       char n2[PATH_MAX];
       printf("xschem replace_symbol instance %s %s\n",
           escape_chars(n1, inst_ptr[i].instname, PATH_MAX),
           escape_chars(n2, symbol, PATH_MAX)
       );
       fflush(stdout);
     }
     inst_ptr[i].ptr=sym_number;
   }
   bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
   /* update property string from tcl dialog */
   if(!no_change_props)
   {
    if(debug_var>=1) fprintf(errfp, "update_symbol(): no_change_props=%d\n", no_change_props);
    if(only_different) {
          char * ss=NULL;
          my_strdup(119, &ss, inst_ptr[i].prop_ptr);
          if( set_different_token(&ss, new_prop, old_prop, 0, 0) ) {
            if(!pushed) { push_undo(); pushed=1;}
            my_strdup(111, &inst_ptr[i].prop_ptr, ss);
            my_free(&ss);
            set_modify(1);
            prepared_hash_instances=0; /* 20171224 */
            prepared_netlist_structs=0;
            prepared_hilight_structs=0;
          }
    }
    else {
      if(new_prop) {  /* 20111205 */
        if(!inst_ptr[i].prop_ptr || strcmp(inst_ptr[i].prop_ptr, new_prop)) {
          if(debug_var>=1) fprintf(errfp, "update_symbol(): changing prop: |%s| -> |%s|\n", inst_ptr[i].prop_ptr, new_prop);
          if(!pushed) { push_undo(); pushed=1;}
          my_strdup(84, &inst_ptr[i].prop_ptr, new_prop);
          set_modify(1);
          prepared_hash_instances=0; /* 20171224 */
          prepared_netlist_structs=0;
          prepared_hilight_structs=0;
        }
      }  else {  /* 20111205 */
        if(!pushed) { push_undo(); pushed=1;}
        my_strdup(86, &inst_ptr[i].prop_ptr, "");
      }
    }
   }
   my_strdup(88, &name, get_tok_value(inst_ptr[i].prop_ptr, "name", 0));
   if(name && name[0] )  /* 30102003 */
   {  
    if(debug_var>=1) fprintf(errfp, "update_symbol(): prefix!='\\0', name=%s\n", name);

    /* 20110325 only modify prefix if prefix not NUL */
    if(prefix) name[0]=prefix; /* change prefix if changing symbol type; */
    if(debug_var>=1) fprintf(errfp, "update_symbol(): name=%s, inst_ptr[i].prop_ptr=%s\n", name, inst_ptr[i].prop_ptr);
    my_strdup(89, &ptr,subst_token(inst_ptr[i].prop_ptr, "name", name) );
                   /* set name of current inst */
    if(sym_number >=0) {
      if(!pushed) { push_undo(); pushed=1;}
      if(!k) hash_all_names(i);
      new_prop_string(i, ptr, k, disable_unique_names); /* set new prop_ptr */
    }
    type=instdef[inst_ptr[i].ptr].type; /* 20150409 */
    cond= !type || (strcmp(type,"label") && strcmp(type,"ipin") &&
          strcmp(type,"opin") &&  strcmp(type,"iopin"));
    if(cond) inst_ptr[i].flags|=2;
    else inst_ptr[i].flags &=~2;
   }
   if(event_reporting) {
     char *ss=NULL;
     my_strdup(120, &ss, inst_ptr[i].prop_ptr);
     set_different_token(&ss, new_prop, old_prop, ELEMENT, i);
     my_free(&ss);
   }
   my_strdup2(90, &inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name",0)); /* 20150409 */
   /* new symbol bbox after prop changes (may change due to text length) */
   symbol_bbox(i, &inst_ptr[i].x1, &inst_ptr[i].y1, &inst_ptr[i].x2, &inst_ptr[i].y2);
 
   bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
  } 
  /* redraw symbol with new props */
  bbox(SET,0.0,0.0,0.0,0.0);
  if(debug_var>=1) fprintf(errfp, "update_symbol(): redrawing inst_ptr.txtprop string\n");
  draw();
  bbox(END,0.0,0.0,0.0,0.0);
  /* 20160308 added if(), leave edited objects selected after updating properties */
  /* unless i am clicking another element with edit property dialog box open */
  /* in this latter case the last pointed element remains selected. */
  if( !strcmp(tclgetvar("editprop_semaphore"), "2")) {
    unselect_all();
    select_object(mousex,mousey,SELECTED, 0);
  }
  rebuild_selected_array();
}

void fill_symbol_editprop_form(int x) 
{
   /* 20160423 if no more stuff selected close editprop toplevel form */
   if(lastselected==0 || selectedgroup[0].type!=ELEMENT) Tcl_GlobalEval(interp, "set editprop_semaphore 0");

   if(!lastselected) return;
   i=selectedgroup[0].n;

   /* 20160423 */
   if(selectedgroup[0].type!=ELEMENT) return;
  
   if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): element %d property=%s\n",i,inst_ptr[i].prop_ptr);
   if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): modified=%d\n", modified);
   if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): symbol=%s\n", inst_ptr[i].name);
   if(inst_ptr[i].prop_ptr!=NULL)
   {
    if(debug_var>=1) fprintf(errfp, "fill_symbol_editprop_form(): element %d property=%s\n",i,inst_ptr[i].prop_ptr);
 
    if(netlist_commands && x==1) {
    /* 20070318 */
      tclsetvar("retval",get_tok_value( inst_ptr[i].prop_ptr,"value",0));
    } else {
      tclsetvar("retval",inst_ptr[i].prop_ptr);
    }
   }
   else
   {
     /* set_inst_prop(i); */  /* getting prop_ptr from template should be done only on 1st placement */
     if(inst_ptr[i].prop_ptr!=NULL) {
      if(netlist_commands && x==1) {
      /* 20070318 */
        tclsetvar("retval",get_tok_value( inst_ptr[i].prop_ptr,"value",0));
      } else {
        tclsetvar("retval",inst_ptr[i].prop_ptr);
      }
     } else {
      if(netlist_commands && x==1) {
      /* 20070318 */
        tclsetvar("retval","");
      } else {
        tclsetvar("retval","");
      }
     }
   }

   /* moved here from update_symbol() 20110413 */
   if(netlist_commands && x==1) {
     my_strdup(91, &old_prop, inst_ptr[i].prop_ptr);
   } else {
     my_strdup(92, &old_prop, (char *) tclgetvar("retval"));
   }
   /* /20110413 */

   tclsetvar("symbol",inst_ptr[i].name);
}

void change_elem_order(void)
{
   Instance tmpinst;
   Box tmpbox;
   Wire tmpwire;
   char tmp_txt[50]; /* overflow safe */
   int c, new_n;

    rebuild_selected_array();
    if(lastselected==1)
    {
     my_snprintf(tmp_txt, S(tmp_txt), "%d",selectedgroup[0].n);
     tclsetvar("retval",tmp_txt);
     tcleval("text_line {Object Sequence number} 0");
     if(strcmp(tclgetvar("rcode"),"") )
     {
      push_undo(); /* 20150327 */
      set_modify(1);
      prepared_hash_instances=0; /* 20171224 */
      prepared_netlist_structs=0;
      prepared_hilight_structs=0;
     }
     sscanf(tclgetvar("retval"), "%d",&new_n);

     if(selectedgroup[0].type==ELEMENT)
     {
      if(new_n>=lastinst) new_n=lastinst-1;
      tmpinst=inst_ptr[new_n];
      inst_ptr[new_n]=inst_ptr[selectedgroup[0].n];
      inst_ptr[selectedgroup[0].n]=tmpinst;
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
     else if(selectedgroup[0].type==WIRE)
     {
      if(new_n>=lastwire) new_n=lastwire-1;
      tmpwire=wire[new_n];
      wire[new_n]=wire[selectedgroup[0].n];
      wire[selectedgroup[0].n]=tmpwire;
      if(debug_var>=1) fprintf(errfp, "change_elem_order(): selected element %d\n", selectedgroup[0].n);
     }

    }
}

/* x=0 use tcl text widget  x=1 use vim editor  x=2 only view data */
void edit_property(int x)
{
 int j;
 if(!has_x) return;
 rebuild_selected_array(); /* from the .sel field in objects build */
 if(lastselected==0 )      /* the array of selected objs */
 {
   if(netlist_type==CAD_VERILOG_NETLIST && current_type==SCHEMATIC) {
    if(schverilogprop!=NULL)    /*09112003 */
      tclsetvar("retval",schverilogprop);
    else
      tclsetvar("retval","");
   }
   else if(netlist_type==CAD_SPICE_NETLIST && current_type==SCHEMATIC) { /* 20100217 */
    if(schprop!=NULL) 
      tclsetvar("retval",schprop);
    else
      tclsetvar("retval","");
   }
   else if(netlist_type==CAD_TEDAX_NETLIST && current_type==SCHEMATIC) { /* 20100217 */
    if(schtedaxprop!=NULL) 
      tclsetvar("retval",schtedaxprop);
    else
      tclsetvar("retval","");
   }
   else { /* this is used for symbol global props also */
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
     if(current_type==SYMBOL && /* 20120404 added case for symbol editing, use schvhdlprop regardless of netlisting mode */
        (!schvhdlprop || strcmp(schvhdlprop, tclgetvar("retval") ) ) ) { /* symbol edit */
        set_modify(1); push_undo(); /* 20150327 */
        my_strdup(93, &schvhdlprop, (char *) tclgetvar("retval"));
     } else if(netlist_type==CAD_VERILOG_NETLIST && current_type==SCHEMATIC && /* 20120228 check if schverilogprop NULL */
        (!schverilogprop || strcmp(schverilogprop, tclgetvar("retval") ) ) ) { /* 20120209 */
        set_modify(1); push_undo(); /* 20150327 */
        my_strdup(94, &schverilogprop, (char *) tclgetvar("retval")); /*09112003 */
    
     } else if(netlist_type==CAD_SPICE_NETLIST && current_type==SCHEMATIC && /* 20120228 check if schprop NULL */
        (!schprop || strcmp(schprop, tclgetvar("retval") ) ) ) { /* 20120209 */
        set_modify(1); push_undo(); /* 20150327 */
        my_strdup(95, &schprop, (char *) tclgetvar("retval")); /*09112003  */

     } else if(netlist_type==CAD_TEDAX_NETLIST && current_type==SCHEMATIC && /* 20120228 check if schprop NULL */
        (!schtedaxprop || strcmp(schtedaxprop, tclgetvar("retval") ) ) ) { /* 20120209 */
        set_modify(1); push_undo(); /* 20150327 */
        my_strdup(96, &schtedaxprop, (char *) tclgetvar("retval")); /*09112003  */

     } else if(netlist_type==CAD_VHDL_NETLIST && current_type==SCHEMATIC && /* 20120228 check if schvhdlprop NULL */
        (!schvhdlprop || strcmp(schvhdlprop, tclgetvar("retval") ) ) ) { /* netlist_type==CAD_VHDL_NETLIST */
        set_modify(1); push_undo(); /* 20150327 */
        my_strdup(97, &schvhdlprop, (char *) tclgetvar("retval"));
     }
   }

   /* update the bounding box of vhdl "architecture" instances that embed */
   /* the schvhdlprop string. 04102001 */
   for(j=0;j<lastinst;j++)
   {
    if( !strcmp( (inst_ptr[j].ptr+instdef)->type, "architecture") ) /* 20150409 */
    {
      if(debug_var>=1) fprintf(errfp, "edit_property(): updating vhdl architecture\n");
      symbol_bbox(j, &inst_ptr[j].x1, &inst_ptr[j].y1,
                        &inst_ptr[j].x2, &inst_ptr[j].y2);
    } 
   } /* end for(j */
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
   } else { /* 20161208 */
     tclsetvar("retval","");
   }
   tcleval("text_line {Input property:} 0");
   if(strcmp(tclgetvar("rcode"),"") )
   {
    set_modify(1); push_undo(); /* 20150327 */
    my_strdup(98, &arc[selectedgroup[0].col][selectedgroup[0].n].prop_ptr,
     (char *) tclgetvar("retval"));
   }
   break;

  case RECT:
   edit_rect_property();
   break;
  case WIRE:
   edit_wire_property();
   break;
  case POLYGON: /* 20171115 */
   edit_polygon_property();
   break;
  case LINE:
   edit_line_property();
   break;
  case TEXT:
   edit_text_property(x);
   break;             
 }

}


