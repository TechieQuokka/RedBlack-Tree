#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "rbtree.h"
#include "rbtree_utils.h"

/* Benchmark configuration */
#define MAX_BENCHMARK_SIZE 100000
#define NUM_ITERATIONS 5
#define NUM_SEARCH_OPS 10000

/* Timer utilities */
typedef struct {
    clock_t start;
    clock_t end;
    double elapsed;
} timer_t;

void timer_start(timer_t *timer) {
    timer->start = clock();
}

void timer_stop(timer_t *timer) {
    timer->end = clock();
    timer->elapsed = ((double)(timer->end - timer->start)) / CLOCKS_PER_SEC;
}

/* Comparison function for integers */
int int_compare(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

/* Create integer pointer */
int *create_int(int value) {
    int *ptr = malloc(sizeof(int));
    if (ptr) {
        *ptr = value;
    }
    return ptr;
}

/* Benchmark insertion performance */
void benchmark_insertion() {
    printf("=== Insertion Benchmark ===\n");
    printf("Size     | Time (s) | Ops/sec  | Height | Valid\n");
    printf("---------|----------|----------|--------|------\n");
    
    int sizes[] = {100, 500, 1000, 5000, 10000, 50000, 100000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        double total_time = 0.0;
        int total_height = 0;
        bool all_valid = true;
        
        for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
            rb_tree_t *tree = rb_tree_create(int_compare, free);
            timer_t timer;
            
            timer_start(&timer);
            
            /* Insert sequential values */
            for (int i = 0; i < sizes[s]; i++) {
                int *value = create_int(i);
                rb_insert(tree, value);
            }
            
            timer_stop(&timer);
            
            total_time += timer.elapsed;
            total_height += rb_height(tree);
            all_valid = all_valid && rb_is_valid(tree);
            
            rb_tree_destroy(tree);
        }
        
        double avg_time = total_time / NUM_ITERATIONS;
        double avg_height = (double)total_height / NUM_ITERATIONS;
        double ops_per_sec = sizes[s] / avg_time;
        
        printf("%8d | %8.4f | %8.0f | %6.1f | %s\n",
               sizes[s], avg_time, ops_per_sec, avg_height, 
               all_valid ? "Yes" : "No");
    }
}

/* Benchmark search performance */
void benchmark_search() {
    printf("\n=== Search Benchmark ===\n");
    printf("Size     | Time (s) | Searches/sec | Hit Rate\n");
    printf("---------|----------|--------------|----------\n");
    
    int sizes[] = {1000, 5000, 10000, 50000, 100000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        rb_tree_t *tree = rb_tree_create(int_compare, free);
        
        /* Populate tree */
        for (int i = 0; i < sizes[s]; i++) {
            int *value = create_int(i);
            rb_insert(tree, value);
        }
        
        /* Prepare random search keys */
        int *search_keys = malloc(sizeof(int) * NUM_SEARCH_OPS);
        for (int i = 0; i < NUM_SEARCH_OPS; i++) {
            search_keys[i] = rand() % (sizes[s] * 2); /* 50% hit rate */
        }
        
        timer_t timer;
        int hits = 0;
        
        timer_start(&timer);
        
        /* Perform searches */
        for (int i = 0; i < NUM_SEARCH_OPS; i++) {
            void *result = rb_search(tree, &search_keys[i]);
            if (result) hits++;
        }
        
        timer_stop(&timer);
        
        double searches_per_sec = NUM_SEARCH_OPS / timer.elapsed;
        double hit_rate = (double)hits / NUM_SEARCH_OPS * 100.0;
        
        printf("%8d | %8.4f | %12.0f | %7.1f%%\n",
               sizes[s], timer.elapsed, searches_per_sec, hit_rate);
        
        free(search_keys);
        rb_tree_destroy(tree);
    }
}

