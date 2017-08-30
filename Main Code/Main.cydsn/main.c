#include <project.h>
#include <stdlib.h>
#include <stdio.h>

/* Function Prototypes */
void TurnRight();
void WheelSettings(int side, int dir, int speed);
void Drive(int revs, int dir, int speed);
void Pivot();
CY_ISR_PROTO(Ultrasonic_Handler);
void FireUltrasonic(int ultraNo);


/* Global Variables */
int OneRev = 14550; // how many shaft encoder counts is one revolution of wheels
int ultraCounter[2] = {0,0};  //These need to increase to 4 for all 4 ultras
float ultraDistance[2] = {0,0};
int controlRegValue = 0;
char tx[50]; //used for sending things through UART

//Under this is old notes, might not be applicable anymore
//0th bit is the 0th / 1st ultrasonic
//When this gets upgraded to 4 ultrasonics, the 0th and 1st bit will be used

/*--------------------------------------------------------------------------*/
/* Main Code */
int main()
{
        
	//Initialising all modules
    CyGlobalIntEnable;
    
    //The following should be put into a Start() function eventually
    PWM_Start();
    RightQuadDec_Start();
	LeftQuadDec_Start();
    UART_Start();
    echo_isr_StartEx(Ultrasonic_Handler);
    ultraTimer_Start();
    //Ok that wasn't actaully that much
    
    // Driving Sequence
    WheelSettings(1,0,1);
    WheelSettings(0,0,1);
    Drive(5,1,3); // drive forward, fast speed, 3 revolutions
    Drive(5,-1,3); // drive backwards
    Drive(2,1,3); // Drive forwards, fast speed, 3 revolutions
    Pivot(); // turn around
    
    for(;;) 
    {
        FireUltrasonic(0);
        FireUltrasonic(1);
    }
    
    return 0;
}// end main
/*--------------------------------------------------------------------------*/
void WheelSettings(int side, int dir, int speed)
{
    /* RIGHT WHEEL */
    if (side == 1)
    {
    	switch(dir)
    	{
    		case 1: // move forward
    			RightMotorOut1_Write(1);
    			RightMotorOut2_Write(0);
    			break;
    		case -1: // move backwards
    			RightMotorOut1_Write(0);
    			RightMotorOut2_Write(1);
    			break;
    		case 0: // stop moving
    			RightMotorOut1_Write(0);
    			RightMotorOut2_Write(0);
    	}
    	switch(speed)
    	{
    		case 1: // slow
    			PWM_WriteCompare1(130);
    			break;
    		case 2: // medium
    			PWM_WriteCompare1(150);
    			break;
    		case 3: // fast
    			PWM_WriteCompare1(170);
    	}
    }
    
    /* LEFT WHEEL */
    else if (side == 0)
    {
        switch(dir)
    	{
    		case 1: // move forward
    			LeftMotorOut1_Write(1);
    			LeftMotorOut2_Write(0);
    			break;
    		case -1: // move backwards
    			LeftMotorOut1_Write(0);
    			LeftMotorOut2_Write(1);
    			break;
    		case 0: // stop moving
    			LeftMotorOut1_Write(0);
    			LeftMotorOut2_Write(0);
    	}
    	switch(speed)
    	{
    		case 1: // slow
    			PWM_WriteCompare2(130);
    			break;
    		case 2: // medium
    			PWM_WriteCompare2(150);
    			break;
    		case 3: // fast
    			PWM_WriteCompare2(170);
    	}
    }
} // end WheelSettings
/*--------------------------------------------------------------------------*/
void TurnRight()
{
	int LeftCounter = 0;
    LeftQuadDec_SetCounter(0);
	LED_Write(0);
	WheelSettings(0,1,3); //set left wheel to spin forward at med speed
	WheelSettings(1,-1,3); // set right wheel to spin forward at slow speed


	while (LeftCounter<OneRev)
	{
        LeftCounter = abs(LeftQuadDec_GetCounter());
	}
    WheelSettings(0,0,1);
    WheelSettings(1,0,1);
    LED_Write(1);
		
} // end TurnRight
/*--------------------------------------------------------------------------*/
void Drive(int revs, int dir, int speed)
{
    int RightCounter = 0;
    int LeftCounter = 0;
    int counter = revs*14550; // converts number of revolutions required to a shaft encoder counter value
    int FlagCheck = 500;
    RightQuadDec_SetCounter(0);
    LeftQuadDec_SetCounter(0);
    WheelSettings(1,dir,speed); // right wheel
    WheelSettings(0,dir,speed); // left wheel
    
    
    while (LeftCounter < counter)
    {
        LeftCounter = abs(LeftQuadDec_GetCounter());
        // Readjusting slower wheel to go straight
        if (LeftCounter == FlagCheck)
        {
            WheelSettings(0,0,1);
            RightCounter = abs(RightQuadDec_GetCounter());
            while (RightCounter < LeftCounter)
            {
                RightCounter = abs(RightQuadDec_GetCounter());
            }
            WheelSettings(0,dir,speed);
            FlagCheck += 500;
        } 
    }
    //stop wheels
    WheelSettings(0,0,1); 
    WheelSettings(1,0,1);
}
/*--------------------------------------------------------------------------*/
void Pivot()
{
    int LeftCounter = 0;
    LeftQuadDec_SetCounter(0);
	WheelSettings(0,1,3); //set left wheel to spin forwards
	WheelSettings(1,-1,3); // set right wheel to spin backwards


	while (LeftCounter<2*OneRev+1300)
	{
        LeftCounter = abs(LeftQuadDec_GetCounter());
	}
    WheelSettings(0,0,1);
    WheelSettings(1,0,1);

}
/*--------------------------------------------------------------------------*/
CY_ISR(Ultrasonic_Handler) {
    ultraCounter[controlRegValue] = ultraTimer_ReadCapture();
    ultraDistance[controlRegValue] = (65535-ultraCounter[controlRegValue])/58; //Distance measured in cm
    sprintf(tx, "Ultra %d = %dcm\r\n", controlRegValue, (int) ultraDistance[controlRegValue]);
    UART_PutString(tx);
    
}
/*--------------------------------------------------------------------------*/
void FireUltrasonic(int ultraNo)
// Function to fire a given ultrasonic sensor
// Input: ultranNo = which ultrasonic sensor is being fired, takes values from 0 to 
{
    ultraControl_Write(ultraNo); // write to Mux which ultrasonic to use
    switch (ultraNo)
    {
        case 0:     
            ultraTrigger_0_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_0_Write(0); // finishes init pulse
            CyDelay(100);
            break;
        
        case 1:
     
            ultraTrigger_1_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_1_Write(0); // finishes init pulse
            CyDelay(100);
            break;

        /*
        case 2:
     
            ultraTrigger_2_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_2_Write(0); // finishes init pulse
            CyDelay(100);
            break;
        
        case 3:
    
            ultraTrigger_3_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_3_Write(0); // finishes init pulse
            CyDelay(100);
         */
    }
}
/*--------------------------------------------------------------------------*/
