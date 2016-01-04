/*
  Lights.
*/

// FFT
#define LOG_OUT 1 // Use the log output function.
#define FFT_N 16 // Set to 16 point FFT.

#include "Arduino.h"
#include <FFT.h>

/*
  Declarations
  */

#define NUM_LEDS 4
#define MIC_PIN A5

#define MOVING_AVG_ORDER 8

int pins[NUM_LEDS] = { 5, 6, 9, 10 };

uint8_t fft_buffer[MOVING_AVG_ORDER][FFT_N/2];
int current_index = 0;
float moving_avgs[FFT_N/2]; // The moving averages of the FFT spectrum.
float lows[FFT_N/2]; // The lows of the last samples for all bins.
float highs[FFT_N/2]; // The highs of the last samples for all bins.

/*
  The setup function runs once when you press reset or power the board.
  */
void setup() {
  // Initialize digital pin 13 as an output.

  Serial.begin(9600);

  // Set up LEDs.
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // Set array holding moving averages to contain arrays of zeros.
  memset(fft_buffer, 0, sizeof(fft_buffer));

  // FFT
  TIMSK0 = 0; // Turn off timer0 for lower jitter.
  ADCSRA = 0xe5; // Set the adc to free running mode.
  ADMUX = 0x40; // Use adc0.
  DIDR0 = 0x01; // Turn off the digital input for adc0.
}

/*
  The loop function runs over and over again forever.
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

    // Calculate lows and highs.

    // For each bin.
    for (int i = 0; i < FFT_N/2; i++) {
      lows[i] = fft_log_out[i];
      highs[i] = fft_log_out[i];

      for (int j = 0; j < MOVING_AVG_ORDER; j++) {
        lows[i] = min(lows[i], fft_buffer[j][i]);
        highs[i] = max(highs[i], fft_buffer[j][i]);
      }
    }

    // Calculate moving averages.

    float b = 1 / (MOVING_AVG_ORDER + 1.0); // Filter coefficient.

    // For each bin.
    for (int i = 0; i < FFT_N/2; i++) {
      // Calculate output.
      moving_avgs[i] = b * fft_log_out[i];

      for (int j = 0; j < MOVING_AVG_ORDER; j++) {
        moving_avgs[i] += b * fft_buffer[j][i];
      }

      // Add new FFT value to buffer.
      fft_buffer[current_index][i] = fft_log_out[i];
    }

    current_index = (current_index + 1) % MOVING_AVG_ORDER; // Update the index.

    // Update LEDs.

    for (int i = 0; i < NUM_LEDS; i++) {
      // Downsample spectrum output to 4 bins.
      float value = fft_log_out[i*2];
      float avg = moving_avgs[i*2];
      float low = lows[i*2];
      float high = highs[i*2];

      float level = abs(value - avg);
      analogWrite(pins[i], 10 * level);

      Serial.print(low);
      Serial.print(" <- ");
      Serial.print(value);
      Serial.print(" -> ");
      Serial.print(high);
      Serial.print(" : ");
      Serial.print(avg);
      Serial.print(" (");
      Serial.print(level);
      Serial.print(")");
      Serial.print("\t\t");
    }
    Serial.println("");
  }
}
