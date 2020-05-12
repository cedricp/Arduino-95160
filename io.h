#pragma once
#include <Arduino.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


class DigitalPin{
	uint8_t m_pin, m_bit;
	volatile uint8_t *m_out;
	volatile uint8_t *m_in;

	static void turnOffPWM(uint8_t timer)
	{
		switch (timer)
		{
			#if defined(TCCR1A) && defined(COM1A1)
			case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
			#endif
			#if defined(TCCR1A) && defined(COM1B1)
			case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
			#endif
			#if defined(TCCR1A) && defined(COM1C1)
			case TIMER1C:   cbi(TCCR1A, COM1C1);    break;
			#endif

			#if defined(TCCR2) && defined(COM21)
			case  TIMER2:   cbi(TCCR2, COM21);      break;
			#endif

			#if defined(TCCR0A) && defined(COM0A1)
			case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
			#endif

			#if defined(TCCR0A) && defined(COM0B1)
			case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
			#endif
			#if defined(TCCR2A) && defined(COM2A1)
			case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
			#endif
			#if defined(TCCR2A) && defined(COM2B1)
			case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
			#endif

			#if defined(TCCR3A) && defined(COM3A1)
			case  TIMER3A:  cbi(TCCR3A, COM3A1);    break;
			#endif
			#if defined(TCCR3A) && defined(COM3B1)
			case  TIMER3B:  cbi(TCCR3A, COM3B1);    break;
			#endif
			#if defined(TCCR3A) && defined(COM3C1)
			case  TIMER3C:  cbi(TCCR3A, COM3C1);    break;
			#endif

			#if defined(TCCR4A) && defined(COM4A1)
			case  TIMER4A:  cbi(TCCR4A, COM4A1);    break;
			#endif
			#if defined(TCCR4A) && defined(COM4B1)
			case  TIMER4B:  cbi(TCCR4A, COM4B1);    break;
			#endif
			#if defined(TCCR4A) && defined(COM4C1)
			case  TIMER4C:  cbi(TCCR4A, COM4C1);    break;
			#endif
			#if defined(TCCR4C) && defined(COM4D1)
			case TIMER4D:	cbi(TCCR4C, COM4D1);	break;
			#endif

			#if defined(TCCR5A)
			case  TIMER5A:  cbi(TCCR5A, COM5A1);    break;
			case  TIMER5B:  cbi(TCCR5A, COM5B1);    break;
			case  TIMER5C:  cbi(TCCR5A, COM5C1);    break;
			#endif
		}
	}

	void pinMode(uint8_t mode)
	{
		m_bit = digitalPinToBitMask(m_pin);
		uint8_t port = digitalPinToPort(m_pin);
		volatile uint8_t *reg;

		if (port == NOT_A_PIN) return;

		// JWS: can I let the optimizer do this?
		reg = portModeRegister(port);
		m_out = portOutputRegister(port);
		m_in = portInputRegister(port);

		if (mode == INPUT) {
			uint8_t oldSREG = SREG;
	        cli();
			*reg &= ~m_bit;
			*m_in &= ~m_bit;
			SREG = oldSREG;
		} else if (mode == INPUT_PULLUP) {
			uint8_t oldSREG = SREG;
	        cli();
			*reg &= ~m_bit;
			*m_in |= m_bit;
			SREG = oldSREG;
		} else {
			uint8_t oldSREG = SREG;
	        cli();
			*reg |= m_bit;
			SREG = oldSREG;
		}
	}

public:
	enum PIN_MODE{
		MODE_INPUT = INPUT,
		MODE_OUTPUT = OUTPUT,
		MODE_INPUT_PULLUP = INPUT_PULLUP,
		MODE_UNDEF
	};

	DigitalPin(uint8_t pin, PIN_MODE mode = MODE_UNDEF){
		m_pin = pin;
		if (mode < MODE_UNDEF){
			set_mode(mode);
		}
	}

	void set_mode(PIN_MODE mode){
		pinMode(mode);
		uint8_t timer = digitalPinToTimer(m_pin);
		if (timer != NOT_ON_TIMER) turnOffPWM(timer);
	}

	void set(bool output){
		uint8_t oldSREG = SREG;
		cli();

		if (!output) {
			*m_out &= ~m_bit;
		} else {
			*m_out |= m_bit;
		}

		SREG = oldSREG;
	}

	inline void set_fast(bool output){
		if (!output) {
			*m_out &= ~m_bit;
		} else {
			*m_out |= m_bit;
		}
	}

	inline bool get(){
		if (*m_in & m_bit) return true;
		return false;
	}
};
