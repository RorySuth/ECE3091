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
uint32 count = 0;
int cSensorFlag = 0;
uint32 frequency[4] = {0,0,0};
char tx[50];
int compare_occured;
char red[4] = "Red";
char blue[5] = "Blue";
char green[6] = "Green";
char *colours[3] = {red, blue, green};


int colourSense2();
CY_ISR(Compare_ISR_Handler){
 /* `#START ISR_Compare_Interrupt` */
 /* Call the ReadStatusRegister to clear the Interrupt Status bit */
PWM_1_ReadStatusRegister();
/* Set the flag to indicate occurence of the interrupt */
compare_occured = 1;
 /* END */
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    Compare_ISR_StartEx(Compare_ISR_Handler);
    PWM_1_Start();
    Counter_1_Start();
    UART_Start();
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        
        colourSense2();
            
        }
    }
//Function for colour sensor 2
int colourSense2(){
    
    int colour; //1 is R, 2 is B, 3 is G
    int waitOne; //Used for skipping the first returned value as it could be the value for the previous colour
    
    //Set the colour sensor to red
    /*  Red is 00
        Blue is 01
        Clear is 10
        Green is 11*/
    colourSensorOutS2_Write(0);
    colourSensorOutS3_Write(0);
    while (compare_occured == 0 || waitOne == 0){
        //Wait for a new value
        if (compare_occured) {
            waitOne = 1;
            compare_occured = 0;
        }
    }
    
    count = Counter_1_ReadCapture(); //Read the number of captures in 50ms
    frequency[1] = 20 * count; //Multiply by 20 so it is the number of counts in a second; hence Hz.
    sprintf(tx, "Red is %ld \t", frequency[1]);
    UART_PutString(tx);
    compare_occured = 0; //Set the flag back to zero so this part doesn't loop continuously.
    waitOne = 0;
    
    //Set the colour sensor to blue
    //colourSensorOutS2_Write(0);
    colourSensorOutS3_Write(1);
    //Wait for a new value
    while (compare_occured == 0 || waitOne == 0){
       //Wait for a new value 
        if (compare_occured) {
            waitOne = 1;
            compare_occured = 0;
        }
    }
    count = Counter_1_ReadCapture(); //Read the number of captures in 50ms
    frequency[2] = 20 * count; //Multiply by 20 so it is the number of counts in a second; hence Hz.
    sprintf(tx, "Blue is %ld \t", frequency[2]);
    UART_PutString(tx);
    compare_occured = 0; //Set the flag back to zero so this part doesn't loop continuously.
    waitOne = 0;
    
    //Set the colour sensor to green
    colourSensorOutS2_Write(1);
    //colourSensorOutS3_Write(1);
    while (compare_occured == 0 || waitOne == 0){
        //Wait for a new value
        if (compare_occured) {
            waitOne = 1;
            compare_occured = 0;
        }
    }
    count = Counter_1_ReadCapture(); //Read the number of captures in 50ms
    frequency[3] = 20 * count; //Multiply by 20 so it is the number of counts in a second; hence Hz.
    sprintf(tx, "Green is %ld \n", frequency[3]);
    UART_PutString(tx);
    compare_occured = 0; //Set the flag back to zero so this part doesn't loop continuously.
    waitOne = 0;
    
    frequency[0] = frequency[1] +frequency[2] + frequency[3];
    //See if the values are very low, if so, do not calculate the main colour
    if (frequency[0] < 800) {
        colour = 0;
    }
    else{
        if(frequency[2] < frequency[1]) {
            frequency[0] = frequency[1];
            colour = 1;
        }
        else {
            frequency[0] = frequency[2];
            colour = 2;
        }
        if (frequency[0] <= frequency[3]) {
            colour = 3;
        }
        sprintf(tx, "Main colour is %s \n", *(colours+colour-1));
        UART_PutString(tx);
    }

    return colour;
}

/* [] END OF FILE */
