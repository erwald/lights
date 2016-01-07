/*
  Lights.
*/

// #define DEBUG 1

// FFT
#define LOG_OUT 1 // Use the log output function.
#define FHT_N 16 // Set to 16 point FHT.

#include "Arduino.h"
#include <FHT.h>
#include <Filters.h>

/*
  Declarations
  */

#define NUM_LEDS 2
#define BINS_PER_LED 4

#define MOVING_AVG_ORDER 8

int pins[NUM_LEDS] = { 5, 10 };
int pin_fft_map[NUM_LEDS][BINS_PER_LED] = { {0, 1, 2, 3} , {4, 5, 6, 7} };

uint8_t fft_buffer[MOVING_AVG_ORDER][FHT_N];
int current_index = 0;
float moving_avgs[FHT_N]; // The moving averages of the FFT spectrum.
float lows[FHT_N]; // The lows of the last samples for all bins.
float highs[FHT_N]; // The highs of the last samples for all bins.

// FilterOnePole highPassFilter( HIGHPASS, 10000 ); // A high pass filter at 20 hz.

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

  // FHT
  TIMSK0 = 0; // Turn off timer0 for lower jitter.
  ADCSRA = 0xe5; // Set the adc to free running mode.
  ADMUX = 0x40; // Use adc0.
  DIDR0 = 0x01; // Turn off the digital input for adc0.
}

/*
  The loop function runs over and over again forever.
  */
void loop() {
  // FHT
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      // Serial.println(m);
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();

    // Calculate lows and highs.

    // For each bin.
    for (int i = 0; i < FHT_N; i++) {
      lows[i] = fht_log_out[i];
      highs[i] = fht_log_out[i];

      for (int j = 0; j < MOVING_AVG_ORDER; j++) {
        lows[i] = min(lows[i], fft_buffer[j][i]);
        highs[i] = max(highs[i], fft_buffer[j][i]);
      }
    }

    // Calculate moving averages.

    float b = 1 / (MOVING_AVG_ORDER + 1.0); // Filter coefficient.

    // For each bin.
    for (int i = 0; i < FHT_N; i++) {
      // Calculate output.
      moving_avgs[i] = b * fht_log_out[i];

      for (int j = 0; j < MOVING_AVG_ORDER; j++) {
        moving_avgs[i] += b * fft_buffer[j][i];
      }

      // Add new FFT value to buffer.
      fft_buffer[current_index][i] = fht_log_out[i];
    }

    current_index = (current_index + 1) % MOVING_AVG_ORDER; // Update the index.

    // Update LEDs.

    int scale_factor = FHT_N / NUM_LEDS;

    for (int i = 0; i < NUM_LEDS; i++) {
      // Downsample spectrum output to 2 bins.
      float value = 0;
      float avg = 0;
      float low = 0;
      float high = 0;

      for (int j = 0; j < BINS_PER_LED; j++) {
        int idx = pin_fft_map[i][j];
        value += fht_log_out[idx];
        avg += moving_avgs[idx];
        low += lows[idx];
        high += highs[idx];
      }

      value = value / (float)scale_factor;
      avg = avg / (float)scale_factor;
      low = low / (float)scale_factor;
      high = high / (float)scale_factor;

      float level = max(value - avg, 0);
      analogWrite(pins[i], 100 * level);

      #ifdef DEBUG
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
      #endif
    }
    #ifdef DEBUG
    Serial.println("");
    #endif
  }
}
