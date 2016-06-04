

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
RF24 radio(8,7);

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
String swVersion = "SW ver: 1.0 Date: 04/06/2016";


void setup(void)
{

//play_OK();
  
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

 // play_OK();


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
        done = radio.read(&RecvPayload,len);
     
        delay(5);
      }
  
    RecvPayload[len] = 0; // null terminate string
    

   Serial.println(RecvPayload);
   
   if(RecvPayload[1] == 'C' && RecvPayload[6] == 'U')
   {
    play_OK();
   }
   else
   {
    play_wrong();
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


