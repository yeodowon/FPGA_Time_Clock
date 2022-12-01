/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include "xil_exception.h"
#include "xintc.h"

#include "../src/Button/Button.h"

#define CHANNEL_1	1
#define GPIO_PHOTO_DEVICE_ID	XPAR_GPIO_0_DEVICE_ID
#define PHOTO_INT_MSK 			XGPIO_IR_CH1_MASK

#define INTC_DEVICE_ID			XPAR_INTC_0_DEVICE_ID
#define INTC_PHOTO_INT_VEC_ID	XPAR_INTC_0_GPIO_0_VEC_ID

#define BUZZER_DEVICE_BASE_ADDR		0x44A00000
#define BUZZER_CONTROL_REG			*(uint32_t *)BUZZER_DEVICE_BASE_ADDR
#define BUZZER_FREQ_REG				*(uint32_t *)(BUZZER_DEVICE_BASE_ADDR + 4)

#define STEPMOTOR_SEC_DEVICE_BASE_ADDR	0x44A10000
#define STEPMOTOR_SEC_CONTROL_REG	*(uint32_t *)(STEPMOTOR_SEC_DEVICE_BASE_ADDR + 0)
#define STEPMOTOR_SEC_SPEED_REG		*(uint32_t *)(STEPMOTOR_SEC_DEVICE_BASE_ADDR + 4)

#define STEPMOTOR_MIN_DEVICE_BASE_ADDR	0x44A20000
#define STEPMOTOR_MIN_CONTROL_REG	*(uint32_t *)(STEPMOTOR_MIN_DEVICE_BASE_ADDR + 0)
#define STEPMOTOR_MIN_SPEED_REG		*(uint32_t *)(STEPMOTOR_MIN_DEVICE_BASE_ADDR + 4)

#define STEPMOTOR_HOUR_DEVICE_BASE_ADDR	0x44A30000
#define STEPMOTOR_HOUR_CONTROL_REG	*(uint32_t *)(STEPMOTOR_HOUR_DEVICE_BASE_ADDR + 0)
#define STEPMOTOR_HOUR_SPEED_REG	*(uint32_t *)(STEPMOTOR_HOUR_DEVICE_BASE_ADDR + 4)

#define TICK_GENERATOR_DEVICE_BASE_ADDR	0x44A40000
#define TICK_DATA_REG				*(uint32_t *)TICK_GENERATOR_DEVICE_BASE_ADDR

#define FND_TIMER_DEVICE_BASE_ADDR		0x44A50000
#define FND_TIMER_DATA_REG			*(uint32_t *)(FND_TIMER_DEVICE_BASE_ADDR + 0)
#define FND_TIMER_CONTROL_REG		*(uint32_t *)(FND_TIMER_DEVICE_BASE_ADDR + 4)

uint32_t prev_BuzzerTick = 0;
uint32_t prev_TimerTick = 0;

void GpioHandler(void *CallBackRef);

XGpio Gpio_Photo;
XGpio GPIO_SW;
XIntc Intc;

#define INIT			0
#define READY			1
#define READYCOMPLETE	2
#define RUN				3

#define BUZZEROFF	0
#define BUZZERON	1

#define READY_SPEED	250000
#define SEC_SPEED	68270
#define MIN_SPEED	1138
#define HOUR_SPEED	95

#define MOTOR_STOP			1
#define MOTOR_CLOCK			0
#define MOTOR_COUNTERCLOCK	2

#define HourMinOn		0
#define HourMinOff		1
#define SecMsecOn		2
#define SecMsecOff		3

#define TIMEROFF		0
#define TIMERON			1

int StepMotorState = INIT;
int buzzerState = BUZZEROFF;
int TimerState = TIMEROFF;

int value = 0;

void Photo_init()
{
	XGpio_Initialize(&Gpio_Photo, GPIO_PHOTO_DEVICE_ID);
	XGpio_SetDataDirection(&Gpio_Photo, CHANNEL_1, 0x07);
}

