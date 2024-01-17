// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h> /* Needed for the macros */
#include <linux/jiffies.h>
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/kobject.h>
#include <linux/module.h> /* Needed by all modules */
#include <linux/poll.h>
#include <linux/rwlock.h>
#include <linux/slab.h>
#include <linux/string.h>

#define MY_ID "12345678"
#define MY_ID_LEN 9

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Rahul RS");
///< The description -- see modinfo
MODULE_DESCRIPTION("Hello World");
///< The version of the module
MODULE_VERSION("0.1");

static struct kobject *eud_kobj;
static char input[PAGE_SIZE];
static rwlock_t memlock = __RW_LOCK_UNLOCKED(memlock);

/*
** This function will be called when we write the Misc Device file
*/
static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	char *my_str = MY_ID;
	char input[MY_ID_LEN];

	strncpy(input, buf, count);
	if (strncmp(input, my_str, MY_ID_LEN - 1))
		return -EINVAL;
	return count;
}

/*
** This function will be called when we read the Misc Device file
*/
static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	ssize_t retval = 0;
	char *my_str = MY_ID;

	strncpy(buf, my_str, MY_ID_LEN);
	retval += MY_ID_LEN;
	return retval;
}

/*
** This function will be called when we write the Misc Device file
*/
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	ssize_t retval = 0;
	unsigned long flags;

	write_lock_irqsave(&memlock, flags);

	retval = snprintf(input, PAGE_SIZE, "%s\n", buf);
	write_unlock_irqrestore(&memlock, flags);
	return retval;
}

/*
** This function will be called when we read the Misc Device file
*/
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	ssize_t retval = 0;
	unsigned long flags;

	read_lock_irqsave(&memlock, flags);

	retval = snprintf(buf, PAGE_SIZE, "%s\n", input);

	read_unlock_irqrestore(&memlock, flags);
	return retval;
}

static ssize_t jiff_show(struct kobject *kobj, struct kobj_attribute *attr,
			 char *buf)
{
	ssize_t retval = 0;
	char jiff_char[20];

	snprintf(jiff_char, sizeof(jiff_char), "%lu", jiffies);
	strncpy(buf, jiff_char, sizeof(jiff_char));

	retval += sizeof(jiff_char);
	return retval;
}

static struct kobj_attribute id_attr = __ATTR_RW(id);
static struct kobj_attribute jiff_attr = __ATTR_RO(jiff);
static struct kobj_attribute foo_attr = __ATTR(foo, 0644, foo_show, foo_store);

static int __init hello_start(void)
{
	int result;
	/*
	 * create sysfs sub dir eudyptula
	 */
	eud_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!eud_kobj) {
		pr_err("[SYS_ERROR] failed to create and add "
		       "kobject\n");
		return -ENOMEM;
	}
	/**
	 * create a new file in eudyptula dir with same read and write logic as
	 * task_06
	 */
	result = sysfs_create_file(eud_kobj, &id_attr.attr);
	if (result) {
		pr_err("[SYS_ERROR] failed to create id file\n");
		kobject_put(eud_kobj);
		eud_kobj = NULL;
		return result;
	}
	/**
	 * create file jiffies readonly gives current kernel jiffies value
	 */
	result = sysfs_create_file(eud_kobj, &jiff_attr.attr);
	if (result) {
		pr_err("[SYS_ERROR] failed to create jiff file\n");
		kobject_put(eud_kobj);
		eud_kobj = NULL;
		return result;
	}
	/*
	 * create  file readonly except root user foo
	 */
	result = sysfs_create_file(eud_kobj, &foo_attr.attr);
	if (result) {
		pr_err("[SYS_ERROR] failed to create foo file\n");
		kobject_put(eud_kobj);
		eud_kobj = NULL;
		return result;
	}
	return 0;
}

static void __exit hello_end(void)
{
	if (eud_kobj) {
		sysfs_remove_file(eud_kobj, &id_attr.attr);
		sysfs_remove_file(eud_kobj, &foo_attr.attr);
		sysfs_remove_file(eud_kobj, &jiff_attr.attr);
		kobject_put(eud_kobj);
	} else {
		printk("[SYS_ERROR] exit kobject doesnt exist\n");
	}
}

module_init(hello_start);
module_exit(hello_end);
