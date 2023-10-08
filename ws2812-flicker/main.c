#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>
#include "light_ws2812.h"

#ifndef F_CPU
#define F_CPU 16000000UL // Assuming a 16 MHz clock for ATmega328p
#endif

#define BUTTON_PIN PB2
#define PIR_PIN PB1

bool signal_state = false;
const float flicker_duration = 5.0;
const float max_timeout = 8.0;
const float speedup_factor = 3.0;  // intervals are divided by this to get more intervals per unit time

int light_state = 1; // 1 = on, 0 = off
//
// Assuming you have a strip of 10 LEDs
#define NUM_LEDS 10

// Declare an array to hold the LED color data
uint8_t ledData[NUM_LEDS * 3];

// Function to set an LED to a specific color
void setLED(uint8_t led, uint8_t red, uint8_t green, uint8_t blue) {
    ledData[led * 3] = green;      // GRB format
    ledData[led * 3 + 1] = red;
    ledData[led * 3 + 2] = blue;
}

// Function to update all LEDs to a single color
void setAllLEDs(uint8_t red, uint8_t green, uint8_t blue) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        setLED(i, red, green, blue);
    }
}

void turn_on_light(){
    //setAllLEDs(191, 120, 105);
    setAllLEDs(50,50,50);
    ws2812_sendarray(ledData, sizeof(ledData));
}

void turn_off_light(){
    setAllLEDs(0, 0, 0);
    ws2812_sendarray(ledData, sizeof(ledData));
}

bool button_pressed() {
    // presently we're using an internal pull-up,
    // so by default the input is high.
    // Check if A1 (PB2) is high
    if (PINB & (1 << BUTTON_PIN)) {
        return false;
    } else {
        return true;
    }
}

bool pir_motion_detected() {
    // PIR sensor appears to pull logic low when nothing is happening.
    // So no Pull-down needed.
    // Check if A(?) (PB1) is high
    if (PINB & (1 << PIR_PIN)) {
        return true;
    } else {
        return false;
    }
}

// Define the piecewise function
float piecewise_function(float x) {
    if (0 <= x && x <= 5) {
        return 1 - x * 0.2 / 5;  // Drop 0.2 in 5
    } else if (5 < x && x <= 6) {
        return 0.8 - (x - 5) * 0.4 / 1;  // drop 0.4 in 1
    } else if (6 < x && x <= 10) {
        return 0.4 - (x - 6) * 0.2 / 4;  // drop 0.2 in 4
    } else {
        return -1;
    }
    //    return 1 - x*0.2/5  # Drop 0.2 in 5
    //elif 5 < x <= 6:
    //    return 0.8 - (x - 5)*0.4/1 # drop 0.4 in 1
    //elif 6 < x <= 10:
    //    return 0.4 - (x - 6)*0.2/4 # drop 0.2 in 4
}

// Generate intervals
void get_intervals(float* intervals, int* size) {
    float duration = flicker_duration;
    float tsum = 0.0;
    *size = 0;
    float interval = 0.0;
    while (1) {
        // interval units are ... 
        // send a value to piecewise_function between 0 and 10
        interval = piecewise_function((float)rand() / (float)(RAND_MAX / 10));
        //interval = 0.5;  // fake for testing
        if (interval <= 0) break;  // this means an error in interval calculations, right?
        tsum += interval / speedup_factor;
        if (tsum > duration) {
            intervals[*size] = duration;
            //intervals[*size] = duration - tsum;
            (*size)++;
            break;
        }
        intervals[*size] = tsum;
        (*size)++;
    }
}

int main(void) {
    float timeout = max_timeout;
    float intervals[100];
    int size = 0;

    // Setup
    // Configure A1 (PB2) as input
    DDRB &= ~(1 << BUTTON_PIN);
    // Configure A(?) (PB1) as input for PIR sensor; It appears to drive high when motion is detected.
    // PIR sensor appears to pull logic low when nothing is happening, so no pull-down needed.
    DDRB &= ~(1 << PIR_PIN);
    
    // Configure ws2812_pin PB0 as output
    DDRB |= _BV(ws2812_pin);  // Replace 'B' with the appropriate port

    // Enable pull-up resistor on A1 (PB2) for Button
    PORTB |= (1 << BUTTON_PIN);

    //Except we're using a real resistor I think
    //setAllLEDs(191, 120, 105);
    // Signal startup with red LEDs for 3 seconds; we'll notice crashes...
    // red
    setAllLEDs(50, 0, 0);
    
    // Update LED strip
    ws2812_sendarray(ledData, sizeof(ledData));
    
    // Wait for 3 seconds
    _delay_ms(1500);

    // test this next
    setAllLEDs(50,50,50);
    ws2812_sendarray(ledData, sizeof(ledData));

    while (1) {
        if (timeout < max_timeout) {
            _delay_ms(1); // delay for 1ms
            timeout += 0.001;  // 1 ms
        }

        // New event triggered, but only if we're not processing/coolin down from another recent one
        if (timeout >= max_timeout) {

            //setAllLEDs(0, 50, 0);  // green
            //ws2812_sendarray(ledData, sizeof(ledData));
            //_delay_ms(1000);
    
            // Real logic triggered by button presses
            if (button_pressed()) {
                //setAllLEDs(0, 0, 50);  // blue
                //ws2812_sendarray(ledData, sizeof(ledData));
                //_delay_ms(1000);
                timeout = 0;
                signal_state = false;  // TODO unused
                size = 0;
                get_intervals(intervals, &size);  // lazy replacement of values in intervals?
            }
            if (pir_motion_detected()) {
                //setAllLEDs(50, 50, 0);  // red-green = yellow?
                //ws2812_sendarray(ledData, sizeof(ledData));
                //_delay_ms(1000);
                timeout = 0;
                signal_state = false;  // TODO unused
                size = 0;
                get_intervals(intervals, &size);  // lazy replacement of values in intervals?
            }
        }

        // If we have intervals, then we're flickering
        if (size > 0) {
            float interval = intervals[size - 1];
            if (size == 1) {
                size = 0;
                turn_on_light();
            } else if (max_timeout - interval < timeout) {
                if (light_state == 1) {
                    turn_off_light();
                    light_state = 0;
                } else {
                    turn_on_light();
                    light_state = 1;
                }
                size--;
            }
        }
    }

    return 0;
}
