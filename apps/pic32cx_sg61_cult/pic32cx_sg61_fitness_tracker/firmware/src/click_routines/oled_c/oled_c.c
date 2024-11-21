/*
 *   Basic OLEDC SSD1351 display driver
 */

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

#include <stdint.h>
#include <stdbool.h>

#include "oled_c.h"
#include "oled_c_interface.h"

#define SSD1351_DEFAULT_REMAP      (SSD1351_RMP_INC_HOR | SSD1351_RMP_COLOR_REV | \
                                    SSD1351_RMP_SEQ_RGB | SSD1351_RMP_SCAN_REV | \
                                    SSD1351_RMP_SPLIT_ENABLE | SSD1351_COLOR_65K)

static const uint8_t *OLED_font;
static uint8_t OLED_fontWidth;
static uint8_t OLED_fontHeight;

static void OLED_ByteWrite(uint8_t byte) 
{
    OLED_Write(&byte, 1);
}

static void OLED_SendOneByteOfData(uint8_t byte)
{
    OLED_DC_SetHigh();
    OLED_DelayMS(1);
    OLED_ByteWrite(byte);
    OLED_DC_SetLow();
}

static void OLED_SendBytesOfData(uint8_t *data, size_t len)
{
    OLED_DC_SetHigh();
    OLED_DelayMS(1);
    OLED_Write(data, len);
    OLED_DC_SetLow();
}

static void OLEDC_StartTransmission(void)
{
    OLED_CS_SetLow();
    OLED_DelayUS(100); // Give enough time for the pin to go to LOW before transmitting
}

static void OLEDC_EnterCommandMode(void)
{
    OLED_DC_SetLow();
    OLED_DelayUS(100); // Give enough time for the pin to go to LOW before transmitting
}

static void OLEDC_EnterDataMode(void)
{
    OLED_DC_SetHigh();
    OLED_DelayUS(100);
}

static void OLEDC_StopTransmission(void)
{
    while (OLED_IsTrasmitBusy()) ; 
    OLED_CS_SetHigh();
}

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
void OLEDC_Initialize( void)
{
    OLED_CS_SetLow();
    OLED_DelayMS(1);
    OLED_CommOpen();
    OLED_WriteEnable();
    OLED_EN_High();
    OLED_RST_High();
    OLED_DelayMS(1);
    OLED_RST_Low();
    OLED_DelayMS(10);
    OLED_RST_High();
    OLED_DelayMS(200);
    OLED_DC_SetLow();

    OLEDC_StartTransmission();
    
    OLEDC_EnterCommandMode();
    OLED_ByteWrite(SSD1351_COMMAND_LOCK);
    OLED_SendOneByteOfData(SSD1351_DEFAULT_OLED_LOCK);
    OLED_ByteWrite(SSD1351_COMMAND_LOCK);
    OLED_SendOneByteOfData(SSD1351_DEFAULT_CMD_LOCK);    
    OLED_ByteWrite(SSD1351_SLEEP_ON);

    OLED_ByteWrite(SSD1351_SET_REMAP);
    OLED_SendOneByteOfData(SSD1351_DEFAULT_REMAP);    
    OLED_ByteWrite(SSD1351_MUX_RATIO);
    OLED_SendOneByteOfData(SSD1351_DEFAULT_MUX_RATIO);
    OLED_ByteWrite(SSD1351_SET_START_LINE);
    OLED_SendOneByteOfData(SSD1351_DEFAULT_START_LINE);
    OLED_ByteWrite(SSD1351_SET_OFFSET);
    OLED_SendOneByteOfData(SSD1351_DEFAULT_OFFSET);

    OLED_ByteWrite(SSD1351_MODE_NORMAL);
    OLED_ByteWrite(SSD1351_SLEEP_OFF);
    
    OLEDC_StopTransmission();

    OLED_DelayMS(100);
}

void OLEDC_SetFont(const uint8_t *font, uint8_t fontWidth, uint8_t fontHeight) 
{
    OLED_font = font;
    
    OLED_fontWidth = fontWidth;
    OLED_fontHeight = fontHeight;
}

