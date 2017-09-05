
#include <project.h>
#include <stdlib.h>
#include <stdio.h>

/* Function Prototypes */
void Start();
void AvoidBlock();
void Turn(int side);
void WheelSettings(int side, int dir, int speed);
void Drive(double revs, int dir, int speed);
void Pivot();
double SenseDistChange(int ultraNum, int refDist);
int isStraight();
void Straighten();
void FireUltrasonic(int ultraNum);
CY_ISR_PROTO(Ultrasonic_Handler);
CY_ISR_PROTO(ResetButton_Handler);
CY_ISR_PROTO(TaskButton_Handler);
//colour sensing 
void LEDSequence(); 
void ColourSensing();


/* Global Variables */
int OneRev = 14550; // how many shaft encoder counts is one revolution of wheels
int ultraCounter[2] = {0,0};  //These need to increase to 4 for all 4 ultras
float ultraDistance[2] = {0,0};
int controlRegValue = 0;
char tx[50]; //used for sending things through UART
int ultraNum = 0;
int *ultraNumPointer = &ultraNum;
int taskNum = 0; // used to keep track of how many times the task button has been pressed, initially goes to task 1

//colour sensing 
int LightIntensity = 0;
int AmbientIntensity = 0;
int RedIntensity = 0;
int BlueIntensity = 0;
int GreenIntensity = 0;
int LargestLightIntensity;
char LargestColour[10];

/*  Notes
   
For all of the functions used which take side as input:
    right = 1
    left = 0
    forward = 1
    backwards = -1
Under this is old notes, might not be applicable anymore
    0th bit is the 0th / 1st ultrasonic
    When this gets upgraded to 4 ultrasonics, the 0th and 1st bit will be used
*/
/*--------------------------------------------------------------------------*/
/* Main Code */
int main()
{
    Start();
    LED_Write(0);
    for(;;)
    { 

    }
    return 0;
}// end main
/*--------------------------------------------------------------------------*/
// (1) Function used to control speed and direction of a given wheel
void WheelSettings(int side, int dir, int speed)
{
    /* RIGHT WHEEL */
    if (side == 1)
    {
    	switch(dir)
    	{
    		case 1: // move forward
    			RightMotorOut1_Write(0);
    			RightMotorOut2_Write(1);
    			break;
    		case -1: // move backwards
    			RightMotorOut1_Write(1);
    			RightMotorOut2_Write(0);
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
    			LeftMotorOut1_Write(0);
    			LeftMotorOut2_Write(1);
    			break;
    		case -1: // move backwards
    			LeftMotorOut1_Write(1);
    			LeftMotorOut2_Write(0);
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
// (2) function to make the robot turn 90deg either right of left
void Turn(int side)
{
    // side = which direction to turn; right = 1, left = 0
    int LeftCounter = 0;
    int RightCounter = 0;
    
    // set up shaft encoders
    LeftQuadDec_SetCounter(0);
    RightQuadDec_SetCounter(0);
    
    switch (side)
    {
        // turn right
        case (1):
            WheelSettings(0,1,3); //set left wheel to spin forward 
	        WheelSettings(1,-1,3); // set right wheel to spin backwards
            break;
        
        // turn left
        case (0):
            WheelSettings(0,-1,3); //set left wheel to spin backwards 
	        WheelSettings(1,1,3); // set right wheel to spin forwards 
    }
    
    while (LeftCounter<OneRev)
    {
        LeftCounter = abs(LeftQuadDec_GetCounter());
    } 

    // stop
    
    // adjust for right wheel lagging
    WheelSettings(0,0,1); // turn left wheel off
    WheelSettings(1,0,1); // turn right wheel off
      
} // end Turn
/*--------------------------------------------------------------------------*/
// (3)function to make the robot move forwards or backwards at a given speed
void Drive(double revs, int dir, int speed)
{
    int RightCounter = 0;
    int LeftCounter = 0;
    int counter = revs*OneRev; // converts number of revolutions required to a shaft encoder counter value
    int FlagCheckInc; // how many counts between each wheel lag check
    int FlagCheck = 250; // when to first start checking for wheel lag
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
            WheelSettings(0,0,1); // turn left wheel off
            RightCounter = abs(RightQuadDec_GetCounter());
            while (RightCounter < LeftCounter)
            {
                RightCounter = abs(RightQuadDec_GetCounter());
            }
            WheelSettings(0,dir,speed); // turns left wheel back on 
            FlagCheck += FlagCheckInc;
        } 
    }
    //stop wheels
    WheelSettings(0,0,1); 
    WheelSettings(1,0,1);
}
/*--------------------------------------------------------------------------*/
/* (4)turns the robot 180deg  */
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
    ultraCounter[ultraNum] = ultraTimer_ReadCapture();
    ultraDistance[ultraNum] = (65535-ultraCounter[ultraNum])/58; //Distance measured in cm
    sprintf(tx, "Ultra %d = %dcm\r\n", ultraNum, (int) ultraDistance[ultraNum]);
    UART_PutString(tx);  
}
/*--------------------------------------------------------------------------*/
CY_ISR(TaskButton_Handler)
{
    // The task button allows to move sequentially from task 0 to task 4
    // you can't move onto the next task until the present task has finished
    // task 0 is a "rest" mode
   
    // DEBUGGING CODE
    /*
    Pin_LED_Write(1);
    CyDelay(10);
    Pin_LED_Write(0);
    */
    
    if (taskNum < 4){taskNum++;} // increment task number counter to go to next task 
    else {taskNum = 1;} // if at task 4 go back to task 1

    switch(taskNum)
    {
        // Drives forward, then back
        case(1):
            LED_Write(1);
            CyDelay(10);
            LED_Write(0);
            Drive(2,1,3); // forward for 2*revs, fast
            Drive(2,-1,3); // reverse for 2*revs, fast
            break;
            
        // Drives forwards, then pivots, then drives forward back to base
        case(2):
            LED_Write(1);
            CyDelay(10);
            LED_Write(0);
            Drive(2,1,3); // forward for 2*revs, fast
            Pivot();
            Drive(1,1,3); // forward for 1*rev, fast
            
        case(3):
        // avoids block
            LED_Write(1);
            CyDelay(10);
            LED_Write(0);
           // AvoidBlock();
            break;
        
        case(4): 
        // colour sensing  
            while(1){
                ColourSensing();
            }
            break;

    }    
    TaskButton_isr_ClearPending();
}
/*--------------------------------------------------------------------------*/
CY_ISR(ResetButton_Handler)
{
    // resets code when reset button is pressed
    
    LED_Write(1);
    CyDelay(100);
    LED_Write(0);
    ResetButton_isr_ClearPending();
    CySoftwareReset();
}
/*--------------------------------------------------------------------------*/

