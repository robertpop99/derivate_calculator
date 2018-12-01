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
      t->r = NULL;
      t->l = NULL;
    }
  else if(function(t->expr))
  {
    t->r = tree_parser(exp, poz, t);
    if( function(t->expr) == 2 ) t->l = tree_parser(exp, poz, t);
    else t->l = NULL;
  }
  else if(operator(t->expr[0]))
  {
    t->r = tree_parser(exp, poz, t);
    t->l = tree_parser(exp, poz, t);
  }
  return t;
}

void tree_to_infix(char *, int *, tree *);

//checks if conditions are met for puting brackets
int condition_for_brackets(tree *t, tree *son)
{//might need changes
  if( isdigit(son->expr[1]) ) return 0;
  if( son->expr[1] == 'x') return 0;
  if(t->expr[0] == '*' && son->expr[0] == '*') return 0;
  if(t->expr[0] == '+' && son->expr[0] == '+') return 0;
  if(operator(t->expr[0]) && operator(son->expr[0]) &&
     operator(t->expr[0]) > operator(son->expr[0])) return 1;
  if(function(t->expr) && operator(son->expr[0])) return 1;
  if( t->r == son &&  operator(t->expr[0]) && operator(t->expr[0]) != 4 &&
    operator(t->expr[0]) == operator(son->expr[0]) ) return 1;
  if( t->l == son && operator(t->expr[0]) == 4 &&
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

  if(t->l != NULL)
    tree_to_infix(res, poz, t->l);

  strcat(res,", "); *poz += 2;

  if(t->r != NULL)
    tree_to_infix(res, poz, t->r);
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
      if(t->l != NULL)
        brackets_for_branch(res, poz, t, t->l);

      strcat(res,t->expr);
      *poz += strlen(t->expr);
      *(res + *poz) = ' ';
      *poz += 1;
      *(res + *poz) = '\0';

      if(t->r != NULL)
        brackets_for_branch(res, poz, t, t->r);
    }
  if(t->f == NULL)
    *(res + *poz - 1) = '\0';
}

//deallocates a tree
void free_tree(tree *t)
{
  if(t->r != NULL)
    free_tree(t->r);
  if(t->l != NULL)
    free_tree(t->l);
  free(t);
}

//makes a copy of a tree
tree *tree_copy(tree *t)
{
  tree *ct = malloc(sizeof(tree));
  strcpy(ct->expr,t->expr);
  ct->f = NULL;
  if(t->r != NULL)
  {
    ct->r = tree_copy(t->r);
    ct->r->f = ct;
  }
  else
    ct->r = NULL;
  if(t->l != NULL)
  {
    ct->l = tree_copy(t->l);
    ct->l->f = ct;
  }
  else ct->l = NULL;
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
  double p = atof(t->r->expr);
  if(p == 1 || p == 0)
  {
    free(t->r); t->r = NULL;
    free(t->l); t->l = NULL;
    if(p == 1) strcpy(t->expr,"1");
    else strcpy(t->expr,"0");
  }
  else
  {
    tree *t2 = malloc(sizeof(tree));
    tree *t3 = malloc(sizeof(tree));
    strcpy(t2->expr,t->expr);
    t2->f = t; t2->r = t->r; t2->l = t->l;
    t2->r->f = t2; t2->l->f = t2;
    strcpy(t->expr,"*");
    t->r = t2; t->l = t3;
    put_number(t3->expr, p);
    t3->f = t; t3->r = NULL; t3->l = NULL;
    put_number(t2->r->expr, p - 1);
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
  t->l = t1; t1->f = t; t1->r = NULL; t1->l = NULL;
  t4->f = t2; t4->r = t->r; t4->l = NULL;
  t->r = t2; t2->f = t; t2->r = t4; t2->l = t3;
  t3->f = t2; t3->r = NULL; t3->l = NULL;
}

//derives c ^ x
void derive_exponential(tree *t)
{
  if( strcmp(t->l->expr,"e") )
  {
    strcpy(t->expr,"*");
    tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
    tree *t3 = tree_copy(t->l);
    strcpy(t1->expr,"^"); strcpy(t2->expr,"ln");
    t1->f = t, t1->r = t->r; t1->l = t->l; t->l = t1;
    t2->f = t; t2->r = t3; t2->l = NULL; t->r = t2;
    t3->f = t2;
  }
}

//derive ln x
void derive_ln(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree));
  strcpy(t1->expr,"1");
  t->l = t1; t1->f = t; t1->r = NULL; t1->l = NULL;
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
  t1->f = t; t1->l = NULL; t1->r = NULL; t->l = t1;
  t3->f = t2; t3->r = t->r; t3->l = NULL; t->r = t2;
  t2->f = t; t2->l = t3; t2->r = t4;
  t4->f = t2; t4->r = NULL; t4->l = NULL;
}

