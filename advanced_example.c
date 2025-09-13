#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rbtree.h"
#include "rbtree_utils.h"

/* Employee structure for demonstration */
typedef struct {
    int id;
    char name[32];
    char department[16];
    double salary;
    int years_experience;
} employee_t;

/* Comparison function for employees (by ID) */
int employee_compare(const void *a, const void *b) {
    const employee_t *emp_a = (const employee_t *)a;
    const employee_t *emp_b = (const employee_t *)b;
    return (emp_a->id > emp_b->id) - (emp_a->id < emp_b->id);
}

/* Print function for employees */
void print_employee(const void *data) {
    const employee_t *emp = (const employee_t *)data;
    printf("ID:%d(%s)", emp->id, emp->name);
}

/* Print detailed employee info */
void print_employee_detailed(void *data, void *context) {
    (void)context; /* Unused parameter */
    const employee_t *emp = (const employee_t *)data;
    printf("  ID: %d | %-15s | %-10s | $%8.2f | %2d years\n",
           emp->id, emp->name, emp->department, 
           emp->salary, emp->years_experience);
}

/* Employee factory function */
employee_t *create_employee(int id, const char *name, const char *dept, 
                           double salary, int experience) {
    employee_t *emp = malloc(sizeof(employee_t));
    if (!emp) return NULL;
    
    emp->id = id;
    strncpy(emp->name, name, sizeof(emp->name) - 1);
    emp->name[sizeof(emp->name) - 1] = '\0';
    strncpy(emp->department, dept, sizeof(emp->department) - 1);
    emp->department[sizeof(emp->department) - 1] = '\0';
    emp->salary = salary;
    emp->years_experience = experience;
    
    return emp;
}

/* Clone function for employees */
void *clone_employee(const void *data) {
    const employee_t *src = (const employee_t *)data;
    employee_t *copy = malloc(sizeof(employee_t));
    if (copy) {
        memcpy(copy, src, sizeof(employee_t));
    }
    return copy;
}

/* Salary calculation context */
typedef struct {
    double total_salary;
    int count;
    double max_salary;
    double min_salary;
} salary_stats_t;

/* Visitor function to calculate salary statistics */
void calculate_salary_stats(void *data, void *context) {
    const employee_t *emp = (const employee_t *)data;
    salary_stats_t *stats = (salary_stats_t *)context;
    
    stats->total_salary += emp->salary;
    stats->count++;
    
    if (stats->count == 1) {
        stats->max_salary = emp->salary;
        stats->min_salary = emp->salary;
    } else {
        if (emp->salary > stats->max_salary) {
            stats->max_salary = emp->salary;
        }
        if (emp->salary < stats->min_salary) {
            stats->min_salary = emp->salary;
        }
    }
}

/* Department filter context */
typedef struct {
    const char *target_dept;
    int count;
} dept_filter_t;

/* Visitor function to count employees by department */
void count_by_department(void *data, void *context) {
    const employee_t *emp = (const employee_t *)data;
    dept_filter_t *filter = (dept_filter_t *)context;
    
    if (strcmp(emp->department, filter->target_dept) == 0) {
        filter->count++;
    }
}

void demo_basic_operations() {
    printf("\n=== Basic Operations Demo ===\n");
    
    rb_tree_t *company = rb_tree_create(employee_compare, free);
    
    /* Add employees */
    rb_insert(company, create_employee(1001, "Alice Johnson", "Engineering", 85000, 5));
    rb_insert(company, create_employee(1003, "Bob Smith", "Marketing", 65000, 3));
    rb_insert(company, create_employee(1002, "Carol Davis", "Engineering", 92000, 8));
    rb_insert(company, create_employee(1005, "David Wilson", "Sales", 58000, 2));
    rb_insert(company, create_employee(1004, "Eva Brown", "HR", 71000, 6));
    rb_insert(company, create_employee(1007, "Frank Miller", "Engineering", 78000, 4));
    rb_insert(company, create_employee(1006, "Grace Lee", "Marketing", 69000, 3));
    
    printf("Company Database (sorted by ID):\n");
    printf("  ID  | Name            | Department | Salary   | Experience\n");
    printf("------|-----------------|------------|----------|-----------\n");
    rb_inorder_walk(company, print_employee_detailed, NULL);
    
    printf("\nTree size: %zu employees\n", rb_size(company));
    printf("Tree height: %d\n", rb_height(company));
    printf("Is valid RB-tree: %s\n", rb_is_valid(company) ? "Yes" : "No");
    
    rb_tree_destroy(company);
}

void demo_tree_statistics() {
    printf("\n=== Tree Statistics Demo ===\n");
    
    rb_tree_t *tree = rb_tree_create(employee_compare, free);
    
    /* Insert employees */
    for (int i = 1; i <= 15; i++) {
        char name[32];
        snprintf(name, sizeof(name), "Employee_%02d", i);
        rb_insert(tree, create_employee(1000 + i, name, "IT", 50000 + i * 1000, i % 10));
    }
    
    /* Get and print statistics */
    rb_tree_stats_t stats = rb_get_statistics(tree);
    rb_print_statistics(&stats);
    
    rb_tree_destroy(tree);
}

