/* File: token.c
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
#define SPACE(c) ( c=='\n' || c==' ' || c=='\t' || \
                   c=='\0' || c==';' || c=='/' )

/* 20150317 */
#define SPACE2(c) ( SPACE(c) || c=='\'' || c== '"')
#define SPACE3(c) ( SPACE(c) || c=='*' ) /* 20150418 used in translate() */

enum status {XBEGIN, XTOKEN, XSEPARATOR, XVALUE, XEND, XENDTOK};

struct hashentry {
                  struct hashentry *next;
                  unsigned int hash;
                  char *token;
                  char *value;
                 };

static struct hashentry *table[HASHSIZE];

/* calculate the hash function relative to string s */
static unsigned int hash(char *tok)
{
  unsigned int hash = 0;
  int c;

  while ( (c = *tok++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  return hash;
}

int name_strcmp(char *s, char *d) /* compare strings up to '\0' or'[' */
{
 int i=0;
 while(1)
 {
  if(d[i]=='\0' || d[i]=='[')
  {
    if(s[i]!='\0' && s[i]!='[') return 1;
    return 0;
  }
  if(s[i]=='\0' || s[i]=='[')
  {
    if(d[i]!='\0' && d[i]!='[') return 1;
    return 0;
  }
  if(s[i]!=d[i]) return 1;
  i++;
 }
}


/* 20180926 added token_size */
static struct hashentry *hash_lookup(char *token, char *value,int remove, size_t token_size)
/*    token        value      remove    ... what ... */
/* -------------------------------------------------------------------------- */
/* "whatever"    "whatever"     0       insert in hash table if not in. */
/*                                      if already present just return entry  */
/*                                      address, NULL otherwise */
/* "whatever"       NULL        0       lookup in hash table,return entry addr. */
/*                                      return NULL if not found */
/* "whatever"    "whatever"     1       delete entry if found,return NULL */
/* "whatever"       NULL        1       delete entry if found,return NULL */
{
 unsigned int hashcode; 
 unsigned int index;
 struct hashentry *entry, *saveptr, **preventry;
 char *ptr;
 int s ;
 

  if(token==NULL) return NULL;
  hashcode=hash(token); 
  index=hashcode % HASHSIZE; 
  entry=table[index];
  preventry=&table[index];
  while(1) {
    if( !entry ) {              /* empty slot */
      if(value && !remove) {            /* insert data */
        s=sizeof( struct hashentry );
        ptr= my_malloc(425, s);
        entry=(struct hashentry *)ptr;
        *preventry=entry;
        entry->next=NULL;
        entry->hash=hashcode;
        entry->token=NULL;
        entry->token = my_malloc(426, token_size + 1);
        memcpy(entry->token,token, token_size + 1);
        entry->value = value;
        return NULL; /* if element inserted return NULL since it was not in table */
      }
      return entry;
    }
    if( entry->hash==hashcode && !strcmp(token,entry->token) ) {
      /* found a matching token */
      if(remove) {              /* remove token from the hash table ... */
        saveptr=entry->next;
        my_free(&entry->token);
        my_free(&entry);
        *preventry=saveptr;
        return NULL;
      }
      else return entry;        /* found matching entry, return the address */
    } 
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
  }
}

static  int collisions, max_collisions=0, n_elements=0;



static struct hashentry *free_hash_entry(struct hashentry *entry)
{
  struct hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(&(entry->token));
    n_elements++;
    collisions++;
    if(debug_var>=3) fprintf(errfp, "free_hash_entry(): removing entry %lu\n", (unsigned long)entry);
    /* entry->token and entry->value dont need to be freed since */
    /* entry struct allocated with token and value included */
    my_free(&entry);
    entry = tmp;
  }
  return NULL;
}


void free_hash(void) /* remove the whole hash table  */
{
 int i;
  
 if(debug_var>=3) fprintf(errfp, "free_hash(): removing hash table\n");
 n_elements=0;
 for(i=0;i<HASHSIZE;i++)
 {
  collisions=0;
  table[i] = free_hash_entry( table[i] );
  if(collisions>max_collisions) max_collisions=collisions;

 }
 if(debug_var>=1) fprintf(errfp, "# free_hash(): max_collisions=%d n_elements=%d hashsize=%d\n", 
                   max_collisions, n_elements, HASHSIZE);
 max_collisions=0;
}

/* insert **only** name in hash table */
void hash_proplist(int i, int remove) /* 20171205 */
{
  char *name=inst_ptr[i].instname;
  
  if(name && name[0]) hash_lookup(name, "", remove, strlen(name));
}

void check_unique_names(int rename)
{
  int i;
  char *tmp = NULL;
  int newpropcnt = 0;
  /* int save_draw; */

  if(rename == 1) {
    /* save_draw = draw_window;*/
    /* draw_window=1; */
    bbox(BEGIN,0.0,0.0,0.0,0.0);
    set_modify(1); push_undo();
    prepared_hash_instances=0;
    prepared_netlist_structs=0;
    prepared_hilight_structs=0;
  }
  free_hash();
  for(i=0;i<lastinst;i++) {
    if(inst_ptr[i].instname[0] && hash_lookup(inst_ptr[i].instname, "", 0, strlen(inst_ptr[i].instname))) {
      if(rename == 1) bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
      inst_ptr[i].flags |=4;
      hilight_nets=1;
      my_strdup(511, &tmp, inst_ptr[i].prop_ptr);
      new_prop_string(&inst_ptr[i].prop_ptr, tmp, newpropcnt++, !rename);
      my_strdup2(512, &inst_ptr[i].instname, get_tok_value(inst_ptr[i].prop_ptr, "name", 0)); /* 20150409 */
      hash_proplist(i, 0);
      if(rename == 1) bbox(ADD, inst_ptr[i].x1, inst_ptr[i].y1, inst_ptr[i].x2, inst_ptr[i].y2);
    }
  }
  if(rename == 1) {
    bbox(SET,0.0,0.0,0.0,0.0);
    draw();
    bbox(END,0.0,0.0,0.0,0.0);
  }
  my_free(&tmp);
  draw_hilight_net(1);
  /* draw_window = save_draw; */
}



int match_symbol(char *name)  /* never returns -1, if symbol not found load systemlib/missing.sym */
{
 int i,found;

 found=0;
 for(i=0;i<lastinstdef;i++)
 {
  /* if(debug_var>=1) fprintf(errfp, "match_symbol(): name=%s, instdef[i].name=%s\n",name, instdef[i].name);*/
  if(strcmp(name, instdef[i].name) == 0)
  {
   if(debug_var>=2) fprintf(errfp, "match_symbol(): found matching symbol:%s\n",name);
   found=1;break;
  }
 }
 if(!found)
 {
  if(debug_var>=1) fprintf(errfp, "match_symbol(): matching symbol not found:%s, loading\n",name);
  if(load_symbol_definition(name, NULL)==-1) return -1;
 }
 if(debug_var>=2) fprintf(errfp, "match_symbol(): returning %d\n",i);
 return i;
}

/* update **s modifying only the token values that are */
/* different between *new and *old */
/* return 1 if s modified 20081221 */
int set_different_token(char **s,char *new, char *old, int object, int n)
{
 register int c, state=XBEGIN, space;
 static char *token=NULL, *value=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int mod;

 mod=0;
 if(debug_var>=1) fprintf(errfp, "set_different_token(): *s=%s, new=%s, old=%s\n",*s, new, old);
 if(new==NULL) return 0;
 sizetok=CADCHUNKALLOC;
 if(token==NULL) token=my_malloc(427, sizetok);
 else my_realloc(428, &token,sizetok);
 sizeval=CADCHUNKALLOC;
 if(value==NULL) value=my_malloc(429, sizeval);
 else my_realloc(430, &value,sizeval);
 while(1) {
  c=*new++; 
  space=SPACE(c) ;
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;
  if(value_pos>=sizeval) {
   sizeval+=CADCHUNKALLOC;
   my_realloc(431, &value,sizeval);
  }
  if(token_pos>=sizetok) {
   sizetok+=CADCHUNKALLOC;
   my_realloc(432, &token,sizetok);
  }
  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) {
   if(c=='"' && !escape) quote=!quote;
   /*else value[value_pos++]=c; */
   value[value_pos++]=c;
   if(c=='\\') {
     escape=1;
     /*c=*new++; */
   } else escape=0;
  }
  else if(state==XENDTOK || state==XSEPARATOR) {
   if(token_pos) {
     token[token_pos]='\0'; 
     token_pos=0;
   }
  } else if(state==XEND) {
   value[value_pos]='\0';
   value_pos=0;
   if(strcmp(value, get_tok_value(old,token,1))) {

    if(event_reporting && object == ELEMENT) {
      char n1[PATH_MAX];
      char n2[PATH_MAX];
      char n3[PATH_MAX];
      printf("xschem setprop instance %s %s %s\n", 
           escape_chars(n1, inst_ptr[n].instname, PATH_MAX), 
           escape_chars(n2, token, PATH_MAX), 
           escape_chars(n3, value, PATH_MAX)
      );
      fflush(stdout);
    }

    mod=1;
    my_strdup(433, s, subst_token(*s, token, value) );
   }
   state=XBEGIN;
  }
  if(c=='\0') break;
 }
 return mod;
}


