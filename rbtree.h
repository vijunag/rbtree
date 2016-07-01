/*
 * Author: Vijay Nag
 * RBTree implementation.
 * Courtesy: https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
 *
 * A Red-Black Tree is a balanced search tree data structure. The balance is
 * maintained by recoloring of nodes and minimal tree rotations. An AVL tree is
 * the best among the balanced tree data structure, however AVL trees
 * insertion/deletion involve way too many rotations. An AVL tree is preferred
 * when searches are more when compared to insertion/deletion operations.
 * RB trees insertion/deletion operation minimizes the number of rotations
 * by recoloring the nodes to meet certain invariants of the tree. So an RB tree
 * is alternative for AVL tree when there are frequent insertions/deletions.
 *
 * The following are the four invariants of an RB tree:
 * 1) The color of the node is either red or black.
 * 2) Root node is always black(red node can't have red parent or children)
 * 3) Two adjacent nodes cannot be red i.e parent and children cannot be red.
 * 4) Any path from root-NULL must always contain same number of black nodes.
 *
 */
#ifndef __RB_TREE_H_
#define __RB_TREE_H_

/* color sequence for printing elements */
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define YELLOW  "\033[33m"  /*consider this black if terminal is black */

#ifndef OFFSET_OF
#define OFFSET_OF(_t, _m) \
    (unsigned long)&(((_t*)0)->_m)
#endif /*OFFSET_OF*/

typedef enum {
  NODE_COLOR_NONE,
  NODE_COLOR_BLACK,
  NODE_COLOR_RED,
  NODE_COLOR_DOUBLE_BLACK,
  NODE_COLOR_MAX
} NodeColor_t;

struct rbnode {
  NodeColor_t color;
  struct rbnode *left;
  struct rbnode *right;
  struct rbnode *parent;
  void *key;
};
typedef struct rbnode rbnode_t;

typedef int (*compareFunc)(void *lhs, void *rhs);
typedef int (*printerFunc)(void *key);

typedef struct rbtree {
  rbnode_t *root;
  compareFunc comparer;
  unsigned long rb_offset;
  int size;
} rbtree_t;

/* Use the below two macros for plain old data-types*/
#define INIT_RBTREE(_rbtree, _comparer)   \
({                                        \
  (_rbtree)->root = NULL;                 \
  (_rbtree)->comparer = _comparer;        \
  (_rbtree)->size = 0;                    \
})

#define INIT_RBNODE(_rbnode, _key)     \
({                                     \
 (_rbnode)->left = NULL;               \
 (_rbnode)->right = NULL;              \
 (_rbnode)->parent = NULL;             \
 (_rbnode)->key = (void*)_key;         \
 (_rbnode)->color = NODE_COLOR_RED;    \
})

/*Use the below macros for user defined data types
 *Look at rbtree.c for example usage
 */
#define INIT_RBTREE_TYPE(_rbtree, _comparer, _type, _mem) \
({                                                        \
  (_rbtree)->root = NULL;                                 \
  (_rbtree)->comparer = _comparer;                        \
  (_rbtree)->size = 0;                                    \
  (_rbtree)->rb_offset = OFFSET_OF(_type, _mem);          \
})

#define INIT_RBNODE_TYPE(_elem, _rbnode, _type, _key)     \
({                                                        \
  INIT_RBNODE(&((_elem)->_rbnode), &((_elem)->_key));     \
})

#define INSERT_RBNODE(_rbtree, _rbnode) \
    insertRbNode(_rbtree, _rbnode);

#define DELETE_RBNODE(_rbtree, _key) \
    deleteRbNode(_rbtree_t, _key);

#define INSERT_RBNODE_TYPE(_rbtree, _elem, _mem) \
    INSERT_RBNODE(_rbtree, &((_elem)->_mem));

#define RBTREE_SEARCH(_tree, _type, _key)                              \
({                                                                     \
  rbnode_t *node = BstSearch((_tree)->root, _key, (_tree)->comparer);  \
  _type *_elem = NULL;                                                 \
  if (node) {                                                          \
    _elem = (_type*)((unsigned long)node-(_tree)->rb_offset);          \
  }                                                                    \
  (_type*)_elem;                                                       \
})

