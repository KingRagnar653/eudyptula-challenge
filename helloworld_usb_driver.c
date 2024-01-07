// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/hid.h>
#include <linux/init.h>   /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */
#include <linux/usb.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Rahul RS");
///< The description -- see modinfo
MODULE_DESCRIPTION("usb driver hello world");
///< The version of the module
MODULE_VERSION("0.1");
#define PRINT_USB_INTERFACE_DESCRIPTOR(i)                                      \
	{                                                                      \
		pr_info("USB_INTERFACE_DESCRIPTOR:\n");                        \
		pr_info("-----------------------------\n");                    \
		pr_info("bLength: 0x%x\n", i.bLength);                         \
		pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType);         \
		pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber);       \
		pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting);     \
		pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints);             \
		pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass);         \
		pr_info("bInterfaceSubClass: 0x%x\n", i.bInterfaceSubClass);   \
		pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);   \
		pr_info("iInterface: 0x%x\n", i.iInterface);                   \
		pr_info("\n");                                                 \
	}
#define PRINT_USB_ENDPOINT_DESCRIPTOR(e)                                       \
	{                                                                      \
		pr_info("USB_ENDPOINT_DESCRIPTOR:\n");                         \
		pr_info("------------------------\n");                         \
		pr_info("bLength: 0x%x\n", e.bLength);                         \
		pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);         \
		pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);       \
		pr_info("bmAttributes: 0x%x\n", e.bmAttributes);               \
		pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);           \
		pr_info("bInterval: 0x%x\n", e.bInterval);                     \
		pr_info("\n");                                                 \
	}
/*
 * this function called when usb device is inserted
 */
static int hw_usb_probe(struct usb_interface *interface,
			const struct usb_device_id *id)
{
	unsigned int i;
	unsigned int endpoint_count;
	struct usb_host_interface *iface_desc = interface->cur_altsetting;

	dev_info(&interface->dev,
		 "USB driver probed: Vendorid : 0x%02x , Productid : 0x%02x\n",
		 id->idVendor, id->idProduct);

	endpoint_count = iface_desc->desc.bNumEndpoints;

	PRINT_USB_INTERFACE_DESCRIPTOR(iface_desc->desc);
	for (i = 0; i < endpoint_count; i++) {
		PRINT_USB_ENDPOINT_DESCRIPTOR(iface_desc->endpoint[i].desc);
	}

	return 0;
}
/*
 *functiona called when usb device disconnected
 */
static void hw_usb_disconnect(struct usb_interface *interface)
{
	dev_info(&interface->dev, "USB device disconnected\n");
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

#ifdef IS_NEW_METHOD_USED
/*
 * this replace module_init and module_exit
 */
module_usb_driver(hw_usb_driver);
#else
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
#endif /* IS_NEW_METHOD */
