#include "platform.h"

void pattern_initial_state_dut0()
{
	u8 Buff_dut0_XGPIO_0[8];
	//relay control
	Buff_dut0_XGPIO_0[0] = 0x00;                            //REG0005 ouput value[7:0]
	Buff_dut0_XGPIO_0[1] = 0xDF;							//REG0006
	Buff_dut0_XGPIO_0[2] = 0x40;							//REG0007 output value[15:8]
	Buff_dut0_XGPIO_0[3] = 0x9F;							//REG0008
	Buff_dut0_XGPIO_0[4] = 0x00|(dut0.g_uartPatternNum); 	//REG0009 output value[23:16]
	Buff_dut0_XGPIO_0[5] = 0xE0;							//REG000a
	Buff_dut0_XGPIO_0[6] = 0x00;							//REG000b output value[31:24]
	Buff_dut0_XGPIO_0[7] = 0xFF;							//REG000c
	XGpio_dut0_Relay_WriteByte(XPAR_AXI_GPIO_dut0_1_BASEADDR,Buff_dut0_XGPIO_0);
	msdelay(10);

	//DUT IC power off
	XGpio_2_WriteBit(0,dut0_FT2_PWR_CTRL_OFFSET,0x01);
	msdelay(50);
}


