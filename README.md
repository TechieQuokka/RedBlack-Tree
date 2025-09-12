# Red-Black Tree Implementation in C

A complete, production-ready implementation of Red-Black Trees in C with a clean API, comprehensive testing, and excellent performance characteristics.

## Features

- **Generic Data Support**: Works with any data type through function pointers
- **Memory Efficient**: Uses sentinel nodes to reduce memory overhead
- **Guaranteed Performance**: O(log n) for all major operations
- **Complete API**: Insert, delete, search, traversal, and utility functions
- **Robust Error Handling**: Comprehensive error codes and validation
- **Well Tested**: Extensive test suite with edge cases and performance tests

## Files

- `rbtree.h` - Header file with API definitions
- `rbtree.c` - Complete Red-Black Tree implementation
- `test.c` - Comprehensive test suite
- `example.c` - Real-world usage example (employee database)
- `Makefile` - Build system with multiple targets

## Documentation

- [`docs/API_Reference.md`](docs/API_Reference.md) - Complete API documentation
- [`docs/Algorithm_Details.md`](docs/Algorithm_Details.md) - Algorithm implementation details
- [`docs/Performance_Analysis.md`](docs/Performance_Analysis.md) - Performance analysis and benchmarks  
- [`docs/Usage_Examples.md`](docs/Usage_Examples.md) - Comprehensive usage examples
- [`RedBlack_Tree_Implementation_Paper.md`](RedBlack_Tree_Implementation_Paper.md) - Technical paper and design rationale

## Quick Start

### Building

```bash
# Build everything (default)
make

# Run tests
make test

# Build static library
make library

# Build with debug symbols
make debug

# Build optimized release
make release
```

### Basic Usage

```c
#include "rbtree.h"

// Define comparison function
int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int main() {
    // Create tree
    rb_tree_t *tree = rb_tree_create(int_compare, free);
    
    // Insert data
    int *data = malloc(sizeof(int));
    *data = 42;
    rb_insert(tree, data);
    
    // Search
    int search_key = 42;
    int *found = rb_search(tree, &search_key);
    
    // Clean up
    rb_tree_destroy(tree);
    return 0;
}
```

## API Reference

### Tree Management
- `rb_tree_create()` - Create new tree with comparison and cleanup functions
- `rb_tree_destroy()` - Destroy tree and free all memory

### Data Operations
- `rb_insert()` - Insert element (O(log n))
- `rb_delete()` - Delete element (O(log n))
- `rb_search()` - Search for element (O(log n))

### Navigation
- `rb_min()` - Find minimum element
- `rb_max()` - Find maximum element
- `rb_successor()` - Find next larger element
- `rb_predecessor()` - Find next smaller element

### Traversal
- `rb_inorder_walk()` - Visit elements in sorted order
- `rb_preorder_walk()` - Visit root first
- `rb_postorder_walk()` - Visit children first

### Utility
- `rb_size()` - Get number of elements
- `rb_height()` - Get tree height
- `rb_is_valid()` - Validate Red-Black properties
- `rb_is_empty()` - Check if tree is empty

## Error Codes

- `RB_OK` - Success
- `RB_ERROR` - General error
- `RB_NOT_FOUND` - Element not found
- `RB_DUPLICATE` - Duplicate element
- `RB_MEMORY_ERROR` - Memory allocation failed

## Performance

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Insert    | O(log n)       | O(1)            |
| Delete    | O(log n)       | O(1)            |
| Search    | O(log n)       | O(1)            |
| Min/Max   | O(log n)       | O(1)            |
| Traversal | O(n)           | O(1)            |

## Memory Usage

- **Per Node**: ~40 bytes on 64-bit systems
- **Tree Overhead**: ~48 bytes + sentinel node
- **Total**: O(n) space complexity

## Examples

### Integer Tree

```c
rb_tree_t *tree = rb_tree_create(int_compare, free);
int *value = malloc(sizeof(int));
*value = 100;
rb_insert(tree, value);
```

### String Tree

```c
int string_compare(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

rb_tree_t *tree = rb_tree_create(string_compare, NULL);
rb_insert(tree, "hello");
rb_insert(tree, "world");
```

### Custom Structures

See `example.c` for a complete example using employee records with ID-based sorting.

## Testing

The implementation includes comprehensive tests:

- **Basic Operations**: Insert, delete, search
- **Tree Properties**: Red-Black invariant validation
- **Edge Cases**: Empty tree, duplicates, non-existent keys
- **Performance**: Large dataset operations
- **Memory**: Leak detection and cleanup

Run tests with:
```bash
make test
```

## Thread Safety

This implementation is **not thread-safe** by default. For concurrent access:
- Use external synchronization (mutexes, rwlocks)
- Consider read-write locks for better read performance
- Lock-free alternatives are possible but significantly more complex

## Installation

```bash
# System-wide installation (requires root)
make install

# Manual installation
cp rbtree.h /usr/local/include/
cp bin/librbtree.a /usr/local/lib/
```

## License

This implementation is provided as-is for educational and production use. Feel free to modify and distribute according to your needs.

## Contributing

1. Run the test suite: `make test`
2. Validate with different compilers and sanitizers
3. Add tests for new features
4. Maintain the Red-Black Tree invariants
5. Follow the existing code style

## Technical Details

For detailed information about the algorithm, data structures, and implementation decisions, see the comprehensive technical paper: `RedBlack_Tree_Implementation_Paper.md`.