#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <string.h>
#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#endif


/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */

#define FRAME_WIDTH  (1080)
#define FRAME_HEIGHT (1920)

#define REGFLAG_DELAY               (0xFE)
#define REGFLAG_END_OF_TABLE        (0xFD)	/* END OF REGISTERS MARKER */
/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

#ifdef BUILD_LK
#define GPIO_LP3101_ENN   GPIO_LCD_BIAS_ENN_PIN
#define GPIO_LP3101_ENP   GPIO_LCD_BIAS_ENP_PIN
#endif


static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)\
			(lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update))

#define dsi_set_cmdq(pdata, queue_size, force_update)\
			(lcm_util.dsi_set_cmdq(pdata, queue_size, force_update))

#define wrtie_cmd(cmd)\
			(lcm_util.dsi_write_cmd(cmd))

#define write_regs(addr, pdata, byte_nums)\
			(lcm_util.dsi_write_regs(addr, pdata, byte_nums))

#define read_reg(cmd)\
			(lcm_util.dsi_dcs_read_lcm_reg(cmd))

#define read_reg_v2(cmd, buffer, buffer_size)\
			(lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size))

#define dsi_lcm_set_gpio_out(pin, out)						(lcm_util.set_gpio_out(pin, out))
#define dsi_lcm_set_gpio_mode(pin, mode)					(lcm_util.set_gpio_mode(pin, mode))
#define dsi_lcm_set_gpio_dir(pin, dir)						(lcm_util.set_gpio_dir(pin, dir))
#define dsi_lcm_set_gpio_pull_enable(pin, en)				(lcm_util.set_gpio_pull_enable(pin, en))

#define   LCM_DSI_CMD_MODE							(0)

static unsigned int esd_last_backlight = 255;
#ifndef BUILD_LK
#define set_gpio_lcd_enp(cmd) lcm_util.set_gpio_lcd_enp_bias(cmd)
#define set_gpio_lcd_enn(cmd) lcm_util.set_gpio_lcd_enn_bias(cmd)

#endif

#ifdef BUILD_LK
#define LP3101_SLAVE_ADDR_WRITE  0x7C
static struct mt_i2c_t lp3101_i2c;

static int lp3101_write_bytes(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0] = addr;
	write_data[1] = value;

	lp3101_i2c.id = I2C0;
	/* Since i2c will left shift 1 bit, we need to set FAN5405 I2C address to >>1 */
	lp3101_i2c.addr = (LP3101_SLAVE_ADDR_WRITE >> 1);
	lp3101_i2c.mode = ST_MODE;
	lp3101_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&lp3101_i2c, write_data, len);
	/* LCD_DEBUG("%s: i2c_write: ret_code: %d\n", __func__, ret_code); */
	if (ret_code < 0)
		printf("lp3101 write data fail 1 !!\n");
	return ret_code;
}
#endif

static void init_lcm_registers(void)
{

	unsigned int data_array[16];

	data_array[0] = 0x00043902;
	data_array[1] = 0x050778FF;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00001100;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00001201;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00002202;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00006003;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00000204;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x000778FF;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00000068;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00000155;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00000051;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00002C53;
	dsi_set_cmdq(data_array, 2, 1);


	data_array[0] = 0x00013902;
	data_array[1] = 0x00000011;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(120);

	data_array[0] = 0x00013902;
	data_array[1] = 0x00000029;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);
	data_array[0] = 0x00013902;
	data_array[1] = 0x00000035;
	dsi_set_cmdq(data_array, 2, 1);
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode = BURST_VDO_MODE;
#endif

	/* DSI */
	/* Command mode setting */
	/* 1 Three lane or Four lane */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 8;
	params->dsi.vertical_backporch = 24;
	params->dsi.vertical_frontporch = 8;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 40;
	params->dsi.horizontal_backporch = 120;
	params->dsi.horizontal_frontporch = 120;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;
	/* params->dsi.pll_select=1;     //0: MIPI_PLL; 1: LVDS_PLL */
	/* Bit rate calculation */
	/* 1 Every lane speed */
	params->dsi.PLL_CLOCK = 500;
	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range = 4;
	params->dsi.pll_div1 = 0;	/* div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps */
	params->dsi.pll_div2 = 0;	/* div2=0,1,2,3;div1_real=1,2,4,4 */
	params->dsi.fbk_div = 0x13;	/* 0x12  // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real) */
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd			= 0x0a;
	params->dsi.lcm_esd_check_table[0].count		= 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
}
static void lcm_init_power(void)
{
		int ret = 0;
		unsigned char cmd = 0x0;
		unsigned char data = 0xFF;

#ifdef BUILD_LK
		/* data sheet 136 page ,the first AVDD power on */
		mt_set_gpio_mode(GPIO_LP3101_ENP, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LP3101_ENP, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LP3101_ENP, GPIO_OUT_ONE);
		MDELAY(5);
		mt_set_gpio_mode(GPIO_LP3101_ENN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LP3101_ENN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LP3101_ENN, GPIO_OUT_ONE);
#else
		/* set_gpio_lcd_power_enable(1); */
		/* set_gpio_lcd_enp(1); */
		set_gpio_lcd_enp(1);
		MDELAY(2);
		set_gpio_lcd_enn(1);
#endif
		cmd = 0x00;
		data = 0x0F;
		ret = lp3101_write_bytes(cmd, data);
		MDELAY(2);
		cmd = 0x01;
		data = 0x0F;
		ret = lp3101_write_bytes(cmd, data);
		MDELAY(10);
}
static void lcm_suspend_power(void)
{
#ifndef BUILD_LK
		set_gpio_lcd_enn(0);
		MDELAY(5);
		set_gpio_lcd_enp(0);

#endif
}

