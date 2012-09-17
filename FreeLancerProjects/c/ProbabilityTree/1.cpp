#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <string.h>
#include "1.h"


void rotateRight(bt_node *y)
{
	bt_node *x = y->l;
    bt_node *p = y->p;
    y->l = x->r;
  if (y->l != NULL)
	  y->l->p = y;

  if(p != NULL){
	  if (p->r == y)
		  p->r = x;
	  else
		  p->l = x; 
  }

  x->p = p;
  x->r = y;
  y->p = x;
}

void rotateLeft(bt_node *x)
{
  bt_node *y = x->r;
  bt_node *p = x->p;
  x->r = y->l;
  if (x->r != NULL)
    x->r->p = x;

  if(p != NULL){
	  if (p->l == x)
		p->l = y;
	  else
		p->r = y;
  }
  y->p = p;
  y->l = x;
  x->p = y;
}

void bubbleUp(bt_node *tree)
{
	bt_node *p = tree->p;
	if(p == NULL)
		return;

	if (tree->priority < p->priority ) {
      if (p->l == tree)
		  rotateRight(tree->p);
      else
		  rotateLeft(tree->p);

       bubbleUp(tree);
    }
}

void bubbleDown(bt_node *tree)
{
	double lcPriority = tree->l != NULL ? tree->l->priority : 2.0;
    double rcPriority = tree->r != NULL ? tree->r->priority : 2.0;
    double minPriority = (lcPriority < rcPriority) ? lcPriority : rcPriority;
	if (tree->priority <= minPriority)
      return;

    if (lcPriority < rcPriority)
      rotateRight(tree);
    else
      rotateLeft(tree);

    bubbleDown(tree);
}

void externalInsert(void *d, bt_node **tree, bt_driver *driver){

	double priority = (double)rand()/RAND_MAX;

    bt_insert (d, priority, tree, NULL, driver);
	while((*tree)->p != NULL){
		*tree = (*tree)->p;
	}
}

void bt_insert (void *d, double priority, bt_node **tree, bt_node *parent, bt_driver *driver)
{
	if (*tree==NULL)
	{
		*tree=(bt_node*)malloc(sizeof(bt_node));
		(*tree)->l=(*tree)->r=NULL;
		(*tree)->p = parent;
		(*tree)->d=(*driver->create)(d);
		(*tree)->priority = priority;
		bubbleUp(*tree);
		return;
	}
	if((*driver->cmp)(d, (*tree)->d)<0){
		bt_insert(d, priority,&(*tree)->l, *tree, driver);
	}
	else{
		bt_insert(d, priority,&(*tree)->r, *tree, driver);
	}
}

void bt_remove (void *d, bt_node **tree, bt_driver *driver)
{
int a=(*driver->cmp)(d,(*tree)->d);
	if (*tree==NULL) 
		return;
	if (a==0)
	{
		(*tree)->priority = 1.5;
		bubbleDown(*tree);
		bt_node *p = (*tree)->p;
		if(p->l == (*tree))
			p->l = NULL;
		else
			p->r = NULL;

		(*driver->kill)((*tree)->d);
		free(*tree);
	}
	else
		bt_remove(d, a<0?&(*tree)->l:&(*tree)->r, driver);
}



bt_node *bt_find (void *d, bt_node **tree, bt_driver *driver)
{
	if (*tree==NULL)
		return NULL;
	int b=(*driver->cmp)(d,(*tree)->d);
	if (b==0)
		return *tree;
	else
		return bt_find(d, b<0?&(*tree)->l:&(*tree)->r, driver);
}

void bt_sizecount (bt_node *tree, int *c)
{
	if (tree==NULL) return;
	(*c)++;
	bt_sizecount(tree->l, c); 
	bt_sizecount(tree->r, c);
}

int bt_size(bt_node *tree)
{
	int c=0;;
	bt_sizecount(tree, &c);
	return c;
}

void bt_kill (bt_node *tree, bt_driver *driver)
{
	if (tree==NULL) return;
	bt_kill(tree->l, driver); 
	bt_kill(tree->r, driver);
	(*driver->kill)(tree->d);
	free (tree);
}

void bt_treverse (bt_node *tree, void (*action)(void*))
{
	if (tree==NULL) return;
	bt_treverse (tree->l, action);
	(*action)(tree->d);
	bt_treverse(tree->r, action);
	(*action)(tree->d);
}

void* doubleCreate(void *data){
	 return data;
 }

 void doubleKill(void *data){
 }

 int doubleComp(void *data1,void* data2){
	 double left = data1 == NULL?  DBL_MIN : *((double *)data1);
	 double right = data2 == NULL?  DBL_MIN : *((double *)data2);
	 return right>left ? -1 : right == left ? 0 : 1;
 }

 bt_driver* createDoubleDriver(){
	 bt_driver *driver = (bt_driver *) malloc(sizeof(bt_driver));	
	 driver->cmp=doubleComp;
	 driver->create = doubleCreate;
	 driver->kill = doubleKill;
	 return driver;
 }
 
 void* stringCreate(void *src){
	 char * source = (char *) src;
	 char * data;
	 int len;
	 if(source == NULL)
		 return NULL;

	 len = strlen(source);
     data = (char *)calloc(len+1,sizeof(char));
	 strcpy(data,source);
	 return data;
 }

 void stringKill(void *data){
	 free(data);
 }

 int stringComp(void *data1,void* data2){
	 char * left = data1 == NULL?  "" : (char *)data1;
	 char * right = data2 == NULL?  "" : (char *)data2;
	 return strcmp(left,right);
 }


 bt_driver* createStringDriver(){
	 bt_driver *driver = (bt_driver *) malloc(sizeof(bt_driver));	
	 driver->cmp=stringComp;
	 driver->create = stringCreate;
	 driver->kill = stringKill;
	 return driver;
 }