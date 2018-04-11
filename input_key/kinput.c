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

static void input_dev_init(struct input_dev *dev)
{
  __set_bit(EV_SYN, dev->evbit);
  __set_bit(EV_KEY, dev->evbit);

  __set_bit(KEY_ESC, dev->keybit);
}

struct input_dev *inpdev;

static  __init int kinput_init(void)
{
  int ret;
  printk(KERN_NOTICE "KINPUT DEV INIT\n");
  inpdev = input_allocate_device();
  if(IS_ERR_OR_NULL(inpdev)){
    ret = -ENOMEM;
    goto err_alloc;
  }

  input_dev_init(inpdev);
  ret = input_register_device(inpdev);
  if(IS_ERR_VALUE(ret))
    goto err_input_reg;

  ret = request_irq(IRQ_EINT(26), handler, IRQF_TRIGGER_RISING |
          IRQF_TRIGGER_FALLING, DEV_NAME, inpdev);
  if(IS_ERR_VALUE(ret))
    goto err_request_irq;

  return 0;

  err_request_irq:
    input_unregister_device(inpdev);
  err_input_reg:
    input_free_device(inpdev);
  err_alloc:
    return ret;
}

static __exit void kinput_exit(void)
{
  printk(KERN_NOTICE "KINPUT DEV EXIT\n");
  free_irq(IRQ_EINT(26), inpdev);
  input_unregister_device(inpdev);
  input_free_device(inpdev);
}

module_init(kinput_init);
module_exit(kinput_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.11");
