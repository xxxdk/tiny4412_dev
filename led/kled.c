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

#define DEV_NAME  "kled"
#define GPM4COM   0x110002E0
#define LED_MAJOR 233

volatile unsigned long *led_config = NULL;
volatile unsigned long *led_dat = NULL;

static int led_major = LED_MAJOR;

struct led_dev{
  struct cdev cdev;
};

struct led_dev *led_devp;

int led_open(struct inode *inode, struct file *filp)
{
  printk("led_open\n");
  *led_config &= ~(0xffff);
  *led_config |= (0x1111);

  return 0;
}

int led_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  int ret,val;
  ret = copy_from_user(&val,buf,count);
  if(ret != count){
    printk("copy_from_user error!\n");
    return -1;
  }
  switch (val)
  {
    case 0:
      printk(KERN_EMERG"led1_on\n");
      *led_dat &= ~0x1;
      break;
    case 1:
      printk(KERN_EMERG"led2_on\n");
      *led_dat &= ~0x2;
      break;
    case 2:
      printk(KERN_EMERG"led3_on\n");
      *led_dat &= ~0x4;
      break;
    case 3:
      printk(KERN_EMERG"led4_on\n");
      *led_dat &= ~0x8;
      break;
    case 4:
      printk(KERN_EMERG"led_all_on\n");
      *led_dat &= ~0xf;
      break;
    case 5:
      printk(KERN_EMERG"led_all_off\n");
      *led_dat |= 0xf;
      break;
  }
  return 0;
}

int led_close(struct inode *inode, struct file *filp)
{
  printk("led_all_close\n");
  *led_dat |= 0xf;
  return 0;
}

struct file_operations led_fops = {
  .owner = THIS_MODULE,
  .open = led_open,
  .release = led_close,
  .write = led_write,
};

static void led_setup_cdev(struct led_dev *dev, int index)
{
  int err, devno;
  devno = MKDEV(led_major, index);
  cdev_init(&dev->cdev, &led_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &led_fops;
  err = cdev_add(&dev->cdev, devno, 1);

  if(err){
    printk(KERN_NOTICE "Error %d add cdevdemo %d", err, index);
  }
}

static __init int test_init(void)
{
  int ret;
  dev_t devno;
  struct class *led_class;

  printk(KERN_NOTICE "led_init\n");
  devno = MKDEV(led_major, 0);

  if(led_major){
    printk(KERN_NOTICE "======== cdevdemo_init 1");
    ret = register_chrdev_region(devno, 1, DEV_NAME);
  }else{
    printk(KERN_NOTICE "======== cdevdemo_init 2");
    ret = alloc_chrdev_region(&devno,0,1,DEV_NAME);
    led_major = MAJOR(devno);
  }

  if(ret < 0){
    printk(KERN_NOTICE "======== cdevdemo_init 3");
    return ret;
  }

  led_devp = kmalloc(sizeof(struct led_dev),GFP_KERNEL);
  if(!led_devp){
    ret = -ENOMEM;
    printk(KERN_NOTICE "Error add cdevdemo");
    goto fail_malloc;
  }

  memset(led_devp,0,sizeof(struct led_dev));
  printk(KERN_NOTICE "======== cdevdemo_init 3");
  led_setup_cdev(led_devp, 0);

  led_class = class_create(THIS_MODULE,DEV_NAME);
  device_create(led_class, NULL, MKDEV(led_major, 0), NULL, DEV_NAME);
  printk(KERN_NOTICE "======== cdevdemo_init 4");

  led_config = (volatile unsigned long *)ioremap(GPM4COM,16);
//  led_dat = led_config + 1;
  led_dat = (volatile unsigned long *)ioremap(GPM4COM+1,16);

  return 0;

  fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static __exit void test_exit(void)
{
  printk("led_exit\n");
  cdev_del(&led_devp->cdev);
  unregister_chrdev_region(MKDEV(led_major,0),1);
  kfree(led_devp);
  iounmap(led_config);
  iounmap(led_dat);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("20118.03.14");