/* state machine that parses a string made up of <token>=<value> ... */
/* couples and returns the value of the given token  */
/* if s==NULL or no match return empty string */
/* NULL tok NOT ALLOWED !!!!!!!! */
/* never returns NULL... */
/* with_quotes: */
/* 0: eat non escaped quotes (") */
/* 1: return unescaped quotes as part of the token value if they are present */
/* 2: eat backslashes */
/* 3: 1+2  :) */
char *get_tok_value(const char *s,const char *tok, int with_quotes)
{
  static char *result=NULL;
  static char *token=NULL;
  static int size=0;
  static int  sizetok=0;
  register int c, state=XBEGIN, space;
  register int token_pos=0, value_pos=0;
  int quote=0;
  int escape=0;
 
  if(!size) {
    size=CADCHUNKALLOC;
    sizetok=CADCHUNKALLOC;
    my_realloc(434, &result,size);
    my_realloc(435, &token,sizetok);
  }
  if(s==NULL){result[0]='\0'; return result;}
    if(debug_var>=3) fprintf(errfp, "get_tok_value(): looking for <%s> in <%s>\n",tok,s);
  while(1) {
    c=*s++;
    space=SPACE(c) ;
    if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
    else if( state==XTOKEN && space) state=XENDTOK;
    else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
    else if( state==XSEPARATOR && !space) state=XVALUE;
    else if( state==XVALUE && space && !quote && !escape ) state=XEND;

    if(value_pos>=size) {
      size+=CADCHUNKALLOC;
      my_realloc(436, &result,size);
    }
    if(token_pos>=sizetok) {
      sizetok+=CADCHUNKALLOC;
      my_realloc(437, &token,sizetok);
    }
    if(state==XTOKEN) token[token_pos++]=c;
    else if(state==XVALUE) {
      if(c=='"') {
        if(!escape) quote=!quote;
        if((with_quotes & 1) || escape)  result[value_pos++]=c;
      }
      else if( !((c=='\\') && (with_quotes & 2)) ) result[value_pos++]=c; /* 20150411 fixed logical expression */
      else if( (c=='\\') && escape ) result[value_pos++]=c; /* 20170414 add escaped backslashes */
      escape = (c=='\\' && !escape);
    } else if(state==XENDTOK || state==XSEPARATOR) {
        if(token_pos) {
          token[token_pos]='\0';
          token_pos=0;
        }

    } else if(state==XEND) {
      result[value_pos]='\0';
      if( !strcmp(token,tok) ) {
        get_tok_value_size = value_pos; /* return also size so to avoid using strlen 20180926 */
        return result;
      }
      value_pos=0;
      state=XBEGIN;
    }
    if(c=='\0') {
      result[0]='\0';
      get_tok_value_size = 0; /* return also size so to avoid using strlen 20180926 */
      return result;
    }
  }
}

/* return template string excluding name=... and token=value where token listed in extra */
/* 20081206 */
char *get_sym_template(char *s,char *extra)
{
 static char *result=NULL;
 static char *value=NULL;
 static char *token=NULL;
 static int sizeres=0;
 static int sizeval=0;
 static int  sizetok=0;
 register int c, state=XBEGIN, space;
 register int token_pos=0, value_pos=0, result_pos=0;
 int quote=0;
 int escape=0;
 int with_quotes=0;
 int l;
/* with_quotes: */
/* 0: eat non escaped quotes (") */
/* 1: return unescaped quotes as part of the token value if they are present */
/* 2: eat backslashes */
/* 3: 1+2  :) */

 if(!sizeval) {
   sizeval=CADCHUNKALLOC;
   sizetok=CADCHUNKALLOC;
   my_realloc(438, &value,sizeval);
   my_realloc(439, &token,sizetok);
 }
 l = strlen(s);
 if(l >= sizeres) {
   sizeres = l+1;
   my_realloc(440, &result,sizeres);
 }
 if(s==NULL){result[0]='\0'; return result;}
 while(1) {
  c=*s++; 
  space=SPACE(c) ;
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;
  if(value_pos>=sizeval) {
    sizeval+=CADCHUNKALLOC;
    my_realloc(441, &value,sizeval);
  }
  if(token_pos>=sizetok) {
    sizetok+=CADCHUNKALLOC;
    my_realloc(442, &token,sizetok);
  }
  if(state==XBEGIN) {
    result[result_pos++] = c;
  } else if(state==XTOKEN) {
    token[token_pos++]=c;
  } else if(state==XVALUE) {
    if(c=='"') {
     if(!escape) quote=!quote;
     if((with_quotes & 1) || escape)  value[value_pos++]=c;
    }
    else if( (c=='\\') && (with_quotes & 2) )  ;  /* dont store backslash */
    else value[value_pos++]=c;
    if(c=='\\') escape=1;
    else escape=0;
  } else if(state==XEND) {
    value[value_pos]='\0';
    if((!extra || !strstr(extra, token)) && strcmp(token,"name")) {
      memcpy(result+result_pos, value, value_pos+1); /* 20180923 */
      result_pos+=value_pos;
    }
    result[result_pos++] = c;
    value_pos=0;
    token_pos=0;
    state=XBEGIN;
  } else if(state==XENDTOK || state==XSEPARATOR) {
    if(token_pos) {
      token[token_pos]='\0';
      if((!extra || !strstr(extra, token)) && strcmp(token,"name")) {
        memcpy(result+result_pos, token, token_pos+1); /* 20180923 */
        result_pos+=token_pos;
        result[result_pos++] = c;
      }
      token_pos=0;
    }
  }
  if(c=='\0') {
    return result;
  }
 }
}

const char *find_bracket(const char *s)
{
 while(*s!='['&& *s!='\0') s++;
 return s;
}

void new_prop_string(char **new_prop,const char *old_prop, int fast, int disable_unique_names)
{
/* given a old_prop property string, return a new */
/* property string in new_prop such that the element name is */
/* unique in current design (that is, element name is changed */
/* if necessary) */
/* if old_prop=NULL return NULL */
/* if old_prop does not contain a valid "name" or empty return old_prop */
 static char prefix, *old_name=NULL, *new_name=NULL;
 const char *tmp;
 const char *tmp2;
 int q,qq;
 static int last[256];
 int old_name_len; /* 20180926 */
 int new_name_len;

 if(!fast) {for(q=1;q<=255;q++) last[q]=1;}
 
 if(old_prop==NULL) 
 { 
  if(debug_var>=1) fprintf(errfp, "new_prop_string():-0-  old=%s fast=%d\n", old_prop,fast);
  my_strdup(443, new_prop,NULL);
  return;
 }
 if(debug_var>=1) fprintf(errfp, "new_prop_string(): new=%s   old=%s\n",*new_prop, old_prop);
 old_name_len = my_strdup(444, &old_name,get_tok_value(old_prop,"name",0) ); /* added old_name_len 20180926 */
 if(old_name==NULL) 
 { 
  /*my_strdup(445, new_prop,NULL); */
  my_strdup(446, new_prop,old_prop);  /* 03102001 changed to copy old props if no name */
  return;
 }
 prefix=old_name[0];
 /* don't change old_prop if name does not conflict. */
 if(disable_unique_names || hash_lookup(old_name, NULL, 0, old_name_len) == NULL)
 {
  my_strdup(447, new_prop, old_prop);
  if(debug_var>=1) fprintf(errfp, "new_prop_string():-1-  new=%s old=%s fast=%d\n",*new_prop, old_prop,fast);
  return;
 }
 tmp=find_bracket(old_name);
 my_realloc(448, &new_name, old_name_len + 40); /* strlen(old_name)+40); */ /* 20180926 */
 qq=fast ?  last[(int)prefix] : 1; 
 if(debug_var>=1) fprintf(errfp, "new_prop_string(): -2- new=%s old=%s fast=%d\n",*new_prop, old_prop,fast);
 for(q=qq;;q++)
 {
  new_name_len = my_snprintf(new_name, old_name_len + 40, "%c%d%s", prefix,q, tmp); /* added new_name_len 20180926 */
  if(hash_lookup(new_name, NULL, 0, new_name_len) == NULL) 
  {
   last[(int)prefix]=q+1;
   break;
  }
 } 
 tmp2 = subst_token(old_prop, "name", new_name); 
 if(strcmp(tmp2, old_prop) ) {
   if(debug_var>=1) fprintf(errfp, "new_prop_string(): tmp2=%s, old_prop=%s\n", tmp2, old_prop);
   my_strdup(449, new_prop, tmp2);
 }
}

