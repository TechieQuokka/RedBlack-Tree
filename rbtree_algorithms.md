# Red-Black Tree Implementation Strategy

## Core Algorithms Design

### 1. Tree Rotation Operations

#### Left Rotation
```
     x                y
    / \              / \
   α   y      =>    x   γ
      / \          / \
     β   γ        α   β
```

#### Right Rotation
```
     y                x
    / \              / \
   x   γ      =>    α   y
  / \                  / \
 α   β                β   γ
```

### 2. Insertion Algorithm

**Phase 1: Standard BST Insertion**
- Insert node as in regular BST
- Color new node RED
- Set parent pointers correctly

**Phase 2: Fix Red-Black Properties**
- Case 1: Uncle is RED → Recolor parent, uncle, grandparent
- Case 2: Uncle is BLACK, triangle → Rotate parent
- Case 3: Uncle is BLACK, line → Rotate grandparent + recolor

### 3. Deletion Algorithm

**Phase 1: Find Node to Delete**
- No children: Simple deletion
- One child: Replace with child
- Two children: Replace with successor/predecessor

**Phase 2: Fix Red-Black Properties (if deleted node was BLACK)**
- Case 1: Sibling is RED → Rotate + recolor, convert to other cases
- Case 2: Sibling BLACK, both nephews BLACK → Recolor sibling
- Case 3: Sibling BLACK, left nephew RED, right BLACK → Rotate sibling
- Case 4: Sibling BLACK, right nephew RED → Rotate parent + recolor

### 4. Memory Management Strategy

- Use single sentinel NIL node (shared by all leaves)
- NIL node is always BLACK
- Reduces memory overhead compared to individual NIL nodes
- Simplifies boundary condition handling

### 5. Complexity Analysis

- **Search**: O(log n) - balanced tree height
- **Insert**: O(log n) - search + constant fixup rotations
- **Delete**: O(log n) - search + constant fixup rotations
- **Space**: O(n) - one node per element + sentinel

### 6. Implementation Invariants

1. Root is always BLACK
2. NIL nodes are BLACK
3. RED nodes have BLACK children
4. All paths from root to leaves have same black-height
5. Tree maintains BST property: left < parent < right