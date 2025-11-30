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
#define EINT0_STATUS_LED 16


u8 password[] = "1234",pass[10],repass[10];
s32 hour=2,min=0,sec=0,date=28,month=11,year=2025,day=5,is_login,temp,EnHr=2,EnMin=0,ExHr=8,ExMin=0;
u32 key,i;

cu8 UserCGRAM[8][8] = {
    {0x0E,0x11,0x11,0x1F,0x1B,0x1B,0x1F,0x00}, // Lock
    {0x04,0x0E,0x0E,0x0E,0x1F,0x00,0x00,0x00}, // Key
    {0x1F,0x11,0x15,0x1B,0x00,0x00,0x00,0x00}, // Backspace
    {0x01,0x03,0x16,0x1C,0x08,0x00,0x00,0x00}, // Checkmark
    {0x1B,0x0E,0x04,0x0E,0x1B,0x00,0x00,0x00}, // Wrong
    {0x04,0x06,0x1F,0x06,0x04,0x00,0x00,0x00}, // Arrow
    {0x04,0x0E,0x0E,0x04,0x00,0x04,0x00,0x00}  // Warning
};

void display_title(){
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("   TIME GUARD   ");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD(" ACCESS SHIELD  ");
    delay_ms(1000);
    CmdLCD(CLEAR_LCD);
}

void display_RTC(){
    CmdLCD(CLEAR_LCD);
    while(digitalRead(ENTRY_SW)){

        // Get and display the current time on LCD
	    GetRTCTimeInfo(&hour,&min,&sec);
	    DisplayRTCTime(hour,min,sec);

	    GetRTCDateInfo(&date,&month,&year);
	    DisplayRTCDate(date,month,year);

	    GetRTCDay(&day);
	    DisplayRTCDay(day);
    }
}

u32 check_password(){
    i = 0;
    memset(pass, 0, sizeof(pass));
    BuildCGRAM((u8 *)UserCGRAM[0], 8);
    CmdLCD(GOTO_LINE1_POS0);
    CharLCD(0);
    CmdLCD(GOTO_LINE1_POS0 + 2);
    StrLCD("ENTER PASSWORD");
    CmdLCD(GOTO_LINE2_POS0 + 5);
    while(i<4){
        key = KeyScan();//for key press
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
        while(ColScan()==0);//wait for key release
    }
    if(strcmp(password,pass)==0){
        CmdLCD(CLEAR_LCD);
        BuildCGRAM((u8 *)UserCGRAM[3], 8);
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0 + 2);
        StrLCD("LOGIN SUCCESS   ");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("WAIT 2 SECONDS     ");
        is_login=1;
        delay_ms(2000);
        CmdLCD(CLEAR_LCD);
        // IOCLR0 = 1<<25;
        // IOSET0 = 1<<26;
        return 1;
    }
    else{
        CmdLCD(CLEAR_LCD);
        BuildCGRAM((u8*)UserCGRAM[4],8);
        CmdLCD(GOTO_LINE1_POS0);
        CharLCD(0);
        CmdLCD(GOTO_LINE1_POS0 + 2);
        StrLCD("WRONG PASSWORD  ");
        delay_ms(2000);
        return 0;
    }
}
void change_time(){
    u8 str[4];
    CmdLCD(CLEAR_LCD);
    StrLCD("SET HOURS (0-23)");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    hour=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("SET MIN (0-59)");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    min=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("SET SEC (0-59)");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    sec=my_atoi(str);

    SetRTCTimeInfo(hour,min,sec);
    CmdLCD(CLEAR_LCD);
}
void change_password(){
    i = 0;
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("NEW PASSWORD    ");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("                   ");
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
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("RE-ENTER PASS   ");
        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("                   ");
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
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("PASSWORD CHANGED");
            delay_ms(2000);
            CmdLCD(CLEAR_LCD);
            // IOCLR0 = 1<<25;
            // IOSET0 = 1<<26;
        }
        else{
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("NEW PASS RE PASS");
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("NOT MATCHED    ");
            delay_ms(2000);
            CmdLCD(CLEAR_LCD);
        }
    }
        
}
void change_date(){
    u8 str[4];
    CmdLCD(CLEAR_LCD);
    StrLCD("SET DATE (1-31)");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    date=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("SET MONTH (1-12)");
    for(temp=0;temp<2;temp++){
        key=KeyScan();
        str[temp]=key;
        CmdLCD(GOTO_LINE2_POS0+temp);
        CharLCD(key);
        delay_ms(200);
        while(ColScan()==0);
    }
    str[temp] = '\0'; 
    month=my_atoi(str);

    CmdLCD(CLEAR_LCD);
    StrLCD("SET YEAR");
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

    CmdLCD(CLEAR_LCD);
    StrLCD("0:S 1:M 2:T 3:W");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("4:T 5:F 6:S");
    delay_ms(200);
    key=KeyScan();
    while(ColScan()==0);
    day=key-'0';
    // CharLCD(key);
    delay_ms(500);
    SetRTCDateInfo(date,month,year);
    SetRTCDay(day);
    CmdLCD(CLEAR_LCD);
    // delay_ms(500);
    // CharLCD('A');
    // CmdLCD(CLEAR_LCD);
    StrLCD("DATE UPDATED");

    delay_ms(500);
    CmdLCD(CLEAR_LCD);
    // return;
    
}

void open_menu(){
    int flag;
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("1:CDATE 2:CTIME");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("3:CPWD 4:EXIT");
    
    key=KeyScan();
    
    CmdLCD(CLEAR_LCD);
    while(ColScan()==0);
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
            }else{
                flag=1;
                StrLCD("NOT AUTHORIZED");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("   LOGIN FIRST   ");
                delay_ms(1000);
                CmdLCD(CLEAR_LCD);
                check_password();
            }
            if(flag==1){
                change_password();
            }
            break;  
        default:
            break; 
    }
    
}
void eint0_isr(void) __irq{
    // IOSET1 = 1<<28;
    
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
    // IOCLR1 = 1<<EINT0_STATUS_LED;
    //eint0 isr user activity ends
    //clear EINT0 status in External Interrupt Peripheral 
    EXTINT = 1<<0;
    //clear EINT0 status in VIC peripheral
    VICVectAddr = 0;
    // IOCLR1 = 1<<28;
}

main(){
    init_system();
    delay_ms(10);
    // IODIR0 &= ~((1<<ENTRY_SW) | (1<<EINT_SW));
    IODIR0 &= ~(1<<ENTRY_SW);

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
    // IOSET0 = 1<<25;
    
    while(1){
        do{
            display_title();
            display_RTC();
        }while(digitalRead(ENTRY_SW));
        CmdLCD(CLEAR_LCD);
        //for entry switch
        if(digitalRead(ENTRY_SW)==0){
            u32 count=0,fail=0,status;
            while(count<3){
                status = check_password();
                if(status==1){
                    break;
                }
                CmdLCD(CLEAR_LCD);
                BuildCGRAM((u8*)UserCGRAM[6],8);
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
            }
        }
        /*while(1){
            CmdLCD(CLEAR_LCD);
            display_RTC();
        }*/
    }
}


