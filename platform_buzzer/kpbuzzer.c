#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>


#define DEV_NAME  "kpbuzzer"
#define GPD0CON   0x114000A0
#define BELL_MAJOR  234


volatile unsigned long *bell_config = NULL;
volatile unsigned long *bell_dat = NULL;

static int bell_major = BELL_MAJOR;

struct bell_dev{
  struct cdev cdev;
};

static struct bell_dev *bell_devp;

static int bell_open(struct inode *inode, struct file *filp)
{
  printk(KERN_NOTICE "buzzer open\n");
  *bell_config &= ~(0xf);
  *bell_config |= (0x1);
  return 0;
}

static int bell_ioctl(struct file *filp, unsigned int request, unsigned long arg)
{
  switch(request){
  case 0:
    printk(KERN_NOTICE "BELL ON\n");
    *bell_dat |= 0x1;
    break;
  case 1:
    printk(KERN_NOTICE "BELL OFF\n");
    *bell_dat &= ~0x1;
    break;
  default:
    printk(KERN_NOTICE "BELL FAIL\n");
    break;
  }
  return 0;
}

static int bell_close(struct inode *inode, struct file *filp)
{
  printk(KERN_NOTICE "BELL CLOSE\n");
  *bell_dat &= ~0x1;
  return 0;
}

static struct file_operations bell_fop = {
  .owner = THIS_MODULE,
  .open = bell_open,
  .release = bell_close,
  .unlocked_ioctl = bell_ioctl,
};

static void dev_setup_cdev(struct bell_dev *dev, int index)
{
  int err, devno;
  devno = MKDEV(bell_major, index);
  cdev_init(&dev->cdev, &bell_fop);
  dev->cdev.owner = THIS_MODULE;
  err = cdev_add(&dev->cdev, devno, 1);
  if(err){
    printk(KERN_NOTICE "Error %d add cdev %d", err, index);
  }
}

static int bell_probe(struct platform_device *pdev)
{
  int ret;
  dev_t devno;
  struct class *bell_class;

  printk(KERN_NOTICE "BELL INIT\n");
  devno = MKDEV(bell_major, 0);

  if(bell_major){
    printk(KERN_NOTICE "REGISTER CHRDEV\n");
    ret = register_chrdev_region(devno,1,DEV_NAME);
  }else{
    printk(KERN_NOTICE "ALLOC CHRDEV\n");
    ret = alloc_chrdev_region(&devno,0,1,DEV_NAME);
    bell_major = MAJOR(devno);
  }
  if(ret < 0){
    return ret;
  }

  bell_devp = devm_kzalloc(&pdev->dev,sizeof(struct bell_dev),GFP_KERNEL);
  if(!bell_devp){
    ret = -ENOMEM;
    printk(KERN_NOTICE "ERROR ADD CDEV");
    goto fail_malloc;
  }

  dev_setup_cdev(bell_devp, 0);
  bell_class = class_create(THIS_MODULE,DEV_NAME);
  device_create(bell_class, NULL ,MKDEV(bell_major,0), NULL, DEV_NAME);
  bell_config = (volatile unsigned long *)ioremap(GPD0CON, 16);
  bell_dat = bell_config + 1;

  return 0;

  fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static int bell_remove(struct platform_device *pdev)
{
  printk(KERN_NOTICE "BELL EXIT\n");
  cdev_del(&bell_devp->cdev);
  unregister_chrdev_region(MKDEV(bell_major,0),1);
  kfree(bell_devp);
  iounmap(bell_config);
  iounmap(bell_dat);

  return 0;
}

static struct platform_driver kpbuzzer_driver = {
  .driver = {
    .name = DEV_NAME,
    .owner = THIS_MODULE,
  },
  .probe = bell_probe,
  .remove = bell_remove,
};

module_platform_driver(kpbuzzer_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.03.27");
