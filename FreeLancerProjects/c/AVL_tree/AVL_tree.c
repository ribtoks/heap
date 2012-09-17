#include "AVL_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitFactoryStruct(FuncFactory* factory, ConstructorFunc constructor,
		       DestructorFunc destructor,  CompareFunc comparator,
		       PrintFunc printer)
{
   factory->constructor = constructor;
   factory->compareFunc = comparator;
   factory->destructorFunc = destructor;
   factory->printFunc = printer;
}

void* StringConstructor(const void* data)
{
   const char* from = (const char*) data;
   char* str = 0;
  
   int n = strlen(from) + 1;
   str = (char*)malloc(n*sizeof(char));
   return strcpy(str, from);
}

int StringComparer(const void* line1, const void* line2)
{
   const char* str1 = (const char*) line1;
   const char* str2 = (const char*) line2;

   return strcmp(str1, str2);
}

void StringDestructor(void* data)
{
   free(data);
   data = 0;
}

void StringPrinter(const void* line)
{
   const char* str = (const char*)line;
   printf(str);
}

void InitStringFactoryStruct(FuncFactory* factory)
{
   InitFactoryStruct(factory,
		     StringConstructor,
		     StringDestructor,
		     StringComparer,
		     StringPrinter);
}

void* IntegerConstructor(const void* from)
{
   const int* data = (const int*)from;

   int* a = (int*)malloc(sizeof(int));

   (*a) = (*data);

   return a;
}

int IntegerComparer(const void* op1, const void* op2)
{
   const int* a = (const int*) op1;
   const int* b = (const int*) op2;

   return (*a) - (*b);
}

void IntegerDestructor(void* a)
{
   free(a);
   a = 0;
}

void IntegerPrinter(const void* data)
{
   const int* a = (const int*)data;
   printf("%d", (*a));
}


void InitIntegerFactoryStruct(FuncFactory* factory)
{
   InitFactoryStruct(factory,
		     IntegerConstructor,
		     IntegerDestructor,
		     IntegerComparer,
		     IntegerPrinter);
}



// ---------------------------------------------
// ------------ TreeNode realization -----------
// ---------------------------------------------

TreeNode* CreateTreeNode()
{
   return (TreeNode*)malloc(sizeof(TreeNode));
}

void DeleteTreeNode(TreeNode* node)
{
   free(node->value);
   node->value = 0;
   free(node);
   node = 0;
}

bool IsARightChild(TreeNode* node)
{
   if ((node->parent == 0) || (node->parent->right_child != node))
      return false;
   else
      return true;
}

bool IsALeftChild(TreeNode* node)
{
   if ((node->parent == 0) || (node->parent->left_child != node))
      return false;
   else
      return true;
}

bool IsALeaf(TreeNode* node)
{
   return ( (node->right_child == 0) && (node->left_child == 0) );
}

void PrintTreeNode(TreeNode* node, PrintFunc printer, const int level)
{
   const TreeNode* tn = node;
   int spaces = 0;

   if (tn != 0)
      PrintTreeNode(tn->right_child, printer, level + 1);

   for (spaces = 0; spaces < level; ++spaces)
      printf("       ");

   if (tn != 0)
   {
      printer(tn->value);
      printf("\n");
   }
   else
      printf("@\n");

   if (tn != 0)
      PrintTreeNode(tn->left_child, printer, level + 1);
}


void PrintTreeNode_InOrder(TreeNode* tn, PrintFunc printer)
{
   if (tn == 0)
      return;

   if (tn->left_child != 0)
      PrintTreeNode_InOrder(tn->left_child, printer);

   printf("[");
   printer(tn->value);

   if (tn->valuecount > 1)
      printf(" - %d", tn->valuecount);

   printf("]\n");

   if (tn->right_child != 0)
      PrintTreeNode_InOrder(tn->right_child, printer);
}

// ---------------------------------------------
// ------------ AVL tree realization -----------
// ---------------------------------------------