char *subst_token(const char *s, const char *tok, const char *new_val)
/* given a string <s> with multiple "token=value ..." assignments */
/* substitute <tok>'s value with <new_val> */
/* if tok not found in s add tok=new_val at end. 04052001 */
/* if new_val is empty ('\0') set token value to "" (token="") */
/* if new_val is NULL *remove* 'token=val' from s */
{
 static char *result=NULL;
 int size=0, tmp;
 register int c, state=XBEGIN, space;
 static char *token=NULL;
 int sizetok=0;
 int token_pos=0, result_pos=0, result_save_pos = 0;
 int quote=0;
 int done_subst=0;
 int escape=0;

 if(debug_var>=1) fprintf(errfp, "subst_token(%s, %s, %s)\n", s, tok, new_val);
 if(new_val && !strcmp(new_val, "DELETE")) {
    if(!strcmp(tok,"name")) {
      fprintf(errfp,"subst_token(): Can not DELETE name attribute\n");
      my_strdup(450, &result, s);
      return result;
    }
    new_val = NULL;
 }
 sizetok=CADCHUNKALLOC;
 if(token==NULL) token=my_malloc(451, sizetok);
 else my_realloc(452, &token,sizetok);
 size=CADCHUNKALLOC;
 if(result==NULL) result=my_malloc(453, size);
 else my_realloc(454, &result,size);
 if(s==NULL){result[0]='\0';}
 while( s ) {
  c=*s++; 
  space=SPACE(c);
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') {
    result_save_pos = result_pos-1;
    if(result_pos < 0) result_pos = 0;
    token_pos=0 ;
    state=XTOKEN; 
  } else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;
  if(result_pos >= size) {
   size += CADCHUNKALLOC;
   my_realloc(455, &result, size);
  }
  if(token_pos >= sizetok) {
   sizetok += CADCHUNKALLOC;
   my_realloc(456, &token, sizetok); /* 20171104 **Long** standing bug fixed, was doing realloc on result instead of token. */
                               /* causing the program to crash on first very long token encountered */
  }
  if(state==XTOKEN) {
       token[token_pos++] = c;
       result[result_pos++] = c;
  } else if(state==XSEPARATOR) {
       token[token_pos] = '\0'; 
       if(!strcmp(token,tok) && !new_val) {
         result_pos = result_save_pos;
         if(debug_var>=3) fprintf(errfp, "subst_token(): result_pos=%d\n", result_pos);
       } else {
         result[result_pos++] = c;
       }
  } else if(state==XVALUE) {
    if(c == '"' && !escape) quote=!quote;
    if(c == '\\') escape = 1;
    else escape = 0;
    if(!strcmp(token,tok))
    {
     if(!done_subst)
     {
       if(new_val && new_val[0]) {
         tmp = strlen(new_val);
       } else if(new_val) {
         new_val = "\"\"";
         tmp = 2;
       }
       if(new_val) {
         if(result_pos + tmp >= size)
         {
           size = (1 + (result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
           my_realloc(457, &result, size);
         }
         memcpy(result + result_pos ,new_val, tmp + 1); /* 20180923 */
         result_pos += tmp;
       }
       done_subst = 1;
     }
    }
    else result[result_pos++]=c;
  } else if(state==XENDTOK) {
     token[token_pos]='\0';
     result[result_pos++] = c;
  } else if(state == XEND) {
       result[result_pos++] = c;
       state = XBEGIN;
  } else if(state == XBEGIN) {
       result[result_pos++] = c;
  }
  if(c == '\0')  break;
 } /* end while */
 if(!done_subst) { /* 04052001 if tok not found add tok=new_value at end */
  if(result[0] == '\0' && new_val) {
   if(!new_val[0]) new_val = "\"\"";
   tmp = strlen(new_val) + strlen(tok) + 2;
   if(result_pos + tmp >= size) {
     size = (1 + (result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(458, &result,size);
   }
   my_realloc(459, &result, size);
   my_snprintf(result, size, "%s=%s", tok, new_val ); /* overflow safe 20161122 */
  }
  else if(new_val) {
   if(!new_val[0]) new_val = "\"\"";
   tmp = strlen(new_val) + strlen(tok) + 2; /* 20171104 */
   if(result_pos + tmp >= size) {
     size = (1 + (result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(460, &result,size);
   }
   my_snprintf(result + result_pos - 1, size, " %s=%s", tok, new_val ); /* 20171104, 20171201 -> result_pos-1 */
  }
 }
 if(debug_var >= 2) fprintf(errfp, "subst_token(): returning: %s\n",result);
 return result;
}

const char *get_trailing_path(const char *str, int no_of_dir, int skip_ext)
{
  static char s[PATH_MAX];
  size_t len;
  int ext_pos, dir_pos, n_ext, n_dir, c, i;

  my_strncpy(s, str, S(s));
  len = strlen(s);

  for(ext_pos=len, dir_pos=len, n_ext=0, n_dir=0, i=len; i>=0; i--) {
    c = s[i];
    if(c=='.' && ++n_ext==1) ext_pos = i;
    if(c=='/' && ++n_dir==no_of_dir+1) dir_pos = i;
  }
  if(skip_ext) s[ext_pos] = '\0';

  if(dir_pos==len) return s;
  if(debug_var>=2) fprintf(errfp, "get_trailing_path(): str=%s, no_of_dir=%d, skip_ext=%d\n", 
                   str, no_of_dir, skip_ext);
  if(debug_var>=2) fprintf(errfp, "get_trailing_path(): returning: %s\n", s+(dir_pos<len ? dir_pos+1 : 0));
  return s+(dir_pos<len ? dir_pos+1 : 0);
}

const char *skip_dir(const char *str)
{
  return get_trailing_path(str, 0, 1);
}
    
const char *get_cell(const char *str, int no_of_dir)
{ 
  return get_trailing_path(str, no_of_dir, 1);
}

const char *get_cell_w_ext(const char *str, int no_of_dir)
{ 
  return get_trailing_path(str, no_of_dir, 0);
}



 int count_labels(char *s)
{
 int i=1;
 int c;
 
 if(s==NULL) return 1;
 while( (c=(*s++)) )
 {
  if(c==',') i++;
 }
 return i;
}

void print_vhdl_element(FILE *fd, int inst) /* 20071217 */
{
 int i=0, mult, tmp, tmp1;
 const char *str_ptr;
 register int c, state=XBEGIN, space;
 const char *lab;
 static char *name=NULL;
 static char  *generic_value=NULL, *generic_type=NULL;
 static char *template=NULL,*s, *value=NULL,  *token=NULL;
 int no_of_pins=0, no_of_generics=0;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 if(get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"vhdl_format",2)[0] != '\0') {  /* 20071217 */
  print_vhdl_primitive(fd, inst); /*20071217 */
  return;
 }

 my_strdup(461, &template,
     (inst_ptr[inst].ptr+instdef)->templ); /* 2015049 */

 my_strdup(462, &name,inst_ptr[inst].instname); /* 20161210 */
 /* my_strdup(463, &name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); */

 if(name==NULL) return;
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 no_of_generics= (inst_ptr[inst].ptr+instdef)->rects[GENERICLAYER];

 s=inst_ptr[inst].prop_ptr;

/* print instance name and subckt */
  if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing inst name & subcircuit name\n");
 if( (lab = expandlabel(name, &tmp)) != NULL)
   fprintf(fd, "%d %s : %s\n", tmp, lab, skip_dir(inst_ptr[inst].name) );
 else  /*  name in some strange format, probably an error */
   fprintf(fd, "1 %s : %s\n", name, skip_dir(inst_ptr[inst].name) );
  if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing generics passed as properties\n");


 /* -------- print generics passed as properties */

 tmp=0;
 /* 20080213 use generic_type property to decide if some properties are strings, see later */
 my_strdup(464, &generic_type, get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"generic_type",2));

 while(1)
 {
  c=*s++;
  if(c=='\\') {
    escape=1;
    c=*s++;
  }
  else 
   escape=0;
  space=SPACE(c);
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;
  if(value_pos>=sizeval) {
   sizeval+=CADCHUNKALLOC;
   my_realloc(465, &value,sizeval);
  }
  if(token_pos>=sizetok) {
   sizetok+=CADCHUNKALLOC;
   my_realloc(466, &token,sizetok);
  }
  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) {
    if(c=='"' && !escape) quote=!quote;
    else value[value_pos++]=c;
  }
  else if(state==XENDTOK || state==XSEPARATOR) {
    if(token_pos) {
      token[token_pos]='\0';
      token_pos=0;
    }
  } else if(state==XEND) {
    token_number++;
    value[value_pos]='\0';
    value_pos=0;
 
    if(value[0] != '\0') /* token has a value */
    {
     if(token_number>1)
     {
       if(tmp == 0) {fprintf(fd, "generic map(\n");tmp++;tmp1=0;}
       if(tmp1) fprintf(fd, " ,\n");
   
       /* 20080213  put "" around string type generics! */
       if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
         fprintf(fd, "  %s => \"%s\"", token, value);
       } else {
         fprintf(fd, "  %s => %s", token, value);
       }
       /* /20080213 */
 
       tmp1=1;
     }
    }
    state=XBEGIN;
  }
  if(c=='\0')  /* end string */
  {
   break ;
  }
 }

 /* -------- end print generics passed as properties */
     if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing generic maps \n");

    /* print generic map */
    for(i=0;i<no_of_generics;i++)
    {
      my_strdup(467, &generic_type,get_tok_value(
        (inst_ptr[inst].ptr+instdef)->boxptr[GENERICLAYER][i].prop_ptr,"type",0));
      my_strdup(468, &generic_value,   inst_ptr[inst].node[no_of_pins+i] );
      /*my_strdup(469, &generic_value, get_tok_value( */
      /*  (inst_ptr[inst].ptr+instdef)->boxptr[GENERICLAYER][i].prop_ptr,"value") ); */
      str_ptr = get_tok_value(
        (inst_ptr[inst].ptr+instdef)->boxptr[GENERICLAYER][i].prop_ptr,"name",0);
   if(generic_value) {                  /*03062002 dont print generics if unassigned */
      if(tmp) fprintf(fd, " ,\n");
      if(!tmp) fprintf(fd, "generic map (\n");
      fprintf(fd,"   %s => %s",
                            str_ptr ? str_ptr : "<NULL>",
                            generic_value ? generic_value : "<NULL>"  );
      tmp=1;
   }
 }
 if(tmp) fprintf(fd, "\n)\n");



 
  if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing port maps \n");
 /* print port map */
 fprintf(fd, "port map(\n" );
 tmp=0;
 for(i=0;i<no_of_pins;i++)
 {
   if( (str_ptr =  pin_node(inst,i, &mult, 0)) )
   {
     if(tmp) fprintf(fd, " ,\n");
     fprintf(fd, "   %s => %s",
       get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
       str_ptr);
     tmp=1;
   }
 }
 fprintf(fd, "\n);\n\n");
  if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): ------- end ------ \n");
 
}

