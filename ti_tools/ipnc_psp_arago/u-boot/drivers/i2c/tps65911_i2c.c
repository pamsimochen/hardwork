#include <common.h>
#include <i2c.h>
#include <tps65911.h>

extern void rtc_init(void);

int tps65911_init()
{
	rtc_init();
	return 0;
}

int tps65911_config(u8 addr, u8 val)
{
	int ret;
	int old_bus;

	old_bus = I2C_GET_BUS();
	I2C_SET_BUS(0);
	ret = i2c_write(TPS65911_SLAVE_ADDR, addr, 0x1, &val, 0x1);
	if (ret != 0) {
		puts ("Error writing tps65911.\n");
	}
	if(old_bus != 0)
		I2C_SET_BUS(old_bus);
	return ret;
}

int tps65911_read(u8 addr, u8* buf, int length)
{
	int old_bus;

	old_bus = I2C_GET_BUS();
	I2C_SET_BUS(0);
	if (i2c_read(TPS65911_SLAVE_ADDR, addr, 0x1, buf, length) != 0) {
		puts ("Error reading tps65911.\n");
		return -1;
	}
	if(old_bus != 0)
		I2C_SET_BUS(old_bus);
	return 0;
}
