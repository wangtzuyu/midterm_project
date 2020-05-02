#include "mbed.h"
#include "math.h"
#include "mbed_events.h"
#include "DA7212.h"
#include <cmath>

/*
#include "uLCD_4DGL.h"
uLCD_4DGL uLCD(D1, D0, D2);
*/

Ticker time_up;
//DigitalOut led1(LED1);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
InterruptIn btn(SW2);
EventQueue printfQueue;
EventQueue eventQueue;
void acc(); 
int mode=1;
int song=0;

void blink_led1() {
  led1 = !led1;
  led2 = 1;
  led3 = 1;
}

void blink_led2() {
  led1 = 1;
  led2 = !led2;
  led3 = 1;
}

void blink_led3() {
  led1 = 1;
  led2 = 1;
  led3 = !led3;
}

void mode_selection() {
  printf("mode=");
  printf("%i\r\n",mode);
  //printf("%1.4f\r\n",t[0]);
  // using DNN to choose mode
}
void song_selection() {
   printf("song=");
   
   switch(mode){
      case 0: 
         if(song==0){
            song=2;
         }
         else {
            song=song-1;
         }
      break;
      case 1: 
         if(song==2){
            song=0;
         }
         else {
            song=song+1;
         }
      break;
      case 2: //
      break;
   }
   printf("%i\r\n",song);
}

void playing() {
   printf("playing\r\n");
   switch(song){
      case 0://song0
      break; 
      case 1://song1
      break;
      case 2://song2
      break;
   }
}

int state=0;
void btn_fall_irq() {
   state=state+1;
   state=state%3;
   if(state==1){
      time_up.attach( &blink_led1, 0.2 );
      printfQueue.call(&mode_selection);
   }
   if(state==2){
      time_up.attach( &blink_led2, 0.2 );
      printfQueue.call(&song_selection);
   }
   if(state==0){
      time_up.attach( &blink_led3, 0.2 );
      printfQueue.call(&playing);
   }
}

#include "fsl_port.h"
#include "fsl_gpio.h"
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1

// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);
/*
void acc(){
   pc.baud(115200);
   uint8_t who_am_i, data[2], res[6];
   int16_t acc16;
   float t[3];
   // Enable the FXOS8700Q
   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);
   // Get the slave address
   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);
   //pc.printf("Here is %x\r\n", who_am_i);
   float time=0.0;
   while (true) {
      FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);
      acc16 = (res[0] << 6) | (res[1] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[0] = ((float)acc16) / 4096.0f;
      acc16 = (res[2] << 6) | (res[3] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[1] = ((float)acc16) / 4096.0f;
      acc16 = (res[4] << 6) | (res[5] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[2] = ((float)acc16) / 4096.0f;
      
         
         printf("%1.4f\r\n",t[0]);
         printf("%1.4f\r\n",t[1]);
         printf("%1.4f\r\n",t[2]);
         
         float tilt;
         float SQRTxxyy= sqrt(t[0]*t[0]+t[1]*t[1]) ;
         if(atan(t[2]/SQRTxxyy)>(3.1415/4)){
            tilt=0;
         }
         else{
            tilt=1;
         }
      printf("%1.2f\r\n",tilt);      
      //printf("%1.2f\n",time);
      wait(1);
      time=time+1;
      if(time>10.0)
      break;
      
      
   }   
}
*/
int main() {
  // low priority thread for calling printf()
  Thread printfThread(osPriorityLow);
  printfThread.start(callback(&printfQueue, &EventQueue::dispatch_forever));
  // normal priority thread for other events
  Thread eventThread(osPriorityNormal);
  eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
  // call blink_led2 every second, automatically defering to the eventThread
  //Ticker ledTicker;
  //ledTicker.attach(eventQueue.event(&blink_led2), 1.0f);
  // button fall still runs in the ISR
   led2 =1;
  btn.fall(&btn_fall_irq);
  while (1) {wait(1);}
}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}


void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}
