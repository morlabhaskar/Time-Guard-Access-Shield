
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

// #define ENTRY_SW 0 //@P0.0 ENTRY
#define EINT_SW 1  //@P0.1 EINT0

#define EINT0_PIN_0_1 3
#define EINT0_VIC_CHNO 14
// #define EINT0_STATUS_LED 16

u8 password[] = "1234",pass[10],repass[10];
s32 hour=3,min=0,sec=0,date=28,month=11,year=2025,day=5,is_login,temp;
s32 EnHr=2,EnMin=0,ExHr=8,ExMin=0;
u32 key,i;

void display_title(){
    CmdLCD(GOTO_LINE1_POS0+3);
    StrLCD("TIME GUARD");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD(" ACCESS SHIELD  ");
    delay_ms(1000);
    CmdLCD(CLEAR_LCD);
}

void display_RTC(){
	GetRTCTimeInfo(&hour,&min,&sec);
	DisplayRTCTime(hour,min,sec);

	GetRTCDateInfo(&date,&month,&year);
	DisplayRTCDate(date,month,year);

	GetRTCDay(&day);
	DisplayRTCDay(day);
}
u32 check_working_hours(){
    s32 cHour=hour;
    if((EnHr<=cHour)&&(ExHr>=hour)){
        return 1;
    }
    return 0;
}

u32 check_password(){
    CmdLCD(CLEAR_LCD);
    i = 0;
    memset(pass, 0, sizeof(pass));
    CmdLCD(GOTO_LINE1_POS0 );
    StrLCD("ENTER PASSWORD");
    while(1){
        CmdLCD(GOTO_LINE2_POS0 + i);
        key = KeyScan();

        if(key >= '0' && key <= '9'){
            pass[i] = key;
            CharLCD('*');
            i++;
        }
        else if((key == 'C')&&(i>0)){
            i--;
            pass[i]='\0';
            CmdLCD(GOTO_LINE2_POS0+i);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0+i);
        }
        else if(key=='+') break;
        while(ColScan()==0);
    }
    pass[i]='\0';
    do{
        key=KeyScan();
        while(ColScan()==0);
    }while(key!='+');
    if(strcmp(password,pass)==0){
        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("LOGIN SUCCESS   ");
        IOCLR0 = 1<<2;
        IOSET0 = 1<<25;
        is_login=1;
        delay_ms(2000);
        CmdLCD(CLEAR_LCD);
        return 1;
    }
    else{
        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("WRONG PASSWORD  ");
        delay_ms(2000);
        return 0;
    }
}
void entry(){
    if(check_working_hours()==0){
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("  ENTRY TIME        ");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("   EXCEEDED         ");
        delay_ms(1000);
        CmdLCD(CLEAR_LCD);
    }
    else{
        u32 count=0,fail=0,status;
        while(count<3){
            status = check_password();
            if(status==1) break;
            CmdLCD(CLEAR_LCD);
            if(count!=3){
                CmdLCD(GOTO_LINE1_POS0);
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
            CmdLCD(CLEAR_LCD);
        }
    }
}

void change_time(){
    u8 str[4];
    s32 var;
    CmdLCD(CLEAR_LCD);
    do{
       
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("SET HOURS-HH");
        temp=0;
        CmdLCD(GOTO_LINE2_POS0+temp);
        while(temp<2){
            key=KeyScan();
            if (key >= '0' && key <= '9'){
                CharLCD(key);
                str[temp]=key;
                temp++;
            }
            else if ((key == 'C')&&(temp>0)){
                temp--;
                str[temp] = '\0';
                CmdLCD(GOTO_LINE2_POS0 + temp);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0  + temp);
            }
            else if(key=='+'&& temp > 0){
                break;
            }
            while (ColScan() == 0);
        }
        str[temp] = '\0';
        var=my_atoi(str);
    }while(var<-1 || var>23);
    hour=var;
    var=0;
    CmdLCD(CLEAR_LCD);
    do{
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("SET MINUTES-MM");
        temp=0;
        CmdLCD(GOTO_LINE2_POS0+temp);
        while(temp<2){
            key=KeyScan();
            if (key >= '0' && key <= '9'){
                CharLCD(key);
                str[temp]=key;
                temp++;
            }
            else if ((key == 'C')&&(temp>0)){
                temp--;
                str[temp] = '\0';
                CmdLCD(GOTO_LINE2_POS0 + temp);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0  + temp);
            }
            else if(key=='+'&& temp > 0){
                break;
            }
            while (ColScan() == 0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<-1 || var>59);
    min=var;
    var=0;
    CmdLCD(CLEAR_LCD);
    do{
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("SET SECONDS-SS");
        temp=0;
        CmdLCD(GOTO_LINE2_POS0+temp);
        while(temp<2){
            key=KeyScan();
            if (key >= '0' && key <= '9'){
                CharLCD(key);
                str[temp]=key;
                temp++;
            }
            else if ((key == 'C')&&(temp>0)){
                temp--;
                str[temp] = '\0';
                CmdLCD(GOTO_LINE2_POS0 + temp);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0  + temp);
            }
            else if(key=='+' && temp > 0){
                break;
            }
            while (ColScan() == 0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str); 
    }while(var<-1 || var>59);
    sec=var;
    SetRTCTimeInfo(hour,min,sec);
    CmdLCD(CLEAR_LCD);
    StrLCD("TIME UPDATED");
    delay_ms(1000);
    CmdLCD(CLEAR_LCD);
    StrLCD("                ");
}
void change_password(){
    i = 0;
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0+2);
    StrLCD("NEW PASSWORD");
    // while(i<4){
    while(1){
        CmdLCD(GOTO_LINE2_POS0 + i);
        key = KeyScan();
        if(key >= '0' && key <= '9'){
            pass[i] = key;
            CharLCD('*');
            i++;
        }
        else if((key == 'C')&&(i>0)){
            i--;
            pass[i]='\0';
            CmdLCD(GOTO_LINE2_POS0+i);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0+i);
        }
        else if(key=='+') break;
        while(ColScan()==0);
    }
    pass[i]='\0';
    do{
        key=KeyScan();
        while(ColScan()==0);
    }while(key!='+');
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
        CmdLCD(GOTO_LINE1_POS0+2);
        StrLCD("RE-ENTER PASS");
        // while(i<4){
        while(1){
            CmdLCD(GOTO_LINE2_POS0 + i);
            key = KeyScan();
            if(key >= '0' && key <= '9'){
                repass[i] = key;
                CharLCD('*');
                i++;
            }
            else if((key == 'C')&&(i>0)){
                i--;
                repass[i]='\0';
                CmdLCD(GOTO_LINE2_POS0+i);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0+i);
            }
            else if(key=='+') break;
            while(ColScan()==0);
        }
        repass[i]='\0';
        do{
            key=KeyScan();
            while(ColScan()==0);
        }while(key!='+');
        if (strcmp(pass, repass) == 0){
            strcpy(password, pass);
            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0+2);
            StrLCD("PASSWORD");
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("UPDATE SUCCESS");
            delay_ms(2000);
            CmdLCD(CLEAR_LCD);
        }
        else{
            CmdLCD(GOTO_LINE1_POS0+2);
            StrLCD("NEW PASS RE ");
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("PASS NOT MATCHED");
            delay_ms(2000);
            CmdLCD(CLEAR_LCD);
        }
    }     
    CmdLCD(CLEAR_LCD);
    StrLCD("                      ");
}

