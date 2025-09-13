#include "rbtree_utils.h"
#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Internal helper functions */
static void collect_node_stats(rb_tree_t *tree, rb_node_t *node, 
                              rb_tree_stats_t *stats, int depth);
static void print_node_structure(rb_tree_t *tree, rb_node_t *node, 
                                void (*print_data)(const void *data), 
                                int depth, char *prefix, bool is_last);
static void print_node_dot(rb_tree_t *tree, rb_node_t *node, 
                          void (*print_data)(const void *data), FILE *file);
static bool nodes_equal(rb_tree_t *tree1, rb_node_t *node1, 
                       rb_tree_t *tree2, rb_node_t *node2);
static void count_range_nodes(rb_tree_t *tree, rb_node_t *node, 
                             const void *min_key, const void *max_key, size_t *count);
static void walk_range_nodes(rb_tree_t *tree, rb_node_t *node, 
                            const void *min_key, const void *max_key, 
                            rb_visit_func_t visit, void *context);

/* Get comprehensive statistics about the tree */
rb_tree_stats_t rb_get_statistics(rb_tree_t *tree) {
    rb_tree_stats_t stats = {0, 0, 0, 0, 1000000, 0.0};
    
    if (!tree || tree->root == tree->nil) {
        return stats;
    }
    
    collect_node_stats(tree, tree->root, &stats, 1);
    
    if (stats.total_nodes > 0) {
        stats.avg_depth = stats.avg_depth / stats.total_nodes;
    }
    
    return stats;
}

static void collect_node_stats(rb_tree_t *tree, rb_node_t *node, 
                              rb_tree_stats_t *stats, int depth) {
    if (node == tree->nil) {
        if (depth < stats->min_depth) {
            stats->min_depth = depth;
        }
        return;
    }
    
    stats->total_nodes++;
    stats->avg_depth += depth;
    
    if (node->color == RB_RED) {
        stats->red_nodes++;
    } else {
        stats->black_nodes++;
    }
    
    if (depth > stats->max_depth) {
        stats->max_depth = depth;
    }
    
    collect_node_stats(tree, node->left, stats, depth + 1);
    collect_node_stats(tree, node->right, stats, depth + 1);
}

/* Print tree statistics */
void rb_print_statistics(rb_tree_stats_t *stats) {
    printf("=== Red-Black Tree Statistics ===\n");
    printf("Total nodes:     %zu\n", stats->total_nodes);
    printf("Red nodes:       %zu (%.1f%%)\n", stats->red_nodes,
           stats->total_nodes > 0 ? 100.0 * stats->red_nodes / stats->total_nodes : 0.0);
    printf("Black nodes:     %zu (%.1f%%)\n", stats->black_nodes,
           stats->total_nodes > 0 ? 100.0 * stats->black_nodes / stats->total_nodes : 0.0);
    printf("Max depth:       %d\n", stats->max_depth);
    printf("Min depth:       %d\n", stats->min_depth);
    printf("Average depth:   %.2f\n", stats->avg_depth);
    printf("Theoretical min: %.0f\n", 
           stats->total_nodes > 0 ? log2(stats->total_nodes + 1) : 0.0);
    printf("Theoretical max: %.0f\n", 
           stats->total_nodes > 0 ? 2.0 * log2(stats->total_nodes + 1) : 0.0);
    printf("================================\n");
}

/* Print tree structure with visual representation */
void rb_print_tree_structure(rb_tree_t *tree, void (*print_data)(const void *data)) {
    if (!tree || !print_data) {
        return;
    }
    
    printf("Tree Structure:\n");
    if (tree->root == tree->nil) {
        printf("  (empty)\n");
        return;
    }
    
    char prefix[1024] = "";
    print_node_structure(tree, tree->root, print_data, 0, prefix, true);
}

static void print_node_structure(rb_tree_t *tree, rb_node_t *node, 
                                void (*print_data)(const void *data), 
                                int depth, char *prefix, bool is_last) {
    (void)prefix;   /* Unused parameter */
    (void)is_last;  /* Unused parameter */
    
    if (node == tree->nil) {
        return;
    }
    
    /* Print indentation */
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    /* Print node data and color */
    printf("[%c] ", node->color == RB_RED ? 'R' : 'B');
    print_data(node->data);
    printf("\n");
    
    /* Print children */
    if (node->left != tree->nil) {
        print_node_structure(tree, node->left, print_data, depth + 1, prefix, false);
    }
    
    if (node->right != tree->nil) {
        print_node_structure(tree, node->right, print_data, depth + 1, prefix, true);
    }
}

