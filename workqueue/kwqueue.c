#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
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
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define DEV_NAME  "kwqueue"

struct tasklet_struct task_t;
struct workqueue_struct *mywork;
struct work_struct work;

static void task_func(unsigned long data)
{
  if(in_interrupt())
    printk("%s in interrupt handle\n", __FUNCTION__);
}

static void mywork_func(struct work_struct *work)
{
  if(in_interrupt())
    printk("%s in interrupt handle\n", __FUNCTION__);
  msleep(2);
  printk("%s in process handle\n", __FUNCTION__);
}

static irqreturn_t irq_func(int irq, void *dev_id)
{
  tasklet_schedule(&task_t);
  schedule_work(&work);
  if(in_interrupt())
    printk("%s in interrupt handle\n", __FUNCTION__);
  printk("key irq:%d\n", irq);
  return IRQ_HANDLED;
}

static __init int kwqueue_irq_init(void)
{
  int err, irq_num, data_t;
  err = 0;
  data_t = 100;
  mywork = create_workqueue("my work");
  INIT_WORK(&work, mywork_func);

  queue_work(mywork, &work);
  tasklet_init(&task_t, task_func, data_t);
  printk("kwqueue irq init\n");
  irq_num = gpio_to_irq(EXYNOS4_GPX3(2));
  err = request_irq(irq_num, irq_func,
    IRQF_TRIGGER_FALLING, DEV_NAME, (void*)"key1");
  if(err != 0){
    free_irq(irq_num, (void *)"key1");
    return -1;
  }
  return 0;
}

static __exit void kwqueue_irq_exit(void)
{
  int irq_num;
  printk("workqueue irq exit\n");
  irq_num = gpio_to_irq(EXYNOS4_GPX3(2));
  destroy_workqueue(mywork);
  free_irq(irq_num, (void *)"key1");
}

module_init(kwqueue_irq_init);
module_exit(kwqueue_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.16");
