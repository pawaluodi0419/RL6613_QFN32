#include "platform.h"

u8 _by_Pattern4_dut0()
{
	static u8 read_adc_index = 0;
	static u8 read_adc_count = 5;
	u8 i;
	u8 Buff_dut0_XGPIO_0[8];
	u16 vmon_0p25v_adc_code_high;
	u16 vmon_0p25v_adc_code_low;
	u16 vmon_0p25v_adc_code;
	static u16 vmon_0p25v_adc_code_sum = 0;
	u16 vmon_0p25v_adc_code_average;

	switch(dut0.g_pattern_step)
	{
	//1.VMON�л���ADC 0.25V: XGPIO13�趨���low, XGPIO4�趨���high
	//2.Rd relay control: XGPIO0�趨���low, XGPIO2�趨���high, XGPIO3�趨���high
	case 0x0000:
	{
		if(dut0.g_dut_pattern_status_buf[7] == 0x00)
		{
			Buff_dut0_XGPIO_0[0] = 0x3C;                            //REG0005 ouput value[7:0]
			Buff_dut0_XGPIO_0[1] = 0xC2;							//REG0006
			Buff_dut0_XGPIO_0[2] = 0x00;							//REG0007 output value[15:8]
			Buff_dut0_XGPIO_0[3] = 0xDF;							//REG0008
			Buff_dut0_XGPIO_0[4] = 0x00|(dut0.g_uartPatternNum); 	//REG0009 output value[23:16]
			Buff_dut0_XGPIO_0[5] = 0xE0;							//REG000a
			Buff_dut0_XGPIO_0[6] = 0x0A;							//REG000b output value[31:24]
			Buff_dut0_XGPIO_0[7] = 0xF5;							//REG000c

			XGpio_dut0_Relay_WriteByte(XPAR_AXI_GPIO_dut0_1_BASEADDR,Buff_dut0_XGPIO_0);
			dut0.g_dut_pattern_status_buf[7] = 0x01;
			//xil_printf("PatternNum = %x\r\n",dut0.g_uartPatternNum);
			xil_printf("dut0.pattern4_gpio_control_completed!\r\n");
			msdelay(10);
		}
		else if(dut0.g_dut_pattern_status_buf[7] == 0x01)
		{
			if(dut0.g_relay_control_timer == 0 )
			{
				dut0.g_dut_pattern_status_buf[7] = 0x00;
				dut0.g_pattern_step++;
			}
		}
		//output fail result
		else
		{
			dut0.g_result_fail = 0x01;
			dut0.g_result_fail_tmrcount = 0xffff;
		}
		break;
	}

	//write pattern index
	case 0x0001:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writemem;
		dut0.g_pattern_smbus_control_buf[2] = 0x33;
		dut0.g_pattern_smbus_control_buf[3] = 0xdf;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;
		dut0.g_pattern_smbus_control_buf[5] = 0x04;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut0 write pattern index =%x\r\n", dut0.g_pattern_smbus_control_buf[5]);
			for(i=1; i<31; i++)
			{
				dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut0.g_pattern_step++;
			//msdelay(1020);
		}
		break;
	}

	//polling ack bit
	case 0x0002:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut0.g_pattern_smbus_control_buf[2] = 0x33;
		dut0.g_pattern_smbus_control_buf[3] = 0xdf;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut0 polling pattern ack bit =%x\r\n", dut0.g_pattern_smbus_control_buf[10]);
			if(dut0.g_pattern_smbus_control_buf[10] == 0x84)
			{
				for(i=1; i<31; i++)
				{
					dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut0.g_pattern_timer = 0xfff;
				dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut0.g_pattern_step = 0x00;
				dut0.g_dut_pattern_status_buf[2]++;
				dut0.g_uartPatternEnable = 0x00;
				dut0.g_uartPatternNum = (dut0.g_uartPatternNum++ & 0xff);
				//dut0.g_uartPatternNum = (dut0.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut0_1_BASEADDR,dut0.g_uartPatternNum);
				xil_printf("dut0.pattern4_pass!\r\n\r\n");
			}
			else
			{
				dut0.g_result_fail = 0x01;
				dut0.g_result_fail_tmrcount = 0xffff;
			}
		}
		break;
	}
/*
	//��ȡVMON 0.25V ADC code�ĸ�8bit
	case 0x0003:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut0.g_pattern_smbus_control_buf[2] = 0x51;
		dut0.g_pattern_smbus_control_buf[3] = 0xda;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			//xil_printf("dut0.g_pattern_smbus_control_buf[10] =%x\r\n", dut0.g_pattern_smbus_control_buf[10]);
			vmon_0p25v_adc_code_high = (dut0.g_pattern_smbus_control_buf[10]<<2);

			for(i=1; i<31; i++)
			{
				dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut0.g_pattern_step++;
			msdelay(10);
		}
		break;
	}
	//��ȡVMON 0.25V ADC code�ĵ�2bit
	case 0x0004:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut0.g_pattern_smbus_control_buf[2] = 0x50;
		dut0.g_pattern_smbus_control_buf[3] = 0xda;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			//xil_printf("dut0.g_pattern_smbus_control_buf[10] =%x\r\n", dut0.g_pattern_smbus_control_buf[10]);
			vmon_0p25v_adc_code_low = dut0.g_pattern_smbus_control_buf[10];
			vmon_0p25v_adc_code = (vmon_0p25v_adc_code_high | vmon_0p25v_adc_code_low) & 0x3ff;
			xil_printf("dut0 read_adc_count =%x   vmon_0p25v_adc_code =%x\r\n", read_adc_index, vmon_0p25v_adc_code);
			vmon_0p25v_adc_code_sum += vmon_0p25v_adc_code;
			read_adc_index++;
			read_adc_count--;

			if(read_adc_count == 0)
			{
				vmon_0p25v_adc_code_average = vmon_0p25v_adc_code_sum / 5;

				for(i=1; i<31; i++)
				{
					dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				read_adc_index = 0;
				read_adc_count = 5;
				vmon_0p25v_adc_code_sum = 0;
				dut0.g_pattern_timer = 0xfff;
				dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut0.g_pattern_step = 0x00;
				dut0.g_dut_pattern_status_buf[2]++;
				dut0.g_uartPatternEnable = 0x00;
				dut0.g_uartPatternNum = (dut0.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut0_1_BASEADDR,dut0.g_uartPatternNum);
				xil_printf("dut0 vmon_0p25v_adc_code_average =%x\r\n", vmon_0p25v_adc_code_average);
				xil_printf("dut0.pattern3_pass!\r\n\r\n");
			}
			else
			{
				for(i=1; i<31; i++)
				{
					dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut0.g_pattern_step--;
			}
		}
		break;
	}
*/
	}

	if(dut0.g_result_fail == 0x01)
	{
		xil_printf("dut0.pattern4_fail! fail_step = %x\r\n",dut0.g_pattern_step);
		pattern_safe_state_dut0();
	}

	return 0;
}
