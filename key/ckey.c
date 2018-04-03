#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void delay(void);

int main(int argc, char **argv)
{
  int fd;
  unsigned int val = 0;
  char key_buf[4] = {0};
  fd = open("/dev/kkey", O_RDWR);
  if(-1 == fd){
    printf("open fair\n");
    return -1;
  }
  while(1){
    val = read(fd, key_buf, 4);
    switch (val) {
      case 7:
        printf("the first_key press! key_val=%u\n",val);
        delay();
        break;
      case 11:
        printf("the second_key press! key_val=%u\n",val);
        delay();
        break;
      case 13:
        printf("the third_key press! key_val=%u\n",val);
        delay();
        break;
      case 14:
        printf("the fourth_key press! key_val=%u\n",val);
        delay();
        break;
      default:
        printf("no key is press!\n");
        delay();
    }
  }
  return 0;
}

void delay(void)
{
  unsigned int i = 0xffffff;
  while(i--);
}
