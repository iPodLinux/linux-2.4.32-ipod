/* */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/i2c-id.h>

#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/gpio.h>


/* GPIO E3 - SDA
 * GPIO E4 - SCL
*/

#define PIN_SDA (1<<3)
#define PIN_SCL (1<<4)

static void oki_ml67x_i2c_inc_use(struct i2c_adapter *adap)
{
	MOD_INC_USE_COUNT;
}

static void oki_ml67x_i2c_dec_use(struct i2c_adapter *adap)
{
	MOD_DEC_USE_COUNT;
}


static void oki_ml67x_bit_setscl(void *data, int val)
{
	gpio_change(OKI_GPPOE, val ? PIN_SCL : 0x0, val ? 0x0 : PIN_SCL);
	gpio_change(OKI_GPPME, val ? 0x0 : PIN_SCL, val ? PIN_SCL : 0x0);
}

static void oki_ml67x_bit_setsda(void *data, int val)
{
	gpio_change(OKI_GPPOE, val ? PIN_SDA : 0x0, val ? 0x0 : PIN_SDA);
	gpio_change(OKI_GPPME, val ? 0x0 : PIN_SDA, val ? PIN_SCL : 0x0);
}

static int oki_ml67x_bit_getscl(void *data)
{
	gpio_change(OKI_GPPME, 0x00, PIN_SCL);
	return (__raw_readl(OKI_GPPIE) & PIN_SCL) ? 1 : 0;
}	

static int oki_ml67x_bit_getsda(void *data)
{
	gpio_change(OKI_GPPME, 0x00, PIN_SDA);
	return (__raw_readl(OKI_GPPIE) & PIN_SDA) ? 1 : 0;
}	


struct i2c_algo_bit_data oki_ml67x_bit_data = {
	.setsda		= oki_ml67x_bit_setsda,
	.setscl		= oki_ml67x_bit_setscl,
	.getsda		= oki_ml67x_bit_getsda,
	.getscl		= oki_ml67x_bit_getscl,
	.udelay		= 10,
	.mdelay		= 10,
	.timeout	= 100
};

struct i2c_adapter oki_ml67x_i2c_adapter = {
	.name 		= "OKI_ML67X I2C Adapter",
	.id		= I2C_ALGO_EXP,
	.algo_data	= &oki_ml67x_bit_data,
	.inc_use	= oki_ml67x_i2c_inc_use,
	.dec_use	= oki_ml67x_i2c_dec_use,
};

int __init oki_ml67x_i2c_init(void)
{
	int res;

	gpctl_modify(0x00, OKI_GPCTL_I2C);

	res = i2c_bit_add_bus(&oki_ml67x_i2c_adapter);
	if (res != 0) {
		printk(KERN_ERR "ERROR: Could not install OKI ML67X I2C\n");
		return res;
	}

	printk(KERN_INFO "gpctl set to %04x\n", __raw_readl(OKI_GPCTL)); 

	return 0;
}

void __exit oki_ml67x_i2c_exit(void)
{
	i2c_bit_del_bus(&oki_ml67x_i2c_adapter);
}

module_init(oki_ml67x_i2c_init);
module_exit(oki_ml67x_i2c_exit);
