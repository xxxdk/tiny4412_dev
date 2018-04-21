#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
  int fd;
  fd = open("/dev/kpbuzzer", O_RDWR);
  if(-1 == fd){
    printf("open fair!\n");
    return -1;
  }
  while(1){
    ioctl(fd,1);
    sleep(1);
    ioctl(fd,0);
    sleep(1);
  }
  return 0;
}
