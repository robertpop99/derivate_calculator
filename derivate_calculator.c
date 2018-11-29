#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

struct tree
{
  char expr[100];
  struct tree *l; //left son
  struct tree *r; //right son
  struct tree *f; //father
};
typedef struct tree tree;

//checks if a character is an operator and, if so, returns its precedence
int operator(char c)
{
  switch(c)
  {
    case '+' : return 2;
    case '-' : return 2;
    case '*' : return 3;
    case '/' : return 3;
    case '^' : return 4;
  }
  return 0;
}

//given a string, returns number of arguments if it is a known function
int function(char *c)
{
  if( *c == '-') return function(c + 1);
  if( !strcmp(c,"ln") ) return 1;
  if( !strcmp(c,"log") ) return 2;
  if( !strcmp(c,"lg") ) return 1;
  if( !strcmp(c,"sin") ) return 1;
  if( !strcmp(c,"cos") ) return 1;
  if( !strcmp(c,"tg") ) return 1;
  if( !strcmp(c,"ctg") ) return 1;
  if( !strcmp(c,"arcsin") ) return 1;
  if( !strcmp(c,"arccos") ) return 1;
  if( !strcmp(c,"arctg") ) return 1;
  if( !strcmp(c,"arcctg") ) return 1;
  if( !strcmp(c,"sqrt") ) return 1;
  if( !strcmp(c,"abs") ) return 1;
  return 0;
}

int value(char *c)
{
  if( !strcmp(c,"e") ) return 1;
  if( !strcmp(c,"pi") ) return 1;
  if( !strcmp(c,"-e") ) return 1;
  if( !strcmp(c,"-pi") ) return 1;
  return 0;
}

//push a number to the output queue
void number(char *exp, char *res, int *i, int *j)
{
  *(res + *j) = *(exp + *i);
  *i += 1; *j += 1;
  while( isdigit(*(exp + *i)) || *(exp + *i) == '.' || *(exp + *i) == '-')
  {
    *(res + *j) = *(exp + *i);
    *i += 1; *j += 1;
  }
  *(res + *j) = ' ';
  *j += 1;
  *(res + *j) = '\0';
}

//push a varaible (x) to the output queue
void variable(char *exp, char *res, int *i, int *j)
{
  if(*(exp + *i) == '-')
  {
    *(res + *j) = *(exp + *i);
    *j += 1; *i += 1;
    *(res + *j) = *(exp + *i);
    *j += 1; *i += 1;
  }
  else
  {
    *(res + *j) = *(exp + *i);
    *j += 1; *i += 1;
  }
  *(res + *j) = ' ';
  *j += 1;
  *(res + *j) = '\0';
}

//returns the last element in the stack
void last(char *stack, int poz, char *l)
{
  int k = poz - 2, i = 0;
  while(k != 0 && stack[k - 1] != ' ')
    k--;
  while( stack[k] != ' ' )
    {
      l[i] = stack[k];
      i++; k++;
    }
  l[i] = '\0';
}

//push a symbol to the stack ( operator or bracket)
void symbol_push(char *exp, char *stack, int *i, int *poz)
{
  *(stack + *poz) = *(exp + *i);
  *poz += 1; *i += 1;
  *(stack + *poz) = ' ';
  *poz += 1;
  *(stack + *poz) = '\0';
}

//
void operator_case(char *exp, char *res, char *stack, int *i, int *j, int *poz)
{
  if( *poz == 0 )
  {
    symbol_push(exp, stack, i, poz);
    return;
  }
  char l[50];
  last(stack, *poz, l);
  while( (function(l) || operator(l[0]) > operator(*(exp + *i))
          || (operator(l[0]) == operator(*(exp + *i)) && operator(l[0]) != 4) )
          && l[0] != '(' && *poz != 0)
  {
    strcat(res,l);
    *j += strlen(l);
    *(res + *j) = ' ';
    *j += 1;
    *(res + *j) = '\0';
    *poz -= strlen(l) + 1;
    if(*poz != 0) last(stack, *poz, l);
  }
  symbol_push(exp, stack, i, poz);
}

