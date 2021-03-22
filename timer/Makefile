KERNELDIR := /usr/src/linux-headers-4.15.0-139-generic
CURRENT_PATH := $(shell pwd)
obj-m := timer.o
build: kernel_modules
kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
