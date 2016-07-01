
#include <stdio.h>
#include <string.h>
#include "rbtree.h"

typedef struct Foo {
  int key;
} Foo;

typedef struct Bar {
  int bar;
  Foo barmap_key;
  rbnode_t rbnode;
} Bar;

static int strcomparer(void *lhs, void *rhs)
{
  char *s1 = (char*)lhs;
  char *s2 = (char*)rhs;

  return strcmp(s1, s2);
}

static int intcomparer(void *lhs, void *rhs)
{
  int a = *(int*)lhs;
  int b = *(int*)rhs;

  if (a == b) return 0;
  return a < b ? -1 : 1;
}

static int foocomparer(void *lhs, void *rhs)
{
  Foo *l = (Foo*)lhs;
  Foo *r = (Foo*)rhs;

  if (l->key == r->key) return 0;
  return l->key < r->key ? -1 : 1;
}

static int pfunc_str(void *key)
{
  printf("%s ", (char*)key);
}

static int pfunc_int(void *key)
{
  printf("%d ", *(int*)key);
}

static int pfunc_bar(void *key)
{
   pfunc_int((void*)&(((Foo*)key)->key));
}

#define CHECK_IF_FOUND(_pfunc, _key) \
do { \
  if (NULL != node) { \
      printf("Search key "); \
      _pfunc(node->key); \
      printf(" found\n"); \
  } else { \
      printf("Search key "); \
      _pfunc(_key); \
      printf(" not found\n"); \
  } \
} while(0);

void test_string_insertion(void)
{
  rbtree_t stringMap;
  rbnode_t string1;
  char *key1 = "hello";
  rbnode_t string2;
  char *key2 = "world";
  rbnode_t string3;
  char *key3 = "foo";
  rbnode_t string4;
  char *key4 = "bar";
  rbnode_t string5;
  char *key5 = "abc";

  INIT_RBTREE(&stringMap, strcomparer);

  INIT_RBNODE(&string1, key1);
  INIT_RBNODE(&string2, key2);
  INIT_RBNODE(&string3, key3);
  INIT_RBNODE(&string4, key4);
  INIT_RBNODE(&string5, key5);

  printf("Inserting strings to stringMap\n");
  insertRbNode(&stringMap, &string1);
  inorderTraversal(&stringMap, pfunc_str);
  printf("\n");
  insertRbNode(&stringMap, &string2);
  inorderTraversal(&stringMap, pfunc_str);
  printf("\n");
  insertRbNode(&stringMap, &string3);
  inorderTraversal(&stringMap, pfunc_str);
  printf("\n");
  insertRbNode(&stringMap, &string4);
  inorderTraversal(&stringMap, pfunc_str);
  printf("\n");
  insertRbNode(&stringMap, &string5);
  inorderTraversal(&stringMap, pfunc_str);
  printf("\n");

  printf("Performing inorder traversal for strings\n");
  inorderTraversal(&stringMap, pfunc_str);
  printf("\n");

  printf("Performing search in the tree\n");
  rbnode_t *node = searchKey(&stringMap, key1);
  CHECK_IF_FOUND(pfunc_str, key1);
  node = searchKey(&stringMap, key5);
  CHECK_IF_FOUND(pfunc_str, key5);
}

void test_int_insertion(void)
{
  rbtree_t intMap;
  rbnode_t int1;
  int key1 = 7;
  rbnode_t int2;
  int key2 = 6;
  rbnode_t int3;
  int key3 = 5;
  rbnode_t int4;
  int key4 = 4;
  int key5 = 99;

  INIT_RBTREE(&intMap, intcomparer);

  INIT_RBNODE(&int1, &key1);
  INIT_RBNODE(&int2, &key2);
  INIT_RBNODE(&int3, &key3);
  INIT_RBNODE(&int4, &key4);

  printf("Inserting int to intMap\n");
  insertRbNode(&intMap, &int1);
  insertRbNode(&intMap, &int2);
  insertRbNode(&intMap, &int3);
  insertRbNode(&intMap, &int4);

  printf("Performing inorder traversal for int\n");
  inorderTraversal(&intMap, pfunc_int);
  printf("\n");

  printf("Performing search in the tree\n");
  rbnode_t *node = searchKey(&intMap, &key1);
  CHECK_IF_FOUND(pfunc_int, &key1);
  key5 = 99;
  node = searchKey(&intMap, &key5);
  CHECK_IF_FOUND(pfunc_int, &key5);
}

static void test_bar_insertion(void)
{
  Bar b1, b2, b3;
  rbtree_t barMap;
  Foo key;
  b1.barmap_key.key = 1;
  b2.barmap_key.key = 2;
  b3.barmap_key.key = 3;

  key.key = 1;

  INIT_RBTREE_TYPE(&barMap, foocomparer, Bar, rbnode);

  INIT_RBNODE_TYPE(&b1,rbnode, Bar, barmap_key);
  INIT_RBNODE_TYPE(&b2,rbnode, Bar, barmap_key);
  INIT_RBNODE_TYPE(&b3,rbnode, Bar, barmap_key);

  printf("Inserting int to barMap\n");

  INSERT_RBNODE_TYPE(&barMap, &b1, rbnode);
  INSERT_RBNODE_TYPE(&barMap, &b2, rbnode);
  INSERT_RBNODE_TYPE(&barMap, &b3, rbnode);

  printf("Performing inorder traversal for barmap\n");
  inorderTraversal(&barMap, pfunc_bar);
  printf("\n");

  printf("Performing a search of key = %d element\n", key.key);
  Bar *elem = RBTREE_SEARCH(&barMap, Bar, &key);

  if (elem != &b1) {
    printf("Wrong insertion!!!\n");
  } else {
    printf("Bar element successfully retrieved!!!\n");
  }

  key.key = 4;
  elem = RBTREE_SEARCH(&barMap, Bar, &key);
  if (!elem) {
    printf("Key %d element not found\n", key.key);
  }
}

int main()
{
 test_string_insertion();

 test_int_insertion();

 test_bar_insertion();
}

