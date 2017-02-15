CSC 541 Advanced Data Stucture - Homework Project 1
---------------------------------------------------

Description:

A simple text editor which supports insert, delete, append and so on.
The program is based on heighted-Balanced Tree. And some code is adapted 
from the textbook: Peter Brass Advanced data structure. 

The idea is building a weight-balanced tree with the index of sentences
as key, and sentence as object. We use the first tree model from the 
textbook (p.24), which only leaves contain objects.

When there's a edit operation, we keep track of the path when we traverse 
the tree inside a stack. And after the operation, we can rebalance the 
tree by traverse back the path and applying tree rebalance operations 
(right, left rotations).

The time complexity of insertion, deletion, append should be O(lg(N)) 
since rebalance operations are constant time and the height-balanced 
tree's height is at most lg(N). Hence, each operation will at most 
traverse the tree twice. Time complexity is O(lg(N)).

Contibution:

We both spent some time writing and debugging in every function, so our 
contribution is even.

