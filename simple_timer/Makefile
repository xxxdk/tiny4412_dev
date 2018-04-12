#将你所写的驱动程序编译成模块形式
obj-m	+= ktimer.o
#你需要的内核
KERNEL := /home/project/Downloads/linux-3.5
#模块编译
all:
	make -C $(KERNEL) M=`pwd` modules
	# arm-linux-gcc cmisc.c -o cmisc
#模块清除
clean:
	make -C $(KERNEL) M=`pwd` clean
# 	rm -rf cmisc
#
# #编译上层app应用程序
# app:
# 	arm-linux-gcc led.c -o cmisc
