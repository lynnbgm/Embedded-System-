ifneq ($(KERNELRELEASE),)
	obj-m := mytimer.o
else
	KERNELDIR := $(EC535)/gumstix/linux-2.6.21gum/
	PWD := $(shell pwd)
	ARCH := arm
	CROSS := arm-linux-

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=$(ARCH) clean
	rm Module.symvers 
endif
