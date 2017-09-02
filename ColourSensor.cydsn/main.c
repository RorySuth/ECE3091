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
#include <string.h>

//Global Variables 
int LightIntensity = 0;
int AmbientIntensity = 0;
int RedIntensity = 0;
int BlueIntensity = 0;
int GreenIntensity = 0;
int LargestLightIntensity;
char LargestColour[10];

extern int ADC_Flag;

//function declarations 
void LEDSequence(); 

/*main======================================================================================================================
Function turns LEDS on an off in a sequence to test the colour of a puck, the reflection is the highest this corrospondds to the puck colour 
*/
int main()
{
    // int ADCdone = 0; // used to check if ADC has finished converting
    //initalise variables
    char Tx[50];
    int colourFlag=0; 
    int lightThreshold = 2000; //from testing 
    
    //initalise hardware 
    TIA_Start();
    ADC_Start();
    UART_Start();
    IDAC_Start();
    IDAC_SetValue(1);
    ADC_IRQ_Enable();
    
    //turn LEDs off initally 
    BlueLED_Write(1);
    GreenLED_Write(1);
    RedLED_Write(1);
    
    
    for(;;)
    {   
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
        
        //feedback buzzer 
        
/*
        if(RedIntensity > BlueIntensity) {
            bigger1 = RedIntensity;
            sprintf(bigger1Flag, "Red"); //This means Red was selected
        }
        else {
            bigger1 = BlueIntensity;
            bigger1Flag = 2; //This means blue was selected
        }
         
        if(GreenIntensity > AmbientIntensity) {
            bigger2 = GreenIntensity;
            bigger2Flag = 3; //This means Green was selected
        }
        else {
            bigger1 = AmbientIntensity;
            bigger1Flag = 4; //This means Ambient was selected
        }
        
        if(bigger1 > bigger2) {
            bigger2 = GreenIntensity;
            biggestFlag = 3; //This means Green was selected
        }
        else {
            bigger2 = AmbientIntensity;
            biggFlag = 4; //This means Ambient was selected
        }

        //end colour testing if a value below the threshold is obtained with circular buffer 
        
    }
}
*/
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