static void lcm_resume_power(void)
{
#ifndef BUILD_LK
	lcm_init_power();
#endif
}


static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(5);
	SET_RESET_PIN(1);
	MDELAY(20);
	init_lcm_registers();
}

static void lcm_suspend(void)
{
		unsigned int data_array[16];

		data_array[0] = 0x00280500; /* Display Off */
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(20);

		data_array[0] = 0x00100500; /* Sleep In */
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(120);

}


static void lcm_resume(void)
{
#ifndef BUILD_LK
	lcm_init();
#endif
}


#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
		       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);
	unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
	unsigned char y0_LSB = (y0 & 0xFF);
	unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
	unsigned char y1_LSB = (y1 & 0xFF);

	unsigned int data_array[16];

	data_array[0] = 0x00053902;
	data_array[1] =
	    (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] =
	    (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[2] = (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif
static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0;
	unsigned char buffer[2];
	unsigned int array[16];
	unsigned int data_array[16];

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(100);

	data_array[0] = 0x00043902;
	data_array[1] = 0x010778FF;
	dsi_set_cmdq(data_array, 2, 1);

	array[0] = 0x00023700;	/* read id return two byte,version and id */
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x00, buffer, 2);
	id = buffer[0];		/* we only need ID */
#ifdef BUILD_LK
	dprintf(0, "%s, LK ili7807d debug: ili7807d 0x%08x\n", __func__,
	       id);
#else
	pr_info("%s,kernel ili7807d horse debug: ili7807d id = 0x%08x\n",
	       __func__, id);
#endif
	if (id == 0x78)
		return 1;
	else
		return 0;

}


struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 2, {0xFF, 0xFF} } ,

	{REGFLAG_END_OF_TABLE, 0x00, {} }
};
static struct LCM_setting_table lcm_cabc_mode_setting[] = {
	{0x55, 1, {0x00} } ,
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};




static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {
		unsigned cmd;

		cmd = table[i].cmd;

		switch (cmd) {
		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
#ifdef BUILD_LK
			dprintf(0, "[LK]push_table end\n");
#endif
			break;
		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}
}


static void lcm_set_backlight(unsigned int level)
{
	unsigned int value = level;

	lcm_backlight_level_setting[0].para_list[0] = ((value>>4)&0x0f);
	lcm_backlight_level_setting[0].para_list[1] = (value<<4)&0xf0;
	push_table(lcm_backlight_level_setting,
			sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);

}

static void lcm_set_backlight_cmdq(void *handle, unsigned int level)
{
	unsigned int value = level;
	esd_last_backlight =  level;

#ifndef BUILD_LK
	pr_info("call %s, level=%d\n", __func__, level);
#endif
	lcm_backlight_level_setting[0].para_list[0] = ((value>>4)&0x0f);
	lcm_backlight_level_setting[0].para_list[1] = (value<<4)&0xf0;
	push_table(lcm_backlight_level_setting,
			sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);

}

static void lcm_esd_recover_backlight(void)
{
	lcm_backlight_level_setting[0].para_list[0] = ((esd_last_backlight>>4)&0x0f);
	lcm_backlight_level_setting[0].para_list[1] = (esd_last_backlight<<4)&0xf0;
	push_table(lcm_backlight_level_setting,
			sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);

}

#ifndef BUILD_LK
static int lcm_set_cabc_mode(int mode)
{
	unsigned int value = 0;

	printk(KERN_ALERT"call %s, mode=%d\n", __func__, mode);

	switch (mode) {
	case OFF:
		value = 0x00;
		break;
	case UI:
		value = 0x01;
		break;
	case STILL_IMAGE:
		value = 0x02;
		break;
	case MOVING_IMAGE:
		value = 0x03;
		break;
	}
	lcm_cabc_mode_setting[0].para_list[0] = value;
	push_table(lcm_cabc_mode_setting, sizeof(lcm_cabc_mode_setting) / sizeof(struct LCM_setting_table), 1);
	return 0;
}
#endif



LCM_DRIVER ili7807d_fhd_dsi_vdo_djn_lcm_drv = {
	.name = "ili7807d_fhd_dsi_vdo_djn",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init_power = lcm_init_power,
	.suspend_power = lcm_suspend_power,
	.resume_power = lcm_resume_power,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.update = lcm_update,
#endif
	.set_backlight = lcm_set_backlight,
#ifndef BUILD_LK
	.set_backlight_cmdq = lcm_set_backlight_cmdq,
	.set_cabc_mode = lcm_set_cabc_mode,
	.esd_recover_backlight = lcm_esd_recover_backlight,
#endif
};
