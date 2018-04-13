#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>

#define DEV_NAME  "ksmisc"

static int ksmisc_open(struct inode *inode, struct file *file)
{
  printk(KERN_NOTICE "ksmisc open\n");
  return 0;
}

struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = ksmisc_open,
};

struct miscdevice kmisc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = DEV_NAME,
  .fops = &fops,
};

static int __init ksmisc_init(void)
{
  int ret;
  ret = misc_register(&kmisc);
  if(ret != 0)
    return ret;
  printk(KERN_NOTICE "ksmisc init\n");
  ksmisc_open(NULL, NULL);
  return ret;
}

static void __exit ksmisc_exit(void)
{
  misc_deregister(&kmisc);
}

module_init(ksmisc_init);
module_exit(ksmisc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.04.13");
