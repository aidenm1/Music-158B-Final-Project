#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc;

float fundfreqs[] = {261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.25};
float ratios[] = {1, 1.12246204831, 1.25992104989, 1.33483985417, 1.49830707688, 1.68179283051, 1.88774862536, 2};

float freqout;
float rawPrevNote = -999.0;

void MyCallback(float **in, float **out, size_t size) {
  Serial.begin(9600);
  float sine_signal;
  osc.SetFreq(freqout);
  for (size_t i = 0; i < size; i++) {
    sine_signal = osc.Process();
    out[0][i] = sine_signal;
    out[1][i] = sine_signal;
  }
}


void setup() {
  // put your setup code here, to run once:
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  osc.Init(sample_rate);
  osc.SetAmp(0.1);
  osc.SetWaveform(osc.WAVE_SIN);

  DAISY.begin(MyCallback);

}

void loop() {
  // put your main code here, to run repeatedly:
  analogReadResolution(16);
  float rawCurrentNote = analogRead(A0);//wrapped 
  float temp = rawCurrentNote;
  float rawCurrentScale = analogRead(A1);
  if (abs(rawCurrentNote - rawPrevNote) > 50) {
    rawCurrentNote = rawPrevNote;
  }
  int currentNote = min(max(((-8) * rawCurrentNote)/(14000) + 16.57, 0.0), 7.0);
  int currentScale = min(max(((-13) * rawCurrentScale)/(16000) + 19.5, 0.0), 12.0);
  freqout = fundfreqs[currentScale] * ratios[currentNote];
  rawPrevNote = temp;
}
