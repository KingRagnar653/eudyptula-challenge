// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/init.h>   /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Rahul RS");
///< The description -- see modinfo
MODULE_DESCRIPTION("Hello World");
///< The version of the module
MODULE_VERSION("0.1");

static int __init hello_start(void)
{
	pr_info("Hello world\n");
	return 0;
}

static void __exit hello_end(void) { pr_info("Goodbye\n"); }

module_init(hello_start);
module_exit(hello_end);
