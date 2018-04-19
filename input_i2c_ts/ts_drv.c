#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include "ts.h"

struct ts_st *ts;
struct ts_info_st *pdata;

irqreturn_t ts_handler(int irq, void *data)
{
  printk("ts touch irq:%d\n", irq);
  return IRQ_HANDLED;
}

int ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int ret;
  printk("probe\n");

  pdata = client->dev.platform_data;
  ts = kzalloc(sizeof(*ts), GFP_KERNEL);
  if(!ts)
    return -ENOMEM;
  printk("malloc success\n");

  ts->client = client;
  ts->irq = pdata->irq;
  ts->xres = pdata->xres;
  ts->yres = pdata->yres;
  ts->pressure = pdata->pressure;
  ts->dev = input_allocate_device();
  if(!ts->dev){
    ret = -ENOMEM;
    goto alloc_input_error;
  }
  ts->dev->name = client->name;
  ts->dev->phys = "ft5x0x_ts";
  ts->dev->uniq = "201804119";
  ts->dev->id.bustype = BUS_I2C;
  ts->dev->id.vendor = 10010;
  ts->dev->id.product = 10111;
  ts->dev->id.version = 1;

  ret = input_register_device(ts->dev);
  if(ret)
    goto register_input_error;
  ret = request_irq(ts->irq, ts_handler,
    IRQF_TRIGGER_FALLING, client->name, ts);
  if(ret)
    goto irq_error;
  printk("register irq success\n");
  return ret;

  irq_error:
    free_irq(ts->irq, &ts);
  register_input_error:
    input_free_device(ts->dev);
  alloc_input_error:
    kfree(ts);

  return 0;
}

struct i2c_device_id id_table[] = {
  {"myts", 123},
  {},
};

struct i2c_driver ts_drv = {
  .probe = ts_probe,
  .driver = {
    .name = "myts",
  },
  .id_table = id_table,
};

static __init int hello_init(void)
{
  int ret;
  printk("Hello kernel installed\n");
  ret = i2c_add_driver(&ts_drv);
  if(ret != 0){
    printk("i2c add dirver fail\n");
    i2c_del_driver(&ts_drv);
    return -1;
  }
  printk("i2c add success\n");
  return 0;
}

static __exit void hello_exit(void)
{
  printk("goodbye\n");
  i2c_del_driver(&ts_drv);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.19");
