# Red-Black Tree Usage Examples

## Basic Usage Patterns

### Integer Tree Example

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

// Comparison function for integers
int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// Print function for traversal
void print_int(const void *data, void *context) {
    printf("%d ", *(const int*)data);
}

int main() {
    // Create tree
    rb_tree_t *tree = rb_tree_create(int_compare, free);
    
    // Insert some values
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        int *data = malloc(sizeof(int));
        *data = values[i];
        rb_insert(tree, data);
    }
    
    // Print sorted values
    printf("Sorted values: ");
    rb_inorder_walk(tree, print_int, NULL);
    printf("\n");
    
    // Search for a value
    int search_key = 40;
    int *found = rb_search(tree, &search_key);
    printf("Found %d: %s\n", search_key, found ? "Yes" : "No");
    
    // Clean up
    rb_tree_destroy(tree);
    return 0;
}
```

### String Tree Example

```c
#include "rbtree.h"
#include <stdio.h>
#include <string.h>

int string_compare(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

void print_string(const void *data, void *context) {
    printf("\"%s\" ", (const char*)data);
}

int main() {
    rb_tree_t *tree = rb_tree_create(string_compare, NULL);
    
    // Note: Using string literals (no need to free)
    const char *words[] = {"banana", "apple", "cherry", "date"};
    int n = sizeof(words) / sizeof(words[0]);
    
    for (int i = 0; i < n; i++) {
        rb_insert(tree, (void*)words[i]);
    }
    
    printf("Sorted strings: ");
    rb_inorder_walk(tree, print_string, NULL);
    printf("\n");
    
    rb_tree_destroy(tree);
    return 0;
}
```

## Advanced Examples

### Employee Database

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int employee_id;
    char name[50];
    double salary;
    int department_id;
} employee_t;

// Compare by employee ID
int employee_compare(const void *a, const void *b) {
    const employee_t *emp_a = (const employee_t *)a;
    const employee_t *emp_b = (const employee_t *)b;
    return (emp_a->employee_id > emp_b->employee_id) - 
           (emp_a->employee_id < emp_b->employee_id);
}

void print_employee(const void *data, void *context) {
    const employee_t *emp = (const employee_t *)data;
    printf("ID: %d, Name: %-20s, Salary: $%8.2f, Dept: %d\n",
           emp->employee_id, emp->name, emp->salary, emp->department_id);
}

employee_t *create_employee(int id, const char *name, double salary, int dept_id) {
    employee_t *emp = malloc(sizeof(employee_t));
    if (!emp) return NULL;
    
    emp->employee_id = id;
    strncpy(emp->name, name, sizeof(emp->name) - 1);
    emp->name[sizeof(emp->name) - 1] = '\0';
    emp->salary = salary;
    emp->department_id = dept_id;
    
    return emp;
}

int main() {
    rb_tree_t *employees = rb_tree_create(employee_compare, free);
    
    // Add employees
    rb_insert(employees, create_employee(1001, "Alice Johnson", 75000, 10));
    rb_insert(employees, create_employee(1003, "Bob Smith", 82000, 20));
    rb_insert(employees, create_employee(1002, "Carol Wilson", 68000, 10));
    rb_insert(employees, create_employee(1005, "David Brown", 95000, 30));
    rb_insert(employees, create_employee(1004, "Eva Davis", 71000, 20));
    
    printf("Employee Database (sorted by ID):\n");
    printf("=====================================\n");
    rb_inorder_walk(employees, print_employee, NULL);
    
    // Search for specific employee
    employee_t search_key = {1003, "", 0, 0};
    employee_t *found = rb_search(employees, &search_key);
    
    if (found) {
        printf("\nFound employee 1003:\n");
        print_employee(found, NULL);
    }
    
    // Find salary range (using min/max)
    employee_t *min_id = rb_min(employees);
    employee_t *max_id = rb_max(employees);
    
    printf("\nEmployee ID range: %d to %d\n", 
           min_id->employee_id, max_id->employee_id);
    
    rb_tree_destroy(employees);
    return 0;
}
```

### Custom Data with Complex Comparison

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char category[20];
    int priority;
    char description[100];
    time_t created_time;
} task_t;

