/*******************************************************************************
  OLED C click routine interface header file

  Company
    Microchip Technology Inc.

  File Name
    oled_c_interface.h

  Summary
    OLED C click routine interface implementation File.

  Description
    This file defines the interface to the OLED C click routines and MHC PLIB APIs.

  Remarks:
    None.
 
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
    (c) 2024 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */
// DOM-IGNORE-END

#ifndef _OLED_C_INTERFACE_H
#define _OLED_C_INTERFACE_H

#include <xc.h>
#include <stdint.h>
#include "definitions.h"                // SYS function prototypes

/** Click routine interfaces/resources Macro Declarations
 *
 *  1. SPI
 *  The OLED C click on PIC32CX SG61 Curiosity Ultra uses
 *  ATMBUSADAPTER-XPRO mikroBUS adapter mounted on EXT1. 
 *  Connect OLED C to the mikroBUS adapter, then plug the adapter into EXT1
 *  The SPI lines from MCU coming to this socket are from
 *  SERCOM0 peripheral. The SPI is configured to use manual chip select.
 *
 * 2. Timer
 *  The OLED C click on PIC32CX SG61 Curiosity Ultra uses
 *  Systick timer module on the MCU to implement the time delay
 *
 * 3. PORTs
 *  The OLED C click uses the following ports pins.
 *  The pins are configured using the MHC Pin configurator
 *  Legend:
 *  PIN_NAME
 *      EXT1_PIN - MCU_PIN - FUNCTION - MCC_PIN_NAME - MCC_PIN_FUNCTION - MCC_PIN_MODE - Direction - Latch
 *  
 *  DISPLAY_CS
 *      P15 - PC24 - CS - DISPLAY_CS - GPIO - Digital - Out - High
 *  DISPLAY_RW
 *      P3 - PB08 - R/W - DISPLAY_RW - GPIO - Digital - Out - Low
 *  DISPLAY_EN
 *      P9 - PD12 - EN - DISPLAY_EN - GPIO - Digital - Out - High
 *  DISPLAY_RST
 *      P5 - PD10 - Reset - DISPLAY_RST - GPIO - Digital - Out - High
 *  DISPLAY_DC
 *      
 */

// SPI Definitions
#define OLED_CommOpen()
#define OLED_Write(x, y)        SERCOM0_SPI_Write(x, y)
#define OLED_IsTrasmitBusy()    SERCOM0_SPI_IsTransmitterBusy()

// Timer Definitions
#define OLED_DelayMS(ms)        SYSTICK_DelayMs(ms)
#define OLED_DelayUS(us)        SYSTICK_DelayUs(us)

// pin definitions
#define OLED_WriteEnable()      DISPLAY_RW_Clear()
#define OLED_EN_High()           DISPLAY_EN_Set()
#define OLED_RST_Low()         DISPLAY_RST_Clear()
#define OLED_RST_High()        DISPLAY_RST_Set()
#define OLED_DC_SetLow()        DISPLAY_DC_Clear()
#define OLED_DC_SetHigh()       DISPLAY_DC_Set()
#define OLED_CS_SetLow()        DISPLAY_CS_Clear()
#define OLED_CS_SetHigh()       DISPLAY_CS_Set()



#endif

/*******************************************************************************
 End of File
 */
