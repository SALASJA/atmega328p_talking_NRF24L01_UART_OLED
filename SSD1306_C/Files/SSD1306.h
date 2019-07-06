#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED
/*
||
||  Filename:	 		SSD1306.h
||  Title: 			    SSD1306 Driver
||  Author: 			Efthymios Koktsidis
||	Email:				efthymios.ks@gmail.com
||  Compiler:		 	AVR-GCC
||	Description:
||	This library can drive SSD1306 based GLCD.
||
*/

//----- Headers ------------//
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "IO_Macros.h"
#include "SSD1306_Settings.h"
#include "TWI.h"
//--------------------------//

static const uint8_t text_table[26][5] =   {{48,72,72,120,72},      //A
									 		{120,88,112,88,120},    //B
									 		{56,64,64,64,56},       //C
									 		{112,72,72,72,112},     //D
									 		{120,64,120,64,120},    //E
									 		{120,64,112,64,64},     //F
									 		{56,64,88,72,48},       //G
									 		{72,72,120,72,72},      //H
									 		{48,48,48,48,48},       //I
									 		{8,8,72,72,48},         //J
									 		{72,80,96,80,72},       //K
									 		{64,64,64,64,120},      //L
									 		{68,108,84,68,68},      //M
											{72,104,88,72,72},      //N
									 		{120,72,72,72,120},     //O
									 		{120,72,120,64,64},     //P
									 		{120,72,72,120,8},      //Q
									 		{120,72,120,80,72},     //R
									 		{56,64,48,8,112},        //S
									 		{120,32,32,32,32},      //T
									 		{72,72,72,72,120},      //U
									 		{68,68,68,40,16},       //V
									 		{68,68,84,84,48},       //W
									 		{68,40,16,40,68},       //X
									 		{72,72,120,16,16},      //Y
									 		{124,8,16,32,124}};     //Z
//----- Auxiliary data ---------------------------//
#define __GLCD_I2C_Address							0x3C	//0x3C or 0x3D

#define __GLCD_RW									0
#define __GLCD_SA0									1
#define __GLCD_DC									6
#define __GLCD_CO									7

#if (GLCD_Size == GLCD_128_64)
	#define __GLCD_Screen_Width          			128
	#define __GLCD_Screen_Height         			64
#elif (GLCD_Size == GLCD_128x32)
	#define __GLCD_Screen_Width         			128
	#define __GLCD_Screen_Height        			32
#else
	#define __GLCD_Screen_Width          			96
	#define __GLCD_Screen_Height         			16
#endif
#define	__GLCD_Screen_Line_Height					8
#define __GLCD_Screen_Lines							__GLCD_Screen_Height / __GLCD_Screen_Line_Height

//Fundamental Command Table
#define __GLCD_Command_Display_On							0xAF
#define __GLCD_Command_Display_Off							0xAE
#define __GLCD_Command_Constrast_Set						0x81
#define __GLCD_Command_Display_All_On_Resume				0xA4
#define __GLCD_Command_Display_All_On						0xA5
#define __GLCD_Command_Display_Normal						0xA6
#define __GLCD_Command_Display_Inverse						0xA7

//Scrolling Command Table
#define __GLCD_Command_Scroll_Activate						0x2F
#define __GLCD_Command_Scroll_Deactivate					0x2E	
#define __GLCD_Command_Scroll_Left							0x27
#define __GLCD_Command_Scroll_Right							0x26
#define __GLCD_Command_Scroll_Vertical_Left					0x2A
#define __GLCD_Commad_Scroll_Vertical_Right					0x29
#define __GLCD_Command_Scroll_Vertical_Area_Set				0xA3

//Addressing Setting Command Table
#define __GLCD_Command_Page_Addressing_Column_Lower_Set		0x00
#define __GLCD_Command_Page_Addressing_Column_Higher_Set	0x10
#define __GLCD_Command_Page_Addressing_Page_Start_Set		0xB0
#define __GLCD_Command_Page_Address_Set						0x22
#define __GLCD_Command_Memory_Addressing_Set				0x20
#define __GLCD_Command_Column_Address_Set					0x21

//Hardware Configuration
#define __GLCD_Command_Display_Start_Line_Set				0x40
#define __GLCD_Command_Display_Offset_Set					0xD3
#define __GLCD_Command_Segment_Remap_Set					0xA0
#define __GLCD_Command_Multiplex_Radio_Set					0xA8
#define __GLCD_Command_Com_Output_Scan_Inc					0xC0
#define __GLCD_Command_Com_Output_Scan_Dec					0xC8
#define __GLCD_Command_Com_Pins_Set							0xDA

//Timing and Driving Scheme Setting Command Table
#define __GLCD_Command_Display_Clock_Div_Ratio_Set			0xD5
#define __GLCD_Command_Display_Oscillator_Frequency_Set		0xD5
#define __GLCD_Command_Precharge_Period_Set					0xD9
#define __GLCD_Command_VCOMH_Deselect_Level_Set				0xDB
#define __GLCD_Command_Nop									0xE3

//Charge Pump Command Table
#define __GLCD_Command_Charge_Pump_Set						0x8D

