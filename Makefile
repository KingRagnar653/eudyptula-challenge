#comment this flag to use old _init _exit funcs
#EXTRA_CFLAGS += -IS_NEW_METHOD_USED

obj-m = hello_world.o helloworld_usb_driver.o helloworld_misc_driver.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean