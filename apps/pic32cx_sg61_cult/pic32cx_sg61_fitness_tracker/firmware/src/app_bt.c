/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_bt.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
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

#include "app_bt.h"
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


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_BT_Initialize function.

    Application strings and buffers are be defined outside this structure.
 */

static APP_BT_DATA app_btData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static RNBD_INIT_DATA rnbdInitData;


// notify listeners about the BT App events
static void APP_BT_Notify(APP_BT_EVENTS notification)
{
    if (app_btData.callback != NULL)
    {
        app_btData.callback(notification);
    }
}

// if SW1 is pressed after reset, the RNBD device will be re-configured entirely
static bool APP_BT_CheckForceFactoryReset()
{
    return (0 == SW1_Get());
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Public Functions
// *****************************************************************************
// *****************************************************************************

// register listeners to BT App events
void APP_BT_CallbackRegister(APP_BT_CALLBACK callback)
{
    app_btData.callback = callback;
}

// update the heart rate value from main app
void APP_BT_SetHeartRate(uint16_t heartRateValue)
{
    app_btData.heartRateValue = heartRateValue;

    app_btData.hrDataReady = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

// initialization of the BT App state machine
void APP_BT_Initialize( void )
{
    app_btData.appState = APP_BT_STATE_INIT;

    memcpy(rnbdInitData.devName, APP_BT_DEVICE_NAME, strlen(APP_BT_DEVICE_NAME));
    
    // check for RNBD Force Factory Reset
    rnbdInitData.rnbdForceFactoryReset = APP_BT_CheckForceFactoryReset();
}
/******************************************************************************
  Function:
    void APP_BT_Tasks ( void )

  Remarks:
    See prototype in app_bt.h.
 */
void APP_BT_Tasks( void )
{
    // this is 16 bit converted to hex, max FFFF - 4 chars + end of string \0
    char btServiceHeartRateValue[5] = "";

    switch (app_btData.appState) {
        case APP_BT_STATE_INIT:
        {
            if (true == rnbdInitData.rnbdForceFactoryReset) {
                printf("APP_BT: SW 1 was pressed. BT Device will be asked to fully re-initialize.\r\n");
            }
            
            app_btData.appState = APP_BT_RNBD_INITIALIZE;
            break;
        }
        
        case APP_BT_RNBD_INITIALIZE:
        {
            // the initialization of RNBD is a state machine
            // call it till init complete
            switch(RNBD_Initialize(rnbdInitData)) {
                case RNBD_INIT_COMPLETE:
                {
                    // notify the listeners that initialization succeeded
                    APP_BT_Notify(APP_BT_INITIALIZED);

                    printf("APP_BT: RNBD Module Initialization Complete !\r\n");

                    app_btData.appState = APP_BT_WAIT_FOR_DATA;
                    
                    break;
                }

                case RNBD_INIT_ERROR:
                {
                    // notify the listeners that initialization failed
                    APP_BT_Notify(APP_BT_INIT_ERROR);

                    printf("APP_BT ERROR: RNBD Module Initialization Failed !\r\n");

                    app_btData.appState = APP_BT_ERROR;
                    
                    break;
                }

                case RNBD_MODULE_NOT_FOUND:
                {
                    // notify the listeners that initialization failed
                    APP_BT_Notify(APP_BT_MODULE_NOT_FOUND);

                    printf("APP_BT INFO: RNBD Module Not Found !\r\n");

                    app_btData.appState = APP_BT_RUN_WITHOUT_MODULE;
                    
                    break;
                }
                
                default:
                {
                    // do nothing
                    break;
                }
            }

            break;
        }


        case APP_BT_WAIT_FOR_DATA:
        {
            // wait for data to be sent over BT module
            if (true == app_btData.hrDataReady)
            {
                printf("APP_BT: Sending heart rate over the air: %d\r\n", app_btData.heartRateValue);

                // prepare the data to be sent
                sprintf(btServiceHeartRateValue, "%x", (uint16_t)app_btData.heartRateValue);

                // send the data
                if (false == RNBD_WriteLocalCharacteristic(RNBD_SEND_HR_DATA_HANDLE, strlen(RNBD_SEND_HR_DATA_HANDLE), btServiceHeartRateValue, strlen(btServiceHeartRateValue)))
                {
                    // this failure should maybe handled
                    // experienced that sometimes the data does not go but it does on another reading
                    printf("APP_BT: Failed to send the data over the air !\r\n");
                }

                app_btData.appState = APP_BT_HR_DATA_SENT;
            }
            
            break;
        }
        
        case APP_BT_HR_DATA_SENT:
        {
            // notify the listeners that the data has been sent
            APP_BT_Notify(APP_BT_HR_SENT);

            // data is not ready anymore
            app_btData.hrDataReady = false;

            // go and wait for data
            app_btData.appState = APP_BT_WAIT_FOR_DATA;

            break;
        }

        case APP_BT_RUN_WITHOUT_MODULE:
        {
            if (true == app_btData.hrDataReady) {
                printf("APP_BT: Running with no BT Module Attached. Data not sent over the air. !\r\n");
                
                APP_BT_Notify(APP_BT_HR_SENT);
                
                app_btData.hrDataReady = false;
            }
            break;
        }
        
        case APP_BT_ERROR:
        {
            // do nothing
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
