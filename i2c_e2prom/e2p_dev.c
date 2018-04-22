#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>

static struct i2c_client *e2p_client;
static struct i2c_board_info e2p_info={
  I2C_BOARD_INFO("at24c08", 0x50);
};

static int e2p_dev_init(void)
{
  struct i2c_adapter *i2c_adap;

  i2c_adap = i2c_get_adapter(0);
  e2p_client = i2c_new_device(i2c_adap, &e2p_info);
  i2c_put_adapter(i2c_adap);

  return 0;
}

static void e2p_dev_exit(void)
{
  i2c_unregister_device(e2p_client);
}

module_init(e2p_dev_init);
module_exit(e2p_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("X.D.KAI");
MODULE_VERSION("2018.04.23");
