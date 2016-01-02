/*
  Lights.
*/

// FFT
#define LOG_OUT 1 // use the log output function
#define FFT_N 16 // set to 16 point fft

#include "Arduino.h"
#include <FFT.h>

/*
  Declarations
*/

#define NUM_LEDS 4
#define MIC_PIN A5

int pins[NUM_LEDS] = { 2,3,6,7 };

/*
  The setup function runs once when you press reset or power the board
  */
void setup() {
  // initialize digital pin 13 as an output.

  Serial.begin(9600);

  // Set up LEDs
  for (int i=0; i < NUM_LEDS; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // pinMode(MIC_PIN, INPUT);

  // FFT
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

/*
  The loop function runs over and over again forever
  */
void loop() {
  // FFT
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FFT_N * 2; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
    // Serial.write(255); // send a start byte
    // Serial.write(fft_log_out, 128); // send out the data
    for (int i = 0; i < FFT_N; i++) {
      Serial.print(fft_log_out[i]);
      Serial.print(" : ");
    }
    Serial.println("");

    for (int i = 0; i < NUM_LEDS; i++) {
      analogWrite(pins[i], fft_log_out[4 + i]);
    }
  }

  // int vol = analogRead(MIC_PIN);
  // Serial.println(vol);

  // for (int i=0; i < NUM_LEDS; i++) {
  //   Serial.print(pins[i]);
  //   digitalWrite(pins[i], HIGH);   // turn the LED on (HIGH is the voltage level)
  //   delay(1000);              // wait for a second
  //   digitalWrite(pins[i], LOW);    // turn the LED off by making the voltage LOW
  //   //delay(1000);              // wait for a second
  // }
}
