#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

// Define buzzer pin (change according to your connection)
#define BUZZER_PORT PORTB
#define BUZZER_DDR DDRB
#define BUZZER_PIN PB0

// Function to generate tone on passive buzzer
// Note: Limited to compile-time known frequencies
/*void beep() {
	// Fixed frequency beep (approximately 1kHz)
	// Duration fixed at 500ms (can be adjusted)
	for(uint16_t i = 0; i < 500; i++) {  // 500ms duration
		for(uint8_t j = 0; j < 100; j++) {  // Approx 1kHz frequency
			BUZZER_PORT |= (1 << BUZZER_PIN);
			_delay_us(5);  // Half period for ~1kHz
			BUZZER_PORT &= ~(1 << BUZZER_PIN);
			_delay_us(5);  // Half period for ~1kHz
		}
	}
}*/
void beep() {
	// Optimized for ~2.5kHz (more efficient frequency)
	for(uint16_t i = 0; i < 1250; i++) {  // 500ms duration
		BUZZER_PORT |= (1 << BUZZER_PIN);
		_delay_us(200);  // Quarter period for ~2.5kHz
		BUZZER_PORT &= ~(1 << BUZZER_PIN);
		_delay_us(200);  // Quarter period for ~2.5kHz
	}
}

int main(void) {
	char cmd;
	
	// Initialize buzzer pin as output
	BUZZER_DDR |= (1 << BUZZER_PIN);
	BUZZER_PORT &= ~(1 << BUZZER_PIN); // Start with buzzer off
	
	serial_init(9600);
	serial_string("HC05 8-Wheel Control Ready\n");

	while (1) {
		if(serial_available()) {
			cmd = serial_read();
			switch(cmd) {
				case 'A':
				serial_string("You've pressed A - Buzzer sound\n");
				beep(); // Fixed frequency beep
				break;
			}
		}
		_delay_ms(10);
	}
	return 0;
}