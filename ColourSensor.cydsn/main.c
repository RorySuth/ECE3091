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
#include <project.h>
#include<stdio.h>

//Global Variables 
int LightIntensity = 0;

//function declarations 
void LEDSequence(); 

/*main======================================================================================================================
Function turns LEDS on an off in a sequence to test the colour of a puck, the reflection is the highest this corrospondds to the puck colour 
*/
int main()
{
    // int ADCdone = 0; // used to check if ADC has finished converting
    //initalise variables
    int LEDSeqCount=0; 
    char Tx[50];
    
    //initalise hardware 
    TIA_Start();
    ADC_Start();
    UART_Start();
    IDAC_Start();
    IDAC_SetValue(1);
    
    //turn LEDs off initally 
    BlueLED_Write(1);
    GreenLED_Write(1);
    RedLED_Write(1);
    
    
    for(;;)
    {   
        
        LightIntensity = ADC_Read32();  //Read16 give signed values 
        CyDelay(1000);
        sprintf(Tx, "%d\r\n", (int) LightIntensity);
        UART_PutString(Tx);
        
        //Test colour 
        if (LightIntensity == 100000) {
            LEDSequence();
            LightIntensity=0; //reset 
            
         }
        ++LightIntensity;
        //end colour testing if a value below the threshold is obtained with circular buffer 
        
    }
}

/* [] END OF FILE */
/* -------------------------------------------------------------------------------------------- */

/*LEDSequence=======================================================================================

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
        //Turn red off and blue on 
        RedLED_Write(1);
        BlueLED_Write(0);
       // CyDelay(1000); 
    }
    
    else {//error case - turn all LEDS off 
        BlueLED_Write(1);
        GreenLED_Write(1);
        RedLED_Write(1);
    }
    
}  /*end LEDSequence-------------------------------------------------------------------------*/