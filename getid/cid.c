#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
  int fd;
  unsigned int id_val = 0;
  char id_buf[4] = {0};

  fd = open("/dev/kid", O_RDWR);
  if(-1 == fd){
    printf("open fair\n");
    return -1;
  }
  id_val = read(fd, id_buf, 4);
  printf("id valuex = %#x\n", id_val);

  close(fd);

  return 0;
}
