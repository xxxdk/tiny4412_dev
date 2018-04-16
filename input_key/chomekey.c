//本程序只能运行一次，后续就不能使用
#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
  struct input_event event_key;
  int fd, value, type, buffer[10]={0};

  fd = open("/dev/input/event3", O_RDWR);
  if(-1 == fd){
    printf("open home key fail\n");
    return -1;
  }
  printf("open home key success\n");
  while(1){
    read(fd, &event_key, sizeof(event_key));
    printf("home key return type:%#x\n", event_key.type);
    switch(event_key.type){
      case EV_SYN:
        printf("sync\n");
        break;
      case EV_KEY:
        printf("home key return code:%#x\n", event_key.code);
        printf("home key return value:%d\n", event_key.value);
      defalut:
        break;
    }
  }

  return 0;
}
