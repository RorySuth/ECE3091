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


/*----------------Declarations--------------*/
void Start();
uint16 count1;
float distance_measured_1;
char tx[50];
int servo_count;


/*----------------Interrupts-----------*/
CY_ISR(Ultrasonic_1_Handler)
{
    count1 = Ultrasonic_Timer_1_ReadCapture();
    distance_measured_1 = (65535-count1)/58; //Distance measured in cm
    
    sprintf(tx, "%dcm\r\n" , (int) distance_measured_1);
    UART_PutString(tx);
}


int main(void)

{
    CyGlobalIntEnable; /* Enable global interrupts. */
    Start();
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        if (Echo_1_Read() == 0){
            Trigger_1_Write(1);
            CyDelayUs(10);
            Trigger_1_Write(0);
        }
            switch (servo_count) {
            case 0: {
                PWM_1_WriteCompare(1000);
                break;
            }
            case 1: {
                PWM_1_WriteCompare(1500);
                break;
            }
            case 2: {
                PWM_1_WriteCompare(2250);
                break;
            }
            case 3: {
                PWM_1_WriteCompare(1500);
                servo_count = -1;
                break;
            }
            
        }
        ++servo_count;
        CyDelay(1000);
        
    }
}


void Start() {
    Ultrasonic_Timer_1_Start();
    UART_Start();
    // Start interrupts
    echo_1_isr_StartEx(Ultrasonic_1_Handler);
    PWM_1_Start();
   
}
/* [] END OF FILE */
