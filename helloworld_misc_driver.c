// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/fs.h>
#include <linux/init.h>   /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/miscdevice.h>
#include <linux/module.h> /* Needed by all modules */

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Rahul RS");
///< The description -- see modinfo
MODULE_DESCRIPTION("Hello World");
///< The version of the module
MODULE_VERSION("0.1");
/*
** This function will be called when we write the Misc Device file
*/
static ssize_t hw_misc_write(struct file *file, const char __user *buf,
			     size_t len, loff_t *ppos)
{
	pr_info("Hello World misc device write\n");

	/* We are not doing anything with this data now */

	return len;
}

/*
** This function will be called when we read the Misc Device file
*/
static ssize_t hw_misc_read(struct file *filp, char __user *buf, size_t count,
			    loff_t *f_pos)
{
	pr_info("Hello World misc device read\n");

	return 0;
}
/*
 * file operation structure misc
 */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = hw_misc_read,
    .write = hw_misc_write,
    .llseek = no_llseek,
};
/*
 * misc device structure
 */
struct miscdevice hw_misc_driver = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "hw_misc_driver",
    .fops = &fops,
};

static int __init hello_start(void)
{
	pr_info("register hw misc driver\n");
	misc_register(&hw_misc_driver);
	return 0;
}

static void __exit hello_end(void)
{
	pr_info("de-register hw misc driver\n");
	misc_deregister(&hw_misc_driver);
}

module_init(hello_start);
module_exit(hello_end);
