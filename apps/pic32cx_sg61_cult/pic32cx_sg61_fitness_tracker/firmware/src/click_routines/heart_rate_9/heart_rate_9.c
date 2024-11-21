/*******************************************************************************
  Heart Rate 9 click routine source file

  Company
    Microchip Technology Inc.

  File Name
    heart_rate9.c

  Summary
    Heart Rate 9 click routine Implementation File.

  Description
    This file defines the interface to the Heart Rate 9 click using USART PLIB.
    This click routine provides to read the sensor data on Heart Rate 9 click.

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

/**
  Section: Included Files
 */

#include <string.h>
#include "heart_rate_9.h"
#include "heart_rate_9_interface.h"

/**
  Section: Macro Declarations
 */
// received data is of format: 15;AAAA;BBBB;CCCC;DDD
#define HR9_CLICK_RECEIVE_BUFFER_SIZE (HR9_CLICK_USART_READ_BUFFER_SIZE * 10ul)
#define HR9_CLICK_NUMBER_OF_SAMPLES_TO_PROCESS (HR9_CLICK_RECEIVE_BUFFER_SIZE / 20ul)
#define HR9_CLICK_UART_CALLBACK_THRESHOLD 10ul

typedef enum
{
    HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_1 = 0,
    HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_2,
    HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_3,
    HR9_CLICK_BUFFER_PROCESS_STATE_POSITION_FIND,
    HR9_CLICK_BUFFER_PROCESS_STATE_READ,
} HR9_CLICK_BUFFER_PROCESS_STATE;

/**
  Section: Variable Definitions
 */
static HR9_CLICK_BUFFER_PROCESS_STATE bufferProcessState = HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_1;

static volatile char rxData[HR9_CLICK_UART_CALLBACK_THRESHOLD + 5ul] = {0};
static volatile char rxBuff[HR9_CLICK_RECEIVE_BUFFER_SIZE] = {0};
static volatile uint16_t rxBuffInIndex = 0;
static uint16_t rxBuffOutIndex = 0;
static bool readingHeartRate = false;
static bool clickIntrefaceInitialized = false;
static uint16_t processCounter = 0;
static volatile uint32_t numberOfReceivedBytes = 0;


/**
  Section: Private function prototypes
 */
static void HR9_RxBuffUpdate(void);
static void HR9_ClickReset(void);

/**
  Section: Driver APIs
 */
static void HR9_ReadCallback(SERCOM_USART_EVENT event, uintptr_t context)
{
    uint32_t nBytesAvailable = 0;
    
    if (event == CLICK_HEARTRATE9_USART_EVENT_READ_THRESHOLD_REACHED)
    {
        /* Receiver should atleast have the thershold number of bytes in the ring buffer */
        nBytesAvailable = CLICK_HEARTRATE9_USART_ReadCountGet();
        
        // read the USART buffers
        numberOfReceivedBytes = CLICK_HEARTRATE9_USART_Read((uint8_t*)&rxData[0], nBytesAvailable);
        
        // update the hr buffer
        HR9_RxBuffUpdate();
        
        // clear the rx data
        memset((void*)&rxData[0], 0x00, numberOfReceivedBytes);
    }
}

static void HR9_RxBuffUpdate(void)
{
    // prevent filling the buffer if we are not reading anything...
    // this is inside the interrupt.
    if (false == readingHeartRate) {
        return;
    }
    
    if((rxBuffInIndex + numberOfReceivedBytes) >= HR9_CLICK_RECEIVE_BUFFER_SIZE)
    {
        memcpy((char *)&rxBuff[rxBuffInIndex], (char *)rxData, (HR9_CLICK_RECEIVE_BUFFER_SIZE - rxBuffInIndex));
        memcpy((char *)rxBuff, (char *)&rxData[HR9_CLICK_RECEIVE_BUFFER_SIZE - rxBuffInIndex], (rxBuffInIndex + numberOfReceivedBytes - HR9_CLICK_RECEIVE_BUFFER_SIZE));
    }
    else
    {
        memcpy((char *)&rxBuff[rxBuffInIndex], (char *)&rxData[0], numberOfReceivedBytes);
    }

    rxBuffInIndex += numberOfReceivedBytes;
    if(rxBuffInIndex >= HR9_CLICK_RECEIVE_BUFFER_SIZE)
    {
        rxBuffInIndex = rxBuffInIndex - HR9_CLICK_RECEIVE_BUFFER_SIZE;
    }
}

