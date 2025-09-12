#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

typedef struct {
    int id;
    char name[50];
    double salary;
} employee_t;

int employee_compare(const void *a, const void *b) {
    const employee_t *emp_a = (const employee_t *)a;
    const employee_t *emp_b = (const employee_t *)b;
    
    return (emp_a->id > emp_b->id) - (emp_a->id < emp_b->id);
}

void print_employee(void *data, void *context) {
    (void)context;
    const employee_t *emp = (const employee_t *)data;
    printf("ID: %d, Name: %s, Salary: $%.2f\n", emp->id, emp->name, emp->salary);
}

void free_employee(void *data) {
    free(data);
}

employee_t *create_employee(int id, const char *name, double salary) {
    employee_t *emp = malloc(sizeof(employee_t));
    if (!emp) return NULL;
    
    emp->id = id;
    strncpy(emp->name, name, sizeof(emp->name) - 1);
    emp->name[sizeof(emp->name) - 1] = '\0';
    emp->salary = salary;
    
    return emp;
}

int main() {
    printf("Red-Black Tree Example: Employee Database\n");
    printf("==========================================\n\n");
    
    rb_tree_t *employee_db = rb_tree_create(employee_compare, free_employee);
    if (!employee_db) {
        fprintf(stderr, "Failed to create employee database\n");
        return 1;
    }
    
    printf("Adding employees to database...\n");
    
    employee_t *employees[] = {
        create_employee(101, "Alice Johnson", 75000.0),
        create_employee(205, "Bob Smith", 82000.0),
        create_employee(150, "Charlie Brown", 68000.0),
        create_employee(175, "Diana Prince", 95000.0),
        create_employee(120, "Eve Wilson", 71000.0)
    };
    
    int num_employees = sizeof(employees) / sizeof(employees[0]);
    
    for (int i = 0; i < num_employees; i++) {
        if (!employees[i]) {
            fprintf(stderr, "Failed to create employee\n");
            continue;
        }
        
        rb_result_t result = rb_insert(employee_db, employees[i]);
        if (result != RB_OK) {
            printf("Failed to insert employee ID %d\n", employees[i]->id);
            free_employee(employees[i]);
        } else {
            printf("Added: %s (ID: %d)\n", employees[i]->name, employees[i]->id);
        }
    }
    
    printf("\nDatabase size: %zu employees\n", rb_size(employee_db));
    printf("Tree height: %d\n", rb_height(employee_db));
    printf("Tree is valid: %s\n\n", rb_is_valid(employee_db) ? "Yes" : "No");
    
    printf("All employees (sorted by ID):\n");
    printf("-----------------------------\n");
    rb_inorder_walk(employee_db, print_employee, NULL);
    
    printf("\nSearching for specific employees:\n");
    printf("---------------------------------\n");
    
    int search_ids[] = {150, 999, 101};
    int num_searches = sizeof(search_ids) / sizeof(search_ids[0]);
    
    for (int i = 0; i < num_searches; i++) {
        employee_t search_key = {search_ids[i], "", 0.0};
        employee_t *found = (employee_t*)rb_search(employee_db, &search_key);
        
        if (found) {
            printf("Found employee ID %d: %s (Salary: $%.2f)\n", 
                   found->id, found->name, found->salary);
        } else {
            printf("Employee ID %d not found\n", search_ids[i]);
        }
    }
    
    printf("\nEmployee with lowest and highest ID:\n");
    printf("-----------------------------------\n");
    employee_t *min_emp = (employee_t*)rb_min(employee_db);
    employee_t *max_emp = (employee_t*)rb_max(employee_db);
    
    if (min_emp) printf("Lowest ID: %d (%s)\n", min_emp->id, min_emp->name);
    if (max_emp) printf("Highest ID: %d (%s)\n", max_emp->id, max_emp->name);
    
    printf("\nFinding successor and predecessor:\n");
    printf("---------------------------------\n");
    employee_t ref_key = {150, "", 0.0};
    employee_t *succ = (employee_t*)rb_successor(employee_db, &ref_key);
    employee_t *pred = (employee_t*)rb_predecessor(employee_db, &ref_key);
    
    printf("Employee ID 150's successor: %s\n", 
           succ ? succ->name : "None");
    printf("Employee ID 150's predecessor: %s\n", 
           pred ? pred->name : "None");
    
    printf("\nRemoving employee ID 120...\n");
    employee_t delete_key = {120, "", 0.0};
    rb_result_t result = rb_delete(employee_db, &delete_key);
    
    if (result == RB_OK) {
        printf("Employee ID 120 removed successfully\n");
        printf("New database size: %zu employees\n", rb_size(employee_db));
        
        printf("\nRemaining employees:\n");
        printf("-------------------\n");
        rb_inorder_walk(employee_db, print_employee, NULL);
    } else {
        printf("Failed to remove employee ID 120\n");
    }
    
    printf("\nTree is still valid: %s\n", rb_is_valid(employee_db) ? "Yes" : "No");
    
    rb_tree_destroy(employee_db);
    printf("\nEmployee database cleaned up successfully.\n");
    
    return 0;
}