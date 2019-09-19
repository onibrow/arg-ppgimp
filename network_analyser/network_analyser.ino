/*
  ad5933-test
    Reads impedance values from the AD5933 over I2C and prints them serially.
*/

#include <Wire.h>
#include "AD5933.h"

#define START_FREQ  (1000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (99)
// TODO: TUNE THIS
#define REF_RESIST  (200000)

int sel_0 = 2;
int sel_1 = 3;

double gain[NUM_INCR + 1];
int phase[NUM_INCR + 1];

void setup(void)
{
  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  Serial.println("AD5933 Test Started!");

  // Perform initial configuration. Fail if any one of these fail.
  if (!(AD5933::reset() &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1)))
  {
    Serial.println("FAILED in initialization!");
    while (true) ;
  }

  // Set Analog Mux Selection Pins
  pinMode(sel_0, OUTPUT);
  pinMode(sel_1, OUTPUT);
  digitalWrite(sel_0, HIGH);
  digitalWrite(sel_1, LOW);
  delay(1);
  
  // Perform calibration sweep
  if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR + 1))
    Serial.println("Calibrated!");
  else
    Serial.println("Calibration failed...");

}

void loop(void)
{
  digitalWrite(sel_0, LOW);
  digitalWrite(sel_1, LOW);
  frequencySweepEasy();
  delay(1);
  Serial.println("00 Sweep (510k)");
  delay(5000);

  digitalWrite(sel_0, HIGH);
  digitalWrite(sel_1, LOW);
  frequencySweepEasy();
  delay(1);
  Serial.println("01 Sweep (200k)");
  delay(5000);
//
//  digitalWrite(sel_0, LOW);
//  digitalWrite(sel_1, HIGH);
//  delay(1);
//  frequencySweepEasy();
//  Serial.println("10 Sweep (0.1uF)");
//  delay(5000);
//  
//  digitalWrite(sel_0, HIGH);
//  digitalWrite(sel_1, HIGH);
//  delay(1);
//  frequencySweepEasy();
//  Serial.println("11 Sweep (100K + 1uF)");
//  delay(5000);

}

// Easy way to do a frequency sweep. Does an entire frequency sweep at once and
// stores the data into arrays for processing afterwards. This is easy-to-use,
// but doesn't allow you to process data in real time.
void frequencySweepEasy() {
  // Create arrays to hold the data
  int real[NUM_INCR + 1], imag[NUM_INCR + 1];

  // Perform the frequency sweep
  if (AD5933::frequencySweep(real, imag, NUM_INCR + 1)) {
    // Print the frequency data
    int cfreq = START_FREQ / 1000;
    for (int i = 0; i < NUM_INCR + 1; i++, cfreq += FREQ_INCR / 1000) {
      // Print raw frequency data
      
      Serial.print(cfreq*1000);
      Serial.print(": R=");
      Serial.print(real[i]);
      Serial.print("/I=");
      Serial.print(imag[i]);
      

      // Compute impedance
      double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
      double impedance = 1 / (magnitude * gain[i]);
      Serial.print("  Gain=");
      Serial.print(1/ gain[i]);
      Serial.print("  |Z|=");
      Serial.println(impedance);
    }
//    Serial.println("Frequency sweep complete!");
  } else {
//    Serial.println("Frequency sweep failed...");
  }
}