void print_generic(FILE *fd, char *ent_or_comp, int symbol)
{
 int tmp;
 register int c, state=XBEGIN, space;
 static char *template=NULL,*format=NULL, *s, *value=NULL,  *token=NULL;
 static char *type=NULL, *generic_type=NULL, *generic_value=NULL;
 char *str_tmp;
 int i, sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 my_strdup(470, &format, get_tok_value(instdef[symbol].prop_ptr,"format",0));
 my_strdup(471, &generic_type, get_tok_value(instdef[symbol].prop_ptr,"generic_type",0));
 my_strdup(472, &template, instdef[symbol].templ); /* 20150409 */
 if( !template || !(template[0]) )  return;
  if(debug_var>=2) fprintf(errfp, "print_generic(): symbol=%d template=%s \n", symbol, template);

 fprintf(fd, "%s %s ",ent_or_comp, skip_dir(instdef[symbol].name));
 if(!strcmp(ent_or_comp,"entity"))
  fprintf(fd, "is\n");
 else
  fprintf(fd, "\n");
 s=template;
 tmp=0;
 while(1)
 {
  c=*s++;
  if(c=='\\')
  {
    escape=1;
    c=*s++;
  }
  else 
   escape=0;
  space=SPACE(c);
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;
  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(473, &value,sizeval);
  }
  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(474, &token,sizetok);
  }
  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
  }
  else if(state==XENDTOK || state==XSEPARATOR) {
    if(token_pos) {
      token[token_pos]='\0';
      token_pos=0;
    }
  } else if(state==XEND)                    /* got a token */
  {
   token_number++;
   value[value_pos]='\0';
   value_pos=0;
   my_strdup(475, &type, get_tok_value(generic_type,token,0));

   if(value[0] != '\0') /* token has a value */
   {
    if(token_number>1)
    {
      if(!tmp) {fprintf(fd, "generic (\n");}
      if(tmp) fprintf(fd, " ;\n");
      if(!type || strcmp(type,"string") ) { /* 20080213 print "" around string values 20080418 check for type==NULL */
        fprintf(fd, "  %s : %s := %s", token, type? type:"integer", value);
      } else {
        fprintf(fd, "  %s : %s := \"%s\"", token, type? type:"integer", value);
      }                                         /* /20080213 */

      tmp=1;
    }
   }
   state=XBEGIN;
  }
  if(c=='\0')  /* end string */
  {
   break ;
  }
 }






  for(i=0;i<instdef[symbol].rects[GENERICLAYER];i++)
  {
    my_strdup(476, &generic_type,get_tok_value(
              instdef[symbol].boxptr[GENERICLAYER][i].prop_ptr,"generic_type",0));
    my_strdup(477, &generic_value, get_tok_value(
              instdef[symbol].boxptr[GENERICLAYER][i].prop_ptr,"value",2) ); /*<< 170402 */
    str_tmp = get_tok_value(instdef[symbol].boxptr[GENERICLAYER][i].prop_ptr,"name",0);
    if(!tmp) fprintf(fd, "generic (\n");
    if(tmp) fprintf(fd, " ;\n");
    fprintf(fd,"  %s : %s",str_tmp ? str_tmp : "<NULL>",
                             generic_type ? generic_type : "<NULL>"  );
    if(generic_value &&generic_value[0])
      fprintf(fd," := %s", generic_value);
    tmp=1;
  }
  if(tmp) fprintf(fd, "\n);\n");
}


void print_verilog_param(FILE *fd, int symbol) /*16112003 */
{
 register int c, state=XBEGIN, space;
 static char *template=NULL, *s, *value=NULL,  *generic_type=NULL, *token=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 /* my_strdup(478, &template, get_tok_value(instdef[symbol].prop_ptr,"template",0)); */
 my_strdup(479, &template, instdef[symbol].templ); /* 20150409 20171103 */
 my_strdup(480, &generic_type, get_tok_value(instdef[symbol].prop_ptr,"generic_type",0));
 if( !template || !(template[0]) )  return;
  if(debug_var>=2) fprintf(errfp, "print_verilog_param(): symbol=%d template=%s \n", symbol, template);

 s=template;
 while(1)
 {
  c=*s++;
  if(c=='\\')
  {
    escape=1;
    c=*s++;
  }
  else 
   escape=0;
  space=SPACE(c);
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(481, &value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(482, &token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
  }
  else if(state==XENDTOK || state==XSEPARATOR) {
    if(token_pos) {
      token[token_pos]='\0';
      token_pos=0;
    }
  } else if(state==XEND)                    /* got a token */
  {
   token_number++;
   value[value_pos]='\0';
   value_pos=0;

   if(value[0] != '\0') /* token has a value */
   {
    if(token_number>1)
    {

      /* 20080915 put "" around string params */
      if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
        fprintf(fd, "  parameter   %s = \"%s\" ;\n", token,  value);
      }
      else  {
        fprintf(fd, "  parameter   %s = %s ;\n", token,  value);
      }
    }
   }
   state=XBEGIN;
  }
  if(c=='\0')  /* end string */
  {
   break ;
  }
 }

}









void print_spice_element(FILE *fd, int inst)
{
 int i=0, mult, tmp;
 const char *str_ptr=NULL; 
 register int c, state=XBEGIN, space;
 static char *template=NULL,*format=NULL,*s, *value=NULL, *name=NULL,  *token=NULL;
 const char *lab;
 int pin_number; /* 20180911 */
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 int quote=0; /* 20171029 */
 /* struct hashentry *ptr; */

 my_strdup(483, &template,
     (inst_ptr[inst].ptr+instdef)->templ); /* 20150409 */

 my_strdup(484, &name,inst_ptr[inst].instname); /* 20161210 */
 /* my_strdup(485, &name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); */

 my_strdup(486, &format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"format",0));
 if((name==NULL) || (format==NULL) ) return; /* do no netlist unwanted insts(no format) */
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 s=format;
 if(debug_var>=1) fprintf(errfp, "print_spice_element: name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

 /* begin parsing format string */
 while(1)
 {
  c=*s++; 
  if(c=='"' && escape) { 
    quote=!quote; /* 20171029 */
  }
  if(c=='"' && !escape ) c=*s++;
  if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
  /* 20150317 use SPACE2() instead of SPACE() */
  space=SPACE2(c);
                              /* 20151028 */
  if( state==XBEGIN && c=='@' && !escape) state=XTOKEN;

  /* 20171029 added !escape, !quote */
  else if( state==XTOKEN && (space || c == '@')  && token_pos > 1 && !escape && !quote) state=XSEPARATOR;

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(487, &token,sizetok);
  }

  if(state==XTOKEN) {
    if(c!='\\' || escape) token[token_pos++]=c; /* 20171029 remove escaping backslashes */
  }
  else if(state==XSEPARATOR)                    /* got a token */
  {
   token[token_pos]='\0'; 
   token_pos=0;

   value = get_tok_value(inst_ptr[inst].prop_ptr, token+1, 2);
   if(value[0] == '\0')
   value=get_tok_value(template, token+1, 0);

   if(value[0]!='\0')
   {  /* instance names (name) and node labels (lab) go thru the expandlabel function. */
      /*if something else must be parsed, put an if here! */

    if(!(strcmp(token+1,"name") && strcmp(token+1,"lab"))  /* expand name/labels */
                && ((lab = expandlabel(value, &tmp)) != NULL) )
      fputs(lab,fd);
    else fputs(value,fd);
   }
   else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                        /* in hash table */
   {
    fputs(skip_dir(inst_ptr[inst].name),fd);
   }
   else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                        /* in hash table */
   {
     /* fputs(schematic[currentsch],fd); */
     fputs(current_name, fd); /* 20190519 */
   }
   else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
   {                                    /* and node number: m1 n1 m2 n2 .... */
    for(i=0;i<no_of_pins;i++)
    {
      str_ptr =  pin_node(inst,i, &mult, 0);
      /* fprintf(errfp, "inst: %s  --> %s\n", name, str_ptr); */
      fprintf(fd, "@%d %s ", mult, str_ptr);
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
    for(i=0;i<no_of_pins;i++) {
     if(!strcmp(
          get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
          token+2
         )
       ) {
       str_ptr =  pin_node(inst,i, &mult, 0);
       fprintf(fd, "@%d %s ", mult, str_ptr);
       break; /* 20171029 */
     }
    }
   }
   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
   else if(token[0]=='@' && token[1]=='#') {
       pin_number = atoi(token+2); 
       if(pin_number < no_of_pins) {
         str_ptr =  pin_node(inst,pin_number, &mult, 0);
         fprintf(fd, "@%d %s ", mult, str_ptr);
       }
   }
   else if(!strcmp(token,"@tcleval")) { /* 20171029 */
     /* char tclcmd[strlen(token)+100]; */
     size_t s;
     char *tclcmd=NULL;
     s = token_pos + strlen(name) + strlen(inst_ptr[inst].name) + 100;
     tclcmd = my_malloc(488, s);
     Tcl_ResetResult(interp);
     my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, inst_ptr[inst].name);
     tcleval(tclcmd);
     fprintf(fd, "%s", Tcl_GetStringResult(interp));
     my_free(&tclcmd);
   } /* /20171029 */
                 /* 20151028 dont print escaping backslashes */
   if(c != '@' && c!='\0' && (c!='\\'  || escape) ) fputc(c,fd);
   if(c == '@') s--;
   state=XBEGIN;
  }
                 /* 20151028 dont print escaping backslashes */
  else if(state==XBEGIN && c!='\0' && (c!='\\' || escape))  fputc(c,fd);
  if(c=='\0') 
  {
   fputc('\n',fd);
   return ;
  }
  if(c=='\\')  escape=1;  else escape=0;

 }
}





