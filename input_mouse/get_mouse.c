#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
  struct input_event event_mouse;
  int fd, value, type, buffer[10]={0};

  fd = open("/dev/input/event5", O_RDWR);
  if(-1 == fd){
    printf("open mouse fail\n");
    return -1;
  }
  while(1){
    read(fd, &event_mouse, sizeof(event_mouse));
    switch(event_mouse.type){
      case EV_SYN:
        printf("sync\n");
        break;
      case EV_REL:
        printf("rel:%#x\n", event_mouse.code);
        if(event_mouse.code == REL_X){
          printf("event_mouse.code_X:%d\n", event_mouse.code);
          printf("event_mouse.value_X:%d\n", event_mouse.value);
        }
        if(event_mouse.code == REL_Y){
          printf("event_mouse.code_Y:%d\n", event_mouse.code);
          printf("event_mouse.value_Y:%d\n", event_mouse.value);
        }
      defalut:
        break;
    }
  }

  return 0;
}
