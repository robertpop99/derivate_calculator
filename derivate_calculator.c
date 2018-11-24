#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct tree tree;
struct tree
{
  char *expr;
  tree *l;
  tree *r;
};

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

//given a string, returns 1 if it is a known function
int function(char *c)
{
  if( !strcmp(c,"ln") ) return 1;
  if( !strcmp(c,"log") ) return 1;
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
  return 0;
}

//push a number to the output queue
void number(char *exp, char *res, int *i, int *j)
{
  *(res + *j) = *(exp + *i);
  *i += 1; *j += 1;
  while( isdigit(*(exp + *i)) )
  {
    *(res + *j) = *(exp + *i);
    *i += 1; *j += 1;
  }
  *(res + *j) = ' ';
  *j += 1;
}

//push a varaible (x) to the output queue
void variable(char *exp, char *res, int *i, int *j)
{
  *(res + *j) = *(exp + *i);
  *j += 1; *i += 1;
  *(res + *j) = ' ';
  *j += 1;
}

//returns the last element in the stack
void last(char *stack, int poz, char *l)
{
  int k = poz - 2, i = 0;
  while( stack[k - 1] != ' ' && k != 0)
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
    *poz -= strlen(l) + 1;
    if(*poz != 0) last(stack, *poz, l);
  }
  symbol_push(exp, stack, i, poz);
}

int is_function(char *exp, int i)
{
  int k = 0;
  char l[50];
  while( k < 50 && isalpha(*(exp + i)))
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
  while( k < 50 && isalpha(*(exp + i)))
  {
    l[k] = *(exp + i);
    i++; k++;
  }
  l[k] = '\0';
  return value(l);
}

void function_case(char *exp, char *stack, int *i, int *poz)
{
  while(isalpha(*(exp + *i)))
  {
    *(stack + *poz) = *(exp + *i);
    *poz += 1; *i += 1;
  }
  *(stack + *poz) = ' ';
  *poz += 1;
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
    *(res + *j) = ' ';
    *j += 1;
    *poz -= strlen(l) + 1;
  }
  *(res + *j) = '\0';
  return 0;
}

//converts a given string from infix notation to rpn notation
int rpn_convertor(char *exp, char *res)
{
  int i = 0, j = 0, poz = 0;
  char stack[1000]; stack[0] = '\0';
  while(*(exp + i) != '\0')
  {
    if( isdigit(*(exp + i)) )
        number(exp, res, &i, &j);
    else if( *(exp + i) == 'x' )
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

//---------------------------------------------------------------------------
//testing

void input()
{
  char exp[1000];
  printf("d / dx * ");
  fgets(exp, sizeof(exp), stdin);

}

void test()
{

  printf("All tests passed!\n");
}

int main()
{
  char exp[50] = "sin pi", res[50];
  memset(res, 0, sizeof(res));
  int c = rpn_convertor(exp,res);
  printf("%s\n",res );
//  printf("%d\n",c );
  return 0;
}
