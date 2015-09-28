//  constants related to the Arduino Nano pin use
const int clkIn = 2;           // the digital (clock) input
const int digPin[2] = {3, 4};  // the digital output pins
const int pinOffset = 5;       // the first DAC pin (from 5-12)

//  variables for interrupt handling of the clock input
volatile int clkState = LOW;

unsigned long lastClock = 0;
unsigned long time = 0;
// Set dt to 500millis i.e. 120bpm
unsigned long dt = 500, dt1 = 500, dt2 = 500, dt3 = 500, prevDt = 500;
float multipliers[8] = {4.0, 3.0, 2.0, 1.0, 1.0, 1.0/2.0, 1.0/3.0, 1.0/4.0};
unsigned int clockDts[8] = {500, 500, 500, 500, 500, 500, 500, 500};
unsigned long lastTicks[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int gateDt = 10;

void setup() {
    Serial.begin(9600);

    // set up the digital (clock) input
    pinMode(clkIn, INPUT);

    // set up the digital outputs
    for (int i=0; i<2; i++) {
        pinMode(digPin[i], OUTPUT);
        digitalWrite(digPin[i], LOW);
    }

    // set up the 8-bit DAC output pins
    for (int i=0; i<8; i++) {
        pinMode(pinOffset+i, OUTPUT);
        digitalWrite(pinOffset+i, LOW);
    }

    attachInterrupt(0, clockInterupt, RISING);
}

void loop()
{
    int i;

    time = millis();

    if (clkState == HIGH) {
        clkState = LOW;  // reset for the next clock
        dt = time - lastClock;
        lastClock = time;
        dt = avgDt(dt);
        if (prevDt != dt) {
            updateClockDts();
        }
        prevDt = dt;
    }

    for (i=0; i<8; i++) {
        if (time - lastTicks[i] > clockDts[i]) {
            lastTicks[i] = time;
            digitalWrite(pinOffset+i, HIGH);
        } else if ((time - lastTicks[i]) > gateDt) {
            digitalWrite(pinOffset+i, LOW);
        }
    }
}

void updateClockDts() {
    for (int i=0; i<8; i++) {
        clockDts[i] = dt * multipliers[i];
    }
}

unsigned long avgDt(unsigned long dt) {
    dt1 = dt2;
    dt2 = dt3;
    dt3 = dt;
    return (dt1 + dt2 + dt3) / 3;
}

void clockInterupt()
{
    clkState = HIGH;
}






