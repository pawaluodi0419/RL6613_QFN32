#include "platform.h"

u8 _by_Pattern_PD_msg_dut0()
{
	u8 i;
	u8 Buff_dut0_XGPIO_0[8];
	u8 Loop_run_enable = 0;

	switch(dut0.g_pattern_step)
	{
	//reset to flashǰ�������FT mode������XGPIO13�趨���high, XGPIO5�趨���low
	case 0x0000:
	{
		if(dut0.g_dut_pattern_status_buf[7] == 0x00)
		{
			Buff_dut0_XGPIO_0[0] = 0x00;                            //REG0005 ouput value[7:0]
			Buff_dut0_XGPIO_0[1] = 0xC2;							//REG0006
			Buff_dut0_XGPIO_0[2] = 0x60;							//REG0007 output value[15:8]
			Buff_dut0_XGPIO_0[3] = 0x9F;							//REG0008
			Buff_dut0_XGPIO_0[4] = 0x00|(dut0.g_uartPatternNum); 	//REG0009 output value[23:16]
			Buff_dut0_XGPIO_0[5] = 0xE0;							//REG000a
			Buff_dut0_XGPIO_0[6] = 0x0A;							//REG000b output value[31:24]
			Buff_dut0_XGPIO_0[7] = 0xF5;							//REG000c

			XGpio_dut0_Relay_WriteByte(XPAR_AXI_GPIO_dut0_1_BASEADDR,Buff_dut0_XGPIO_0);
			dut0.g_dut_pattern_status_buf[7] = 0x01;
			//xil_printf("PatternNum = %x\r\n",dut0.g_uartPatternNum);
			//xil_printf("dut0.pattern_PD_msg_gpio_control_completed!\r\n");
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

	//reset to flash
	case 0x0001:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_reset_to_flash;
		dut0.g_pattern_smbus_control_buf[2] = 0xda;
		dut0.g_pattern_smbus_control_buf[3] = 0x0b;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			for(i=1; i<31; i++)
			{
				dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut0.g_pattern_step++;
			msdelay(100);
		}
		break;
	}

	//PD msg test relay control��XGPIO0�趨���high, XGPIO24�趨���high
	//check GPIO8Ϊhigh��ʼPD msg test��XGPIO5�趨���high
	//U2_MUX test relay control��XGPIO25�趨���low
	//DP����test relay control��XGPIO27�趨���low
	case 0x0002:
	{
		if(dut0.g_dut_pattern_status_buf[7] == 0x00)
		{
			Buff_dut0_XGPIO_0[0] = 0x21;                            //REG0005 ouput value[7:0]
			Buff_dut0_XGPIO_0[1] = 0xC2;							//REG0006
			Buff_dut0_XGPIO_0[2] = 0x60;							//REG0007 output value[15:8]
			Buff_dut0_XGPIO_0[3] = 0x9F;							//REG0008
			Buff_dut0_XGPIO_0[4] = 0x00|(dut0.g_uartPatternNum); 	//REG0009 output value[23:16]
			Buff_dut0_XGPIO_0[5] = 0xE0;							//REG000a
			Buff_dut0_XGPIO_0[6] = 0x01;							//REG000b output value[31:24]
			Buff_dut0_XGPIO_0[7] = 0xF4;							//REG000c

			XGpio_dut0_Relay_WriteByte(XPAR_AXI_GPIO_dut0_1_BASEADDR,Buff_dut0_XGPIO_0);
			dut0.g_dut_pattern_status_buf[7] = 0x01;
			//xil_printf("PatternNum = %x\r\n",dut0.g_uartPatternNum);
			xil_printf("dut0.pattern_PD_msg_gpio_control_completed!\r\n");
			msdelay(10);
		}
		else if(dut0.g_dut_pattern_status_buf[7] == 0x01)
		{
			if(dut0.g_relay_control_timer == 0 )
			{
				dut0.g_dut_pattern_status_buf[7] = 0x00;
				dut0.g_pattern_step++;
				//msdelay(550);
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

	//�趨U2_MUXΪU2_MUX
	case 0x0003:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writephy;
		dut0.g_pattern_smbus_control_buf[2] = 0x3b;
		dut0.g_pattern_smbus_control_buf[3] = 0x01;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;
		dut0.g_pattern_smbus_control_buf[5] = 0x00;
		dut0.g_pattern_smbus_control_buf[6] = 0x00;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			for(i=1; i<31; i++)
			{
				dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut0.g_pattern_step++;
		}
		break;
	}

	//�趨U2_MUXΪU2_MUX
	case 0x0004:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writephy;
		dut0.g_pattern_smbus_control_buf[2] = 0x0b;
		dut0.g_pattern_smbus_control_buf[3] = 0x00;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;
		dut0.g_pattern_smbus_control_buf[5] = 0x08;
		dut0.g_pattern_smbus_control_buf[6] = 0x0c;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			for(i=1; i<31; i++)
			{
				dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut0.g_pattern_step++;
		}
		break;
	}

	//DP����
	case 0x0005:
	{
		dut0.g_pattern_smbus_control_buf[1] = smbus_cmd_type_writephy;
		dut0.g_pattern_smbus_control_buf[2] = 0x70;
		dut0.g_pattern_smbus_control_buf[3] = 0x01;
		dut0.g_pattern_smbus_control_buf[4] = 0x01;
		dut0.g_pattern_smbus_control_buf[5] = 0x00;
		dut0.g_pattern_smbus_control_buf[6] = 0x08;

		smbus0_irq_handle(dut0.g_pattern_smbus_control_buf);
		if(dut0.g_pattern_smbus_control_buf[0] != smbus_road_done_pass)
		{
			break;
		}
		else
		{
			for(i=1; i<31; i++)
			{
				dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;

			dut0.g_start_test_flag2 = 0x02;
			dut0.g_pattern_step++;
		}
		break;
	}

	//polling ack bit
	case 0x0006:
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
			if(dut0.g_pattern_smbus_control_buf[10] == 0x80)
			{
				for(i=1; i<31; i++)
				{
					dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
				}

				dut0.g_ack_bit_reread_timer = 20;
				dut0.g_pattern_timer = 0xfff;
				dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
				dut0.g_pattern_step = 0x00;
				dut0.g_dut_pattern_status_buf[2]++;
				dut0.g_uartPatternEnable = 0x00;
				dut0.g_uartPatternNum = 0x15;
				//dut0.g_uartPatternNum = (dut0.g_uartPatternNum++ & 0xff);
				result_output_for_v50(XPAR_AXI_GPIO_dut0_1_BASEADDR,dut0.g_uartPatternNum);
				xil_printf("dut0.pattern_PD_msg_pass!\r\n\r\n");
			}
			else
			{
				if(dut0.g_ack_bit_reread_timer > 0)
				{
					for(i=1; i<31; i++)
					{
						dut0.g_pattern_smbus_control_buf[i] = CLEAR_;
					}
					dut0.g_pattern_smbus_control_buf[0] = smbus_road_waiting;

					//xil_printf("dut0.g_ack_bit_reread_timer =%x\r\n", dut0.g_ack_bit_reread_timer);
					dut0.g_ack_bit_reread_timer--;
					msdelay(100);	//ÿ100ms pollingһ�ν����timeoutʱ��2s
				}
				else
				{
					dut0.g_result_fail = 0x01;
					dut0.g_result_fail_tmrcount = 0xffff;
				}
			}
		}
		break;
	}
	}

	if(dut0.g_result_fail == 0x01)
	{
		dut0.g_ack_bit_reread_timer = 20;
		xil_printf("dut0.pattern_PD_msg_fail! fail_step = %x\r\n",dut0.g_pattern_step);
		pattern_safe_state_dut0();
	}

	return 0;
}