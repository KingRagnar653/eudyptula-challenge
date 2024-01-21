#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

struct identity {
  char name[20];
  int id;
  bool busy;
  struct list_head mylist;
};

LIST_HEAD(mylinkedlist);

static int identity_create(char *name, int id) {
  struct identity *elem;

  elem = kmalloc(sizeof(struct identity), GFP_KERNEL);
  elem->id = id;
  strncpy(elem->name, name, strlen(name));

  list_add(&elem->mylist, &mylinkedlist);
  return 0;
}

struct identity *identity_find(int id) {
  struct identity *id_elem = NULL;
  list_for_each_entry(id_elem, &mylinkedlist, mylist) {
    if (id_elem->id == id) {
      return id_elem;
    }
  }
  return 0;
}

void identity_destroy(int id) {
  struct identity *id_elem = NULL;
  list_for_each_entry(id_elem, &mylinkedlist, mylist) {
    if (id_elem->id == id) {
      __list_del_entry(&id_elem->mylist);
      kfree(id_elem);
    }
  }
}

static int __init my_init(void) {
  struct identity *temp;

  identity_create("Alice", 1);
  identity_create("Bob", 2);
  identity_create("Dave", 3);
  identity_create("Gena", 10);

  temp = identity_find(3);
  pr_debug("id 3 = %s\n", temp->name);

  temp = identity_find(42);
  if (temp == NULL)
    pr_debug("id 42 not found\n");

  identity_destroy(2);
  identity_destroy(1);
  identity_destroy(10);
  identity_destroy(42);
  identity_destroy(3);

  return 0;
}

static void __exit my_exit(void) {
  pr_err("[LINK_LIST] exit module function called\n");
}

module_init(my_init);
module_exit(my_exit);