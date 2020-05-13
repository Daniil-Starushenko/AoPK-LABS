//lab5
#include <dos.h>
#include <stdio.h>
#include <conio.h>


void reset_alarm();

void interrupt(*oldAlarmHandler) (...);
void interrupt newAlarmHandler(...) {

    puts("working alarm");

    outp(0xA1, inp(0xA1) | 0x01);

    outp(0x70, 0xB);//запретить прерывания от будильника
    outp(0x71, inp(0x71) & 0xDF);// (11011111)

        oldAlarmHandler();

    reset_alarm();
}

unsigned int sleep_duration;

void interrupt(*oldRTCHandler) (...);
void interrupt newRTCHandler(...) {

    sleep_duration--;
    oldRTCHandler();
}


int BCD_to_decimal(int number)
{
    return ((number / 16 * 10) + (number % 16));
}

int decimal_to_BCD(int number)
{
    return ((number / 10 * 16) + (number % 10));
}

void get_time()
{

    outp(0x70, 0x04); //часы
    printf("%02d:", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x02); //минуты
    printf("%02d:", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x00); //секунды
    printf("%02d\n\n", BCD_to_decimal(inp(0x71)));
}

void get_date()

    char day_of_week[7][10] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

    outp(0x70, 0x07); //день
    printf("%02d.", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x08); //месяц
    printf("%02d.", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x09); //год
    printf("%02d\n", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x06); //день недели
    printf("%s\n", day_of_week[BCD_to_decimal(inp(0x71)) - 1]);
}

void get_alarm_time() {
    outp(0x70, 0x05); //часы
    printf("Alarm:\n%02d:", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x03); //минуты
    printf("%02d:", BCD_to_decimal(inp(0x71)));
    outp(0x70, 0x01); //секунды
    printf("%02d\n\n", BCD_to_decimal(inp(0x71)));
}

void set_time(int flag)
{
    int day;
    int month;
    int year;
    int week_day;
    int hours;
    int minutes;
    int seconds;


    do
    {
        rewind(stdin);
        printf("hours: ");
        scanf("%i", &hours);
    } while (hours < 0 || hours > 23);
    hours = decimal_to_BCD(hours);

    do
    {
        rewind(stdin);
        printf("minute: ");
        scanf("%i", &minutes);
    } while (minutes < 0 || minutes > 59);
    minutes = decimal_to_BCD(minutes);

    do
    {
        rewind(stdin);
        printf("second: ");
        scanf("%i", &seconds);
    } while (seconds < 0 || seconds > 59);
    seconds = decimal_to_BCD(seconds);



    if (flag)
    {

        do
        {
            rewind(stdin);
            printf("DAY OF MONTH: ");
            scanf("%i", &day);
        } while (day < 1 || day > 31);
        day = decimal_to_BCD(day);

        do
        {
            rewind(stdin);
            printf("MONTH: ");
            scanf("%i", &month); пользователя)
        } while (month < 1 || month > 12);
        month = decimal_to_BCD(month);

        do
        {
            rewind(stdin);
            printf("YEAR: ");
            scanf("%i", &year);
        } while (year < 0 || year > 99);
        year = decimal_to_BCD(year);

        do
        {
            rewind(stdin);
            printf("DAY OF WEEK: ");
            scanf("%i", &week_day); пользователя)
        } while (week_day < 1 || week_day > 7);
        week_day = decimal_to_BCD(week_day);


        do
        {
            outp(0x70, 0x0A);
        } while (inp(0x71) & 0x80);//7 бит A=0-готов к записи/чтению (& 10000000)


        outp(0x70, 0x0B);
        outp(0x71, inp(0x71) | 0x80); //7 бит B=0-нормальный ход (| 10000000)-остановка часов,затрагивает только 1 бит

        outp(0x70, 0x07);
        outp(0x71, day);
        outp(0x70, 0x08);
        outp(0x71, month);
        outp(0x70, 0x09);
        outp(0x71, year);
        outp(0x70, 0x06);
        outp(0x71, week_day);
        outp(0x70, 0x04);
        outp(0x71, hours);
        outp(0x70, 0x02);
        outp(0x71, minutes);
        outp(0x70, 0x00);
        outp(0x71, seconds);

        outp(0x70, 0x0B);
        outp(0x71, inp(0x71) & 0x7F); //7 бит B=1-остановка часов (& 01111111)-нормальный ход,затрагивает только 1 бит

    }
    else
    {

        outp(0x70, 0x05);
        outp(0x71, hours);
        outp(0x70, 0x03);
        outp(0x71, minutes);
        outp(0x70, 0x01);
        outp(0x71, seconds);


        _disable();
        oldAlarmHandler = getvect(0x4A);
        setvect(0x4A, newAlarmHandler);
        _enable();

        outp(0x70, 0x0B);
        outp(0x71, (inp(0x71) | 0x20)); // разрешить прерывание для будильника(00100000)

        outp(0xA1, (inp(0xA0) & 0xFE));//размаскировать прерывания от RTC(11111110)

    }
}

void reset_alarm() {
    if (oldAlarmHandler == NULL)//не установлен
    {
        return;
    }

    _disable();
    setvect(0x4A, oldAlarmHandler);
    _enable();

    outp(0x70, 0x05);
    outp(0x71, 0);
    outp(0x70, 0x03);
    outp(0x71, 0);
    outp(0x70, 0x01);
    outp(0x71, 0);
}

void delay()
{

    rewind(stdin);
    printf("Input delay in ms: ");

    scanf("%d", &sleep_duration);

    _disable();
    oldRTCHandler = getvect(0x70);
    setvect(0x70, newRTCHandler);
    _enable();

    printf("stop\n");


    outp(0xA1, inp(0xA1) & 0xFE);//размаскировать прерывания от RTC(11111110)

    outp(0x70, 0x0B);
    outp(0x71, inp(0x71) | 0x40);//разрешение периодических прерываний(01000000)


    while (sleep_duration > 0);

    outp(0x70, 0x0B);
    outp(0x71, inp(0x71) & 0xBF);//запрещение периодических прерываний(10111111)

    outp(0xA1, inp(0xA1) | 0x01);//маскировать прерывания от RTC(00000001)

    _disable();
    setvect(0x70, oldRTCHandler);
    _enable();

    printf("start\n");

    return;
}

int main()
{
    clrscr();
    char choice;
    do
    {
        rewind(stdin);

        get_date();
        get_time();
        get_alarm_time();

        printf("1. Set Time\n");
        printf("2. Sleep (in ms)\n");
        printf("3. Set Alarm\n");
        printf("4. show time\n");
        printf("0. Exit\n\n");

        choice = getch();
        rewind(stdin);
        clrscr();

        switch (choice)
        {
        case '1':
            set_time(1);//для RTC
            break;

        case '2':
            delay();
            break;

        case '3':
            set_time(0);//для будильника
            break;
        case '4':
            break;

        }
    } while (choice != '0');

    return 0;
}
