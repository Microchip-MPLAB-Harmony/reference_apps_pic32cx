/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_bt.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_BT_Initialize" and "APP_BT_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_BT_STATES" definition).  Both
    are defined here for convenience.
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

#ifndef _APP_BT_H
#define _APP_BT_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "rnbd/rnbd_interface.h"
#include "rnbd/rnbd_helpers.h"
#include "rnbd/rnbd.h"
#include "peripheral/port/plib_port.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

#define APP_BT_DEVICE_NAME "MCHP_PIC32CX_SG16_HR"
    
// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/
typedef enum
{
    /* Application's state machine's initial state. */
    APP_BT_STATE_INIT = 0,
    APP_BT_RNBD_INITIALIZE,
    APP_BT_WAIT_FOR_DATA,
    APP_BT_HR_DATA_SENT,
    APP_BT_RUN_WITHOUT_MODULE,
    APP_BT_ERROR

} APP_BT_STATES;

// events to notify listeners
typedef enum
{
    APP_BT_INITIALIZED = 0,
    APP_BT_MODULE_NOT_FOUND,
    APP_BT_HR_SENT,
    APP_BT_INIT_ERROR
} APP_BT_EVENTS;

// the callback function signature expected by the event notifier
typedef void (*APP_BT_CALLBACK) (uintptr_t context);

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */
typedef struct
{
    // application state machine states
    APP_BT_STATES appState;

    // flag - hr data ready to be sent
    bool hrDataReady;
    
    // flag - rnbd must be fully initialized
    bool rnbdForceFactoryReset;
    
    // callback to signal application events
    APP_BT_CALLBACK callback;
    
    // the heart rate value
    uint16_t heartRateValue;
    
} APP_BT_DATA;

/*******************************************************************************
  Function:
    void APP_BT_CallbackRegister ( APP_BT_CALLBACK callback )

  Summary:
     

  Description:
    This function 

  Precondition:
    

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    
    </code>

  Remarks:
    This routine registers the callback functions of the event notifier
*/
void APP_BT_CallbackRegister(APP_BT_CALLBACK callback);

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_BT_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_BT_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_BT_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/
void APP_BT_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_BT_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_BT_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */
void APP_BT_Tasks( void );

// update the heart rate value from main app
void APP_BT_SetHeartRate(uint16_t heartRateValue);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_BT_H */

/*******************************************************************************
 End of File
 */

