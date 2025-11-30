#include<LPC21xx.h>
#include<string.h>
#include "lcd.h"
#include "lcd_defines.h"
#include "pin_connect_block.h"
#include "types.h"
#include "rtc.h"
#include "kpm.h"
#include "delay.h"
#include "arduino.h"
#include "system_init.h"

#define ENTRY_SW 0 //@P0.0 ENTRY
#define EINT_SW 1  //@P0.1 EINT0

#define EINT0_PIN_0_1 3
#define EINT0_VIC_CHNO 14
// #define EINT0_STATUS_LED 16

u8 password[] = "1234",pass[10],repass[10];
s32 hour=1,min=0,sec=0,date=28,month=11,year=2025,day=5,is_login,temp;
s32 EnHr=2,EnMin=0,ExHr=8,ExMin=0;
u32 key,i;

cu8 LockLUT[]={0x0E,0x11,0x11,0x1F,0x1B,0x1B,0x1F,0x00};
cu8 SuccessLUT[]={0x01,0x03,0x16,0x1C,0x08,0x00,0x00,0x00};
cu8 WarningLUT[]={0x04,0x0E,0x0E,0x04,0x00,0x04,0x00,0x00};
cu8 ClockLUT[]={0x0E,0x11,0x15,0x15,0x11,0x0E,0x00,0x00};

void display_title(){
    BuildCGRAM((u8*)ClockLUT,8);
    CmdLCD(GOTO_LINE1_POS0+1);
    CharLCD(0);
    CmdLCD(GOTO_LINE1_POS0+3);
    StrLCD("TIME GUARD");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD(" ACCESS SHIELD  ");
    delay_ms(1500);
    CmdLCD(CLEAR_LCD);
}

void display_RTC(){
    CmdLCD(CLEAR_LCD);
    CmdLCD(CLEAR_LCD);
    while(digitalRead(ENTRY_SW)){
	    GetRTCTimeInfo(&hour,&min,&sec);
	    DisplayRTCTime(hour,min,sec);

	    GetRTCDateInfo(&date,&month,&year);
	    DisplayRTCDate(date,month,year);

	    GetRTCDay(&day);
	    DisplayRTCDay(day);
    }
}
u32 check_working_hours(){
    s32 cHour=HOUR,cMin=MIN;
    if((EnHr<=cHour)&&(ExHr>=hour)){
        return 1;
    }
    return 0;
}

u32 check_password(){
    CmdLCD(CLEAR_LCD);
    i = 0;
    memset(pass, 0, sizeof(pass));
    BuildCGRAM((u8 *)LockLUT, 8);
    CmdLCD(GOTO_LINE1_POS0);
    CharLCD(0);
    CmdLCD(GOTO_LINE1_POS0 + 2);
    StrLCD("ENTER PASSWORD");
    CmdLCD(GOTO_LINE2_POS0 + 5);
    while(i<4){
        key = KeyScan();
        if(key >= '0' && key <= '9'){
            pass[i] = key;
            // CharLCD(key);
            CharLCD('*');
            i++;
        }
        else if(key == 'C'){
            i--;
            pass[i]='\0';
            CmdLCD(GOTO_LINE2_POS0+5+i);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0+5+i);
        }
        pass[i]='\0';
        while(ColScan()==0);
    }
    if(strcmp(password,pass)==0){
        CmdLCD(CLEAR_LCD);
        BuildCGRAM((u8 *)SuccessLUT, 8);
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0 + 2);
        StrLCD("LOGIN SUCCESS   ");
        is_login=1;
        delay_ms(2000);
        CmdLCD(CLEAR_LCD);
        return 1;
    }
    else{
        CmdLCD(CLEAR_LCD);
        BuildCGRAM((u8*)WarningLUT,8);
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0 + 2);
        StrLCD("WRONG PASSWORD  ");
        delay_ms(2000);
        return 0;
    }
}
void entry(){
    if(check_working_hours()==0){
        BuildCGRAM((u8 *)ClockLUT, 8);
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0 + 2);
        StrLCD("ENTRY TIME");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("  EXCEEDED");
        delay_ms(1000);
    }
    else{
        u32 count=0,fail=0,status;
        while(count<3){
            status = check_password();
            if(status==1) break;
            CmdLCD(CLEAR_LCD);
            BuildCGRAM((u8*)WarningLUT,8);
            CmdLCD(GOTO_LINE1_POS0);
            CharLCD(0);
            if(count!=3){
                CmdLCD(GOTO_LINE1_POS0+2);
                StrLCD("TRY AGAIN");
            }
            delay_ms(1000);
            count++;
        }
        if(count==3) fail=1;
        if(fail){
            CmdLCD(CLEAR_LCD);
            StrLCD("3TIMES ATTEMPTED");
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("  LOGIN FAILED  ");
            delay_ms(1000);
        }
    }
}

