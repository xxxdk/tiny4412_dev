#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

#define DEV_NAME  "kwait"
#define DEV_MAJOR 237

struct kwait_dev{
  struct cdev cdev;
};
struct kwait_dev *kwait_devp;
struct task_struct *task;

volatile bool empty = true;
static int dev_major = DEV_MAJOR;

static int kwait_open(struct inode *inode, struct file *file)
{
  printk(KERN_NOTICE "WAIT QUEUE OPEN\n");
  return 0;
}

static int kwait_close(struct inode *inode, struct file *file)
{
  printk(KERN_NOTICE "WAIT QUEUE CLOSE\n");
  return 0;
}

static ssize_t kwait_read(struct file *filp, char __user *buf, size_t size,\
  loff_t *off)
{
  int ret;
  while(empty){
    printk(KERN_NOTICE "WHILE START\n");
    if(filp->f_flags & O_NONBLOCK)
      return 0;
    task = current;
    set_current_state(TASK_INTERRUPTIBLE);
    printk(KERN_NOTICE "WHILE WAIT\n");
    schedule();
    printk(KERN_NOTICE "WHILE WAIT S\n");
    if(signal_pending(current))
      return -ERESTARTSYS;
    printk(KERN_NOTICE "READ: WAKE UP\n");
  }
  printk(KERN_NOTICE "WHILE END\n");
  ret = size;
  empty = true;
  return ret;
}

static ssize_t kwait_write(struct file *filp, const char __user *buf,\
  size_t size, loff_t *off)
{
  int ret;
  empty = false;
  wake_up_process(task);
  ret = size;
  return ret;
}

struct file_operations kw_fops = {
  .owner= THIS_MODULE,
  .open = kwait_open,
  .read = kwait_read,
  .write = kwait_write,
  .release = kwait_close,
};

static void dev_setuo_cdev(struct kwait_dev *dev, int index)
{
  int err, devno;
  devno = MKDEV(dev_major, index);
  cdev_init(&dev->cdev, &kw_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &kw_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if(err)
    printk(KERN_NOTICE "Err %d add id cdev %d\n", err, index);
}

static __init int kwait_init(void)
{
  int ret;
  dev_t devno;
  struct class *kw_class;

  printk(KERN_NOTICE "KW INIT\n");
  devno = MKDEV(dev_major, 0);
  if(dev_major){
    printk(KERN_NOTICE "REGISTER CHRDEV\n");
    ret = register_chrdev_region(devno, 1, DEV_NAME);
  }else{
    printk(KERN_NOTICE "ALLOC CHRDEV\n");
    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    dev_major = MAJOR(devno);
  }
  if(ret < 0)
    return ret;
  kwait_devp = kmalloc(sizeof(struct kwait_dev), GFP_KERNEL);
  if(!kwait_devp){
    ret = -ENOMEM;
    printk(KERN_NOTICE "ERROR CDEV\n");
    goto fail_malloc;
  }
  memset(kwait_devp, 0, sizeof(struct kwait_dev));
  dev_setuo_cdev(kwait_devp, 0);

  kw_class = class_create(THIS_MODULE, DEV_NAME);
  device_create(kw_class, NULL, MKDEV(dev_major, 0), NULL, DEV_NAME);

  return 0;

  fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static __exit void kwait_exit(void)
{
  printk(KERN_NOTICE "WAIT QUEUE EXIT\n");
  unregister_chrdev_region(dev_major, 1);
  kfree(kwait_devp);
  kwait_devp = NULL;
}

module_init(kwait_init);
module_exit(kwait_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.11");