void Intr_init()
{
	//Interrupt setup
	XIntc_Initialize(&Intc, INTC_DEVICE_ID);	// Interrupt controller device initialize.

	//Interrupt Controller�� Vector Table�� Jump�� �Լ� �Ҵ�
	XIntc_Connect(&Intc,
					INTC_PHOTO_INT_VEC_ID,
					(Xil_ExceptionHandler)GpioHandler,
					&Gpio_Photo);

	//Enable the Interrupt vector at the interrupt controller
	XIntc_Enable(&Intc, INTC_PHOTO_INT_VEC_ID);

	//Start the interrupt controller such that interrupt are recognized
	//and handled by the processor.
	XIntc_Start(&Intc, XIN_REAL_MODE);

	//GPIO Interrupt Enable
	XGpio_InterruptEnable(&Gpio_Photo, CHANNEL_1);
	XGpio_InterruptGlobalEnable(&Gpio_Photo);

	//Exception Setup
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								(Xil_ExceptionHandler)XIntc_InterruptHandler,
								&Intc);
	Xil_ExceptionEnable();
	//--�Լ��̸��� �ּ�, �Լ� ��()�� ȣ�⿬����
}

void StepMotor_Init()
{
	STEPMOTOR_HOUR_CONTROL_REG =  MOTOR_STOP;
	STEPMOTOR_MIN_CONTROL_REG = MOTOR_STOP;
	STEPMOTOR_SEC_CONTROL_REG = MOTOR_STOP;
}

void StepMotorState_Select()
{
	switch (StepMotorState)
	{
	case INIT:
		if(Button_GetState(&UPButton) == ACTIVE)
			StepMotorState = READY;
		break;

	case READY:
		if((STEPMOTOR_SEC_CONTROL_REG == MOTOR_STOP) && (STEPMOTOR_MIN_CONTROL_REG == MOTOR_STOP) && (STEPMOTOR_HOUR_CONTROL_REG == MOTOR_STOP))
			StepMotorState = READYCOMPLETE;
		break;

	case READYCOMPLETE:
		if(Button_GetState(&RIGHTButton) == ACTIVE)
			StepMotorState = RUN;
		break;

	case RUN:
		if(Button_GetState(&DOWNButton) == ACTIVE)
			StepMotorState = INIT;
		break;
	}
}
int sec,min,hour = 0;

// ���ͷ�Ʈ ���񽺿��� READY ������ �� �浹���� �� �ذ��ؾ� ��
void StepMotor_Action()
{
	switch (StepMotorState)
	{
	case INIT:
		STEPMOTOR_HOUR_CONTROL_REG = MOTOR_STOP;
		STEPMOTOR_MIN_CONTROL_REG = MOTOR_STOP;
		STEPMOTOR_SEC_CONTROL_REG = MOTOR_STOP;

		/* ��, ��, ���� Photo Interrupter�� ���� flag (sec, min, hour) */
		sec = 0;
		min = 0;
		hour = 0;
		break;
	case READY:
		if(!sec)
		{
			STEPMOTOR_SEC_CONTROL_REG = MOTOR_COUNTERCLOCK;
			STEPMOTOR_SEC_SPEED_REG = READY_SPEED;
		}
		else if(sec)
		{
			STEPMOTOR_SEC_CONTROL_REG = MOTOR_STOP;
		}

		if(!min)
		{
			STEPMOTOR_MIN_CONTROL_REG = MOTOR_COUNTERCLOCK;
			STEPMOTOR_MIN_SPEED_REG = READY_SPEED;
		}
		else if(min)
		{
			STEPMOTOR_MIN_CONTROL_REG = MOTOR_STOP;
		}

		if(!hour)
		{
			STEPMOTOR_HOUR_CONTROL_REG = MOTOR_COUNTERCLOCK;
			STEPMOTOR_HOUR_SPEED_REG = READY_SPEED;
		}
		else if(hour)
		{
			STEPMOTOR_HOUR_CONTROL_REG = MOTOR_STOP;
		}
		break;

	case RUN:
		STEPMOTOR_HOUR_CONTROL_REG = MOTOR_CLOCK;
		STEPMOTOR_MIN_CONTROL_REG = MOTOR_CLOCK;
		STEPMOTOR_SEC_CONTROL_REG = MOTOR_CLOCK;
		STEPMOTOR_HOUR_SPEED_REG = HOUR_SPEED;
		STEPMOTOR_MIN_SPEED_REG = MIN_SPEED;
		STEPMOTOR_SEC_SPEED_REG = SEC_SPEED;
		break;
	}
}

