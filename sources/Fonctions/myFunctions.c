#include "myFunctions.h"

void GPIOL_Handler(void);

extern int cpt,taille;
extern uint8_t value;
uint8_t valueLed;

uint32_t rxByte;

void exempleIO2Chenillard(uint8_t type, uint32_t delay)
{
    uint8_t i=0;

    switch (type) {
    case 1:
        //mont�e
        for(i=0;i<8;i++)
        {
            tca9535.Output.all = 0x03; //all led off

            if(i==0)
                tca9535.Output.ports.P0.bit.B0 = 0;
            if(i==1)
                tca9535.Output.ports.P0.bit.B1 = 0;
            else
                tca9535.Output.ports.P0.all ^= 1<<i;

            updateIO2();
            sleep_ms(delay);
        }

        //descente
        for(i=7;i>=0&&i<=7;i--)
        {
            tca9535.Output.all = 0x03; //all led off

            if(i==0)
                tca9535.Output.ports.P0.bit.B0 = 0;
            if(i==1)
                tca9535.Output.ports.P0.bit.B1 = 0;
            else
                tca9535.Output.ports.P0.all ^= 1<<i;

            updateIO2();
            sleep_ms(delay);
        }
        break;
    default:
        //mont�e
        for(i=0;i<8;i++)
        {
            tca9535.Output.all = 0x03; //all led off

            if(i==0)
                tca9535.Output.ports.P0.bit.B0 = 0;
            if(i==1)
                tca9535.Output.ports.P0.bit.B1 = 0;
            else
                tca9535.Output.ports.P0.all ^= 1<<i;

            updateIO2();
            sleep_ms(delay);
        }
        break;
    }
}

uint8_t checkButtonPressInt(void)
{
    uint8_t ret = 0xFF;

    if(i2c_Read(&rxByte, IOEXP_ADDR, PORT0_IN))
    {
        ret = (~rxByte & 0x70);
    }
    return ret;
}

BUTTON_TYPE checkButtonPress(void)
{
    BUTTON_TYPE ret = NO_BUTTON;
    if(i2c_Read(&rxByte, IOEXP_ADDR, PORT0_IN))
    {
        ret = (BUTTON_TYPE)((~rxByte & 0x70) & 0xFF);
    }
    return ret;
}

void configInterrupt(void)
{
    //Configure Pin PL2 for input
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);						
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_2);
    //Optionnel ==
    //GPIODirModeSet(GPIO_PORTL_BASE, GPIO_PIN_2, GPIO_DIR_MODE_IN);
    //GPIOPadConfigSet(GPIO_PORTL_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    //GPIOIntDisable(GPIO_PORTL_BASE, GPIO_PIN_2);
    //GPIOIntClear(GPIO_PORTL_BASE, GPIO_PIN_2);
    //==

    //Configure Pin PL2 for IOExpender interruption.

    GPIOIntRegister(GPIO_PORTL_BASE, GPIOL_Handler);					// Fonctionne avec une fonction quelquonque qui n'est pas d�fini dans le .s ! ?

    GPIOIntTypeSet(GPIO_PORTL_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTL_BASE, GPIO_PIN_2);
    //IntPrioritySet ( INT_GPIOL, 0xe0); 								// Not Needed
    IntEnable(INT_GPIOL);
    //IntMasterEnable();                                                // Not needed
}

void GPIOL_Handler()
{
    GPIOIntClear(GPIO_PORTL_BASE, GPIO_PIN_2);
    valueLed = checkButtonPressInt();   //lecture ne marche pas
    //SysCtlIntClear(INT_GPIOL);
}

void setGreenLed(bool v){

    value = (v)?0xBF:0xFF;
    i2c_Write(IOEXP_ADDR, value, PORT1_OUT);
}

void setRedLed(bool v){
    value = (v)?0xDF:0xFF;
    i2c_Write(IOEXP_ADDR, value, PORT1_OUT);
}

void setYellowLed(bool v){
    value = (v)?0xEF:0xFF;
    i2c_Write(IOEXP_ADDR, value, PORT1_OUT);
}

void setLed(uint8_t v){
    i2c_Write(IOEXP_ADDR, v, PORT1_OUT);
}

void exempleButtonPressInt(void)
{
    static uint8_t currentLed = 0xFF; //green on

    //currentLed = valueLed;
    currentLed ^= (0x40 | valueLed);

    valueLed = 0x00;

    setLed(currentLed);
    sleep_ms(250);

    currentLed ^= 0x40;
    setLed(currentLed);
    sleep_ms(250);


}

void exampleButtonPress()
{
    uint8_t v = (uint8_t)checkButtonPress();
    switch (v)
    {
    case PLUS_PRESSED:
        //Plus is pressed
        setGreenLed(1);
        break;

    case CAL_PRESSED:
        //CAL is pressed
        setGreenLed(0);
        setRedLed(0);
        setYellowLed(0);
        break;

    case PLUS_PRESSED|MINUS_PRESSED:
    //Plus is pressed
    setYellowLed(1);
    break;


    case MINUS_PRESSED:
        //MINUS is pressed
        setRedLed(1);
        break;

    case NO_BUTTON:
        //No button pressed
        break;

    default :
        break;
    }
}

void exampleChenillard(int type)
{
    switch (type)
    {
    case 1:
    {
        cpt=0;
        value = 0xE0;
        while(1)
        {
            sleep_ms(120);
            if(cpt==0)
            {
                value ^= (5<<4);
            }else if(cpt == 1)
            {
                value ^= (6<<4);
            }else if(cpt == 2){
                value ^= (3<<4);
            }else{
                value = 0x8F;
            }
            (cpt>=2)?cpt=0:cpt++;

            i2c_Write(IOEXP_ADDR, value, PORT1_OUT);
        }
        //break;
    }
    default:
    {
        cpt=0;
        value = 0xEF;
        while(1)
        {
            sleep_ms(500);
            if(cpt==0)
            {
                value ^= (5<<4);
            }else if(cpt == 1)
            {
                value ^= (6<<4);
            }else if(cpt == 2){
                value ^= (3<<4);
            }else{
                value = 0x8F;
            }
            (cpt>=2)?cpt=0:cpt++;

            i2c_Write(IOEXP_ADDR, value, PORT1_OUT);
        }
        //break;
    }
    }


}

void sleep_ms(uint32_t time)
{
    // SysCtlDelay(n) performs n loop.
    // For each loop, 3 instructions. Period for each loop = 3* (1/freq osc), 1 CLK per instruction
    // Temporization = 300000*(3*1/80MHz) = 11 ms
    SysCtlDelay(time*80000/3);
}

void sleep_s(uint32_t time)
{
    // SysCtlDelay(n) performs n loop.
    // For each loop, 3 instructions. Period for each loop = 3* (1/freq osc), 1 CLK per instruction
    // Temporization = 300000*(3*1/80MHz) = 11 ms
    SysCtlDelay(time*80*1e6/3);
}
