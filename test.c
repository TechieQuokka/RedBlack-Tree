#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include "rbtree.h"

int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void print_int(void *data, void *context) {
    (void)context;
    printf("%d ", *(const int*)data);
}

void free_int(void *data) {
    free(data);
}

int *create_int(int value) {
    int *ptr = malloc(sizeof(int));
    if (ptr) {
        *ptr = value;
    }
    return ptr;
}

void test_basic_operations() {
    printf("=== Testing Basic Operations ===\n");
    
    rb_tree_t *tree = rb_tree_create(int_compare, free_int);
    assert(tree != NULL);
    assert(rb_is_empty(tree));
    assert(rb_size(tree) == 0);
    
    int values[] = {10, 20, 30, 15, 25, 5, 1};
    int n = sizeof(values) / sizeof(values[0]);
    
    printf("Inserting values: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", values[i]);
        int *data = create_int(values[i]);
        assert(data != NULL);
        rb_result_t result = rb_insert(tree, data);
        assert(result == RB_OK);
    }
    printf("\n");
    
    printf("Tree size: %zu\n", rb_size(tree));
    printf("Tree height: %d\n", rb_height(tree));
    printf("Tree is valid: %s\n", rb_is_valid(tree) ? "Yes" : "No");
    
    printf("Inorder traversal: ");
    rb_inorder_walk(tree, print_int, NULL);
    printf("\n");
    
    for (int i = 0; i < n; i++) {
        int *found = (int*)rb_search(tree, &values[i]);
        assert(found != NULL);
        assert(*found == values[i]);
    }
    printf("All search operations successful\n");
    
    int *min_val = (int*)rb_min(tree);
    int *max_val = (int*)rb_max(tree);
    printf("Minimum: %d, Maximum: %d\n", *min_val, *max_val);
    
    int search_val = 20;
    int *succ = (int*)rb_successor(tree, &search_val);
    int *pred = (int*)rb_predecessor(tree, &search_val);
    printf("Successor of %d: %s\n", search_val, succ ? "found" : "not found");
    printf("Predecessor of %d: %s\n", search_val, pred ? "found" : "not found");
    
    rb_tree_destroy(tree);
    printf("Basic operations test passed!\n\n");
}

void test_deletion() {
    printf("=== Testing Deletion ===\n");
    
    rb_tree_t *tree = rb_tree_create(int_compare, free_int);
    assert(tree != NULL);
    
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        int *data = create_int(values[i]);
        rb_insert(tree, data);
    }
    
    printf("Initial tree (size: %zu): ", rb_size(tree));
    rb_inorder_walk(tree, print_int, NULL);
    printf("\n");
    
    int delete_values[] = {10, 25, 50};
    int delete_n = sizeof(delete_values) / sizeof(delete_values[0]);
    
    for (int i = 0; i < delete_n; i++) {
        printf("Deleting %d... ", delete_values[i]);
        rb_result_t result = rb_delete(tree, &delete_values[i]);
        assert(result == RB_OK);
        printf("Tree is valid: %s, size: %zu\n", 
               rb_is_valid(tree) ? "Yes" : "No", rb_size(tree));
        
        int *found = (int*)rb_search(tree, &delete_values[i]);
        assert(found == NULL);
    }
    
    printf("Final tree: ");
    rb_inorder_walk(tree, print_int, NULL);
    printf("\n");
    
    rb_tree_destroy(tree);
    printf("Deletion test passed!\n\n");
}

void test_edge_cases() {
    printf("=== Testing Edge Cases ===\n");
    
    rb_tree_t *tree = rb_tree_create(int_compare, free_int);
    
    int non_existent = 999;
    rb_result_t result = rb_delete(tree, &non_existent);
    assert(result == RB_NOT_FOUND);
    
    int *found = (int*)rb_search(tree, &non_existent);
    assert(found == NULL);
    
    int *min_val = (int*)rb_min(tree);
    int *max_val = (int*)rb_max(tree);
    assert(min_val == NULL);
    assert(max_val == NULL);
    
    int value = 42;
    int *data = create_int(value);
    rb_insert(tree, data);
    
    int *duplicate = create_int(value);
    result = rb_insert(tree, duplicate);
    assert(result == RB_DUPLICATE);
    free(duplicate);
    
    rb_tree_destroy(tree);
    printf("Edge cases test passed!\n\n");
}

void test_large_dataset() {
    printf("=== Testing Large Dataset ===\n");
    
    rb_tree_t *tree = rb_tree_create(int_compare, free_int);
    const int N = 1000;
    
    clock_t start = clock();
    
    for (int i = 0; i < N; i++) {
        int *data = create_int(rand() % (N * 2));
        rb_insert(tree, data);
    }
    
    clock_t end = clock();
    double insert_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Inserted %d random values\n", N);
    printf("Final tree size: %zu\n", rb_size(tree));
    printf("Tree height: %d (optimal: ~%.1f)\n", rb_height(tree), log2(rb_size(tree)));
    printf("Tree is valid: %s\n", rb_is_valid(tree) ? "Yes" : "No");
    printf("Insert time: %.4f seconds\n", insert_time);
    
    start = clock();
    for (int i = 0; i < N / 2; i++) {
        int search_val = rand() % (N * 2);
        rb_search(tree, &search_val);
    }
    end = clock();
    double search_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Search time for %d operations: %.4f seconds\n", N/2, search_time);
    
    rb_tree_destroy(tree);
    printf("Large dataset test passed!\n\n");
}

void test_string_data() {
    printf("=== Testing String Data ===\n");
    
    int string_compare(const void *a, const void *b) {
        return strcmp((const char*)a, (const char*)b);
    }
    
    void print_string(void *data, void *context) {
        (void)context;
        printf("\"%s\" ", (const char*)data);
    }
    
    rb_tree_t *tree = rb_tree_create(string_compare, NULL);
    
    const char *words[] = {"banana", "apple", "cherry", "date", "elderberry"};
    int n = sizeof(words) / sizeof(words[0]);
    
    for (int i = 0; i < n; i++) {
        rb_insert(tree, (void*)words[i]);
    }
    
    printf("String tree (sorted): ");
    rb_inorder_walk(tree, print_string, NULL);
    printf("\n");
    
    const char *search_word = "cherry";
    char *found = (char*)rb_search(tree, search_word);
    printf("Searching for \"%s\": %s\n", search_word, found ? "Found" : "Not found");
    
    rb_tree_destroy(tree);
    printf("String data test passed!\n\n");
}

int main() {
    printf("Red-Black Tree Implementation Test Suite\n");
    printf("==========================================\n\n");
    
    srand((unsigned int)time(NULL));
    
    test_basic_operations();
    test_deletion();
    test_edge_cases();
    test_large_dataset();
    test_string_data();
    
    printf("All tests passed successfully!\n");
    return 0;
}