#include<LPC21xx.h>
#include "types.h"
#include "arduino_defines.h"

void pinMode(u32 pinNo,u32 pinDir){
    if(pinNo<32){
        if(pinDir==OUTPUT){
            IODIR0 |= 1<<pinNo;
        }
        else{
            IODIR0 &= ~(1<<pinNo);
        }
    }
    else if((pinNo>=32) && (pinNo<=47)){
        if(pinDir==OUTPUT){
            IODIR1 |= 1<<pinNo;
        }
        else{
            IODIR1 &= ~(1<<pinNo);
        }
    }
}
u32 digitalRead(u32 pinNo){
    u32 bit;
    if(pinNo<32){
        bit = ((IOPIN0>>pinNo)&1);
    }
    else if((pinNo>=32) && (pinNo<=47)){
        bit = ((IOPIN1>>(pinNo-16))&1);
    }
    return bit;
}
void digitalWrite(u32 pinNo,u32 bit){
    if(pinNo<32){
        bit ? (IOSET0 = 1<<pinNo) : (IOCLR0 = 1<<pinNo);
    }
    else if((pinNo>=32) && (pinNo<=47)){
        bit ? (IOSET1 = (1<<(pinNo-16))) : (IOCLR1 = (1<<(pinNo-16)));
    }
}
u32 power_of_2(u32 exponent){
    u32 result=1;
    while(exponent){
        result *= 2;
        exponent--;
    }
    return result;
}
void portMode(u32 pinStartNo,u32 nPins,u32 pinsDir){
    if(pinStartNo<32){
        if(pinsDir==OUTPUT){
            // IODIR0 |= (power_of_2(nPins)-1)<<pinStartNo;
            IODIR0 |= ((1<<nPins)-1)<<pinStartNo;
        }
        else{
            IODIR0 &= ~(((1<<nPins)-1)<<pinStartNo);
        }
    }
    else if((pinStartNo>=32) && (pinStartNo<=47)){
        if(pinsDir==OUTPUT){
            IODIR1 |= ((1<<nPins)-1)<<(pinStartNo-16);
        }
        else{
            IODIR1 &= ~(((1<<nPins)-1)<<(pinStartNo-16));
        }
    }
}
void write2Pins(u32 pinStartNo,u32 nPins,u32 data){
    if(pinStartNo<32){
        IOCLR0 = ((1<<nPins)-1)<<pinStartNo;
        IOSET0 = data<<pinStartNo;
        //IOPIN0 = ((IOPIN0 &~(((1<<nPins)-1)<<pinStartNo)) | (data<<pinStartNo));
    }
    else if((pinStartNo>=32) && (pinStartNo<=47)){
        IOCLR1 = ((1<<nPins)-1)<<(pinStartNo-16);
        IOSET1 = data<<(pinStartNo-16);
        //IOPIN1 = ((IOPIN1 &~(((1<<nPins)-1)<<(pinStartNo-16))) | (data<<(pinStartNo-16)));
    }
}
u32 readPins(u32 pinStartNo,u32 nPins){
    u32 data;
    if(pinStartNo<32){
        data = ((IOPIN0>>pinStartNo)&((1<<nPins)-1));
    }
    else if((pinStartNo>=32) && (pinStartNo<=47)){
        data = ((IOPIN1>>(pinStartNo-16))&((1<<nPins)-1));
    }
	return data;
}
