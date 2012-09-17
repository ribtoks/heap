#include <stdio.h>

struct bt_node
{
	void *d;
	bt_node *l,*r, *p;
	double priority;
};

typedef void* (*f_datacreate)(void *);
typedef void (*f_datakill)(void *);
typedef int (*f_datacmp)(void*,void*);

struct bt_driver
{
	f_datacreate create;
	f_datakill kill;
	f_datacmp cmp;
};

 bt_driver* createDoubleDriver();
 bt_driver* createStringDriver();

void externalInsert(void *d, bt_node **tree, bt_driver *driver);
void bt_insert (void *d, double priority, bt_node **tree, bt_node *parent, bt_driver *driver);
void bt_remove (void *d, bt_node **tree, bt_driver *driver);
bt_node *bt_find (void *d, bt_node **tree, bt_driver *driver);
void bt_treverse (bt_node *tree, void (*action)(void*));
int bt_size (bt_node *tree);
void bt_kill (bt_node *tree, bt_driver *driver);