/* (5)Fires the ultrasonic  */
void FireUltrasonic(int localNum)
// Function to fire a given ultrasonic sensor
// Input: ultranNo = which ultrasonic sensor is being fired, takes values from 0 to 
{
    *ultraNumPointer = localNum; //Passes the value out to the global version of ultraNum
    ultraControl_Write(localNum); // write to Mux which ultrasonic to use
    switch (localNum)
    {
        //left side
        case 0:     
            ultraTrigger_0_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_0_Write(0); // finishes init pulse
            CyDelay(1);
            break;
        
        // right side
        case 1:
            ultraTrigger_1_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_1_Write(0); // finishes init pulse
            CyDelay(1);
            break;
        
        // left side on front
        case 2:
            ultraTrigger_2_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_2_Write(0); // finishes init pulse
            CyDelay(100);
            break;
        
        // right side on front
        case 3:
            ultraTrigger_3_Write(1); // starts init pulse
            CyDelayUs(10);
            ultraTrigger_3_Write(0); // finishes init pulse
            CyDelay(100);
    }
}
/*--------------------------------------------------------------------------*/
// (6)turns the robot 180deg  */WORK IN PROGRESS - NOT WORKING YET
void AvoidBlock()
{
    int initDist = 0; // initial distance detected
    Drive(0.3,1,3); // drive forward for 0.3*revs at medium speed
    Turn(0); // turn right
    FireUltrasonic(0);
    initDist = ultraDistance[0]; 
    
    // commence sensing for block
    
    
    Sense(0, 2, 0.5); // sense using ultrasonic 0, use 3x reference distance as stopping criteria, step size = 0.3 revs
   
    Drive(0.3,1,1);
    Turn(1); // turn left
    Drive(2,1,3);
    Turn(1); // turn left
    Sense(0, 2, 0.3);
    Turn(1); // turn left
    Drive (2,1,3); // drive forwards   
    
}
/*--------------------------------------------------------------------------------*/
/* (7) */
double SenseDistChange(int ultraNum, int refDist)
{
// returns the change in distance between as a fraction 
// (e.g. if distance measured is 0.5*refDist, returns 0.5
// ultraNum = which ultrasonic sensor is to be used
// refDist = distance to be compared with

    int dist = 0; // distance measured by ultrasonic sensor
    FireUltrasonic(ultraNum);
    dist = ultraDistance[ultraNum];
    return abs(dist - refDist)/refDist;  
 }
 /*-------------------------------------------------------------------------------------*/
// (8) Start up routine
void Start()
{
    //Initialising all interrupts
    CyGlobalIntEnable;
    TaskButton_isr_StartEx(TaskButton_Handler);
    ResetButton_isr_StartEx(ResetButton_Handler);
    echo_isr_StartEx(Ultrasonic_Handler);
    TaskButton_isr_ClearPending();
    ResetButton_isr_ClearPending();
    echo_isr_ClearPending();

    //Initialising all modules
    PWM_Start();
    RightQuadDec_Start();
	LeftQuadDec_Start();
    UART_Start();
    ultraTimer_Start();
    //ColourSensing
    TIA_Start();
    ADC_Start();
    IDAC_Start();
    IDAC_SetValue(1);
    ADC_IRQ_Enable();
    
}
/*--------------------------------------------------------------------------------------*/
// (9) checks if robot is straight relative to an object using the front two ultrasonic sensors
int isStraight()
{
    // returns difference in distances measured, i.e. rightDist - leftDist
    int leftDist = 0; // distance measured from sensor 3 (front left)
    int rightDist = 0; // distance measured from sensor 4 (front right)
    
    FireUltrasonic(3);
    leftDist = ultraDistance[3];
    FireUltrasonic(4);
    rightDist = ultraDistance[4];
    return (rightDist - leftDist);   
}
/*--------------------------------------------------------------------------------------*/
// straightens robot up relative to an object using the front two sensors (sensors 3 and 4)
// (10) WORK IN PROGRESS - DOESNT WORK YET
void Straighten()
{
    int diffDist = 0; // difference in distance measured between front two sensors
    int stop = 0; // flag used to stop corrections
    
    diffDist = isStraight();
    
    while(stop == 0)
    {
        if (abs(diffDist) < 2) 
        {stop=1;}
        
    }
    
}

