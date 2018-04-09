#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/fb.h>
#include <linux/backlight.h>
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
#define BUZZER_PWM_GPIO     EXYNOS4_GPD0(0)
// #define PWM_MAJOR           237

static struct pwm_device *pwm4buzzer;
static struct semaphore lock;

// struct pwm_buzzer_dev{
//   struct cdev cdev;
// };
// struct pwm_buzzer_dev *pwm_devp;


static void pwm_buzzer_set_freq(unsigned long freq)
{
  int period_ns = NS_IN_1HZ / freq;
  pwm_config(pwm4buzzer, period_ns/2, period_ns);
  pwm_enable(pwm4buzzer);
  s3c_gpio_cfgpin(BUZZER_PWM_GPIO, S3C_GPIO_SFN(2));
}

static void pwm_buzzer_stop(void)
{
  s3c_gpio_cfgpin(BUZZER_PWM_GPIO, S3C_GPIO_OUTPUT);
  pwm_config(pwm4buzzer, 0, NS_IN_1HZ/100);
  pwm_disable(pwm4buzzer);
}

static int pwm_buzzer_open(struct inode *inode, struct file *file)
{
  if(!down_trylock(&lock))
    return 0;
  else
    return -EBUSY;
}

static int pwm_buzzer_close(struct inode *inode, struct file *file)
{
  up(&lock);
  return 0;
}

static long pwm_buzzer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
  switch(cmd){
  case PWM_IOCTL_SET_FREQ:
    if(arg == 0)
      return -EINVAL;
    pwm_buzzer_set_freq(arg);
    break;
  case PWM_IOCTL_STOP:
  default:
    pwm_buzzer_stop();
    break;
  }
  return 0;
}

struct file_operations pwm_buzzer_fops = {
  .owner = THIS_MODULE,
  .open = pwm_buzzer_open,
  .release = pwm_buzzer_close,
  .unlocked_ioctl = pwm_buzzer_ioctl,
};

struct miscdevice pwm_misc_dev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = DEV_NAME,
  .fops = &pwm_buzzer_fops,
};

// static dev_setup_cdev()
// {
//   int err, devno;
// }

static __init int pwm_buzzer_init(void)
{
  int ret;

  ret = gpio_request(BUZZER_PWM_GPIO, DEV_NAME);
  if(ret){
    printk(KERN_NOTICE "REQUEST GPIO %d FOR PWM FAILED\n", BUZZER_PWM_GPIO);
    return ret;
  }

  gpio_set_value(BUZZER_PWM_GPIO, 0);
  s3c_gpio_cfgpin(BUZZER_PWM_GPIO, S3C_GPIO_OUTPUT);

  pwm4buzzer = pwm_request(BUZZER_PWM_ID, DEV_NAME);
  if(IS_ERR(pwm4buzzer)){
    printk(KERN_NOTICE "REQUEST PWM %d FOR %s FAILED\n", BUZZER_PWM_ID, DEV_NAME);
    // return -ENODEV;
    ret = PTR_ERR(pwm4buzzer);
    printk("ERROR IS %d", ret);
    return ret;
  }
  pwm_buzzer_stop();

  sema_init(&lock, 1);
  ret = misc_register(&pwm_misc_dev);
  printk(KERN_NOTICE DEV_NAME "\tinitialized\n");
  return 0;
}

static __exit void pwm_buzzer_exit(void)
{
  pwm_buzzer_stop();
  misc_deregister(&pwm_misc_dev);
  gpio_free(BUZZER_PWM_GPIO);
}

module_init(pwm_buzzer_init);
module_exit(pwm_buzzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.08");
