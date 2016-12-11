/* File: expandlabel.y
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

/* label parser */

%{
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct          // used in expandlabel.y
{
 char *str;             // label name
 int m;                // label multiplicity, number of wires
} Stringptr;


#define YYERROR_VERBOSE 
#define SIGN(x) ( (x) < 0 ? -1: 1 )
#define INITIALIDXSIZE 8

extern Stringptr dest_string; // 20140108

static int idxsize=INITIALIDXSIZE;
extern int yylex();
extern FILE *errfp;
extern void *my_malloc(size_t size);
extern void my_free(void *ptr);
extern void my_realloc(void *ptr,size_t size);
extern void my_strdup(char **dest, char *src);
extern int debug_var;

static void yyerror (const char *s)  /* Called by yyparse on error */
{
  if(debug_var >=3) fprintf(errfp, "yyerror(): yyparse():%s\n", s);
}

static char *expandlabel_strdup(char *src)
{
 char *ptr;
 if(src==NULL || src[0]=='\0') {
   ptr=NULL;
   my_strdup(&ptr,"");
   return ptr;
 }
 else
 {
  ptr=NULL;
  my_strdup(&ptr,src);
  if(debug_var >=3) fprintf(errfp, "expandlabel_strdup: duplicated %lu string %s\n",(unsigned long)ptr,src);
  return ptr;
 }
}

static char *my_strcat2(char *s1, char c, char *s2)
// concatenates s1 and s2, with c in between
{
 int l1=0,l2=0;
 char *res;

 if(s1!=NULL) l1=strlen(s1);
 if(s1!=NULL) l2=strlen(s2);
 res=my_malloc(l1+l2+2); // 2 strings plus spc and '\0'
 *res='\0';
 if(l1) strcpy(res, s1);
 res[l1]=c;
 if(l2) strcpy(res+l1+1, s2);
 return res;
}

static char *my_strmult2(int n, char *s)
// if n==0 returns "\0"
{
 register int i, len;
 register char *pos,*prev;
 char *str;

 if(n==0) return expandlabel_strdup("");
 len=strlen(s);
 prev=s;
 str=my_malloc( (len+1)*n);
 str[0]='\0';
 for(pos=s;pos<=s+len;pos++) {
   if(*pos==',' || pos==s+len) {
     for(i=1;i<=n;i++) {
       strncat(str,prev,pos-prev);
       if(i<n) strcat(str,",");
     }
     if(*pos==',') strcat(str,",");
     prev=pos+1;
   }
 }
 return str;
}

static char *my_strmult(int n, char *s)
// if n==0 returns "\0"
{
 register int i, len;
 register char *pos;
 char *str;

 if(n==0) return expandlabel_strdup("");
 len=strlen(s);
 str=pos=my_malloc( (len+1)*n);
 for(i=1;i<=n;i++)
 {
  strcpy(pos,s);
  pos[len]=',';
  pos+=len+1;
 } 
 *(pos-1)='\0';
 return str;
}

static char *my_strbus(char *s, int *n)
{
 int i,l;
 char *res=NULL;
 static char *tmp=NULL;
 my_realloc(&res, n[0]*(strlen(s)+20));
 my_realloc(&tmp, strlen(s)+30);
 l=0;
 for(i=1;i<n[0];i++)
 {
  sprintf(tmp, "%s[%d],", s, n[i]);
  strcpy(res+l,tmp);
  l+=strlen(tmp);
 }
 sprintf(res+l, "%s[%d]", s, n[i]);
 return res;
}

static void check_idx(int **ptr,int n)
{
 if(n>=idxsize) 
 {
  idxsize*=2;
  if(debug_var >=3)  fprintf(errfp, "check_idx(): reallocating idx array: size=%d\n",idxsize);
  my_realloc(ptr, idxsize*sizeof(int));
 }
}

%}


%union{
int   val;  /* For returning numbers.*/
Stringptr ptr;  /* strings, that is, identifiers */
char * str;
int  *idx;  /* for bus index & bus index ranges */
}

/* BISON Declarations: terminal tokens*/
%token <val> B_NUM
%token <val> B_CAR
%token <val> B_IDXNUM
%token <str> B_NAME
%token <str> B_LINE
/* BISON Declarations: non terminal symbols*/
%type <ptr> list
%type <idx> index

/* operator precedences (bottom = highest)  and associativity  */
%left B_CAR
%left ','
%left ':'
%left '*'

/* Grammar follows */
%%
input:    /* empty string. allows ctrl-D as input */
        | input line
;
line:	  list   	{
                         my_strdup( &(dest_string.str),$1.str); //19102004
                         my_free($1.str); //19102004
                         dest_string.m=$1.m;
			}
