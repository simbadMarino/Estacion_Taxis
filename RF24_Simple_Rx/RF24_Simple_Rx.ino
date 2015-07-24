/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(5,4);

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
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role;


char SendPayload[31] = "";
char RecvPayload[31] = "";
unsigned long time1;
unsigned long time2;
boolean flag = 0;

void setup(void)
{


  
  role = role_pong_back;

  //
  // Print preamble
  //

  Serial.begin(115200);
  printf_begin();
  printf("\n\rRF24/examples/pingpair/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);

  //
  // Setup and configure rf radio
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
  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  if ( role == role_ping_out )
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
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
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
        if(flag == 0){
        time1 = millis();
        }
        else if(flag == 1){
        time2 =  millis() - time1;
        
        }
        flag = !flag;
        delay(5);
      }
  
    RecvPayload[len] = 0; // null terminate string
    

    Serial.print(RecvPayload);
    Serial.println();
   // printf("DiffTime= %d\n\r",time2);
  //  RecvPayload[0] = 0;  // Clear the buffers
  }
}  
// vim:cin:ai:sts=2 sw=2 ft=cpp
