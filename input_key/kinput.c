#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/atomic.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/input.h>


#define DEV_NAME "kinput"

irqreturn_t handler(int num, void *data)
{
  struct input_dev *dev;
  dev = data;
  printk(KERN_NOTICE "%d: IRQ HANDLER\n", num);

#if 1
  input_report_key(dev, KEY_ESC, !gpio_get_value(EXYNOS4_GPX3(2)));
#endif

  input_sync(dev);

  return IRQ_HANDLED;
}

void input_dev_init(struct input_dev *dev)
{
  __set_bit(EV_SYN, dev->evbit);
  __set_bit(EV_KEY, dev->evbit);

  __set_bit(KEY_ESC, dev->keybit);
}

struct input_dev *inpdev;

int kinput_init(void)
{
  int ret;
  printk(KERN_NOTICE "KINPUT DEV INIT\n");
  dev = input_allocate_device();
  if(IS_ERROR_NULL(dev)){
    ret = -ENOMEM;
    goto err_alloc;
  }
}
