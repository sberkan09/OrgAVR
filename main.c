/*
 * proje_ele519.c
 *
 * Created: 10.08.2024 11:04:33
 * Author : Sami Berkan
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

void writeEeprom(uint16_t, int8_t);
uint8_t readEeprom(uint16_t);

uint8_t key, record;
uint16_t recordedLength, firstRecord, SecondRecord;

int main(void)
{
	record = 0x00;
	
	DDRC = 0x00; //input
	PORTC = 0xFF; // pull-up
	
	DDRD = 0b00111100; // PC2:5 output
	PORTD = 0x00; // low
	

	PCICR |= (1<<PCIE1); // Pin change interrupt 0(PORTC) enable
	PCMSK1 |= (1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10)|(1<<PCINT11); // PC0:3
	sei();
	
	DDRB = (1<<PORTB1); // PB1(OC1A) output
	
	
	DDRB |= (1<<PORTB0);
	PORTB &= ~(1<<PORTB0); // Use PB0 as GND if necessary
	
	// TIMER1 Settings
	TCCR1B |= (1<<WGM12); // CTC mode
	TCCR1A |= (1<<COM1A0); // Toggle output
	TCCR1B |= (1<<CS11); // Start TIMER1, Prescale factor N=8

	while (1)
	{
		TCCR1B &= ~(1<<CS11);
	}
}

ISR(PCINT1_vect){
	int8_t tone, dur, offset = 0;
	
	PORTD = ~(0x04); // 4th row low
	_delay_us(50);
	if(~PINC & (1<<PINC3)){
		offset = 12;
	} // * S1
	
	if(~PINC & (1<<PINC2)){
		OCR1B=0x30;
	} // 0 S5
	
	if(~PINC & (1<<PINC1)){
		uint16_t sth = (((uint16_t)readEeprom(0) << 8) | (readEeprom(1))) - 2;
		if (sth != 0 && record == 0 && sth < 50) {
			uint8_t melody[sth];
			for (uint16_t i = 0; i < sth; i++) {
				melody[i] = readEeprom(i + 2);
			}
			
			for(uint8_t i = 0; i < sth; i++){
				tone = melody[i]; // Get instant tone
				dur = 25*4-10; // Get instant duration
				OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone)/12.0))-1;
				
				TCCR1B |= (1<<CS11); // Start tone
				for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
					_delay_ms(5);
				}
				TCCR1B &= ~(1<<CS11); // Stop tone
				_delay_ms(100); // Wait some before playing next tone
			}
			_delay_ms(1000); // Wait some before playing next melody cycle
		} else if (record == 0x01) {
			
		}
	} // # S9
	
	if(~PINC & (1<<PINC0)){
		record = 0x01;
	} // D S13
	
	PORTD = ~(0x20); // 1st row low
	
	_delay_us(50);
	
	if(~PINC & (1<<PINC3)){ // S4
		tone=9;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // do 1
	
	if(~PINC & (1<<PINC2)){ // S8
		tone=7;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // re 2
	
	if(~PINC & (1<<PINC1)){ // S12
		tone=5;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // mi 3
	
	if(~PINC & (1<<PINC0)){ // S16
		int melody1[] = {5,5,4,2,2,4,5,7,9,9,7,5,7,9,9};
		int durations1[] = {4,4,4,4,4,4,4,4,4,4,4,4,6,2,8};

		for(uint8_t i=0; i<15; i++){
			tone = melody1[i]; // Get instant tone
			dur = 25*durations1[i]-10; // Get instant duration
			OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone)/12.0))-1;
			
			TCCR1B |= (1<<CS11); // Start tone
			for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
				_delay_ms(5);
			}
			TCCR1B &= ~(1<<CS11); // Stop tone
			_delay_ms(100); // Wait some before playing next tone
		}
		_delay_ms(1000); // Wait some before playing next melody cycle
	} // A
	
	PORTD = ~(0x10); // 2nd row low
	_delay_us(50);
	if(~PINC & (1<<PINC3)){ // S3
		tone=4;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // fa 4
	
	if(~PINC & (1<<PINC2)){ // S7
		tone=2;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // sol 5
	
	if(~PINC & (1<<PINC1)){ // S11
		tone=0;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // la 6
	
	if(~PINC & (1<<PINC0)){ // S15
		int melody1[] = {9,2,0,5,4,7,5,5,4,2,0,9,5,5,4,9,7};
		int durations1[] = {4,2,4,6,4,2,4,4,4,4,6,4,4,8,4,4,4};
		for(uint8_t i=0; i<14; i++){
		
			tone = melody1[i]; // Get instant tone
			dur = 25*durations1[i]-10; // Get instant duration
			OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone)/12.0))-1;
		
			TCCR1B |= (1<<CS11); // Start tone
			for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
				_delay_ms(5);
			}
			TCCR1B &= ~(1<<CS11); // Stop tone
			_delay_ms(100); // Wait some before playing next tone
		}
		_delay_ms(1000); // Wait some before playing next melody cycle
	} // B

	PORTD = ~(0x08); // 3rd row low
	_delay_us(50);
	if(~PINC & (1<<PINC3)){ // S2
		tone = -2;
		dur = 90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} // si // 7
	
	if(~PINC & (1<<PINC2)){ // S6
		tone = -3;
		dur=90;
		OCR1A = (uint16_t)round(2272*pow(2.0,(double)(tone - offset)/12.0))-1; // Compute OCR to generate frequency of the tone
		TCCR1B |= (1<<CS11); // Start tone
		for(uint8_t t=0; t<dur; t++){ // Play the tone for the duration
			_delay_ms(5);
		}
		TCCR1B &= ~(1<<CS11); // Stop tone
		if (record != 0x00)
			writeEeprom(recordedLength++, tone - offset);
		_delay_ms(100);
	} //ince do // 8
	
	if(~PINC & (1<<PINC1)){OCR1B=0x39;} // 9 // S10
	
	if(~PINC & (1<<PINC0)){
		if (record != 0) {
			record = 0x00;
			writeEeprom(0, recordedLength >> 8);
			writeEeprom(1, recordedLength & 0x00FF);
			recordedLength = 0x02;
		}
	} // C // S14

	PORTD = 0x00; // low
	TCCR1B &= ~(1<<CS11);
	PORTB &= ~(1<<PORTB1);
}

void writeEeprom(uint16_t adr, int8_t value) {
	
	// EEPROM Programming Process
	while(EECR & (1<<EEPE)); // Wait until previous program completed
	EEARL = (uint8_t)(0x00FF & adr); // First, address lowbyte first
	EEARH = (uint8_t)(adr>>8); // Then, address highbyte
	EEDR = value; // Recorded data
	EECR |= (1<<EEMPE); // Master program enable
	EECR |= (1<<EEPE); // Program enable
}

uint8_t readEeprom(uint16_t adr) {
	
	while(EECR & (1<<EEPE)); // Wait until previous program completed
	EEARL = (uint8_t)(0x00FF & adr); // First, address lowbyte first
	EEARH = (uint8_t)(adr>>8); // Then, address highbyte
	EECR |= (1<<EERE); // Read enable 
	return EEDR; // Read recorded data
}