void change_time(){
    u8 str[4];
    s32 var;
    CmdLCD(CLEAR_LCD);
    BuildCGRAM((u8 *)ClockLUT, 8);
    do{
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0+2);
        StrLCD("SET HOURS-HH");
        for(temp=0;temp<2;temp++){
            key=KeyScan();
            str[temp]=key;
            CmdLCD(GOTO_LINE2_POS0+temp);
            CharLCD(key);
            delay_ms(200);
            while(ColScan()==0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<-1 || var>23);
    hour=var;
    var=0;
    CmdLCD(CLEAR_LCD);
    do{
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0+2);
        StrLCD("SET MINUTES-MM");
        for(temp=0;temp<2;temp++){
            key=KeyScan();
            str[temp]=key;
            CmdLCD(GOTO_LINE2_POS0+temp);
            CharLCD(key);
            delay_ms(200);
            while(ColScan()==0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<-1 || var>59);
    min=var;
    var=0;
    CmdLCD(CLEAR_LCD);
    do{
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0+2);
        StrLCD("SET SECONDS-SS");
        for(temp=0;temp<2;temp++){
            key=KeyScan();
            str[temp]=key;
            CmdLCD(GOTO_LINE2_POS0+temp);
            CharLCD(key);
            delay_ms(200);
            while(ColScan()==0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<-1 || var>59);
    sec=var;
    SetRTCTimeInfo(hour,min,sec);
    CmdLCD(CLEAR_LCD);
    StrLCD("TIME UPDATED");
    delay_ms(500);
    CmdLCD(CLEAR_LCD);
    StrLCD("   ");
}
void change_password(){
    i = 0;
    CmdLCD(CLEAR_LCD);
    BuildCGRAM((u8 *)LockLUT, 8);
    CmdLCD(GOTO_LINE1_POS0);
    CharLCD(0);
    CmdLCD(GOTO_LINE1_POS0+2);
    StrLCD("NEW PASSWORD");
    CmdLCD(GOTO_LINE2_POS0 + 5);
    while (i < 4){
        key = KeyScan();
        if (key >= '0' && key <= '9'){
            pass[i] = key;
            CharLCD('*');
            i++;
        }
        else if (key == 'C'){
            i--;
            pass[i] = '\0';
            CmdLCD(GOTO_LINE2_POS0 + 5 + i);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0 + 5 + i);
        }
        pass[i] = '\0';
        while (ColScan() == 0);
    }
    if (strcmp(password, pass) == 0){
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("PRESENT PASS");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("PREV PASS SAME");
        delay_ms(3000);
        CmdLCD(CLEAR_LCD);
    }
    else{
        i = 0;
        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0+2);
        StrLCD("RE-ENTER PASS");
        CmdLCD(GOTO_LINE2_POS0 + 5);
        while (i < 4){
            key = KeyScan();
            if (key >= '0' && key <= '9'){
                repass[i] = key;
                CharLCD('*');
                i++;
            }
            else if (key == 'C'){
                i--;
                repass[i] = '\0';
                CmdLCD(GOTO_LINE2_POS0 + 5 + i);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0 + 5 + i);
            }
            repass[i] = '\0';
            while (ColScan() == 0);
        }
        if (strcmp(pass, repass) == 0){
            strcpy(password, pass);
            CmdLCD(CLEAR_LCD);
            BuildCGRAM((u8*)SuccessLUT,8);
            CmdLCD(GOTO_LINE1_POS0);
            CharLCD(0);
            CmdLCD(GOTO_LINE1_POS0+2);
            StrLCD("PASSWORD");
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("UPDATE SUCCESS");
            delay_ms(2000);
            CmdLCD(CLEAR_LCD);
        }
        else{
            BuildCGRAM((u8*)WarningLUT,8);
            CmdLCD(GOTO_LINE1_POS0);
            CharLCD(0);
            CmdLCD(GOTO_LINE1_POS0+2);
            StrLCD("NEW PASS RE ");
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("PASS NOT MATCHED");
            delay_ms(2000);
            CmdLCD(CLEAR_LCD);
        }
    }     
    CmdLCD(CLEAR_LCD);
    StrLCD("    ");
}

