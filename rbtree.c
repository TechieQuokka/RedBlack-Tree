#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static rb_node_t *rb_node_create(rb_tree_t *tree, void *data);
static void rb_node_destroy(rb_tree_t *tree, rb_node_t *node);
static rb_node_t *rb_tree_minimum_node(rb_tree_t *tree, rb_node_t *node);
static rb_node_t *rb_tree_maximum_node(rb_tree_t *tree, rb_node_t *node);
static rb_node_t *rb_tree_successor_node(rb_tree_t *tree, rb_node_t *node);
static rb_node_t *rb_tree_predecessor_node(rb_tree_t *tree, rb_node_t *node);
static rb_node_t *rb_find_node(rb_tree_t *tree, const void *data);
static void rb_left_rotate(rb_tree_t *tree, rb_node_t *x);
static void rb_right_rotate(rb_tree_t *tree, rb_node_t *y);
static void rb_insert_fixup(rb_tree_t *tree, rb_node_t *z);
static void rb_delete_fixup(rb_tree_t *tree, rb_node_t *x);
static void rb_transplant(rb_tree_t *tree, rb_node_t *u, rb_node_t *v);
static void rb_inorder_walk_node(rb_tree_t *tree, rb_node_t *node, rb_visit_func_t visit, void *context);
static void rb_preorder_walk_node(rb_tree_t *tree, rb_node_t *node, rb_visit_func_t visit, void *context);
static void rb_postorder_walk_node(rb_tree_t *tree, rb_node_t *node, rb_visit_func_t visit, void *context);
static int rb_height_node(rb_tree_t *tree, rb_node_t *node);
static bool rb_is_valid_node(rb_tree_t *tree, rb_node_t *node, int *black_height);

rb_tree_t *rb_tree_create(rb_compare_func_t compare_func, rb_free_func_t free_func) {
    if (!compare_func) {
        return NULL;
    }
    
    rb_tree_t *tree = malloc(sizeof(rb_tree_t));
    if (!tree) {
        return NULL;
    }
    
    tree->nil = malloc(sizeof(rb_node_t));
    if (!tree->nil) {
        free(tree);
        return NULL;
    }
    
    tree->nil->color = RB_BLACK;
    tree->nil->left = tree->nil;
    tree->nil->right = tree->nil;
    tree->nil->parent = tree->nil;
    tree->nil->data = NULL;
    
    tree->root = tree->nil;
    tree->size = 0;
    tree->compare = compare_func;
    tree->free_data = free_func;
    
    return tree;
}

static void destroy_node_data(void *data, void *context) {
    rb_tree_t *tree = (rb_tree_t *)context;
    if (tree->free_data) {
        tree->free_data(data);
    }
}

void rb_tree_destroy(rb_tree_t *tree) {
    if (!tree) {
        return;
    }
    
    if (tree->free_data) {
        rb_postorder_walk(tree, destroy_node_data, tree);
    }
    
    rb_node_t *current = tree->root;
    while (current != tree->nil) {
        rb_node_t *temp = current;
        if (current->left != tree->nil) {
            current = current->left;
        } else if (current->right != tree->nil) {
            current = current->right;
        } else {
            current = current->parent;
            if (current != tree->nil) {
                if (current->left == temp) {
                    current->left = tree->nil;
                } else {
                    current->right = tree->nil;
                }
            }
            free(temp);
        }
    }
    
    free(tree->nil);
    free(tree);
}

static rb_node_t *rb_node_create(rb_tree_t *tree, void *data) {
    rb_node_t *node = malloc(sizeof(rb_node_t));
    if (!node) {
        return NULL;
    }
    
    node->data = data;
    node->color = RB_RED;
    node->left = tree->nil;
    node->right = tree->nil;
    node->parent = tree->nil;
    
    return node;
}

static void rb_node_destroy(rb_tree_t *tree, rb_node_t *node) {
    if (node != tree->nil) {
        if (tree->free_data) {
            tree->free_data(node->data);
        }
        free(node);
    }
}

static void rb_left_rotate(rb_tree_t *tree, rb_node_t *x) {
    rb_node_t *y = x->right;
    
    x->right = y->left;
    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    
    y->left = x;
    x->parent = y;
}

static void rb_right_rotate(rb_tree_t *tree, rb_node_t *y) {
    rb_node_t *x = y->left;
    
    y->left = x->right;
    if (x->right != tree->nil) {
        x->right->parent = y;
    }
    
    x->parent = y->parent;
    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    
    x->right = y;
    y->parent = x;
}

