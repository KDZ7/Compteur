# Simple Makefile for the COUNTER kernel module

# Module name
obj-m := counter.o

# Kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target: Build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean the build files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
