KVERS = $(shell uname -r)
KERNEL_BUILD = /lib/modules/$(KVERS)/build

obj-m += PDE.o

all:
	$(MAKE) -C $(KERNEL_BUILD) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_BUILD) M=$(PWD) clean
