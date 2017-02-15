/**
 * CSC541 Advanced data structure - Homework Porject 1
 *
 * A simplified text editor
 *
 * Arthor: Te I, Jason Chuang
 * Email: ti@ncsu.edu,
 *        tchuang3@ncsu.edu
 * Reference: Peter Brass Advanced data structures
 */
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define STACK_MAX 20000000
#define MAX_LINE_LENGTH 1024

// Tree node structure
typedef struct text_t {
  struct text_t *left;
  struct text_t *right;
  // total sentence
  int nodeCount;
  // Height
  int height;
} text_t;

// Stack used to trace path when traverse node
text_t * stack[STACK_MAX];
int stack_size = 0;

// Push text input stack
void push(text_t *text) {
  if (stack_size < STACK_MAX) {
    stack[stack_size++] = text;
  }
  else {
    fprintf(stderr, "Error: Exceed max stack.\n");
    exit(EXIT_FAILURE);
  }
}

// Pop from stack
text_t * pop() {
  if (stack_size > 0) {
    return stack[--stack_size];
  }
  else {
    fprintf(stderr, "Error: Nothing to pop.\n");
    exit(EXIT_FAILURE);
  }
}

// Create node
text_t * getNode() {
  text_t *tmp = (text_t *)malloc(sizeof(text_t));
  tmp->left = NULL;
  tmp->right = NULL;
  tmp->nodeCount = 0;
  tmp->height = 0;
  return tmp;
}

// Create Tree
// Reference: textbook p.26
text_t * create_text() {
  text_t *tmp = getNode();
  tmp->left = NULL;
  tmp->nodeCount = 0;
  tmp->height = 0;
  return tmp;
}

// Recursive find the height of the tree
int findHeight(text_t *txt) {
  if (txt->left == NULL || txt->right == NULL) {
    // Empty tree or leaf
    return 0;
  }
  else {
    int leftHeight = findHeight(txt->left);
    int rightHeight = findHeight(txt->right);
    return leftHeight < rightHeight ? rightHeight + 1 : leftHeight + 1;
  }
}

// Rotations to balanc the heights of the subtrees
// Reference: textbook p.27
void leftRotation(text_t *txt) {
  text_t *tmp_node = txt->left;
  txt->left = txt->right;
  txt->right = txt->left->right;
  txt->left->right = txt->left->left;
  txt->left->left = tmp_node;
  txt->left->nodeCount = txt->left->left->nodeCount + txt->left->right->nodeCount;
}

void rightRotation(text_t *txt) {
  text_t *tmp_node = txt->right;
  txt->right = txt->left;
  txt->left = txt->right->left;
  txt->right->left = txt->right->right;
  txt->right->right = tmp_node;
  txt->right->nodeCount = txt->right->left->nodeCount + txt->right->right->nodeCount;
}

// Rebalance tree
// After insert, delete, and append, we need to rebalance the tree.
void rebalance(text_t *tmp) {
  // Reference: textbook p.56
  int finished = 0;
  while (stack_size > 0 && !finished) {
    int tmp_height, old_height;
    tmp = pop();
    old_height = tmp->height;
    if ( tmp->left->height - tmp->right->height == 2) {
      // If right and left subtrees' height difference == 2
      if ( tmp->left->left->height - tmp->right->height == 1 ) {
        // left left is deeper, right rotate
        rightRotation( tmp );
        tmp->right->height = tmp->right->left->height + 1;
        tmp->height = tmp->right->height + 1;
      }
      else {
        // left right subtree is deeper, left rotate left subtree,
        // then right rotate tmp
        leftRotation( tmp->left );
        rightRotation( tmp );
        tmp_height = tmp->left->left->height; 
        tmp->left->height  = tmp_height + 1;
        tmp->right->height = tmp_height + 1;
        tmp->height = tmp->height + 2;
      }
    }
    // Same idea with deeper right subtree
    else if ( tmp->right->height - tmp->left->height == 2) {
      if (tmp->right->right->height - tmp->left->height == 1) {
        leftRotation( tmp );
        tmp->left->height = tmp->right->left->height + 1;
        tmp->height = tmp->left->height + 1;
      }
      else {
        rightRotation( tmp->right );
        leftRotation( tmp );
        tmp_height = tmp->right->right->height;
        tmp->left->height  = tmp_height + 1;
        tmp->right->height = tmp_height + 1;
        tmp->height = tmp_height + 2;
      }
    }
    else {
      tmp->height = (tmp->left->height > tmp->right->height) ? 
        tmp->left->height + 1 : tmp->right->height + 1;
    }
    if (tmp->height == old_height) {
      finished = 1;
    }
  }
}

char * get_line( text_t *txt, int index) {
  if (txt == NULL || txt->left == NULL) {
    // Empty tree
    return NULL;
  }
  text_t *tmp = txt;
  while (tmp->right != NULL) {
    // Traverse to leaf
    if (index <= tmp->left->nodeCount) {
      tmp = tmp->left;
    }
    else {
      index = index - (tmp->left->nodeCount);
      tmp = tmp->right;
    }
  }
  // Check key exists
  return (tmp->left == NULL) ? NULL : (char *)tmp->left;
}