void Buzzer_Init()
{
	BUZZER_CONTROL_REG = 1;
}

void BuzzerState_Select()
{
	switch (buzzerState)
	{
	case BUZZEROFF:
		prev_BuzzerTick = TICK_DATA_REG;
		break;

	case BUZZERON:
		if(TICK_DATA_REG - prev_BuzzerTick >= 500)
		{
			buzzerState = BUZZEROFF;
		}
		break;
	}
}

void Buzzer_Action()
{
	switch (buzzerState)
	{
	case BUZZEROFF:
		BUZZER_CONTROL_REG = 1;
		break;

	case BUZZERON:
		BUZZER_CONTROL_REG = 0;
		BUZZER_FREQ_REG = 440;
		break;
	}
}

void FND_Timer_Init()
{
	FND_TIMER_DATA_REG = value;
}

void FND_Timer_ModeSelect()
{
	switch(TimerState)
	{
	case TIMEROFF:
		if(StepMotorState == RUN)
			TimerState = TIMERON;
		break;

	case TIMERON:
		if(StepMotorState != RUN)
			TimerState = TIMEROFF;
		break;
	}
}

void FND_Timer_Action()
{
	FND_TIMER_DATA_REG = value;
	switch(TimerState)
	{
	case TIMEROFF:
		value = 0;
		break;

	case TIMERON:
		if(TICK_DATA_REG - prev_TimerTick >= 10)
		{
			prev_TimerTick = TICK_DATA_REG;
			value++;
		}
		break;
	}
}

void Switch_init()
{
    //SW init
    XGpio_Initialize(&GPIO_SW, XPAR_AXI_GPIO_2_DEVICE_ID);
    XGpio_SetDataDirection(&GPIO_SW, CHANNEL_1, 0x03);
}

void Switch_Select()
{
	if(XGpio_DiscreteRead(&GPIO_SW, CHANNEL_1) == HourMinOn)
	{
		FND_TIMER_CONTROL_REG = HourMinOn;
	}
	if(XGpio_DiscreteRead(&GPIO_SW, CHANNEL_1) == HourMinOff)
	{
		FND_TIMER_CONTROL_REG = HourMinOff;
	}
	if(XGpio_DiscreteRead(&GPIO_SW, CHANNEL_1) == SecMsecOn)
	{
		FND_TIMER_CONTROL_REG = SecMsecOn;
	}
	if(XGpio_DiscreteRead(&GPIO_SW, CHANNEL_1) == SecMsecOff)
	{
		FND_TIMER_CONTROL_REG = SecMsecOff;
	}
}

int main()
{
    init_platform();

    Button_init();
    Button_GetInstance();
    Photo_init();
    Intr_init();
    StepMotor_Init();
    Buzzer_Init();
    Switch_init();
    FND_Timer_Init();

    while(1)
    {
    	StepMotorState_Select();
    	StepMotor_Action();

    	BuzzerState_Select();
    	Buzzer_Action();


    	Switch_Select();

    	FND_Timer_ModeSelect();
    	FND_Timer_Action();
    }
    cleanup_platform();
    return 0;
}

void GpioHandler(void *CallBackRef)
{
	XGpio *pGpio = (XGpio *)CallBackRef;

	switch(StepMotorState)
	{
	case READY:
		if((XGpio_DiscreteRead(pGpio, CHANNEL_1) & 0x01)) // Sec_Motor
		{
			sec = 1;
		}
		if((XGpio_DiscreteRead(pGpio, CHANNEL_1) & 0x02)) // Min_Motor
		{
			min = 1;
		}
		if((XGpio_DiscreteRead(pGpio, CHANNEL_1) & 0x04)) // Hour_Motor
		{
			hour = 1;
		}
		break;
	case RUN:
		if((XGpio_DiscreteRead(pGpio, CHANNEL_1) & 0x01)) // Buzzer On always 60sec(=1min)
		{
			buzzerState = BUZZERON;
		}
		break;
	}

	XGpio_InterruptClear(pGpio, CHANNEL_1);
}