void change_date(){
    u8 str[4];
    s32 var;
    do{
        CmdLCD(CLEAR_LCD);
        StrLCD("SET DATE - DD");
        temp=0;
        CmdLCD(GOTO_LINE2_POS0+temp);
        while(temp<2){
            key=KeyScan();
            if (key >= '0' && key <= '9'){
                CharLCD(key);
                str[temp]=key;
                temp++;
            }
            else if ((key == 'C')&&(temp>0)){
                temp--;
                str[temp] = '\0';
                CmdLCD(GOTO_LINE2_POS0 + temp);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0  + temp);
            }
            else if(key=='+'){
                break;
            }
            while(ColScan() == 0);
        }
        str[temp] = '\0'; 
        var=my_atoi(str);
    }while(var<1 || var>31);
    date=var;
    var=0;
    do{
        CmdLCD(CLEAR_LCD);
        StrLCD("SET MONTH - MM");
        temp=0;
        CmdLCD(GOTO_LINE2_POS0+temp);
        while(temp<2){
            key=KeyScan();
            if (key >= '0' && key <= '9'){
                CharLCD(key);
                str[temp]=key;
                temp++;
            }
            else if ((key == 'C')&&(temp>0)){
                temp--;
                str[temp] = '\0';
                CmdLCD(GOTO_LINE2_POS0 + temp);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0  + temp);
            }
            else if(key=='+'){
                break;
            }
            while (ColScan() == 0);
        }
        str[temp] = '\0';  
        var=my_atoi(str);
    }while(var<1 || var>12);
    month=var;
    var=0;
    CmdLCD(CLEAR_LCD);
    StrLCD("SET YEAR - YYYY");
    temp=0;
    CmdLCD(GOTO_LINE2_POS0+temp);
    while(temp<4){
        key=KeyScan();
        if (key >= '0' && key <= '9'){
            CharLCD(key);
            str[temp]=key;
            temp++;
        }
        else if ((key == 'C')&&(temp>0)){
            temp--;
            str[temp] = '\0';
            CmdLCD(GOTO_LINE2_POS0 + temp);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0  + temp);
        }
        while (ColScan() == 0);
    }
    str[temp] = '\0';  
    do{
        key=KeyScan();
        while(ColScan()==0);
    }while(key!='+'); 
    year=my_atoi(str);
    do{
        CmdLCD(CLEAR_LCD);
        StrLCD("0:S 1:M 2:T 3:W");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("4:T 5:F 6:S");
        key=KeyScan();
        while(ColScan()==0);
        day=key-'0';
    }while(day<0 || day>6);
    SetRTCDateInfo(date,month,year);
    SetRTCDay(day);
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0+2);
    StrLCD("DATE UPDATED");
    delay_ms(500);
    CmdLCD(CLEAR_LCD);
    StrLCD("                 ");
}