int is_function(char *exp, int i)
{
  int k = 0;
  char l[50];
  while( k < 50 && (isalpha(*(exp + i)) || *(exp + i) == '-') )
  {
    l[k] = *(exp + i);
    i++; k++;
  }
  l[k] = '\0';
  return function(l);
}

int is_value(char *exp, int i)
{
  int k = 0;
  char l[50];
  while( k < 50 && (isalpha(*(exp + i)) || *(exp + i) == '-') )
  {
    l[k] = *(exp + i);
    i++; k++;
  }
  l[k] = '\0';
  return value(l);
}

void function_case(char *exp, char *stack, int *i, int *poz)
{
  while(isalpha(*(exp + *i)) || *(exp + *i) == '-')
  {
    *(stack + *poz) = *(exp + *i);
    *poz += 1; *i += 1;
  }
  *(stack + *poz) = ' ';
  *poz += 1;
  *(stack + *poz) = '\0';
}

void value_case(char *exp, char *res, int *i, int *j)
{
  while(isalpha(*(exp + *i)))
  {
    *(res + *j) = *(exp + *i);
    *j += 1; *i += 1;
  }
  *(res + *j) = ' ';
  *j += 1;
  *(res + *j) = '\0';
}

int rbracket_case(char *exp, char *res, char*stack, int *i, int *j, int *poz)
{
  char l[50];
  last(stack, *poz, l);
  while(l[0] != '(')
  {
    strcat(res,l);
    *j += strlen(l);
    *(res + *j) = ' ';
    *j += 1;
    *(res + *j) = '\0';
    *poz -= strlen(l) + 1;
    if(*poz == 0) return -1;
    last(stack, *poz, l);
  }
  *poz -= 2;
  *i += 1;
  return 0;
}

int push_all(char *res, char*stack, int *j, int *poz)
{
  char l[50];
  while(*poz != 0)
  {
    last(stack, *poz, l);
    if(l[0] == '(' || l[0] == ')') return -1;
    strcat(res,l);
    *j += strlen(l);
    *(res + *j) = '\0';
    *(res + *j) = ' ';
    *j += 1;
    *(res + *j) = '\0';
    *poz -= strlen(l) + 1;
  }
  *(res + *j - 1) = '\0';
  return 0;
}

//converts a given string from infix notation to rpn notation
int rpn_convertor(char *exp, char *res)
{
  int i = 0, j = 0, poz = 0;
  char stack[1000]; stack[0] = '\0';
  while(*(exp + i) != '\0')
  {
    if( isdigit(*(exp + i)) || (*(exp + i) == '-' && isdigit(*(exp + i + 1))) )
        number(exp, res, &i, &j);
    else if( *(exp + i) == 'x' || (*(exp + i) == '-' && *(exp + i + 1) == 'x'))
        variable(exp, res, &i, &j);
    else if( is_value(exp, i) )
          value_case(exp, res, &i, &j);
    else if( is_function(exp, i) )
        function_case(exp, stack, &i, &poz);
    else if( operator(*(exp + i)) )
        operator_case(exp, res, stack, &i, &j, &poz);
    else if( *(exp + i) == '(')
        symbol_push(exp, stack, &i, &poz);
    else if( *(exp + i) == ')')
    {
      int c = rbracket_case(exp, res, stack, &i, &j, &poz);
      if( c == -1) return -1;
    }
    else i++;
  }
  return push_all(res, stack, &j, &poz);
}

//puts in l the element before the specified position
void get_elem(char *exp, int poz, char *l)
{
  int k = poz - 1, i = 0;
  while(k != 0 && exp[k - 1] != ' ')
    k--;
  while( k <= poz - 1 )
    {
      l[i] = exp[k];
      i++; k++;
    }
  l[i] = '\0';
}

