// Import Minim.
import ddf.minim.*;
import ddf.minim.analysis.*;

Minim minim;
AudioPlayer song;
FFT fft;

float maxFFT = 100.0;

void setup() {
  // Set up view.
  size(1024, 768);
  noStroke();

  // Set up Minim.
  minim = new Minim(this);
  song = minim.loadFile("mannnn.mp3", 256);
  song.play();

  // an FFT needs to know how
  // long the audio buffers it will be analyzing are
  // and also needs to know
  // the sample rate of the audio it is analyzing
  fft = new FFT(song.bufferSize(), song.sampleRate());
}

void draw() {
  background(0);

  // first perform a forward fft on one of song's buffers
  // I'm using the mix buffer
  //  but you can use any one you like
  fft.forward(song.mix);

  // Draw FFT spectrum.
  stroke(255, 222);
  fill(255, 222);

  float specHeight = 100;
  float specWidth = width / 2;
  float specSpacing = specWidth / fft.specSize();

  text("FFT", 0, 20);

  for(int i = 0; i < fft.specSize(); i++) {
    float scaledFft = 1 - (maxFFT - fft.getBand(i)) / maxFFT; // Between 1 and 0.

    // Draw the line for frequency band i.
    line(i * specSpacing, specHeight, i * specSpacing, specHeight - scaledFft * specHeight);
  }

  // Downsample spectrum output to 2 bins.
  float[] intensities = {0.0, 0.0};

  for(int i = 0; i < fft.specSize(); i++) {
    int binIdx = intensities.length * i / fft.specSize();
    intensities[binIdx] += fft.getBand(i) / (fft.specSize() / intensities.length);
  }

  for(int i = 0; i < intensities.length; i++) {
    float lineHeight = specHeight + (1 - intensities[i]);
    line(i * (specWidth / 2), lineHeight, (i + 1) * (specWidth / 2), lineHeight);
  }

  // Draw "light bulbs".
  fill(255, 222);
  stroke(0, 0);

  float intensity1 = intensities[0];
  float intensity2 = intensities[1];
  float lampSize = 100;

  text("Lamp 1", width / 3, height - lampSize * 2);
  text("Lamp 2", 2 * width / 3, height - lampSize * 2);

  fill(intensity1 * 255);
  ellipse(width / 3, height - lampSize, lampSize, lampSize);
  fill(intensity2 * 255);
  ellipse(2 * width / 3, height - lampSize, lampSize, lampSize);
}
