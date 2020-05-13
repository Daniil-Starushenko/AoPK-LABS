//lab5AoPK
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define Continue 1
#define MAX_SIZE 9
#define MAX_COUNT 10000

struct Time { //структура, с помощью котрой мы будем переводить в инт
  char hour;
  char minutes;
  char seconds;
};

void getRtcValue();
void setRtcValue();
void setAlarm();
void resetAlarm();
void delay();

void interrupt(*alarmInterrupt) (...); // для запоминания старого обработчика
void interrupt newAlarmInterrupt(...) { // новый обработчик 

  puts("The right time is now");


  outp(0xA1, inp(0xA1) | 0x01);// берем маску прерываний засовыывем в А1

  outp(0x70, 0x0B); // выбор регистра 0B через порт 70
  outp(0x71, inp(0x71) & 0xDF);// запрет прерывания от будильника

  alarmInterrupt();
  resetAlarm();
}


unsigned int delayTime;

void interrupt(*RTCInterrupt) (...); // место для прерывания старого
void interrupt newRTCInterrupt(...) { // новое прерывание(обработчик)
  delayTime--;
  RTCInterrupt();
}



int main() {

  char userInput;
 // выбор того, что мы хоотим сделать
  while(Continue) {
    rewind(stdin);
    printf("Select from the above\n");
    printf("1. Get time\n");
    printf("2. Set time\n");
    printf("3. Delay\n");
    printf("4. Set alarm\n");
    printf("5. Exit\n");

    userInput = getch();
    rewind(stdin);
    switch(userInput) {
      case '1':
        getRtcValue();
        break;
      case '2':
        setRtcValue();
        break;
      case '3':
        delay();
        break;
      case '4':
        setAlarm();
        break;
      case '5':
        return 0;
    }
  }
}

int bcdToInt(char* source) { // перевод из двоично-десятичног в int
  return( ((*source) & 0x0f) + 10 * (((*source) & 0xf0) >> 4));
}


struct Time getNewTime() { // функция, в которой мы записываем значения времени для будильника
  struct Time temp;
  char *newTime;
  newTime = (char*)malloc(MAX_SIZE);
  printf("Enter time (02:45:08) << ");
  scanf("%8s", newTime);

  temp.hour = (char)((newTime[0]- 0x30) * 0x10 + newTime[1] - 0x30);
  temp.minutes = (char)((newTime[3]- 0x30) * 0x10 + newTime[4] - 0x30);
  temp.seconds = (char)((newTime[6]- 0x30) * 0x10 + newTime[7] - 0x30);

  free(newTime);
  return temp;
}


void setAlarm() {
  struct Time alarmTime = getNewTime(); // наше время, которое мы хотим поставить

  outp(0x70, 0x05); // регистр часа
  outp(0x71, alarmTime.hour); // установили значение

  outp(0x70, 0x03); // регистр минуты
  outp(0x71, alarmTime.minutes); // установили значение

  outp(0x70, 0x01); // регистр секунды
  outp(0x71, alarmTime.seconds); //установили значение
    
  outp(0xA1, inp(0xA1) | 0x00);
    
  _disable();
  alarmInterrupt = getvect(0x4A); // сохранили значение старого обработчика
  setvect(0x4A, newAlarmInterrupt); // используем свой обработчик
  _enable();

  outp(0x70, 0x0B);
  outp(0x71, (inp(0x71) | 0x20));

  outp(0xA1, (inp(0xA0) & 0xFE));

}

void resetAlarm() {

  _disable();
  setvect(0x4A, alarmInterrupt); // устанавливаем стандартное прерывание
  _enable();

  outp(0x70, 0x05);
  outp(0x71, 0);
  outp(0x70, 0x03);
  outp(0x71, 0);
  outp(0x70, 0x01);
  outp(0x71, 0);
}

void setRtcValue() {
  struct Time newTime = getNewTime();
  disable();

  unsigned int count = 0;

  while(count++ != MAX_COUNT) {
    outp(0x70, 0x0A);
    if(!(inp(0x71) & 0x80)) {
      break;
    }
  }

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) | 0x80);

  outp(0x70, 0x04);
  outp(0x71, newTime.hour);

  outp(0x70, 0x02);
  outp(0x71,  newTime.minutes);

  outp(0x70, 0x00);
  outp(0x71, newTime.seconds);

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) & 0x7F);

  enable();
}

void getRtcValue() { // получить текущее время
  struct Time currentTime;

  outp(0x70, 0x4); // бит часа
  currentTime.hour = inp(0x71); // считываем значение

  outp(0x70, 0x2);// бит минуты
  currentTime.minutes = inp(0x71); //считаываем значение

  outp(0x70, 0x0);// бит секунды
  currentTime.seconds = inp(0x71); //считываем значение

  printf("RTC time >> %d:%d:%d\n", bcdToInt(&currentTime.hour), bcdToInt(&currentTime.minutes), bcdToInt(&currentTime.seconds)); // перевод для возможности вывести

}

void delay() { // задержка
  rewind(stdin);
  printf("Enter delay time in ms: ");

  scanf("%d", &delayTime);

  _disable();
  RTCInterrupt = getvect(0x70); // старое прерывание
  setvect(0x70, newRTCInterrupt);
  _enable();

  printf("Program has stopped for %dms\n", delayTime);

  outp(0xA1, inp(0xA1) & 0xFE); // размаскировать прерыавние от RTC(11111110)

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) | 0x40);//разрешение периодических прерывания

  while(delayTime != 0);

  outp(0x70, 0x0B);
  outp(0x71, inp(0x71) & 0xBF); //запрет периодических прерываний(10111111)

  outp(0xA1, inp(0xA1) | 0x01);//маскировать прерывание от rtc(00000001)

  _disable();
  setvect(0x70, RTCInterrupt); // восстановление прерывания
  _enable();

  printf("You can continue working\n");

}
