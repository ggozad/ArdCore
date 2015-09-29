//  constants related to the Arduino Nano pin use
const int clkIn = 2;     // the digital (clock) input
const int clockPin = 3;  // the digital output pins
const int resetPin = 4;
const int pinOffset = 5; // the first DAC pin (from 5-12)

const int resetIn = 3;   // The analog reset input
const int clockMultIn = 2;

// variables for interrupt handling of the clock input
volatile int clkState = LOW;
int resetState = LOW;

unsigned long time = 0, lastClock = 0, lastReset = 0;

// Set initial dt to 500millis i.e. 120bpm
unsigned long dt = 500, dt1 = 500, dt2 = 500, dt3 = 500, prevDt = 500;

// Clock multipliers
float clockMultiplier = 1;
float multipliers[8] = {4.0, 3.0, 2.0, 1.0, 1.0, 1.0/2.0, 1.0/3.0, 1.0/4.0};

unsigned int clockDts[8] = {500, 500, 500, 500, 500, 500, 500, 500};

// The last time an output was high on the expander
unsigned long lastTicks[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// The gate duration, 10ms by default.
unsigned int gateDt = 10;

void setup() {
    Serial.begin(9600);

    // set up the digital (clock) input
    pinMode(clkIn, INPUT);

    // set up the digital outputs
    pinMode(clockPin, OUTPUT);
    digitalWrite(clockPin, LOW);
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);

    // set up the expander output pins
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

    if ((time - lastClock) > gateDt) {
        digitalWrite(clockPin, LOW);
    }

    if ((time - lastReset) > gateDt) {
        resetState = LOW;
        digitalWrite(resetPin, LOW);
    }

    // When a clock is received...
    if (clkState == HIGH) {

        // Reset for the next clock
        clkState = LOW;
        dt = time - lastClock;
        // Bring the clock in the range of 0.25 to 2.0 of whatever it is
        clockMultiplier = ((analogRead(clockMultIn) >> 7) + 1.0) / 4.0;
        dt = dt * clockMultiplier;
        digitalWrite(clockPin, HIGH);

        lastClock = time;
        dt = avgDt(dt);

        // Update the dts when the trigers will clock if the new clock dt has changed.
        if (prevDt != dt) {
            updateClockDts();
        }
        prevDt = dt;
    }

    // Out to the expander gates.
    for (i=0; i<8; i++) {
        if (time - lastTicks[i] > clockDts[i]) {
            lastTicks[i] = time;
            digitalWrite(pinOffset+i, HIGH);
        } else if ((time - lastTicks[i]) > gateDt) {
            digitalWrite(pinOffset+i, LOW);
        }
    }

    // // Handle reset, when D0 is > 2.5V
    if (analogRead(resetIn) > 511 && resetState == LOW) {
        resetState = HIGH;
        digitalWrite(resetPin, HIGH);
        lastReset = time;
        for (i=0; i<8; i++) {
            lastTicks[i] = time;
            digitalWrite(pinOffset+i, HIGH);
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
