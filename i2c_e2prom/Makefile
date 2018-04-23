#将你所写的驱动程序编译成模块形式
obj-m	+= e2p_drv.o
# obj-m += e2p_dev.o
#你需要的内核
# KERNEL := /home/sni/workSpace/linux-3.5
KERNEL := /home/project/Downloads/linux-3.5
#模块编译
all:
	make -C $(KERNEL) M=`pwd` modules
	arm-linux-gcc e2p.c -o e2p
#模块清除
clean:
	make -C $(KERNEL) M=`pwd` clean
	rm -rf e2p

#编译上层app应用程序
bin:
	arm-linux-gcc e2p.c -o e2p
