/* File: token.c
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
#define SPACE(c) ( c=='\n' || c==' ' || c=='\t' || \
                   c=='\0' || c==';' || c=='/' )

// 20150317
#define SPACE2(c) ( SPACE(c) || c=='\'' || c== '"')
#define SPACE3(c) ( SPACE(c) || c=='*' ) // 20150418 used in translate()

enum status {XBEGIN, XTOKEN, XSEPARATOR, XVALUE, XEND, XENDTOK};

struct hashentry {
		  struct hashentry *next;
		  unsigned int hash;
		  char *token;
		  char *name;
		  char *value;
		 };

static struct hashentry *table[HASHSIZE];

// calculate the hash function relative to string s
static unsigned int hash(char *tok, char *name)
{
 register unsigned int h=0;
 while(*tok) {
  h^=*tok++; // 20161221 xor
  h=(h>>5) | (h<<(8*sizeof(unsigned int)-5)); // 20161221 rotate
 }
 while(*name!='\0' && *name!='[') {
  h^=*name++; // 20161221 xor
  h=(h>>5) | (h<<(8*sizeof(unsigned int)-5)); // 20161221 rotate
}
 if(debug_var>=2) fprintf(errfp, "hash(): %d name=%s, tok=%s\n",h%HASHSIZE, name, tok);
 return h;
}

int name_strcmp(char *s, char *d) // compare strings up to '\0' or'['
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

struct hashentry *hash_lookup(char *token, char *name, char *value,int remove)
//    token        value      remove    ... what ...
// --------------------------------------------------------------------------
// "whatever"    "whatever"     0	insert in hash table if not in.
//					if already present just return entry 
//					address, NULL otherwise
// "whatever"       NULL        0	lookup in hash table,return entry addr.
//					return NULL if not found
// "whatever"    "whatever"     1	delete entry if found,return NULL
// "whatever"       NULL        1	delete entry if found,return NULL
{
 unsigned int hashcode, index;
 struct hashentry *entry, *saveptr, **preventry;
 char *ptr;
 int t,n,v,s ;
 
if(name==NULL || token==NULL) return NULL;
 hashcode=hash(token,name); 
 index=hashcode % HASHSIZE; 
 entry=table[index];
 preventry=&table[index];
 if(debug_var>=3) fprintf(errfp, "hash_lookup(): ");
 while(1)
 {
  if(debug_var>=3) fprintf(errfp, "*");
  if( !entry )		// empty slot
  {
   if(value && !remove)		// insert data
   {
    if(debug_var>=3) fprintf(errfp, "hash_lookup(): inserting token <%s>, name <%s> , value <%s>\n",
     token, name, value);
    s=sizeof( struct hashentry );
    t=strlen(token)+1;
    n=strlen(name)+1;
    v=strlen(value)+1;
    ptr= my_malloc(s + t + n + v );
    entry=(struct hashentry *)ptr;
    ptr+=s;
    entry->next=NULL;
    entry->token=(char *)ptr;
    ptr+=t;
    strcpy(entry->token,token);
    entry->name=(char *)ptr;
    ptr+=n;
    strcpy(entry->name,name);
    entry->value=(char *)ptr;
    strcpy(entry->value,value);
    entry->hash=hashcode;
    *preventry=entry;
    return NULL; // <<< if element inserted return NULL since it was not in table
   }
   return entry;
  }
  if( entry -> hash==hashcode && 
      strcmp(token,entry->token)==0 &&
      name_strcmp(name,entry->name)==0 )
			// found a matching token
  {
   if(remove) 		// remove token from the hash table ...
   {
    saveptr=entry->next;
    my_free(entry);
    *preventry=saveptr;
    return NULL;
   }
   else return entry;	// found matching entry, return the address
  } 
  preventry=&entry->next; // descend into the list.
  entry = entry->next;
 }
 if(debug_var>=3) fprintf(errfp, "\n");
}

static  int collisions, max_collisions=0;
static struct hashentry *free_hash_entry(struct hashentry *entry)
{
 if(entry) 
 {
  collisions++;
  entry->next = free_hash_entry( entry->next );
  if(debug_var>=3) fprintf(errfp, "free_hash_entry(): removing entry %lu\n", (unsigned long)entry);
  my_free(entry);
 }
 return NULL;
}

void free_hash(void) // remove the whole hash table 
{
 int i;
  
 if(debug_var>=3) fprintf(errfp, "free_hash(): removing hash table\n");
 for(i=0;i<HASHSIZE;i++)
 {
  collisions=0;
  table[i] = free_hash_entry( table[i] );
  if(collisions>max_collisions) max_collisions=collisions;

 }
 if(debug_var>=1) fprintf(errfp, "# free_hash(): max_collisions=%d\n", max_collisions);
}

// state machine that parses a string made up of <token> <value> ...
// couples and inserts the tokens in the hash table
// the first token must be name=... as it is used for
// indexing information in the table
// if name not found, nothing is inserted in the hash table.
void hash_proplist(char *s,int remove)
{
 register int c, state=XBEGIN, space;
 static char *token=NULL, *value=NULL, *name=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 if(debug_var>=3) fprintf(errfp, "hash_proplist(): parsing %s\n",s?s:"<NULL>");
 if(s==NULL) return;

 sizetok=CADCHUNKALLOC;
 if(token==NULL) token=my_malloc(sizetok);
 else my_realloc(&token,sizetok);

 sizeval=CADCHUNKALLOC;
 if(value==NULL) value=my_malloc(sizeval);
 else my_realloc(&value,sizeval);

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
  space=SPACE(c) ;
  if( state==XBEGIN && !space ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
  }

  else if(state==XEND) 
  {
   token[token_pos]='\0'; 
   token_pos=0;
   token_number++;
   value[value_pos]='\0';
   value_pos=0;
   if(strcmp(token,"name")==0)
   {
     if(debug_var>=1) fprintf(errfp, "hash_proplist(): duplicating 'name' (%s) with \"%s\"\n",name, value);
     my_strdup(&name,value);
     if(debug_var>=1) fprintf(errfp, "hash_proplist(): duplicated 'name' (%s) with \"%s\"\n",name, value);
   }
   else if(token_number==1) return; // if first token is not "name" exit, put nothing into hash 18112002
   if(name!=NULL && name[0] != '\0') 
   {
    //struct hashentry *a;
    if(debug_var>=1) fprintf(errfp, "hash_proplist(): putting %s=%s in hash tbl\n",token,value);
    hash_lookup(token,name,value,remove);
   }
   state=XBEGIN;
  }
  if(c=='\0') break;
 }
}

int match_symbol(char *name)  // never returns -1, if symbol not found load systemlib/missing.sym
{
 int i,found;

 found=0;
 for(i=0;i<lastinstdef;i++)
 {
  if(strcmp(name, instdef[i].name) == 0)
  {
   if(debug_var>=1) fprintf(errfp, "match_symbol(): find matching symbol:%s\n",name);
   found=1;break;
  }
 }
 if(!found)
 {
  if(debug_var>=1) fprintf(errfp, "match_symbol(): matching symbol not found:%s, loading\n",name);
  if(load_symbol_definition(name)==-1) return -1;
 }
 if(debug_var>=1) fprintf(errfp, "match_symbol(): returning %d\n",i);
 return i;
}



// update **s modifying only the token values that are
// different between *new and *old
// return 1 if s modified 20081221
int set_different_token(char **s,char *new, char *old)
{
 register int c, state=XBEGIN, space;
 static char *token=NULL, *value=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int mod;

 mod=0;
 if(debug_var>=1) fprintf(errfp, "set_different_token(): new=%s, old=%s\n", new, old);
 if(debug_var>=3) fprintf(errfp, "set_different_token(): parsing %s\n",new);
 if(new==NULL) return 0;

 sizetok=CADCHUNKALLOC;
 if(token==NULL) token=my_malloc(sizetok);
 else my_realloc(&token,sizetok);

 sizeval=CADCHUNKALLOC;
 if(value==NULL) value=my_malloc(sizeval);
 else my_realloc(&value,sizeval);

 while(1)
 {
  c=*new++; 
  space=SPACE(c) ;
  if( state==XBEGIN && !space ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   //else value[value_pos++]=c;
   value[value_pos++]=c;
   if(c=='\\')
   {
     escape=1;
     //c=*new++;
   }
  else 
   escape=0;
  }

  else if(state==XEND) 
  {
   token[token_pos]='\0'; 
   token_pos=0;
   value[value_pos]='\0';
   value_pos=0;

   if(strcmp(value, get_tok_value(old,token,1)))
   {
    mod=1;
    my_strdup(s, subst_token(*s, token, value) );
   }
   state=XBEGIN;
  }
  if(c=='\0') break;
 }
 return mod;
}


// state machine that parses a string made up of <token>=<value> ...
// couples and returns the value of the given token 
// if s==NULL or no match return empty string
// NULL tok NOT ALLOWED !!!!!!!!
// never returns NULL...
// with_quotes:
// 0: eat non escaped quotes (")
// 1: return unescaped quotes as part of the token value if they are present
// 2: eat backslashes
// 3: 1+2  :)
char *get_tok_value(char *s,char *tok, int with_quotes)
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
    my_realloc(&result,size);
    my_realloc(&token,sizetok);
  }
  if(s==NULL){result[0]='\0'; return result;}
    if(debug_var>=3) fprintf(errfp, "get_tok_value(): looking for <%s> in <%s>\n",tok,s);
  while(1) {
    c=*s++;
    space=SPACE(c) ;
    if( state==XBEGIN && !space ) state=XTOKEN;
    else if( state==XTOKEN && space) state=XEND;
    else if( state==XTOKEN && c=='=') state=XSEPARATOR;
    else if( state==XSEPARATOR && !space) state=XVALUE;
    else if( state==XVALUE && space && !quote) state=XEND;
    if(value_pos>=size) {
      size+=CADCHUNKALLOC;
      my_realloc(&result,size);
    }
    if(token_pos>=sizetok) {
      sizetok+=CADCHUNKALLOC;
      my_realloc(&token,sizetok);
    }
    if(state==XTOKEN) token[token_pos++]=c;
    else if(state==XVALUE) {
      if(c=='"') {
        if(!escape) quote=!quote;
        if((with_quotes & 1) || escape)  result[value_pos++]=c;
      }
      else if( !((c=='\\') && (with_quotes & 2)) ) result[value_pos++]=c; // 20150411 fixed logical expression
      escape = (c=='\\' && !escape);
    }
    else if(state==XEND) {
      token[token_pos]='\0';
      result[value_pos]='\0';
      if( !strcmp(token,tok) ) return result;
      value_pos=0;
      token_pos=0;
      state=XBEGIN;
    }
    if(c=='\0') {
      result[0]='\0';
      return result;
    }
  }
}

// return template string excluding name=... and token=value where token listed in extra
// 20081206
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
// with_quotes:
// 0: eat non escaped quotes (")
// 1: return unescaped quotes as part of the token value if they are present
// 2: eat backslashes
// 3: 1+2  :)

 if(!sizeval) {
   sizeval=CADCHUNKALLOC;
   sizetok=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
   my_realloc(&token,sizetok);
 }
 l = strlen(s);
 if(l >= sizeres) {
   sizeres = l+1;
   my_realloc(&result,sizeres);
 }

 if(s==NULL){result[0]='\0'; return result;}
 while(1)
 {
  c=*s++; 
  space=SPACE(c) ;
  if( state==XBEGIN && !space ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XENDTOK;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
    sizeval+=CADCHUNKALLOC;
    my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
    sizetok+=CADCHUNKALLOC;
    my_realloc(&token,sizetok);
  }

  if(state==XBEGIN) {
    result[result_pos++] = c;
  }
  else if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
    if(c=='"')
    {
     if(!escape) quote=!quote;
     if((with_quotes & 1) || escape)  value[value_pos++]=c;
    }
    else if( (c=='\\') && (with_quotes & 2) )  ;  // dont store backslash
    else value[value_pos++]=c;
    if(c=='\\')
      escape=1;
    else 
     escape=0;
  }
  else if(state==XEND) 
  {
    token[token_pos]='\0'; 
    value[value_pos]='\0';

    if((!extra || !strstr(extra, token)) && strcmp(token,"name"))  {
      strcpy(result+result_pos, token);
      result_pos+=token_pos;
      result[result_pos++] = '=';
      strcpy(result+result_pos, value);
      result_pos+=value_pos;
    }
    result[result_pos++] = c;
    value_pos=0;
    token_pos=0;
    state=XBEGIN;
  }
  else if(state==XENDTOK)
  {
    token[token_pos]='\0';
    if((!extra || !strstr(extra, token)) && strcmp(token,"name"))  {
      strcpy(result+result_pos, token);
      result_pos+=token_pos;
    }
    result[result_pos++] = c;
    token_pos=0;
    state=XBEGIN;
  }
  if(c=='\0')
  {
    return result;
  }
 }
}

char *find_bracket(char *s)
{
 while(*s!='['&& *s!='\0') s++;
 return s;
}

void new_prop_string(char **new_prop,char *old_prop, int fast)
{
// given a old_prop property string, return a new
// property string in new_prop such that the element name is
// unique in current design (that is, element name is changed
// if necessary)
// if old_prop=NULL or "" or does not contain name=... then return NULL 
 static char prefix, *old_name=NULL, *new_name=NULL, *tmp;
 char *tmp2;
 int q,qq;
 static int last[256];
 static int not_zero=0;
 
 if(!fast && not_zero) {for(q=1;q<=255;q++) last[q]=0;not_zero=0;}
 
 if(old_prop==NULL) 
 { 
  if(debug_var>=1) fprintf(errfp, "new_prop_string():-0-  old=%s fast=%d\n", old_prop,fast);
  my_strdup(new_prop,NULL);
  return;
 }
 if(debug_var>=1) fprintf(errfp, "new_prop_string(): new=%s   old=%s\n",*new_prop, old_prop);
 my_strdup(&old_name,get_tok_value(old_prop,"name",0) );
 if(old_name==NULL) 
 { 
  //my_strdup(new_prop,NULL);
  my_strdup(new_prop,old_prop);  // 03102001 changed to copy old props if no name
  return;
 }
 prefix=old_name[0];
 // don't change old_prop if name does not conflict.
 if(hash_lookup("name", old_name, NULL, 0) == NULL)
 {
  my_strdup(new_prop, old_prop);
  if(debug_var>=1) fprintf(errfp, "new_prop_string():-1-  new=%s old=%s fast=%d\n",*new_prop, old_prop,fast);
  return;
 }
 tmp=find_bracket(old_name);
 my_realloc(&new_name, strlen(old_name)+40);
 qq=fast ?  last[(int)prefix] : 1; 
 if(debug_var>=1) fprintf(errfp, "new_prop_string(): -2- new=%s old=%s fast=%d\n",*new_prop, old_prop,fast);
 for(q=qq;;q++)
 {
  sprintf(new_name, "%c%d%s", prefix,q, tmp); // overflow safe 20161122
  if(hash_lookup("name", new_name, NULL, 0) == NULL) 
  {
   if(fast) {last[(int)prefix]=q+1;not_zero=1;}
   break;
  }
 } 


 tmp2 = subst_token(old_prop, "name", new_name); 
 if(strcmp(tmp2, old_prop) ) {
   modified=1;
   if(debug_var>=1) fprintf(errfp, "new_prop_string(): tmp2=%s, old_prop=%s\n", tmp2, old_prop);
   my_strdup(new_prop, tmp2);
 }
}

char *subst_token(char *s,char *tok, char *new_val)
// given a string <s> with multiple "token=value ..." assignments
// substitute <tok>'s value with <new_val>
// if tok not found in s add tok=new_val at end. 04052001
{
 static char *result=NULL;
 int size=0, tmp;
 register int c, state=XBEGIN, space;
 static char *token=NULL;
 int sizetok=0;
 int token_pos=0, result_pos=0;
 int quote=0;
 int done_subst=0;
 int escape=0;

 if(new_val==NULL || new_val[0]=='\0') 
 {
  my_strdup(&result, s);
  return result;
 }
 sizetok=CADCHUNKALLOC;
 if(token==NULL) token=my_malloc(sizetok);
 else my_realloc(&token,sizetok);

 size=CADCHUNKALLOC;
 if(result==NULL) result=my_malloc(size);
 else my_realloc(&result,size);


 if(s==NULL){result[0]='\0';}
 while( s ) {
  c=*s++; 
  space=SPACE(c);
  if( state==XBEGIN && !space ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(result_pos>=size)
  {
   size+=CADCHUNKALLOC;
   my_realloc(&result,size);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&result,sizetok);
  }

  if(state==XTOKEN) 
  {
       token[token_pos++]=c;
       result[result_pos++]=c;
  } 
  else if(state==XSEPARATOR) 
  {
       token[token_pos]='\0'; 
       token_pos=0;
       result[result_pos++]=c;
  }
  else if(state==XVALUE) 
  {
       if(c=='"' && !escape) quote=!quote;
     
       if(c=='\\')
       {
         escape=1;
         //c=*s++;
       }
       else 
        escape=0;
       if(!strcmp(token,tok))
       {
        if(!done_subst)
        {
         tmp=strlen(new_val);
         if(result_pos + tmp>=size)
         {
          size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
          my_realloc(&result,size);
         }
         strcpy(result + result_pos ,new_val);
         result_pos+=tmp;
         done_subst=1;
        }
       }
       else result[result_pos++]=c;
  }
  else if(state==XEND) {
       token[token_pos]='\0'; 
       token_pos=0;
       result[result_pos++]=c;
       state=XBEGIN;
  }
  else if(state==XBEGIN) {
       result[result_pos++]=c;
  }
  if(c=='\0')  break;
 } // end while


 if(!done_subst)  // 04052001 if tok not found add tok=new_value at end
 {
  if(result[0]=='\0') 
  {
   my_realloc(&result,strlen(new_val)+strlen(tok)+2 );
   sprintf( result, "%s=%s", tok, new_val ); // overflow safe 20161122
  }
  else
  {
   my_realloc(&result,result_pos+strlen(new_val)+strlen(tok)+2 );
   sprintf( result, "%s %s=%s", result, tok, new_val ); // overflow safe 20161122
  }
 }
  if(debug_var>=2) fprintf(errfp, "subst_token(): returning: %s\n",result);
 return result;
}

char *skip_dir(char *str)
{
 int c;
 char *ptr=str;
 if(!str) return "(NULL)";
 while((c=*str))
 {
  if(c=='/') ptr=str+1;
  str++;
 }
 return ptr;
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

void print_vhdl_element(FILE *fd, int inst) // 20071217
{
 int i=0, mult, tmp, tmp1;
 char *str_ptr;
 register int c, state=XBEGIN, space;
 static char  *lab=NULL, *name=NULL;
 static char  *generic_value=NULL, *generic_type=NULL, *format=NULL;
 static char *template=NULL,*s, *value=NULL,  *token=NULL;
 int no_of_pins=0, no_of_generics=0;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 if(get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"vhdl_format",2)[0] != '\0') {  // 20071217
  print_vhdl_primitive(fd, inst); //20071217
  return;
 }

 my_strdup(&template,
     // get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"template",2)); // 20150409
     (inst_ptr[inst].ptr+instdef)->templ); // 2015049

 my_strdup(&name,inst_ptr[inst].instname); // 20161210
 // my_strdup(&name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0));

 my_strdup(&format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"format",0));
 if((name==NULL) || (format==NULL) ) return; //<<<<<<<<<<
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 no_of_generics= (inst_ptr[inst].ptr+instdef)->rects[GENERICLAYER];

 s=inst_ptr[inst].prop_ptr;

// print instance name and subckt
  if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing inst name & subcircuit name\n");
 if( (lab = expandlabel(name, &tmp)) != NULL)
   fprintf(fd, "%d %s : %s\n", tmp, lab, skip_dir(inst_ptr[inst].name) );
 else  //  name in some strange format, probably an error <<<
   fprintf(fd, "1 %s : %s\n", name, skip_dir(inst_ptr[inst].name) );
  if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing generics passed as properties\n");


 // -------- print generics passed as properties

 tmp=0;
 // 20080213 use generic_type property to decide if some properties are strings, see later
 my_strdup(&generic_type, get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"generic_type",2));

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
  if( state==XBEGIN && !space) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
  }
  else if(state==XEND) 
  {
   token[token_pos]='\0';
   token_pos=0;
   token_number++;
   value[value_pos]='\0';
   value_pos=0;

   if(value[0] != '\0') // token has a value
   {
    if(token_number>1)
    {
      if(tmp == 0) {fprintf(fd, "generic map(\n");tmp++;tmp1=0;}
      if(tmp1) fprintf(fd, " ,\n");
  
      // 20080213  put "" around string type generics!
      if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
        fprintf(fd, "  %s => \"%s\"", token, value);
      } else {
        fprintf(fd, "  %s => %s", token, value);
      }
      // /20080213

      tmp1=1;
    }
   }
   state=XBEGIN;
  }
  if(c=='\0')  // end string
  {
   break ;
  }
 }

 // -------- end print generics passed as properties
     if(debug_var>=2) fprintf(errfp, "print_vhdl_element(): printing generic maps \n");

    // print generic map
    for(i=0;i<no_of_generics;i++)
    {
      my_strdup(&generic_type,get_tok_value(
        (inst_ptr[inst].ptr+instdef)->boxptr[GENERICLAYER][i].prop_ptr,"type",0));
      my_strdup(&generic_value,   inst_ptr[inst].node[no_of_pins+i] );
      //my_strdup(&generic_value, get_tok_value(
      //  (inst_ptr[inst].ptr+instdef)->boxptr[GENERICLAYER][i].prop_ptr,"value") );
      str_ptr = get_tok_value(
        (inst_ptr[inst].ptr+instdef)->boxptr[GENERICLAYER][i].prop_ptr,"name",0);
   if(generic_value) {			//03062002 dont print generics if unassigned
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
 // print port map
 fprintf(fd, "port map(\n" );
 tmp=0;
 for(i=0;i<no_of_pins;i++)
 {
   if( (str_ptr =  pin_node(inst,i, &mult)) )
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

 my_strdup(&format, get_tok_value(instdef[symbol].prop_ptr,"format",0));
 my_strdup(&generic_type, get_tok_value(instdef[symbol].prop_ptr,"generic_type",0));
 // my_strdup(&template, get_tok_value(instdef[symbol].prop_ptr,"template",2)); // 07042005, was:  , 0)); // 20150409
 my_strdup(&template, instdef[symbol].templ); // 20150409
 if( !template || !(template[0]) )  return; //<<<<<<<<<<
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
  if( state==XBEGIN && !space ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
  }
  else if(state==XEND)                    // got a token
  {
   token[token_pos]='\0';
   token_pos=0;
   token_number++;
   value[value_pos]='\0';
   value_pos=0;
   my_strdup(&type, get_tok_value(generic_type,token,0));

   if(value[0] != '\0') // token has a value
   {
    if(token_number>1)
    {
      if(!tmp) {fprintf(fd, "generic (\n");}
      if(tmp) fprintf(fd, " ;\n");
      if(!type || strcmp(type,"string") ) { // 20080213 print "" around string values 20080418 check for type==NULL, 20081211
        fprintf(fd, "  %s : %s := %s", token, type? type:"integer", value);
      } else {
        fprintf(fd, "  %s : %s := \"%s\"", token, type? type:"integer", value);
      }  					// /20080213

      tmp=1;
    }
   }
   state=XBEGIN;
  }
  if(c=='\0')  // end string
  {
   break ;
  }
 }






  for(i=0;i<instdef[symbol].rects[GENERICLAYER];i++)
  {
    my_strdup(&generic_type,get_tok_value(
              instdef[symbol].boxptr[GENERICLAYER][i].prop_ptr,"generic_type",0));
    my_strdup(&generic_value, get_tok_value(
              instdef[symbol].boxptr[GENERICLAYER][i].prop_ptr,"value",2) ); //<< 170402
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


void print_verilog_param(FILE *fd, int symbol) //16112003
{
 register int c, state=XBEGIN, space;
 static char *template=NULL, *s, *value=NULL,  *generic_type=NULL, *token=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 my_strdup(&template, get_tok_value(instdef[symbol].prop_ptr,"template",0));
 my_strdup(&generic_type, get_tok_value(instdef[symbol].prop_ptr,"generic_type",0));
 if( !template || !(template[0]) )  return; //<<<<<<<<<<
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
  if( state==XBEGIN && !space ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
  }
  else if(state==XEND)                    // got a token
  {
   token[token_pos]='\0';
   token_pos=0;
   token_number++;
   value[value_pos]='\0';
   value_pos=0;

   if(value[0] != '\0') // token has a value
   {
    if(token_number>1)
    {

      // 20080915 put "" around string params
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
  if(c=='\0')  // end string
  {
   break ;
  }
 }

}














void print_spice_element(FILE *fd, int inst)
{
 int i=0, mult, tmp;
 char *str_ptr;
 register int c, state=XBEGIN, space;
 static char *template=NULL,*format=NULL,*s, *value=NULL, *name=NULL, *lab=NULL, *token=NULL;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 struct hashentry *ptr;

 my_strdup(&template,
     // get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"template",2)); // 20150409
     (inst_ptr[inst].ptr+instdef)->templ); // 20150409

 my_strdup(&name,inst_ptr[inst].instname); // 20161210
 // my_strdup(&name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0));

 my_strdup(&format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"format",0));
 if((name==NULL) || (format==NULL) ) return; //<<<<< do no netlist unwanted insts(no format)
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 s=format;
 if(debug_var>=1) fprintf(errfp, "print_spice_element: name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

 // begin parsing format string
 while(1)
 {
  c=*s++; 
  if(c=='"' && !escape) c=*s++;
  // 20150317 use SPACE2() instead of SPACE()
  space=SPACE2(c);
                              // 20151028
  if( state==XBEGIN && c=='@' && !escape) state=XTOKEN;
  else if( state==XTOKEN && space) state=XSEPARATOR;

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XSEPARATOR) 			// got a token
  {
   token[token_pos]='\0'; 
   token_pos=0;
   ptr=hash_lookup(token+1, name, NULL, 0);  // lookup token
   if(ptr!=NULL) 
     value=ptr->value;
   else
     value=get_tok_value(template, token+1,0); // if not found get tok from tmplt

   // alternative way
   //value = get_tok_value(inst_ptr[inst].prop_ptr, token+1);
   //if(value[0] == '\0')
   // value=get_tok_value(template, token+1);

   if(value[0]!='\0')
   {  // instance names (name) and node labels (lab) go thru the expandlabel function.
      //if something else must be parsed, put an if here!

    if(!(strcmp(token+1,"name") && strcmp(token+1,"lab"))  // expand name/labels
                && ((lab = expandlabel(value, &tmp)) != NULL) )
      fputs(lab,fd);
    else fputs(value,fd);
   }
   else if(strcmp(token,"@symname")==0)	// of course symname must not be present 
					// in hash table
   {
    fputs(skip_dir(inst_ptr[inst].name),fd);
   }
   else if(strcmp(token,"@schname")==0)	// of course schname must not be present 
					// in hash table
   {
    fputs(schematic[currentsch],fd);
   }
   else if(strcmp(token,"@pinlist")==0)	// of course pinlist must not be present 
					// in hash table. print multiplicity
   {					// and node number: m1 n1 m2 n2 ....
    for(i=0;i<no_of_pins;i++)
    {
      str_ptr =  pin_node(inst,i, &mult);
      fprintf(fd, "@%d %s ", mult, str_ptr);
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    // recognize single pins 15112003
    for(i=0;i<no_of_pins;i++) {
     if(!strcmp(
          get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
          token+2
         )
       ) {
       str_ptr =  pin_node(inst,i, &mult);
       fprintf(fd, "@%d %s ", mult, str_ptr);
     }
    }
   }


                 // 20151028 dont print escaping backslashes
   if(c!='\0' && (c!='\\'  || escape) ) fputc(c,fd);
   state=XBEGIN;
  }
                 // 20151028 dont print escaping backslashes
  else if(state==XBEGIN && c!='\0' && (c!='\\' || escape))  fputc(c,fd);
  if(c=='\0') 
  {
   fputc('\n',fd);
   return ;
  }
  if(c=='\\')  escape=1;  else escape=0;

 }
}



void print_verilog_element(FILE *fd, int inst)
{
 int i=0, mult, tmp;
 char *str_ptr;
 static char  *lab=NULL, *name=NULL;
 static char  *format=NULL, *generic_type=NULL;
 static char *template=NULL,*s;
 int no_of_pins=0;


 int  tmp1;
 register int c, state=XBEGIN, space;
 static char *value=NULL,  *token=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int token_number=0;


 // 20080915 use generic_type property to decide if some properties are strings, see later
 my_strdup(&generic_type, get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"generic_type",2));
 
 if(get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"verilog_format",2)[0] != '\0') {
  print_verilog_primitive(fd, inst); //15112003
  return;
 }
 my_strdup(&template,
     // get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"template",2)); // 20150409
     (inst_ptr[inst].ptr+instdef)->templ); // 20150409

 my_strdup(&name,inst_ptr[inst].instname); // 20161210
 // my_strdup(&name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0));

 my_strdup(&format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"format",0));
 if((name==NULL) || (format==NULL) ) return; //<<<<<<<<<<
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];

 s=inst_ptr[inst].prop_ptr;

// print instance  subckt
  if(debug_var>=2) fprintf(errfp, "print_verilog_element(): printing inst name & subcircuit name\n");
   fprintf(fd, "%s\n", skip_dir(inst_ptr[inst].name) );








 // -------- print generics passed as properties

 tmp=0;
 while(1)
 {
  c=*s++;
  if(c=='\\')
  {
    c=*s++;
  }
  /* else */ // removed 20121123
  space=SPACE(c);
  if( state==XBEGIN && !space) state=XTOKEN;
  else if( state==XTOKEN && space) state=XEND;
  else if( state==XTOKEN && c=='=') state=XSEPARATOR;
  else if( state==XSEPARATOR && !space) state=XVALUE;
  else if( state==XVALUE && space && !quote) state=XEND;

  if(value_pos>=sizeval)
  {
   sizeval+=CADCHUNKALLOC;
   my_realloc(&value,sizeval);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XVALUE) 
  {
   // if(c=='"' && !escape) quote=!quote;  // 20060908
   // else value[value_pos++]=c; 	   // 20060908
    value[value_pos++]=c;
  }
  else if(state==XEND) 
  {
   token[token_pos]='\0';
   token_pos=0;
   token_number++;
   value[value_pos]='\0';
   value_pos=0;

   if(value[0] != '\0') // token has a value
   {
    if(token_number>1)
    {
      if(tmp == 0) {fprintf(fd, "#(\n---- start parameters\n");tmp++;tmp1=0;}
      if(tmp1) fprintf(fd, " ,\n");
      // 20080915 put "" around string params
      if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
        fprintf(fd, "  .%s ( \"%s\" )", token, value);
      } else {
        fprintf(fd, "  .%s ( %s )", token, value);
      }
      tmp1=1;
    }
   }
   state=XBEGIN;
  }
  if(c=='\0')  // end string
  {
   break ;
  }
 }
 if(tmp) fprintf(fd, "\n---- end parameters\n)\n");

 // -------- end print generics passed as properties

