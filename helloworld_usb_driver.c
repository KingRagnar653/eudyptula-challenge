// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/init.h>   /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */
#include <linux/usb.h>
#include <linux/hid.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Rahul RS");
///< The description -- see modinfo
MODULE_DESCRIPTION("usb driver hello world");
///< The version of the module
MODULE_VERSION("0.1");
/*
 * this function called when usb device is inserted
 */
static int hw_usb_probe(struct usb_interface *inteface, const struct usb_device_id *id)
{
	pr_alert("USB Keyboard plugged in\n");
	return 0;
}
/*
 *functiona called when usb device disconnected
 */
static void hw_usb_disconnect(struct usb_interface *interface)
{
	pr_alert("USB Keyboard disconnected\n");
}
/*
 * list of usb device id driver supports
 */
static const struct usb_device_id id_table[] = {
    {USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD)},
    {},
};
MODULE_DEVICE_TABLE(usb, id_table);
/*
 * usb_driver struct to communicate with usb core
 */
static struct usb_driver hw_usb_driver = {
    .name = "HelloWorld usb driver",
    .probe = hw_usb_probe,
    .disconnect = hw_usb_disconnect,
    .id_table = id_table,
};

static int __init hello_start(void)
{
	pr_debug("Hello world\n");
	return usb_register(&hw_usb_driver);
}

static void __exit hello_end(void)
{
	pr_debug("Goodbye\n");
	usb_deregister(&hw_usb_driver);
}

module_init(hello_start);
module_exit(hello_end);