// Sort by category first, then by priority (higher first)
int task_compare(const void *a, const void *b) {
    const task_t *task_a = (const task_t *)a;
    const task_t *task_b = (const task_t *)b;
    
    // First compare by category
    int cmp = strcmp(task_a->category, task_b->category);
    if (cmp != 0) return cmp;
    
    // Then by priority (higher priority first)
    return (task_b->priority > task_a->priority) - 
           (task_b->priority < task_a->priority);
}

void print_task(const void *data, void *context) {
    const task_t *task = (const task_t *)data;
    printf("%-15s Priority: %d - %s\n", 
           task->category, task->priority, task->description);
}

task_t *create_task(const char *category, int priority, const char *desc) {
    task_t *task = malloc(sizeof(task_t));
    if (!task) return NULL;
    
    strncpy(task->category, category, sizeof(task->category) - 1);
    task->category[sizeof(task->category) - 1] = '\0';
    
    task->priority = priority;
    
    strncpy(task->description, desc, sizeof(task->description) - 1);
    task->description[sizeof(task->description) - 1] = '\0';
    
    task->created_time = time(NULL);
    
    return task;
}

int main() {
    rb_tree_t *tasks = rb_tree_create(task_compare, free);
    
    // Add tasks
    rb_insert(tasks, create_task("urgent", 5, "Fix critical bug"));
    rb_insert(tasks, create_task("development", 3, "Implement feature A"));
    rb_insert(tasks, create_task("urgent", 4, "Deploy hotfix"));
    rb_insert(tasks, create_task("maintenance", 2, "Update documentation"));
    rb_insert(tasks, create_task("development", 4, "Code review"));
    rb_insert(tasks, create_task("urgent", 5, "Security patch"));
    
    printf("Task List (sorted by category, then priority):\n");
    printf("==============================================\n");
    rb_inorder_walk(tasks, print_task, NULL);
    
    rb_tree_destroy(tasks);
    return 0;
}
```

## Memory Management Examples

### Dynamic String Storage

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string_duplicate(const char *str) {
    size_t len = strlen(str) + 1;
    char *dup = malloc(len);
    if (dup) {
        memcpy(dup, str, len);
    }
    return dup;
}

int string_compare(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

void print_string(const void *data, void *context) {
    printf("\"%s\" ", (const char*)data);
}

int main() {
    // Create tree with free function for dynamic strings
    rb_tree_t *tree = rb_tree_create(string_compare, free);
    
    const char *words[] = {"dynamic", "memory", "management", "example"};
    int n = sizeof(words) / sizeof(words[0]);
    
    for (int i = 0; i < n; i++) {
        char *str_copy = string_duplicate(words[i]);
        if (str_copy) {
            rb_insert(tree, str_copy);
        }
    }
    
    printf("Dynamically allocated strings: ");
    rb_inorder_walk(tree, print_string, NULL);
    printf("\n");
    
    // Tree destruction will automatically free all strings
    rb_tree_destroy(tree);
    return 0;
}
```

### Reference Counting Example

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int value;
    int ref_count;
} ref_counted_int_t;

ref_counted_int_t *create_ref_int(int value) {
    ref_counted_int_t *obj = malloc(sizeof(ref_counted_int_t));
    if (obj) {
        obj->value = value;
        obj->ref_count = 1;
    }
    return obj;
}

void release_ref_int(void *data) {
    ref_counted_int_t *obj = (ref_counted_int_t *)data;
    obj->ref_count--;
    if (obj->ref_count <= 0) {
        printf("Releasing ref_counted_int: %d\n", obj->value);
        free(obj);
    }
}

int ref_int_compare(const void *a, const void *b) {
    const ref_counted_int_t *ia = (const ref_counted_int_t *)a;
    const ref_counted_int_t *ib = (const ref_counted_int_t *)b;
    return (ia->value > ib->value) - (ia->value < ib->value);
}