void append_line( text_t *txt, char * new_line) {
  if (txt == NULL) {
    fprintf(stderr, "Append line before tree was created.\n");
    exit(EXIT_FAILURE);
  }
  if (txt->left == NULL) {
    // Empty tree
    txt->left = (text_t *)new_line;
    txt->nodeCount = 1;
    txt->right = NULL;
    txt->height = 0;
    return;
  }

  text_t *tmp = txt;
  while (tmp->right != NULL) {
    // Traverse down the tree
    push(tmp);
    tmp->nodeCount += 1;
    tmp = tmp->right;
  }
  // Create two new nodes.
  text_t *leftLeaf, *rightLeaf;
  leftLeaf  = getNode();
  rightLeaf = getNode();
  // Save original node's value and node count.
  leftLeaf->left = tmp->left; 
  leftLeaf->nodeCount = tmp->nodeCount;
  rightLeaf->left = (text_t *) new_line;
  rightLeaf->nodeCount = 1;
  tmp->left  = leftLeaf;
  tmp->right = rightLeaf;
  tmp->nodeCount = 2;
  tmp->height = 1;

  // Rebalance
  rebalance(tmp);
  // Reset stack.
  stack_size = 0;
}

char * set_line( text_t *txt, int index, char * new_line) {
  char *obj = (char *)malloc(sizeof(char*) * MAX_LINE_LENGTH);
  if (txt == NULL || txt->left == NULL) {
    // Empty tree
    return NULL;
  }
  else if (txt->right == NULL) {
    // Leaf
    return (char*)txt->left;
  }

  text_t *tmp = txt;
  while (tmp->right != NULL) {
    // Traverse down tree to find the key
    if (index <= tmp->left->nodeCount) {
      tmp = tmp->left;
    }
    else {
      index -= tmp->left->nodeCount;
      tmp = tmp->right;
    }
  }
  if (index == 1) {
    strcpy(obj, (char *)tmp->left);
    tmp->left = (text_t *) new_line;
    return obj;
  }
  // key not exists
  return NULL;
}

void insert_line( text_t * txt, int index, char * new_line) {
  // Tree has not been created
  if (txt == NULL) {
    fprintf(stderr, "Insert line into empty tree.");
    exit(EXIT_FAILURE);
  }

  // Empty tree
  if (txt->left == NULL) {
    txt->left = (text_t *) new_line;
    txt->nodeCount = 1;
    txt->right = NULL;
    txt->height = 0;
    return;
  }

  // Index larger than number of sentences
  if (txt->nodeCount < index) {
    append_line(txt, new_line);
    return;
  }

  // Only one leaf
  if (txt->nodeCount == 1) {
    text_t *leftLeaf, *rightLeaf;
    leftLeaf  = getNode();
    rightLeaf = getNode();
    leftLeaf->left = txt->left;
    leftLeaf->nodeCount = 1;
    rightLeaf->left = (text_t *) new_line;
    rightLeaf->nodeCount = 1;
    txt->left = leftLeaf;
    txt->right = rightLeaf;
    txt->nodeCount = 2;
    txt->height = 1;
    return;
  }

  // General case
  text_t *tmp = txt;
  while (tmp->right != NULL) {
    push(tmp);
    tmp->nodeCount += 1; // Insert line, plus 1
    if (index <= tmp->left->nodeCount) {
      tmp = tmp->left;
    }
    else {
      index -= tmp->left->nodeCount;
      tmp = tmp->right;
    }
  }
  // Insert text before the index
  text_t *leftLeaf, *rightLeaf;
  leftLeaf = getNode();
  rightLeaf = getNode();
  leftLeaf->left = (text_t *) new_line;
  leftLeaf->nodeCount = 1;
  rightLeaf->left = tmp->left;
  rightLeaf->nodeCount = 1;
  tmp->left = leftLeaf;
  tmp->right = rightLeaf;
  tmp->nodeCount = 2;
  tmp->height = 1;

  // Rebalance
  rebalance(tmp);
  // Reset stack.
  stack_size = 0;

}

char * delete_line( text_t *txt, int index) {
  // Tree not created
  if (txt == NULL) {
    fprintf(stderr, "delete before create tree.\n");
    exit(EXIT_FAILURE);
  }
  // Empty tree or index exceed txt length
  if (txt->left == NULL || index > txt->nodeCount) {
    return NULL;
  }

  // General case
  text_t *tmp, *upperNode, *otherNode;
  tmp = txt;
  while (tmp->right != NULL) {
    push(tmp);
    upperNode = tmp;
    if ( index <= tmp->left->nodeCount ) {
      otherNode = tmp->right;
      tmp = tmp->left;
    }
    else {
      index -= tmp->left->nodeCount;
      otherNode = tmp->left;
      tmp = tmp->right;
    }
  }
  pop();
  if (index == 1) {
    upperNode->left = otherNode->left;
    upperNode->right = otherNode->right;
    upperNode->height = otherNode->height;
    upperNode->nodeCount = otherNode->nodeCount;
    char* obj = (char *) tmp->left;
    
    // Reduce count in the path
    text_t *pathNode;
    int tmpSize = stack_size;
    while (stack_size > 0) {
      pathNode = pop();
      pathNode->nodeCount -= 1;
    }
    stack_size = tmpSize;

    // Rebalance 
    rebalance(tmp);
    // Reset stack.
    stack_size = 0;

    return obj;
  }
  else {
    stack_size = 0;
    return NULL;
  }   
}

// Return the total number of sentence
int length_text(text_t *txt) {
  if (txt == NULL) return -1;
  return txt->left == NULL ? 0 : txt->nodeCount;
}



