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
#include <linux/timer.h>  /*timer*/
#include <asm/uaccess.h>  /*jiffies*/
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/input.h>

struct input_dev *dev;

static irqreturn_t irq_func(int irq, void *dev_id)
{
  struct input_dev *keydev;
  keydev = dev_id;
  input_report_key(keydev, KEY_HOME, !gpio_get_value(EXYNOS4_GPX3(2)));
  input_sync(keydev);
  printk("irq:%d\n", irq);
  return IRQ_HANDLED;
}

static __init int khome_irq_init(void)
{
  int err = 0;
  int irq_num = 0;
  int ret;
  // struct input_id id;
  dev = input_allocate_device();
  if(IS_ERR_OR_NULL(dev)){
    ret = -ENOMEM;
    goto err_alloc;
  }

  dev->name = "khome_key";
  dev->phys = "xdk_create_key";
  dev->uniq = "20180416";
  dev->id.bustype = BUS_HOST;
  dev->id.vendor = ID_PRODUCT;
  dev->id.version = ID_VENDOR;
  set_bit(EV_SYN, dev->evbit);
  set_bit(EV_KEY, dev->evbit);
  set_bit(KEY_HOME, dev->keybit);

  ret = input_register_device(dev);
  if(IS_ERR_VALUE(ret))
    goto err_input_reg;
  irq_num = gpio_to_irq(EXYNOS4_GPX3(2));
  err = request_irq(irq_num, irq_func, IRQF_TRIGGER_FALLING, "khome_key", dev);
  if(err != 0)
    goto free_irq_flag;
  return 0;
err_input_reg:
  input_unregister_device(dev);
free_irq_flag:
  free_irq(irq_num, (void *)"khome_key");
err_alloc:
  return ret;
}

static __exit void khome_irq_exit(void)
{
  int irq_num;
  irq_num = gpio_to_irq(EXYNOS4_GPX3(2));
  free_irq(irq_num, dev);
}

module_init(khome_irq_init);
module_exit(khome_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.16");