void change_date(){
    u8 str[4];
    s32 var;
    do{
        CmdLCD(CLEAR_LCD);
        StrLCD("SET DATE - DD");
        for(temp=0;temp<2;temp++){
            key=KeyScan();
            str[temp]=key;
            CmdLCD(GOTO_LINE2_POS0+temp);
            CharLCD(key);
            delay_ms(200);
            while(ColScan()==0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<1 || var>31);
    date=var;
    var=0;
    do{
        CmdLCD(CLEAR_LCD);
        StrLCD("SET MONTH - MM");
        for(temp=0;temp<2;temp++){
            key=KeyScan();
            str[temp]=key;
            CmdLCD(GOTO_LINE2_POS0+temp);
            CharLCD(key);
            delay_ms(200);
            while(ColScan()==0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<1 || var>12);
    month=var;
    var=0;
    CmdLCD(CLEAR_LCD);
    StrLCD("SET YEAR - YYYY");
    for(temp=0;temp<4;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    year=my_atoi(str);
    do{
        CmdLCD(CLEAR_LCD);
        StrLCD("0:S 1:M 2:T 3:W");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("4:T 5:F 6:S");
        delay_ms(200);
        key=KeyScan();
        while(ColScan()==0);
        day=key-'0';
    }while(day<0 || day>6);
    delay_ms(500);
    SetRTCDateInfo(date,month,year);
    SetRTCDay(day);
    CmdLCD(CLEAR_LCD);
    BuildCGRAM((u8*)SuccessLUT,8);
    CmdLCD(GOTO_LINE1_POS0);
    CharLCD(0);
    CmdLCD(GOTO_LINE1_POS0+2);
    StrLCD("DATE UPDATED");
    delay_ms(500);
    CmdLCD(CLEAR_LCD);
    StrLCD("   ");
}

void change_working_hours(){
    u8 str[4];
    CmdLCD(CLEAR_LCD);
    StrLCD("ENTR ETRY HRS-HH");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    EnHr=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("ENTR ETRY MIN-MM ");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    temp=my_atoi(str);
    EnMin=temp;

    CmdLCD(CLEAR_LCD);
    StrLCD("EXT ETRY HRS-HH");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    ExHr=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("ENTR ETRY MIN-MM");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    ExMin=my_atoi(str);
    CmdLCD(CLEAR_LCD);
    BuildCGRAM((u8*)SuccessLUT,8);
    CmdLCD(GOTO_LINE1_POS0);
    CharLCD(0);
    CmdLCD(GOTO_LINE1_POS0+2);
    StrLCD("EMP WORKING");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("HOURS UPDATED");
    delay_ms(1000);
    CmdLCD(CLEAR_LCD);
    StrLCD("   ");
}

void open_menu(){
    int flag;
    key = -1;
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("1:CHANGE DATE 2:CHANGE TIME 3:CHANGE PASSWORD");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("4:CHANGE EMPLOYEE WORKING HOURS   5:EXIT     ");
    while(key == -1){
        CmdLCD(0x18); 
        delay_ms(300);
        key = KeyScan_NonBlocking();  // NON BLOCKING
    }
    key=KeyScan();
    while(ColScan()==0);
    CmdLCD(CLEAR_LCD);
    switch(key){
        case '1':
            change_date();
            break;   
        case '2':
            change_time();
            break;   
        case '3':
            if((is_login==1)){
                change_password();
            }
            else{
                BuildCGRAM((u8*)WarningLUT,8);
                CmdLCD(GOTO_LINE1_POS0);
                CharLCD(0);
                CmdLCD(GOTO_LINE1_POS0+2);
                StrLCD("NOT AUTHORIZED");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("   LOGIN FIRST   ");
                delay_ms(1000);
                IN:
                CmdLCD(CLEAR_LCD);
                flag=check_password();
                if(flag==0){
                    goto IN;
                }
            }
            if(flag==1){
                if(check_working_hours()==1){
                    change_password();
                }
                else{
                    BuildCGRAM((u8 *)ClockLUT, 8);
                    CmdLCD(GOTO_LINE1_POS0);
                    CharLCD(0);
                    CmdLCD(GOTO_LINE1_POS0 + 2);
                    StrLCD("ENTRY TIME");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("  EXCEEDED");
                    delay_ms(1000);
                    CmdLCD(CLEAR_LCD);
                }
            }
            break; 
        case '4':
            change_working_hours();
            break;
        case '5':
            break;
        default:
            break; 
    }
}

void eint0_isr(void) __irq{
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("   SHOW MENU   ");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("   TO PRESS 1   "); 
    key=KeyScan();
    while(ColScan()==0);
    switch(key){
        case '1':
            open_menu();
            break;
        default:
            break;
    }
    //eint0 isr user activity ends
    //clear EINT0 status in External Interrupt Peripheral 
    EXTINT = 1<<0;
    //clear EINT0 status in VIC peripheral
    VICVectAddr = 0;
}

main(){
    init_system();
    delay_ms(10);
    // IODIR0 &= ~(1<<ENTRY_SW);

    //cfg p0.1 pin as EINT0 input pin
    CfgPortPinFunc(0,1,EINT0_PIN_0_1);
    VICIntEnable = 1<<EINT0_VIC_CHNO;
    VICVectCntl0 = (1<<5) | EINT0_VIC_CHNO;
    VICVectAddr0 = (u32)eint0_isr;
    EXTMODE  = 1<<0;   // Edge trigger
    EXTPOLAR = 0;      // Falling edge

    // Set the initial time (hours, minutes, seconds)
	SetRTCTimeInfo(hour,min,sec);
	SetRTCDateInfo(date,month,year);
	SetRTCDay(day);
    while(1){
        do{
            display_title();
            display_RTC();
        }while(digitalRead(ENTRY_SW));
        CmdLCD(CLEAR_LCD);
        if(digitalRead(ENTRY_SW)==0){
            entry();
        }
        while(1){
            display_RTC();
        }
    }
    // while(1){
    //     do{
    //         display_title();
    //         display_RTC();
    //         key1=KeyScan();
    //         while(ColScan()==0);
    //         CmdLCD(CLEAR_LCD);
    //         CharLCD(key1);
    //         delay_ms(500);
    //     }while(key1!='/');
    //     // CmdLCD(CLEAR_LCD);
    //     if(key1=='/'){
    //         entry();
    //     }
    //     while(1){
    //         display_RTC();
    //     }
    // }


}