//derive ctg x
void derive_ctg(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  strcpy(t1->expr,"-1"); strcpy(t2->expr,"^");
  strcpy(t3->expr,"sin"); strcpy(t4->expr,"2");
  t1->f = t; t1->l = NULL; t1->r = NULL; t->l = t1;
  t3->f = t2; t3->r = t->r; t3->l = NULL; t->r = t2;
  t2->f = t; t2->l = t3; t2->r = t4;
  t4->f = t2; t4->r = NULL; t4->l = NULL;
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
  t1->r = NULL; t1->l = NULL; t1->f = t; t->l = t1;
  t2->f = t; t2->l = NULL; t2->r = t3;
  t3->f = t2; t3->l = t4; t3->r = t5;
  t4->f = t3; t4->l = NULL; t4->r = NULL;
  t5->f = t3; t5->l = t->r; t5->r = t6; t->r = t2;
  t6->f = t5; t6->r = NULL; t6->l = NULL;
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
  t1->r = NULL; t1->l = NULL; t1->f = t; t->l = t1;
  t2->f = t; t2->l = NULL; t2->r = t3;
  t3->f = t2; t3->l = t4; t3->r = t5;
  t4->f = t3; t4->l = NULL; t4->r = NULL;
  t5->f = t3; t5->l = t->r; t5->r = t6; t->r = t2;
  t6->f = t5; t6->r = NULL; t6->l = NULL;
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
  t1->f = t; t1->r = NULL; t1->l = NULL; t->l = t1;
  t2->f = t; t2->l = t3; t2->r = t4;
  t3->f = t2; t3->r = NULL; t3->l = NULL;
  t4->f = t2; t4->l = t->r; t4->r = t5; t->r = t2;
  t5->f = t4; t5->r = NULL; t5->l = NULL;
}

//derive arcctg x
void derive_arcctg(tree *t)
{
  strcpy(t->expr,"/");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  tree *t5 = malloc(sizeof(tree));
  strcpy(t1->expr,"-1"); strcpy(t2->expr,"+");
  strcpy(t3->expr,"1"); strcpy(t4->expr,"^"); strcpy(t5->expr,"2");
  t1->f = t; t1->r = NULL; t1->l = NULL; t->l = t1;
  t2->f = t; t2->l = t3; t2->r = t4;
  t3->f = t2; t3->r = NULL; t3->l = NULL;
  t4->f = t2; t4->l = t->r; t4->r = t5; t->r = t2;
  t5->f = t4; t5->r = NULL; t5->l = NULL;
}

//derive abs x
void derive_abs(tree *t)
{
  tree *t1 = tree_copy(t);
  strcpy(t->expr,"/");
  t->l = t->r; t->r = t1; t1->f = t;
}

//derive log (n , x)
void derive_log(tree *t)
{
  if(!strcmp(t->expr,"lg"))
  {
    tree *t4 = malloc(sizeof(tree));
    strcpy(t4->expr,"10");
    t4->f = t; t4->r = NULL; t4->l = NULL; t->l = t4;
  }
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree));
  strcpy(t->expr,"/"); strcpy(t1->expr,"1");
  strcpy(t2->expr,"*"); strcpy(t3->expr,"ln");
  t1->f = t; t1->r = NULL; t1->l = NULL;
  t2->f = t; t2->r = t3; t2->l = t->r;
  t3->f = t2; t3->r = t->l; t3->l = NULL;
  t->l = t1; t->r = t2;
}

//derive 1 / x
void derive_oneoverx(tree *t)
{
  strcpy(t->l->expr,"-1");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  strcpy(t1->expr,"^"); strcpy(t2->expr,"2");
  t1->f = t; t1->r = t2; t1->l = t->r; t->r = t1;
  t2->f = t1; t2->r = NULL; t2->l = NULL;
}