static void rb_insert_fixup(rb_tree_t *tree, rb_node_t *z) {
    while (z->parent->color == RB_RED) {
        if (z->parent == z->parent->parent->left) {
            rb_node_t *y = z->parent->parent->right;
            if (y->color == RB_RED) {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rb_left_rotate(tree, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_right_rotate(tree, z->parent->parent);
            }
        } else {
            rb_node_t *y = z->parent->parent->left;
            if (y->color == RB_RED) {
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rb_right_rotate(tree, z);
                }
                z->parent->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                rb_left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = RB_BLACK;
}

rb_result_t rb_insert(rb_tree_t *tree, void *data) {
    if (!tree || !data) {
        return RB_ERROR;
    }
    
    rb_node_t *z = rb_node_create(tree, data);
    if (!z) {
        return RB_MEMORY_ERROR;
    }
    
    rb_node_t *y = tree->nil;
    rb_node_t *x = tree->root;
    
    while (x != tree->nil) {
        y = x;
        int cmp = tree->compare(data, x->data);
        if (cmp < 0) {
            x = x->left;
        } else if (cmp > 0) {
            x = x->right;
        } else {
            free(z);
            return RB_DUPLICATE;
        }
    }
    
    z->parent = y;
    if (y == tree->nil) {
        tree->root = z;
    } else if (tree->compare(data, y->data) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }
    
    rb_insert_fixup(tree, z);
    tree->size++;
    
    return RB_OK;
}

static void rb_transplant(rb_tree_t *tree, rb_node_t *u, rb_node_t *v) {
    if (u->parent == tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

static void rb_delete_fixup(rb_tree_t *tree, rb_node_t *x) {
    while (x != tree->root && x->color == RB_BLACK) {
        if (x == x->parent->left) {
            rb_node_t *w = x->parent->right;
            if (w->color == RB_RED) {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RB_BLACK && w->right->color == RB_BLACK) {
                w->color = RB_RED;
                x = x->parent;
            } else {
                if (w->right->color == RB_BLACK) {
                    w->left->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_right_rotate(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->right->color = RB_BLACK;
                rb_left_rotate(tree, x->parent);
                x = tree->root;
            }
        } else {
            rb_node_t *w = x->parent->left;
            if (w->color == RB_RED) {
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == RB_BLACK && w->left->color == RB_BLACK) {
                w->color = RB_RED;
                x = x->parent;
            } else {
                if (w->left->color == RB_BLACK) {
                    w->right->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_left_rotate(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->left->color = RB_BLACK;
                rb_right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = RB_BLACK;
}

static rb_node_t *rb_find_node(rb_tree_t *tree, const void *data) {
    rb_node_t *current = tree->root;
    
    while (current != tree->nil) {
        int cmp = tree->compare(data, current->data);
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            return current;
        }
    }
    
    return tree->nil;
}

rb_result_t rb_delete(rb_tree_t *tree, const void *data) {
    if (!tree || !data) {
        return RB_ERROR;
    }
    
    rb_node_t *z = rb_find_node(tree, data);
    if (z == tree->nil) {
        return RB_NOT_FOUND;
    }
    
    rb_node_t *y = z;
    rb_node_t *x;
    rb_color_t y_original_color = y->color;
    
    if (z->left == tree->nil) {
        x = z->right;
        rb_transplant(tree, z, z->right);
    } else if (z->right == tree->nil) {
        x = z->left;
        rb_transplant(tree, z, z->left);
    } else {
        y = rb_tree_minimum_node(tree, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rb_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    if (y_original_color == RB_BLACK) {
        rb_delete_fixup(tree, x);
    }
    
    rb_node_destroy(tree, z);
    tree->size--;
    
    return RB_OK;
}

void *rb_search(rb_tree_t *tree, const void *data) {
    if (!tree || !data) {
        return NULL;
    }
    
    rb_node_t *node = rb_find_node(tree, data);
    return (node != tree->nil) ? node->data : NULL;
}

static rb_node_t *rb_tree_minimum_node(rb_tree_t *tree, rb_node_t *node) {
    while (node->left != tree->nil) {
        node = node->left;
    }
    return node;
}

static rb_node_t *rb_tree_maximum_node(rb_tree_t *tree, rb_node_t *node) {
    while (node->right != tree->nil) {
        node = node->right;
    }
    return node;
}

void *rb_min(rb_tree_t *tree) {
    if (!tree || tree->root == tree->nil) {
        return NULL;
    }
    
    rb_node_t *node = rb_tree_minimum_node(tree, tree->root);
    return node->data;
}

void *rb_max(rb_tree_t *tree) {
    if (!tree || tree->root == tree->nil) {
        return NULL;
    }
    
    rb_node_t *node = rb_tree_maximum_node(tree, tree->root);
    return node->data;
}

static rb_node_t *rb_tree_successor_node(rb_tree_t *tree, rb_node_t *node) {
    if (node->right != tree->nil) {
        return rb_tree_minimum_node(tree, node->right);
    }
    
    rb_node_t *y = node->parent;
    while (y != tree->nil && node == y->right) {
        node = y;
        y = y->parent;
    }
    return y;
}

static rb_node_t *rb_tree_predecessor_node(rb_tree_t *tree, rb_node_t *node) {
    if (node->left != tree->nil) {
        return rb_tree_maximum_node(tree, node->left);
    }
    
    rb_node_t *y = node->parent;
    while (y != tree->nil && node == y->left) {
        node = y;
        y = y->parent;
    }
    return y;
}

void *rb_successor(rb_tree_t *tree, const void *data) {
    if (!tree || !data) {
        return NULL;
    }
    
    rb_node_t *node = rb_find_node(tree, data);
    if (node == tree->nil) {
        return NULL;
    }
    
    rb_node_t *succ = rb_tree_successor_node(tree, node);
    return (succ != tree->nil) ? succ->data : NULL;
}

void *rb_predecessor(rb_tree_t *tree, const void *data) {
    if (!tree || !data) {
        return NULL;
    }
    
    rb_node_t *node = rb_find_node(tree, data);
    if (node == tree->nil) {
        return NULL;
    }
    
    rb_node_t *pred = rb_tree_predecessor_node(tree, node);
    return (pred != tree->nil) ? pred->data : NULL;
}

static void rb_inorder_walk_node(rb_tree_t *tree, rb_node_t *node, rb_visit_func_t visit, void *context) {
    if (node != tree->nil) {
        rb_inorder_walk_node(tree, node->left, visit, context);
        visit(node->data, context);
        rb_inorder_walk_node(tree, node->right, visit, context);
    }
}

static void rb_preorder_walk_node(rb_tree_t *tree, rb_node_t *node, rb_visit_func_t visit, void *context) {
    if (node != tree->nil) {
        visit(node->data, context);
        rb_preorder_walk_node(tree, node->left, visit, context);
        rb_preorder_walk_node(tree, node->right, visit, context);
    }
}

static void rb_postorder_walk_node(rb_tree_t *tree, rb_node_t *node, rb_visit_func_t visit, void *context) {
    if (node != tree->nil) {
        rb_postorder_walk_node(tree, node->left, visit, context);
        rb_postorder_walk_node(tree, node->right, visit, context);
        visit(node->data, context);
    }
}

void rb_inorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context) {
    if (tree && visit) {
        rb_inorder_walk_node(tree, tree->root, visit, context);
    }
}

void rb_preorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context) {
    if (tree && visit) {
        rb_preorder_walk_node(tree, tree->root, visit, context);
    }
}

void rb_postorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context) {
    if (tree && visit) {
        rb_postorder_walk_node(tree, tree->root, visit, context);
    }
}

size_t rb_size(rb_tree_t *tree) {
    return tree ? tree->size : 0;
}

bool rb_is_empty(rb_tree_t *tree) {
    return tree ? (tree->size == 0) : true;
}

static int rb_height_node(rb_tree_t *tree, rb_node_t *node) {
    if (node == tree->nil) {
        return 0;
    }
    
    int left_height = rb_height_node(tree, node->left);
    int right_height = rb_height_node(tree, node->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}

int rb_height(rb_tree_t *tree) {
    if (!tree) {
        return -1;
    }
    return rb_height_node(tree, tree->root);
}

static bool rb_is_valid_node(rb_tree_t *tree, rb_node_t *node, int *black_height) {
    if (node == tree->nil) {
        *black_height = 1;
        return true;
    }
    
    if (node->color == RB_RED) {
        if (node->left->color != RB_BLACK || node->right->color != RB_BLACK) {
            return false;
        }
    }
    
    int left_black_height, right_black_height;
    if (!rb_is_valid_node(tree, node->left, &left_black_height) ||
        !rb_is_valid_node(tree, node->right, &right_black_height)) {
        return false;
    }
    
    if (left_black_height != right_black_height) {
        return false;
    }
    
    *black_height = left_black_height + (node->color == RB_BLACK ? 1 : 0);
    return true;
}

bool rb_is_valid(rb_tree_t *tree) {
    if (!tree) {
        return false;
    }
    
    if (tree->root != tree->nil && tree->root->color != RB_BLACK) {
        return false;
    }
    
    int black_height;
    return rb_is_valid_node(tree, tree->root, &black_height);
}

static void print_data_wrapper(void *data, void *context) {
    void (*print_func)(const void *) = (void (*)(const void *))context;
    print_func(data);
}

void rb_print_tree(rb_tree_t *tree, void (*print_data)(const void *data)) {
    if (!tree || !print_data) {
        return;
    }
    
    printf("Red-Black Tree (size: %zu, height: %d)\n", tree->size, rb_height(tree));
    if (tree->root == tree->nil) {
        printf("Empty tree\n");
    } else {
        rb_inorder_walk(tree, print_data_wrapper, (void *)print_data);
    }
}