void MakeNewRoot(AVL_tree* tree, const void* value)
{
   TreeNode* root = 0;

   tree->root = CreateTreeNode();
   root = tree->root;
   tree->root->value = tree->functions->constructor(value);
   root->left_child = 0;
   root->right_child = 0;
   root->parent = 0;
   root->bal = 0;
   root->valuecount = 1;
   root->nodecount = 1;
}

TreeNode* FindNode(const AVL_tree* tree, const void* value)
{
   TreeNode* tn = tree->root;
   CompareFunc cmp = tree->functions->compareFunc;
   while ( (tn != 0) && ( cmp(tn->value, value) != 0 ))
   {
      if (cmp(tn->value, value) > 0)
	 tn = tn->left_child;
      else
	 tn = tn->right_child;
   }

   if (tn == 0)
      return (tree->zero_node);
   else
      if (IsALeftChild(tn) || IsARightChild(tn))
      {
	 return tn;
	 //if (IsALeftChild(tn))
	 //  return &(tn->parent->left_child);
	 //else
	 //  return &(tn->parent->right_child);
      }
      else 
	 if (tn == tree->root)
	    return (tree->root);
   return (tree->zero_node);
}

void AttachNode(AVL_tree* tree, TreeNode* parent, 
		TreeNode** child, const void* insertValue)
{
   //TreeNode* child = *child_ptr;
  
   (*child) = CreateTreeNode();

   //tree->functions->constructor(&((*child)->value), insertValue);
   (*child)->value = tree->functions->constructor(insertValue);
   (*child)->left_child = 0;
   (*child)->right_child = 0;
   (*child)->parent = parent;
   (*child)->nodecount = 1;
   (*child)->valuecount = 1;
   (*child)->bal = 0;
}

void AdjustNodeCountToRoot(AVL_tree* tree, TreeNode* tn, int incr)
{
   while (tn != 0)
   {
      tn->nodecount += incr;
      tn = tn->parent;
   }
}


void RotateRight(TreeNode** node)
{
   int leftcount = 0, rightcount = 0;
   TreeNode* tn = (*node)->left_child;
   (*node)->left_child = tn->right_child;
  
   if (tn->right_child)
      tn->right_child->parent = (*node);
  
   tn->right_child = (*node);
   tn->parent = (*node)->parent;
   (*node)->parent = tn;

   leftcount = (*node)->left_child ? 
      (*node)->left_child->nodecount : 0;
   rightcount = (*node)->right_child ?
      (*node)->right_child->nodecount : 0;

   (*node)->nodecount = leftcount + rightcount + 1;

   leftcount = tn->left_child ? tn->left_child->nodecount : 0;
   rightcount = tn->right_child ? tn->right_child->nodecount : 0;

   tn->nodecount = leftcount + rightcount + 1;
   (*node) = tn;
}

void RotateLeft(TreeNode** node)
{
   int leftcount = 0, rightcount = 0;
   TreeNode* tn = (*node)->right_child;
   (*node)->right_child = tn->left_child;
  
   if (tn->left_child)
      tn->left_child->parent = (*node);

   tn->left_child = (*node);
   tn->parent = (*node)->parent;
   (*node)->parent = tn;

   leftcount = (*node)->left_child ? 
      (*node)->left_child->nodecount : 0;
   rightcount = (*node)->right_child ? 
      (*node)->right_child->nodecount : 0;

   (*node)->nodecount = leftcount + rightcount + 1;

   leftcount = tn->left_child ? 
      tn->left_child->nodecount : 0;
   rightcount = tn->right_child ? 
      tn->right_child->nodecount : 0;

   tn->nodecount = leftcount + rightcount + 1;
  
   (*node) = tn;
}

void Cleanup(TreeNode* tn)
{
   if (tn == 0)
      return;

   if (tn->left_child != 0)
   {
      Cleanup(tn->left_child);
      tn->left_child = 0;
   }

   if (tn->right_child != 0)
   {
      Cleanup(tn->right_child);
      tn->right_child = 0;
   }

   DeleteTreeNode(tn);
}

