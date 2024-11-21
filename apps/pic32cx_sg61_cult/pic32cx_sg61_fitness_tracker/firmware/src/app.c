/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

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

#include "app.h"
#include "app_bt.h"
#include "display.h"
#include "click_routines/heart_rate_9/heart_rate_9.h"

#include <stdio.h>

// *****************************************************************************
// *****************************************************************************
// Section: Local Constants Definitions
// *****************************************************************************
// *****************************************************************************
#define IDLE_LED_ON()       LED1_Clear()
#define IDLE_LED_OFF()      LED1_Set()
#define IDLE_LED_TOGGLE()   LED1_Toggle()

#define BUSY_LED_ON()       LED2_Clear()
#define BUSY_LED_OFF()      LED2_Set()
#define BUSY_LED_TOGGLE()   LED2_Toggle()


// *****************************************************************************
// *****************************************************************************
// Section: Local Functions Declarations
// *****************************************************************************
// *****************************************************************************
// see function definitions for documentation
static void APP_SysTickCallback(uintptr_t context);
static void APP_SwitchPressCallback(uintptr_t context);
static void APP_BluetoothStatusListner(uintptr_t context);
static void APP_LED_Busy(void);
static void APP_LED_Idle(void);
static void APP_LEDs_Blinking(void);
static void APP_SysTickCallback(uintptr_t context);

// *****************************************************************************
// *****************************************************************************
// Section: Local Data Definitions
// *****************************************************************************
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
 */
/* APP Data */
static APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
 */
// callback for the switch press
static void APP_SwitchPressCallback(uintptr_t context)
{
    appData.switchPressed = true;
}

// sysTick callback
static void APP_SysTickCallback(uintptr_t context)
{
    if (true == appData.ledsBlinking) {
        APP_LEDs_Blinking();
    }
}

// listener for APP_BT messages
static void APP_BluetoothStatusListner(uintptr_t context)
{
    switch (context) {
        case APP_BT_INITIALIZED:
        {
            APP_DEBUG_PRINT("INFO: Bluetooth Module Initialized\r\n");

            appData.bluetoothReady = true;

            break;
        }

        case APP_BT_MODULE_NOT_FOUND:
        {
            APP_DEBUG_PRINT("INFO: Bluetooth Module Not Found. Running without Bluetooth\r\n");

            appData.bluetoothReady = true;

            break;
        }

        case APP_BT_HR_SENT:
        {
            APP_DEBUG_PRINT("INFO: Bluetooth Module HR Data Sent\r\n");

            appData.bluetoothReady = true;

            break;
        }

        case APP_BT_INIT_ERROR:
        {
            APP_DEBUG_PRINT("ERROR: Bluetooth Module Notify Error\r\n");

            APP_DisplayScreenBTInitFailed();
            
            appData.appState = APP_STATE_ERROR_BT_INIT_FAILED;

            appData.bluetoothReady = false;

            break;
        }
    }
}

// lights the LED assigned to Busy status
static void APP_LED_Busy(void)
{
    BUSY_LED_ON();
    IDLE_LED_OFF();
}

// lights the LED assigned to Idle status
static void APP_LED_Idle(void)
{
    BUSY_LED_OFF();
    IDLE_LED_ON();
}

// toggle the LEDs 
static void APP_LEDs_Blinking(void)
{
    static SYSTICK_TIMEOUT timeout;

    if (SYSTICK_IsTimeoutReached(&timeout))
    {
        BUSY_LED_TOGGLE();
        IDLE_LED_TOGGLE();
        SYSTICK_StartTimeOut(&timeout, 250UL);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void)
{
    // initialize the appData
    appData.appState = APP_STATE_INIT;
    appData.bluetoothReady = false;
    appData.dataReady = false;
    appData.switchPressed = false;

    // light the Busy LED
    APP_LED_Busy();

    // start the LEDs blinking
    appData.ledsBlinking = true;

    // start the SYSTEM Tick
    SYSTICK_TimerStart();

    // set the callback to switch press
    EIC_CallbackRegister(EIC_PIN_1, APP_SwitchPressCallback, 0);

    // register callback function for BT application
    APP_BT_CallbackRegister(APP_BluetoothStatusListner);

    // set a callback for sysTick interrupts
    SYSTICK_TimerCallbackSet(APP_SysTickCallback, (uintptr_t)NULL);

    // initialize the display
    APP_DisplayInitialize();

    // initialize the heartrate sensor module
    HR9_ClickInitialize();

    APP_DEBUG_PRINT("INFO: Application initialized\r\n");
}
/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void)
{
    /* Check the application's current state. */
    switch (appData.appState) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {
            // show the first screen
            APP_DisplayShowInitializationScreen();

            appData.appState = APP_STATE_WAIT_FOR_MODULES;

            break;
        }

        case APP_STATE_WAIT_FOR_MODULES:
        {
            if (true == appData.bluetoothReady)
            {
                appData.ledsBlinking = false;
                
                printf("APP: Application waiting for switch press\r\n");

                // shot the help screen
                APP_DisplayScreenHelp();

                appData.appState = APP_STATE_WAIT_FOR_SWITCH_PRESS;
            }

            break;
        }

        case APP_STATE_WAIT_FOR_SWITCH_PRESS:
        {
            // lit the idle LED
            APP_LED_Idle();

            if (true == appData.switchPressed)
            {
                // clear the flag
                appData.switchPressed = false;

                // lit the busy LED
                APP_LED_Busy();

                printf("APP: Switch Pressed, reading HR sensor\r\n");

                // display the wait for HR read screen
                APP_DisplayScreenReadingHR();

                appData.appState = APP_STATE_WAIT_FOR_HR;
            }

            break;
        }

        case APP_STATE_WAIT_FOR_HR:
        {
            // wait for the heart rate have bytes ready
            if (true == HR9_IsByteReady())
            {
                // read the heart rate value
                appData.heartRateValue = HR9_ReadByte();

                if (appData.heartRateValue > 0) // !TODO should also validate reads that are 0 ?
                {
                    printf("APP: HR Sensor read Heart Rate value: %d\r\n", appData.heartRateValue);

                    appData.appState = APP_STATE_HR_READY;
                }
            }

            break;
        }

        case APP_STATE_HR_READY:
        {
            APP_DEBUG_PRINT("INFO: Displaying the HR\r\n");
            
            // display the heart rate value on the screen
            APP_DisplayScreenHR(appData.heartRateValue);

            APP_DEBUG_PRINT("INFO: Triggering the AP_BT to send data over the air\r\n");
            
            // set the heart rate value to the BT App
            APP_BT_SetHeartRate((uint16_t)appData.heartRateValue);
            
            // BT gets busy to process the HR value - will notify when it is ready again
            appData.bluetoothReady = false;

            appData.appState = APP_STATE_WAIT_DATA_WRITING;

            break;
        }

        case APP_STATE_WAIT_DATA_WRITING:
        {
            // wait for the BT to be ready again
            if (true == appData.bluetoothReady) {
                printf("APP: Waiting for switch press\r\n");

                appData.appState = APP_STATE_WAIT_FOR_SWITCH_PRESS;
            }

            break;
        }

        case APP_STATE_ERROR_BT_INIT_FAILED:
        {
            // turn on both LEDs
            IDLE_LED_ON();
            BUSY_LED_ON();
            
            // blink them periodically, based on sysTick
            appData.ledsBlinking = true;

            break;
        }
    }
}

/*******************************************************************************
 End of File
 */
