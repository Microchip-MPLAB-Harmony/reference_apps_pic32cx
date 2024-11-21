/** \file rnbd_helpers.h
 *  \brief This file contains additional APIs to access features support by RNBD series devices.
 */
/*
    (c) 2023 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "rnbd/rnbd.h"

/* This value depends upon the System Clock Frequency, Baudrate value and Error percentage of Baudrate*/
#define RESPONSE_TIMEOUT 65U 

/* RNBd COMMANDS and VALUES used below */
#define RNBD_STOP_ADVERTISING_COMMAND "Y"
#define RNBD_START_ADVERTISING_COMMAND "A"

#define BT_SERVICE_BITMAP 0xC0
#define BT_SERVICE_APPEARENCE "0340"
#define BT_SERVICE_UUID "180D"
#define BT_SERVICE_CHARACTERISTIC_1 "2A37,10,05"
#define BT_SERVICE_CHARACTERISTIC_2 "2A38,02,05"
#define BT_SERVICE_CHARACTERISTIC_3 "2A39,08,05"
#define BT_SERVICE_HRBS "1005,03"

#define RNBD_NEW_LINE "\r\n"
#define RNBD_GET_NAME_COMMAND "GN\r\n"
#define RNBD_SET_APPEARANCE_COMMAND "SDA,"
#define RNBD_SET_SERVICE_UUID_COMMAND "PS,"
#define RNBD_SET_SERVICE_CHARACTERISTICS_COMMAND "PC,"
#define RNBD_WRITE_LOCAL_CHARACTERISTICS_COMMAND "SHW,"
#define RNBD_WRITE_LOCAL_CHARACTERISTIC_PREFIX ",00"

#define RNBD_COMMAND_RESPONSE "\r\nCMD> "
#define RNBD_AOK_COMMAND_RESPONSE "AOK\r\nCMD> "

#define RNBD_SEND_HR_DATA_HANDLE "1002"

// 32 bytes are enough to send commands to RNBD
#define RNBD_CMD_BUFFER_LENGTH  32

typedef enum
{
    RNBD_INIT = 0,
    RNBD_CHECK_FORCE_FACTORY_RESET,
    RNBD_CHECK_MODULE_NAME,
    RNBD_FACTORY_RESET,
    RNBD_REBOOT,
    RNBD_CMD,
    RNBD_STOP_ADVERTISING,
    RNBD_START_ADVERTISING,
    RNBD_SET_NAME,
    RNBD_SET_PROFILE,
    RNBD_SET_APPEARANCE,
    RNBD_SERVICE_UUID,
    RNBD_SET_SERVICE_CHARACTERISTICS_1,
    RNBD_SET_SERVICE_CHARACTERISTICS_2,
    RNBD_SET_SERVICE_CHARACTERISTICS_3,
    RNBD_SET_HRBS,
    RNBD_MODULE_NOT_FOUND,
    RNBD_INIT_ERROR,
    RNBD_INIT_COMPLETE
} RNBD_INITIALIZATION_STATES;


typedef struct 
{
    bool rnbdForceFactoryReset;
    
    char devName[21]; // bluetooth device name limited to 20 characters, last character is the string terminator \0
} RNBD_INIT_DATA;


RNBD_INITIALIZATION_STATES RNBD_Initialize(RNBD_INIT_DATA rnbdInitData);

bool RNBD_SetAppearance(const char *appearance, uint8_t appearanceLen);
bool RNBD_SetServiceUUID(const char *uuid, uint8_t uuidLen);
bool RNBD_SetServiceCharacteristic(const char *service, uint8_t serviceLen);
bool RNBD_WriteLocalCharacteristic(const char *handle, uint8_t handleLen, const char *data, uint8_t dataLen);
bool RNBD_CheckName(char* devName, int devNameLen);

/*******************************************************************************
 End of File
 */
