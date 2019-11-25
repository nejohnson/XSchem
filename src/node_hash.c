/* File: node_hash.c
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

static struct node_hashentry *table[HASHSIZE];

static unsigned int hash(char *tok)
{
  unsigned int hash = 0;
  int c;

  while ( (c = *tok++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  return hash;
}


struct node_hashentry **get_node_table_ptr(void)
{
 return table;
}

void print_vhdl_signals(FILE *fd)
{
 struct node_hashentry *ptr;
 int i, found;
 int mult,j;
 static char *class=NULL;

 found=0;
 for(i=0;i<HASHSIZE;i++)
 {
  ptr = table[i];
  while(ptr)
  {
   if(ptr->d.port == 0 )
   {
    found = 1;
    if(ptr->token[0]=='#')
    {
     mult=get_unnamed_node(3, 0,  atoi((ptr->token)+4) );
    }
    else 
    {
     mult=1;
    }
    if(debug_var>=2) fprintf(errfp, " print_vhdl_signals(): node: %s mult: %d value=%s \n\n",
           ptr->token,mult, ptr->value?ptr->value:"NULL");

    if( ptr->class && ptr->class[0] )
     my_strdup(277, &class, ptr->class);
    else
     my_strdup(278, &class, "signal");

    if(mult>1)
    {
     for(j=mult-1;j>=0;j--)
     { 
      fprintf(fd, "%s %s[%d] : ", class, ptr->token[0]=='#' ? ptr->token+1 : ptr->token,j);
      if(ptr->sig_type && ptr->sig_type[0])
      { 
         fprintf(fd, "%s", ptr->sig_type);
      }
      else
        fprintf(fd, "std_logic");
      if(ptr->value && ptr->value[0]) fprintf(fd, " := %s ", ptr->value);
      fprintf(fd, " ; %s\n", ptr->orig_tok);
     }
    }
    else
    {
     fprintf(fd, "%s %s : ", class, ptr->token[0]=='#' ? ptr->token+1 : ptr->token);

     if(ptr->sig_type && ptr->sig_type[0])
     { 
        fprintf(fd, "%s", ptr->sig_type);
     }
     else
       fprintf(fd, "std_logic");
     if(ptr->value && ptr->value[0]) fprintf(fd, " := %s ", ptr->value);
     fprintf(fd, " ; %s\n", ptr->orig_tok);
    }
   }
   ptr = ptr->next;
  }
 }
 if(found) fprintf(fd, "\n" );
}



void print_verilog_signals(FILE *fd)
{
 struct node_hashentry *ptr;
 int i, found;
 int mult,j;

 if(debug_var>=2) fprintf(errfp, " print_verilog_signals(): entering routine\n");
 found=0;
 for(i=0;i<HASHSIZE;i++)
 {
  ptr = table[i];
  while(ptr)
  {
   if(ptr->d.port == 0 )
   {
    found = 1;
    if(ptr->token[0]=='#')
    {
     mult=get_unnamed_node(3, 0,  atoi((ptr->token)+4) );
    }
    else 
    {
     mult=1;
    }
    if(debug_var>=2) fprintf(errfp, " print_verilog_signals(): node: %s mult: %d value=%s \n\n",
           ptr->token,mult, ptr->value?ptr->value:"NULL");

    if(mult>1)
    {
     for(j=mult-1;j>=0;j--)
     { 
      if(ptr->verilog_type && ptr->verilog_type[0]) /*09112003 */
      { 
         fprintf(fd, "%s ", ptr->verilog_type);
      }
      else
        fprintf(fd, "wire ");
      fprintf(fd, "%s[%d] ", ptr->token[0]=='#' ? ptr->token+1 : ptr->token,j);
      if(ptr->value && ptr->value[0]) fprintf(fd, "= %s ", ptr->value);
      fprintf(fd, "; // %s\n", ptr->orig_tok);
     }
    }
    else
    {

     if(ptr->verilog_type && ptr->verilog_type[0])  /*09112003 */
     { 
        fprintf(fd, "%s ", ptr->verilog_type);
     }
     else
       fprintf(fd, "wire ");
     fprintf(fd, "%s ", ptr->token[0]=='#' ? ptr->token+1 : ptr->token);
     if(ptr->value && ptr->value[0]) fprintf(fd, "= %s ", ptr->value);
     fprintf(fd, "; // %s\n", ptr->orig_tok);
    }
   }
   ptr = ptr->next;
  }
 }
 if(found) fprintf(fd, "\n" );
}


