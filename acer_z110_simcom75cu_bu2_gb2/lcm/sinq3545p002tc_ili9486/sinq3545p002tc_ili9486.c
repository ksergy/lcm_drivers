/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#include <linux/string.h>
#ifdef BUILD_UBOOT
#include <asm/arch/mt6575_gpio.h>
#else
#include <mach/mt6575_gpio.h>
#endif
#include "lcm_drv.h"



// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

//#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)
#define LCM_ID (0x9486)
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))

//punk
#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define SET_LSCE_LOW   SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH  SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW   SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH  SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW   SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH  SET_GPIO_OUT(LSDA_GPIO_PIN, 1)

#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSDA_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)

#define GET_LSDA_BIT mt_get_gpio_in(LSDA_GPIO_PIN)


#define CTRL_ID  (0 << 8)
#define DATA_ID  (1 << 8)


static __inline void spi_send_data(unsigned int data)
{
    unsigned int i;

    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        data <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
    
}


static __inline unsigned char  spi_get_byte(unsigned int reg)
{
    unsigned int i;
    int k;
    unsigned char getbyte=0;

    reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        reg <<= 1;
    }
    
    SET_LSDA_INPUT;
    UDELAY(2);    
  //  SET_LSCK_LOW;
   // UDELAY(2);      
       SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);
    
    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte |= (1 << k);
        }
       SET_LSCK_LOW;       
        UDELAY(5);
       SET_LSCK_HIGH;    
        UDELAY(5);
    }    

    UDELAY(2);    
    
    SET_LSDA_OUTPUT;
    
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

    return getbyte;
}

static __inline unsigned char spi_get_3bytes(unsigned int reg,unsigned char data[])
{
	unsigned int i;
	int k;
	unsigned char getbyte1=0;
	unsigned char getbyte2=0;	
	unsigned char getbyte3=0;	
	unsigned char getbyte4=0;	
	unsigned char getbyte5=0;	
	unsigned char getbyte6=0;	

	data[0]=0;
	data[1]=0;
	data[2]=0;
	data[3]=0;
	data[4]=0;
	data[5]=0;

    reg =  (CTRL_ID | (reg & 0xFF));
    
    SET_LSCE_LOW;
    UDELAY(1);
    SET_LSCK_HIGH;
    SET_LSDA_HIGH;
    UDELAY(1);

    for (i = 0; i < 9; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 8)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        reg <<= 1;
    }
    
    SET_LSDA_INPUT;
    UDELAY(2);    
	
    SET_LSCK_LOW;
    UDELAY(2);      
    SET_LSCK_HIGH;        
    UDELAY(2);
   // SET_LSCK_LOW;
    //UDELAY(2);
	
    
    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte1 |= (1 << k);
        }
	SET_LSCK_LOW;
	UDELAY(5);
        SET_LSCK_HIGH;        
        UDELAY(5);

    }    

    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte2 |= (1 << k);
        }
	SET_LSCK_LOW;
	UDELAY(5);
        SET_LSCK_HIGH;        
        UDELAY(5);

    }    

    for (k = 7; k >= 0; k--)
    {
        if (GET_LSDA_BIT) {
            getbyte3 |= (1 << k);
        }
	SET_LSCK_LOW;
	UDELAY(5);
        SET_LSCK_HIGH;        
        UDELAY(5);

    }   	

	for (k = 7; k >= 0; k--)
	{
		if (GET_LSDA_BIT) {
		    getbyte4 |= (1 << k);
		}
	SET_LSCK_LOW;
	UDELAY(5);
        SET_LSCK_HIGH;        
        UDELAY(5);

	}  
	for (k = 7; k >= 0; k--)
	{
		if (GET_LSDA_BIT) {
		    getbyte5 |= (1 << k);
		}
	SET_LSCK_LOW;
	UDELAY(5);
        SET_LSCK_HIGH;        
        UDELAY(5);

	}  
	for (k = 7; k >= 0; k--)
	{
		if (GET_LSDA_BIT) {
			getbyte6 |= (1 << k);
		}
	SET_LSCK_LOW;
	UDELAY(5);
        SET_LSCK_HIGH;        
        UDELAY(5);

	}  
    UDELAY(2);    
    
    SET_LSDA_OUTPUT;
    
    SET_LSDA_HIGH;
    SET_LSCE_HIGH;

	data[0]=getbyte1;
	data[1]=getbyte2;	
	data[2]=getbyte3;	
	data[4]=getbyte1;
	data[5]=getbyte2;	
	data[6]=getbyte3;	
	
    return getbyte1;
}


static int resume_backlight = 0;
static __inline void send_ctrl_cmd(unsigned int cmd)
{
    unsigned int out = (CTRL_ID | (cmd & 0xFF));
    spi_send_data(out);
}

