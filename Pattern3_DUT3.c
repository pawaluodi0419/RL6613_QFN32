#include "platform.h"

u8 _by_Pattern3_dut3()
{
	static u8 read_adc_index = 0;
	static u8 read_adc_count = 5;
	u8 i;
	u8 Buff_dut3_XGPIO_0[8];
	u16 vmon_1v_adc_code_high;
	u16 vmon_1v_adc_code_low;
	u16 vmon_1v_adc_code;
	static u16 vmon_1v_adc_code_sum = 0;
	u16 vmon_1v_adc_code_average;

	switch(dut3.g_pattern_step)
	{
	//1.VMON切换到ADC 1V: XGPIO13设定输出low, XGPIO4设定输出low
	//2.Rp relay control: XGPIO0设定输出low, XGPIO2设定输出high, XGPIO3设定输出low
	case 0x0000:
	{
		if(dut3.g_dut_pattern_status_buf[7] == 0x00)
		{
			Buff_dut3_XGPIO_0[0] = 0x24;                            //REG0005 ouput value[7:0]
			Buff_dut3_XGPIO_0[1] = 0xC2;							//REG0006
			Buff_dut3_XGPIO_0[2] = 0x00;							//REG0007 output value[15:8]
			Buff_dut3_XGPIO_0[3] = 0xDF;							//REG0008
			Buff_dut3_XGPIO_0[4] = 0x00|(dut3.g_uartPatternNum); 	//REG0009 output value[23:16]
			Buff_dut3_XGPIO_0[5] = 0xE0;							//REG000a
			Buff_dut3_XGPIO_0[6] = 0x0A;							//REG000b output value[31:24]
			Buff_dut3_XGPIO_0[7] = 0xF5;							//REG000c

			XGpio_dut3_Relay_WriteByte(XPAR_AXI_GPIO_dut3_1_BASEADDR,Buff_dut3_XGPIO_0);
			dut3.g_dut_pattern_status_buf[7] = 0x01;
			//xil_printf("PatternNum = %x\r\n",dut3.g_uartPatternNum);
			xil_printf("dut3.pattern3_gpio_control_completed!\r\n");
			msdelay(10);
		}
		else if(dut3.g_dut_pattern_status_buf[7] == 0x01)
		{
			if(dut3.g_relay_control_timer == 0 )
			{
				dut3.g_dut_pattern_status_buf[7] = 0x00;
				dut3.g_pattern_step++;
			}
		}
		//output fail result
		else
		{
			dut3.g_result_fail = 0x01;
			dut3.g_result_fail_tmrcount = 0xffff;
		}
		break;
	}

	//write pattern index
	case 0x0001:
	{
		dut3.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writemem;
		dut3.g_pattern_smbus_control_buf[2] = 0x33;
		dut3.g_pattern_smbus_control_buf[3] = 0xdf;
		dut3.g_pattern_smbus_control_buf[4] = 0x01;
		dut3.g_pattern_smbus_control_buf[5] = 0x03;

		smbus3_irq_handle(dut3.g_pattern_smbus_control_buf);
		if(dut3.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut3 write pattern index =%x\r\n", dut3.g_pattern_smbus_control_buf[5]);
			for(i=1; i<31; i++)
			{
				dut3.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut3.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut3.g_pattern_step++;
			//msdelay(1660);
		}
		break;
	}

	//polling ack bit
	case 0x0002:
	{
		dut3.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut3.g_pattern_smbus_control_buf[2] = 0x33;
		dut3.g_pattern_smbus_control_buf[3] = 0xdf;
		dut3.g_pattern_smbus_control_buf[4] = 0x01;

		smbus3_irq_handle(dut3.g_pattern_smbus_control_buf);
		if(dut3.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut3 polling pattern ack bit =%x\r\n", dut3.g_pattern_smbus_control_buf[10]);
			if(dut3.g_pattern_smbus_control_buf[10] == 0x83)
			{
				for(i=1; i<31; i++)
				{
					dut3.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut3.g_pattern_timer = 0xfff;
				dut3.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut3.g_pattern_step = 0x00;
				dut3.g_dut_pattern_status_buf[2]++;
				dut3.g_uartPatternEnable = 0x00;
				dut3.g_uartPatternNum = (dut3.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut3_1_BASEADDR,dut3.g_uartPatternNum);
				xil_printf("dut3.pattern3_pass!\r\n\r\n");
			}
			else
			{
				dut3.g_result_fail = 0x01;
				dut3.g_result_fail_tmrcount = 0xffff;
			}
		}
		break;
	}
/*
	//读取VMON 1V ADC code的高8bit
	case 0x0003:
	{
		dut3.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut3.g_pattern_smbus_control_buf[2] = 0x51;
		dut3.g_pattern_smbus_control_buf[3] = 0xda;
		dut3.g_pattern_smbus_control_buf[4] = 0x01;

		smbus3_irq_handle(dut3.g_pattern_smbus_control_buf);
		if(dut3.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			//xil_printf("dut3.g_pattern_smbus_control_buf[10] =%x\r\n", dut3.g_pattern_smbus_control_buf[10]);
			vmon_1v_adc_code_high = (dut3.g_pattern_smbus_control_buf[10]<<2);

			for(i=1; i<31; i++)
			{
				dut3.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut3.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut3.g_pattern_step++;
			msdelay(10);
		}
		break;
	}
	//读取VMON 1V ADC code的低2bit
	case 0x0004:
	{
		dut3.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut3.g_pattern_smbus_control_buf[2] = 0x50;
		dut3.g_pattern_smbus_control_buf[3] = 0xda;
		dut3.g_pattern_smbus_control_buf[4] = 0x01;

		smbus3_irq_handle(dut3.g_pattern_smbus_control_buf);
		if(dut3.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			//xil_printf("dut3.g_pattern_smbus_control_buf[10] =%x\r\n", dut3.g_pattern_smbus_control_buf[10]);
			vmon_1v_adc_code_low = dut3.g_pattern_smbus_control_buf[10];
			vmon_1v_adc_code = (vmon_1v_adc_code_high | vmon_1v_adc_code_low) & 0x3ff;
			xil_printf("dut3 read_adc_count =%x   vmon_1v_adc_code =%x\r\n", read_adc_index, vmon_1v_adc_code);
			vmon_1v_adc_code_sum += vmon_1v_adc_code;
			//xil_printf("vmon_1v_adc_code_sum =%x\r\n", vmon_1v_adc_code_sum);
			read_adc_index++;
			read_adc_count--;

			if(read_adc_count == 0)
			{
				vmon_1v_adc_code_average = vmon_1v_adc_code_sum / 5;

				for(i=1; i<31; i++)
				{
					dut3.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				read_adc_index = 0;
				read_adc_count = 5;
				vmon_1v_adc_code_sum = 0;
				dut3.g_pattern_timer = 0xfff;
				dut3.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut3.g_pattern_step = 0x00;
				dut3.g_dut_pattern_status_buf[2]++;
				dut3.g_uartPatternEnable = 0x00;
				dut3.g_uartPatternNum = (dut3.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut3_1_BASEADDR,dut3.g_uartPatternNum);
				xil_printf("dut3 vmon_1v_adc_code_average =%x\r\n", vmon_1v_adc_code_average);
				xil_printf("dut3.pattern2_pass!\r\n\r\n");
			}
			else
			{
				for(i=1; i<31; i++)
				{
					dut3.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut3.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut3.g_pattern_step--;
			}
		}
		break;
	}
*/
	}

	if(dut3.g_result_fail == 0x01)
	{
		xil_printf("dut3.pattern3_fail! fail_step = %x\r\n",dut3.g_pattern_step);
		pattern_safe_state_dut3();
	}

	return 0;
}