void print_tedax_element(FILE *fd, int inst)
{
 int i=0, mult;
 const char *str_ptr=NULL; 
 register int c, state=XBEGIN, space;
 static char *template=NULL,*format=NULL,*s, *value=NULL, *name=NULL, *token=NULL;
 static char *pinname=NULL, *extra=NULL, *extra_pinnumber=NULL;
 static char *pinnumber=NULL;
 static char *numslots=NULL;
 int pin_number; /* 20180911 */
 char *extra_token, *extra_ptr, *extra_token_val;
 char *extra_pinnumber_token, *extra_pinnumber_ptr;
 char *saveptr1, *saveptr2;
 const char *tmp;
 int instance_based=0;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 int quote=0; /* 20171029 */
 /* struct hashentry *ptr; */


 my_strdup(489, &extra, get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"extra",2));
 my_strdup(41, &extra_pinnumber, get_tok_value(inst_ptr[inst].prop_ptr,"extra_pinnumber",2));
 if(!extra_pinnumber) my_strdup(490, &extra_pinnumber, get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"extra_pinnumber",2));
 my_strdup(491, &template,
     (inst_ptr[inst].ptr+instdef)->templ); /* 20150409 */
 my_strdup(492, &numslots, get_tok_value(inst_ptr[inst].prop_ptr,"numslots",2));
 if(!numslots) my_strdup(493, &numslots, get_tok_value(template,"numslots",2));
 if(!numslots) my_strdup(494, &numslots, "1");

 my_strdup(495, &name,inst_ptr[inst].instname); /* 20161210 */
 /* my_strdup(496, &name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); */

 my_strdup(497, &format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"tedax_format",0));
 if(name==NULL) return; 
 if(!format || !format[0]) return; /* do no netlist unwanted insts(no format) */
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];

 fprintf(fd, "begin_inst %s numslots %s\n", name, numslots);
 for(i=0;i<no_of_pins; i++) {
   char pnumber[100], *pname=NULL,*pnumber_ptr;
   my_snprintf(pnumber, S(pnumber), "pinnumber(%d)", i);
   my_strdup2(498, &pinname, get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0));
   pname =my_malloc(49, get_tok_value_size+30);
   my_snprintf(pname, get_tok_value_size+30, "pinnumber(%s)", pinname);
   if(!pinname) my_strdup(499, &pinnumber, "--UNDEF--");
   else {
     pnumber_ptr = get_tok_value(inst_ptr[inst].prop_ptr, pname, 0);
     if(pnumber_ptr[0]) my_strdup(51, &pinnumber, pnumber_ptr);
     else my_strdup(40, &pinnumber, get_tok_value(inst_ptr[inst].prop_ptr, pnumber, 0));
     if(!pinnumber) my_strdup(500, &pinnumber, get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"pinnumber",0));
   }
   if(!pinnumber) my_strdup(501, &pinnumber, "--UNDEF--");
   tmp = pin_node(inst,i, &mult, 0);
   if(tmp && strcmp(tmp, "<UNCONNECTED_PIN>")) {
     fprintf(fd, "conn %s %s %s %s %d\n", name, tmp, pinname, pinnumber, i+1);
   }
   my_free(&pname);
 }

 if(extra){ 
   /* fprintf(errfp, "extra_pinnumber: |%s|\n", extra_pinnumber); */
   /* fprintf(errfp, "extra: |%s|\n", extra); */
   for(extra_ptr = extra, extra_pinnumber_ptr = extra_pinnumber; ; extra_ptr=NULL, extra_pinnumber_ptr=NULL) {
     extra_pinnumber_token=my_strtok_r(extra_pinnumber_ptr, " ", &saveptr1);
     extra_token=my_strtok_r(extra_ptr, " ", &saveptr2);
     if(!extra_token) break;
     /* fprintf(errfp, "extra_pinnumber_token: |%s|\n", extra_pinnumber_token); */
     /* fprintf(errfp, "extra_token: |%s|\n", extra_token); */
     instance_based=0;
     extra_token_val=get_tok_value(inst_ptr[inst].prop_ptr, extra_token, 0);
     if(!extra_token_val[0]) extra_token_val=get_tok_value(template, extra_token, 0);
     else instance_based=1;
     if(!extra_token_val[0]) extra_token_val="--UNDEF--";
     
     fprintf(fd, "conn %s %s %s %s %d", name, extra_token_val, extra_token, extra_pinnumber_token, i+1);
     i++;
     if(instance_based) fprintf(fd, " # instance_based");
     fprintf(fd,"\n");
   }
 }
     
 if(format) {
  s=format;
  if(debug_var>=1) fprintf(errfp, "print_tedax_element: name=%s, tedax_format=%s netlist_count=%d\n",name,format, netlist_count);
  /* begin parsing format string */
  while(1)
  {
   c=*s++; 
   if(c=='"' && escape) { 
     quote=!quote; /* 20171029 */
   }
   if(c=='"' && !escape ) c=*s++;
   if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
   /* 20150317 use SPACE2() instead of SPACE() */
   space=SPACE2(c);
                               /* 20151028 */
   if( state==XBEGIN && c=='@' && !escape) state=XTOKEN;
 
   /* 20171029 added !escape, !quote */
   else if( state==XTOKEN && (space || c == '@')  && token_pos > 1 && !escape && !quote) state=XSEPARATOR;
 
   if(token_pos>=sizetok)
   {
    sizetok+=CADCHUNKALLOC;
    my_realloc(502, &token,sizetok);
   }
  
   if(state==XTOKEN) {
     if(c!='\\' || escape) token[token_pos++]=c; /* 20171029 remove escaping backslashes */
   }
   else if(state==XSEPARATOR)                   /* got a token */
   {
    token[token_pos]='\0'; 
    token_pos=0;
 
    value = get_tok_value(inst_ptr[inst].prop_ptr, token+1, 2);
    if(value[0] == '\0')
    value=get_tok_value(template, token+1, 0);
 
    if(value[0]!='\0')
    {
      fputs(value,fd);
    }
    else if(strcmp(token,"@symname")==0)        /* of course symname must not be present  */
                                        /* in hash table */
    {
     fputs(skip_dir(inst_ptr[inst].name),fd);
    }
    else if(strcmp(token,"@schname")==0)        /* of course schname must not be present  */
                                        /* in hash table */
    {
     /* fputs(schematic[currentsch],fd); */
     fputs(current_name, fd); /* 20190519 */
    }
    else if(strcmp(token,"@pinlist")==0)        /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
    {                                   /* and node number: m1 n1 m2 n2 .... */
     for(i=0;i<no_of_pins;i++)
     {
       str_ptr =  pin_node(inst,i, &mult, 0);
       /* fprintf(errfp, "inst: %s  --> %s\n", name, str_ptr); */
       fprintf(fd, "@%d %s ", mult, str_ptr);
     }
    }
    else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     for(i=0;i<no_of_pins;i++) {
      if(!strcmp(
           get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
           token+2
          )
        ) {
        str_ptr =  pin_node(inst,i, &mult, 0);
        fprintf(fd, "%s", str_ptr);
        break; /* 20171029 */
      }
     }

    /* this allow to print in netlist any properties defined for pins.
     * @#n:property, where 'n' is the pin index (starting from 0) and 
     * 'property' the property defined for that pin (property=value) 
     * in case this property is found the value for it is printed.
     * if device is slotted (U1:m) and property value for pin
     * is also slotted ('a,b,c,d') then print the m-th substring.
     * slot numbers start from 1
     */
    } else if(token[0]=='@' && token[1]=='#') {  /* 20180911 */
      if( strchr(token, ':') )  {

        int n;
        char *subtok = my_malloc(503, sizetok * sizeof(char));
        char *subtok2 = my_malloc(42, sizetok * sizeof(char)+20);
        subtok[0]='\0';
        n=-1;
        sscanf(token+2, "%d:%s", &n, subtok);
        if(n!=-1 && subtok[0]) {
          my_snprintf(subtok2, sizetok * sizeof(char)+20, "%s(%d)", subtok, n);
          value = get_tok_value(inst_ptr[inst].prop_ptr,subtok2,0);
          if( n>=0 && n < (inst_ptr[inst].ptr+instdef)->rects[PINLAYER]) {
            if(!value[0]) value = get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr,subtok,0);
          } 
          if(value[0]) {
            char *ss;
            int slot;
            if( (ss=strchr(inst_ptr[inst].instname, ':')) ) {
              sscanf(ss+1, "%d", &slot);
              value = find_nth(value, ':', slot);
            }
            fprintf(fd, "%s", value);
          }
        }
        my_free(&subtok);
        my_free(&subtok2);
      } else {
        /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
        /* @#n --> return net name attached to pin of index 'n' */
        pin_number = atoi(token+2);
        if(pin_number < no_of_pins) {
          str_ptr =  pin_node(inst,pin_number, &mult, 0);
          fprintf(fd, "%s", str_ptr);
        }
      }
    }
    else if(!strncmp(token,"@tcleval", 8)) { /* 20171029 */
      /* char tclcmd[strlen(token)+100] ; */
      size_t s;
      char *tclcmd=NULL;
      s = token_pos + strlen(name) + strlen(inst_ptr[inst].name) + 100;
      tclcmd = my_malloc(504, s);
      Tcl_ResetResult(interp);
      my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, inst_ptr[inst].name);
      tcleval(tclcmd);
      fprintf(fd, "%s", Tcl_GetStringResult(interp));
      my_free(&tclcmd);
      /* fprintf(errfp, "%s\n", tclcmd); */
    } /* /20171029 */
 
 
                  /* 20151028 dont print escaping backslashes */
    if(c!='@' && c!='\0' && (c!='\\'  || escape) ) fputc(c,fd);
    if(c == '@') s--;
    state=XBEGIN;
   }
                  /* 20151028 dont print escaping backslashes */
   else if(state==XBEGIN && c!='\0' && (c!='\\' || escape))  fputc(c,fd);
   if(c=='\0') 
   {
    fputc('\n',fd);
    break ;
   }
   if(c=='\\')  escape=1;  else escape=0;
 
  }
 } /* if(format) */
 fprintf(fd,"end_inst\n");
}