//return the type of expresion contained in a tree, 0 = only numbers, 1 = only
//one x, 2 = both numbers and xs (a function)
int exp_type(tree *t)
{
    if(t == NULL) return -1;
    if(isdigit(t->expr[0]) || (t->expr[0] == '-' && isdigit(t->expr[1]))
        || value(t->expr))
      return 0;
    if(t->expr[0] == 'x')
      return 1;
    if(t->expr[0] == '-' && t->expr[1] == 'x')
    {
      tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
      strcpy(t->expr,"*"); strcpy(t1->expr,"-1"); strcpy(t2->expr,"x");
      t1->f = t; t1->l = NULL; t1->r = NULL; t->l = t1;
      t2->f = t; t2->l = NULL; t2->r = NULL; t->r = t2;
      return 2;
    }
    int r = exp_type(t->l), l = exp_type(t->r);

    if(r == 0 && l == 0) return 0;
    if(r == -1 && l == 0 || r == 0 && l == -1) return 0;
    return 2;
}

//derives a tree full of numbers
void derive_num_tree(tree *t)
{
  if(t->r != NULL) free_tree(t->r);
  if(t->l != NULL) free_tree(t->l);
  t->r = NULL; t->l = NULL;
  derive_const(t);
}

void derive(tree *t);

//derive x
void derive_x(tree *t)
{
  if(t->expr[0] == '-')
    strcpy(t->expr,"-1");
  else
    strcpy(t->expr,"1");
}

//derives f + g
void derive_plus(tree *t)
{
  int l = exp_type(t->l), r = exp_type(t->r);
  if(l == 0 && r == 0)
  {
     derive_num_tree(t);
     return;
  }
  if(l == 0) derive_num_tree(t->l);
  if(r == 0) derive_num_tree(t->r);
  if(l == 1) derive_x(t->l);
  if(r == 1) derive_x(t->r);
  if(l == 2) derive(t->l);
  if(r == 2) derive(t->r);
}

//derive f - g
void derive_minus(tree *t)
{
  if(function(t->expr))
  {
    strcpy(t->expr,t->expr + 1);
    tree *t1 = tree_copy(t), *t2 = malloc(sizeof(tree));
    strcpy(t2->expr,"-1"); strcpy(t->expr,"*");
    t2->f = t; t2->l = NULL; t2->r = NULL; t->l = t2;
    t->r = t1; t1->f = t;
    derive(t1);
    return;
  }
  int l = exp_type(t->l), r = exp_type(t->r);
  if(l == 0 && r == 0)
  {
     derive_num_tree(t);
     return;
  }
  if(l == 0) derive_num_tree(t->l);
  if(r == 0) derive_num_tree(t->r);
  if(l == 1) derive_x(t->l);
  if(r == 1) derive_x(t->r);
  if(l == 2) derive(t->l);
  if(r == 2) derive(t->r);
}

//derive f * g
void derive_times(tree *t)
{
  int l = exp_type(t->l), r = exp_type(t->r);
  if(l == 0 && r == 0){
    derive_num_tree(t); return;
  }
  if(l == 0){
    if(r == 1) derive_x(t->r);
    else derive(t->r); return;
  }
  if(r == 0){
    if(l == 1) derive_x(t->l);
    else derive(t->l); return;
  }
  tree *f = tree_copy(t->l), *g = tree_copy(t->r);
  strcpy(t->expr,"+");
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  strcpy(t1->expr,"*"); strcpy(t2->expr,"*");
  t1->f = t; t1->l = t->l; t1->r = g;
  t2->f = t; t2->l = f; t2->r = t->r;
  t->l->f = t1; t->r->f = t2; t->l = t1; t->r = t2;
  f->f = t1; g->f = t2;
  derive(t1->l); derive(t2->r);
}