/* Benchmark deletion performance */
void benchmark_deletion() {
    printf("\n=== Deletion Benchmark ===\n");
    printf("Size     | Time (s) | Deletions/sec | Valid\n");
    printf("---------|----------|---------------|------\n");
    
    int sizes[] = {1000, 5000, 10000, 50000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        double total_time = 0.0;
        bool all_valid = true;
        
        for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
            rb_tree_t *tree = rb_tree_create(int_compare, free);
            
            /* Populate tree */
            for (int i = 0; i < sizes[s]; i++) {
                int *value = create_int(i);
                rb_insert(tree, value);
            }
            
            /* Create random deletion order */
            int *delete_order = malloc(sizeof(int) * sizes[s]);
            for (int i = 0; i < sizes[s]; i++) {
                delete_order[i] = i;
            }
            
            /* Shuffle array */
            for (int i = sizes[s] - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = delete_order[i];
                delete_order[i] = delete_order[j];
                delete_order[j] = temp;
            }
            
            timer_t timer;
            timer_start(&timer);
            
            /* Delete half the elements */
            int num_deletions = sizes[s] / 2;
            for (int i = 0; i < num_deletions; i++) {
                rb_delete(tree, &delete_order[i]);
            }
            
            timer_stop(&timer);
            
            total_time += timer.elapsed;
            all_valid = all_valid && rb_is_valid(tree);
            
            free(delete_order);
            rb_tree_destroy(tree);
        }
        
        double avg_time = total_time / NUM_ITERATIONS;
        double deletions_per_sec = (sizes[s] / 2) / avg_time;
        
        printf("%8d | %8.4f | %13.0f | %s\n",
               sizes[s], avg_time, deletions_per_sec, 
               all_valid ? "Yes" : "No");
    }
}

/* Memory usage benchmark */
void benchmark_memory() {
    printf("\n=== Memory Usage Benchmark ===\n");
    printf("Size     | Memory (KB) | Bytes/Node | Efficiency\n");
    printf("---------|-------------|------------|----------\n");
    
    int sizes[] = {100, 500, 1000, 5000, 10000, 50000, 100000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        rb_tree_t *tree = rb_tree_create(int_compare, free);
        
        /* Populate tree */
        for (int i = 0; i < sizes[s]; i++) {
            int *value = create_int(i);
            rb_insert(tree, value);
        }
        
        size_t memory_usage = rb_memory_usage(tree);
        double memory_kb = (double)memory_usage / 1024.0;
        double bytes_per_node = (double)memory_usage / sizes[s];
        double efficiency = rb_memory_efficiency(tree);
        
        printf("%8d | %11.2f | %10.1f | %8.1f%%\n",
               sizes[s], memory_kb, bytes_per_node, efficiency);
        
        rb_tree_destroy(tree);
    }
}

/* Height analysis */
void benchmark_height_analysis() {
    printf("\n=== Height Analysis ===\n");
    printf("Size     | Actual | Theoretical | Efficiency\n");
    printf("         | Height | Min    Max  | Factor\n");
    printf("---------|--------|-------------|----------\n");
    
    int sizes[] = {100, 500, 1000, 5000, 10000, 50000, 100000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        double total_height = 0.0;
        
        for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
            rb_tree_t *tree = rb_tree_create(int_compare, free);
            
            /* Insert in random order for better analysis */
            int *values = malloc(sizeof(int) * sizes[s]);
            for (int i = 0; i < sizes[s]; i++) {
                values[i] = i;
            }
            
            /* Shuffle */
            for (int i = sizes[s] - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = values[i];
                values[i] = values[j];
                values[j] = temp;
            }
            
            /* Insert shuffled values */
            for (int i = 0; i < sizes[s]; i++) {
                int *value = create_int(values[i]);
                rb_insert(tree, value);
            }
            
            total_height += rb_height(tree);
            
            free(values);
            rb_tree_destroy(tree);
        }
        
        double avg_height = total_height / NUM_ITERATIONS;
        double min_height = log2(sizes[s] + 1);
        double max_height = 2.0 * log2(sizes[s] + 1);
        double efficiency = (max_height - avg_height) / (max_height - min_height) * 100.0;
        
        printf("%8d | %6.1f | %6.1f %6.1f | %8.1f%%\n",
               sizes[s], avg_height, min_height, max_height, efficiency);
    }
}