void OLEDC_DrawCharacter(uint8_t x, uint8_t y, uint8_t sx, uint8_t sy, char ch, uint16_t color) 
{
    const uint8_t *fontP;
    uint8_t i_x, i_y, j_x, j_y;
    uint8_t cols[2], rows[2], clr[2] = {0};
    uint8_t currCharByte;

    
    if (OLED_font == NULL) {
        return;
    }
    fontP = &OLED_font[(ch - ' ') * OLED_fontWidth];
    
    clr[0] |= color >> 8;
    clr[1] |= color & 0x00FF;

    OLEDC_StartTransmission();
    
    for (i_x = 0; i_x < OLED_fontWidth * sx; i_x += sx) 
    { // For each COLUMN of our text
        currCharByte = *fontP++;
        for (i_y = 0; i_y < OLED_fontHeight * sy; i_y += sy) 
        {
            if (currCharByte & 0x01) 
            {
                cols[0] = SSD1351_COL_OFF + x + i_x;
                cols[1] = SSD1351_COL_OFF + x + i_x + sx - 1;
                rows[0] = SSD1351_ROW_OFF + y + i_y;
                rows[1] = SSD1351_ROW_OFF + y + i_y + sy - 1;
                
                OLEDC_EnterCommandMode();
                OLED_ByteWrite(SSD1351_SET_COL_ADDRESS);
                OLED_SendBytesOfData(cols, 2);
                OLED_ByteWrite(SSD1351_SET_ROW_ADDRESS);
                OLED_SendBytesOfData(rows, 2);
                
                OLED_ByteWrite(SSD1351_WRITE_RAM);
                OLEDC_EnterDataMode();
                for (j_x = x + i_x; j_x < x + i_x + sx; j_x++) 
                {
                    for (j_y = y + i_y; j_y < y + i_y + sy; j_y++)
                    {
                        OLED_Write(clr, 2);
                    }
                }
            }
            currCharByte >>= 1;
        }
    }

    OLEDC_StopTransmission();
}

void OLEDC_DrawCharacterWithBackground(uint8_t x, uint8_t y, uint8_t sx, uint8_t sy, char ch, uint16_t color, uint16_t bg_color) 
{
    const uint8_t *fontP;
    if (OLED_font == NULL) {
        return;
    }
    uint8_t i_x, i_y, i, curr_char_byte, line = 0;
    uint8_t cols[2], rows[2], clr[2] = {0}, bg_clr[2] = {0};

    clr[0] |= color >> 8;
    clr[1] |= color & 0x00FF;
    bg_clr[0] |= bg_color >> 8;
    bg_clr[1] |= bg_color & 0x00FF;
    cols[0] = SSD1351_COL_OFF + x;
    cols[1] = SSD1351_COL_OFF + x + OLED_fontWidth * sx - 1;
    rows[0] = SSD1351_ROW_OFF + y;
    rows[1] = SSD1351_ROW_OFF + y + OLED_fontHeight * sy - 1;
    
    OLEDC_StartTransmission();
    
    OLEDC_EnterCommandMode();
    OLED_ByteWrite(SSD1351_SET_COL_ADDRESS);
    OLED_SendBytesOfData(cols, 2);
    OLED_ByteWrite(SSD1351_SET_ROW_ADDRESS);
    OLED_SendBytesOfData(rows, 2);
    OLED_ByteWrite(SSD1351_WRITE_RAM);
    OLEDC_EnterDataMode();

    while (line < OLED_fontHeight)
    {
        for (i_y = 0; i_y < sy; i_y++)
        {
            fontP = &OLED_font[(ch - ' ') * OLED_fontWidth];
            for (i_x = 0; i_x < OLED_fontWidth * sx; i_x += sx)
            { // For each COLUMN of our text
                curr_char_byte = *fontP++;
                curr_char_byte >>= line;
                i = sx;
                while (i--)
                {
                    if (curr_char_byte & 0x01) 
                    {
                        OLED_Write(clr, 2);
                    }
                    else 
                    {
                        OLED_Write(bg_clr, 2);
                    }
                }
            }
        }
        line++;
    }
    
    OLEDC_StopTransmission();
}

void OLEDC_DrawString(uint8_t x, uint8_t y, uint8_t sx, uint8_t sy, char *string, uint16_t color)
{
    while (*string) {
        OLEDC_DrawCharacter(x, y, sx, sy, *string++, color);
        x += OLED_fontWidth * sx + 1;
    }
}