//taxes in a string in rpn notation and transforms it into a tree
//poz points at the end of the last element + 1
tree *tree_parser(char *exp, int *poz, tree *f)
{
  tree *t = malloc(sizeof(tree));
  t->f = f;
  get_elem(exp, *poz, t->expr);
  *poz -= strlen(t->expr) + 1;
  if( isdigit(t->expr[0]) || t->expr[0] == 'x' || is_value(t->expr, 0)
      || (t->expr[0] == '-' && (isdigit(t->expr[1]) || t->expr[1] == 'x')) )
    {
      t->l = NULL;
      t->r = NULL;
    }
  else if(function(t->expr))
  {
    t->l = tree_parser(exp, poz, t);
    if( function(t->expr) == 2 ) t->r = tree_parser(exp, poz, t);
    else t->r = NULL;
  }
  else if(operator(t->expr[0]))
  {
    t->l = tree_parser(exp, poz, t);
    t->r = tree_parser(exp, poz, t);
  }
  return t;
}

void tree_to_infix(char *, int *, tree *);

//checks if conditions are met for puting brackets
int condition_for_brackets(tree *t, tree *son)
{
  if( isdigit(son->expr[1]) ) return 0;
  //if( son->expr[1] == 'x') return 0;
  if(operator(t->expr[0]) && operator(son->expr[0]) &&
     operator(t->expr[0]) > operator(son->expr[0])) return 1;
  if(function(t->expr) && operator(son->expr[0])) return 1;
  if( t->l == son &&  operator(t->expr[0]) && operator(t->expr[0]) != 4 &&
    operator(t->expr[0]) == operator(son->expr[0]) ) return 1;
  if( t->r == son && operator(t->expr[0]) == 4 &&
    operator(t->expr[0]) == operator(son->expr[0]) ) return 1;
  return 0;
}
//adds brackests if neded, when converting a tree to infix notation
void brackets_for_branch(char *res, int *poz, tree *t, tree *son)
{
  if( condition_for_brackets(t, son) )
  {
    *(res + *poz) = '('; *poz += 1;
    *(res + *poz) = '\0';
    *(res + *poz) = ' '; *poz += 1;
    *(res + *poz) = '\0';

    tree_to_infix(res, poz, son);

    *(res + *poz) = ')'; *poz += 1;
    *(res + *poz) = '\0';
    *(res + *poz) = ' '; *poz += 1;
    *(res + *poz) = '\0';
  }
  else
    tree_to_infix(res, poz, son);
}

//writes a function with 2 arguments in infix notation
void doubleArgF(char *res, int *poz, tree *t)
{
  strcat(res,t->expr);
  *poz += strlen(t->expr);
  strcat(res," ( "); *poz += 3;

  if(t->r != NULL)
    tree_to_infix(res, poz, t->r);

  strcat(res,", "); *poz += 2;

  if(t->l != NULL)
    tree_to_infix(res, poz, t->l);
  strcat(res,") "); *poz += 2;
}

//coverts a tree to a string in infix notation
//poz points at the end of the last element + 1
void tree_to_infix(char *res, int *poz, tree *t)
{
  if(t->f == NULL)
        *res = '\0';
  if(!strcmp(t->expr,"log"))
      doubleArgF(res, poz, t);
  else
    {
      if(t->r != NULL)
        brackets_for_branch(res, poz, t, t->r);

      strcat(res,t->expr);
      *poz += strlen(t->expr);
      *(res + *poz) = ' ';
      *poz += 1;
      *(res + *poz) = '\0';

      if(t->l != NULL)
        brackets_for_branch(res, poz, t, t->l);
    }
  if(t->f == NULL)
    *(res + *poz - 1) = '\0';
}

//deallocates a tree
void free_tree(tree *t)
{
  if(t->l != NULL)
    free_tree(t->l);
  if(t->r != NULL)
    free_tree(t->r);
  free(t);
}

//makes a copy of a tree
tree *tree_copy(tree *t)
{
  tree *ct = malloc(sizeof(tree));
  strcpy(ct->expr,t->expr);
  ct->f = NULL;
  if(t->l != NULL)
  {
    ct->l = tree_copy(t->l);
    ct->l->f = ct;
  }
  else
    ct->l = NULL;
  if(t->r != NULL)
  {
    ct->r = tree_copy(t->r);
    ct->r->f = ct;
  }
  else ct->r = NULL;
  return ct;
}

//puts a double number to a string, removing unnecessary '0'
void put_number(char *exp, double d)
{
  char s[100] = "";
  sprintf(s,"%f",d);
  int i = strlen(s) - 1;
  while(s[i] == '0')
  {
    s[i] = '\0';
    i--;
  }
  if(s[i] == '.') s[i] = '\0';
  strcpy(exp,s);
}

