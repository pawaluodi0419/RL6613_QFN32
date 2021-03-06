#include "platform.h"

u8 _by_PatternB_dut1()
{
	u8 i;
	u8 Buff_dut1_XGPIO_0[8];

	switch(dut1.g_pattern_step)
	{
	//CC1/CC2切换到CC1_TRH/CC2_TRH: XGPIO0设定输出low, XGPIO2设定输出low
	case 0x0000:
	{
		if(dut1.g_dut_pattern_status_buf[7] == 0x00)
		{
			Buff_dut1_XGPIO_0[0] = 0x20;                            //REG0005 ouput value[7:0]
			Buff_dut1_XGPIO_0[1] = 0xC2;							//REG0006
			Buff_dut1_XGPIO_0[2] = 0x40;							//REG0007 output value[15:8]
			Buff_dut1_XGPIO_0[3] = 0x9F;							//REG0008
			Buff_dut1_XGPIO_0[4] = 0x00|(dut1.g_uartPatternNum); 	//REG0009 output value[23:16]
			Buff_dut1_XGPIO_0[5] = 0xE0;							//REG000a
			Buff_dut1_XGPIO_0[6] = 0x0A;							//REG000b output value[31:24]
			Buff_dut1_XGPIO_0[7] = 0xF5;							//REG000c

			XGpio_dut1_Relay_WriteByte(XPAR_AXI_GPIO_dut1_1_BASEADDR,Buff_dut1_XGPIO_0);
			dut1.g_dut_pattern_status_buf[7] = 0x01;
			xil_printf("dut1.patternB_gpio_control_completed!\r\n");
			msdelay(10);
		}
		else if(dut1.g_dut_pattern_status_buf[7] == 0x01)
		{
			if(dut1.g_relay_control_timer == 0 )
			{
				dut1.g_dut_pattern_status_buf[7] = 0x00;
				dut1.g_pattern_step++;
			}
		}
		//output fail result
		else
		{
			dut1.g_result_fail = 0x01;
			dut1.g_result_fail_tmrcount = 0xffff;
		}
		break;
	}

	//驱动DAC MCP4822 VOUTA(channel CC1)输出0.3V
	case 0x0001:
	{
		dut1.g_dac_data_buf[0] = 0x01;	//0x01: CC1 0x03: CC2	如果电压值超过2V, 需要写成0x05/0x07
		dut1.g_dac_data_buf[1] = 0x58;	//Data L
		dut1.g_dac_data_buf[2] = 0x02;	//Data H
		dut1.g_dac_data_buf[3] = 0x1c;	//Data L
		dut1.g_dac_data_buf[4] = 0x02;	//Data H
		dut1.g_dac_data_buf[5] = 0x94;	//Data L
		dut1.g_dac_data_buf[6] = 0x02;	//Data H

		_by_mcp4822_output_control_dut1(dut1.g_dac_data_buf);
		dut1.g_pattern_step++;
		//xil_printf("dut1_dac_writen =%x,%x,%x\r\n",dut1.g_dac_data_buf[0],dut1.g_dac_data_buf[1],dut1.g_dac_data_buf[2]);
		break;
	}

	//驱动DAC MCP4822 VOUTB(channel CC2)输出0.3V
	case 0x0002:
	{
		dut1.g_dac_data_buf[0] = 0x03;	//0x01: CC1 0x03: CC2	如果电压值超过2V, 需要写成0x05/0x07
		dut1.g_dac_data_buf[1] = 0x58;	//Data L
		dut1.g_dac_data_buf[2] = 0x02;	//Data H
		dut1.g_dac_data_buf[3] = 0x1c;	//Data L
		dut1.g_dac_data_buf[4] = 0x02;	//Data H
		dut1.g_dac_data_buf[5] = 0x94;	//Data L
		dut1.g_dac_data_buf[6] = 0x02;	//Data H

		_by_mcp4822_output_control_dut1(dut1.g_dac_data_buf);
		dut1.g_pattern_step++;
		//xil_printf("dut1_dac_writen =%x,%x,%x\r\n",dut1.g_dac_data_buf[0],dut1.g_dac_data_buf[1],dut1.g_dac_data_buf[2]);
		break;
	}

	//write pattern index
	case 0x0003:
	{
		dut1.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writemem;
		dut1.g_pattern_smbus_control_buf[2] = 0x33;
		dut1.g_pattern_smbus_control_buf[3] = 0xdf;
		dut1.g_pattern_smbus_control_buf[4] = 0x01;
		dut1.g_pattern_smbus_control_buf[5] = 0x0b;

		smbus1_irq_handle(dut1.g_pattern_smbus_control_buf);
		if(dut1.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut1 write pattern index =%x\r\n", dut1.g_pattern_smbus_control_buf[5]);
			for(i=1; i<31; i++)
			{
				dut1.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut1.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut1.g_pattern_step++;
			//msdelay(50);
		}
		break;
	}

	//polling ack bit
	case 0x0004:
	{
		dut1.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut1.g_pattern_smbus_control_buf[2] = 0x33;
		dut1.g_pattern_smbus_control_buf[3] = 0xdf;
		dut1.g_pattern_smbus_control_buf[4] = 0x01;

		smbus1_irq_handle(dut1.g_pattern_smbus_control_buf);
		if(dut1.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut1 polling pattern ack bit =%x\r\n", dut1.g_pattern_smbus_control_buf[10]);
			if(dut1.g_pattern_smbus_control_buf[10] == 0x8B)
			{
				for(i=1; i<31; i++)
				{
					dut1.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut1.g_pattern_timer = 0xfff;
				dut1.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut1.g_pattern_step = 0x00;
				dut1.g_dut_pattern_status_buf[2]++;
				dut1.g_uartPatternEnable = 0x00;
				dut1.g_uartPatternNum = (dut1.g_uartPatternNum++ & 0xff);
				dut1.g_uartPatternNum = (dut1.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut1_1_BASEADDR,dut1.g_uartPatternNum);
				xil_printf("dut1.patternB_pass!\r\n\r\n");
			}
			else
			{
				dut1.g_result_fail = 0x01;
				dut1.g_result_fail_tmrcount = 0xffff;
			}
		}
		break;
	}
	}

	if(dut1.g_result_fail == 0x01)
	{
		xil_printf("dut1.patternB_fail! fail_step = %x\r\n",dut1.g_pattern_step);
		pattern_safe_state_dut1();
	}

	return 0;
}
