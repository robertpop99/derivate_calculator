#include "lists.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int item;

struct node
{
  item cont;
  struct node *next;
  struct node *previous;
};
typedef struct node node;

struct list
{
  node *def;
  node *current;
  node *end;
};
typedef struct list list;

list *newList(item d)
{
  list *l = malloc(sizeof(list));
  l->def = malloc(sizeof(node));
  l->end = malloc(sizeof(node));
  l->def->cont = d;
  l->current = NULL;
  l->def->next = l->end;
  l->end->previous = l->def;
  l->def->previous = NULL;
  l->end->next = NULL;
  return l;
}

static void freeNode(node *n)
{
  if(n->next != NULL)
    freeNode(n->next);
  free(n);
}

void freeList(list *l)
{
    freeNode(l->def);
    free(l);
}

void startF(list *l)
{
     l->current = l->def->next;
}

void startB(list *l)
{
     l->current = l->end->previous;
}

bool endF(list *l)
{

  return (l->current == l->end);
}

bool endB(list *l)
{

  return (l->current == l->def);
}

bool nextF(list *l)
{
  if(l->current == l->end) return false;
  l->current = l->current->next;
  return true;
}

bool nextB(list *l)
{
  if(l->current == l->def) return false;
  l->current = l->current->previous;
  return true;
}

void insertF(list *l, item x)
{
    node *n = malloc(sizeof(node));
    n->cont = x;
    if(l->def->next == l->end)
    {
        l->def->next = n;
        l->end->previous = n;
        n->next = l->end;
        n->previous = l->def;
        l->current = n;
    }
    else
    {
      n->next = l->current;
      n->previous = l->current->previous;
      l->current->previous->next = n;
      l->current->previous = n;
    }
}

void insertB(list *l, item x)
{
  node *n = malloc(sizeof(node));
  n->cont = x;
  if(l->def->next == l->end)
  {
    l->def->next = n;
    l->end->previous = n;
    n->next = l->end;
    n->previous = l->def;
    l->current = n;
  }
  else
  {
    n->previous = l->current;
    n->next = l->current->next;
    l->current->next->previous = n;
    l->current->next = n;
  }
}

item getF(list *l)
{
  if(l->current == l->end) return l->def->cont;
  return l->current->cont;
}

item getB(list *l)
{
  if(l->current == l->def) return l->def->cont;
  return l->current->cont;
}

bool setF(list *l, item x)
{
  if(l->current == l->end) return false;
  l->current->cont = x;
  return true;
}

bool setB(list *l, item x)
{
  if(l->current == l->def) return false;
  l->current->cont = x;
  return true;
}

bool deleteF(list *l)
{
  if(l->current == l->end) return false;
  l->current->previous->next = l->current->next;
  l->current->next->previous = l->current->previous;
  free(l->current);
  return true;
}

bool deleteB(list *l)
{
  if(l->current == l->def) return false;
  l->current->previous->next = l->current->next;
  l->current->next->previous = l->current->previous;
  free(l->current);
  return true;
}

//-----------------------------------------------------------------------------
// Testing for the lists module. Strings are used to describe lists. The strings
// "|37", "3|7", "37|" represent a list of two items, with the current position
// at the start, middle or end.
//#ifdef test_lists

#include <assert.h>

// Convert a string description to a list.
static list *toList(char *pic) {
    int pos = strchr(pic, '|') - pic;
    list *l = newList(-1);
    for (int i = 0; i < strlen(pic); i++) {
        if (pic[i] == '|') continue;
        int n = pic[i] - '0';
        insertF(l, n);
    }
    startF(l);
    for (int i = 0; i < pos; i++) nextF(l);
    return l;
}

// Convert a list to a string description.
static void toString(list *l, char s[]) {
    int pos = 0;
    while (! endB(l)) { pos++; nextB(l);}
    int i = 0;
    while (! endF(l)) {
        if (i == pos) s[i++] = '|';
        s[i++] = '0' + getF(l);
        nextF(l);
    }
    if (i == pos) s[i++] = '|';
    s[i++] = '\0';
}

// Check a particular function. The first argument is the name of the function.
// The second and third arguments describes the list before and after the call.
// The fourth, fifth and sixth arguments are an item to pass as an argument, an
// item to expect as a result, and a boolean to expect as a result, if any.
static bool check(char *op, char *lb, char *la, item x, item y, bool b) {
    bool r = true;
    item z = 0;
    list *l = toList(lb);
    if (strcmp(op, "startF") == 0) startF(l);
    else if (strcmp(op, "startB") == 0) startB(l);
    else if (strcmp(op, "endF") == 0) r = endF(l);
    else if (strcmp(op, "endB") == 0) r = endB(l);
    else if (strcmp(op, "nextF") == 0) r = nextF(l);
    else if (strcmp(op, "nextB") == 0) r = nextB(l);
    else if (strcmp(op, "insertF") == 0) insertF(l, x);
    else if (strcmp(op, "insertB") == 0) insertB(l, x);
    else if (strcmp(op, "getF") == 0) z = getF(l);
    else if (strcmp(op, "getB") == 0) z = getB(l);
    else if (strcmp(op, "setF") == 0) r = setF(l, x);
    else if (strcmp(op, "setB") == 0) r = setB(l, x);
    else if (strcmp(op, "deleteF") == 0) r = deleteF(l);
    else if (strcmp(op, "deleteB") == 0) r = deleteB(l);
    else return false;
    if (r != b || z != y) return false;
    char s[100];
    toString(l, s);
    freeList(l); printf("%s\n",s );
    return strcmp(s, la) == 0;
}

