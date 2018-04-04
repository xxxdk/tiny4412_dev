#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
//#include <linux/fb.h>
//#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#define DEV_NAME  "kpwm"
#define PWM_IOCTL_SET_FREQ  1
#define PWM_IOCTL_STOP      0
#define NS_IN_1HZ           (1000000000UL)
#define BUZZER_PWM_ID       0
#define BUZZER_PWM_GPIO     EXYNOS4_GPD(0)
#define PWM_MAJOR           237

static struct pwm_device *pwm4buzzer;
static struct semaphore lock;

struct pwm_dev{
  struct cdev cdev;
};

struct pwm_dev *pwm_devp;

static void pwm_set_freq(unsigned long freq)
{

}

static pwm_stop(void)
{

}

static int pwm_open()
{

}

static int pwm_close()
{

}

static long pwm_ioctl()
{

}

struct file_operations pwm_fops ={

}