void AdjustForAdd(AVL_tree* tree, TreeNode* new_child)
{
   bool rotate_flag = false;
   bool bal_was_changed = false;
   int new_child_orig_bal = 0;

   TreeNode* new_parent = new_child->parent;
   TreeNode* new_grandparent = new_parent->parent;
  
   ++(new_parent->nodecount);
  
   if (IsARightChild(new_child))
   {
      ++(new_parent->bal);
      bal_was_changed = true;
   }
   else
   {
      --(new_parent->bal);
      bal_was_changed = true;
   }

   while (new_grandparent != 0)
   {
      ++(new_grandparent->nodecount);

      if (IsARightChild(new_parent))
      {
	 if ((new_parent->bal != 0) && bal_was_changed)
	    ++(new_grandparent->bal);
	 else
	    bal_was_changed = false;
      }
      else
	 if (IsALeftChild(new_parent))
	 {
	    if ((new_parent->bal != 0) && bal_was_changed)
	       --(new_grandparent->bal);
	    else
	       bal_was_changed = false;
	 }

      if ((new_grandparent->bal < -1) || (new_grandparent->bal > 1))
      {
	 rotate_flag = true;
	 break;
      }

      new_child = new_parent;
      new_parent = new_grandparent;
      new_grandparent = new_grandparent->parent;
   }

   if (rotate_flag)
   {
      new_child_orig_bal = new_child->bal;

      if (IsALeftChild(new_parent) && IsALeftChild(new_child))
      {
	 if (IsALeftChild(new_grandparent))
	    RotateRight(&(new_grandparent->parent->left_child));
	 else
	    if (IsARightChild(new_grandparent))
	       RotateRight(&(new_grandparent->parent->right_child));
	    else
	       RotateRight(&(tree->root));

	 new_parent->bal = 0;
	 new_parent->right_child->bal = 0;
	 AdjustNodeCountToRoot(tree, new_parent->parent, 1);
      }
      else
	 if (IsARightChild(new_parent) && IsARightChild(new_child))
	 {
	    if (IsALeftChild(new_grandparent))
	       RotateLeft(&(new_grandparent->parent->left_child));
	    else
	       if (IsARightChild(new_grandparent))
		  RotateLeft(&(new_grandparent->parent->right_child));
	       else
		  RotateLeft(&(tree->root));

	    new_parent->bal = 0;
	    new_parent->left_child->bal = 0;
	    AdjustNodeCountToRoot(tree, new_parent->parent, 1);
	 }
	 else
	    if (IsALeftChild(new_parent) && IsARightChild(new_child))
	    {
	       RotateLeft(&(new_grandparent->left_child));

	       if (IsALeftChild(new_grandparent))
		  RotateRight(&(new_grandparent->parent->left_child));
	       else
		  if (IsARightChild(new_grandparent))
		     RotateRight(&(new_grandparent->parent->right_child));
		  else
		     RotateRight(&(tree->root));
	      
	       new_grandparent->parent->bal = 0;
	  
	       if (new_child_orig_bal == 0)
	       {
		  new_grandparent->bal = 0;
		  new_grandparent->parent->left_child->bal = 0;
	       }
	       else
		  if (new_child_orig_bal == -1)
		  {
		     new_grandparent->bal = 1;
		     new_grandparent->parent->left_child->bal = 0;
		  }
		  else
		     if (new_child_orig_bal == 1)
		     {
			new_grandparent->bal = 0;
			new_grandparent->parent->left_child->bal = -1;
		     }

	       AdjustNodeCountToRoot(tree, new_grandparent->parent->parent, 1);
	    }
	    else
	       if (IsARightChild(new_parent) && IsALeftChild(new_child))
	       {
		  RotateRight(&(new_grandparent->right_child));

		  if (IsALeftChild(new_grandparent))
		     RotateLeft(&(new_grandparent->parent->left_child));
		  else
		     if (IsARightChild(new_grandparent))
			RotateLeft(&(new_grandparent->parent->right_child));
		     else
			RotateLeft(&(tree->root));

		  new_grandparent->parent->bal = 0;

		  if (new_child_orig_bal == 0)
		  {
		     new_grandparent->bal = 0;
		     new_grandparent->parent->right_child->bal = 0;
		  }
		  else
		     if (new_child_orig_bal == -1)
		     {
			new_grandparent->bal = 0;
			new_grandparent->parent->right_child->bal = 1;
		     }
		     else
			if (new_child_orig_bal == 1)
			{
			   new_grandparent->bal = -1;
			   new_grandparent->parent->right_child->bal = 0;
			}
		
		  AdjustNodeCountToRoot(tree, new_grandparent->parent->parent, 1);
	       }
   }
}

