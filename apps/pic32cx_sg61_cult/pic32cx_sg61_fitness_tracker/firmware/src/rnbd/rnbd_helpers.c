/** \file rnbd_helpers.c
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

#include <string.h>
#include <stdio.h>

#include "rnbd_helpers.h"

bool RNBD_CheckName(char* devName, int devNameLen) 
{
    char cmdPrompt[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Module Command Response */

    // create the expected response based on the received device name
    // copy the device name
    memcpy(&cmdPrompt[0], &devName[0], devNameLen);
    
    // add the trailing response from rnbd module
    memcpy(&cmdPrompt[devNameLen], RNBD_COMMAND_RESPONSE, strlen(RNBD_COMMAND_RESPONSE));
    
    return RNBD_SendCommand_ReceiveResponse(RNBD_GET_NAME_COMMAND, strlen(RNBD_GET_NAME_COMMAND), cmdPrompt, strlen(cmdPrompt));
}

bool RNBD_StopAdvertising()
{
    uint8_t index = 0, len = 0;
    char cmdBuf[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Command TX Buffer */
    
    len = strlen(RNBD_STOP_ADVERTISING_COMMAND);
    memcpy(&cmdBuf[index], RNBD_STOP_ADVERTISING_COMMAND, len);
    index += len;

    memcpy(&cmdBuf[index], RNBD_NEW_LINE, strlen(RNBD_NEW_LINE));

    return RNBD_SendCommand_ReceiveResponse(cmdBuf, strlen(cmdBuf), RNBD_AOK_COMMAND_RESPONSE, strlen(RNBD_AOK_COMMAND_RESPONSE));
}

bool RNBD_StartAdvertising()
{
    uint8_t index = 0, len = 0;
    char cmdBuf[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Command TX Buffer */
    
    len = strlen(RNBD_START_ADVERTISING_COMMAND);
    memcpy(&cmdBuf[index], RNBD_START_ADVERTISING_COMMAND, len);
    index += len;

    memcpy(&cmdBuf[index], RNBD_NEW_LINE, strlen(RNBD_NEW_LINE));

    return RNBD_SendCommand_ReceiveResponse(cmdBuf, strlen(cmdBuf), RNBD_AOK_COMMAND_RESPONSE, strlen(RNBD_AOK_COMMAND_RESPONSE));
}

bool RNBD_SetAppearance(const char *appearance, uint8_t appearanceLen) 
{
    uint8_t index = 0, len = 0;
    char cmdBuf[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Command TX Buffer */
    
    len = strlen(RNBD_SET_APPEARANCE_COMMAND);
    memcpy(&cmdBuf[index], RNBD_SET_APPEARANCE_COMMAND, len);
    index += len;

    memcpy(&cmdBuf[index], appearance, appearanceLen);
    index += appearanceLen;

    memcpy(&cmdBuf[index], RNBD_NEW_LINE, strlen(RNBD_NEW_LINE));
    
    return RNBD_SendCommand_ReceiveResponse(cmdBuf, strlen(cmdBuf), RNBD_AOK_COMMAND_RESPONSE, strlen(RNBD_AOK_COMMAND_RESPONSE));
}

bool RNBD_SetServiceUUID(const char *uuid, uint8_t uuidLen) 
{
    uint8_t index = 0, len = 0;
    char cmdBuf[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Command TX Buffer */
    
    len = strlen(RNBD_SET_SERVICE_UUID_COMMAND);
    memcpy(&cmdBuf[index], RNBD_SET_SERVICE_UUID_COMMAND, len);
    
    index += len;
    memcpy(&cmdBuf[index], uuid, uuidLen);
    
    index += uuidLen;
    memcpy(&cmdBuf[index], RNBD_NEW_LINE, strlen(RNBD_NEW_LINE));

    return RNBD_SendCommand_ReceiveResponse(cmdBuf, strlen(cmdBuf), RNBD_AOK_COMMAND_RESPONSE, strlen(RNBD_AOK_COMMAND_RESPONSE));
}

bool RNBD_SetServiceCharacteristic(const char *service, uint8_t serviceLen) 
{
    uint8_t index = 0, len = 0;
    char cmdBuf[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Command TX Buffer */
    
    len = strlen(RNBD_SET_SERVICE_CHARACTERISTICS_COMMAND);
    memcpy(&cmdBuf[index], RNBD_SET_SERVICE_CHARACTERISTICS_COMMAND, len);
    
    index += len;
    memcpy(&cmdBuf[index], service, serviceLen);

    index += serviceLen;
    memcpy(&cmdBuf[index], RNBD_NEW_LINE, strlen(RNBD_NEW_LINE));

    return RNBD_SendCommand_ReceiveResponse(cmdBuf, strlen(cmdBuf), RNBD_AOK_COMMAND_RESPONSE, strlen(RNBD_AOK_COMMAND_RESPONSE));
}

bool RNBD_WriteLocalCharacteristic(const char *handle, uint8_t handleLen, const char *data, uint8_t dataLen) 
{
    uint8_t index = 0, len = 0;
    char cmdBuf[RNBD_CMD_BUFFER_LENGTH] = {0x00}; /**< Command TX Buffer */

    len = strlen(RNBD_WRITE_LOCAL_CHARACTERISTICS_COMMAND);
    memcpy(&cmdBuf[index], RNBD_WRITE_LOCAL_CHARACTERISTICS_COMMAND, len);
    
    index += len;
    memcpy(&cmdBuf[index], handle, handleLen);

    index += handleLen;

    if (dataLen != 0) {
        len = strlen(RNBD_WRITE_LOCAL_CHARACTERISTIC_PREFIX);
        memcpy(&cmdBuf[index], RNBD_WRITE_LOCAL_CHARACTERISTIC_PREFIX, len);

        index += len;
        memcpy(&cmdBuf[index], data, dataLen);
    }

    index += dataLen;
    memcpy(&cmdBuf[index], RNBD_NEW_LINE, strlen(RNBD_NEW_LINE));

    return RNBD_SendCommand_ReceiveResponse(cmdBuf, strlen(cmdBuf), RNBD_AOK_COMMAND_RESPONSE, strlen(RNBD_AOK_COMMAND_RESPONSE));
}


RNBD_INITIALIZATION_STATES RNBD_Initialize(RNBD_INIT_DATA rnbdInitData)
{
    static RNBD_INITIALIZATION_STATES currentInitState;
    static RNBD_INITIALIZATION_STATES rnbdInitState = RNBD_INIT;
    static RNBD_INITIALIZATION_STATES nextState = RNBD_INIT;
    bool rnbdCommandResult = true;
    
    currentInitState = rnbdInitState;
    
    
    switch (rnbdInitState) {
        case RNBD_INIT:
        {
            rnbdCommandResult = RNBD_Init();
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Initializing\r\n");
                rnbdInitState = RNBD_CMD;
                nextState = RNBD_CHECK_FORCE_FACTORY_RESET;
            }
            break;
        }        
        case RNBD_REBOOT:
        {
            rnbdCommandResult = RNBD_RebootCmd();
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Reboot Completed\r\n");
                rnbdInitState = RNBD_CMD;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_CMD:
        {
            rnbdCommandResult = RNBD_EnterCmdMode();
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Entered CMD Mode\r\n");
                
                rnbdInitState = RNBD_STOP_ADVERTISING;
            } else {
                printf("RNBD: Failed entering CMD Mode\r\n");

                rnbdInitState = RNBD_MODULE_NOT_FOUND;
            }
            break;
        }
        case RNBD_STOP_ADVERTISING:
        {
            rnbdCommandResult = RNBD_StopAdvertising(rnbdInitData.devName, strlen(rnbdInitData.devName));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Advertising stopped\r\n");
                rnbdInitState = nextState;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            
            break;
        }
        case RNBD_CHECK_FORCE_FACTORY_RESET:
        {
            if (true == rnbdInitData.rnbdForceFactoryReset) {
                printf("RNBD: Application asked for Bluetooth Module Factory Reset\r\n");

                rnbdInitState = RNBD_FACTORY_RESET;
            } else {
                rnbdInitState = RNBD_CHECK_MODULE_NAME;
            }
            
            break;
        }
        case RNBD_CHECK_MODULE_NAME:
        {
            printf("RNBD: Check Module Name: %s\r\n", rnbdInitData.devName);
            
            rnbdCommandResult = RNBD_CheckName((char*)rnbdInitData.devName, strlen(rnbdInitData.devName));
            
            if (rnbdCommandResult) {
                printf("RNBD: Connected module name matches %s.\r\n\tSkipping RNBD register initialization.\r\n", rnbdInitData.devName);
                
                rnbdInitState = RNBD_START_ADVERTISING;
            } else {
                printf("RNBD: Connected module name does not match %s. Module will be reset to factory settings and re-initialized.\r\n", rnbdInitData.devName);
                
                rnbdInitState = RNBD_FACTORY_RESET;
            }
            
            break;
        }
        case RNBD_FACTORY_RESET:
        {
            rnbdCommandResult = RNBD_FactoryReset(SF_2);
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Factory Reset Done\r\n");
                nextState = RNBD_SET_PROFILE;
                rnbdInitState = RNBD_CMD;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SET_PROFILE:
        {
            rnbdCommandResult = RNBD_SetServiceBitmap(BT_SERVICE_BITMAP);
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Service Bitmap Set to: 0x%02x\r\n", BT_SERVICE_BITMAP);
                rnbdInitState = RNBD_SET_APPEARANCE;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SET_APPEARANCE:
        {
            rnbdCommandResult = RNBD_SetAppearance(BT_SERVICE_APPEARENCE, strlen(BT_SERVICE_APPEARENCE));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Appearance Set to: %s\r\n", BT_SERVICE_APPEARENCE);
                nextState = RNBD_SERVICE_UUID;
                rnbdInitState = RNBD_REBOOT;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SERVICE_UUID:
        {
            rnbdCommandResult = RNBD_SetServiceUUID(BT_SERVICE_UUID, strlen(BT_SERVICE_UUID));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Service UUID Set to: %s\r\n", BT_SERVICE_UUID);
                rnbdInitState = RNBD_SET_SERVICE_CHARACTERISTICS_1;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SET_SERVICE_CHARACTERISTICS_1:
        {
            rnbdCommandResult = RNBD_SetServiceCharacteristic(BT_SERVICE_CHARACTERISTIC_1, strlen(BT_SERVICE_CHARACTERISTIC_1));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Service Characteristics Set to: %s\r\n", BT_SERVICE_CHARACTERISTIC_1);
                rnbdInitState = RNBD_SET_SERVICE_CHARACTERISTICS_2;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SET_SERVICE_CHARACTERISTICS_2:
        {
            rnbdCommandResult = RNBD_SetServiceCharacteristic(BT_SERVICE_CHARACTERISTIC_2, strlen(BT_SERVICE_CHARACTERISTIC_2));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Service Characteristics Set to: %s\r\n", BT_SERVICE_CHARACTERISTIC_2);
                rnbdInitState = RNBD_SET_SERVICE_CHARACTERISTICS_3;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SET_SERVICE_CHARACTERISTICS_3:
        {
            rnbdCommandResult = RNBD_SetServiceCharacteristic(BT_SERVICE_CHARACTERISTIC_3, strlen(BT_SERVICE_CHARACTERISTIC_3));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Service Characteristics Set to: %s\r\n", BT_SERVICE_CHARACTERISTIC_3);
                nextState = RNBD_SET_HRBS;
                rnbdInitState = RNBD_REBOOT;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        case RNBD_SET_HRBS:
        {
            rnbdCommandResult = RNBD_WriteLocalCharacteristic(BT_SERVICE_HRBS, strlen(BT_SERVICE_HRBS), 0, 0);
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: HRBS Set to: %s\r\n", BT_SERVICE_HRBS);
                nextState = RNBD_SET_NAME;
                rnbdInitState = RNBD_REBOOT;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }
        
        case RNBD_SET_NAME:
        {
            rnbdCommandResult = RNBD_SetName(rnbdInitData.devName, strlen(rnbdInitData.devName));
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Device Name Set to: %s\r\n", rnbdInitData.devName);
                rnbdInitState = RNBD_START_ADVERTISING;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            break;
        }

        case RNBD_START_ADVERTISING:
        {
            rnbdCommandResult = RNBD_StartAdvertising();
            if (rnbdCommandResult) {
                rnbdCommandResult = false;
                printf("RNBD: Advertising started\r\n");
                rnbdInitState = RNBD_INIT_COMPLETE;
            } else {
                rnbdInitState = RNBD_INIT_ERROR;
            }
            
            break;
        }

        case RNBD_MODULE_NOT_FOUND:
        {
            // print the error only one time and sit in this state forever
            if (currentInitState != rnbdInitState) {
                printf("RNBD: Module Not found: %d\r\n", currentInitState);
                currentInitState = rnbdInitState;
            }
            break;
        }
        
        case RNBD_INIT_ERROR:
        {
            // print the error only one time and sit in this state forever
            if (currentInitState != rnbdInitState) {
                printf("RNBD: Initialization Error: %d\r\n", currentInitState);
                currentInitState = rnbdInitState;
            }
            break;
        }
        
        case RNBD_INIT_COMPLETE:
        {
            printf("RNBD: Initialization Complete\r\n");
            
            break;
        }
    }
    
    return currentInitState;
}

/*******************************************************************************
 End of File
 */
