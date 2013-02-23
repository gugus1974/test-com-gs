#include "stdio.h"
#include "serbuf.h"
#include "consolle.h"
#include <reg167.h>
#include <bitfield.h>
#include <intnum.h>
#pragma ot(3,SPEED)

unsigned int myTFR;

void NMI_handler(void) interrupt NMITRAP using TRAPREG
{
	myTFR = TFR;
	TFR = 0;	
}

void STOV_handler(void) interrupt STOTRAP using TRAPREG
{
	myTFR = TFR;
	TFR = 0;	
}

void STUN_handler(void) interrupt STUTRAP using TRAPREG
{
	myTFR = TFR;
	TFR = 0;	
}

void BTRAP_handler(void) interrupt BTRAP using TRAPREG
{
	myTFR = TFR;
	TFR = 0;	
}
