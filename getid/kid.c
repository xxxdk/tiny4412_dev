#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>

#define DEV_NAME  "kid"
#define PRO_ID    0x10000000
#define ID_MAJOR  236

unsigned int *virtul = NULL;

static int id_major = ID_MAJOR;

struct id_dev{
  struct cdev cdev;
};

struct id_dev *id_devp;

int dev_open(struct inode *inode, struct file *filp)
{
  printk(KERN_NOTICE "ID OPEN\n");
  return 0;
}

void dev_close(struct inode *inode, struct file *filp)
{
  printk(KERN_NOTICE "ID CLOSE\n");
}

ssize_t id_read(struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
  if(size < 4)
    return -1;
  return ioread32(virtul);
}

struct file_operations id_fops = {
  .owner = THIS_MODULE,
  .open = dev_open,
  .read = id_read,
  .release = dev_close,
};

static void dev_setup_cdev(struct id_dev *dev, int index)
{
  int err, devno;
  devno = MKDEV(id_major,index);
  cdev_init(&dev->cdev, &id_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &id_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if(err)
    printk(KERN_NOTICE "Err %d add id cdev %d\n", err, index);
}

static __init int id_init(void)
{
  int ret;
  dev_t devno;
  struct class *id_class;

  printk(KERN_NOTICE "ID INIT\n");
  devno = MKDEV(id_major, 0);
  if(id_major){
    printk(KERN_NOTICE "REGISTER CHRDEV\n");
    ret = register_chrdev_region(devno, 1, DEV_NAME);
  }else{
    printk(KERN_NOTICE "ALLOC CHRDEV\n");
    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    id_major = MAJOR(devno);
  }
  if(ret < 0)
    return ret;

  id_devp = kmalloc(sizeof(struct id_dev),GFP_KERNEL);
  if(!id_devp){
    ret = -ENOMEM;
    printk(KERN_NOTICE "ERROR CDEV\n");
    goto fail_malloc;
  }
  memset(id_devp, 0, sizeof(struct id_dev));
  dev_setup_cdev(id_devp, 0);

  id_class = class_create(THIS_MODULE, DEV_NAME);
  device_create(id_class, NULL, MKDEV(id_major, 0), NULL, DEV_NAME);
  virtul = ioremap(PRO_ID, 4);
  if(IS_ERR_OR_NULL(virtul))
    return -1;

  return 0;

  fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static __exit void id_exit(void)
{
  printk(KERN_NOTICE "ID EXIT\n");
  unregister_chrdev_region(id_major, 1);
  iounmap(virtul);
}

module_init(id_init);
module_exit(id_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.02");