/* Print tree in DOT format for Graphviz */
void rb_print_dot_format(rb_tree_t *tree, void (*print_data)(const void *data), FILE *file) {
    if (!tree || !print_data || !file) {
        return;
    }
    
    fprintf(file, "digraph RedBlackTree {\n");
    fprintf(file, "  node [shape=circle];\n");
    fprintf(file, "  rankdir=TB;\n");
    
    if (tree->root != tree->nil) {
        print_node_dot(tree, tree->root, print_data, file);
    }
    
    fprintf(file, "}\n");
}

static void print_node_dot(rb_tree_t *tree, rb_node_t *node, 
                          void (*print_data)(const void *data), FILE *file) {
    if (node == tree->nil) {
        return;
    }
    
    /* Print node with color */
    fprintf(file, "  \"%p\" [label=\"", (void*)node);
    print_data(node->data);
    fprintf(file, "\" style=filled fillcolor=%s];\n", 
            node->color == RB_RED ? "red" : "black");
    
    /* Print edges to children */
    if (node->left != tree->nil) {
        fprintf(file, "  \"%p\" -> \"%p\" [label=\"L\"];\n", 
                (void*)node, (void*)node->left);
        print_node_dot(tree, node->left, print_data, file);
    }
    
    if (node->right != tree->nil) {
        fprintf(file, "  \"%p\" -> \"%p\" [label=\"R\"];\n", 
                (void*)node, (void*)node->right);
        print_node_dot(tree, node->right, print_data, file);
    }
}

/* Iterator implementation */
rb_iterator_t *rb_iterator_create(rb_tree_t *tree) {
    if (!tree) {
        return NULL;
    }
    
    rb_iterator_t *iter = malloc(sizeof(rb_iterator_t));
    if (!iter) {
        return NULL;
    }
    
    iter->tree = tree;
    iter->stack_capacity = 64; /* Initial capacity */
    iter->stack = malloc(sizeof(rb_node_t*) * iter->stack_capacity);
    if (!iter->stack) {
        free(iter);
        return NULL;
    }
    
    iter->stack_size = 0;
    iter->current = tree->nil;
    
    return iter;
}

void rb_iterator_destroy(rb_iterator_t *iter) {
    if (iter) {
        free(iter->stack);
        free(iter);
    }
}

void *rb_iterator_first(rb_iterator_t *iter) {
    if (!iter || !iter->tree) {
        return NULL;
    }
    
    iter->stack_size = 0;
    iter->current = iter->tree->root;
    
    /* Go to leftmost node */
    while (iter->current != iter->tree->nil) {
        if (iter->stack_size >= iter->stack_capacity) {
            iter->stack_capacity *= 2;
            iter->stack = realloc(iter->stack, 
                                sizeof(rb_node_t*) * iter->stack_capacity);
            if (!iter->stack) {
                return NULL;
            }
        }
        
        iter->stack[iter->stack_size++] = iter->current;
        iter->current = iter->current->left;
    }
    
    if (iter->stack_size > 0) {
        iter->current = iter->stack[--iter->stack_size];
        return iter->current->data;
    }
    
    return NULL;
}

void *rb_iterator_next(rb_iterator_t *iter) {
    if (!iter || !iter->tree || iter->current == iter->tree->nil) {
        return NULL;
    }
    
    /* If current has right child, go to leftmost in right subtree */
    if (iter->current->right != iter->tree->nil) {
        iter->current = iter->current->right;
        while (iter->current->left != iter->tree->nil) {
            if (iter->stack_size >= iter->stack_capacity) {
                iter->stack_capacity *= 2;
                iter->stack = realloc(iter->stack, 
                                    sizeof(rb_node_t*) * iter->stack_capacity);
                if (!iter->stack) {
                    return NULL;
                }
            }
            iter->stack[iter->stack_size++] = iter->current;
            iter->current = iter->current->left;
        }
        return iter->current->data;
    }
    
    /* Otherwise, go up the stack */
    if (iter->stack_size > 0) {
        iter->current = iter->stack[--iter->stack_size];
        return iter->current->data;
    }
    
    iter->current = iter->tree->nil;
    return NULL;
}