/* wrapper to node_hash_lookup that handles buses */
/* warning, in case of buses return only pointer to first bus element */
struct node_hashentry *bus_hash_lookup(char *token, char *dir,int remove,int port,
       char *sig_type,char *verilog_type, char *value, char *class)
{
 char *start, *string_ptr, c;
 int mult;
 char *string=NULL;
 struct node_hashentry *ptr1=NULL, *ptr2=NULL;

 if(token==NULL) return NULL;
 if( token[0] == '#')
 {
   my_strdup(279, &string, token);
 }
 else
 {
   if(debug_var >=3) fprintf(errfp, "bus_hash_lookup(): expanding node: %s\n", token);
   my_strdup(280, &string, expandlabel(token,&mult));
   if(debug_var >=3) fprintf(errfp, "bus_hash_lookup(): done expanding node: %s\n", token);
 }
 if(string==NULL) return NULL;
 string_ptr = start = string; 
 while(1)
 {
  c=(*string_ptr);
  if(c==','|| c=='\0')
  {
    *string_ptr='\0';  /* set end string at comma position.... */
    /* insert one bus element at a time in hash table */
    ptr1=node_hash_lookup(start, dir, remove,port, sig_type, verilog_type, value, class, token);
    if(!ptr2) ptr2=ptr1;
    if(debug_var >=3) fprintf(errfp, "bus_hash_lookup(): processing node: %s\n", start);
    *string_ptr=c;     /* ....restore original char */
    start=string_ptr+1;
  }
  if(c==0) break;
  string_ptr++;
 }
 /* if something found return first pointer */
 my_free(&string);
 return ptr2;
}


struct node_hashentry *node_hash_lookup(char *token, char *dir,int remove,int port,
       char *sig_type, char *verilog_type, char *value, char *class, char *orig_tok)
/*    token        dir      remove    ... what ... */
/* -------------------------------------------------------------------------- */
/* "whatever"     "in"/"out"    0       insert in hash table if not in and return NULL */
/*                                      if already present just return entry address  */
/*                                      and update in/out fields sum up port field */
/*                                      return NULL otherwise */
/* */
/* "whatever"     whatever      1       delete entry if found return NULL */
/* "whatever"     whatever      2       only look up element, dont insert */
{
 unsigned int hashcode, index;
 struct node_hashentry *entry, *saveptr, **preventry;
 char *ptr;
 int s ;
 struct drivers d;

 if(debug_var>=3) fprintf(errfp, "node_hash_lookup(): called with: %s dir=%s remove=%d port=%d\n",
        token, dir, remove, port);
 d.in=d.out=d.inout=0;
 if(!strcmp(dir,"in") )  d.in=1;
 else if(!strcmp(dir,"out") ) d.out=1;
 else if(!strcmp(dir,"inout") ) d.inout=1;
 d.port=port;
 if(token==NULL || token[0]==0 ) return NULL;
 hashcode=hash(token); 
 index=hashcode % HASHSIZE; 
 entry=table[index];
 preventry=&table[index];
 while(1)
 {
  if( !entry )                  /* empty slot */
  {
   if( remove==0 )              /* insert data */
   {
    s=sizeof( struct node_hashentry );
    ptr= my_malloc(281, s );
    entry=(struct node_hashentry *)ptr;
    entry->next = NULL;
    entry->token = entry->sig_type = entry->verilog_type = 
                   entry->value = entry->class = entry->orig_tok = NULL;
    my_strdup(282, &(entry->token),token);
    if(sig_type &&sig_type[0]) my_strdup(283,  &(entry->sig_type), sig_type); /* 24092001 */
    if(verilog_type &&verilog_type[0]) my_strdup(284,  &(entry->verilog_type), verilog_type); /* 09112003 */
    if(class && class[0]) my_strdup(285,  &(entry->class), class); /* 07102001 */
    if(orig_tok && orig_tok[0]) my_strdup(286,  &(entry->orig_tok), orig_tok); /* 08102001 */
    if(value && value[0]) my_strdup(287,  &(entry->value), value); /* 27092001 */
    entry->d.port=d.port;
    entry->d.in=d.in;
    entry->d.out=d.out;
    entry->d.inout=d.inout;
    entry->hash=hashcode;
    *preventry=entry;
    if(debug_var>=3) fprintf(errfp, "node_hash_lookup(): hashing %s : value=%s\n\n",
           entry->token, entry->value? entry->value:"NULL");
    if(debug_var>=3) fprintf(errfp, "node_hash_lookup(): hashing %s in=%d out=%d inout=%d port=%d\n",
                token, d.in, d.out, d.inout, d.port);
   }
   return NULL; /* whether inserted or not return NULL since it was not in */
  }
  if( entry -> hash==hashcode && strcmp(token,entry->token)==0 ) /* found matching tok */
  {
   if(remove==1)                /* remove token from the hash table ... */
   {
    saveptr=entry->next;
    if(entry->token) my_free(& entry->token);
    if(entry->verilog_type) my_free(& entry->verilog_type); /* 09112003 */
    if(entry->sig_type) my_free(& entry->sig_type); /* 24092001 */
    if(entry->class) my_free(& entry->class); /* 07102001 */
    if(entry->orig_tok) my_free(& entry->orig_tok); /* 07102001 */
    if(entry->value) my_free(& entry->value); /* 27092001 */
    my_free(&entry);
    *preventry=saveptr;
    return NULL;
   }
   else /* found matching entry, return the address and update in/out count */
   {
    entry->d.port+=port;
    entry->d.in+=d.in;
    entry->d.out+=d.out;
    entry->d.inout+=d.inout;
    if(sig_type && sig_type[0] !='\0')
      my_strdup(288,  &(entry->sig_type), sig_type); /* 24092001 */
    if(verilog_type && verilog_type[0] !='\0')
      my_strdup(289,  &(entry->verilog_type), verilog_type); /* 09112003 */
    if(value && value[0] !='\0')
      my_strdup(290,  &(entry->value), value); /* 27092001 */
    if(debug_var>=3) fprintf(errfp, "node_hash_lookup(): hashing %s : value=%s\n\n",
           entry->token, entry->value? entry->value:"NULL");
    return entry;
   }
  } 
  preventry=&entry->next; /* descend into the list. */
  entry = entry->next;
 }
}