void OLEDC_DrawStringWithBackground(uint8_t x, uint8_t y, uint8_t sx, uint8_t sy, char *string, uint16_t color, uint16_t bg_color)
{
    while (*string) {
        OLEDC_DrawCharacterWithBackground(x, y, sx, sy, *string++, color, bg_color);
        x += OLED_fontWidth * sx + 1;
    }
}

void OLEDC_DrawRectangle(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, uint16_t color)
{
    uint8_t cols[2], rows[2], clr[2] = {0};
    uint8_t x, y;
    uint8_t end_x = start_x + width - 1;
    uint8_t end_y = start_y + height - 1;
    
    start_x = (start_x >= SSD1351_SCREEN_WIDTH) ? (SSD1351_SCREEN_WIDTH - 1) : start_x;
    end_x = (end_x >= SSD1351_SCREEN_WIDTH) ? (SSD1351_SCREEN_WIDTH - 1) : end_x;
    start_y = (start_y >= SSD1351_SCREEN_HEIGHT) ? (SSD1351_SCREEN_HEIGHT - 1) : start_y;
    end_y = (end_y >= SSD1351_SCREEN_HEIGHT) ? (SSD1351_SCREEN_HEIGHT - 1) : end_y;

    if((end_x < start_x) || (end_y < start_y)) {
        return;
    }

    cols[0] = SSD1351_COL_OFF + start_x;
    cols[1] = SSD1351_COL_OFF + end_x;
    rows[0] = SSD1351_ROW_OFF + start_y;
    rows[1] = SSD1351_ROW_OFF + end_y;
    clr[0] |= color >> 8;
    clr[1] |= color & 0x00FF;

    OLEDC_StartTransmission();

    OLEDC_EnterCommandMode();
    OLED_ByteWrite(SSD1351_SET_COL_ADDRESS);
    OLED_SendBytesOfData(cols, 2);
    OLED_ByteWrite(SSD1351_SET_ROW_ADDRESS);
    OLED_SendBytesOfData(rows, 2);
    OLED_ByteWrite(SSD1351_WRITE_RAM);
    OLEDC_EnterDataMode();

    for (x = start_x; x < end_x + 1; x++) 
    {
        for (y = start_y; y < end_y + 1; y++)
        {
            OLED_Write(clr, 2);
        }
    }
    OLEDC_StopTransmission();
}

void OLEDC_DrawImage(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, const uint8_t *img)
{
    uint8_t cols[2],rows[2];
    uint16_t pos = 0;
    uint8_t end_x = start_x + width - 1;
    uint8_t end_y = start_y + height - 1;
    uint16_t pixels_total;
    
    start_x = (start_x >= SSD1351_SCREEN_WIDTH) ? (SSD1351_SCREEN_WIDTH - 1) : start_x;
    end_x = (end_x >= SSD1351_SCREEN_WIDTH) ? (SSD1351_SCREEN_WIDTH - 1) : end_x;
    start_y = (start_y >= SSD1351_SCREEN_HEIGHT) ? (SSD1351_SCREEN_HEIGHT - 1) : start_y;
    end_y = (end_y >= SSD1351_SCREEN_HEIGHT) ? (SSD1351_SCREEN_HEIGHT - 1) : end_y;

    pixels_total = (end_x - start_x + 1) * (end_y - start_y + 1);

    if((end_x < start_x) || (end_y < start_y)) {
        return;
    }
    
    cols[0] = SSD1351_COL_OFF + start_x;
    cols[1] = SSD1351_COL_OFF + end_x;
    rows[0] = SSD1351_ROW_OFF + start_y;
    rows[1] = SSD1351_ROW_OFF + end_y;
    
    OLEDC_StartTransmission();

    OLEDC_EnterCommandMode();
    OLED_ByteWrite(SSD1351_SET_COL_ADDRESS);
    OLED_SendBytesOfData(cols, 2);
    OLED_ByteWrite(SSD1351_SET_ROW_ADDRESS);
    OLED_SendBytesOfData(rows, 2);
    OLED_ByteWrite(SSD1351_WRITE_RAM);
    OLEDC_EnterDataMode();
    
    while(pixels_total--)
    {
        OLED_ByteWrite(img[pos + 1]);
        OLED_ByteWrite(img[pos]);
        pos += 2;
    }
    OLEDC_StopTransmission();
}

/*******************************************************************************
 End of File
 */
