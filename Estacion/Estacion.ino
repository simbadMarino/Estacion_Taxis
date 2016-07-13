

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <Tone.h>
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
Tone notePlayer[1];
RF24 radio(9,10);   //Uno Estacion
//RF24 radio(5,4);   //Mega Estacion

// sets the role of this unit in hardware.  Connect to GND to be the 'pong' receiver
// Leave open to be the 'ping' transmitter
const int role_pin = 7;

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

// The various roles supported by this sketch
typedef enum { role_sender = 1, role_receiver } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role;

static uint32_t message_count = 0;


char SendPayload[31] = "";
char RecvPayload[31] = "";
unsigned long time1;
unsigned long time2;
boolean flag = 0;
boolean rfDataflag = 0;
boolean flagtoto=0;
String model;
String swVersion = "SW ver: 1.1 Date: 12/07/2016";
int i=0;

void setup(void)
{


  
  role = role_receiver;

  //
  // Print preamble
  //

  Serial.begin(115200);
  printf_begin();

  //
  // Setup and configure rf radio##############################################################################
  //

  radio.begin();
  //Setting Data rate to 2Mbps
  radio.setDataRate(RF24_2MBPS);
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(0,5);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(32);
  radio.setPALevel(RF24_PA_MAX);
 // radio.setChannel(70);
  radio.enableDynamicPayloads();

 

  if ( role == role_sender )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the confeiguration of the rf unit for debugging
  //
  Serial.println("Odologger Station System");
  Serial.println(swVersion);
  Serial.println("");
  Serial.println("");
  radio.printDetails();

  notePlayer[0].begin(5);

  play_OK();
  delay(1000);
  play_wrong();
  radio.setAutoAck(false);

  //###########################################################################################################
}

void loop(void)
{

nRF_receive();


}


void nRF_receive(void) {
  int len = 0;
  if ( radio.available() ) {
      bool done = false;
      while ( !done ) {
        len = radio.getDynamicPayloadSize();
        //Serial.println("Length:");      //DEBUG
        //Serial.println(len);            //DEBUG
        if(len == 26 || len == 27 || len == 28 )   // Check Lenght of string received( 27 = Chofer & Unidad tags OK, 28 = One tag NOTOK, 29 = 2 tags NOTOK)
        {
          radio.setAutoAck(true);
          done = radio.read(&RecvPayload,len);
          
        }
        else
        {
          radio.setAutoAck(false);
          //Serial.println("Length NOT OK");
          done = radio.read(&RecvPayload,len);
          //break;
          
        }
     
        delay(5);
        radio.setAutoAck(false);
      }
  
    RecvPayload[len] = 0; // null terminate string
    

   
   
   if(RecvPayload[1] == 'C' && RecvPayload[6] == 'U' && RecvPayload[11] == 'K' && RecvPayload[18] == 'V' && RecvPayload[22] == 'D')
   {
   
    Serial.println(RecvPayload);
    play_OK();
    for(i=0;i<31;i++)
    {
      RecvPayload[i] = 0;
    }
   }
   else
   {
    Serial.println(RecvPayload);
    play_wrong();
     for(i=0;i<31;i++)
    {
      RecvPayload[i] = 0;
    }
   }
   
   

  }
}



  void play_wrong()
{

  notePlayer[0].play(NOTE_B2);
  delay(300);
  notePlayer[0].stop();
  delay(100);
  notePlayer[0].play(NOTE_B2);
  delay(180);
  notePlayer[0].stop();
  delay(100);
}

void play_OK()
{

    notePlayer[0].play(NOTE_D4);
    delay(200);
    notePlayer[0].play(NOTE_B4);
    delay(180);
    notePlayer[0].stop();
    delay(100);
}