//derives x ^ c
void derive_pow(tree *t)
{
  double p = atof(t->l->expr);
  if(p == 1 || p == 0)
  {
    free(t->l); t->l = NULL;
    free(t->r); t->r = NULL;
    if(p == 1) strcpy(t->expr,"1");
    else strcpy(t->expr,"0");
  }
  else
  {
    tree *t2 = malloc(sizeof(tree));
    tree *t3 = malloc(sizeof(tree));
    strcpy(t2->expr,t->expr);
    t2->f = t; t2->l = t->l; t2->r = t->r;
    t2->l->f = t2; t2->r->f = t2;
    strcpy(t->expr,"*");
    t->l = t2; t->r = t3;
    put_number(t3->expr, p);
    t3->f = t; t3->l = NULL; t3->r = NULL;
    put_number(t2->l->expr, p - 1);
  }
}

//derives a constant
void derive_const(tree *t)
{
  strcpy(t->expr,"0");
}

//derives sqrt x
void derive_sqrt(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  strcpy(t1->expr,"1"); strcpy(t2->expr,"*"); strcpy(t3->expr,"2");
  strcpy(t4->expr,"sqrt");
  t->r = t1; t1->f = t; t1->l = NULL; t1->r = NULL;
  t4->f = t2; t4->l = t->l; t4->r = NULL;
  t->l = t2; t2->f = t; t2->l = t4; t2->r = t3;
  t3->f = t2; t3->l = NULL; t3->r = NULL;
}

//derives c ^ x
void derive_exponential(tree *t)
{
  if( strcmp(t->r->expr,"e") )
  {
    strcpy(t->expr,"*");
    tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
    tree *t3 = tree_copy(t->r);
    strcpy(t1->expr,"^"); strcpy(t2->expr,"ln");
    t1->f = t, t1->l = t->l; t1->r = t->r; t->r = t1;
    t2->f = t; t2->l = t3; t2->r = NULL; t->l = t2;
    t3->f = t2;
  }
}

//derive ln x
void derive_ln(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree));
  strcpy(t1->expr,"1");
  t->r = t1; t1->f = t; t1->l = NULL; t1->r = NULL;
}

//derive sin x
void derive_sin(tree *t)
{
  strcpy(t->expr,"cos");
}

//derive cos x
void derive_cos(tree *t)
{
  strcpy(t->expr,"-sin");
}

//derive tg x
void derive_tg(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  strcpy(t1->expr,"1"); strcpy(t2->expr,"^");
  strcpy(t3->expr,"cos"); strcpy(t4->expr,"2");
  t1->f = t; t1->r = NULL; t1->l = NULL; t->r = t1;
  t3->f = t2; t3->l = t->l; t3->r = NULL; t->l = t2;
  t2->f = t; t2->r = t3; t2->l = t4;
  t4->f = t2; t4->l = NULL; t4->r = NULL;
}

//derive ctg x
void derive_ctg(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  strcpy(t1->expr,"-1"); strcpy(t2->expr,"^");
  strcpy(t3->expr,"sin"); strcpy(t4->expr,"2");
  t1->f = t; t1->r = NULL; t1->l = NULL; t->r = t1;
  t3->f = t2; t3->l = t->l; t3->r = NULL; t->l = t2;
  t2->f = t; t2->r = t3; t2->l = t4;
  t4->f = t2; t4->l = NULL; t4->r = NULL;
}

//derive arcsin x
void derive_arcsin(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  tree *t5 = malloc(sizeof(tree)), *t6 = malloc(sizeof(tree));
  strcpy(t1->expr,"1"); strcpy(t2->expr,"sqrt"); strcpy(t3->expr,"-");
  strcpy(t4->expr,"1"); strcpy(t5->expr,"^"); strcpy(t6->expr,"2");
  t1->l = NULL; t1->r = NULL; t1->f = t; t->r = t1;
  t2->f = t; t2->r = NULL; t2->l = t3;
  t3->f = t2; t3->r = t4; t3->l = t5;
  t4->f = t3; t4->r = NULL; t4->l = NULL;
  t5->f = t3; t5->r = t->l; t5->l = t6; t->l = t2;
  t6->f = t5; t6->l = NULL; t6->r = NULL;
}