;
list:     B_NAME	{ 
			 if(debug_var>=3) fprintf(errfp, "yyparse(): B_NAME (%lu) \n", (unsigned long) $1);
			 $$.str = expandlabel_strdup($1); // 19102004 prima era =$1
                         my_free($1);  //191020004
                         $1=NULL; //191020004
			 $$.m = 1;
			}
	| B_LINE	{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): B_LINE\n");
			 $$.str = expandlabel_strdup($1); // 19102004 prima era =$1
                         my_free($1);  //191020004
                         $1=NULL; //191020004
			 $$.m = 1;
			}
	| list '*' B_NUM{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): list * B_NUM\n");
                         if(debug_var>=3) fprintf(errfp, "yyparse(): |%s| %d \n",$1.str,$3);
			 $$.str=my_strmult2($3,$1.str);
                         if(debug_var>=3) fprintf(errfp, "yyparse(): |%s|\n",$$.str);
			 $$.m = $3 * $1.m;
			 my_free($1.str);
                         $1.str=NULL;  // 19102004
			}
	| B_NUM '*' list{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): B_NUM * list\n");
			 $$.str=my_strmult($1,$3.str);
			 $$.m = $1 * $3.m;
			 my_free($3.str);
                         $3.str=NULL; // 19102004
			}
	| list ',' list	{ 
			 if(debug_var>=3) fprintf(errfp, "yyparse(): list , list\n");
			 $$.str=my_strcat2($1.str, ',', $3.str);
			 $$.m = $1.m + $3.m;
			 my_free($1.str), my_free($3.str);
                         $1.str=NULL; // 19102004
                         $3.str=NULL; // 19102004
			}
	| list B_CAR list
			{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): list B_CAR list\n");
			 $$.str=my_strcat2($1.str, $2, $3.str);
			 $$.m = $1.m + $3.m;
			 my_free($1.str), my_free($3.str);
                         $1.str=NULL; // 19102004
                         $3.str=NULL; // 19102004
			}
	| '(' list ')'	{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): ( list )\n");
			 $$=$2;
			}
	| B_NAME  '[' index  ']' 
			{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): making bus: n=%d\n",$3[0]);
			 $$.str=my_strbus($1,$3); my_free($1); 
                         $1=NULL; //19102004
			 if(debug_var>=3) fprintf(errfp, "yyparse(): done making bus: n=%d\n",$3[0]);
			 $$.m=$3[0];
			 my_free($3); //19102004
			 $3=NULL; //19102004
			 idxsize=INITIALIDXSIZE;
			}
;
index:	  B_IDXNUM ':' B_IDXNUM ':' B_IDXNUM
			{
			 int i;					// 20120229
			 int sign;

                         sign = SIGN($3-$1);
			 $$=my_malloc(INITIALIDXSIZE*sizeof(int));
			 $$[0]=0;
			 if(debug_var>=3) fprintf(errfp, "yyparse(): parsing first idx range\n");
			 for(i=$1;;i+=sign*$5)
			 {
			  check_idx(&$$,++$$[0]);
			  $$[$$[0]]=i;

			  if(sign==1 && i>=$3) break;
			  if(sign==-1 && i<=$3) break;
			 }					// /20120229
			}
	| B_IDXNUM ':' B_IDXNUM
			{
			 int i;
			 $$=my_malloc(INITIALIDXSIZE*sizeof(int));
			 $$[0]=0;
			 if(debug_var>=3) fprintf(errfp, "yyparse(): parsing first idx range\n");
			 for(i=$1;;i+=SIGN($3-$1))
			 {
			  check_idx(&$$,++$$[0]);
			  $$[$$[0]]=i;
			  if(i==$3) break;
			 }
			}
	| B_IDXNUM	{ 
			 if(debug_var>=3) fprintf(errfp, "yyparse(): parsing first idx item\n");
			 $$=my_malloc(INITIALIDXSIZE*sizeof(int));
			 $$[0]=0;
			  check_idx(&$$, ++$$[0]);
			 $$[$$[0]]=$1;
			}
	| index ',' B_IDXNUM ':' B_IDXNUM ':' B_IDXNUM
			{
			 int i;					// 20120229
			 int sign;

			 sign = SIGN($5-$3);
			 if(debug_var>=3) fprintf(errfp, "yyparse(): parsing comma sep idx range\n");
			 for(i=$3;;i+=sign*$7)
			 {
			  check_idx(&$$, ++$$[0]);
			  $$[$$[0]]=i;
			  if(sign==1 && i>=$5) break;
			  if(sign==-1 && i<=$5) break;
			 }					// /20120229
			}
	| index ',' B_IDXNUM ':' B_IDXNUM
			{
			 int i;
			 if(debug_var>=3) fprintf(errfp, "yyparse(): parsing comma sep idx range\n");
			 for(i=$3;;i+=SIGN($5-$3))
			 {
			  check_idx(&$$, ++$$[0]);
			  $$[$$[0]]=i;
 			  if(i==$5) break;
			 }
			}
	| index ',' B_IDXNUM
			{
			 if(debug_var>=3) fprintf(errfp, "yyparse(): parsing comma sep idx list\n");
			  check_idx(&$$, ++$$[0]);
			 $$[$$[0]]=$3;
			}
;
%%

