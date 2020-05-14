//lab6apс
#include <dos.h>
#include <conio.h>
#include <stdio.h>

#define true 1
#define false 0
#define COUNT 3

int cont = true;
int again = true;
void printKeyCode();

void interrupt(*oldKeyboardInterrupt)(...);
void interrupt newKeyboardnterrupt(...){
    //char keyCode = 0;
    if(inp(0x60) == 0x01) cont = false;
    
    if(inp(0x60) == 0xFE){  // код ошибки, байт не обработан
        again = true;
    } else again = false;
    
printKeyCode();
outp(0x20, 0x20);
//сюрос контрооллера прерывания
}

void setKeyboardInterrupt(){
    _disable();
     oldKeyboardInterrupt = getvect(0x09); // keyboard
     setvect(0x09, newKeyboardnterrupt);
     _enable();
    }
void resetKeyboardInterrupt(){
     _disable();
      setvect(0x09, oldKeyboardInterrupt);
      _enable();
    }
void setIndicator(unsigned char mask);
void blink(void);

int main() {
    setKeyboardInterrupt();
    
    blink();
    while (cont);
    
    resetKeyboardInterrupt();
    return 0;
}

int writeToPort(unsigned char mask){
    for(int i = 0; i <= COUNT; i++){
        while(inp(0x64) & 0x02);
        outp(0x60, mask);
        delay(50);
         if(again == 0) return true;
        }
    return false;
    }

void setIndicator(unsigned char mask){
    again = 1;
    if(!writeToPort(0xED)){ // для управления индикатором
        return;
    }
    again = 1;
    if(!writeToPort(mask)){ // маска
        return;
    }
}

unsigned char mask[] = {
     0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
     0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
     0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
     0x06, 0x06,  0x06, 0x06, 0x06,0
};
int delayTime[] = {50, 200,50,50,50,50,200,200,
   200,50,50,50,50,200,50,50,50,50,200,200,50,
    200,50,200,200,50, 200,50,200,0
};

void blink(){
    
    for(int i = 0; mask[i] != 0; i++){
        setIndicator(mask[i]);
        delay(delayTime[i]);
        setIndicator(0x00);
        delay(100);
    }
}

void printKeyCode(){

    printf("key:%x\n", inp(0x60));
}
