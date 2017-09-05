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
#define PI 3.14159265358979323846

    int I2CReady;
    uint8 compassAddress = 0x1E;

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

void ResetCompassPointer(){
    I2C_MasterSendStart(compassAddress, 0); //Write mode
    I2C_MasterWriteByte(0x03);  //Points the compasss to the first output register
    I2C_MasterSendStop();
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    I2C_Start();
    UART_Start();
    CompassInterrupt_StartEx(CompassInterruptHandler);
    

    int8 I2CRXBuffer[6] = {0,0,0,0,0,0};
    int8 * rxPointer = &I2CRXBuffer[0];
    //uint8 xMSB, xLSB, yMSB, yLSB, zMSB, zLSB;
    int16 x,y,z;
    int16 xCal,yCal,zCal; //Calibration values
    double direction; //Direction the robot is facing
    char tx[50];

    
    
    /*--------------------Initialise the compass---------*/
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x00); //Select Config Reg A
    I2C_MasterWriteByte(0x50); //Selects 4x avgeraging, 15Hz output and normal mode (0x51 for self test)
    I2C_MasterSendStop(); //Finishes communications
    
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x01); //Select Config Reg B
    I2C_MasterWriteByte(0x50); //Select Gain +/- 2.5 Ga
    I2C_MasterSendStop(); //Finishes communications
    
    I2C_MasterSendStart(compassAddress, 0); //Starts communication with the chip in write mode
    I2C_MasterWriteByte(0x02); //Selects the mode register
    I2C_MasterWriteByte(0x00); //Selects continuous operation mode
    I2C_MasterSendStop(); //Finishes communications
    CyDelay(1000);
    
    /*---------Perform self test calibration------*/
    I2C_MasterClearStatus(); //Important but not sure why... Doesn't work without it
    I2C_MasterReadBuf(compassAddress, (uint8 *) rxPointer, 0x06, I2C_MODE_COMPLETE_XFER); //Sacrificial Read because the first values are always 0
    CyDelay(67);
    
    ResetCompassPointer();
    
    I2C_MasterReadBuf(compassAddress, (uint8 *) rxPointer, 0x06, I2C_MODE_COMPLETE_XFER);
    while (I2CReady == 0){
        //Wait for interrupt for new data    
    }
    
    ResetCompassPointer();
    
    I2C_MasterClearStatus(); //Important but not sure why... Doesn't work without it
    I2C_MasterReadBuf(compassAddress, (uint8 *) rxPointer, 0x06, I2C_MODE_COMPLETE_XFER); //Read Data
    
    
    xCal = concatenate(I2CRXBuffer[0],I2CRXBuffer[1]);
    zCal = concatenate(I2CRXBuffer[2],I2CRXBuffer[3]);
    yCal = concatenate(I2CRXBuffer[4],I2CRXBuffer[5]);
    sprintf(tx, "x = %d\r\n", xCal);
    UART_PutString(tx);
    sprintf(tx, "y = %d\r\n", yCal);
    UART_PutString(tx);
    sprintf(tx, "z = %d \r\n", zCal);
    UART_PutString(tx);
    UART_PutString("\r\n");
    I2CReady = 0;
    CyDelay(1000);
    
    for(;;)
   {
        while (I2CReady == 0){
            //Wait for interrupt for new data    
        }
        
        I2C_MasterSendStart(compassAddress, 0); //Write mode
        I2C_MasterWriteByte(0x03);  //Points the compasss to the first output register
        I2C_MasterSendStop();
        
        I2C_MasterClearStatus(); //Important but not sure why... Doesn't work without it
        I2C_MasterReadBuf(compassAddress, (uint8 *) rxPointer, 0x06, I2C_MODE_COMPLETE_XFER); //Read Data
        
        //Turn data into values
        x = concatenate(I2CRXBuffer[0],I2CRXBuffer[1]);
        z = concatenate(I2CRXBuffer[2],I2CRXBuffer[3]);
        y = concatenate(I2CRXBuffer[4],I2CRXBuffer[5]);
        
        direction = atan2(y,x);
        //direction += 
        if (direction < 0) {
            direction += 2*PI;
        }
        
        direction = direction * 180 / PI;
        
        //Now adjust for magnetic deinclination
        //Use http://www.magnetic-declination.com/ to get the angle
        //Ours is 11 degrees 44 minutes
        //Convert to degrees 11.73
        
        direction += 11.73;
        
        sprintf(tx, "x = %d\r\n", x);
        UART_PutString(tx);
        sprintf(tx, "y = %d\r\n", y);
        UART_PutString(tx);
        sprintf(tx, "direction = %d \r\n", (int) direction);
        UART_PutString(tx);
        UART_PutString("\r\n");
        
        I2CReady = 0;
        
        
    }
}


/* [] END OF FILE */
