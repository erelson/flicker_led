#include <avr/io.h>
#include <util/delay.h>
#include "light_ws2812.h"

// Assuming you have a strip of 10 LEDs
#define NUM_LEDS 140

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

int main(void) {
    // Assuming ws2812_pin and ws2812_port are already set,
    // usually, this is done in "light_ws2812.h"
    
    // Configure ws2812_pin as output
    DDRB |= _BV(ws2812_pin);  // Replace 'B' with the appropriate port
    
    while (1) {
        // RGB
        //// Set all LEDs to red
        //setAllLEDs(25, 25, 25);
        //
        //// Update LED strip
        //ws2812_sendarray(ledData, sizeof(ledData));
        //
        //// Wait for 3 seconds
        //_delay_ms(3000);
        
        // Set all LEDs to white
        setAllLEDs(191, 120, 105);
        
        // Update LED strip
        ws2812_sendarray(ledData, sizeof(ledData));
        
        // Wait for 3 seconds
        _delay_ms(3000);
    }
    
    return 0;
}
