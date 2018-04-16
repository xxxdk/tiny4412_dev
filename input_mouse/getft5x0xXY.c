#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <inttypes.h>
#include <errno.h>

int touch_fd = -1;

int ft5x0x_ts_init(void)
{
  touch_fd = open("/dev/input/event0", O_RDONLY);
  if(touch_fd < 0){
    printf("open fail\n");
    return -1;
  }
  return 0;
}

int get_ft5x0x_postion(int *x, int *y)
{
  int touch_ret = -1;
  struct input_event ft5x0x_ts;

  touch_ret = read(touch_fd, &ft5x0x_ts, sizeof(ft5x0x_ts));
  if(touch_ret < 0)
    printf("read fail\n");
  switch (ft5x0x_ts.type) {
    case EV_SYN:
      printf("SYNC\n");
    case EV_ABS:
      if(ft5x0x_ts.code == ABS_MT_POSITION_X)
        *x = ft5x0x_ts.value;
      if(ft5x0x_ts.code == ABS_MT_POSITION_Y)
        *y = ft5x0x_ts.value;
    default:
      break;
  }
  return 0;
}

int main(int argc, char const *argv[])
{
  int tp_ret;
  int ft_x = 0;
  int ft_y = 0;

  tp_ret = ft5x0x_ts_init();
  if(-1 == tp_ret){
    printf("tp init fail\n");
    return -1;
  }
  printf("tp init success\n");

  while(1){
    get_ft5x0x_postion(&ft_x, &ft_y);
    printf("ft_x:%d\nft_y:%d\n", ft_x, ft_y);
    usleep(100);
  }

  return 0;
}
