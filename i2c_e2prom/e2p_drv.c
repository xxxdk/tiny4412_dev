#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

static int major;
static struct cdev *e2p_dev;
static struct class *e2p_class;
static struct i2c_client *e2p_client;

static ssize_t e2p_read(struct file *file, char __user *buf,
  size_t count, loff_t *off)
{
  unsigned char addr, data;

  if(copy_from_user(&addr, buf, 1))
    return -EFAULT;
  data = i2c_smbus_read_byte_data(e2p_client, addr);
  if(copy_to_user(buf, &data, 1))
    return -EFAULT;
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

static void dev_setup_cdev(struct cdev *dev, int index)
{
  int err, devno;
  devno = MKDEV(major, index);
  cdev_init(dev, &e2p_fops);
  err = cdev_add(dev, devno, 1);

  if(err){
    printk(KERN_NOTICE "Error %d add cdevdemo %d", err, index);
  }
}

static int __devinit e2p_probe(struct i2c_client *client,
  const struct i2c_device_id *id)
{
  int ret;
  dev_t devno;
  e2p_client = client;
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  ret = alloc_chrdev_region(&devno, 0, 1, "at24c08");
  if(ret < 0)
    return ret;
  major = MAJOR(devno);

  e2p_dev = kmalloc(sizeof(struct cdev), GFP_KERNEL);
  if(!e2p_dev){
    ret = -ENOMEM;
    goto fail_malloc;
  }
  memset(e2p_dev,0,sizeof(struct cdev));
  dev_setup_cdev(e2p_dev, 0);

  e2p_class = class_create(THIS_MODULE, "at24c08");
  device_create(e2p_class, NULL, MKDEV(major, 0), NULL, "at24c08");

  return 0;

  fail_malloc:
    unregister_chrdev_region(devno,1);
    return ret;
}

static int __devexit e2p_remove(struct i2c_client *client)
{
  printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  device_destroy(e2p_class, MKDEV(major, 0));
  class_destroy(e2p_class);
  cdev_del(e2p_dev);
  unregister_chrdev_region(MKDEV(major, 0), 1);
  kfree(e2p_dev);
  unregister_chrdev(major, "at24c08");

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
  i2c_add_driver(&e2p_driver);
  return 0;
}

static __exit void e2p_drv_exit(void)
{
  i2c_del_driver(&e2p_driver);
}

module_init(e2p_drv_init);
module_exit(e2p_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.23");
