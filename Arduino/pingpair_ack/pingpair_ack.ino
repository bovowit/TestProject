
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// translate
RF24 radio(7, 8);

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };              // Radio pipe addresses for the 2 nodes to communicate.
typedef enum { role_ping_out = 1, role_pong_back } role_e;                 // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back" };  // The debug-friendly names of those roles
role_e role = role_pong_back;                                              // The role of the current running sketch

                                       // data 
byte counter = 1;
byte data[32] = { 0, };
byte rBuff[32] = { 0, };
int sensitivity = 10;
unsigned long synctime = 300000; // 30 msec
unsigned long stime = 0;
unsigned long currenttime = 0;
bool bOneShot = false;

void setup()
{
  Serial.begin(115200);
  printf_begin();
  Serial.print(F("\n\rRF24/examples/pingpair_ack/\n\rROLE: "));
  Serial.println(role_friendly_name[role]);
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Setup and configure rf radio
  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(0, 15);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging

  stime = micros();

}

void loop(void)
{
  if (role == role_ping_out)
  {
    if (radio.available())
    {
      //while (radio.available())
      //{
      radio.read(&rBuff, 32);
      printf("RECV command : %s from master \n\r", (char*)&rBuff);
      if (rBuff[0] == 0x22)
      {
        currenttime = micros();
        radio.writeFast(&currenttime, 32);
        Serial.println("answer sync time ............");
      }
      //}
    }

    int _value = analogRead(A0);
    if (_value > sensitivity && bOneShot == false)
    {
      bOneShot = true;
      stime = micros();
      printf("Detected Impact - %d : time %d \n\r", _value, stime);
    }
    else
    {
      if (micros() - stime > synctime)   // 30 msec 마다 무효화, 노이즈 처리
      {
        stime = 0;
        bOneShot = false;
      }
      return;
    }

    //memcpy(data, stime, sizeof(stime));
    radio.stopListening();                                  // First, stop listening so we can talk.
    if (!radio.writeFast(&stime, 32))// &data, 32))
    {
      Serial.println(F("failed. clear data "));
      memset(&data, 0, sizeof(stime));
    }

  }


  // Pong back role.  Receive each packet, dump it out, and send it back
  if (role == role_pong_back)
  {
    byte pipeNo;
    byte gotByte;                                       // Dump the payloads until we've gotten everything
    while (radio.available(&pipeNo))
    {
      radio.read(&rBuff, 32);
      //radio.writeAckPayload(pipeNo, &rBuff, 1);
    }
    unsigned long impact_time = 0;
    memcpy(&impact_time, rBuff, sizeof(unsigned long));
    if(impact_time > 0)
    {
      Serial.print("[RECV] "); Serial.print(impact_time); Serial.print(" : "); Serial.println((char*)&rBuff);
    }
  }

  // Change roles
  if (Serial.available())
  {
    char c = toupper(Serial.read());
    if (c == 'T' && role == role_pong_back)
    {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));

      role = role_ping_out;                  // Become the primary transmitter (ping out)
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1, pipes[1]);
    }
    else if (c == 'R' && role == role_ping_out)
    {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));

      role = role_pong_back;                // Become the primary receiver (pong back)
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1, pipes[0]);
      radio.startListening();
    }
  }
}