static __inline void send_data_cmd(unsigned int data)
{
    unsigned int out = (DATA_ID  | (data & 0xFF));
    spi_send_data(out);
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}



static __inline unsigned char get_lcm_register_byte(unsigned int regIndex)
{
    unsigned char tmp;
    tmp = spi_get_byte(regIndex);
    return tmp;
}


static unsigned int lcm_backlight_level=0;

static  __inline void backlight_ctrl(unsigned char level)
{
#if 0  //yanfang
	volatile unsigned char temp,temp1,i,j;
	//kal_uint32 savemask;
	if(level==lcm_backlight_level)
	{
		return;
	}
	else
	{
		if(level)
		{
			mt_set_gpio_mode(GPIO_DISP_BL_EN_PIN, 0);
			mt_set_gpio_dir(GPIO_DISP_BL_EN_PIN, GPIO_DIR_OUT);
			mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ZERO);

			switch(level)
			{
				case 1:
				temp=16;//15
				break;
				case 2:
				temp=14;//13
				break;
				case 3:
				temp=12;//10
				break;
				case 4:
				temp=10;//6
				break;
				case 5:
				temp=7;//2
				break;
				default:
				break;
			}	

			switch(lcm_backlight_level)
			{
				case 1:
				temp1=16;
				break;
				case 2:
				temp1=14;
				break;
				case 3:
				temp1=12;
				break;
				case 4:
				temp1=10;
				break;
				case 5:
				temp1=7;
				break;
				case 0:
				temp1=0;
				break;
				default:
				break;
			}

			if(temp1<temp)
			temp=temp-temp1;
			else
			temp=16+temp-temp1;

			// savemask=SaveAndSetIRQMask();

			for(j=0;j<30;j++);
			mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ONE);
			UDELAY(25); //Ton>20us
			for(i=1;i<temp;i++)
			{
				mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ZERO);
				for(j=0;j<30;j++); //0.5us~500us	//16
				mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ONE);
				for(j=0;j<30;j++); //>0.5us			
			}

			//   RestoreIRQMask(savemask);

			mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ONE);
			for(j=0;j<5;j++);
		}
		else
		{
			mt_set_gpio_mode(GPIO_DISP_BL_EN_PIN, 0);
			mt_set_gpio_dir(GPIO_DISP_BL_EN_PIN, GPIO_DIR_OUT);
			mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ZERO);
			UDELAY(100);
		}

		lcm_backlight_level=level;
	}    
#endif
return;
}



static void init_lcm_registers(void)
{
#ifdef BUILD_UBOOT
	printf("init_lcm_registers----3545ci  \r\n");
#else
	printk("init_lcm_registers----3545ci  \r\n");
#endif

	send_ctrl_cmd(0x11);
	MDELAY(100);
/*
	send_ctrl_cmd(0xF1);
	send_data_cmd(0x36);
	send_data_cmd(0x04);
	send_data_cmd(0x00);
	send_data_cmd(0x3c);
	send_data_cmd(0x0F);
	send_data_cmd(0x8F);

	send_ctrl_cmd(0xF2);
	send_data_cmd(0x18);
	send_data_cmd(0xa3);
	send_data_cmd(0x12);
	send_data_cmd(0x02);
	send_data_cmd(0xb2);
	send_data_cmd(0x12);
	send_data_cmd(0xff);
	send_data_cmd(0x10);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xF4);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x08);
	send_data_cmd(0x91);
	send_data_cmd(0x04);
*/
	send_ctrl_cmd(0xF7);
	send_data_cmd(0xA9);
	send_data_cmd(0x91);
	send_data_cmd(0x2D);
	send_data_cmd(0x0A);
	send_data_cmd(0x4F);

	send_ctrl_cmd(0xF8);
	send_data_cmd(0x21);
	send_data_cmd(0x04);

	send_ctrl_cmd(0xF9);
	send_data_cmd(0x00);
	send_data_cmd(0x08);
#if 1
	send_ctrl_cmd(0xE0);
	send_data_cmd(0x00);
	send_data_cmd(0x04);
	send_data_cmd(0x07);
	send_data_cmd(0x01);
	send_data_cmd(0x10);
	send_data_cmd(0x0A);
	send_data_cmd(0x34);
	send_data_cmd(0x9c);
	send_data_cmd(0x47);
	send_data_cmd(0x09);
	send_data_cmd(0x10);
	send_data_cmd(0x0c);
	send_data_cmd(0x1a);
	send_data_cmd(0x1e);
	send_data_cmd(0x1F);

	send_ctrl_cmd(0xE1);
	send_data_cmd(0x00);
	send_data_cmd(0x21);
	send_data_cmd(0x24);
	send_data_cmd(0x03);
	send_data_cmd(0x0f);
	send_data_cmd(0x06);
	send_data_cmd(0x38);
	send_data_cmd(0x36);
	send_data_cmd(0x4b);
	send_data_cmd(0x05);
	send_data_cmd(0x0f);
	send_data_cmd(0x0e);
	send_data_cmd(0x39);
	send_data_cmd(0x3b);
	send_data_cmd(0x1F);
#else
	send_ctrl_cmd(0xE0);
	send_data_cmd(0x0f);
	send_data_cmd(0x17);
	send_data_cmd(0x15);
	send_data_cmd(0x0b);
	send_data_cmd(0x10);
	send_data_cmd(0x08);
	send_data_cmd(0x4a);
	send_data_cmd(0x75);
	send_data_cmd(0x3e);
	send_data_cmd(0x07);
	send_data_cmd(0x13);
	send_data_cmd(0x06);
	send_data_cmd(0x1e);
	send_data_cmd(0x03);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xE1);
	send_data_cmd(0x0f);
	send_data_cmd(0x32);
	send_data_cmd(0x29);
	send_data_cmd(0x07);
	send_data_cmd(0x09);
	send_data_cmd(0x03);
	send_data_cmd(0x44);
	send_data_cmd(0x54);
	send_data_cmd(0x34);
	send_data_cmd(0x04);
	send_data_cmd(0x10);
	send_data_cmd(0x03);
	send_data_cmd(0x21);
	send_data_cmd(0x21);
	send_data_cmd(0x00);