void change_working_hours(){
    int status;
    u8 str[4];
    status=check_password();
    if(status==0){
        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("                   ");
        return;
    }
    CmdLCD(CLEAR_LCD);
    StrLCD("ENTY ETRY HRS-HH");
    temp=0;
    CmdLCD(GOTO_LINE2_POS0+temp);
    while(temp<2){
        key=KeyScan();
        if (key >= '0' && key <= '9'){
            CharLCD(key);
            str[temp]=key;
            temp++;
        }
        else if ((key == 'C')&&(temp>0)){
            temp--;
            str[temp] = '\0';
            CmdLCD(GOTO_LINE2_POS0 + temp);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0  + temp);
        }
        else if(key=='+'&& temp > 0){
            break;
        }
        while (ColScan() == 0);
    }
    str[temp] = '\0'; 
    EnHr=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("ENTY ETRY MIN-MM ");
    temp=0;
    CmdLCD(GOTO_LINE2_POS0+temp);
    while(temp<2){
        key=KeyScan();
        if (key >= '0' && key <= '9'){
            CharLCD(key);
            str[temp]=key;
            temp++;
        }
        else if ((key == 'C')&&(temp>0)){
            temp--;
            str[temp] = '\0';
            CmdLCD(GOTO_LINE2_POS0 + temp);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0  + temp);
        }
        else if(key=='+'&& temp > 0){
            break;
        }
        while (ColScan() == 0);
    }
    str[temp] = '\0'; 
    temp=my_atoi(str);
    EnMin=temp;

    CmdLCD(CLEAR_LCD);
    StrLCD("EXT HRS-HH");
    temp=0;
    CmdLCD(GOTO_LINE2_POS0+temp);
    while(temp<2){
        key=KeyScan();
        if (key >= '0' && key <= '9'){
            CharLCD(key);
            str[temp]=key;
            temp++;
        }
        else if ((key == 'C')&&(temp>0)){
            temp--;
            str[temp] = '\0';
            CmdLCD(GOTO_LINE2_POS0 + temp);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0  + temp);
        }
        else if(key=='+'&& temp > 0){
            break;
        }
        while (ColScan() == 0);
    }
    str[temp] = '\0';  
    ExHr=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("EXT MIN-MM");
    temp=0;
    CmdLCD(GOTO_LINE2_POS0+temp);
    while(temp<2){
        key=KeyScan();
        if (key >= '0' && key <= '9'){
            CharLCD(key);
            str[temp]=key;
            temp++;
        }
        else if ((key == 'C')&&(temp>0)){
            temp--;
            str[temp] = '\0';
            CmdLCD(GOTO_LINE2_POS0 + temp);
            CharLCD(' ');
            CmdLCD(GOTO_LINE2_POS0  + temp);
        }
        else if(key=='+'&& temp > 0){
            break;
        }
        while (ColScan() == 0);
    }
    str[temp] = '\0'; 
    ExMin=my_atoi(str);
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("EMP WORKING");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("HOURS UPDATED");
    delay_ms(1000);
    CmdLCD(CLEAR_LCD);
    StrLCD("              ");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("                ");
}

void open_menu(){
    int flag;
    key = -1;
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("1:CHD 2:CHT 3:CP");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("4:CEWH 5:EXIT     ");
    key=KeyScan();
    while(ColScan()==0);
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("                         ");
    switch(key){
        case '1':
            change_date();
            break;   
        case '2':
            change_time();
            break;   
        case '3':
            if(is_login==1){
                change_password();
            }
            else{
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
                    CmdLCD(CLEAR_LCD);
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
    IOSET0 = 1<<26;
    // while(1){
    //     IOSET0 = 1<<26;
    //     IOCLR0 = 1<<26;
    //     delay_ms(200);
    // }
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("   SHOW MENU   ");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("   TO PRESS 1   "); 
    key=KeyScan();
    while(ColScan()==0);
    CmdLCD(CLEAR_LCD);
    switch(key){
        case '1':
            open_menu();
            break;
        default:
            break;
    }
    IOCLR0 |= 1<<26;
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
    IODIR0 |= 1<<26;
    IODIR0 |= 1<<25;
    IODIR0 |= 1<<2;

    IOSET0 = 1<<2;

    //cfg p0.1 pin as EINT0 input pin
    CfgPortPinFunc(0,1,EINT0_PIN_0_1);
    VICIntEnable = 1<<EINT0_VIC_CHNO;
    VICVectCntl0 = (1<<5) | EINT0_VIC_CHNO;
    VICVectAddr0 = (u32)eint0_isr;
    EXTMODE  = 1<<0;   // Edge trigger
    EXTPOLAR = 0;      // Falling edge

	SetRTCTimeInfo(hour,min,sec);
	SetRTCDateInfo(date,month,year);
	SetRTCDay(day);
    display_title();
    
    while(1){
        display_RTC();
        if(ColScan()==0){
            key=KeyScan();
            while(ColScan()==0);
            if(key=='/'){
                entry();
            }
        }
    }
}




