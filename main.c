#include <math.h>
#include "configuration.h"

#define agent_id1  14
#define agent_id2  13
#define agent_id3  12

const char posArray1[8] = {0x11, 0x01, 0x03, 0x02, 0x06, 0x04, 0x14, 0x10};
const char posArray2[8] = {0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08};
const char posArray3[8] = {0x90, 0x10, 0x30, 0x20, 0x60, 0x40, 0xC0, 0x80};

const char motor1Array[15] = {15, 135, 5, 67, 105, 130, 148, 161, 172, 180, 187, 192, 197, 201, 205};
//incremental  = [2 1 0 0 0 0 0 0 0 0 0 0 0 0 0];

const char motor2_1Array[15] = {0xa2, 0xd1, 0xe0, 0xe8, 0xed, 0xf0, 0xf2, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf8, 0xf9, 0xf9};
const char motor2_2Array[15] = {0x3f, 0x1f, 0xbf, 0x8f, 0x3f, 0x5f, 0x9a, 0x47, 0x94, 0x9f, 0x79, 0x2f, 0xc9, 0x4d, 0xbf};

const char motor3Array[15] = {240, 120, 250, 187, 150, 125, 107, 93, 83, 75, 68, 62, 58, 54, 50};
//incremental  = [2 1 0 0 0 0 0 0 0 0 0 0 0 0 0];

//char  counter = 0;
char counter1 = 0;
char counter2 = 0;
char counter3 = 0;

char motor1 = 0;
char motor2 = 0;
char motor3 = 0;
char tmp1   = 0;
char tmp2   = 0;
char tmp3   = 0;

char flag1 = 0;
char flag2 = 0;
char flag3 = 0;
char flag4 = 0;
char motor1run = 0;
char motor2run = 0;
char motor3run = 0;

char incremental1 = 0;
char incremental3 = 0;
int direction1 = 0;
int direction2 = 0;
int direction3 = 0;
int loop1 = 0;
int loop3 = 0;


char counter_uart = 0;

char uart_data[15] = {0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B,0x4B};
char tmp_data=0;


void handle_counter1(void);
void handle_counter2(void);
void handle_counter3(void);
void handle_uart_data(void);
void handle_uart_buffer(char);

void interrupt global_interrupt(){          //single interrupt vector to handle all of ISR's

    //GIE = 0 ;//Global interrupt disable in ISR
    CREN = 1; // seri port kapand?ysa acal?m
    if(RCIF){
       if(FERR == 0 && OERR == 0){
            tmp_data = RCREG;
            handle_uart_buffer(tmp_data);
        }
        else{
             CREN = 0;
            tmp_data = RCREG;  // bos okuma yapal?m
             counter_uart = 0;
        }
        //GIE = 1;
        return;
    }
   
    //Timer0 interrupt
    if(T0IF){
        T0IF = 0; //clear interrupt flag
        flag1 = 1;
        //GIE = 1 ;//Global interrupt enable in ISR
        return;
     }
   
      

     //Timer1 interrupt
    if(TMR1IF){
        TMR1IF = 0; //clear interrupt flag
        flag2 = 1;
        // GIE = 1 ;//Global interrupt enable in ISR
        return;
    }

    //Timer2 interrupt
    if(TMR2IF){
       TMR2IF = 0;
       flag3 = 1;
       //GIE = 1 ;//Global interrupt enable in ISR
       return;
     }
}