/*
 * Rotation algorithm summary:
 * dir and revdir represents the direction and opposite
 * direction of rotation respectively.
 * pivot = root.revdir
 * root.revdir = pivot.dir
 * pivot.dir = root
 * root = pivot
 *
 * Tree before Right rotation
 *           P
 *         /  \
 *        Q    R
 *       / \
 *      A   B
 *
 * Tree after Right rotation
 *          Q
 *        /   \
 *       A    P
 *           / \
 *          B   R
 */

#define ROTATE_TREE(dir, revdir, node, _root)\
({                                           \
   rbnode_t *pivot = node->revdir;           \
   node->revdir = pivot->dir;                \
   if (NULL != pivot->dir)                   \
      pivot->dir->parent = node;             \
   pivot->parent = node->parent;             \
   if (NULL == node->parent) {               \
     *_root = pivot;                         \
   } else if (node == node->parent->left) {  \
     node->parent->left = pivot;             \
   } else {                                  \
     node->parent->right = pivot;            \
   }                                         \
   pivot->dir = node;                        \
})

static inline rbnode_t* BstInsert(rbnode_t *root,
                                  rbnode_t *node,
                                  compareFunc comparer)
{
 if (NULL == root) return node;
/*
 * comparer returns 1 if LHS > RHS
 * else returns 0 if LHS <= RHS
 */
 if (0 < comparer(root->key, node->key)) {
     /*left subtree insertion */
   root->left = BstInsert(root->left, node, comparer);
   root->left->parent = root;
 } else {
   /*right subtree insertion*/
   root->right = BstInsert(root->right, node, comparer);
   root->right->parent = root;
 }
 return root;
}

static inline rbnode_t* BstSearch(rbnode_t *root, void *key,
                                  compareFunc comparer)
{
  if (NULL == root) return NULL;

  int ret = comparer(root->key, key);
  if (!ret)
      return root;

  if (0 < ret) {
    return BstSearch(root->left, key, comparer);
  } else {
    return BstSearch(root->right, key, comparer);
  }
}

/*Algorithm:
 * Let v be the Node to be deleted. Let successor
 * of the node i.e the node that replaces v be u.
 * An rbnode is deleted the following way
 * 1) Perform a standard BST delete. Always delete the leaf node(If not a leaf node
 *    find the inorder successor of the node to be deleted)
 * 2) If either u or v is red, mark the replaced child u as black
 * 3) If both u and v are black, mark u as double black and perform the following till u is double black
 *    3.1) If sibling s of v is black the following cases are possible
 *      3.1.1) If sibling is left child and if its right child is red (or both are red) it is an LR rotation case
 *      3.1.2) If sibling is left child and if its left child is red (or both are red) it is an LL rotation case
 *      3.1.3) if sibling is right child and if its left child is red (or both are red) it is an RL rotation case
 *      3.1.4) if sibling is right child and if its right child is red (or both are red) it is an RR rotation case
 *    3.2) If sibling s of v is black and both its children are black
 *      3.2.1) Mark s as red and make the parent black. If parent is double black, recur for the parent else make
 *             the parent black
 *    3.3)
 */
static inline rbnode_t *BstDelete(rbnode_t *root,
                                  rbnode_t *node)
{
  /* case 1: delete a leaf */
  if (NULL == node->left &&
      NULL == node->right) {
    if (NULL == node->parent)
        return NULL;
    if (node == node->parent->left) {
      node->parent->left = NULL;
    } else {
      node->parent->right = NULL;
    }
  }
  /* case 2: Node with one child */
  if (!(node->left && node->right)) {
   rbnode_t *parent = node->parent;
   if (parent->left == node) { /*left child*/
      parent->left = node->left ? node->left : node->right;
      parent->left->parent = parent;
   } else { /*right child*/
      parent->right = node->left ? node->left : node->right;
      parent->right->parent = parent;
   }
  } else { /* case 3: Node with both children */
   rbnode_t *p = node->right;
   while (NULL != p->left)
     p = p->left;
   if (p->right) {
     p->parent->left = p->right;
     p->right->parent = p->parent;
   } else {
     p->parent->left = NULL;
   }
   if (node->parent && node->parent->left == node) {
     node->parent->left = p;
   } else if (node->parent && node->parent->right == node) {
     node->parent->right = p;
   }
   p->left = node->left;
   p->right = node->right;
   p->parent = node->parent;
   if (root == node)
     root = p;
  }
  return root;
}

