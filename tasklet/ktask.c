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
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>

#define DEV_NAME "ktask"

struct tasklet_struct task_t;

static void task_func(unsigned long data)
{
  if(in_interrupt())
    printk("%s in interrupt handle\n", __FUNCTION__);
}

static irqreturn_t irq_func(int irq, void *dev_id)
{
  tasklet_schedule(&task_t);
  if(in_interrupt())
    printk("%s in interrupt handle\n", __FUNCTION__);
  printk("key irq:%d\n",irq);

  return IRQ_HANDLED;
}

static __init int ktask_init(void)
{
  int err = 0;
  int irq_num;
  int data_t = 100;

  if(in_interrupt())
    printk("%s is interrupt handle\n",__FUNCTION__);
  else{
    printk("%s is proccess handle\n",__FUNCTION__);
    msleep(2);
    printk("%s delay success\n",__FUNCTION__);
  }

  tasklet_init(&task_t, task_func, data_t);
  printk("irq key init\n");

  irq_num = gpio_to_irq(EXYNOS4_GPX3(2));

  err = request_irq(irq_num, irq_func, IRQF_TRIGGER_FALLING,
    DEV_NAME, (void *)"key1");
  if(err != 0){
    free_irq(irq_num, (void *)"key1");
    return err;
  }
  return 0;
}

static __exit void ktask_exit(void)
{
  int irq_num;
  printk("irq key exit\n");
  irq_num = gpio_to_irq(EXYNOS4_GPX3(2));
  free_irq(irq_num, (void *)"key1");
}

module_init(ktask_init);
module_exit(ktask_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.12");