int main(void){
     //Internal RC osc operating at 8Mhz
      OSCCON  = 0x00;
      OSCCON |= 0b01110001;
      OSCTUNE = 0x00; //8Mhz calibrated frequency
      //Internal RC osc operating at 8MHz

      //Timer0 clock=Fosc/4, prescaler = 1/32;
      OPTION_REG &= 0b11010000;
      OPTION_REG |= 0b00000100;
      TMR0 = 0x00;
      T0IE = 1;
      //Timer0 clock=Fosc/4, prescaler = 1/32;

      //Timer1 clock=Fosc/4, prescaler = 1/1;
      TMR1GE = 0;
      T1CONbits.T1CKPS0 = 0;
      T1CONbits.T1CKPS1 = 0;
      T1OSCEN = 0;
      TMR1CS = 0;
      TMR1ON = 1;
      TMR1H = 0x00;
      TMR1H = 0x00;
      TMR1IE = 1;
      //Timer1 clock=Fosc/4, prescaler = 1/1;

   
      //Timer2 clock=Fosc/4, prescaler = 1/16 , postscaler = 1/2;
        T2CONbits.T2CKPS0= 1;
        T2CONbits.T2CKPS1= 1;
        T2CONbits.TOUTPS0 = 1;
        T2CONbits.TOUTPS1 = 0;
        T2CONbits.TOUTPS2 = 0;
        T2CONbits.TOUTPS3 = 0;
        PR2    = 0xFF;
        TMR2ON = 1;
        TMR2IE = 1;
      //Timer2 clock=Fosc/4, prescaler = 1/16 , postscaler = 1/16;

      //UART 9600 8 bit asenkron konfig
      BRG16 = 0;
      BRGH = 0;
      SPBRGH = 0;
      SPBRG = 0x0C; //9600
      SYNC = 0;
      SPEN = 1;
      RCIE = 1;
      CREN = 1;
      //UART 9600 8 bit asenkron konfig


      //PORT lar? konfig�re edelim
      TRISA = 0x00;
      TRISB = 0b00100000;
      TRISC = 0x00;
      ANSEL = 0x00;
      ANSELH = 0x00;
      //PORT lar? konfig�re edelim

      PORTA = 0x00;
      PORTB = 0x00;
      PORTC = 0x00;
      PEIE = 1;
      GIE  = 1;

    while(1){
        if(flag4){
            handle_uart_data();
            flag4 = 0;
        }

          if(flag1){
            handle_counter1();
            flag1 = 0;
        }
          if(flag2){
            handle_counter2();
            flag2 = 0;
        }
          if(flag3){
            handle_counter3();
            flag3 = 0;
        }

    }
}

void handle_counter2(void){
  char temp;
  counter2 += direction2;
  counter2 &= 0x07;
  TMR1H = motor2_1Array[motor2];
  TMR1L = motor2_2Array[motor2];
  temp = (posArray2[counter2] & motor2run) | (posArray3[counter3] & motor3run);
  PORTC = temp;
}

void handle_counter1(void){
    char temp;
    loop1 -= 1;
    if(loop1 < 0){
      TMR0 = motor1Array[motor1];
      loop1 = incremental1;
      counter1 += direction1;
      counter1 &= 0x07;
      temp = posArray1[counter1] & motor1run;
      PORTA = temp;
    }
    else{
    TMR0 = 0;
    }
}

void handle_counter3(void){
    char temp;
    loop3 -= 1;
    if(loop3 < 0){
      PR2 = motor3Array[motor3];
      loop3 = incremental3;
      counter3 += direction3;
      counter3 &= 0x07;
      temp = (posArray2[counter2] & motor2run) | (posArray3[counter3] & motor3run);
      PORTC = temp;
    }
    else{
    PR2 = 0xFF;
    }
    
    
}

void handle_uart_data(void){

   tmp1 =  uart_data[agent_id1];
   if(tmp1 == 75)
      motor1run = 0x00;
   else
      motor1run = 0xFF;

   if(tmp1 >=60){
      tmp1 = tmp1 - 15;
      direction1 = -1;
    }
   else
     direction1 = 1;

   tmp1 = tmp1 - 45;
   if(tmp1 == 0)
     incremental1 = 2;
   else if(tmp1 == 1)
     incremental1 = 1;
   else
     incremental1 = 0;

   motor1 = tmp1;

   tmp2 =  uart_data[agent_id2];
   if(tmp2 == 75)
      motor2run = 0x00;
   else
      motor2run = 0xFF;

   if(tmp2 >=60){
     tmp2 = tmp2 - 15;
     direction2 = -1;
   }
   else
     direction2 = 1;

   tmp2 = tmp2 - 45;
   motor2 =  tmp2;


   tmp3 =  uart_data[agent_id3];
  if(tmp3 == 75)
      motor3run = 0x00;
  else
      motor3run = 0xFF;

  if(tmp3 >=60){
    tmp3 = tmp3 - 15;
    direction3 = -1;
  }
  else
    direction3 = 1;

   tmp3 = tmp3 - 45;
   if(tmp3 == 0)
     incremental3 = 2;
   else if(tmp3 == 1)
     incremental3 = 1;
   else
     incremental3 = 0;

   motor3 = tmp3;
}

 void handle_uart_buffer(char data){
           if(data == 0x55){
                counter_uart = 0;
                 return;
            }

                uart_data[counter_uart] = data;
                counter_uart += 1;
                if(counter_uart == 15){
                    counter_uart = 0;
                    flag4 = 1;
                }
                return;
  }