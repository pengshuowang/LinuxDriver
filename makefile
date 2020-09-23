#1.使用不同的开发板内核时，一定要修改KERN_DIR
#2.KERN_DIR中的内核要事先配置、编译，为了能编译内核，要先设置下列环境变量：
#2.1 ARCH,			比如：export ARCH=arm64
#2.2 CROSS_COMPILE	比如：export CROSS_COMPILE=aarch64-linux-gnu-
#2.3 PATH,			比如：export PATH:=$PATH:/GCC路径

KERN_DIR=

all:
	make -C $(KERN_DIR) M='pwd' modules
	$(CROSS_COMPILE) gcc -o hello_drv_test hello_drv_test.C

clean:
	make -C $(KERN_DIR) M='pwd' modules clean
	rm -f modules.order
	rm -f hello_drv_test

obj-m +=hello_drv.o