#endif

	send_ctrl_cmd(0xB1);
	send_data_cmd(0xb0);//0xd0
	send_data_cmd(0x11);

	send_ctrl_cmd(0xB4);
	send_data_cmd(0x00); //0x02

	send_ctrl_cmd(0xB0);
	send_data_cmd(0x03);

	send_ctrl_cmd(0xB6);
	send_data_cmd(0x22);
	send_data_cmd(0x22);
	send_data_cmd(0x3B);
/*
	send_ctrl_cmd(0xB7);
	send_data_cmd(0xc6);

	send_ctrl_cmd(0xC0);
	send_data_cmd(0x0d);
	send_data_cmd(0x0d);

	send_ctrl_cmd(0xC1);
	send_data_cmd(0x42);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xC2);
	send_data_cmd(0x00);
*/
	send_ctrl_cmd(0xC5);
	send_data_cmd(0x00);
	send_data_cmd(0x4a);  //0x19

	send_ctrl_cmd(0x36);
	send_data_cmd(0x08); //0x08

	send_ctrl_cmd(0x3A);
	send_data_cmd(0x66); // 8bit,16bit=0x55 18bit=0x66  

	//send_ctrl_cmd(0x21);

	send_ctrl_cmd(0x11);

	MDELAY(150);
	send_ctrl_cmd(0x29);

	send_ctrl_cmd(0x2c);	//punk


}

static void config_gpio(void)
{
    const unsigned int USED_GPIOS[] = 
    {
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN
    };

    unsigned int i;

    //lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_DISP_LSCK_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);

    for (i = 0; i < ARY_SIZE(USED_GPIOS); ++ i)
    {
        lcm_util.set_gpio_dir(USED_GPIOS[i], 1);               // GPIO out
        lcm_util.set_gpio_pull_enable(USED_GPIOS[i], 0);
    }

    // Swithc LSA0 pin to GPIO mode to avoid data contention,
    // since A0 is connected to LCM's SPI SDO pin
    //
    //lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_init(void)
{
#ifdef BUILD_UBOOT
	printf("UBOOT lcm_init\r\n");
#else
	printk("kennel lcm_init\r\n");
#endif

	config_gpio();
	SET_RESET_PIN(1);
	MDELAY(0);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(150);

	init_lcm_registers();

}
static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DPI;
	params->ctrl   = LCM_CTRL_GPIO;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	//11.7M pclk
	params->dpi.mipi_pll_clk_ref  = 0;
	params->dpi.mipi_pll_clk_div1 = 26;
	params->dpi.mipi_pll_clk_div2 = 15;
	params->dpi.dpi_clk_div       = 2;	//punk 2
	params->dpi.dpi_clk_duty      = 1;	//punk 1


	params->dpi.clk_pol           = LCM_POLARITY_RISING;	//punk
	params->dpi.de_pol            = LCM_POLARITY_FALLING;	//LCM_POLARITY_FALLING
	params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

	params->dpi.hsync_pulse_width = 10;//punk 20
	params->dpi.hsync_back_porch  = 50;		//punk 8 20
	params->dpi.hsync_front_porch = 50;		//punk 8 20
	params->dpi.vsync_pulse_width = 10;
	params->dpi.vsync_back_porch  = 50;
	params->dpi.vsync_front_porch = 50;

	params->dpi.format            = LCM_DPI_FORMAT_RGB666;
	params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
	params->dpi.is_serial_output  = 0;

	params->dpi.intermediat_buffer_num = 2;

	params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;

}

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;
return 1;	
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	send_ctrl_cmd(0xb0);
	
	send_ctrl_cmd(0xb0);
	send_data_cmd(0x83);
	
	send_ctrl_cmd(0xfb);
	send_data_cmd(0x12);
	//send_ctrl_cmd(0xd3);
	//GetByte(&id1);
	id1=spi_get_byte(0xd3);
	
	send_ctrl_cmd(0xfb);
	send_data_cmd(0x13);
	//send_ctrl_cmd(0xd3);
	//GetByte(&id2);
	id2=spi_get_byte(0xd3);

	id = (id1<<8)|id2;

