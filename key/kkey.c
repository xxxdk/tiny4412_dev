#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#define DEV_NAME  "kkey"
#define GPX3CON   0x11000C60
#define GPX3CON_DAT 0x11000C64
#define KEY_MAJOR 235

volatile unsigned long *key_config = NULL;
volatile unsigned long *key_dat = NULL;

static int key_major = KEY_MAJOR;

struct key_dev{
  struct cdev cdev;
};

struct key_dev *key_devp;

int key_open(struct inode *inode, struct file *filp)
{
  printk(KERN_NOTICE "KEY OPEN\n");
  *key_config &= ~(0xffff << 8);
  return 0;
}

ssize_t key_read(struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
  int ret;
  unsigned char key_val;
  if(size < 4){
    return -1;
  }
  key_val = (*key_dat >> 2) & 0xf;
  ret = copy_to_user(buf, &key_val, sizeof(key_val));
  if(ret != 0){
    printk(KERN_NOTICE "COPY TO USER ERROR\n");
    return -1;
  }

  return key_val;
}

int key_close(struct inode *inode, struct file *filp)
{
  printk(KERN_NOTICE "KEY CLOSE\n");
  return 0;
}

struct file_operations key_fops = {
  .owner = THIS_MODULE,
  .open = key_open,
  .read = key_read,
  .release = key_close,
};

static void dev_setup_cdev(struct key_dev *dev, int index)
{
  int err, devno;
  printk(KERN_NOTICE "dev_setup_cdev\n");
  devno = MKDEV(key_major, index);
  cdev_init(&dev->cdev, &key_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &key_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if(err){
    printk(KERN_NOTICE "Error %d add cdevdemo %d", err, index);
  }
}

static __init int key_init(void)
{
  int ret;
  dev_t devno;
  struct class *key_class;

  printk(KERN_NOTICE "KEY INIT\n");
  devno = MKDEV(key_major,0);
  if(key_major){
    printk(KERN_NOTICE "REGISTER CHRDEV\n");
    ret = register_chrdev_region(devno, 1, DEV_NAME);
  }else{
    printk(KERN_NOTICE "ALLOC CHRDEV\n");
    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    key_major = MAJOR(devno);
  }
  if(ret < 0){
    return ret;
  }

  key_devp = kmalloc(sizeof(struct key_dev),GFP_KERNEL);
  if(!key_devp){
    ret = -ENOMEM;
    printk(KERN_NOTICE "ERROR CDEV\n");
    goto fail_malloc;
  }

  printk(KERN_NOTICE "======== cdevdemo_init 2.8");
  memset(key_devp,0,sizeof(struct key_dev));
  printk(KERN_NOTICE "======== cdevdemo_init 3");
  dev_setup_cdev(key_devp, 0);

  key_class = class_create(THIS_MODULE, DEV_NAME);
  device_create(key_class, NULL, MKDEV(key_major, 0), NULL, DEV_NAME);
  printk(KERN_NOTICE "key_class ok\n");
  key_config = (volatile unsigned long *)ioremap(GPX3CON, 4);
  key_dat = (volatile unsigned long *)ioremap(GPX3CON_DAT, 4);
//  key_dat = key_config + 1;

  return 0;

  fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static __exit void key_exit(void)
{
  printk(KERN_NOTICE "KEY EXIT\n");
  cdev_del(&key_devp->cdev);
  unregister_chrdev_region(MKDEV(key_major, 0), 1);
  kfree(key_devp);
  iounmap(key_config);
  iounmap(key_dat);
}

module_init(key_init);
module_exit(key_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.03.27");