TreeNode* InsertNode(AVL_tree* tree, const void* insert_value, TreeNode* start_node)
{
   TreeNode* tn = 0;
   CompareFunc cmp = 0;

   if (tree->root == 0)
   {
      MakeNewRoot(tree, insert_value);
      return tree->root;
   }

   if (start_node == 0)
      start_node = tree->root;

   tn = start_node;
   cmp = tree->functions->compareFunc;

   while ( (tn != 0) && (cmp(tn->value, insert_value) != 0) )
   {
      if (cmp(insert_value, tn->value) < 0)
      {
	 if (tn->left_child == 0)
	 {
	    AttachNode(tree, tn, &(tn->left_child), insert_value);
	    AdjustForAdd(tree, tn->left_child);
	    return tn->left_child;
	 }
	 else
	    tn = tn->left_child;	
      }
      else
      {
	 if (tn->right_child == 0)
	 {
	    AttachNode(tree, tn, &(tn->right_child), insert_value);
	    AdjustForAdd(tree, tn->right_child);
	    return tn->right_child;
	 }
	 else
	    tn = tn->right_child;
      }
   }
   // my code for incrementing value count...
   ++(tn->valuecount);
   return 0;
}

TreeNode* RotateForRemove(AVL_tree* tree, TreeNode* tn, int delta_balance)
{
   TreeNode* resume_iteration = 0;
   TreeNode* tnp = tn->parent;

   // if (IsALeftChild(tn))
   if (delta_balance > 0)
   {
      resume_iteration = tn->right_child;

      if ((tn->bal == 2) &&
	  ( (tn->right_child->bal == 1) || (tn->right_child->bal == 0) ))
      {
	 if (tn->right_child->bal == 1)
	 {
	    tn->bal = 0;
	    tn->right_child->bal = 0;
	 }
	 else	    
	 {
	    tn->bal = 1;
	    tn->right_child->bal = -1;
	 }

	 if (IsALeftChild(tn))
	    RotateLeft(&(tn->parent->left_child));
	 else
	    if (IsARightChild(tn))
	       RotateLeft(&(tn->parent->right_child));
	    else
	       RotateLeft(&(tree->root));

	 return resume_iteration;
      }
      else
	 if ( (tn->bal == 2) && (tn->right_child->bal == -1) )
	 {
	    if (tn->right_child->left_child->bal == 1)
	    {
	       tn->bal = -1;
	       tn->right_child->bal = 0;
	       tn->right_child->left_child->bal = 0;
	    }
	    else
	       if (tn->right_child->left_child->bal == -1)
	       {
		  tn->bal = 0;
		  tn->right_child->bal = 1;
		  tn->right_child->left_child->bal = 0;
	       }
	       else
	       {
		  tn->bal = 0;
		  tn->right_child->bal = 0;
		  tn->right_child->left_child->bal = 0;
	       }

	    resume_iteration = resume_iteration->left_child;

	    RotateRight(&(tn->right_child));

	    if (IsALeftChild(tn))
	       RotateLeft(&(tn->parent->left_child));
	    else
	       if (IsARightChild(tn))
		  RotateLeft(&(tn->parent->right_child));
	       else
		  RotateLeft(&(tree->root));

	    return resume_iteration;
	 }
   }
   // else if (IsARightChild(tn))
   else
   {
      resume_iteration = tn->left_child;

      if ((tn->bal == -2) &&
	  ( (tn->left_child->bal == -1) || (tn->left_child->bal == 0) ))
      {
	 if (tn->left_child->bal == -1)
	 {
	    tn->bal = 0;
	    tn->left_child->bal = 0;
	 }
	 else	    
	 {
	    tn->bal = -1;
	    tn->left_child->bal = 1;
	 }

	 if (IsALeftChild(tn))
	    RotateRight(&(tn->parent->left_child));
	 else
	    if (IsARightChild(tn))
	       RotateRight(&(tn->parent->right_child));
	    else
	       RotateRight(&(tree->root));

	 return resume_iteration;
      }
      else
	 if ( (tn->bal == -2) && (tn->left_child->bal == 1) )
	 {
	    if (tn->left_child->right_child->bal == -1)
	    {
	       tn->bal = 1;
	       tn->left_child->bal = 0;
	       tn->left_child->right_child->bal = 0;
	    }
	    else
	       if (tn->left_child->right_child->bal == 1)
	       {
		  tn->bal = 0;
		  tn->left_child->bal = -1;
		  tn->left_child->right_child->bal = 0;
	       }
	       else
	       {
		  tn->bal = 0;
		  tn->left_child->bal = 0;
		  tn->left_child->right_child->bal = 0;
	       }

	    resume_iteration = resume_iteration->right_child;

	    RotateRight(&(tn->left_child));

	    if (IsALeftChild(tn))
	       RotateRight(&(tn->parent->left_child));
	    else
	       if (IsARightChild(tn))
		  RotateRight(&(tn->parent->right_child));
	       else
		  RotateRight(&(tree->root));

	    return resume_iteration;
	 }
   }

   return tree->zero_node;
}