void print_verilog_element(FILE *fd, int inst)
{
 int i=0, mult, tmp;
 const char *str_ptr;
 const char *lab;
 static char *name=NULL;
 static char  *generic_type=NULL;
 static char *template=NULL,*s;
 int no_of_pins=0;


 int  tmp1;
 register int c, state=XBEGIN, space;
 static char *value=NULL,  *token=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int token_number=0;


 /* 20080915 use generic_type property to decide if some properties are strings, see later */
 my_strdup(505, &generic_type, get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"generic_type",2));
 
 if(get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"verilog_format",2)[0] != '\0') {
  print_verilog_primitive(fd, inst); /*15112003 */
  return;
 }
 my_strdup(506, &template,
     (inst_ptr[inst].ptr+instdef)->templ); /* 20150409 */

 my_strdup(507, &name,inst_ptr[inst].instname); /* 20161210 */
 /* my_strdup(508, &name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); */

 if(name==NULL) return;
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];

 s=inst_ptr[inst].prop_ptr;

/* print instance  subckt */
  if(debug_var>=2) fprintf(errfp, "print_verilog_element(): printing inst name & subcircuit name\n");
   fprintf(fd, "%s\n", skip_dir(inst_ptr[inst].name) );

 /* -------- print generics passed as properties */
 tmp=0;
 while(1)
 {
  c=*s++;
  if(c=='\\')
  {
    c=*s++;
  }
  space=SPACE(c);
  if( (state==XBEGIN || state==XENDTOK) && !space && c != '=') state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( (state==XTOKEN || state==XENDTOK) && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(509, &value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(510, &token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
    value[value_pos++]=c;
  }
  else if(state==XENDTOK || state==XSEPARATOR) {
    if(token_pos) {
      token[token_pos]='\0';
      token_pos=0;
    }
  } else if(state==XEND) 
  {
   token_number++;
   value[value_pos]='\0';
   value_pos=0;

   if(value[0] != '\0') /* token has a value */
   {
    if(token_number>1)
    {
      /* 20080915 put "" around string params */
      if(strcmp(token,"spice_ignore") && strcmp(token,"vhdl_ignore") && strcmp(token,"tedax_ignore")) {
        if(tmp == 0) {fprintf(fd, "#(\n---- start parameters\n");tmp++;tmp1=0;}
        if(tmp1) fprintf(fd, " ,\n");
        if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
          fprintf(fd, "  .%s ( \"%s\" )", token, value);
        } else {
          fprintf(fd, "  .%s ( %s )", token, value);
        }
        tmp1=1;
      }
    }
   }
   state=XBEGIN;
  }
  if(c=='\0')  /* end string */
  {
   break ;
  }
 }
 if(tmp) fprintf(fd, "\n---- end parameters\n)\n");

 /* -------- end print generics passed as properties */

/* print instance name */
 if( (lab = expandlabel(name, &tmp)) != NULL)
   fprintf(fd, "---- instance %s (\n", lab );
 else  /*  name in some strange format, probably an error */
   fprintf(fd, "---- instance %s (\n", name );
 
  if(debug_var>=2) fprintf(errfp, "print_verilog_element(): printing port maps \n");
 /* print port map */
 tmp=0;
 for(i=0;i<no_of_pins;i++)
 {
   if( (str_ptr =  pin_node(inst,i, &mult, 0)) )
   {
     /*printf("print_verilog_element(): expandlabel: str=%s mult=%d\n", str_ptr, mult); */
     if(tmp) fprintf(fd,"\n");
     fprintf(fd, "  @%d %s %s ", mult, 
       get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
       str_ptr);
     tmp=1;
   }
 }
 fprintf(fd, "\n);\n\n");
  if(debug_var>=2) fprintf(errfp, "print_verilog_element(): ------- end ------ \n");
}


const char *pin_node(int i, int j, int *mult, int hash_prefix_unnamed_net)
{
 int tmp;
 char errstr[2048];
 static const char unconn[]="<UNCONNECTED_PIN>";
 char str_node[40]; /* 20161122 overflow safe */
 if(inst_ptr[i].node[j]!=NULL)
 {
  if((inst_ptr[i].node[j])[0] == '#') /* unnamed net */
  {
   /* get unnamed node multiplicity ( minimum mult found in circuit) */
   *mult = get_unnamed_node(3, 0, strtol((inst_ptr[i].node[j])+4, NULL,10) );
    if(debug_var>=2) fprintf(errfp, "pin_node(): node = %s  n=%d mult=%d\n",
     inst_ptr[i].node[j], atoi(inst_ptr[i].node[j]), *mult);
   if(hash_prefix_unnamed_net) {
     if(*mult>1)   /* unnamed is a bus */
      my_snprintf(str_node, S(str_node), "%s[%d:0]", (inst_ptr[i].node[j]), *mult-1);
     else
      my_snprintf(str_node, S(str_node), "%s", (inst_ptr[i].node[j]) );
   } else {
     if(*mult>1)   /* unnamed is a bus */
      my_snprintf(str_node, S(str_node), "%s[%d:0]", (inst_ptr[i].node[j])+1, *mult-1);
     else
      my_snprintf(str_node, S(str_node), "%s", (inst_ptr[i].node[j])+1 );
   }
   expandlabel(get_tok_value(
           (inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][j].prop_ptr,"name",0), mult);
   return expandlabel(str_node, &tmp);
  }
  else
  {
   expandlabel(get_tok_value(
           (inst_ptr[i].ptr+instdef)->boxptr[PINLAYER][j].prop_ptr,"name",0), mult);
   return expandlabel(inst_ptr[i].node[j], &tmp);
  }
 }
 else
 {
   *mult=1;

   my_snprintf(errstr, S(errstr), "Warning: unconnected pin,  Inst idx: %d, Pin idx: %d  Inst:%s\n", i, j, inst_ptr[i].instname ) ;
   statusmsg(errstr,2);
   if(!netlist_count) {
     inst_ptr[i].flags |=4;
     hilight_nets=1;
   }
   return unconn;
 }
}


