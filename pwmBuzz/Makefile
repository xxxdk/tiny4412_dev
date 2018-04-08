#将你所写的驱动程序编译成模块形式
obj-m	+= kpbuzz.o
#你需要的内核
KERNEL := /home/sni/workSpace/linux-3.5
#模块编译
all:
	make -C $(KERNEL) M=`pwd` modules
	arm-linux-gcc cpbuzz.c -o cpbuzz
#模块清除
clean:
	make -C $(KERNEL) M=`pwd` clean
	rm -rf cpbuzz

#编译上层app应用程序
id:
	arm-linux-gcc cpbuzz.c -o cpbuzz