#ifdef BUILD_UBOOT
	printf("3545ci___lcd_id1=%x,lcd_id2=%x,lcd_id=%x  \r\n",id1,id2,id);
#else
	printk("3545ci___lcd_id1=%x,lcd_id2=%x,lcd_id=%x  \r\n",id1,id2,id);
#endif
	
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
       MDELAY(30);
    return (LCM_ID == id)?1:0;

}

#if 0  //lk
#ifdef BUILD_UBOOT
#define ADC_CHECK_COUNT	5
extern int IMM_GetOneChannelValue(int dwChannel);

static unsigned int lcm_check_idpin_adc(unsigned char count)
{
    int idpin_auxadc = 0;
    int idpin_auxadc_check = 0;
    int i = 0;

    for(i = 0;i < count;i++)
    {
        idpin_auxadc_check = IMM_GetOneChannelValue(AUXADC_LCM_IDPIN_CHANNEL);
        idpin_auxadc += idpin_auxadc_check; 
        printf("LK+++:LCM Check ID pin ADC value[%d]=%d\n",i,idpin_auxadc_check);
    }
    idpin_auxadc = (idpin_auxadc / count);
    printf("LK+++:LCM Check ID pin ADC average value=%d\n",idpin_auxadc);
    return idpin_auxadc;
}
#endif

static unsigned int lcm_compare_id(void)
{
	unsigned int    id1 = 0;
	unsigned int    id2 = 0;
	unsigned int    id = 0;

#ifdef BUILD_UBOOT
    unsigned int    lcm_idpin_value = 0;

	lcm_idpin_value = lcm_check_idpin_adc(ADC_CHECK_COUNT);
#endif

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);//LK@0314,at least 120ms

	set_lcm_register(0xF000,0x55);             
	set_lcm_register(0xF001,0xAA);             
	set_lcm_register(0xF002,0x52);             
	set_lcm_register(0xF003,0x08);             
	set_lcm_register(0xF004,0x01);

	send_ctrl_cmd(0xc500);
	id1 = spi_read_data();
	MDELAY(20);
	send_ctrl_cmd(0xc501);
	id2 = spi_read_data();
	id = (id1<<8)|id2;

#ifdef BUILD_UBOOT
	printf("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#else
	printk("lcd_id1=0x%x,lcd_id2=0x%x,lcd_id=0x%x  \r\n",id1,id2,id);
#endif

#ifdef BUILD_UBOOT
	if((LCM == id)&&(lcm_idpin_value < 180) && (lcm_idpin_value > 100))
#else
   if(LCM == id)
#endif
{
return 1;
}
else
{
return 0;
}
   }
#endif

static void lcm_suspend(void)
{
    send_ctrl_cmd(0x28);
    MDELAY(10);
    send_ctrl_cmd(0x10);
    MDELAY(120);
}


static void lcm_resume(void)
{
    send_ctrl_cmd(0x11);
    MDELAY(150);
    send_ctrl_cmd(0x29);
    send_ctrl_cmd(0x2c);
    MDELAY(50);
	resume_backlight= 1;
} 

static void lcm_setbacklight(unsigned int level)
{
#ifdef BUILD_UBOOT
    printf("UBOOT lcm_setbacklight level=%d\r\n",level);
#else
    printk("kennel lcm_setbacklight level=%d \r\n",level);
#endif
if(resume_backlight == 1)
{
	MDELAY(20);
}
    backlight_ctrl(level);
resume_backlight = 0;	
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER sinq3545p002tc_ili9486_lcm_drv = 
{
    .name			= "sinq3545p002tc_ili9486",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.set_backlight = lcm_setbacklight,//LK
	.compare_id     = lcm_compare_id
};



