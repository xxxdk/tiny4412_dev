#将你所写的驱动程序编译成模块形式
obj-m	+= kbuzzer.o
#你需要的内核
KERNEL := /home/project/Downloads/linux-3.5
#模块编译
#模块清除
clean:
	make -C $(KERNEL) M=`pwd` clean
	rm -rf cbuzzer

#编译上层app应用程序
buzzer:
	arm-linux-gcc cbuzzer.c -o cbuzzer

all:
	make -C $(KERNEL) M=`pwd` modules
	arm-linux-gcc cbuzzer.c -o cbuzzer