static void HR9_RxBufferReset(void)
{
    memset((char*)rxBuff, 0x00, sizeof(rxBuff));

    rxBuffInIndex = 0;
    rxBuffOutIndex = 0;

}

void HR9_ClickInitialize(void)
{
    /* Register callback functions and send start message */
    CLICK_HEARTRATE9_USART_ReadCallbackRegister((SERCOM_USART_RING_BUFFER_CALLBACK)HR9_ReadCallback, 0);
    CLICK_HEARTRATE9_USART_Read((uint8_t*)rxBuff, HR9_CLICK_RECEIVE_BUFFER_SIZE);
    CLICK_HEARTRATE9_USART_ReadThresholdSet(HR9_CLICK_UART_CALLBACK_THRESHOLD);
    CLICK_HEARTRATE9_USART_ReadNotificationEnable(true, false);
    CLICK_HEARTRATE9_TimerStart();
    HR9_ClickReset();
    
    clickIntrefaceInitialized  = true;
}

static void HR9_ClickReset(void)
{
    CLICK_HEARTRATE9_RST_Clear();
    CLICK_HEARTRATE9_DelayMs(20);
    CLICK_HEARTRATE9_RST_Set();
}

bool HR9_IsByteReady(void)
{
    if (clickIntrefaceInitialized == false) {
        HR9_ClickReset();
        clickIntrefaceInitialized  = true;
    }
    
    return true;
}

int16_t HR9_ReadByte(void)
{
    static int16_t currentHeartrate = 0;
    static uint8_t semiColonCount = 0;
    uint16_t localRxBufferIndex = rxBuffInIndex;
    
    readingHeartRate = true;
    
    if(rxBuffOutIndex > localRxBufferIndex)
    {
        localRxBufferIndex = HR9_CLICK_RECEIVE_BUFFER_SIZE;
    }
    while(rxBuffOutIndex < localRxBufferIndex)
    {
        switch(bufferProcessState)
        {
            case HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_1:
            {
                for(; rxBuffOutIndex < localRxBufferIndex; rxBuffOutIndex++)
                {
                    if(rxBuff[rxBuffOutIndex] == '1')
                    {
                        bufferProcessState  = HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_2;
                        rxBuffOutIndex++;
                        break;
                    }
                }
                break;
            }
            case HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_2:
            {
                if(rxBuff[rxBuffOutIndex] == '5')
                {
                    bufferProcessState  = HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_3;
                }
                else
                {
                    bufferProcessState  = HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_1;
                }
                rxBuffOutIndex++;
                break;
            }
            case HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_3:
            {
                if(rxBuff[rxBuffOutIndex] == ';')
                {
                    bufferProcessState  = HR9_CLICK_BUFFER_PROCESS_STATE_POSITION_FIND;
                }
                else
                {
                    bufferProcessState  = HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_1;
                }
                rxBuffOutIndex++;
                break;
            }
            case HR9_CLICK_BUFFER_PROCESS_STATE_POSITION_FIND:
            {
                for(; rxBuffOutIndex < localRxBufferIndex; rxBuffOutIndex++)
                {
                    if(rxBuff[rxBuffOutIndex] == ';')
                    {
                        semiColonCount++;
                        if(semiColonCount == 3)
                        {
                            semiColonCount                = 0;
                            rxBuffOutIndex++;
                            currentHeartrate           = 0;
                            bufferProcessState  = HR9_CLICK_BUFFER_PROCESS_STATE_READ;
                            break;
                        }
                    }
                }
                break;
            }
            case HR9_CLICK_BUFFER_PROCESS_STATE_READ:
            {
                for(; rxBuffOutIndex < localRxBufferIndex; rxBuffOutIndex++)
                {
                    if(rxBuff[rxBuffOutIndex] != ';')
                    {
                        currentHeartrate   = (currentHeartrate * 10) + (rxBuff[rxBuffOutIndex] - 0x30);
                    }
                    else
                    {
                        bufferProcessState = HR9_CLICK_BUFFER_PROCESS_STATE_HEADER_BYTE_1;
                        processCounter++;
                        if(processCounter == HR9_CLICK_NUMBER_OF_SAMPLES_TO_PROCESS)
                        {
                            processCounter = 0;
                            
                            readingHeartRate = false;
                            
                            HR9_RxBufferReset();
                            
                            return((int16_t)currentHeartrate);
                        }
                        break;
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    if(rxBuffOutIndex == HR9_CLICK_RECEIVE_BUFFER_SIZE)
    {
        rxBuffOutIndex = 0;
    }

    return(-1);
}

/*******************************************************************************
 End of File
 */