// print instance name
 if( (lab = expandlabel(name, &tmp)) != NULL)
   fprintf(fd, "---- instance %s (\n", lab );
 else  //  name in some strange format, probably an error <<<
   fprintf(fd, "---- instance %s (\n", name );






 
  if(debug_var>=2) fprintf(errfp, "print_verilog_element(): printing port maps \n");
 // print port map
 tmp=0;
 for(i=0;i<no_of_pins;i++)
 {
   if( (str_ptr =  pin_node(inst,i, &mult)) )
   {
     //printf("print_verilog_element(): expandlabel: str=%s mult=%d\n", str_ptr, mult);
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


char *pin_node(int i, int j, int *mult)
{
 int tmp;
 char errstr[2048];
 static char *name=NULL;
 char str_node[40]; // 20161122 overflow safe
 if(inst_ptr[i].node[j]!=NULL)
 {
  if((inst_ptr[i].node[j])[0] == '#') // unnamed net
  {
   // get unnamed node multiplicity ( minimum mult found in circuit)
   *mult = get_unnamed_node(3, 0, strtol((inst_ptr[i].node[j])+4, NULL,10) );
    if(debug_var>=2) fprintf(errfp, "pin_node(): node = %s  n=%d mult=%d\n",
     inst_ptr[i].node[j], atoi(inst_ptr[i].node[j]), *mult);
   if(*mult>1)   // unnamed is a bus
    my_snprintf(str_node, S(str_node), "%s[%d:0]", (inst_ptr[i].node[j])+1, *mult-1);
   else
    my_snprintf(str_node, S(str_node), "%s", (inst_ptr[i].node[j])+1 );
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
   *mult=0;

   my_strdup(&name, inst_ptr[i].instname); // 20161210
   // my_strdup(&name , get_tok_value(inst_ptr[i].prop_ptr,"name",0) );

   snprintf(errstr, S(errstr), "error: unconnected pin,  inst %d -- %s\n", i,  name ) ;
   statusmsg(errstr,2);
   if(!netlist_count) {
     inst_ptr[i].flags |=4;
     hilight_nets=1;
   }
   return NULL;
 }
}


void print_vhdl_primitive(FILE *fd, int inst) // netlist  primitives, 20071217
{
 int i=0, mult, tmp;
 char *str_ptr;
 register int c, state=XBEGIN, space;
 static char *template=NULL,*format=NULL,*s, *value, *name=NULL, *lab=NULL, *token=NULL;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 struct hashentry *ptr;

 my_strdup(&template,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"template",2));

 my_strdup(&name, inst_ptr[inst].instname); // 20161210
 // my_strdup(&name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0));

 my_strdup(&format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"vhdl_format",0)); // 20071217
 if((name==NULL) || (format==NULL) ) return; //<<<<< do no netlist unwanted insts(no format)
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 s=format;
 if(debug_var>=1) fprintf(errfp, "print_vhdl_primitive: name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

 fprintf(fd, "---- start primitive ");
 lab=expandlabel(name, &tmp);
 fprintf(fd, "%d\n",tmp);
 // begin parsing format string
 while(1)
 {
  c=*s++; 
  if(c=='"' && !escape) c=*s++;
  space=SPACE(c);
  if( state==XBEGIN && c=='@' ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XSEPARATOR;

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XSEPARATOR) 			// got a token
  {
   token[token_pos]='\0'; 
   token_pos=0;
   ptr=hash_lookup(token+1, name, NULL, 0);  // lookup token
   if(ptr!=NULL) 
     value=ptr->value;
   else
     value=get_tok_value(template, token+1,0); // if not found get tok from tmplt

   // alternative way
   //value = get_tok_value(inst_ptr[inst].prop_ptr, token+1);
   //if(value[0] == '\0')
   // value=get_tok_value(template, token+1);

   if(value[0]!='\0')
   {  // instance names (name) and node labels (lab) go thru the expandlabel function.
      //if something else must be parsed, put an if here!

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
   else if(strcmp(token,"@symname")==0)	// of course symname must not be present 
					// in hash table
   {
    fprintf( fd, "%s",skip_dir(inst_ptr[inst].name) );
   }
   else if(strcmp(token,"@schname")==0)	// of course schname must not be present 
					// in hash table
   {
    fputs(schematic[currentsch],fd);
   }
   else if(strcmp(token,"@pinlist")==0)	// of course pinlist must not be present 
					// in hash table. print multiplicity
   {					// and node number: m1 n1 m2 n2 ....
    for(i=0;i<no_of_pins;i++)
    {
      str_ptr =  pin_node(inst,i, &mult);
      //fprintf(fd, "@%d %s ", mult, str_ptr); // 25122004 disabled bus handling, until verilog.awk knows about it
      fprintf(fd, "----pin(%s) ", str_ptr);
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    // recognize single pins 15112003
    for(i=0;i<no_of_pins;i++) {
     if(!strcmp(
          get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
          token+2
         )
       ) {
       str_ptr =  pin_node(inst,i, &mult);
       //fprintf(fd, "@%d %s ", mult, str_ptr); // 25122004 disabled bus handling, until verilog.awk knows about it
       fprintf(fd, "----pin(%s) ", str_ptr);
     }
    }
   }


   if(c!='\0') fputc(c,fd);
   state=XBEGIN;
  }
  else if(state==XBEGIN && c!='\0')  fputc(c,fd);
  if(c=='\0') 
  {
   fputc('\n',fd);
   fprintf(fd, "---- end primitive\n");
   return ;
  }
  if(c=='\\')
  {
    escape=1;
   //c=*s++;
  }
  else 
   escape=0;

 }
}

void print_verilog_primitive(FILE *fd, int inst) // netlist switch level primitives, 15112003
{
 int i=0, mult, tmp;
 char *str_ptr;
 register int c, state=XBEGIN, space;
 static char *template=NULL,*format=NULL,*s=NULL, *value=NULL, *name=NULL, *lab=NULL, *token=NULL;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 struct hashentry *ptr;

 my_strdup(&template,
     // get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"template",2)); // 20150409
     (inst_ptr[inst].ptr+instdef)->templ); // 20150409

 my_strdup(&name,inst_ptr[inst].instname); //20161210
 // my_strdup(&name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0));

 my_strdup(&format,
     get_tok_value((inst_ptr[inst].ptr+instdef)->prop_ptr,"verilog_format",0));
 if((name==NULL) || (format==NULL) ) return; //<<<<< do no netlist unwanted insts(no format)
 no_of_pins= (inst_ptr[inst].ptr+instdef)->rects[PINLAYER];
 s=format;
 if(debug_var>=1) fprintf(errfp, "print_verilog_primitive: name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

 fprintf(fd, "---- start primitive ");
 lab=expandlabel(name, &tmp);
 fprintf(fd, "%d\n",tmp);
 // begin parsing format string
 while(1)
 {
  c=*s++; 
  if(c=='"' && !escape) c=*s++;
  space=SPACE(c);
  if( state==XBEGIN && c=='@' ) state=XTOKEN;
  else if( state==XTOKEN && space) state=XSEPARATOR;

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XSEPARATOR) 			// got a token
  {
   token[token_pos]='\0'; 
   token_pos=0;
   ptr=hash_lookup(token+1, name, NULL, 0);  // lookup token
   if(ptr!=NULL) 
     value=ptr->value;
   else
     value=get_tok_value(template, token+1,0); // if not found get tok from tmplt

   // alternative way
   //value = get_tok_value(inst_ptr[inst].prop_ptr, token+1);
   //if(value[0] == '\0')
   // value=get_tok_value(template, token+1);

   if(value[0]!='\0')
   {  // instance names (name) and node labels (lab) go thru the expandlabel function.
      //if something else must be parsed, put an if here!

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
   else if(strcmp(token,"@symname")==0)	// of course symname must not be present 
					// in hash table
   {
    fprintf( fd, "%s",skip_dir(inst_ptr[inst].name) );
   }
   else if(strcmp(token,"@schname")==0)	// of course schname must not be present 
					// in hash table
   {
    fputs(schematic[currentsch],fd);
   }
   else if(strcmp(token,"@pinlist")==0)	// of course pinlist must not be present 
					// in hash table. print multiplicity
   {					// and node number: m1 n1 m2 n2 ....
    for(i=0;i<no_of_pins;i++)
    {
      str_ptr =  pin_node(inst,i, &mult);
      //fprintf(fd, "@%d %s ", mult, str_ptr); // 25122004 disabled bus handling, until verilog.awk knows about it
      fprintf(fd, "----pin(%s) ", str_ptr);
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    // recognize single pins 15112003
    for(i=0;i<no_of_pins;i++) {
     if(!strcmp(
          get_tok_value((inst_ptr[inst].ptr+instdef)->boxptr[PINLAYER][i].prop_ptr,"name",0),
          token+2
         )
       ) {
       str_ptr =  pin_node(inst,i, &mult);
       //fprintf(fd, "@%d %s ", mult, str_ptr); // 25122004 disabled bus handling, until verilog.awk knows about it
       fprintf(fd, "----pin(%s) ", str_ptr);
     }
    }
   }


   if(c!='\0') fputc(c,fd);
   state=XBEGIN;
  }
  else if(state==XBEGIN && c!='\0')  fputc(c,fd);
  if(c=='\0') 
  {
   fputc('\n',fd);
   fprintf(fd, "---- end primitive\n");
   return ;
  }
  if(c=='\\')
  {
    escape=1;
   //c=*s++;
  }
  else 
   escape=0;

 }
}

// substitute given tokens in a string with their corresponding values
// ex.: name=@name w=@w l=@l ---> name=m112 w=3e-6 l=0.8e-6
char *translate(int inst, char* s)
{
 static char *result=NULL;
 int size=0, tmp;
 register int c, state=XBEGIN, space;
 static char *token=NULL;
 char *tmp_sym_name;
 int sizetok=0;
 int result_pos=0, token_pos=0;
 struct hashentry *ptr;
 struct stat time_buf;
 struct tm *tm;
 char file_name[4096];
 static char *value; // 20100401
 int escape=0; // 20161210
 
 // my_strdup(&name,get_tok_value(inst_ptr[inst].prop_ptr,"name",0)); // 20150409
 size=CADCHUNKALLOC;
 if(result==NULL) result=my_malloc(size);
 else my_realloc(&result,size);

  if(debug_var>=2) fprintf(errfp, "translate(): substituting props in <%s>, instance <%s>\n",
        s?s:"NULL",inst_ptr[inst].instname?inst_ptr[inst].instname:"NULL");

 while(1)
 {
  c=*s++; 
  
  // 20161210
  if(c=='\\')
  {
    escape=1;
    c=*s++;
  }
  else
   escape=0;
  // /20161210

  space=SPACE3(c); // 20150418 use SPACE3
  if( state==XBEGIN && c=='@' && !escape  ) state=XTOKEN; // 20161210 escape
  else if( state==XTOKEN && space) state=XSEPARATOR;

  if(result_pos>=size)
  {
   size+=CADCHUNKALLOC;
   my_realloc(&result,size);
  }

  if(token_pos>=sizetok)
  {
   sizetok+=CADCHUNKALLOC;
   my_realloc(&token,sizetok);
  }

  if(state==XTOKEN) token[token_pos++]=c;
  else if(state==XSEPARATOR) 
  {
   token[token_pos]='\0'; 
   token_pos=0;
   ptr=hash_lookup(token+1, inst_ptr[inst].instname, NULL, 0);

   // 20100401
   if(ptr!=NULL)
     value=ptr->value;
   else {
     value=get_tok_value((inst_ptr[inst].ptr+instdef)->templ, token+1,0); // if not found get tok from tmplt 20150409
   }
   // /20100401


   if(value[0] != 0) {
    tmp=strlen(value);
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(&result,size);
    }
    strcpy(result+result_pos, value);
    result_pos+=tmp;
   } else if(strcmp(token,"@symname")==0) {
    tmp_sym_name=skip_dir(inst_ptr[inst].name);
    tmp=strlen(tmp_sym_name);
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(&result,size);
    }
    strcpy(result+result_pos,tmp_sym_name);
    result_pos+=tmp;
   } else if(strcmp(token,"@sch_last_modified")==0) {
     my_snprintf(file_name, S(file_name), "%s/%s.sch",Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
       inst_ptr[inst].name);
    stat(file_name , &time_buf);
    tm=localtime(&(time_buf.st_mtime) );
    tmp=strlen( asctime(tm));
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(&result,size);
    }
    strcpy(result+result_pos,asctime(tm));
    result_pos+=tmp;
   } else if(strcmp(token,"@sym_last_modified")==0) {
     my_snprintf(file_name, S(file_name), "%s/%s.sym",Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
       inst_ptr[inst].name);
    stat(file_name , &time_buf);
    tm=localtime(&(time_buf.st_mtime) );
    tmp=strlen( asctime(tm));
    if(result_pos + tmp>=size) {
     size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(&result,size);
    }
    strcpy(result+result_pos,asctime(tm));
    result_pos+=tmp;
   } else if(strcmp(token,"@time_last_modified")==0) {
     my_snprintf(file_name, S(file_name), "%s/%s.sch",Tcl_GetVar(interp,"XSCHEM_DESIGN_DIR", TCL_GLOBAL_ONLY),
       schematic[currentsch]);
    if(!stat(file_name , &time_buf)) {  // 20161211
      tm=localtime(&(time_buf.st_mtime) );
      tmp=strlen( asctime(tm));
      if(result_pos + tmp>=size) {
       size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
       my_realloc(&result,size);
      }
      strcpy(result+result_pos,asctime(tm));
      result_pos+=tmp;
    }
   } else if(strcmp(token,"@schname")==0) {
     tmp=strlen(schematic[currentsch]);
     if(result_pos + tmp>=size) {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(&result,size);
     }
     strcpy(result+result_pos,schematic[currentsch]);
     result_pos+=tmp;
   }
   else if(strcmp(token,"@prop_ptr")==0 && inst_ptr[inst].prop_ptr) {
     tmp=strlen(inst_ptr[inst].prop_ptr);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(&result,size);
     }
     strcpy(result+result_pos,inst_ptr[inst].prop_ptr);
     result_pos+=tmp;
   }
   else if(strcmp(token,"@schvhdlprop")==0 && schvhdlprop)
   {
     tmp=strlen(schvhdlprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(&result,size);
     }
     strcpy(result+result_pos,schvhdlprop);
     result_pos+=tmp;
   }
   // 20100217
   else if(strcmp(token,"@schprop")==0 && schprop)
   {
     tmp=strlen(schprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(&result,size);
     }
     strcpy(result+result_pos,schprop);
     result_pos+=tmp;
   }
   // /20100217

   else if(strcmp(token,"@schverilogprop")==0 && schverilogprop) //09112003
   {
     tmp=strlen(schverilogprop);
     if(result_pos + tmp>=size)
     {
      size=(1+(result_pos + tmp) / CADCHUNKALLOC) * CADCHUNKALLOC;
      my_realloc(&result,size);
     }
     strcpy(result+result_pos,schverilogprop);
     result_pos+=tmp;
   }

   result[result_pos++]=c;
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