void traverse_node_hash()
{
 int i;
 struct node_hashentry *entry;
 char str[2048]; /* 20161122 overflow safe */

 if(!show_erc)return;
 for(i=0;i<HASHSIZE;i++)
 {
  entry = table[i];
  while(entry)
  {
   if(entry->d.out + entry->d.inout + entry->d.in == 1)
   {
     my_snprintf(str, S(str), "open net: %s", entry->token);
     if(!netlist_count) bus_hilight_lookup(entry->token, hilight_color,0);
     if(incr_hilight) hilight_color++;
     statusmsg(str,2);
   }
   else if(entry->d.out ==0  && entry->d.inout == 0) 
   {
     my_snprintf(str, S(str), "undriven node: %s", entry->token);
     if(!netlist_count) bus_hilight_lookup(entry->token, hilight_color,0);
     if(incr_hilight) hilight_color++;
     statusmsg(str,2);
   }
   else if(entry->d.out >=2 && entry->d.port>=0)  /*  era d.port>=2   03102001 */
   {
     my_snprintf(str, S(str), "shorted output node: %s", entry->token);
     if(!netlist_count) bus_hilight_lookup(entry->token, hilight_color,0);
     if(incr_hilight) hilight_color++;
     statusmsg(str,2);
   }
   else if(entry->d.in ==0 && entry->d.inout == 0) 
   {
     my_snprintf(str, S(str), "node: %s goes nowhere", entry->token);
     if(!netlist_count) bus_hilight_lookup(entry->token, hilight_color,0);
     if(incr_hilight) hilight_color++;
     statusmsg(str,2);
   }
   else if(entry->d.out >=2 && entry->d.inout == 0 && entry->d.port>=0)  /*  era d.port>=2   03102001 */
   {
     my_snprintf(str, S(str), "shorted output node: %s", entry->token);
     if(!netlist_count) bus_hilight_lookup(entry->token, hilight_color,0);
     if(incr_hilight) hilight_color++;
     statusmsg(str,2);
   }

    if(debug_var>=1) fprintf(errfp, "traverse_node_hash(): node: %s in=%d out=%d inout=%d port=%d\n", 
        entry->token, entry->d.in, entry->d.out, entry->d.inout, entry->d.port);

   entry = entry->next;
  }
 }
}

static  int collisions, max_collisions=0, n_elements=0;

static struct node_hashentry *free_hash_entry(struct node_hashentry *entry)
{
  struct node_hashentry *tmp;

  while(entry) {
    n_elements++; collisions++;
    tmp = entry->next;
    if(entry->token) my_free(&entry->token);
    if(entry->verilog_type) my_free(&entry->verilog_type); /* 09112003 */
    if(entry->sig_type) my_free(&entry->sig_type); /* 24092001 */
    if(entry->class) my_free(&entry->class); /* 07102001 */
    if(entry->orig_tok) my_free(&entry->orig_tok); /* 07102001 */
    if(entry->value) my_free(&entry->value); /* 27092001 */
    my_free(&entry);
    entry = tmp;
  }
  return NULL;
}

void free_node_hash(void) /* remove the whole hash table  */
{
 int i;
  
 if(debug_var>=2) fprintf(errfp, "free_node_hash(): removing hash table\n");
 n_elements=0;
 for(i=0;i<HASHSIZE;i++)
 {
  collisions=0;
  table[i] = free_hash_entry( table[i] );
  if(collisions>max_collisions) max_collisions=collisions;
 }
 if(debug_var>=1) fprintf(errfp, "# free_node_hash(): max_collisions=%d n_elements=%d hashsize=%d\n",
                   max_collisions, n_elements, HASHSIZE);
 max_collisions=0;

}

