#include <dos.h>

struct VIDEO
{
    unsigned char symbol;
    unsigned char attribute;
};

int attribute = 0x00;

void get_reg();
void print(int offset, int);

//IRQ0-7
void interrupt(*int8) (...);
void interrupt(*int9) (...);
void interrupt(*intA) (...);
void interrupt(*intB) (...);
void interrupt(*intC) (...);
void interrupt(*intD) (...);
void interrupt(*intE) (...);
void interrupt(*intF) (...);
//IRQ8-15
void interrupt(*int70) (...);
void interrupt(*int71) (...);
void interrupt(*int72) (...);
void interrupt(*int73) (...);
void interrupt(*int74) (...);
void interrupt(*int75) (...);
void interrupt(*int76) (...);
void interrupt(*int77) (...);

// new interrupt handlers
void interrupt  int60(...) { get_reg(); int8(); }
void interrupt  int61(...) { attribute++;  get_reg(); int9(); }
void interrupt  int62(...) { get_reg(); intA(); }
void interrupt  int63(...) { get_reg(); intB(); }
void interrupt  int64(...) { get_reg(); intC(); }
void interrupt  int65(...) { get_reg(); intD(); }
void interrupt  int66(...) { get_reg(); intE(); }
void interrupt  int67(...) { get_reg(); intF(); }
void interrupt  int68(...) { get_reg(); int70(); }
void interrupt  int69(...) { get_reg(); int71(); }
void interrupt  int6A(...) { get_reg(); int72(); }
void interrupt  int6B(...) { get_reg(); int73(); }
void interrupt  int6C(...) { get_reg(); int74(); }
void interrupt  int6D(...) { get_reg(); int75(); }
void interrupt  int6E(...) { get_reg(); int76(); }
void interrupt  int6F(...) { get_reg(); int77(); }

// fill videomemory pointer
void print(int offset, int val)
{
    char temp;
    int i;
    VIDEO far* screen = (VIDEO far*)MK_FP(0xB800, 0);
    screen += 15*80 + 40 + offset;
    for (i = 7; i >= 0; i--)
    {
        temp = val % 2;
        val /= 2;
        screen->symbol = temp + '0';
        screen->attribute = attribute;
        screen++;
    }
}

// get data from registers
void get_reg()
{
    //MASTER
    int reg;

    reg = inp(0x21);    //masks
    print(0, reg);

    outp(0x20, 0x0B);
    reg = inp(0x20);
    print(9, reg);

    outp(0x20, 0x0A);
    reg = inp(0x20);
    print(18, reg);
    //SLAVE
    reg = inp(0xA1);
    print(80, reg);

    outp(0xA0, 0x0B);
    reg = inp(0xA0);
    print(80 + 9, reg);

    outp(0xA0, 0x0A);
    reg = inp(0xA0);
    print(80 + 18, reg);
}

void init()
{
    //IRQ0-7
    
    int8 = getvect(0x08);
    int9 = getvect(0x09);
    intA = getvect(0x0A);
    intB = getvect(0x0B);
    intC = getvect(0x0C);
    intD = getvect(0x0D);
    intE = getvect(0x0E);
    intF = getvect(0x0F);

    //IRQ8-15
    int70 = getvect(0x70);
    int71 = getvect(0x71);
    int72 = getvect(0x72);
    int73 = getvect(0x73);
    int74 = getvect(0x74);
    int75 = getvect(0x75);
    int76 = getvect(0x76);
    int77 = getvect(0x77);


    
    setvect(0xB8, int60);
    setvect(0xB9, int61);
    setvect(0xBA, int62);
    setvect(0xBB, int63);
    setvect(0xBC, int64);
    setvect(0xBD, int65);
    setvect(0xBE, int66);
    setvect(0xBF, int67);

    setvect(0x08, int68);
    setvect(0x09, int69);
    setvect(0x0A, int6A);
    setvect(0x0B, int6B);
    setvect(0x0C, int6C);
    setvect(0x0D, int6D);
    setvect(0x0E, int6E);
    setvect(0x0F, int6F);

//для функций инициализации
    _disable(); // disable interrupts handling (cli)



    /* MASTER */
    outp(0x20, 0x11);        //ICW1
    outp(0x21, 0xb8);    //ICW2
    outp(0x21, 0x04);        //ICW3
    outp(0x21, 0x01);        //ICW4

    /* SLAVE */
    outp(0xA0, 0x11);        //ICW1
    outp(0xA1, 0x08);   //ICW2
    outp(0xA1, 0x02);        //ICW3
    outp(0xA1, 0x01);        //ICW4

    _enable(); // enable interrupt handling (sti)
}

int main()
{
    unsigned far* fp;
    init();
    // removing psp
    FP_SEG(fp) = _psp; // segment
    FP_OFF(fp) = 0x2c; // offset
    _dos_freemem(*fp);

    _dos_keep(0, (_DS - _CS) + (_SP / 16) + 1);
    return 0;
}

