#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char  **argv) {
  int fd;
  int val = 0;
  fd = open("/dev/kpled",O_RDWR);
  if(-1 == fd)
  {
    printf("open fair!\n");
    return -1;
  }
  while(1)
  {
      val = 0;
      write(fd,&val,4);
      sleep(1);

      val = 1;
      write(fd,&val,4);
      sleep(1);

      val = 2;
      write(fd,&val,4);
      sleep(1);

      val = 3;
      write(fd,&val,4);
      sleep(1);

      val = 5;
      write(fd,&val,4);
      sleep(1);
  }
  return 0;
}
