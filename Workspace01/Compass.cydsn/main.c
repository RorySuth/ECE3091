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

unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;        
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    I2C_Start();
    UART_Start();
    
    uint8 compassAddress = 0x1E;
    uint8 I2CRXBuffer[6];
    uint8 * rxPointer = &I2CRXBuffer[0];
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    //Initialise the compass
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x02); //Selects the mode register
    I2C_MasterWriteByte(0x00); //Selects continuous operation mode
    I2C_MasterSendStop(); //Finishes communications
    
    int16 x,y,z;
    char tx[50];
    
    for(;;)
    {
        I2C_MasterReadBuf(compassAddress, rxPointer, 6, I2C_MODE_COMPLETE_XFER);
        x = concatenate(I2CRXBuffer[0],I2CRXBuffer[1]);
        y = concatenate(I2CRXBuffer[2],I2CRXBuffer[3]);
        z = concatenate(I2CRXBuffer[4],I2CRXBuffer[5]);
        
        sprintf(tx, "x = %d\r\n", x);
        UART_PutString(tx);
        sprintf(tx, "y = %d\r\n", y);
        UART_PutString(tx);
        sprintf(tx, "z = %d\r\n", z);
        UART_PutString(tx);
        /*
        I2CTXBuffer = 0x20; // This selects the mode register in the compass 
        //in the function call below, 2 is the number of bytes to be transmitted. The buffer is 8 bits long, so this number can be constant unless the buffer is increased.
        I2C_MasterWriteBuf(compassAddress, txPointer, 2, I2C_MODE_COMPLETE_XFER);
        */
        
        
    }
}

/* [] END OF FILE */
