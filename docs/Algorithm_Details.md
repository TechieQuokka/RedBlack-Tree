# Red-Black Tree Algorithm Details

## Red-Black Tree Properties

A Red-Black Tree is a binary search tree that maintains balance through node coloring and specific invariants:

1. **Color Property**: Every node is either red or black
2. **Root Property**: The root node is always black
3. **Leaf Property**: All leaf nodes (NIL) are black
4. **Red Property**: Red nodes cannot have red children
5. **Black-Height Property**: All paths from root to leaves have equal black node count

## Tree Rotations

Rotations are fundamental operations that preserve the binary search tree property while changing the tree structure.

### Left Rotation

**Before:**
```
     x                
    / \               
   α   y              
      / \             
     β   γ            
```

**After:**
```
       y
      / \
     x   γ
    / \
   α   β
```

**Algorithm:**
1. Set y = x.right
2. Update x.right = y.left
3. Update parent pointers
4. Set y.left = x

### Right Rotation

**Before:**
```
     y
    / \
   x   γ
  / \
 α   β
```

**After:**
```
     x
    / \
   α   y
      / \
     β   γ
```

**Algorithm:**
1. Set x = y.left
2. Update y.left = x.right
3. Update parent pointers
4. Set x.right = y

## Insertion Algorithm

### Phase 1: BST Insertion
1. Start at root
2. Compare new key with current node
3. Go left if smaller, right if larger
4. Insert at leaf position
5. Color new node RED

### Phase 2: Red-Black Fixup
Handle violations of Red-Black properties:

#### Case 1: Uncle is RED
- **Condition**: Parent and uncle are both red
- **Action**: 
  - Color parent and uncle black
  - Color grandparent red
  - Move up to grandparent and repeat

#### Case 2: Uncle is BLACK (Triangle)
- **Condition**: Uncle black, current node and parent form triangle
- **Action**:
  - Rotate parent to form line configuration
  - Proceed to Case 3

#### Case 3: Uncle is BLACK (Line)
- **Condition**: Uncle black, current node and parent form line
- **Action**:
  - Color parent black
  - Color grandparent red  
  - Rotate grandparent

**Termination**: Root is always colored black

## Deletion Algorithm

More complex than insertion due to multiple cases.

### Phase 1: Node Removal
#### Case 1: No Children
- Simply remove the node

#### Case 2: One Child
- Replace node with its child
- Update parent pointers

#### Case 3: Two Children
- Find successor (minimum in right subtree)
- Replace node's data with successor's data
- Delete successor (which has at most one child)

### Phase 2: Red-Black Fixup (if deleted node was BLACK)

#### Case 1: Sibling is RED
- **Action**: 
  - Color sibling black
  - Color parent red
  - Rotate parent
  - Convert to other cases

#### Case 2: Sibling BLACK, Both Nephews BLACK
- **Action**:
  - Color sibling red
  - Move problem up to parent

#### Case 3: Sibling BLACK, Left Nephew RED, Right BLACK
- **Action**:
  - Color left nephew black
  - Color sibling red
  - Right rotate sibling
  - Convert to Case 4

#### Case 4: Sibling BLACK, Right Nephew RED
- **Action**:
  - Color sibling with parent's color
  - Color parent black
  - Color right nephew black
  - Left rotate parent
  - Terminate

## Search Algorithm

Standard BST search with sentinel optimization:

```c
rb_node_t *rb_search_node(rb_tree_t *tree, const void *key) {
    rb_node_t *current = tree->root;
    
    while (current != tree->nil) {
        int cmp = tree->compare(key, current->data);
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            return current;  // Found
        }
    }
    
    return tree->nil;  // Not found
}
```

## Tree Traversal Algorithms

### Inorder Traversal (Sorted Order)
```
procedure inorder(node):
    if node ≠ NIL:
        inorder(node.left)
        visit(node)
        inorder(node.right)
```

### Preorder Traversal
```
procedure preorder(node):
    if node ≠ NIL:
        visit(node)
        preorder(node.left)
        preorder(node.right)
```

### Postorder Traversal
```
procedure postorder(node):
    if node ≠ NIL:
        postorder(node.left)
        postorder(node.right)
        visit(node)
```

## Complexity Analysis

### Time Complexities
- **Search**: O(log n) - balanced tree height
- **Insert**: O(log n) - search + O(1) rotations
- **Delete**: O(log n) - search + O(1) rotations
- **Min/Max**: O(log n) - tree height
- **Successor/Predecessor**: O(log n) - tree height
- **Traversal**: O(n) - visit each node once

### Space Complexity
- **Tree Storage**: O(n) - one node per element
- **Additional Space**: O(1) - single sentinel NIL node
- **Recursion Stack**: O(log n) - for recursive operations

### Height Guarantee
- **Maximum Height**: 2⌊log₂(n+1)⌋
- **Proof**: Comes from black-height property and red node constraints

## Invariant Validation

The `rb_is_valid()` function verifies:

1. **Root is black**
2. **Red nodes have black children**
3. **All paths have equal black-height**
4. **BST property maintained**

```c
bool validate_rb_properties(rb_node_t *node, int *black_height) {
    if (node == NIL) {
        *black_height = 1;
        return true;
    }
    
    // Check red property
    if (node->color == RED) {
        if (node->left->color == RED || node->right->color == RED) {
            return false;
        }
    }
    
    // Check black-height property recursively
    int left_bh, right_bh;
    if (!validate_rb_properties(node->left, &left_bh) ||
        !validate_rb_properties(node->right, &right_bh)) {
        return false;
    }
    
    if (left_bh != right_bh) {
        return false;
    }
    
    *black_height = left_bh + (node->color == BLACK ? 1 : 0);
    return true;
}
```

## Memory Management

### Sentinel NIL Node
- Single shared NIL node for all leaf positions
- Reduces memory overhead by ~50%
- Simplifies boundary condition checking
- Always colored BLACK

### Node Structure
```c
typedef struct rb_node {
    void *data;              // 8 bytes (64-bit)
    struct rb_node *left;    // 8 bytes
    struct rb_node *right;   // 8 bytes
    struct rb_node *parent;  // 8 bytes
    rb_color_t color;        // 4 bytes
} rb_node_t;                 // Total: ~40 bytes with padding
```

### Memory Allocation Strategy
- **Tree Creation**: Allocate tree structure + NIL sentinel
- **Node Insertion**: Allocate individual nodes as needed
- **Tree Destruction**: Post-order traversal to free all nodes
- **Data Management**: Controlled by user-provided free function