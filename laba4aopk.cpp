#include <stdio.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>


#define CONTINUE 1
#define nTimes 9999

#define EXIT '0'
#define PLAY_SOUND '1'
#define PRINT_DIVISION_RATIO '2'
#define PRINT_STATUS_WORDS '3'
#define GENERATE_RANDOM_NUMBER '4'



// Массив частот для мелодии
float Sherlock[] = {784.00, 523.25, 523.25, 587.32, 659.26, 587.32,
    523.25, 698.46, 523.25, 523.25, 587.32, 659.26, 659.26, 698.46,
    587.32, 587.32, 659.26, 523.25, 523.25, 587.32, 493.88, 493.88,
    392.00, 523.25, 0 };
//массив пауз
int pause[] = {0, 100, 0, 0, 0, 0,  0, 0,
               100, 0, 0, 40,0, 0, 40, 0,
               0,  40, 0, 0, 40,0, 0,  0};
// Массив длительностей
int del[] = {
        300,   700,   200,   200,   200,   200,   200,
        200,   700,   120,   120,   210,   120,   120,
        210,    120,   120,   210,   120,   120,   210,   120,
        120,   1000 };


void tm_sound(float freq, int time, int pause) {

        int cnt, i;

// Задаем режим канала 2 таймера
        outp(0x43, 0xb6);
// Вычисляем задержку для загрузки в
// регистр счетчика таймера
        cnt = 1193180L / freq;
// Загружаем регистр счетчика таймера - сначала
// младший, затем старший байты
        outp(0x42, cnt & 0x00ff);
        outp(0x42, (cnt &0xff00) >> 8);
// Включаем громкоговоритель. Сигнал от
// канала 2 таймера теперь будет проходить
// на вход громкоговорителя.
        outp(0x61, inp(0x61) | 3);
// Выполняем задержку.
        delay(time);
// Выключаем громкоговоритель.
        outp(0x61, inp(0x61) & 0xfc);
    delay(pause);
}

void printStatusWords(void) {
  unsigned char state, stateWord[8];
  int ports[] = {0x40, 0x41, 0x42};
  //11100010
  //маска выбора порта
  int controlWords[] = {11110010, 11101010, 11100110};
  int i, numb;
  for(numb = 0; numb < 3; numb++) {
    outp(0x43, controlWords[numb]);
    state = inp(ports[numb]);
    for (i = 7; i >= 0; i--) {
      stateWord[i] = state%2 + '0';
      state /= 2;
    }
    stateWord[8] = '\0';
    printf("Channel %d: %s\n", numb, stateWord);
    }

}

void printDivisionRatio(void) {
    int ports[] = {0x40, 0x41, 0x42};
    int controlWords[] = {0, 64, 128};
    unsigned lowDivisionRatio, highDivisionRatio, maxDivisionRatio, temp;
    int channelNum, count;
    for(channelNum = 0; channelNum < 3; channelNum++) {
      maxDivisionRatio = 0;
      temp = 0;
      if (channelNum == 2) {
        outp(0x61, inp(0x61) | 3);
      }
      for (count = 0; count  < nTimes; count++) {

        outp(0x43, controlWords[channelNum]); // select a channel
        lowDivisionRatio = inp(ports[channelNum]);
        highDivisionRatio = inp(ports[channelNum]);

        temp = highDivisionRatio * 256 + lowDivisionRatio;

        if(temp > maxDivisionRatio) {
          maxDivisionRatio = temp;
        }
        }
        if(channelNum == 2) {
          outp(0x61, inp(0x61) & 0xfc);
        }
        printf("Channel %d - %4X\n", channelNum, maxDivisionRatio);
    }
}

void randomSet(int maxValue); //, int parity);
int randomGet(void);

void generateRandom(void) {
  int userInput;
  printf("Enter the maximum value of the number\n");
  scanf("%d", &userInput);

  randomSet(userInput);

  printf("Press any key to get result..\n");
  getch();

  printf("Number: %u\n", randomGet());
}

void randomSet(int bound) {
          outp(0x43, 0xb4);
  // Загружаем регистр счетчика таймера - сначала
  // младший, затем старший байты
          outp(0x42, bound & 0x00ff);
          outp(0x42, (bound &0xff00) >> 8);
  // Разрешаем работу канала
          outp(0x61, inp(0x61) | 1);
  }


int randomGet(void) {
       int i;
  // Выдаем команду CLC для фиксирования
  // текущего значения регистра канала 2 таймера
          outp(0x43, 0x86);
  // Вводим младший и старший байты счетчика
          i = inp(0x42);
          i = (inp(0x42) << 8) + i;
          return(i);
  }



//void sound(int, int);

    int main() {
      char userInput;

      while(CONTINUE) {
          printf("Select from the above\n");
          printf("1. Play sound\n");
          printf("2. Print division ratio\n");
          printf("3. Print status words\n");
          printf("4. Get random number\n");
          printf("0. Exit\n");

          userInput = getch();

          switch(userInput) {
            case PLAY_SOUND:
                  int i;
                  for(i=0 ;Sherlock[i] != 0 ;i++)
                  tm_sound(Sherlock[i], del[i], pause[i]);
              break;
            case PRINT_DIVISION_RATIO:
              printDivisionRatio();
              break;
            case PRINT_STATUS_WORDS:
              printf("Status word\n");
              printStatusWords();
              break;
            case GENERATE_RANDOM_NUMBER:
              generateRandom();
              break;
            case EXIT:
              return 0;
            }
        }
    }
  //int i;
  //for(i=0 ;Sherlock[i] != 0 ;i++)
    //tm_sound(Sherlock[i], del[i], pause[i]);