int main() {
    rb_tree_t *tree = rb_tree_create(ref_int_compare, release_ref_int);
    
    // Insert reference counted objects
    for (int i = 0; i < 5; i++) {
        ref_counted_int_t *obj = create_ref_int(i * 10);
        if (obj) {
            rb_insert(tree, obj);
        }
    }
    
    printf("Tree created with %zu elements\n", rb_size(tree));
    
    // This will call release_ref_int for each object
    rb_tree_destroy(tree);
    
    return 0;
}
```

## Traversal Examples

### Statistical Analysis

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int count;
    double sum;
    double min_val;
    double max_val;
} stats_t;

void collect_stats(const void *data, void *context) {
    double value = *(const double *)data;
    stats_t *stats = (stats_t *)context;
    
    stats->count++;
    stats->sum += value;
    
    if (stats->count == 1 || value < stats->min_val) {
        stats->min_val = value;
    }
    
    if (stats->count == 1 || value > stats->max_val) {
        stats->max_val = value;
    }
}

int double_compare(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    return (da > db) - (da < db);
}

int main() {
    rb_tree_t *tree = rb_tree_create(double_compare, free);
    
    // Insert some values
    double values[] = {3.14, 2.71, 1.41, 1.73, 2.23, 0.57, 4.47};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        double *data = malloc(sizeof(double));
        *data = values[i];
        rb_insert(tree, data);
    }
    
    // Collect statistics using traversal
    stats_t stats = {0, 0.0, 0.0, 0.0};
    rb_inorder_walk(tree, collect_stats, &stats);
    
    printf("Statistics:\n");
    printf("  Count: %d\n", stats.count);
    printf("  Sum: %.6f\n", stats.sum);
    printf("  Average: %.6f\n", stats.sum / stats.count);
    printf("  Min: %.6f\n", stats.min_val);
    printf("  Max: %.6f\n", stats.max_val);
    
    rb_tree_destroy(tree);
    return 0;
}
```

### Tree Serialization

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

void serialize_int(const void *data, void *context) {
    FILE *file = (FILE *)context;
    int value = *(const int *)data;
    fprintf(file, "%d\n", value);
}

int int_compare(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

int main() {
    rb_tree_t *tree = rb_tree_create(int_compare, free);
    
    // Insert data
    for (int i = 0; i < 10; i++) {
        int *data = malloc(sizeof(int));
        *data = rand() % 100;
        rb_insert(tree, data);
    }
    
    // Serialize to file
    FILE *file = fopen("tree_data.txt", "w");
    if (file) {
        rb_inorder_walk(tree, serialize_int, file);
        fclose(file);
        printf("Tree serialized to tree_data.txt\n");
    }
    
    rb_tree_destroy(tree);
    return 0;
}
```

## Error Handling Examples

### Robust Insertion with Error Checking

```c
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int safe_insert_int(rb_tree_t *tree, int value) {
    int *data = malloc(sizeof(int));
    if (!data) {
        printf("Memory allocation failed for value %d\n", value);
        return 0;
    }
    
    *data = value;
    rb_result_t result = rb_insert(tree, data);
    
    switch (result) {
        case RB_OK:
            return 1;
            
        case RB_DUPLICATE:
            printf("Value %d already exists\n", value);
            free(data);
            return 0;
            
        case RB_MEMORY_ERROR:
            printf("Tree memory allocation failed\n");
            free(data);
            return 0;
            
        case RB_ERROR:
            printf("Invalid tree or data\n");
            free(data);
            return 0;
            
        default:
            printf("Unknown error\n");
            free(data);
            return 0;
    }
}

int main() {
    rb_tree_t *tree = rb_tree_create(int_compare, free);
    if (!tree) {
        printf("Failed to create tree\n");
        return 1;
    }
    
    int values[] = {10, 20, 10, 30, 20, 40}; // Note: duplicates
    int n = sizeof(values) / sizeof(values[0]);
    
    printf("Inserting values with error checking:\n");
    for (int i = 0; i < n; i++) {
        printf("Inserting %d: ", values[i]);
        if (safe_insert_int(tree, values[i])) {
            printf("Success\n");
        } else {
            printf("Failed\n");
        }
    }
    
    printf("Final tree size: %zu\n", rb_size(tree));
    
    rb_tree_destroy(tree);
    return 0;
}
```

These examples demonstrate the flexibility and power of the Red-Black Tree implementation across various use cases and data types.