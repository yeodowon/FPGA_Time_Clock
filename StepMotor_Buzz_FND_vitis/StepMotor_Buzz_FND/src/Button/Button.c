/*
 * Button.c
 *
 *  Created on: 2022. 10. 18.
 *      Author: kccistc
 */

#include "Button.h"



void Button_init()
{
    //Button init
    XGpio_Initialize(&GPIO_BTN, XPAR_AXI_GPIO_1_DEVICE_ID);
    XGpio_SetDataDirection(&GPIO_BTN, CHANNEL_1, 0xff);
}

void Button_MakeInst(Button *btn, int pinnum)
{
	btn->pinNum = pinnum;
	btn->prevState = RELEASED;
}

void Button_GetInstance()
{
	Button_MakeInst(&UPButton, UPBTN);
	Button_MakeInst(&DOWNButton, DOWNBTN);
	Button_MakeInst(&RIGHTButton, RIGHTBTN);
	Button_MakeInst(&LEFTButton, LEFTBTN);
}

int Button_GetState(Button *btn)
{
	int curState = XGpio_DiscreteRead(&GPIO_BTN, CHANNEL_1) & (1 << btn->pinNum);

	if(curState != RELEASED && btn->prevState == RELEASED)
	{
		usleep(20000);
		print("Pushed BTN!\n");
		btn->prevState = PUSHED;
		return NO_ACTIVE;
	}

	else if(curState == RELEASED && btn->prevState == PUSHED)
	{
		usleep(20000);
		print("Released BTN!\n");
		btn->prevState = RELEASED;
		return ACTIVE;
	}

	return NO_ACTIVE;
}
