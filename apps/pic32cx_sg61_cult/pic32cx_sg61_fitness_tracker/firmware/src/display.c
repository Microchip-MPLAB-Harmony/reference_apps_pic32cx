/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    display.c

  Summary:
    This file contains the source code for the OLED C Click Display routines.

 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "display.h"
#include "click_routines/oled_c/oled_c.h"
#include "click_routines/oled_c/images.h"
#include "click_routines/oled_c/fonts.h"

#include <stddef.h>                     // Defines NULL
#include <stdint.h>                     // Defines uint*
#include <string.h>                     // Defines string functions
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines printf
#include <stdio.h>                      // Defines printf

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define MCHP_LOGO_POS_X 0
#define MCHP_LOGO_POS_Y 0

#define HEART_RATE_IMG_POS_X 24
#define HEART_RATE_IMG_POS_Y  0

#define DISPLAY_MESSAGE_1_X_POS 1
#define DISPLAY_MESSAGE_1_Y_POS 40
#define DISPLAY_MESSAGE_1_MUL   1

#define DISPLAY_MESSAGE_2_X_POS 1
#define DISPLAY_MESSAGE_2_Y_POS 60
#define DISPLAY_MESSAGE_2_MUL   1

#define DISPLAY_MESSAGE_3_X_POS 1
#define DISPLAY_MESSAGE_3_Y_POS 80
#define DISPLAY_MESSAGE_3_MUL   2


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// helper to write a message on line 1
static void DisplayMessageLine1(char* message)
{
    OLEDC_SetFont(demo_font, DEMO_FONT_WIDTH, DEMO_FONT_HEIGHT);

    OLEDC_DrawStringWithBackground(DISPLAY_MESSAGE_1_X_POS, DISPLAY_MESSAGE_1_Y_POS, DISPLAY_MESSAGE_1_MUL, DISPLAY_MESSAGE_1_MUL, message, WHITE, BLACK);
}

// helper to write a message on line 2
static void DisplayMessageLine2(char* message)
{
    OLEDC_SetFont(demo_font, DEMO_FONT_WIDTH, DEMO_FONT_HEIGHT);

    OLEDC_DrawStringWithBackground(DISPLAY_MESSAGE_2_X_POS, DISPLAY_MESSAGE_2_Y_POS, DISPLAY_MESSAGE_2_MUL, DISPLAY_MESSAGE_2_MUL, message, WHITE, BLACK);
}

// helper to write a message on line 3
static void DisplayMessageLine3(char* message)
{
    OLEDC_SetFont(demo_font, DEMO_FONT_WIDTH, DEMO_FONT_HEIGHT);

    OLEDC_DrawStringWithBackground(DISPLAY_MESSAGE_3_X_POS, DISPLAY_MESSAGE_3_Y_POS, DISPLAY_MESSAGE_3_MUL, DISPLAY_MESSAGE_3_MUL, message, WHITE, BLACK);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Public Functions
// *****************************************************************************
// *****************************************************************************

// initialize the display
void APP_DisplayInitialize(void)
{
    OLEDC_Initialize();
}

// helper to fill the screen with some color
void APP_DisplayFillScreen(uint16_t color)
{
    OLEDC_DrawRectangle(0, 0, SSD1351_SCREEN_WIDTH, SSD1351_SCREEN_HEIGHT, color);
}

// initialization screen
void APP_DisplayShowInitializationScreen(void)
{
    APP_DisplayFillScreen(BLACK);

    OLEDC_DrawImage(MCHP_LOGO_POS_X, MCHP_LOGO_POS_Y, mchp_logo_width, mchp_logo_height, mchp_logo);

    DisplayMessageLine1("Initializing...");
    DisplayMessageLine2("Please Wait!");
}

// help screen
void APP_DisplayScreenHelp(void)
{
    APP_DisplayFillScreen(BLACK);

    OLEDC_DrawImage(HEART_RATE_IMG_POS_X, HEART_RATE_IMG_POS_Y, heart_rate_width, heart_rate_height, heart_rate);

    DisplayMessageLine1("Finger on Sensor");
    DisplayMessageLine2("Press SW2");
}

// reading heart rate value screen
void APP_DisplayScreenReadingHR(void)
{
    APP_DisplayFillScreen(BLACK);

    OLEDC_DrawRectangle(HEART_RATE_IMG_POS_X, HEART_RATE_IMG_POS_Y, heart_rate_width, heart_rate_height, BLACK);

    DisplayMessageLine1("SW2 Pressed");
    DisplayMessageLine2("Reading Data...");

}

// heart rate value screen
void APP_DisplayScreenHR(int16_t heartRateValue)
{
    char displayBuffer[16];
    
    printf("Display: Writing the heart rate: %d\r\n", heartRateValue);

    APP_DisplayFillScreen(BLACK);

    OLEDC_DrawImage(HEART_RATE_IMG_POS_X, HEART_RATE_IMG_POS_Y, heart_rate_width, heart_rate_height, heart_rate);

    sprintf(displayBuffer, "%d bpm", (uint16_t)heartRateValue);
    displayBuffer[9] = '\0';

    DisplayMessageLine1("SW2 - new read");
    DisplayMessageLine2("Heart Rate:");
    DisplayMessageLine3(displayBuffer);
}

// BT init error screen
void APP_DisplayScreenBTInitFailed(void)
{
    APP_DisplayFillScreen(BLACK);

    OLEDC_DrawImage(HEART_RATE_IMG_POS_X, HEART_RATE_IMG_POS_Y, heart_rate_width, heart_rate_height, heart_rate);

    DisplayMessageLine1("ERROR: Init Failed");
    DisplayMessageLine2("Reset & SW2 pressed");
}

/*******************************************************************************
 End of File
 */
