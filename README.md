# Arias Research Group - PPG & Impedence Measurement

## Seiya Ono Sp'19

The goal of this project is to collaborate with doctors at UCSD and Dr. Muller's student, Sina Faraji, to create a test bench for our group's Bioimpedence Array and OLED/OPD Oxymeter sensors to measure tissue damage in mice. We hope to use Sina Faraji's new OLED/OPD driver chip to take PPG measurements while using our own impedence arrays and electronics to measure tissue impedence and driver current through our target.

-----

## Specs

- 1 PPG Sensor
    - 1 Red OLED
    - 1 NIR OLED
    - 1 OPD
    - Sina's Driver Chip
- 15 Impedence Electrodes
    - Impedence measurement AFE
    - Current Driver (200uA to 700uA, up to 1mA)
    - 15 to 2 selection mux
- 9V Power
- Coms over Serial

### PPG Sensor

Sina's OLED/OPD Driver Amplifier has the capability to driver 8 pairs of OLEDs and take readings from 8 OPDs. It handles all the amplification through SPI programmable registers. The closed loop in the transimpedence amplifiers can help get rid of any DC components in the OPD readings. It runs off of 1.1V digital logic, and 8V driver power.

If his chip does not go well, we will implement our own OLED drivers and OPD transimpedence amplifiers.

### Impedence Electrodes

We were asked to get our 15 electrode array to be able to take any impedence measurement between any two user selectable electrodes, while also being able to select any two electrodes to drive with a set amount of current. This divides this part of the board into three distance parts - the multiplexor selection and configuration, tunable current source, and impdence measurement. The following section will outline the different chip selection and their respective specs.

#### Multiplexor

Because the board is spec'd to be able to take impedence measurements as well as source current, it is critical to choose a multiplexor that has very low on resistance, while having very high off resistance. Luckily, most CMOS technology has this as standard, so picking an analog mux was not very difficult. We settled with the [ADG1608/1609](https://www.analog.com/en/products/adg1608.html) because of its 3 bit (8 input) mux, as well as its secondary variant 2 bit (4x2 input) DPDT (double pole double throw) configuration. To differentiate, the ADG1608 variant is the 3 bit 8 input mux, and the ADG1609 is the 2 bit 4x2 input mux. See the figure below for clarification.

![img/adg1608-1609.png](mux diff)

By using two pairs of ADG1608's, a 16x2 input 1x2 output can be created. This means the circuit can individually select a pair of electrodes to be used by the impedence analyzer and tunable current source. To select between either of the two inputs, the ADG1609 will be used as a demuxer to select either the impedence analyzer or tunable current source. The DPDT configuration is very usedful in this sceneario because we are trying to always drive a _pari_ of electrodes.