void print_vhdl_primitive(FILE *fd, int inst) /* netlist  primitives, 20071217 */
{
 int i=0, mult, tmp;
 const char *str_ptr;
 register int c, state=XBEGIN, space;
 const char *lab;
 static char *template=NULL,*format=NULL,*s, *value, *name=NULL, *token=NULL;
 int pin_number; /* 20180911 */
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 int quote=0; /* 20171029 */
 /* struct hashentry *ptr; */

 my_strdup(513, &template,
     /* get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"template",2)); */
     (inst_ptr[inst].ptr+instdef)->templ); /* 20150409 20171103 */
 my_strdup(514, &name, inst_ptr[inst].instname); /* 20161210 */
 /* my_strdup(515, &name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); */

 my_strdup(516, &format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"vhdl_format",0)); /* 20071217 */
 if((name==NULL) || (format==NULL) ) return; /*do no netlist unwanted insts(no format) */
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 s=format;
 if(debug_var>=1) fprintf(errfp, "print_vhdl_primitive: name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

 fprintf(fd, "---- start primitive ");
 lab=expandlabel(name, &tmp);
 fprintf(fd, "%d\n",tmp);
 /* begin parsing format string */
 while(1)
 {
  c=*s++; 
  if(c=='"' && escape) {
    quote=!quote; /* 20171029 */
  }
  if(c=='"' && !escape ) c=*s++;
  if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
  space=SPACE(c);
                               /* 20171029 */
  if( state==XBEGIN && c=='@' && !escape ) state=XTOKEN;
  /* 20171029 added !escape, !quote */
  else if( state==XTOKEN && (space || c=='@') && token_pos > 1 && !escape && !quote) state=XSEPARATOR;

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(517, &token,sizetok);
  }

  if(state==XTOKEN) {
    if(c!='\\' || escape) token[token_pos++]=c; /* 20171029 remove escaping backslashes */
  }
  else if(state==XSEPARATOR)                    /* got a token */
  {
   token[token_pos]='\0'; 
   token_pos=0;
   
   value = get_tok_value(inst_ptr[inst].prop_ptr, token+1, 2);
   if(value[0] == '\0')
   value=get_tok_value(template, token+1, 0);

   if(value[0]!='\0')
   {  /* instance names (name) and node labels (lab) go thru the expandlabel function. */
      /*if something else must be parsed, put an if here! */

    if(!(strcmp(token+1,"name"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL) 
         fprintf(fd, "----name(%s)", lab);
      else 
         fprintf(fd, "%s", value);
    }
    else if(!(strcmp(token+1,"lab"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL) 
         fprintf(fd, "----pin(%s)", lab);
      else 
         fprintf(fd, "%s", value);
    }
    else  fprintf(fd, "%s", value);
   }
   else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                        /* in hash table */
   {
    fprintf( fd, "%s",skip_dir(inst_ptr[inst].name) );
   }
   else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                        /* in hash table */
   {
     /* fputs(schematic[currentsch],fd); */
     fputs(current_name, fd); /* 20190519 */
   }
   else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
   {                                    /* and node number: m1 n1 m2 n2 .... */
    for(i=0;i<no_of_pins;i++)
    {
      str_ptr =  pin_node(inst,i, &mult, 0);
      /*fprintf(fd, "@%d %s ", mult, str_ptr); */ /* 25122004 disabled bus handling, until verilog.awk knows about it */
      fprintf(fd, "----pin(%s) ", str_ptr);
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
    for(i=0;i<no_of_pins;i++) {
     if(!strcmp(
          get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
          token+2
         )
       ) {
       str_ptr =  pin_node(inst,i, &mult, 0);
       /*fprintf(fd, "@%d %s ", mult, str_ptr); */ /* 25122004 disabled bus handling, until verilog.awk knows about it */
       fprintf(fd, "----pin(%s) ", str_ptr);
       break; /* 20171029 */
     }
    }
   }
   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
   else if(token[0]=='@' && token[1]=='#') {
       pin_number = atoi(token+2);
       if(pin_number < no_of_pins) {
         str_ptr =  pin_node(inst,pin_number, &mult, 0);
         fprintf(fd, "----pin(%s) ", str_ptr);
       }
   }
   else if(!strncmp(token,"@tcleval", 8)) { /* 20171029 */
     /* char tclcmd[strlen(token)+100] ; */
     size_t s;
     char *tclcmd=NULL;
     s = token_pos + strlen(name) + strlen(inst_ptr[inst].name) + 100;
     tclcmd = my_malloc(518, s);
     Tcl_ResetResult(interp);
     my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, inst_ptr[inst].name);
     tcleval(tclcmd);
     fprintf(fd, "%s", Tcl_GetStringResult(interp));
     my_free(&tclcmd);
   }

                 /* 20180911 dont print escaping backslashes */
   if(c!='@' && c!='\0' && (c!='\\'  || escape) ) fputc(c,fd);
   if(c == '@') s--;
   state=XBEGIN;
  }
                 /* 20180911 dont print escaping backslashes */
  else if(state==XBEGIN && c!='\0' && (c!='\\' || escape))  fputc(c,fd);

  if(c=='\0') 
  {
   fputc('\n',fd);
   fprintf(fd, "---- end primitive\n");
   return ;
  }
  if(c=='\\')
  {
    escape=1;
   /*c=*s++; */
  }
  else 
   escape=0;

 }
}

void print_verilog_primitive(FILE *fd, int inst) /* netlist switch level primitives, 15112003 */
{
 int i=0, mult, tmp;
 const char *str_ptr;
 register int c, state=XBEGIN, space;
 const char *lab;
 static char *template=NULL,*format=NULL,*s=NULL, *value=NULL, *name=NULL, *token=NULL;
 int pin_number; /* 20180911 */
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 int quote=0; /* 20171029 */
 /* struct hashentry *ptr; */

 my_strdup(519, &template,
     (inst_ptr[inst].ptr+instdef)->templ); /* 20150409 */

 my_strdup(520, &name,inst_ptr[inst].instname); /*20161210 */
 /* my_strdup(521, &name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); */

 my_strdup(522, &format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"verilog_format",0));
 if((name==NULL) || (format==NULL) ) return; /*do no netlist unwanted insts(no format) */
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 s=format;
 if(debug_var>=1) fprintf(errfp, "print_verilog_primitive: name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

 fprintf(fd, "---- start primitive ");
 lab=expandlabel(name, &tmp);
 fprintf(fd, "%d\n",tmp);
 /* begin parsing format string */
 while(1)
 {
  c=*s++; 
  if(c=='"' && escape) {
    quote=!quote; /* 20171029 */
  }
  if(c=='"' && !escape ) c=*s++;
  if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
  space=SPACE(c);                
                                 /*20171029 */
  if( state==XBEGIN && c=='@'  && !escape ) state=XTOKEN;
  /* 20171029 added !escape, !quote */
  else if( state==XTOKEN && (space || c == '@') && token_pos > 1 && !escape && !quote) state=XSEPARATOR;

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(523, &token,sizetok);
  }

  if(state==XTOKEN) {
    if(c!='\\' || escape) token[token_pos++]=c; /* 20171029 remove escaping backslashes */
  }
  else if(state==XSEPARATOR)                    /* got a token */
  {
   token[token_pos]='\0'; 
   token_pos=0;

   value = get_tok_value(inst_ptr[inst].prop_ptr, token+1, 2);
   if(value[0] == '\0')
   value=get_tok_value(template, token+1, 0);

   if(value[0]!='\0')
   {  /* instance names (name) and node labels (lab) go thru the expandlabel function. */
      /*if something else must be parsed, put an if here! */

    if(!(strcmp(token+1,"name"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL) 
         fprintf(fd, "----name(%s)", lab);
      else 
         fprintf(fd, "%s", value);
    }
    else if(!(strcmp(token+1,"lab"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL) 
         fprintf(fd, "----pin(%s)", lab);
      else 
         fprintf(fd, "%s", value);
    }
    else  fprintf(fd, "%s", value);
   }
   else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                        /* in hash table */
   {
    fprintf( fd, "%s",skip_dir(inst_ptr[inst].name) );
   }
   else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                        /* in hash table */
   {
     /* fputs(schematic[currentsch],fd); */
     fputs(current_name, fd); /* 20190519 */
   }
   else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
   {                                    /* and node number: m1 n1 m2 n2 .... */
    for(i=0;i<no_of_pins;i++)
    {
      str_ptr =  pin_node(inst,i, &mult, 0);
      /*fprintf(fd, "@%d %s ", mult, str_ptr); */ /* 25122004 disabled bus handling, until verilog.awk knows about it */
      fprintf(fd, "----pin(%s) ", str_ptr);
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
    for(i=0;i<no_of_pins;i++) {
     if(!strcmp(
          get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
          token+2
         )
       ) {
       str_ptr =  pin_node(inst,i, &mult, 0);
       /*fprintf(fd, "@%d %s ", mult, str_ptr); */ /* 25122004 disabled bus handling, until verilog.awk knows about it */
       fprintf(fd, "----pin(%s) ", str_ptr);
       break; /* 20171029 */
     }
    }
   }
   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
   else if(token[0]=='@' && token[1]=='#') {
       pin_number = atoi(token+2);
       if(pin_number < no_of_pins) {
         str_ptr =  pin_node(inst,pin_number, &mult, 0);
         fprintf(fd, "----pin(%s) ", str_ptr);
       }
   }
   else if(!strncmp(token,"@tcleval", 8)) { /* 20171029 */
     /* char tclcmd[strlen(token)+100] ; */
     size_t s;
     char *tclcmd=NULL;
     s = token_pos + strlen(name) + strlen(inst_ptr[inst].name) + 100;
     tclcmd = my_malloc(524, s);
     Tcl_ResetResult(interp);
     my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, inst_ptr[inst].name);
     tcleval(tclcmd);
     fprintf(fd, "%s", Tcl_GetStringResult(interp));
     my_free(&tclcmd);
   }
                 /* 20180911 dont print escaping backslashes */
   if(c!='@' && c!='\0' && (c!='\\'  || escape) ) fputc(c,fd);
   if(c == '@') s--;
   state=XBEGIN;
  }
                 /* 20180911 dont print escaping backslashes */
  else if(state==XBEGIN && c!='\0' && (c!='\\' || escape))  fputc(c,fd);
  if(c=='\0') 
  {
   fputc('\n',fd);
   fprintf(fd, "---- end primitive\n");
   return ;
  }
  if(c=='\\')
  {
    escape=1;
   /*c=*s++; */
  }
  else 
   escape=0;

 }
}


/* 20180911 */
char *find_nth(char *str, char sep, int n)
{
  static char *ss=NULL;
  static char empty[]="";
  int i;
  char *ptr;
  int count;

  my_strdup(525, &ss, str);
  if(!ss) return empty;
  for(i=0, count=1, ptr=ss; ss[i] != 0; i++) {
    if(ss[i]==sep) {
      ss[i]=0;
      if(count==n) {
        return ptr;
      }
      ptr=ss+i+1;
      count++;
    }
  }
  if(count==n) return ptr;
  else return empty;
}

/* substitute given tokens in a string with their corresponding values */
/* ex.: name=@name w=@w l=@l ---> name=m112 w=3e-6 l=0.8e-6 */
char *translate(int inst, char* s)
{
 static char *result=NULL;
 int size=0, tmp;
 register int c, state=XBEGIN, space;
 static char *token=NULL;
 const char *tmp_sym_name;
 int sizetok=0;
 int result_pos=0, token_pos=0;
 /* struct hashentry *ptr; */
 struct stat time_buf;
 struct tm *tm;
 char file_name[PATH_MAX];
 static char *value; /* 20100401 */
 int escape=0; /* 20161210 */
 
 size=CADCHUNKALLOC;
 if(result==NULL) result=my_malloc(526, size);
 else my_realloc(527, &result,size);

  if(debug_var>=2) fprintf(errfp, "translate(): substituting props in <%s>, instance <%s>\n",
        s?s:"NULL",inst_ptr[inst].instname?inst_ptr[inst].instname:"NULL");

 while(1)
 {
  c=*s++; 
  
  /* 20161210 */
  if(c=='\\')
  {
    escape=1;
    c=*s++;
  }
  else
   escape=0;
  /* /20161210 */

  space=SPACE3(c); /* 20150418 use SPACE3 */
  if( state==XBEGIN && c=='@' && !escape  ) state=XTOKEN; /* 20161210 escape */
  else if( state==XTOKEN && (space || c == '@') && token_pos > 1 ) state=XSEPARATOR;

  if(result_pos>=size)
  {
   size+=CADCHUNKALLOC;
   my_realloc(528, &result,size);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(529, &token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XSEPARATOR) 
  {
   token[token_pos]='\0'; 
   token_pos=0;

   value = get_tok_value(inst_ptr[inst].prop_ptr, token+1, 2); /* 20171205 use get_tok_value instead of hash_lookup */
   if(!value[0]) value=get_tok_value((inst_ptr[inst].ptr+instdef)->templ, token+1, 2); /* 20190310 2 instead of 0 */

   if(value[0] != 0) {
    tmp=get_tok_value_size;  /* strlen(value); */  /* 20180926 */
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(530, &result,size);
    }
    memcpy(result+result_pos, value, tmp+1); /* 20180923 */
    result_pos+=tmp;
   } else if(strcmp(token,"@symname")==0) {
    tmp_sym_name=get_cell_w_ext(inst_ptr[inst].name, 0);
    tmp=strlen(tmp_sym_name);
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(531, &result,size);
    }
    memcpy(result+result_pos,tmp_sym_name, tmp+1); /* 20180923 */
    result_pos+=tmp;
   } else if(token[0]=='@' && token[1]=='#') {  /* 20180911 */
     int n;
     char *subtok = my_malloc(532, sizetok * sizeof(char));
     char *subtok4 = my_malloc(55, sizetok * sizeof(char));
     char *subtok2 = my_malloc(43, sizetok * sizeof(char)+20);
     char *subtok3=NULL, *pinname;

     subtok4[0]='\0';
     subtok[0]='\0';
     n=-1;
     sscanf(token+2, "%[^:]:%[^:]", subtok4, subtok);
     if(subtok4[0]) {
       for(n = 0 ; n < (inst_ptr[inst].ptr+instdef)->rects[PINLAYER]; n++) {
         if(!strcmp(get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr,"name",0), subtok4)) break;
       }
     }

     if( n==-1  || n>= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER])  {
       sscanf(token+2, "%d:%s", &n, subtok);
     }
     if(n>=0  && subtok[0] && n < (inst_ptr[inst].ptr+instdef)->rects[PINLAYER]) {
       pinname = get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr,"name",0);
       subtok3 = my_malloc(52, 100+sizetok+get_tok_value_size);
       my_snprintf(subtok3, 100+sizetok+get_tok_value_size, "%s(%s)", subtok, pinname);
       my_snprintf(subtok2, sizetok * sizeof(char)+20, "%s(%d)", subtok, n);
       value = get_tok_value(inst_ptr[inst].prop_ptr,subtok3,0);
       if(!value[0]) value = get_tok_value(inst_ptr[inst].prop_ptr,subtok2,0);
       if(!value[0]) value = get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][n].prop_ptr,subtok,0);
       if(value[0] != 0) {
         char *ss;
         int slot;
         if( (ss=strchr(inst_ptr[inst].instname, ':')) ) {
           sscanf(ss+1, "%d", &slot);
           value = find_nth(value, ':', slot);
         }
         tmp=strlen(value);
         if(result_pos + tmp>=size) {
           size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
           my_realloc(533, &result,size);
         }
         memcpy(result+result_pos, value, tmp+1); /* 20180923 */
         result_pos+=tmp;
       }
     }
     my_free(&subtok);
     my_free(&subtok2);
     my_free(&subtok3);
     my_free(&subtok4);
   } else if(strcmp(token,"@sch_last_modified")==0) {

    my_strncpy(file_name, abs_sym_path(inst_ptr[inst].name, ""), S(file_name));
    stat(file_name , &time_buf);
    tm=localtime(&(time_buf.st_mtime) );
    tmp=strlen( asctime(tm));
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(534, &result,size);
    }
    memcpy(result+result_pos,asctime(tm), tmp+1); /* 20180923 */
    result_pos+=tmp;
   } else if(strcmp(token,"@sym_last_modified")==0) {
    my_strncpy(file_name, abs_sym_path(inst_ptr[inst].name, ""), S(file_name));
    stat(file_name , &time_buf);
    tm=localtime(&(time_buf.st_mtime) );
    tmp=strlen( asctime(tm));
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(535, &result,size);
    }
    memcpy(result+result_pos,asctime(tm), tmp+1); /* 20180923 */
    result_pos+=tmp;
   } else if(strcmp(token,"@time_last_modified")==0) {
    my_strncpy(file_name, abs_sym_path(schematic[currentsch], ""), S(file_name));
    if(!stat(file_name , &time_buf)) {  /* 20161211 */
      tm=localtime(&(time_buf.st_mtime) );
      tmp=strlen( asctime(tm));
      if(result_pos + tmp>=size) {
       size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
       my_realloc(536, &result,size);
      }
      memcpy(result+result_pos,asctime(tm), tmp+1); /* 20180923 */
      result_pos+=tmp;
    }
   } else if(strcmp(token,"@schname")==0) {
     /* tmp=strlen(schematic[currentsch]);*/
     tmp = strlen(current_name); /* 20190519 */
     if(result_pos + tmp>=size) {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(537, &result,size);
     }
     /* memcpy(result+result_pos,schematic[currentsch], tmp+1); */ /* 20180923 */
     memcpy(result+result_pos, current_name, tmp+1); /* 20190519 */
     result_pos+=tmp;
   }
   else if(strcmp(token,"@prop_ptr")==0 && inst_ptr[inst].prop_ptr) {
     tmp=strlen(inst_ptr[inst].prop_ptr);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(538, &result,size);
     }
     memcpy(result+result_pos,inst_ptr[inst].prop_ptr, tmp+1); /* 20180923 */
     result_pos+=tmp;
   }
   else if(strcmp(token,"@schvhdlprop")==0 && schvhdlprop)
   {
     tmp=strlen(schvhdlprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(539, &result,size);
     }
     memcpy(result+result_pos,schvhdlprop, tmp+1); /* 20180923 */
     result_pos+=tmp;
   }
   /* 20100217 */
   else if(strcmp(token,"@schprop")==0 && schprop)
   {
     tmp=strlen(schprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(540, &result,size);
     }
     memcpy(result+result_pos,schprop, tmp+1); /* 20180923 */
     result_pos+=tmp;
   }
   /* /20100217 */

   else if(strcmp(token,"@schtedaxprop")==0 && schtedaxprop)
   {
     tmp=strlen(schtedaxprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(541, &result,size);
     }
     memcpy(result+result_pos,schtedaxprop, tmp+1); /* 20180923 */
     result_pos+=tmp;
   }
   /* /20100217 */

   else if(strcmp(token,"@schverilogprop")==0 && schverilogprop) /*09112003 */
   {
     tmp=strlen(schverilogprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(542, &result,size);
     }
     memcpy(result+result_pos,schverilogprop, tmp+1); /* 20180923 */
     result_pos+=tmp;
   }

   if(c=='@') s--;
   else result[result_pos++]=c;
   state=XBEGIN;
  }
  else if(state==XBEGIN) result[result_pos++]=c; 
  if(c=='\0') 
  {
   result[result_pos]='\0';
   return result;
  }
 }
}


