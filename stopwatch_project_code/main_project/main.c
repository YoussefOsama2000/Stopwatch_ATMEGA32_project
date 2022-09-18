#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned long long g_counter=0;
unsigned char sec;
unsigned char min;
unsigned char hour;
void setupTimer1(void){
	OCR1A=976;
	TCCR1A = (1<<FOC1A)| (1<<FOC1B);
	TCCR1B = (1<<CS12)|(1<<CS10)|(1<<WGM12);
	TCNT1=0;
	TIMSK |=(1<<OCIE1A);
	//	TCCR1B = (1<<WGM12) | (1<<CS11);
}
void setupINT0(void ){
	DDRD &=~(1<<PD2); //set pin 2 at port D as input pin
	PORTD |=(1<<PD2); //enable internal pull up of pin 2 at port D
	MCUCR = (MCUCR&~((1<<ISC01)|(1<<ISC00)))|(1<<ISC01); //make interrupt zero to falling edge
	GICR|=(1<<INT0); //enable interrupt 0
}
void setupINT1(void){
	DDRD &=~(1<<PD3); //set pin 3 at port D as input pin
	PORTD &=~(1<<PD3); //turn off internal pull up of pin 3 at port D
	MCUCR |= (1<<ISC10)|(1<<ISC11); //make interrupt zero to raising edge
	GICR|=(1<<INT1);

}
void setupINT2(void){
	DDRD &=~(1<<PD2); //set pin 2 in port D as input
	PORTD|=(1<<PD2);	//enable internal pull up
	MCUCSR&=~(1<<ISC2);	//enable int2
	GICR|=(1<<INT2); //enable interrupt 2
}
ISR(TIMER1_COMPA_vect)
{
	unsigned long long counter=g_counter;
	sec=counter%60;  //extract seconds
	counter/=60; //convert counter to min
	min=counter%60; //extract min
	counter/=60; //convert to hours
	hour =counter; //the rest of the number is hours
	g_counter++;
}
ISR(INT0_vect){
	g_counter=0;
	sec=0;
	min=0;
	hour=0;
	ICR1=0;
}

ISR(INT1_vect){
	TCCR1B &=~((1<<CS12)|(1<<CS11)|(1<<CS10));

}
ISR(INT2_vect){
	TCCR1B = (1<<CS12)|(1<<CS10)|(1<<WGM12);
}
int main (void)
{
	SREG|=(1<<7); // enable I-global interrupt
	DDRC|=0x0f; // decoder
	DDRA |=0Xff; // enable 6-7 segment
	PORTA |=0x20;
	PORTC &=0xff;
	setupINT0(); //setting up reset button
	setupINT1();
	setupINT2();
	setupTimer1(); //setting up global counter

	for(;;){

		PORTA=(PORTA&0x80)|0x20; //setting value of least digit
		PORTC=(PORTC&0xf0)|((sec%10)&0x0f);//set units of sec
		_delay_ms(2);
		PORTA=(PORTA&0x80)|0x10; //setting value of second least digit
		PORTC=(PORTC&0xf0)|((sec/10)&0x0f);//set tenth of sec
		_delay_ms(2);
		PORTA=(PORTA&0x80)|0x08; //setting value of third least digit
		PORTC=(PORTC&0xf0)|((min%10)&0x0f);//set units of min
		_delay_ms(2);
		PORTA=(PORTA&0x80)|0x04; //setting value of fourth least digit
		PORTC=(PORTC&0xf0)|((min/10)&0x0f);//set tenth of min
		_delay_ms(2);
		PORTA=(PORTA&0x80)|0x02; //setting value of fifth least digit
		PORTC=(PORTC&0xf0)|((hour%10)&0x0f);//set units of hour
		_delay_ms(2);
		PORTA=(PORTA&0x80)|0x01; //setting value of  most digit
		PORTC=(PORTC&0xf0)|((hour/10)&0x0f);//set tenth of hour
		_delay_ms(2);

	}

}
