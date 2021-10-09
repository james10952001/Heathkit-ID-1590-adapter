/*
Adapter for interfacing a potentiometer type wind vane to the Heathkit ID-1590 wind 
speed/direction display. This has been tested with the NRG 200M wind vane but should 
work equally well with other potentiometer type wind vanes. ATMEGA8 is overkill but
was what I had on hand at the time.
v1.00 (c)2020 James Sweet
*/

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>

#define HYSTERESIS	50			// used to prevent the position reading from jumping around
#define LAMPS 		PORTB		        // 8 lamps fits 8 bit IO port perfectly making this simple
#define N 		0b00010000	
#define N_NE		0b00110000
#define NE		0b00100000
#define E_NE		0b01100000
#define E		0b01000000
#define E_SE		0b11000000
#define SE		0b10000000
#define S_SE		0b10000001
#define S		0b00000001
#define S_SW		0b00000011
#define SW		0b00000010
#define W_SW		0b00000110
#define W		0b00000100
#define W_NW		0b00001100
#define NW		0b00001000
#define N_NW		0b00011000

int main(void)
{
	// all 8 pins of PORTB drive lamps
	DDRB = 0xFF; 	// set entire port to output
	LAMPS = 0x00;	// turn all lamps off to start
	// should disable digital input on ADC port
    
	// set up the ADC, ref is Vcc, using ch0 only
   	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADPS0);
   	ADMUX = (1<<REFS0); 

	// variable used for holding the current position reading
	unsigned int position = 0;

	// main loop starts here
	while(1)
	{
       	ADCSRA |= (1<<ADSC); 			// start ADC conversion by setting bit
        do{} while (ADCSRA & (1<<ADSC)); 	// wait for conversion to complete
		
		// provide a bit of hysteresis to prevent flickering, copy 
		// the ADC reading into the position variable if the difference is over 50
		if( abs(ADC-position) > HYSTERESIS)
		{
			position = ADC; 
		}
	
		/*
  		There are 8 lamps with 16 possible states to indicate the wind direction by
		illuminating either one or two adjacent lamps. This simply selects the state
		according to the value read from the wind vane pot by the ADC. 
		*/
        switch (position) {
           	case 0 ... 31:			// Pot wraps around due North, 4 degree dead spot
		case 991 ... 1023:
                	LAMPS = N; 
                	break;
           	case 32 ... 95:			
                	LAMPS = N_NE;
                	break;
           	case 96 ... 159:
                	LAMPS = NE;
                	break;
		case 160 ... 223:
			LAMPS = E_NE;
			break;
		case 224 ... 287:
			LAMPS = E;
			break;
		case 288 ... 351:
			LAMPS = E_SE;
			break;
		case 352 ... 415:
			LAMPS = SE;
			break;
		case 416 ... 479:
			LAMPS = S_SE;
			break;
		case 480 ... 543:
			LAMPS = S;
			break;
		case 544 ... 607:
			LAMPS = S_SW;
			break;
		case 608 ... 671:
			LAMPS = SW;
			break;
		case 672 ... 736:
			LAMPS = W_SW;
			break;
		case 737 ... 799:
			LAMPS = W;
			break;
		case 800 ... 863:
			LAMPS = W_NW;
			break;
		case 864 ... 927:
			LAMPS = NW;
			break;
		case 928 ... 990:
			LAMPS = N_NW;
			break;
            default:
                LAMPS = 0x00; // this should never be possible to hit
                break;
        }
	}
}
