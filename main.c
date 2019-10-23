#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED_1 PB0
#define INPUT	PC0
#define OUTPUT_1	PB1
#define OUTPUT_2	0

	float	start_tuning = 3.5;					//próg od którego ma ruszac
	float tuning = 0.5;								//ile sciagamy sygnalu

void adc_init()
	{
	//REFS1 + REFS0 = AVCC
	//ADLAR = ADC resoult right
	//MUX3...0 = PIN in ADC0
		ADMUX|=(1<<REFS0);
			/*
			|(0<<REFS1)
			|(0<<ADLAR)
			|(0<<MUX3)
			|(0<<MUX2)
			|(0<<MUX1)
			|(0<<MUX0)
			*/

	//ADEN turn ON
	//ADSC start
	//ADFR free runing
	//ADIE interupt off
	//ADPS2...0 division factory = 128

		ADCSRA|=(1<<ADEN)
			|(1<<ADSC)
			|(1<<ADFR)
			|(0<<ADIE)
			|(1<<ADPS2)
			|(1<<ADPS1)
			|(1<<ADPS0);
	}

void pwm_init(uint8_t stop)
	{
		DDRB|=(1<<OUTPUT_1);	//outpin A
			//|(1<<OUTPUT_2);		//outpin B

		TCCR1A|=(1<<COM1A1)					//Clear OC1A on CM, set at BOTTOM
					|(0<<COM1A0)			//Clear OC1A on CM, set at BOTTOM
					|(0<<COM1B1)			//Clear OC1A on CM, set at BOTTOM
					|(0<<COM1B0)			//Clear OC1A on CM, set at BOTTOM
					|(1<<WGM11)				//fast PWM with top=ICR1
					|(0<<WGM10);			//fast PWM with top=ICR1

			TCCR1B|=(0<<ICNC1)				//input capture noise canceler
					|(0<<ICES1)				//input capture edge select
					|(1<<WGM13)				//fast PWM with top=ICR1
					|(1<<WGM12)				//fast PWM with top=ICR1
					|(0<<CS12)				//NO PRESCALER
					|(0<<CS11)				//NO PRESCALER
					|(1<<CS10);				//NO PRESCALER

			ICR1 = 1024;					//TOP!!

			OCR1A = stop;				//STOP!

	}

void funkcja_tuning(float chwilowy_start_tuning, float chwilowy_tuning)
{
	//aktywacja funkcji ADC

	uint16_t prog = ((chwilowy_start_tuning * 1024)/5);				//zamiana wartosci startowej na inta JEST OK!
	uint16_t tuningowany_sygnal =  ((ADC - chwilowy_tuning));		//im wyzsza wartosc ADC tym wiekszy % tuningu,

	if(ADC<prog)
		{
			pwm_init(ADC);
			DDRB|=(1<<LED_1);
			PORTB|=(1<<LED_1);
		}
	if(ADC>=prog)
	{
		pwm_init(tuningowany_sygnal);
		DDRB&=~(1<<LED_1);
		PORTB&=~(1<<LED_1);
	}

}
int main (void)
{

	adc_init();

	while(1)
	{
		funkcja_tuning(start_tuning, tuning);
	}
}


