/*
 * Button.h
 *
 *  Created on: 2022. 10. 18.
 *      Author: kccistc
 */

#ifndef SRC_DRIVER_BUTTON_BUTTON_H_
#define SRC_DRIVER_BUTTON_BUTTON_H_

#include "xgpio.h"
#include "sleep.h"

#define NO_ACTIVE	0
#define ACTIVE		1

#define PUSHED		1
#define RELEASED	0

#define UPBTN		0
#define DOWNBTN		1
#define RIGHTBTN	2
#define LEFTBTN		3

#define CHANNEL_1	1
#define INPUT		1
#define OUTPUT		0

XGpio	GPIO_BTN;

typedef struct _button
{
	int pinNum;
	int prevState;
}Button;

Button UPButton, DOWNButton, RIGHTButton, LEFTButton;

void Button_init();
void Button_MakeInst(Button *btn, int pinnum);
void Button_GetInstance();
int Button_GetState(Button *btn);

#endif /* SRC_DRIVER_BUTTON_BUTTON_H_ */