/* Iterator performance benchmark */
void benchmark_iterator() {
    printf("\n=== Iterator Performance ===\n");
    printf("Size     | Time (s) | Items/sec\n");
    printf("---------|----------|----------\n");
    
    int sizes[] = {1000, 5000, 10000, 50000, 100000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        rb_tree_t *tree = rb_tree_create(int_compare, free);
        
        /* Populate tree */
        for (int i = 0; i < sizes[s]; i++) {
            int *value = create_int(i);
            rb_insert(tree, value);
        }
        
        timer_t timer;
        timer_start(&timer);
        
        /* Iterate through entire tree */
        rb_iterator_t *iter = rb_iterator_create(tree);
        void *data = rb_iterator_first(iter);
        int count = 0;
        
        while (data) {
            count++;
            data = rb_iterator_next(iter);
        }
        
        rb_iterator_destroy(iter);
        timer_stop(&timer);
        
        double items_per_sec = count / timer.elapsed;
        
        printf("%8d | %8.4f | %9.0f\n",
               sizes[s], timer.elapsed, items_per_sec);
        
        rb_tree_destroy(tree);
    }
}

/* Stress test - mixed operations */
void stress_test() {
    printf("\n=== Stress Test (Mixed Operations) ===\n");
    
    rb_tree_t *tree = rb_tree_create(int_compare, free);
    timer_t timer;
    
    const int NUM_OPS = 100000;
    int insertions = 0, deletions = 0, searches = 0;
    
    srand((unsigned int)time(NULL));
    
    timer_start(&timer);
    
    for (int i = 0; i < NUM_OPS; i++) {
        int op = rand() % 10;
        int value = rand() % 50000;
        
        if (op < 5) { /* 50% insertions */
            int *data = create_int(value);
            rb_result_t result = rb_insert(tree, data);
            if (result == RB_DUPLICATE) {
                free(data);
            } else {
                insertions++;
            }
        } else if (op < 7) { /* 20% deletions */
            rb_result_t result = rb_delete(tree, &value);
            if (result == RB_OK) {
                deletions++;
            }
        } else { /* 30% searches */
            rb_search(tree, &value);
            searches++;
        }
        
        /* Validate tree every 10000 operations */
        if (i % 10000 == 0 && i > 0) {
            if (!rb_is_valid(tree)) {
                printf("ERROR: Tree became invalid at operation %d\n", i);
                break;
            }
        }
    }
    
    timer_stop(&timer);
    
    printf("Operations completed: %d\n", NUM_OPS);
    printf("  Insertions: %d\n", insertions);
    printf("  Deletions:  %d\n", deletions);
    printf("  Searches:   %d\n", searches);
    printf("Final tree size: %zu\n", rb_size(tree));
    printf("Final tree height: %d\n", rb_height(tree));
    printf("Total time: %.4f seconds\n", timer.elapsed);
    printf("Operations per second: %.0f\n", NUM_OPS / timer.elapsed);
    printf("Tree valid: %s\n", rb_is_valid(tree) ? "Yes" : "No");
    
    rb_tree_destroy(tree);
}

int main() {
    printf("Red-Black Tree Performance Benchmark\n");
    printf("====================================\n");
    
    srand((unsigned int)time(NULL));
    
    benchmark_insertion();
    benchmark_search();
    benchmark_deletion();
    benchmark_memory();
    benchmark_height_analysis();
    benchmark_iterator();
    stress_test();
    
    printf("\nBenchmark completed successfully!\n");
    return 0;
}