void demo_tree_visualization() {
    printf("\n=== Tree Visualization Demo ===\n");
    
    rb_tree_t *tree = rb_tree_create(employee_compare, free);
    
    /* Insert small set for visualization */
    int ids[] = {1005, 1003, 1007, 1001, 1004, 1006, 1008};
    char names[][16] = {"Alice", "Bob", "Carol", "David", "Eva", "Frank", "Grace"};
    
    for (int i = 0; i < 7; i++) {
        rb_insert(tree, create_employee(ids[i], names[i], "IT", 60000, 3));
    }
    
    rb_print_tree_structure(tree, print_employee);
    
    /* Export to DOT format */
    FILE *dot_file = fopen("tree.dot", "w");
    if (dot_file) {
        rb_print_dot_format(tree, print_employee, dot_file);
        fclose(dot_file);
        printf("\nTree exported to tree.dot (use 'dot -Tpng tree.dot -o tree.png')\n");
    }
    
    rb_tree_destroy(tree);
}

void demo_iterator() {
    printf("\n=== Iterator Demo ===\n");
    
    rb_tree_t *tree = rb_tree_create(employee_compare, free);
    
    /* Add some employees */
    for (int i = 10; i >= 1; i--) {
        char name[32];
        snprintf(name, sizeof(name), "Emp_%02d", i);
        rb_insert(tree, create_employee(1000 + i, name, "Dev", 50000, 2));
    }
    
    /* Iterate through tree */
    rb_iterator_t *iter = rb_iterator_create(tree);
    
    printf("Forward iteration:\n");
    void *data = rb_iterator_first(iter);
    while (data) {
        const employee_t *emp = (const employee_t *)data;
        printf("  %d: %s\n", emp->id, emp->name);
        data = rb_iterator_next(iter);
    }
    
    rb_iterator_destroy(iter);
    rb_tree_destroy(tree);
}

void demo_salary_analysis() {
    printf("\n=== Salary Analysis Demo ===\n");
    
    rb_tree_t *company = rb_tree_create(employee_compare, free);
    
    /* Add employees with varying salaries */
    rb_insert(company, create_employee(1001, "Alice", "Engineering", 95000, 7));
    rb_insert(company, create_employee(1002, "Bob", "Sales", 62000, 3));
    rb_insert(company, create_employee(1003, "Carol", "Engineering", 88000, 5));
    rb_insert(company, create_employee(1004, "David", "Marketing", 71000, 4));
    rb_insert(company, create_employee(1005, "Eva", "Engineering", 103000, 9));
    rb_insert(company, create_employee(1006, "Frank", "Sales", 58000, 2));
    rb_insert(company, create_employee(1007, "Grace", "HR", 67000, 6));
    
    /* Calculate salary statistics */
    salary_stats_t salary_stats = {0.0, 0, 0.0, 0.0};
    rb_inorder_walk(company, calculate_salary_stats, &salary_stats);
    
    printf("Salary Analysis:\n");
    printf("  Total employees: %d\n", salary_stats.count);
    printf("  Average salary: $%.2f\n", salary_stats.total_salary / salary_stats.count);
    printf("  Highest salary: $%.2f\n", salary_stats.max_salary);
    printf("  Lowest salary:  $%.2f\n", salary_stats.min_salary);
    printf("  Total payroll:  $%.2f\n", salary_stats.total_salary);
    
    /* Count by department */
    dept_filter_t eng_filter = {"Engineering", 0};
    rb_inorder_walk(company, count_by_department, &eng_filter);
    printf("  Engineering staff: %d\n", eng_filter.count);
    
    rb_tree_destroy(company);
}

void demo_range_operations() {
    printf("\n=== Range Operations Demo ===\n");
    
    rb_tree_t *tree = rb_tree_create(employee_compare, free);
    
    /* Add employees with IDs 1001-1020 */
    for (int i = 1; i <= 20; i++) {
        char name[32];
        snprintf(name, sizeof(name), "Employee_%02d", i);
        rb_insert(tree, create_employee(1000 + i, name, "IT", 50000 + i * 500, i % 8));
    }
    
    /* Define range */
    employee_t min_emp = {1005, "", "", 0, 0};
    employee_t max_emp = {1015, "", "", 0, 0};
    
    /* Count employees in range */
    size_t count = rb_count_range(tree, &min_emp, &max_emp);
    printf("Employees with IDs 1005-1015: %zu\n", count);
    
    /* Walk range */
    printf("Employee details in range:\n");
    rb_walk_range(tree, &min_emp, &max_emp, print_employee_detailed, NULL);
    
    rb_tree_destroy(tree);
}

void demo_memory_analysis() {
    printf("\n=== Memory Analysis Demo ===\n");
    
    rb_tree_t *tree = rb_tree_create(employee_compare, free);
    
    /* Add various numbers of employees and analyze memory */
    int sizes[] = {10, 50, 100, 500, 1000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        /* Clear tree first */
        rb_tree_destroy(tree);
        tree = rb_tree_create(employee_compare, free);
        
        /* Add employees */
        for (int i = 1; i <= sizes[s]; i++) {
            char name[32];
            snprintf(name, sizeof(name), "Emp_%04d", i);
            rb_insert(tree, create_employee(1000 + i, name, "IT", 50000, 3));
        }
        
        size_t memory_used = rb_memory_usage(tree);
        double efficiency = rb_memory_efficiency(tree);
        
        printf("Size: %4d | Memory: %6zu bytes | Efficiency: %5.1f%% | Per-node: %5.1f bytes\n",
               sizes[s], memory_used, efficiency, (double)memory_used / sizes[s]);
    }
    
    rb_tree_destroy(tree);
}

int main() {
    printf("Advanced Red-Black Tree Demonstration\n");
    printf("====================================\n");
    
    demo_basic_operations();
    demo_tree_statistics();
    demo_tree_visualization();
    demo_iterator();
    demo_salary_analysis();
    demo_range_operations();
    demo_memory_analysis();
    
    printf("\nAll demonstrations completed successfully!\n");
    return 0;
}