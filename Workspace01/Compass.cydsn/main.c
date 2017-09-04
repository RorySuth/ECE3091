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

    int I2CReady;

int16 concatenate(uint8 MSB, uint8 LSB) {
    uint16 var;
        var = MSB;
        var = var << 8;
        var = var | LSB;
    return var;       
}

CY_ISR(CompassInterruptHandler) {
    CompassInterrupt_ClearPending();
    
    I2CReady = 1; //Lets the pooling loop know the compass has new values to read
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    I2C_Start();
    UART_Start();
    CompassInterrupt_StartEx(CompassInterruptHandler);
    
    uint8 compassAddress = 0x1E;
    int8 I2CRXBuffer[6] = {0,0,0,0,0,0};
    int8 * rxPointer = &I2CRXBuffer[0];
    //uint8 xMSB, xLSB, yMSB, yLSB, zMSB, zLSB;
    int16 x,y,z;

    
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
        
        
 
        while (I2CReady == 0){
            
        }
        
        I2C_MasterSendStart(compassAddress, 0); //Write mode
        I2C_MasterWriteByte(0x03);  //Points the compasss to the first output register
        I2C_MasterSendStop();
        
        I2C_MasterClearStatus();
        I2C_MasterReadBuf(compassAddress, (uint8 *) rxPointer, 0x06, I2C_MODE_COMPLETE_XFER);
        //point the compass back to the first output reg

        

        
        x = concatenate(I2CRXBuffer[0],I2CRXBuffer[1]);
        y = concatenate(I2CRXBuffer[2],I2CRXBuffer[3]);
        z = concatenate(I2CRXBuffer[4],I2CRXBuffer[5]);
        
        sprintf(tx, "x = %d\r\n", x);
        UART_PutString(tx);
        sprintf(tx, "y = %d\r\n", y);
        UART_PutString(tx);
        sprintf(tx, "z = %d\r\n", z);
        UART_PutString(tx);
        UART_PutString("\r\n");
        
        I2CReady = 0;
        
        
    }
}


/* [] END OF FILE */