//derive f / g
void derive_divide(tree *t)
{
  int l = exp_type(t->l), r = exp_type(t->r);
  if(l == 0 && r == 0){
    derive_num_tree(t); return;}
  // if(l == 0){
  //   if(r == 1) derive_x(t->r);
  //   else derive(t->r); return;}
  if(r == 0){
    if(l == 1) derive_x(t->l);
    else derive(t->l); return;}
  tree *f = tree_copy(t->l), *g1 = tree_copy(t->r), *g2 = tree_copy(t->r);
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  tree *t5 = malloc(sizeof(tree));
  strcpy(t1->expr,"-"); strcpy(t2->expr,"*"); strcpy(t3->expr,"*");
  strcpy(t4->expr,"^"); strcpy(t5->expr,"2");
  t1->f = t; t1->l = t2; t1->r = t3;
  t2->f = t1; t2->l = t->l; t2->r = g1; t->l->f = t2; t->l = t1;
  t3->f = t1; t3->l = f; t3->r = t->r; t->r->f = t3; t->r = t4;
  t4->f = t; t4->l = g2; t4->r = t5;
  t5->f = t4; t5->l = NULL; t5->r = NULL;
  f->f = t3; g1->f = t2; g2->f = t4;
  derive(t2->l); derive(t3->r);
}

//derive sin f
void sin_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_sin(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_sin(f); derive(t->r);
  }
}

//derive cos f
void cos_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_cos(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_cos(f); derive(t->r);
  }
}

//derive tg f
void tg_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_tg(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_tg(f); derive(t->r);
  }
}

//derive ctg f
void ctg_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_ctg(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_ctg(f); derive(t->r);
  }
}

//derive arcsin f
void arcsin_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_arcsin(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_arcsin(f); derive(t->r);
  }
}

//derive arccos f
void arccos_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_arccos(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_arccos(f); derive(t->r);
  }
}

//derive arctg f
void arctg_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_arctg(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_arctg(f); derive(t->r);
  }
}

//derive arcctg f
void arcctg_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_arcctg(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_arcctg(f); derive(t->r);
  }
}

//derive ln f
void ln_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_ln(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_ln(f); derive(t->r);
  }
}

//derive log (n, f)
void log_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_log(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_log(f); derive(t->r);
  }
}

//derive abs x
void abs_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_abs(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_abs(f); derive(t->r);
  }
}

//derive sqrt x
void sqrt_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_sqrt(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_sqrt(f); derive(t->r);
  }
}

//derive lg x
void lg_composition(tree *t)
{
  int r = exp_type(t->r);
  if(r == 0) derive_num_tree(t);
  else if(r == 1) derive_log(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_log(f); derive(t->r);
  }
}

//derive n ^ f
void exponential_composition(tree *t)
{
  int r = exp_type(t->r), l = exp_type(t->l);
  if(r == 0) derive_num_tree(t);
  else if(r == 1 && l == 0) derive_exponential(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->l = f; f->f = t;
    derive_exponential(f); derive(t->r);
  }
}

//derive f ^ n
void power_composition(tree *t)
{
  int l = exp_type(t->l);
  if(l == 0) derive_num_tree(t);
  else if(l == 1) derive_pow(t);
  else
  {
    tree *f = tree_copy(t);
    strcpy(t->expr,"*");
    t->r = t->l;
    t->l = f; f->f = t;
    derive_pow(f); derive(t->r);
  }
}

//derive f ^ g as g * f ^ (g - 1)
void derive_power_composition(tree *t)
{
  tree *f = tree_copy(t->l), *g = tree_copy(t->r);
  tree *t1 = malloc(sizeof(tree)), *t2 = malloc(sizeof(tree));
  tree *t3 = malloc(sizeof(tree)), *t4 = malloc(sizeof(tree));
  strcpy(t->expr,"*"); strcpy(t1->expr,"*"); strcpy(t2->expr,"^");
  strcpy(t3->expr,"-"); strcpy(t4->expr,"1");
  t1->f = t; t1->l = t->r; t1->r = t2; t->r = t->l; t->l = t1;
  t2->f = t1; t2->l = f; t2->r = t3; f->f = t2;
  t3->f = t2; t3->l = g; t3->r = t4; g->f = t3;
  t4->f = t3; t4->l = NULL; t4->r = NULL;
  derive(t->r);
}

//derive f ^ g
void pow_composition(tree *t)
{
  tree *t1 = tree_copy(t), *t2 = tree_copy(t);
  free_tree(t->l); free_tree(t->r);
  strcpy(t->expr,"+");
  t1->f = t; t2->f = t; t->l = t1; t->r = t2;
  derive_power_composition(t1); exponential_composition(t2);
}

