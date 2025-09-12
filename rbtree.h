#ifndef RBTREE_H
#define RBTREE_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    RB_RED = 0,
    RB_BLACK = 1
} rb_color_t;

typedef enum {
    RB_OK = 0,
    RB_ERROR = -1,
    RB_NOT_FOUND = -2,
    RB_DUPLICATE = -3,
    RB_MEMORY_ERROR = -4
} rb_result_t;

typedef struct rb_node {
    void *data;
    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *parent;
    rb_color_t color;
} rb_node_t;

typedef int (*rb_compare_func_t)(const void *a, const void *b);
typedef void (*rb_visit_func_t)(void *data, void *context);
typedef void (*rb_free_func_t)(void *data);

typedef struct rb_tree {
    rb_node_t *root;
    rb_node_t *nil;
    size_t size;
    rb_compare_func_t compare;
    rb_free_func_t free_data;
} rb_tree_t;

rb_tree_t *rb_tree_create(rb_compare_func_t compare_func, rb_free_func_t free_func);
void rb_tree_destroy(rb_tree_t *tree);

rb_result_t rb_insert(rb_tree_t *tree, void *data);
rb_result_t rb_delete(rb_tree_t *tree, const void *data);
void *rb_search(rb_tree_t *tree, const void *data);

void *rb_min(rb_tree_t *tree);
void *rb_max(rb_tree_t *tree);
void *rb_successor(rb_tree_t *tree, const void *data);
void *rb_predecessor(rb_tree_t *tree, const void *data);

void rb_inorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context);
void rb_preorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context);
void rb_postorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context);

size_t rb_size(rb_tree_t *tree);
int rb_height(rb_tree_t *tree);
bool rb_is_valid(rb_tree_t *tree);
bool rb_is_empty(rb_tree_t *tree);

void rb_print_tree(rb_tree_t *tree, void (*print_data)(const void *data));

#endif /* RBTREE_H */