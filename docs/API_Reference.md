# Red-Black Tree API Reference

## Data Types

### rb_color_t
```c
typedef enum {
    RB_RED = 0,
    RB_BLACK = 1
} rb_color_t;
```
Enumeration for node colors in the Red-Black Tree.

### rb_result_t
```c
typedef enum {
    RB_OK = 0,
    RB_ERROR = -1,
    RB_NOT_FOUND = -2,
    RB_DUPLICATE = -3,
    RB_MEMORY_ERROR = -4
} rb_result_t;
```
Return codes for Red-Black Tree operations.

### Function Pointers
```c
typedef int (*rb_compare_func_t)(const void *a, const void *b);
typedef void (*rb_visit_func_t)(void *data, void *context);
typedef void (*rb_free_func_t)(void *data);
```

## Tree Management Functions

### rb_tree_create
```c
rb_tree_t *rb_tree_create(rb_compare_func_t compare_func, rb_free_func_t free_func);
```
**Description**: Creates a new Red-Black Tree.

**Parameters**:
- `compare_func`: Function to compare two data elements (required)
- `free_func`: Function to free data elements (optional, can be NULL)

**Returns**: Pointer to new tree, or NULL on failure

**Example**:
```c
int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

rb_tree_t *tree = rb_tree_create(int_compare, free);
```

### rb_tree_destroy
```c
void rb_tree_destroy(rb_tree_t *tree);
```
**Description**: Destroys the tree and frees all memory.

**Parameters**:
- `tree`: Tree to destroy

**Note**: Automatically calls the free function for all data elements.

## Data Operations

### rb_insert
```c
rb_result_t rb_insert(rb_tree_t *tree, void *data);
```
**Description**: Inserts data into the tree.

**Parameters**:
- `tree`: Target tree
- `data`: Data to insert

**Returns**:
- `RB_OK`: Success
- `RB_DUPLICATE`: Data already exists
- `RB_MEMORY_ERROR`: Memory allocation failed
- `RB_ERROR`: Invalid parameters

**Time Complexity**: O(log n)

### rb_delete
```c
rb_result_t rb_delete(rb_tree_t *tree, const void *data);
```
**Description**: Removes data from the tree.

**Parameters**:
- `tree`: Target tree
- `data`: Data to remove (compared using tree's compare function)

**Returns**:
- `RB_OK`: Success
- `RB_NOT_FOUND`: Data not in tree
- `RB_ERROR`: Invalid parameters

**Time Complexity**: O(log n)

### rb_search
```c
void *rb_search(rb_tree_t *tree, const void *data);
```
**Description**: Searches for data in the tree.

**Parameters**:
- `tree`: Target tree
- `data`: Data to find

**Returns**: Pointer to found data, or NULL if not found

**Time Complexity**: O(log n)

## Navigation Functions

### rb_min
```c
void *rb_min(rb_tree_t *tree);
```
**Description**: Finds the minimum element in the tree.

**Returns**: Pointer to minimum data, or NULL if tree is empty

### rb_max
```c
void *rb_max(rb_tree_t *tree);
```
**Description**: Finds the maximum element in the tree.

**Returns**: Pointer to maximum data, or NULL if tree is empty

### rb_successor
```c
void *rb_successor(rb_tree_t *tree, const void *data);
```
**Description**: Finds the next larger element.

**Parameters**:
- `tree`: Target tree
- `data`: Reference data

**Returns**: Pointer to successor data, or NULL if none exists

### rb_predecessor
```c
void *rb_predecessor(rb_tree_t *tree, const void *data);
```
**Description**: Finds the next smaller element.

**Parameters**:
- `tree`: Target tree  
- `data`: Reference data

**Returns**: Pointer to predecessor data, or NULL if none exists

## Traversal Functions

### rb_inorder_walk
```c
void rb_inorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context);
```
**Description**: Visits all elements in sorted order.

**Parameters**:
- `tree`: Target tree
- `visit`: Callback function for each element
- `context`: Optional context data passed to callback

### rb_preorder_walk
```c
void rb_preorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context);
```
**Description**: Visits root first, then children.

### rb_postorder_walk
```c
void rb_postorder_walk(rb_tree_t *tree, rb_visit_func_t visit, void *context);
```
**Description**: Visits children first, then root.

## Utility Functions

### rb_size
```c
size_t rb_size(rb_tree_t *tree);
```
**Description**: Returns number of elements in tree.

**Time Complexity**: O(1)

### rb_height
```c
int rb_height(rb_tree_t *tree);
```
**Description**: Returns height of tree.

**Time Complexity**: O(n)

### rb_is_valid
```c
bool rb_is_valid(rb_tree_t *tree);
```
**Description**: Validates Red-Black Tree properties.

**Returns**: true if tree satisfies all Red-Black invariants

### rb_is_empty
```c
bool rb_is_empty(rb_tree_t *tree);
```
**Description**: Checks if tree contains no elements.

### rb_print_tree
```c
void rb_print_tree(rb_tree_t *tree, void (*print_data)(const void *data));
```
**Description**: Prints tree structure and contents.

## Usage Patterns

### Basic Integer Tree
```c
rb_tree_t *tree = rb_tree_create(int_compare, free);

int *value = malloc(sizeof(int));
*value = 42;
rb_insert(tree, value);

int search_key = 42;
int *found = rb_search(tree, &search_key);

rb_tree_destroy(tree);
```

### String Tree  
```c
int string_compare(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

rb_tree_t *tree = rb_tree_create(string_compare, NULL);
rb_insert(tree, "hello");
rb_insert(tree, "world");

char *found = rb_search(tree, "hello");
rb_tree_destroy(tree);
```

### Custom Structure
```c
typedef struct {
    int id;
    char name[50];
} person_t;

int person_compare(const void *a, const void *b) {
    const person_t *pa = a;
    const person_t *pb = b;
    return (pa->id > pb->id) - (pa->id < pb->id);
}

rb_tree_t *tree = rb_tree_create(person_compare, free);
// Insert person structures...
```