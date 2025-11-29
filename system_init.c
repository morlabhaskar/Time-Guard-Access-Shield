#include "lcd.h"
#include "kpm.h"
#include "rtc.h"

void init_system(){
    //LCD Initialization
    Init_LCD();
    //KPM Initialization
    Init_KPM();
    //RTC Initialization
    RTC_Init();
}
