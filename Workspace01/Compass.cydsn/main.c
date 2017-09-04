/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"
#include <math.h>

uint16 concatenate(uint8 MSB, uint8 LSB) {
    uint16 var;
        var = MSB;
        var = var << 8;
        var = var | LSB;
    return var;       
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    I2C_Start();
    UART_Start();
    
    uint8 compassAddress = 0x1E;
    uint8 I2CRXBuffer[6];
    uint8 * rxPointer = &I2CRXBuffer[0];
    //uint8 xMSB, xLSB, yMSB, yLSB, zMSB, zLSB;
    uint16 x,y,z;
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    //Initialise the compass
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x00); //Select Config Reg A
    I2C_MasterWriteByte(0x50); //Selects 4x avgeraging, 15Hz output and normal mode
    I2C_MasterSendStop(); //Finishes communications
    
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x01); //Select Config Reg B
    I2C_MasterWriteByte(0x20); //Select Normal Gain
    I2C_MasterSendStop(); //Finishes communications
    
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x02); //Selects the mode register
    I2C_MasterWriteByte(0x00); //Selects continuous operation mode
    I2C_MasterSendStop(); //Finishes communications
    CyDelay(6);
    
    char tx[50];
    
    for(;;)
    {
        /*
        I2C_MasterSendStart(compassAddress, 1); //Start a read
        xMSB = I2C_MasterReadByte(I2C_ACK_DATA);
        xLSB = I2C_MasterReadByte(I2C_ACK_DATA);
        yMSB = I2C_MasterReadByte(I2C_ACK_DATA);
        yLSB = I2C_MasterReadByte(I2C_ACK_DATA);
        zMSB = I2C_MasterReadByte(I2C_ACK_DATA);
        zLSB = I2C_MasterReadByte(I2C_ACK_DATA);
        I2C_MasterSendStop();
        
        sprintf(tx, "x = %d\r\n", xMSB);
        UART_PutString(tx);
        sprintf(tx, "y = %d\r\n", yMSB);
        UART_PutString(tx);
        sprintf(tx, "z = %d\r\n", zMSB);
        UART_PutString(tx);
        
        
        CyDelay(1000);
        */
        
        
        I2C_MasterSendStart(compassAddress, 0); //Write mode
        I2C_MasterWriteByte(0x03);  //Points the compasss to the first output register
        I2C_MasterSendStop(); 
        
        I2C_MasterClearStatus();
        I2C_MasterReadBuf(compassAddress, rxPointer, 0x06, I2C_MODE_COMPLETE_XFER);

        
        x = concatenate(I2CRXBuffer[0],I2CRXBuffer[1]);
        y = concatenate(I2CRXBuffer[2],I2CRXBuffer[3]);
        z = concatenate(I2CRXBuffer[4],I2CRXBuffer[5]);
        
        sprintf(tx, "x = %d\r\n", x);
        UART_PutString(tx);
        sprintf(tx, "y = %d\r\n", y);
        UART_PutString(tx);
        sprintf(tx, "z = %d\r\n", z);
        UART_PutString(tx);
        CyDelay(67);
        
    }
}

/* [] END OF FILE */