//derive arccos x
void derive_arccos(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  tree *t5 = malloc(sizeof(tree)), *t6 = malloc(sizeof(tree));
  strcpy(t1->expr,"-1"); strcpy(t2->expr,"sqrt"); strcpy(t3->expr,"-");
  strcpy(t4->expr,"1"); strcpy(t5->expr,"^"); strcpy(t6->expr,"2");
  t1->l = NULL; t1->r = NULL; t1->f = t; t->r = t1;
  t2->f = t; t2->r = NULL; t2->l = t3;
  t3->f = t2; t3->r = t4; t3->l = t5;
  t4->f = t3; t4->r = NULL; t4->l = NULL;
  t5->f = t3; t5->r = t->l; t5->l = t6; t->l = t2;
  t6->f = t5; t6->l = NULL; t6->r = NULL;
}

//derive arctg x
void derive_arctg(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  tree *t5 = malloc(sizeof(tree));
  strcpy(t1->expr,"1"); strcpy(t2->expr,"+");
  strcpy(t3->expr,"1"); strcpy(t4->expr,"^"); strcpy(t5->expr,"2");
  t1->f = t; t1->l = NULL; t1->r = NULL; t->r = t1;
  t2->f = t; t2->r = t3; t2->l = t4;
  t3->f = t2; t3->l = NULL; t3->r = NULL;
  t4->f = t2; t4->r = t->l; t4->l = t5; t->l = t2;
  t5->f = t4; t5->l = NULL; t5->r = NULL;
}

//derive arccrg x
void derive_arcctg(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  tree *t5 = malloc(sizeof(tree));
  strcpy(t1->expr,"-1"); strcpy(t2->expr,"+");
  strcpy(t3->expr,"1"); strcpy(t4->expr,"^"); strcpy(t5->expr,"2");
  t1->f = t; t1->l = NULL; t1->r = NULL; t->r = t1;
  t2->f = t; t2->r = t3; t2->l = t4;
  t3->f = t2; t3->l = NULL; t3->r = NULL;
  t4->f = t2; t4->r = t->l; t4->l = t5; t->l = t2;
  t5->f = t4; t5->l = NULL; t5->r = NULL;
}

//derive abs x
void derive_abs(tree *t)
{
  tree *t1 = tree_copy(t);
  strcpy(t->expr,"/");
  t->r = t->l; t->l = t1; t1->f = t;
}

//derive log (n , x)
void derive_log(tree *t)
{
  if(!strcmp(t->expr,"lg"))
  {
    tree *t4 = malloc(sizeof(tree));
    strcpy(t4->expr,"10");
    t4->f = t; t4->l = NULL; t4->r = NULL; t->r = t4;
  }
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree));
  strcpy(t->expr,"/"); strcpy(t1->expr,"1");
  strcpy(t2->expr,"*"); strcpy(t3->expr,"ln");
  t1->f = t; t1->l = NULL; t1->r = NULL;
  t2->f = t; t2->l = t3; t2->r = t->l;
  t3->f = t2; t3->l = t->r; t3->r = NULL;
  t->r = t1; t->l = t2;
}

//derive 1 / x
void derive_oneoverx(tree *t)
{
  strcpy(t->r->expr,"-1");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  strcpy(t1->expr,"^"); strcpy(t2->expr,"2");
  t1->f = t; t1->l = t2; t1->r = t->l; t->l = t1;
  t2->f = t1; t2->l = NULL; t2->r = NULL;
}

//---------------------------------------------------------------------------
//testing

