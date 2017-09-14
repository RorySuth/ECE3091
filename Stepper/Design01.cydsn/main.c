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
void Stepper(int direction, int steps);
int stepperPosition = 0;
uint8 stepperDirection = 0; //FF is down, 0 is up
uint8 * stepperDirectionPointer = &stepperDirection;
CY_ISR_PROTO(StepperCountHandler);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    StepperPWM_Start();
    StepperPWM_Sleep();
    StepperCountInterrupt_StartEx(StepperCountHandler);
    
    for(;;)
    {
        Stepper(0, 200);
        CyDelay(500);
        Stepper(0, 200);
        CyDelay(500);
        Stepper(0, 200);
        CyDelay(500);
        Stepper(1, 200);
        CyDelay(500);
        Stepper(1, 200);
        CyDelay(500);
        Stepper(1, 200);
        CyDelay(500);
    }
}

void Stepper(int direction, int steps) {
    int lastStepperPosition = stepperPosition; //Grabs the current global position before the stepper moves
    *stepperDirectionPointer = direction; //Pass the direction out of the function so the interrupt works
    StepperDirection_Write(direction);
    
    if (direction == 0) { //if the steppe is going up leave steps as positive
    }
    else {               //if the stepper is going down, change steps to -ve for easier calculation
        steps = -steps;
    }
    
    StepperPWM_Wakeup();
    while (stepperPosition != (lastStepperPosition + steps)) { //Checks to see if the global stepper position is equal to the desired end position.
    }
    
    StepperPWM_Sleep(); //Turn off the stepper
    
    
}

CY_ISR(StepperCountHandler){
    StepperPWM_ReadStatusRegister(); //Clear interrupt
    if (stepperDirection == 0) {
        ++stepperPosition; //Increment Stepper
    }
    else {
        --stepperPosition;
    }
}
/* [] END OF FILE */