/* (11) LEDSequence=======================================================================================
Variables: 
**LEDState - 0 (on) & 1 (off)
 Turns sequence of LEDs on and off 
*/
void LEDSequence()
{
    // variables & initalisation
    /*BlueLED_Write(1);
    GreenLED_Write(1);
    RedLED_Write(1);*/
    
    if(BlueLED_Read() && GreenLED_Read() && RedLED_Read()){//case all off 
        // all off - Turn blue on  
        BlueLED_Write(0);
        //CyDelay(1000); 
    }
    
    else if (~BlueLED_Read() && GreenLED_Read() && RedLED_Read()) {
        // Turn Blue off and green on 
        BlueLED_Write(1);
        GreenLED_Write(0);
        //CyDelay(1000); 
    }
    
     else if (BlueLED_Read() && ~GreenLED_Read() && RedLED_Read()) {
        // Turn greeen off and red on 
        GreenLED_Write(1);
        RedLED_Write(0);
       // CyDelay(1000); 
    }
    
     else if(BlueLED_Read() && GreenLED_Read() && ~RedLED_Read()){
        //Turn red off and all off
        RedLED_Write(1);
       // CyDelay(1000); 
    }
    
    else {//error case - turn all LEDS off 
        BlueLED_Write(1);
        GreenLED_Write(1);
        RedLED_Write(1);
    }
    
} 
 /*end LEDSequence-------------------------------------------------------------------------*/


/* (12) ColourSensing=======================================================================================
detects red, green and blue pucks and calls LEDSequence()
*/
void ColourSensing() {
    //initalise variables 
    char Tx[50];
    int colourFlag=0; 
    int lightThreshold = 2100; //from testing 
    
    //turn LEDs off initally 
    BlueLED_Write(1);
    GreenLED_Write(1);
    RedLED_Write(1);
    
    CyDelay(1000);//
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        AmbientIntensity = ADC_Read32();  //Read16 give signed values 
        sprintf(Tx, "Ambient = %d\r\n", (int) AmbientIntensity);
        UART_PutString(Tx);
        
        //Test colour 
        LEDSequence(); // Turns on blue
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        BlueIntensity = ADC_Read32();  //Read16 give signed values 
        sprintf(Tx, "Blue = %d\r\n", (int) BlueIntensity);
        UART_PutString(Tx);
        
        LEDSequence(); // Turns on green
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        GreenIntensity = ADC_Read32();  //Read16 give signed values
        sprintf(Tx, "Green = %d\r\n", (int) GreenIntensity);
        UART_PutString(Tx);
        
        LEDSequence(); // Turns on Red
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        RedIntensity = ADC_Read32();  //Read16 give signed values 
        sprintf(Tx, "Red = %d\r\n", (int) RedIntensity);
        UART_PutString(Tx);
        
        LEDSequence(); // Turns all off
        
        // Finds the largest colour value
        if(RedIntensity > BlueIntensity) {
            sprintf(LargestColour, "Red");
            LargestLightIntensity = RedIntensity;
            colourFlag = 1; 
        }
        else {
            sprintf(LargestColour, "Blue");
            LargestLightIntensity = BlueIntensity;
            colourFlag = 3;
        }
         
        if(GreenIntensity > LargestLightIntensity) {
            sprintf(LargestColour, "Green");
            LargestLightIntensity = GreenIntensity; 
            colourFlag=2;
        }
        
        //Compares largest colour value with a thresholded ambient value 
        if(LargestLightIntensity > AmbientIntensity + lightThreshold) {
        sprintf(Tx, "%s is the highest at = %d\r\n", LargestColour, LargestLightIntensity);
        UART_PutString(Tx);
        }
        else{
            colourFlag=0;
        }
        //feedback LEDS ------------------------------------
        if(colourFlag == 1){
            colourFlag =0;
            RedLED_Write(0);
            CyDelay(500);
            RedLED_Write(1);
        } 
        if(colourFlag == 2){
            colourFlag=0;
            GreenLED_Write(0);
            CyDelay(500);
            GreenLED_Write(1);
        } 
        if(colourFlag == 3){
            colourFlag=0;
            BlueLED_Write(0);
            CyDelay(500);
            BlueLED_Write(1);
        } //--------------------------------------------------[
        

}
/*end ColourSensing-------------------------------------------------------------------------*/
