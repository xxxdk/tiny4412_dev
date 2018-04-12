#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <asm/uaccess.h>

#define DEV_NAME  "ktimer"
#define BUZZER_GPIO EXYNOS4_GPD0(0)

struct timer_list ktimer;

static void bell_init(void)
{
  gpio_request(BUZZER_GPIO, DEV_NAME);
  s3c_gpio_cfgpin(BUZZER_GPIO, S3C_GPIO_OUTPUT);
  gpio_set_value(BUZZER_GPIO, 0);
}

void timer_function(unsigned long value)
{
  while(value){
    gpio_set_value(BUZZER_GPIO, 1);
    printk("buzzer on\n");
    mdelay(500);
    gpio_set_value(BUZZER_GPIO, 0);
    printk("buzzer off\n");
    mdelay(500);
  }
}

static __init int ktimer_init(void)
{
  bell_init();
  init_timer(&ktimer);
  ktimer.data = 1;
  ktimer.expires = jiffies + (5 * HZ);
  ktimer.function = timer_function;
  add_timer(&ktimer);
  return 0;
}

static __exit void ktimer_exit(void)
{
  gpio_free(BUZZER_GPIO);
  del_timer(&ktimer);
}

module_init(ktimer_init);
module_exit(ktimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.12");