void AdjustForRemove(AVL_tree* tree, TreeNode* start_node, int delta_balance)
{
   TreeNode* tn = start_node;
   TreeNode* tnp = 0;
  
   int tn_bal_orig = tn->bal;
   int absdelta = 0;

   if ( (tn->bal == -2) || (tn->bal ==2) )
      tn = RotateForRemove(tree, tn, delta_balance);

   delta_balance = tn_bal_orig - tn->bal;
   absdelta = (delta_balance > 0) ? delta_balance : -delta_balance;

   tnp = tn->parent;

   while (tnp != 0)
   {
      --(tnp->nodecount);

      if (tn->bal == 0)
      {
	 if (IsARightChild(tn))
	 {
	    --(tnp->bal);
	    delta_balance = -1;
	 }
	 else
	 {
	    ++(tnp->bal);
	    delta_balance = 1;
	 }
      }
      else
      {
	 AdjustNodeCountToRoot(tree, tnp->parent, -1);
	 return;
      }

      if ((tnp->bal > -2) && (tnp->bal < 2) )
      {
	 tn = tnp;
	 tnp = tn->parent;
	 continue;
      }

      tn = RotateForRemove(tree, tnp, delta_balance);
      tnp = tn->parent;
   }
}

// --------------------------------------------------
// ------------ AVL tree public functions -----------
// --------------------------------------------------

bool Add(AVL_tree* tree, const void* value)
{
   TreeNode* ret = InsertNode(tree, value, 0);
   if (ret)
      return true;
   return false;
}