// Test newList, freeList.
static void test1() {
    list *l = newList(-1);
    assert(l != NULL);
    freeList(l);
}

// Test insertF, getB
static void test2() {
    list *l = newList(-1);
    insertF(l, 3);
    assert(getB(l) == 3);
    freeList(l);
}

// Test that startF and startB move to the beginning or end of the list.
static void testStart() {
    assert(check("startF", "|", "|", 0, 0, true));
    assert(check("startF", "|37", "|37", 0, 0, true));
    assert(check("startF", "3|7", "|37", 0, 0, true));
    assert(check("startF", "37|", "|37", 0, 0, true));
    assert(check("startB", "|", "|", 0, 0, true));
    assert(check("startB", "|37", "37|", 0, 0, true));
    assert(check("startB", "3|7", "37|", 0, 0, true));
    assert(check("startB", "37|", "37|", 0, 0, true));
}

// Test that endF and endB detect the list being at the end or beginning.
static void testEnd() {
    assert(check("endF", "|", "|", 0, 0, true));
    assert(check("endF", "|37", "|37", 0, 0, false));
    assert(check("endF", "3|7", "3|7", 0, 0, false));
    assert(check("endF", "37|", "37|", 0, 0, true));
    assert(check("endB", "|", "|", 0, 0, true));
    assert(check("endB", "|37", "|37", 0, 0, true));
    assert(check("endB", "3|7", "3|7", 0, 0, false));
    assert(check("endB", "37|", "37|", 0, 0, false));
}

// Test that nextF and nextB move forward or backward in the list.
static void testNext() {
    assert(check("nextF", "|", "|", 0, 0, false));
    assert(check("nextF", "|37", "3|7", 0, 0, true));
    assert(check("nextF", "3|7", "37|", 0, 0, true));
    assert(check("nextF", "37|", "37|", 0, 0, false));
    assert(check("nextB", "|", "|", 0, 0, false));
    assert(check("nextB", "|37", "|37", 0, 0, false));
    assert(check("nextB", "3|7", "|37", 0, 0, true));
    assert(check("nextB", "37|", "3|7", 0, 0, true));
}

// Test that insertF and insertB insert behind the direction of motion.
static void testInsert() {
    assert(check("insertF", "|", "5|", 5, 0, true));
    assert(check("insertF", "|37", "5|37", 5, 0, true));
    assert(check("insertF", "3|7", "35|7", 5, 0, true));
    assert(check("insertF", "37|", "375|", 5, 0, true));
    assert(check("insertB", "|", "|5", 5, 0, true));
    assert(check("insertB", "|37", "|537", 5, 0, true));
    assert(check("insertB", "3|7", "3|57", 5, 0, true));
    assert(check("insertB", "37|", "37|5", 5, 0, true));
}

// Test that getF and getB return the current item.
static void testGet() {
    assert(check("getF", "|", "|", 0, -1, true));
    assert(check("getF", "|37", "|37", 0, 3, true));
    assert(check("getF", "3|7", "3|7", 0, 7, true));
    assert(check("getF", "37|", "37|", 0, -1, true));
    assert(check("getB", "|", "|", 0, -1, true));
    assert(check("getB", "|37", "|37", 0, -1, true));
    assert(check("getB", "3|7", "3|7", 0, 3, true));
    assert(check("getB", "37|", "37|", 0, 7, true));
}

// Test setF and setB. Check's second argument is the expected result.
static void testSet() {
    assert(check("setF", "|", "|", 5, 0, false));
    assert(check("setF", "|37", "|57", 5, 0, true));
    assert(check("setF", "3|7", "3|5", 5, 0, true));
    assert(check("setF", "37|", "37|", 5, 0, false));
    assert(check("setB", "|", "|", 5, 0, false));
    assert(check("setB", "|37", "|37", 5, 0, false));
    assert(check("setB", "3|7", "5|7", 5, 0, true));
    assert(check("setB", "37|", "35|", 5, 0, true));
}

static void testDelete() {
    assert(check("deleteF", "|", "|", 0, 0, false));
    assert(check("deleteF", "|37", "|7", 0, 0, true));
    assert(check("deleteF", "3|7", "3|", 0, 0, true));
    assert(check("deleteF", "37|", "37|", 0, 0, false));
    assert(check("deleteB", "|", "|", 0, 0, false));
    assert(check("deleteB", "|37", "|37", 0, 0, false));
    assert(check("deleteB", "3|7", "|7", 0, 0, true));
    assert(check("deleteB", "37|", "3|", 0, 0, true));
}

int main() {
    test1();
    test2();
    testStart();
    testEnd();
    testNext();
    testInsert();
    testGet();
    testSet();
    testDelete();
    printf("Lists module OK\n");
    return 0;
}

//#endif