//Reset delays (in ms)
#define _GLCD_Delay_1										1
#define _GLCD_Delay_2										10

#if (GLCD_Error_Checking != 0)
	enum GLCD_Status
	{
		GLCD_Ok,
		GLCD_Error
	};
#endif

enum GLCD_Type_t
{
	GLCD_128_64,
	GLCD_128x32,
	GLCD_96x16
};

enum OperatingMode_t
{
	GLCD_Inverted		= __GLCD_Command_Display_Inverse,
	GLCD_Non_Inverted	= __GLCD_Command_Display_Normal
};

enum PrintMode_t
{
	GLCD_Overwrite,
	GLCD_Merge
};

enum Color_t
{
	GLCD_White = 0x00,
	GLCD_Black = 0xFF
};

typedef struct
{
	uint8_t *Name;
	uint8_t Width;
	uint8_t Height;
	uint8_t Lines;
	enum PrintMode_t Mode;
}Font_t;

typedef struct
{
	#if (GLCD_Error_Checking != 0)
		enum GLCD_Status Status;
	#endif
	uint8_t X;
	uint8_t Y;
	enum OperatingMode_t Mode;
	Font_t Font;
}GLCD_t;
//------------------------------------------------//

//----- Prototypes ------------------------------------------------------------//
void GLCD_SendCommand(uint8_t Command);
void GLCD_SendData(const uint8_t Data);
void GLCD_Setup(void);
void GLCD_Reset(void);
#if (GLCD_Error_Checking != 0)
	enum GLCD_Status GLCD_Status(void);
#endif
void GLCD_Render(void);
void GLCD_SetDisplay(const uint8_t On);
void GLCD_SetContrast(const uint8_t Contrast);

void GLCD_Clear(void);
void GLCD_ClearLine(const uint8_t Line);
void GLCD_GotoX(const uint8_t X);
void GLCD_GotoY(const uint8_t Y);
void GLCD_GotoXY(const uint8_t X, const uint8_t Y);
void GLCD_GotoLine(const uint8_t Line);
uint8_t GLCD_GetX(void);
uint8_t GLCD_GetY(void);
uint8_t GLCD_GetLine(void);

void GLCD_SetPixel(const uint8_t X, const uint8_t Y, enum Color_t Color);
void GLCD_SetPixels(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, enum Color_t Color);

void GLCD_DrawBitmap(const uint8_t *Bitmap, uint8_t Width, const uint8_t Height, enum PrintMode_t Mode);
void GLCD_DrawLine(const uint8_t X1, const uint8_t Y1, const uint8_t X2, const uint8_t Y2, enum Color_t Color);
void GLCD_DrawRectangle(const uint8_t X1, const uint8_t Y1, const uint8_t X2, const uint8_t Y2, enum Color_t Color);
void GLCD_DrawRoundRectangle(const uint8_t X1, const uint8_t Y1, const uint8_t X2, const uint8_t Y2, const uint8_t Radius, enum Color_t Color);
void GLCD_DrawTriangle(const uint8_t X1, const uint8_t Y1, const uint8_t X2, const uint8_t Y2, const uint8_t X3, const uint8_t Y3, enum Color_t Color);
void GLCD_DrawCircle(const uint8_t CenterX, const uint8_t CenterY, uint8_t Radius, enum Color_t Color);

void GLCD_FillScreen(enum Color_t Color);
void GLCD_FillRectangle(const uint8_t X1, const uint8_t Y1, const uint8_t X2, const uint8_t Y2, enum Color_t Color);
void GLCD_FillRoundRectangle(const uint8_t X1, const uint8_t Y1, const uint8_t X2, const uint8_t Y2, const uint8_t Radius, enum Color_t Color);
void GLCD_FillTriangle(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t X3, uint8_t Y3, enum Color_t Color);
void GLCD_FillCircle(const uint8_t CenterX, const uint8_t CenterY, const uint8_t Radius, enum Color_t Color);

void GLCD_ScrollLeft(const uint8_t Start, const uint8_t End);
void GLCD_ScrollRight(const uint8_t Start, const uint8_t End);
void GLCD_ScrollDiagonalLeft(const uint8_t Start, const uint8_t End);
void GLCD_ScrollDiagonalRight(const uint8_t Start, const uint8_t End);
void GLCD_ScrollStop(void);

void GLCD_InvertScreen(void);
void GLCD_InvertRect(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);

void GLCD_SetFont(const uint8_t *Name, const uint8_t Width, const uint8_t Height, enum PrintMode_t Mode);
uint8_t GLCD_GetWidthChar(const char Character);
uint16_t GLCD_GetWidthString(const char *Text);
uint16_t GLCD_GetWidthString_P(const char *Text);
void GLCD_PrintChar(char Character);
void GLCD_PrintString(const char *Text);
void GLCD_PrintString_P(const char *Text);
void GLCD_PrintInteger(const int32_t Value);
void GLCD_PrintDouble(double Value, const uint32_t Tens);
uint8_t * getCharacterBitmap(uint8_t c);
void drawText(const char * text,  uint8_t pos_x, uint8_t pos_y);
//-----------------------------------------------------------------------------//
#endif