// randomsearch.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "1.h"

#define PATH "1.txt"

void doubleShowTree(bt_node *tree)
{
	if (!tree)
		return;

	doubleShowTree(tree->l);
	printf("%lf\t", *((double *)tree->d));
	doubleShowTree(tree->r);
}

void doubleShowTreeDebug(bt_node *tree, int level, char* child)
{
	if (!tree)
		return;

	doubleShowTreeDebug(tree->l,level+1,"left");
	printf("%.1lf %i %s\n", *((double *)tree->d), level, child);
	doubleShowTreeDebug(tree->r,level+1,"right");
}

void doubleShowTreeDebugWithPriorities(bt_node *tree, int level, char* child)
{
	if (!tree)
		return;

	doubleShowTreeDebugWithPriorities(tree->l,level+1,"left");
	printf("%.1lf %i %s %.1lf\n", *((double *)tree->d), level, child,tree->priority);
	doubleShowTreeDebugWithPriorities(tree->r,level+1,"right");
}

void stringShowTree(bt_node *tree)
{
	if (!tree)
		return;

	stringShowTree(tree->l);
	printf("%s\n", (char *)tree->d);
	stringShowTree(tree->r);
}


void free(bt_node *tree)
{
	if (!tree)
		return;
}

int main(int argc, char* argv[])
{

	bt_node *tree=NULL;
	bt_driver *driver;
	srand(time(0));
	char *ch = (char *) calloc(100,sizeof(char));
	int c;
	int j = 0;

	driver = createStringDriver();
	FILE *fp = fopen(PATH,"r");

	if (fp==NULL){
		perror ("Unable to open file");
		return -1;
	}
    do {

      c = fgetc (fp);
	  if((c >47 && c < 58 ) || (c > 64 && c < 91)|| (c > 96 && c < 123) || (c > 127 && c < 176) || (c > 223 && c < 242) ||c == 45 || c == 39){
		  ch[j] = (char) c;
		  j++;
	  }
	  else{
		  if(j>0){
              bt_node *node = bt_find(&(ch[0]), &tree, driver);
			  if(node == NULL)
				externalInsert( ch, &tree, driver);
			
			  memset(ch,0,100);
			  
			  j=0;
		  }
	  }
    } while (c != EOF);
    fclose (fp);
	stringShowTree(tree);
    printf("Total word count: %i\n", bt_size(tree));
	bt_kill(tree,driver);
	getch();

	return 0;
}

