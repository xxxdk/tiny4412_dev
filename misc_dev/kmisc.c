#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#define DEV_NAME  "kmisc"

int kmisc_dev_open(struct inode *inode, struct file *file)
{
  printk(KERN_NOTICE "KMISC DEV OPEN\n");
  return 0;
}

int kmisc_dev_close(struct inode *inode, struct file *file)
{
  printk(KERN_NOTICE "KMISC DEV CLOSE\n");
  return 0;
}

struct file_operations kfops = {
  .owner = THIS_MODULE,
  .open = kmisc_dev_open,
  .release = kmisc_dev_close,
};

struct miscdevice kmisc_dev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = DEV_NAME,
  .fops = &kfops,
};

static __init int kmisc_dev_init(void)
{
  int ret, ret_err;
  ret = misc_register(&kmisc_dev);
  if(ret != 0){
    ret_err = ret;
    printk(KERN_NOTICE "MISC REGISTER FAIL\n");
    goto fail;
  }
  printk(KERN_NOTICE "KMISC DEV INIT SUCCESS\n");
  return ret;
fail:
  return ret_err;
}

static __exit void kmisc_dev_exit(void)
{
  misc_deregister(&kmisc_dev);
}

module_init(kmisc_dev_init);
module_exit(kmisc_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.12");
