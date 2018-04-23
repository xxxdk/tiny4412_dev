#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>

struct eeprom_private{
  char *name;
  int size;
  int version;
};

struct eeprom_global{
  struct i2c_client *client;
  struct eeprom_private *private;
  struct miscdevice i2c_misc;
};

static struct eeprom_global *ep_dev;

static int eeprom_recv(const struct i2c_client *client, char *buf, int count)
{
  int ret;
  struct i2c_adapter *adapter = client->adapter;
  struct i2c_msg msg;
  msg.addr = client->addr;
  msg.flags = I2C_M_RD;
  msg.len = count;
  msg.buf = buf;

  ret = i2c_transfer(adapter, &msg, 1);

  return (ret == 1)? count : ret;
}

static int eeprom_send(struct i2c_client *client, char *buf, int count)
{
  int ret;
  struct i2c_adapter *adapter = client->adapter;
  struct i2c_msg msg;
  msg.addr = client->addr;
  msg.buf = buf;
  msg.flags = 0;
  msg.len = count;

  ret = i2c_transfer(adapter, &msg, 1);
  return (ret == 1) ? count : ret;
}

static ssize_t eeprom_read(struct file *file, char __user *buf,
  size_t count, loff_t *off)
{
  int ret;
  char *temp;

  printk("%s\n", __FUNCTION__);

  if(count < 0|| count > ep_dev->private->size)
    return -EINVAL;
  temp = kzalloc(count, GFP_KERNEL);

  ret = eeprom_recv(ep_dev->client, temp, count);
  if(ret < 0)
    goto err_free;

  ret = copy_to_user(buf, temp, count);
  if(ret > 0){
    ret = -EINVAL;
    goto err_free;
  }

  kfree(temp);
  return count;

err_free:
  kfree(temp);
  return ret;
}

static ssize_t eeprom_write(struct file *file, const char __user *buf,
  size_t count, loff_t *off)
{
  int ret;
  char *temp;

  printk("%s\n", __FUNCTION__);

  if(count < 0 || count > ep_dev->private->size)
    return -EINVAL;

  temp = kzalloc(count, GFP_KERNEL);
  ret = copy_from_user(temp, buf, count);
  if(ret > 0){
    ret = -EFAULT;
    goto err_free;
  }

  ret = eeprom_send(ep_dev->client, temp, count);
  if(ret < 0)
    goto err_free;

  kfree(temp);
  return count;

err_free:
  kfree(temp);
  return ret;
}

struct file_operations ep_fops = {
  .read = eeprom_read,
  .write = eeprom_write,
};

static int eeprom_probe(struct i2c_client *client,
  const struct i2c_device_id *id)
{
  struct eeprom_private *p;
  printk("client name: %s\n", id->name);
  p = (struct eeprom_private *)id->driver_data;

  ep_dev = kzalloc(sizeof(struct eeprom_global), GFP_KERNEL);

  ep_dev->client = client;
  ep_dev->private = (struct eeprom_private *)id->driver_data;

  ep_dev->i2c_misc.fops = &ep_fops;
  ep_dev->i2c_misc.minor = 199;
  ep_dev->i2c_misc.name = "at24c08";

  misc_register(&ep_dev->i2c_misc);

  return 0;
}

static int eeprom_remove(struct i2c_client *client)
{
  misc_deregister(&ep_dev->i2c_misc);
  kfree(ep_dev);

  return 0;
}

struct eeprom_private c08_private = {
  .name = "at24c08 test",
  .size = 1024,
  .version = 0x5678,
};

struct i2c_device_id eeprom_id_table[] = {
  {"at24c08",(unsigned long)&c08_private},
};

struct i2c_driver eeprom_drv = {
  .probe = eeprom_probe,
  .remove = eeprom_remove,
  .driver = {
    .name = "eeprom driver"
  },
  .id_table = eeprom_id_table,
};

static __init int eeprom_init(void)
{
  return i2c_add_driver(&eeprom_drv);
}

static __exit void eeprom_exit(void)
{
  i2c_del_driver(&eeprom_drv);
}

module_init(eeprom_init);
module_exit(eeprom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.23");
