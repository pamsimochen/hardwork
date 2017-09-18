/*
 */
#include <common.h>
#include <command.h>
#include <rtc.h>
#include <tps65911.h>

enum {
	IDX_SEC ,
	IDX_MIN ,
	IDX_HOUR,
	IDX_DAY ,
	IDX_MON ,
	IDX_YEAR,
	IDX_WEEK,
	IDX_NUM
};

static int rtc_freeze(void);
static int rtc_run(void);

int rtc_get (struct rtc_time* tm)
{
	u8 table[IDX_NUM];
	int ret;
	ret = tps65911_read(SECONDS_REG, table, IDX_NUM);
	if(ret)
		return ret;
	tm->tm_year = bcd2bin(table[IDX_YEAR]);
	tm->tm_mon  = bcd2bin(table[IDX_MON]);
	tm->tm_wday = bcd2bin(table[IDX_WEEK]);
	tm->tm_mday = bcd2bin(table[IDX_DAY]);
	tm->tm_hour = bcd2bin(table[IDX_HOUR]);
	tm->tm_min  = bcd2bin(table[IDX_MIN]);
	tm->tm_sec  = bcd2bin(table[IDX_SEC]);

	tm->tm_year += 2000;

	return 0;
}

static int rtc_freeze(void)
{
	int ret;
	u8 temp;
	
	ret = tps65911_read(RTC_CTRL_REG, &temp, 1);
	if(ret)
		return ret;
	temp &= (~0x1);
	ret = tps65911_config(RTC_CTRL_REG, temp);
	if(ret)
		return ret;
	do{
		ret = tps65911_read(RTC_STATUS_REG, &temp, 1);
	}while(ret == 0 && (temp & 0x2) != 0);
	return ret;
}

static int rtc_run(void)
{
	int ret;
	u8 temp;
	
	ret = tps65911_read(RTC_CTRL_REG, &temp, 1);
	if(ret)
		return ret;
	temp |= (0x1);
	ret = tps65911_config(RTC_CTRL_REG, temp);
	if(ret)
		return ret;
	do{
		ret = tps65911_read(RTC_STATUS_REG, &temp, 1);
	}while(ret == 0 && (temp & 0x2) == 0);
	return ret;
}

int rtc_set (struct rtc_time* tm)
{
	u8 table[IDX_NUM];
	int i;
	/* we do NOT support the year < 2000 */
	if(tm->tm_year < 2000)
		return -1;
	else
		tm->tm_year -= 2000;
		
	if(rtc_freeze())
		return -1;
	table[IDX_YEAR] = bin2bcd(tm->tm_year);
	table[IDX_MON]  = bin2bcd(tm->tm_mon);
	table[IDX_DAY]  = bin2bcd(tm->tm_mday);
	table[IDX_WEEK] = bin2bcd(tm->tm_wday);
	table[IDX_HOUR] = bin2bcd(tm->tm_hour);
	table[IDX_MIN]  = bin2bcd(tm->tm_min);
	table[IDX_SEC]  = bin2bcd(tm->tm_sec);
	for(i = IDX_SEC;i < IDX_NUM;i++)
		if(tps65911_config(SECONDS_REG + i, table[i]) != 0)
			return -1;
	if(rtc_run())
		return -1;

	return 0;
}

void rtc_reset (void)
{
	int ret;
	u8 temp;
	
	ret = tps65911_read(DEVCTRL_REG, &temp, 1);
	if(ret)
		return;
	temp |= RTC_PWDN;
	ret = tps65911_config(DEVCTRL_REG, temp);
	if(ret)
		return;
	temp &= (~RTC_PWDN);
	ret = tps65911_config(DEVCTRL_REG, temp);
	if(ret)
		return;

	if(rtc_run())
		return;
}

void rtc_init(void)
{
	int ret;
	u8 temp;
	
	ret = tps65911_read(RTC_STATUS_REG, &temp, 1);
	if(ret)
		return;
	/* check if rtc is running */
	if((temp & 0x2) == 0){
		/* Not running, Make it run */
		rtc_run();
	}
}