//tests the convertion from infix no rpn
void test_rpn_convertor()
{
  char a[50] = "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3", b[50];
  rpn_convertor(a, b);
  assert( !strcmp(b,"3 4 2 * 1 5 - 2 3 ^ ^ / +") );
  strcpy(a, "3 / 4 / 5 * 6"); rpn_convertor(a, b);
  assert( !strcmp(b,"3 4 / 5 / 6 *") );
  strcpy(a, "-3 ^ 4 * 5 - 6"); rpn_convertor(a, b);
  assert( !strcmp(b,"-3 4 ^ 5 * 6 -") );
  strcpy(a, "3 ^ (4 * 5 - 6)"); rpn_convertor(a, b);
  assert( !strcmp(b,"3 4 5 * 6 - ^") );
  strcpy(a, "sin (x + tg (x / 2) + e ^ 4 - pi ^ 2)"); rpn_convertor(a, b);
  assert( !strcmp(b,"x x 2 / tg + e 4 ^ + pi 2 ^ - sin") );
  strcpy(a, "cos x / 2"); rpn_convertor(a, b);
  assert( !strcmp(b,"x cos 2 /") );
  strcpy(a, "ln (log (2 , 3) + lg 7)"); rpn_convertor(a, b);
  assert( !strcmp(b,"2 3 log 7 lg + ln") );
  strcpy(a, "sqrt( x ^ 2 / 4)"); rpn_convertor(a, b);
  assert( !strcmp(b,"x 2 ^ 4 / sqrt") );
}

//tests the convertion from tree to infix
void test_tree_to_infix()
{
  char a[50] = "3 / 4 / 5 * 6", b[50], c[50];
  int p, q;
  rpn_convertor(a, b); p  = strlen(b);
  tree *t = tree_parser(b, &p, NULL);
  q = 0; tree_to_infix(c, &q , t);
  assert( !strcmp(a,c)); free_tree(t);
  strcpy(a, "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3"); rpn_convertor(a, b);
  p  = strlen(b); t = tree_parser(b, &p, NULL); q = 0;
  tree_to_infix(c, &q , t); assert( !strcmp(a,c)); free_tree(t); memset(c,0,50);
  strcpy(a, "sin ( x + tg ( x / 2 ) - e ^ -4 + pi ^ 2 )"); rpn_convertor(a, b);
  p  = strlen(b); t = tree_parser(b, &p, NULL); q = 0;
  tree_to_infix(c, &q , t); assert( !strcmp(a,c)); free_tree(t); memset(c,0,50);
  strcpy(a, "log ( 3.14 , ln x * lg 14 )"); rpn_convertor(a, b);
  p  = strlen(b); t = tree_parser(b, &p, NULL); q = 0;
  tree_to_infix(c, &q , t); assert( !strcmp(a,c)); free_tree(t); memset(c,0,50);
}

//tests the derivate of constants and x ^ c
void test_derivate_powconst()
{
  char a[50] = "x 450 ^", b[50]; int p, q;
  p = strlen(a);
  tree *t = tree_parser(a, &p, NULL); derive_pow(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  assert( !strcmp(b,"450 * x ^ 449"));
  strcpy(a,"x 2345.345 ^"), p = strlen(a);
  t = tree_parser(a, &p, NULL); derive_pow(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  assert( !strcmp(b,"2345.345 * x ^ 2344.345"));
  strcpy(a,"x 0 ^"), p = strlen(a);
  t = tree_parser(a, &p, NULL); derive_pow(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  assert( !strcmp(b,"0"));
  strcpy(a,"x 1 ^"), p = strlen(a);
  t = tree_parser(a, &p, NULL); derive_pow(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  assert( !strcmp(b,"1"));
  strcpy(a,"2345.345"), p = strlen(a);
  t = tree_parser(a, &p, NULL); derive_const(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  assert( !strcmp(b,"0"));
  strcpy(a,"1244.1243"), p = strlen(a);
  t = tree_parser(a, &p, NULL); derive_const(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  assert( !strcmp(b,"0"));
}

void test()
{ //fix minus, add basic simplifier
  test_rpn_convertor();
  test_tree_to_infix();
  test_derivate_powconst();
  printf("All tests passed!\n");
}

int main()
{
  test();
  char a[50] = "x 5 - lg", b[50]; int p, q;
  //rpn_convertor(a,b); printf("%s\n",b );
  p = strlen(a);
  tree *t = tree_parser(a, &p, NULL); derive_log(t);
  q = 0; tree_to_infix(b, &q , t); free_tree(t);
  printf("%s\n",b );
  return 0;
}
