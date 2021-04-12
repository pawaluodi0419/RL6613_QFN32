#include "platform.h"

u8 _by_PatternA_dut2()
{
	u8 i;
	u8 Buff_dut2_XGPIO_0[8];

	switch(dut2.g_pattern_step)
	{
	//CC1/CC2�л���CC1_TRH/CC2_TRH: XGPIO0�趨���low, XGPIO2�趨���low
	case 0x0000:
	{
		if(dut2.g_dut_pattern_status_buf[7] == 0x00)
		{
			Buff_dut2_XGPIO_0[0] = 0x20;                            //REG0005 ouput value[7:0]
			Buff_dut2_XGPIO_0[1] = 0xC2;							//REG0006
			Buff_dut2_XGPIO_0[2] = 0x40;							//REG0007 output value[15:8]
			Buff_dut2_XGPIO_0[3] = 0x9F;							//REG0008
			Buff_dut2_XGPIO_0[4] = 0x00|(dut2.g_uartPatternNum); 	//REG0009 output value[23:16]
			Buff_dut2_XGPIO_0[5] = 0xE0;							//REG000a
			Buff_dut2_XGPIO_0[6] = 0x0A;							//REG000b output value[31:24]
			Buff_dut2_XGPIO_0[7] = 0xF5;							//REG000c

			XGpio_dut2_Relay_WriteByte(XPAR_AXI_GPIO_dut2_1_BASEADDR,Buff_dut2_XGPIO_0);
			dut2.g_dut_pattern_status_buf[7] = 0x01;
			xil_printf("dut2.patternA_gpio_control_completed!\r\n");
			msdelay(10);
		}
		else if(dut2.g_dut_pattern_status_buf[7] == 0x01)
		{
			if(dut2.g_relay_control_timer == 0 )
			{
				dut2.g_dut_pattern_status_buf[7] = 0x00;
				dut2.g_pattern_step++;
			}
		}
		//output fail result
		else
		{
			dut2.g_result_fail = 0x01;
			dut2.g_result_fail_tmrcount = 0xffff;
		}
		break;
	}

	//����DAC MCP4822 VOUTA(channel CC1)���0.5V
	case 0x0001:
	{
		dut2.g_dac_data_buf[0] = 0x01;	//0x01: CC1 0x03: CC2	�����ѹֵ����2V, ��Ҫд��0x05/0x07
		dut2.g_dac_data_buf[1] = 0xe8;	//Data L
		dut2.g_dac_data_buf[2] = 0x03;	//Data H
		dut2.g_dac_data_buf[3] = 0xac;	//Data L
		dut2.g_dac_data_buf[4] = 0x03;	//Data H
		dut2.g_dac_data_buf[5] = 0x24;	//Data L
		dut2.g_dac_data_buf[6] = 0x04;	//Data H

		_by_mcp4822_output_control_dut2(dut2.g_dac_data_buf);
		dut2.g_pattern_step++;
		//xil_printf("dut2_dac_writen =%x,%x,%x\r\n",dut2.g_dac_data_buf[0],dut2.g_dac_data_buf[1],dut2.g_dac_data_buf[2]);
		break;
	}

	//����DAC MCP4822 VOUTB(channel CC2)���0.5V
	case 0x0002:
	{
		dut2.g_dac_data_buf[0] = 0x03;	//0x01: CC1 0x03: CC2	�����ѹֵ����2V, ��Ҫд��0x05/0x07
		dut2.g_dac_data_buf[1] = 0xe8;	//Data L
		dut2.g_dac_data_buf[2] = 0x03;	//Data H
		dut2.g_dac_data_buf[3] = 0xac;	//Data L
		dut2.g_dac_data_buf[4] = 0x03;	//Data H
		dut2.g_dac_data_buf[5] = 0x24;	//Data L
		dut2.g_dac_data_buf[6] = 0x04;	//Data H

		_by_mcp4822_output_control_dut2(dut2.g_dac_data_buf);
		dut2.g_pattern_step++;
		//xil_printf("dut2_dac_writen =%x,%x,%x\r\n",dut2.g_dac_data_buf[0],dut2.g_dac_data_buf[1],dut2.g_dac_data_buf[2]);
		break;
	}

	//write pattern index
	case 0x0003:
	{
		dut2.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writemem;
		dut2.g_pattern_smbus_control_buf[2] = 0x33;
		dut2.g_pattern_smbus_control_buf[3] = 0xdf;
		dut2.g_pattern_smbus_control_buf[4] = 0x01;
		dut2.g_pattern_smbus_control_buf[5] = 0x0a;

		smbus2_irq_handle(dut2.g_pattern_smbus_control_buf);
		if(dut2.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut2 write pattern index =%x\r\n", dut2.g_pattern_smbus_control_buf[5]);
			for(i=1; i<31; i++)
			{
				dut2.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut2.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut2.g_pattern_step++;
			//msdelay(50);
		}
		break;
	}

	//polling ack bit
	case 0x0004:
	{
		dut2.g_pattern_smbus_control_buf[1] = smbus_cmd_type_readmem;
		dut2.g_pattern_smbus_control_buf[2] = 0x33;
		dut2.g_pattern_smbus_control_buf[3] = 0xdf;
		dut2.g_pattern_smbus_control_buf[4] = 0x01;

		smbus2_irq_handle(dut2.g_pattern_smbus_control_buf);
		if(dut2.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			xil_printf("dut2 polling pattern ack bit =%x\r\n", dut2.g_pattern_smbus_control_buf[10]);
			if(dut2.g_pattern_smbus_control_buf[10] == 0x8A)
			{
				for(i=1; i<31; i++)
				{
					dut2.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut2.g_pattern_timer = 0xfff;
				dut2.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut2.g_pattern_step = 0x00;
				dut2.g_dut_pattern_status_buf[2]++;
				dut2.g_uartPatternEnable = 0x00;
				dut2.g_uartPatternNum = (dut2.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut2_1_BASEADDR,dut2.g_uartPatternNum);
				xil_printf("dut2.patternA_pass!\r\n\r\n");
			}
			else
			{
				dut2.g_result_fail = 0x01;
				dut2.g_result_fail_tmrcount = 0xffff;
			}
		}
		break;
	}
	}

	if(dut2.g_result_fail == 0x01)
	{
		xil_printf("dut2.patternA_fail! fail_step = %x\r\n",dut2.g_pattern_step);
		pattern_safe_state_dut2();
	}

	return 0;
}