void Remove(AVL_tree* tree, const void* value)
{
   TreeNode* node_to_remove = FindNode(tree, value);

   TreeNode* predecessor = 0;
   TreeNode* temp = 0;

   TreeNode* pp = 0;

   int delta_balance = 0;


   if (node_to_remove->valuecount > 1)
   {
      --(node_to_remove->valuecount);
      return;
   }

   if (node_to_remove == tree->zero_node)
      return;

   if (node_to_remove->left_child == 0)
   {
      temp = node_to_remove;

      if (temp->parent)
      {
	 --(temp->parent->nodecount);

	 if (IsALeftChild(temp))
	 {
	    ++(temp->parent->bal);
	    delta_balance = 1;
	 }
	 else
	    if (IsARightChild(temp))
	    {
	       --(temp->parent->bal);
	       delta_balance = -1;
	    }
      }

      if (node_to_remove->right_child != 0)
      {
	 node_to_remove->right_child->parent = node_to_remove->parent;
	 
	 if (IsALeftChild(node_to_remove))
	    node_to_remove->parent->left_child = node_to_remove->right_child;
	 else
	    if (IsARightChild(node_to_remove))
	       node_to_remove->parent->right_child = node_to_remove->right_child;

      }

      node_to_remove = node_to_remove->right_child;

      AdjustForRemove(tree, temp->parent, delta_balance);

      if (IsALeaf(temp))
	 if (temp->parent)
	 {
	    pp = temp->parent;
	    if (IsALeftChild(temp))
	       pp->left_child = 0;
	    else
	       if (IsARightChild(temp))
		  pp->right_child = 0;
	 }

      DeleteTreeNode(temp);

      return;
   }
   else
      if ( node_to_remove->right_child == 0 )
      {
	 temp = node_to_remove;

	 if (temp->parent)
	 {
	    --(temp->parent->nodecount);
	    if (IsALeftChild(temp))
	    {
	       ++(temp->parent->bal);
	       delta_balance = 1;
	    }
	    else
	       if (IsARightChild(temp))
	       {
		  --(temp->parent->bal);
		  delta_balance = -1;
	       }
	 }

	 node_to_remove->left_child->parent = node_to_remove->parent;
	 
	 if (IsALeftChild(node_to_remove))
	    node_to_remove->parent->left_child = node_to_remove->left_child;
	 else
	    if (IsARightChild(node_to_remove))
	       node_to_remove->parent->right_child = node_to_remove->left_child;

	 AdjustForRemove(tree, temp->parent, delta_balance);

	 if (IsALeaf(temp))
	    if (temp->parent)
	    {
	       pp = temp->parent;
	       if (IsALeftChild(temp))
		  pp->left_child = 0;
	       else
		  if (IsARightChild(temp))
		     pp->right_child = 0;
	    }

	 DeleteTreeNode(temp);

	 return;
      }

   predecessor = node_to_remove->left_child;

   while (predecessor->right_child != 0)
      predecessor = predecessor->right_child;

   tree->functions->destructorFunc(node_to_remove->value);
   node_to_remove->value = tree->functions->constructor(predecessor->value);
   node_to_remove->valuecount = predecessor->valuecount;

   pp = predecessor->parent;

   if (pp == node_to_remove)
   {
      pp->left_child = predecessor->left_child;
      ++(pp->bal);
      delta_balance = 1;

      if (predecessor->left_child != 0)
	 predecessor->left_child->parent = pp;
   }
   else
      if (predecessor->left_child != 0)
      {
	 pp->right_child = predecessor->left_child;
	 predecessor->left_child->parent = pp;
	 --(pp->bal);
	 delta_balance = -1;
      }
      else
      {
	 pp->right_child = 0;
	 --(pp->bal);
	 delta_balance = -1;
      }

   --(pp->nodecount);
   AdjustForRemove(tree, pp, delta_balance);

   DeleteTreeNode(predecessor);
}


int Size(const AVL_tree* tree)
{
   return (tree->root) ? tree->root->nodecount : 0;
}

int Find(const AVL_tree* tree, const void* value)
{
   TreeNode* tn = FindNode(tree, value);
   if (tn != 0)
      return tn->valuecount;
   return -1;
}

void PrintTree(const AVL_tree* tree)
{
   PrintTreeNode(tree->root, tree->functions->printFunc, 0);
}

void Print_InOrder(const AVL_tree* tree)
{
   PrintTreeNode_InOrder(tree->root, tree->functions->printFunc);
}

void CleanupTree(AVL_tree* tree)
{
   Cleanup(tree->root);
   free(tree);
   tree = 0;
}

AVL_tree* CreateAVLTree(FuncFactory* functions)
{
   AVL_tree* tree = (AVL_tree*)malloc(sizeof(AVL_tree));

   tree->functions = functions;
   tree->root = 0;
   tree->zero_node = 0;

   return tree;
}

int NodeElementCount(const TreeNode* node)
{
   int count = 0;

   if (node == 0)
      return 0;
   
   if (node->left_child)
      count = count + NodeElementCount(node->left_child);

   if (node->right_child)
      count = count + NodeElementCount(node->right_child);

   count = count + node->valuecount;

   return count;
}

int ElementCount(const AVL_tree* tree)
{
   return NodeElementCount(tree->root);
}
