/*ctrl-c atexit(close_buzzer);
*/
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PWM_OPEN  1
#define PWM_STOP  0

static void open_buzzer(void);
static void close_buzzer(void);
static void set_buzzer(int freq);
static void stop_buzzer(void);

static int fd = -1;

int main(int argc, char **agrv)
{
  int freq = 1000;
  open_buzzer();
  stop_buzzer();
  while(1){
    while(1){
      set_buzzer(freq);
      if(freq < 20000)
        freq += 100;
      printf("\tFREQ = %d\n", freq);
      if(freq == 20000)
        break;
    }
    while(1){
      set_buzzer(freq);
      if(freq > 1000)
        freq -= 100;
      printf("\tFREQ = %d\n", freq);
      if(freq == 1000)
        break;
    }
  }
}

static void open_buzzer(void)
{
  fd = open("/dev/kpwm", 0);
  if(fd < 0){
    perror("open pwm buzzer device");
    exit(1);
  }
  atexit(close_buzzer);
}

static void close_buzzer(void)
{
  if(fd > 0){
    ioctl(fd, PWM_STOP);
    if(ioctl(fd, 2) < 0){
      perror("ioctl 2:");
    }
    close(fd);
    fd = -1;
  }
}

static void set_buzzer(int freq)
{
  int ret = ioctl(fd, PWM_OPEN, freq);
  if(ret < 0){
    perror("set the frequency of the buzzer");
    exit(1);
  }
}

static void stop_buzzer(void)
{
  int ret = ioctl(fd, PWM_STOP);
  if(ret < 0){
    perror("stop the buzzer");
    exit(1);
  }
  if(ioctl(fd, 2) < 0)
    perror("ioctl 2:");
}
