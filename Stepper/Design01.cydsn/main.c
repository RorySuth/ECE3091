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

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    PWM_Start();
    int StepperDirection = 0; //0 = up?, FFFF = down?
    
    for(;;)
    {
        CyDelay(1000);
        StepperDirection = ~StepperDirection;
        StepperDirection_Write(StepperDirection);
    }
}

/* [] END OF FILE */
