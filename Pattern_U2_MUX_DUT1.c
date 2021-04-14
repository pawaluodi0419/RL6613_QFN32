#include "platform.h"

u8 _by_Pattern_U2_MUX_dut1()
{
	u8 i;

	switch(dut1.g_pattern_step)
	{
	case 0x0000:
	{
		XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR,0x55);
		XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR,0xff);	//向usb host索要测试结果
		msdelay(5);

		dut1.g_uartRecBuf[0] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
		dut1.g_uartRecBuf[1] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
		dut1.g_uartRecBuf[2] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
		xil_printf("dut1.g_uartRecBuf[0] = %x\r\n", dut1.g_uartRecBuf[0]);	//用于判断usb host是否需要reset
		xil_printf("dut1.g_uartRecBuf[1] = %x\r\n", dut1.g_uartRecBuf[1]);	//用于存放测试结果低8位(dut1/dut1)
		xil_printf("dut1.g_uartRecBuf[2] = %x\r\n", dut1.g_uartRecBuf[2]);	//用于存放测试结果高8位(dut2/dut3)

		dut1.g_pattern_step++;

		break;
	}

	case 0x0001:
	{
		if(((dut1.g_uartRecBuf[1]>>4) & 0x0f) == 0x0f)
		{
			for(i=1; i<31; i++)
			{
				dut1.g_pattern_smbus_control_buf[i] = CLEAR_;
			}

			dut1.g_result_polling_tmrcount = 10;
			dut1.g_pattern_timer = 0xfff;
			dut1.g_pattern_smbus_control_buf[0] = smbus_road_waiting;
			dut1.g_pattern_step = 0x00;
			dut1.g_dut_pattern_status_buf[2]++;
			dut1.g_uartPatternEnable = 0x00;
			result_output_for_v50(XPAR_AXI_GPIO_dut1_1_BASEADDR,dut1.g_uartPatternNum);
			dut1.g_uartPatternNum = 0x16;
			xil_printf("dut1.pattern_U2_MUX_and_AUX_postive_test_pass!\r\n\r\n");
		}
		else
		{
			if(dut1.g_result_polling_tmrcount > 0)
			{
				XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR,0x55);
				XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR,0xff);	//向usb host索要测试结果
				msdelay(5);

				dut1.g_uartRecBuf[0] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
				dut1.g_uartRecBuf[1] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
				dut1.g_uartRecBuf[2] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
				xil_printf("dut1.g_uartRecBuf[0] = %x\r\n", dut1.g_uartRecBuf[0]);	//用于判断usb host是否需要reset
				xil_printf("dut1.g_uartRecBuf[1] = %x\r\n", dut1.g_uartRecBuf[1]);	//用于存放测试结果低8位(dut1/dut1)
				xil_printf("dut1.g_uartRecBuf[2] = %x\r\n", dut1.g_uartRecBuf[2]);	//用于存放测试结果高8位(dut2/dut3)
				dut1.g_result_polling_tmrcount--;
				xil_printf("dut1.g_result_polling_tmrcount = %d\r\n", dut1.g_result_polling_tmrcount);
				msdelay(500);	//每500ms polling一次结果
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
		xil_printf("dut1.pattern_U2_MUX_and_AUX_postive_test_fail! fail_step = %x\r\n",dut1.g_pattern_step);
		pattern_safe_state_dut1();
	}

	return 0;
}