unsigned int inthash(unsigned int key) /* 20180926 */
{
  int c2=0x27d4eb2d; /* a prime or an odd constant */
  key = (key ^ 61) ^ (key >> 16);
  key = key + (key << 3);
  key = key ^ (key >> 4);
  key = key * c2;
  key = key ^ (key >> 15);
  return key;
}


/* integer general purpose hash function 20180104 */
struct int_hashentry *int_hash_lookup(struct int_hashentry **table, int token, int remove)
/*    token     remove     ... what ... */
/* ----------------------------------------------------------------- */
/* whatever        0       insert in hash table if not in. */
/*                         if already present just return entry  */
/*                         address, NULL otherwise */
/* whatever        2       lookup in hash table,return entry addr. */
/*                         return NULL if not found */
/* whatever        1       delete entry if found,return NULL */

{
 unsigned int index;
 struct int_hashentry *entry, *saveptr, **preventry;
 int s ;

 index=token % INTHASHSIZE;
 /* index=inthash(token) % INTHASHSIZE; */ /* better? not seen any improvement in hash collisions 20180926 */
 entry=table[index];
 preventry=&table[index];
 while(1) {
  if( !entry ) { /* empty slot */
   if(!remove) { /* insert data */
    s=sizeof( struct int_hashentry );
    entry=(struct int_hashentry *) my_malloc(543, s);
    entry->next=NULL;
    entry->token=token;
    *preventry=entry;
    if(debug_var>=1) fprintf(errfp, "int_hash_lookup(): inserting=%d\n", token);
   }
   return NULL; /* not found or was not in table before insert */
  }
  if( entry -> token==token) { /* found matching token */
   if(remove==1) { /* remove token from the hash table ... */
    saveptr=entry->next;
    my_free(&entry);
    *preventry=saveptr;
    return NULL;
   }
   else return entry;   /* found matching entry, return the address */
  }
  preventry=&entry->next; /* descend into the list. */
  entry = entry->next;
 }
}

void free_int_hash(struct int_hashentry **table) /* remove the whole hash table  */
{
 int i;
 struct int_hashentry *entry, *ptr;

 max_collisions=0;
 for(i=0;i<INTHASHSIZE;i++) {
  collisions=0;
  entry=table[i];
  while(entry) {
    collisions++;
    if(collisions > max_collisions) max_collisions=collisions;
    ptr = entry->next;
    my_free(&entry);
    entry=ptr;
  }
  table[i]=NULL;
 }
 if(debug_var>=1) fprintf(errfp, "free_int_hash(): max_collisions=%d\n", max_collisions);
}

