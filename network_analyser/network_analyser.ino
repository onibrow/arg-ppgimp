/*
  ad5933-test
    Reads impedance values from the AD5933 over I2C and prints them serially.
*/

#include <Wire.h>
#include "AD5933.h"

#define START_FREQ  (10000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (10)
// TODO: TUNE THIS
#define REF_RESIST  (100)

int mux_zero[4] = {11, 10, 9, 8};
int mux_one[4]  = {A3, A2, A1, A0};

double gain[NUM_INCR + 1];
int phase[NUM_INCR + 1];

void setup(void)
{
  // Begin I2C
  Wire.begin();

  Serial.begin(9600);
  // while the serial stream is not open, do nothing:
  while (!Serial)
  {
  } ;
  Serial.println("AD5933 Test Started!");
  Serial.println("Calibrating...");

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
  for (int i = 0; i < 4; i++) {
    pinMode(mux_zero[i], OUTPUT);
    pinMode(mux_one[i],  OUTPUT);
    delay(1);
  }
  reset_muxes();
  delay(1);

  // Perform calibration sweep
  if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR + 1))
    Serial.println("Calibrated!");
  else
    Serial.println("Calibration failed...");

}

void reset_muxes() {
  // Sets the mux to look at calibration resistor
  for (int i = 0; i < 4; i++) {
    digitalWrite(mux_zero[i], HIGH);
    delay(1);
    digitalWrite(mux_one[i],  HIGH);
    delay(1);
  }
  AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR + 1);
  return;
}

void loop(void)
{
    meas_imp(2, 3);
//  set_pins(2, 3);
  delay(3000);
//    meas_imp(16, 16);
    meas_imp(3, 4);
//  set_pins(4, 5);
  delay(3000);
}

void meas_imp(int elec_zero, int elec_one) {
  if (elec_zero == elec_one && elec_zero != 16) {
    Serial.println("ERROR: Cannot measure impedance of same electrode");
    return;
  }
  if (elec_zero > 16 || elec_zero < 1 || elec_one > 16 || elec_one < 1) {
    Serial.println("ERROR: Select electrodes between 1 and 15.");
    return;
  }
  Serial.print("\nMeasuring between ");
  Serial.print(elec_zero);
  Serial.print(" and ");
  Serial.print(elec_one);
  Serial.println(". Starting Sweep...");
  set_pins(elec_zero, elec_one);
  delay(100);
  frequencySweepEasy();
  delay(50);
  reset_muxes();
}

void set_pins(int elec_zero, int elec_one) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(mux_zero[i], ((elec_zero - 1) >> i) & 0x1);
    delay(1);
    digitalWrite(mux_one[i],  ((elec_one  - 1) >> i) & 0x1);
    delay(1);
  }
}

void frequencySweepEasy() {
  // Create arrays to hold the data
  int real[NUM_INCR + 1], imag[NUM_INCR + 1];

  // Perform the frequency sweep
  if (AD5933::frequencySweep(real, imag, NUM_INCR + 1)) {
    // Print the frequency data
    int cfreq = START_FREQ / 1000;
    for (int i = 0; i < NUM_INCR + 1; i++, cfreq += FREQ_INCR / 1000) {
      // Print raw frequency data

      Serial.print(cfreq * 1000);
      Serial.print(": R=");
      Serial.print(real[i]);
      Serial.print("/I=");
      Serial.print(imag[i]);


      // Compute impedance
      double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
      double impedance = 1 / (magnitude * gain[i]) * 1000;
      Serial.print("  Gain=");
      Serial.print(1 / gain[i]);
      Serial.print("  |Z|=");
      Serial.println(impedance);
    }
  } else {
    Serial.println("Frequency sweep failed...");
  }
}