/* Insertion algorithm
 * Insert the element in the normal BST fashion and fix RB invariant violations if any.
 * 1) Every newly inserted node is colored RED
 * 2) Root of the tree is always black.
 * 3) If the parent and uncle are both RED, propogate the RED color upwards by making the
 *    grandparent RED and recolor the uncle/parent as black. The black height still remains
 *    the same. Recurse from grandparent and fix any violations.
 * 4) If uncle is black, the following tree configurations are possible
 *   A) If the node is left child then the following cases are possible
 *     i) Left-Left case - Node is left-left child. Right rotate the grand parent and swap the color with parent
 *    ii) Left-Right case - Node is left-right child. Left rotate the parent and perform step i
 *   iii) Right-Right case - Node is right-right child. Left rotate the grandparent and swap the color with parent
 *    iv) Right-Left case - Node is right-left child. Right rotate the parent and peform step iii
 */

static inline int insertRbNode(rbtree_t *rbtree, rbnode_t *rbnode)
{
#define SWAP_COLOR(node_a, node_b)    \
do {                                  \
 NodeColor_t color = (node_a)->color; \
 (node_a)->color = (node_b)->color;   \
 (node_b)->color = color;             \
} while(0);

   rbnode_t *root = BstInsert(rbtree->root, rbnode, rbtree->comparer);
    /* Let us fix any violation caused by insertion*/
   while(rbnode != root &&
        (rbnode->color != NODE_COLOR_BLACK) &&
        (rbnode->parent->color == NODE_COLOR_RED)) {
      rbnode_t *parent = rbnode->parent;
      rbnode_t *grandparent = parent->parent;
      rbnode_t *uncle = NULL;
      int isParentLeftChild = 0;

      /*Uncle is right child or left child of my grandparent ?*/
      if (parent == grandparent->left) {
          uncle = grandparent->right;
          isParentLeftChild = 1;
      } else {
          uncle = grandparent->left;
      }
      if ((NULL != uncle) && (uncle->color == NODE_COLOR_RED)) {
          grandparent->color = NODE_COLOR_RED;
          parent->color = uncle->color = NODE_COLOR_BLACK;
          rbnode = grandparent;
      } else if (isParentLeftChild) {
         /* Two configurations are possible
          * If the node is left child, then right rotate with pivot as grandparent.
          * Additionally, if the node is right child do a Left rotate pivoted at node and
          * then do the above step.
          */
         if (rbnode == parent->right) {
           ROTATE_TREE(left, right, parent, &root);
           rbnode = parent;
           parent = parent->parent;
         }
         ROTATE_TREE(right, left, grandparent, &root);
         SWAP_COLOR(grandparent, parent);
         rbnode = parent;
      } else {
         if (rbnode == parent->left) {
           ROTATE_TREE(right, left, parent, &root);
           rbnode = parent;
           parent = parent->parent;
         }
         ROTATE_TREE(left, right, grandparent, &root);
         SWAP_COLOR(grandparent, parent);
         rbnode = parent;
      }
  }
  root->color = NODE_COLOR_BLACK;
  rbtree->root = root;
  return 0;
#undef SWAP_COLOR
}

static inline void InorderTraversal(rbnode_t *rbnode, printerFunc pfunc)
{
  if (NULL == rbnode) return;

  InorderTraversal(rbnode->left, pfunc);
  if (NODE_COLOR_BLACK ==  rbnode->color) {
    printf(YELLOW);
  } else {
    printf(RED);
  }
  pfunc(rbnode->key);
  printf(RESET);
  InorderTraversal(rbnode->right, pfunc);
}

static inline void inorderTraversal(rbtree_t *tree, printerFunc pfunc)
{
  InorderTraversal(tree->root, pfunc);
}

static inline rbnode_t *searchKey(rbtree_t *tree, void *key)
{
  return BstSearch(tree->root, key, tree->comparer);
}

static inline int deleteRbNode(rbtree_t *rbtree, rbnode_t *node)
{
  rbnode_t *root = BstDelete(rbtree->root, node);
}

#endif /*__RB_TREE_H_*/