//derive n ^ f or f ^ n or f ^ g
void derive_power(tree *t)
{
  int r = exp_type(t->r), l = exp_type(t->l);
  if(l == 0)
    exponential_composition(t);
  else if(r == 0)
    power_composition(t);
  else
    pow_composition(t);
}

//derives a tree, depending on its content
void derive(tree *t)
{
    if(isdigit(t->expr[0]) || (isdigit(t->expr[1]) && t->expr[0] == '-') )
      derive_const(t);
    if(!strcmp(t->expr,"x") || !strcmp(t->expr,"-x")) derive_x(t);
    switch(t->expr[0])
    {
      case '+' : derive_plus(t);break;
      case '-' : derive_minus(t);break;
      case '*' : derive_times(t);break;
      case '/' : derive_divide(t);break;
      case '^' : derive_power(t);break;
    }
    if(!strcmp(t->expr,"sin")) sin_composition(t);
    else if(!strcmp(t->expr,"cos")) cos_composition(t);
    else if(!strcmp(t->expr,"tg")) tg_composition(t);
    else if(!strcmp(t->expr,"ctg")) ctg_composition(t);
    else if(!strcmp(t->expr,"arcsin")) arcsin_composition(t);
    else if(!strcmp(t->expr,"arccos")) arccos_composition(t);
    else if(!strcmp(t->expr,"arctg")) arctg_composition(t);
    else if(!strcmp(t->expr,"arcctg")) arcctg_composition(t);
    else if(!strcmp(t->expr,"ln")) ln_composition(t);
    else if(!strcmp(t->expr,"log")) log_composition(t);
    else if(!strcmp(t->expr,"abs")) abs_composition(t);
    else if(!strcmp(t->expr,"sqrt")) sqrt_composition(t);
    else if(!strcmp(t->expr,"lg")) lg_composition(t);
}

//eliminates neutral elements such as * 1, + 0, ^ 1
//side represents the left or right side where the neutral element is found
void eliminate_neutral(tree **t, int side)
{
  if(!side)
  {
    (*t)->r->f = (*t)->f;
    tree *tc = *t;
    *t = (*t)->r;
    free(tc->l); free(tc);
  }
  else
  {
    (*t)->l->f = (*t)->f;
    tree *tc = *t;
    *t = (*t)->l;
    free(tc->r); free(tc);
  }
}

//traverses a tree and finds neutral elements that can we simplified
void simplify_neutral(tree **t)
{
  if((*t)->l != NULL) simplify_neutral(&(*t)->l);
  if((*t)->r != NULL) simplify_neutral(&(*t)->r);
  if((*t)->expr[0] == '+' || ((*t)->expr[0] == '-' && (*t)->expr[1] == '\0'))
    if(!strcmp((*t)->l->expr,"0")) eliminate_neutral(t,0);
    else if(!strcmp((*t)->r->expr,"0")) eliminate_neutral(t,1);
  if((*t)->expr[0] == '*')
    if(!strcmp((*t)->l->expr,"1")) eliminate_neutral(t,0);
    else if(!strcmp((*t)->r->expr,"1")) eliminate_neutral(t,1);
  if((*t)->expr[0] == '/')
    if(!strcmp((*t)->r->expr,"1")) eliminate_neutral(t,1);
  if((*t)->expr[0] == '^')
    if(!strcmp((*t)->r->expr,"1")) eliminate_neutral(t,1);
}

