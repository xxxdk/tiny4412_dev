#将你所写的驱动程序编译成模块形式
# obj-m	+= kinput.o
obj-m += khomekey.o
#你需要的内核
KERNEL := /home/sni/workSpace/linux-3.5
#模块编译
all:
	make -C $(KERNEL) M=`pwd` modules
	arm-linux-gcc chomekey.c -o chomekey
#模块清除
clean:
	make -C $(KERNEL) M=`pwd` clean
	rm chomekey

bin:
	arm-linux-gcc chomekey.c -o chomekey
