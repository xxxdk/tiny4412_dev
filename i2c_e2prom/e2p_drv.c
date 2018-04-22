#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

static int major;
static struct class *class;
static struct i2c_client *e2p_client;

static ssize_t e2p_read(struct file *file, char __user *buf,
  size_t count, loff_t *off)
{
  unsigned char addr, data;

  if(copy_from_user(&addr, buf, 1))
    return -EFAULT;
  data = i2c_smbus_read_byte_data(e2p_client, addr);
  copy_to_user(buf, &data, 1);
  return 1;
}

static ssize_t e2p_write(struct file *file, const char __user *buf,
  size_t count, loff_t *off)
{
  unsigned char ker_buf[2], addr, data;

  if(copy_from_user(ker_buf, buf, 2))
    return -EFAULT;
  addr = ker_buf[0];
  data = ker_buf[1];

  printk("addr = %#02x, data = %#02x\n", addr, data);
  if(!i2c_smbus_write_byte_data(e2p_client, addr, data))
    return 2;
  else
    return -EIO;
}

static struct file_operations e2p_fops = {
  .owner = THIS_MODULE,
  .read = e2p_read,
  .write = e2p_write,
};

static int __devinit e2p_probe(struct i2c_client *client,
  const struct i2c_device_id *id)
{
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

static int __devexit e2p_remove(struct i2c_client *client)
{
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

static const struct i2c_device_id e2p_id_table[] = {
  {"at24c08", 0},
  {},
};

static struct i2c_driver e2p_driver = {
  .driver = {
    .name = "e2prom",
    .owner = THIS_MODULE,
  },
  .probe = e2p_probe,
  .remove = e2p_remove,
  .id_table = e2p_id_table,
};

static __init int e2p_drv_init(void)
{
  i2c_add_driver();
}
