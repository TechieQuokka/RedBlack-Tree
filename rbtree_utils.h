#ifndef RBTREE_UTILS_H
#define RBTREE_UTILS_H

#include "rbtree.h"
#include <stdio.h>

/* Statistics structure for tree analysis */
typedef struct {
    size_t total_nodes;
    size_t red_nodes;
    size_t black_nodes;
    int max_depth;
    int min_depth;
    double avg_depth;
} rb_tree_stats_t;

/* Iterator structure for tree traversal */
typedef struct rb_iterator {
    rb_tree_t *tree;
    rb_node_t **stack;
    int stack_size;
    int stack_capacity;
    rb_node_t *current;
} rb_iterator_t;

/* Tree statistics functions */
rb_tree_stats_t rb_get_statistics(rb_tree_t *tree);
void rb_print_statistics(rb_tree_stats_t *stats);

/* Tree visualization functions */
void rb_print_tree_structure(rb_tree_t *tree, void (*print_data)(const void *data));
void rb_print_dot_format(rb_tree_t *tree, void (*print_data)(const void *data), FILE *file);

/* Iterator functions */
rb_iterator_t *rb_iterator_create(rb_tree_t *tree);
void rb_iterator_destroy(rb_iterator_t *iter);
void *rb_iterator_first(rb_iterator_t *iter);
void *rb_iterator_next(rb_iterator_t *iter);
void *rb_iterator_last(rb_iterator_t *iter);
void *rb_iterator_prev(rb_iterator_t *iter);
bool rb_iterator_has_next(rb_iterator_t *iter);
bool rb_iterator_has_prev(rb_iterator_t *iter);

/* Tree comparison */
bool rb_trees_equal(rb_tree_t *tree1, rb_tree_t *tree2);

/* Range operations */
size_t rb_count_range(rb_tree_t *tree, const void *min_key, const void *max_key);
void rb_walk_range(rb_tree_t *tree, const void *min_key, const void *max_key, 
                   rb_visit_func_t visit, void *context);

/* Memory usage analysis */
size_t rb_memory_usage(rb_tree_t *tree);
double rb_memory_efficiency(rb_tree_t *tree);

#endif /* RBTREE_UTILS_H */