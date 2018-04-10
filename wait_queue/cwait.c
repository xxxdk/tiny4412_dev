#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
  int fd, ret;
  char buf[4]={0};
  fd = open("/dev/kwait", O_RDWR);
  if(-1 == fd){
    printf("open fair!\n");
    return -1;
  }

  ret = read(fd, buf, 4);
  printf("R1 %d\n", ret);

  ret = write(fd, buf, 4);
  printf("W1 %d\n", ret);


  // printf("R1 %d\n", read(fd, buf, 2));

  // printf("W1 %d\n", write(fd, buf, 2));
  return 0;
}