bool rb_iterator_has_next(rb_iterator_t *iter) {
    if (!iter || !iter->tree) {
        return false;
    }
    
    return (iter->current != iter->tree->nil && 
            (iter->current->right != iter->tree->nil || iter->stack_size > 0));
}

/* Tree comparison */
bool rb_trees_equal(rb_tree_t *tree1, rb_tree_t *tree2) {
    if (!tree1 || !tree2) {
        return (tree1 == tree2);
    }
    
    if (tree1->size != tree2->size) {
        return false;
    }
    
    return nodes_equal(tree1, tree1->root, tree2, tree2->root);
}

static bool nodes_equal(rb_tree_t *tree1, rb_node_t *node1, 
                       rb_tree_t *tree2, rb_node_t *node2) {
    if (node1 == tree1->nil && node2 == tree2->nil) {
        return true;
    }
    
    if (node1 == tree1->nil || node2 == tree2->nil) {
        return false;
    }
    
    if (tree1->compare(node1->data, node2->data) != 0) {
        return false;
    }
    
    return nodes_equal(tree1, node1->left, tree2, node2->left) &&
           nodes_equal(tree1, node1->right, tree2, node2->right);
}

/* Range operations */
size_t rb_count_range(rb_tree_t *tree, const void *min_key, const void *max_key) {
    if (!tree || !min_key || !max_key) {
        return 0;
    }
    
    size_t count = 0;
    count_range_nodes(tree, tree->root, min_key, max_key, &count);
    return count;
}

static void count_range_nodes(rb_tree_t *tree, rb_node_t *node, 
                             const void *min_key, const void *max_key, size_t *count) {
    if (node == tree->nil) {
        return;
    }
    
    int cmp_min = tree->compare(node->data, min_key);
    int cmp_max = tree->compare(node->data, max_key);
    
    if (cmp_min >= 0 && cmp_max <= 0) {
        (*count)++;
    }
    
    if (cmp_min > 0) {
        count_range_nodes(tree, node->left, min_key, max_key, count);
    }
    
    if (cmp_max < 0) {
        count_range_nodes(tree, node->right, min_key, max_key, count);
    }
    
    if (cmp_min >= 0 && cmp_max <= 0) {
        count_range_nodes(tree, node->left, min_key, max_key, count);
        count_range_nodes(tree, node->right, min_key, max_key, count);
    }
}

void rb_walk_range(rb_tree_t *tree, const void *min_key, const void *max_key, 
                   rb_visit_func_t visit, void *context) {
    if (!tree || !min_key || !max_key || !visit) {
        return;
    }
    
    walk_range_nodes(tree, tree->root, min_key, max_key, visit, context);
}

static void walk_range_nodes(rb_tree_t *tree, rb_node_t *node, 
                            const void *min_key, const void *max_key, 
                            rb_visit_func_t visit, void *context) {
    if (node == tree->nil) {
        return;
    }
    
    int cmp_min = tree->compare(node->data, min_key);
    int cmp_max = tree->compare(node->data, max_key);
    
    if (cmp_min > 0) {
        walk_range_nodes(tree, node->left, min_key, max_key, visit, context);
    }
    
    if (cmp_min >= 0 && cmp_max <= 0) {
        visit(node->data, context);
    }
    
    if (cmp_max < 0) {
        walk_range_nodes(tree, node->right, min_key, max_key, visit, context);
    }
}

/* Memory analysis */
size_t rb_memory_usage(rb_tree_t *tree) {
    if (!tree) {
        return 0;
    }
    
    size_t tree_overhead = sizeof(rb_tree_t) + sizeof(rb_node_t); /* NIL node */
    size_t node_memory = tree->size * sizeof(rb_node_t);
    
    return tree_overhead + node_memory;
}

double rb_memory_efficiency(rb_tree_t *tree) {
    if (!tree || tree->size == 0) {
        return 0.0;
    }
    
    size_t total_memory = rb_memory_usage(tree);
    size_t data_memory = tree->size * sizeof(void*); /* Just pointer size */
    
    return (double)data_memory / total_memory * 100.0;
}