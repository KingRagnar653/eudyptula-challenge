// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h> /* Needed for the macros */
#include <linux/jiffies.h>
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */
#include <linux/poll.h>
#include <linux/rwlock.h>
#include <linux/slab.h>

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

struct dentry *dentry;
static char input[PAGE_SIZE];
static rwlock_t memlock = __RW_LOCK_UNLOCKED(memlock);

/*
** This function will be called when we write the Misc Device file
*/
static ssize_t id_write(struct file *file, const char __user *buf, size_t len,
			loff_t *ppos)
{
	char *my_str = MY_ID;
	char input[MY_ID_LEN];

	if ((len != MY_ID_LEN) || (copy_from_user(input, buf, MY_ID_LEN)) ||
	    (strncmp(input, my_str, MY_ID_LEN - 1)))
		return -EINVAL;
	else
		return len;
}

/*
** This function will be called when we read the Misc Device file
*/
static ssize_t id_read(struct file *filp, char __user *buf, size_t count,
		       loff_t *f_pos)
{
	char *my_str = MY_ID;
	if (*f_pos != 0)
		return 0;
	if ((count < MY_ID_LEN) || (copy_to_user(buf, my_str, MY_ID_LEN)))
		return -EINVAL;
	*f_pos += count;
	return count;
}

static const struct file_operations id_fops = {
    .owner = THIS_MODULE, .read = id_read, .write = id_write};
/*
** This function will be called when we write the Misc Device file
*/
static ssize_t foo_write(struct file *file, const char __user *buf, size_t len,
			 loff_t *ppos)
{
	ssize_t retval = 0;
	unsigned long flags;

	if ((*ppos) >= PAGE_SIZE) {
		printk("writing past the page size\n");
		return -ENOSPC;
	}
	write_lock_irqsave(&memlock, flags);
	if (copy_from_user(input, buf, sizeof(buf))) {
		pr_alert("Error in copy from user func()");
		retval = -EFAULT;
		goto out;
	}
	retval += sizeof(buf);
	(*ppos) += sizeof(buf);
	pr_alert("[DEBUGFS MODULE] Write operation succeeded !!\n");
out:
	write_unlock_irqrestore(&memlock, flags);
	return retval;
}

/*
** This function will be called when we read the Misc Device file
*/
static ssize_t foo_read(struct file *filp, char __user *buf, size_t count,
			loff_t *f_pos)
{
	ssize_t retval = 0;
	unsigned long flags;

	if ((*f_pos) >= PAGE_SIZE) {
		printk("reading past the page size\n");
		return 0;
	}
	read_lock_irqsave(&memlock, flags);

	if (copy_to_user(buf, input, sizeof(input))) {
		pr_alert("Error in copy to user func()");
		retval = -EFAULT;
		goto out;
	}

	(*f_pos) += sizeof(input);
	retval += sizeof(input);
out:
	read_unlock_irqrestore(&memlock, flags);
	pr_alert("[DEBUGFS MODULE] Read operation succeeded !!\n");
	return retval;
}

static const struct file_operations foo_fops = {
    .owner = THIS_MODULE, .read = foo_read, .write = foo_write};

static int __init hello_start(void)
{
	unsigned long t = jiffies;
	/*
	 * create debug sub dir eudyptula /sys/kernel/debug/eudyptula
	 */
	dentry = debugfs_create_dir("eudyptula", NULL);
	/**
	 * create a new file in eudyptula dir with same read and write logic as
	 * task_06
	 */
	if (!(debugfs_create_file("id", 0666, dentry, NULL, &id_fops)))
		return -ENODEV;
	/**
	 * create file jiffies readonly gives current kernel jiffies value
	 */
	if (!(debugfs_create_u32("jiffies", 0444, dentry, (u32 *)&t)))
		return -ENODEV;
	/*
	 * create debugfs file readonly except root user foo
	 */
	if (!(debugfs_create_file("foo", 0644, dentry, NULL, &foo_fops)))
		return -ENODEV;
	return 0;
}

static void __exit hello_end(void) { debugfs_remove_recursive(dentry); }

module_init(hello_start);
module_exit(hello_end);
