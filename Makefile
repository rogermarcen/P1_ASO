obj-m := P1F1.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
install:
	insmod P1F1.ko
delete:
	rmmod P1F1.ko
