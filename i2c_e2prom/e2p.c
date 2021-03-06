#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_usage(char *file)
{
  printf("%s r addr\n", file);
  printf("%s w addr val\n", file);
}

int main(int argc, char **argv)
{
  int fd;
  unsigned char buf[2];

  if((argc != 3) && (argc != 4)){
    print_usage(argv[0]);
    return -1;
  }

  fd = open("/dev/at24c08", O_RDWR);
  if(fd < 0){
    printf("open fail\n");
    return -1;
  }
  printf("open success\n");
  if(strcmp(argv[1], "r") == 0){
    printf("r\n");
    buf[0] = strtoul(argv[2], NULL, 0);
    read(fd, buf, 1);
    printf("data: %c, %d, %#02x\n", buf[0], buf[0], buf[0]);
  }
  else if((strcmp(argv[1], "w") == 0) && (argc == 4)){
    printf("w\n");
    buf[0] == strtoul(argv[2], NULL, 0);
    buf[1] == strtoul(argv[3], NULL, 0);
    if(write(fd, buf, 2) != 2)
      printf("write err, addr = %#02x, data = %#02x\n\n", buf[0], buf[1]);
  }else{
    print_usage(argv[0]);
    return -1;
  }
  return 0;
}