//simplifies a tree
void simplify(tree **t)
{
    simplify_neutral(t);
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

//derives an expresion and checks if it is the same as the expected result
int test_deriver(char *exp, char *res)
{
    char a[100], b[100];
    int p, q;
    rpn_convertor(exp,a);
    p = strlen(a);
    tree *t = tree_parser(a, &p, NULL); derive(t); simplify(&t);
    q = 0; tree_to_infix(b, &q , t); free_tree(t);printf("%s\n",b );
    return (!strcmp(b,res));
}

//tests derive sin
void test_sin()
{
  char a[100], b[100];
  strcpy(a,"sin x"), strcpy(b,"cos x");
  assert(test_deriver(a,b));
  strcpy(a,"sin (x + 5)"), strcpy(b,"cos ( x + 5 )");
  assert(test_deriver(a,b));
  strcpy(a,"sin cos x"), strcpy(b,"cos cos x * ( -sin x )");
  assert(test_deriver(a,b));
  strcpy(a,"sin x + 5"), strcpy(b,"cos x");
  assert(test_deriver(a,b));
  strcpy(a,"sin x ^ 5"), strcpy(b,"5 * sin x ^ 4 * cos x");
  assert(test_deriver(a,b));
}

//tests derive cos
void test_cos()
{
  char a[100], b[100];
  strcpy(a,"cos x"), strcpy(b,"-sin x");
  assert(test_deriver(a,b));
  strcpy(a,"cos (x + -56)"), strcpy(b,"-sin ( x + -56 )");
  assert(test_deriver(a,b));
  strcpy(a,"cos cos x"), strcpy(b,"( -sin cos x ) * ( -sin x )");
  assert(test_deriver(a,b));
  strcpy(a,"cos ( x ^ 5 )"), strcpy(b,"( -sin ( x ^ 5 ) ) * 5 * x ^ 4");
  assert(test_deriver(a,b));
  strcpy(a,"cos ( e ^ x )"), strcpy(b,"( -sin ( e ^ x ) ) * e ^ x");
  assert(test_deriver(a,b));
}

//tests derive tg and derive ctg
void test_tg_ctg()
{
  char a[100], b[100];
  strcpy(a,"tg x"), strcpy(b,"1 / cos x ^ 2");
  assert(test_deriver(a,b));
  strcpy(a,"ctg x"), strcpy(b,"-1 / sin x ^ 2");
  assert(test_deriver(a,b));
  strcpy(a,"tg ( x ^ 7 )"), strcpy(b,"1 / cos ( x ^ 7 ) ^ 2 * 7 * x ^ 6");
  assert(test_deriver(a,b));
  strcpy(a,"ctg ( x + 6 )"), strcpy(b,"-1 / sin ( x + 6 ) ^ 2");
  assert(test_deriver(a,b));
}

//tests derive arcsin and derive arccos
void test_arcsin_arccos()
{
  char a[100], b[100];
  strcpy(a,"arcsin x"), strcpy(b,"1 / sqrt ( 1 - x ^ 2 )");
  assert(test_deriver(a,b));
  strcpy(a,"arccos x"), strcpy(b,"-1 / sqrt ( 1 - x ^ 2 )");
  assert(test_deriver(a,b));
  strcpy(a,"arcsin sin x"), strcpy(b,"1 / sqrt ( 1 - sin x ^ 2 ) * cos x");
  assert(test_deriver(a,b));
  strcpy(a,"arccos ( x ^ 7 )");
  strcpy(b,"-1 / sqrt ( 1 - ( x ^ 7 ) ^ 2 ) * 7 * x ^ 6");
  assert(test_deriver(a,b));
}

//tests derive arctg and arcctg
void test_arctg_arcctg()
{
  char a[100], b[100];
  strcpy(a,"arctg x"), strcpy(b,"1 / ( 1 + x ^ 2 )");
  assert(test_deriver(a,b));
  strcpy(a,"arcctg x"), strcpy(b,"-1 / ( 1 + x ^ 2 )");
  assert(test_deriver(a,b));
  strcpy(a,"arctg ( x ^ 2 + 5 )");
  strcpy(b,"1 / ( 1 + ( x ^ 2 + 5 ) ^ 2 ) * 2 * x");
  assert(test_deriver(a,b));
  strcpy(a,"arcctg sin x"); strcpy(b,"-1 / ( 1 + sin x ^ 2 ) * cos x");
  assert(test_deriver(a,b));
}

//tets derive log and derive ln and derive lg
void test_log()
{

}

void test()
{ //add basic simplifier
  test_rpn_convertor();
  test_tree_to_infix();
  test_derivate_powconst();
  test_sin();
  test_cos();
  test_tg_ctg();
  test_arcsin_arccos();
  test_arctg_arcctg();
  test_log();
  printf("All tests passed!\n");
}

int main()
{
  test();
//   char a[50], b[50], c[50] = "6 * ( 7 * 3 + 5 )"; int p, q;
//   rpn_convertor(c,a);
//   p = strlen(a);
//   tree *t = tree_parser(a, &p, NULL); //derive(t);
// //  simplify(&t);
//   q = 0; tree_to_infix(b, &q , t); free_tree(t);
//   printf("%s\n",b );
  return 0;
}
