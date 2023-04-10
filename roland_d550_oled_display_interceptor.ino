/*
 * Arduino-based interceptor for Roland D-550 OLED display mod 
 * ===========================================================
 * Version 1.0 / 20230405 by Andrew Evdokimov i at iflyhigh ru
 * Creative Commons Attribution 4.0 International  (CC BY 4.0) 
 */

#define PCWAIT 25 // number of cycles to delay during data write
#define DDRB_MASK B00110000 // 0=input 1=output
#define DDRC_MASK B00111111
#define DDRD_MASK B00000011
/*
 * input map optimized for performance
 * LCD -> Arduino 
 * --------------
 * D0  -> D8      PINB0
 * D1  -> D9      PINB1
 * D2  -> D10     PINB2
 * D3  -> D11     PINB3
 * D4  -> D4      PIND4
 * D5  -> D5      PIND5
 * D6  -> D6      PIND6
 * D7  -> D7      PIND7
 * RS  -> D3      PIND3
 * E   -> D2      PIND2
 */
 
// iD contains data received from MCU controlling LCD
#define iD ((PINB & B00001111) | (PIND & B11110000))  
#define iRS PIND
#define iRSpin 3
#define iEpin 2
/*
 * output map optimized for performance
 * OLED-> Arduino 
 * --------------
 * D0  -> A0      PORTC0
 * D1  -> A1      PORTC1
 * D2  -> A2      PORTC2
 * D3  -> A3      PORTC3
 * D4  -> A4      PORTC4
 * D5  -> A5      PORTC5
 * D6  -> D12     PORTB4
 * D7  -> D13     PORTB5
 * RS  -> D1/TX   PORTD1
 * E   -> D0/RX   PORTD0
 */

void setup() {
  DDRB = DDRB_MASK;
  DDRC = DDRC_MASK;
  DDRD = DDRD_MASK;

  PORTB = (0x0 & DDRB_MASK);
  PORTC = (0x0 & DDRC_MASK);
  PORTD = (0x0 & DDRD_MASK);

  // MCU starts in ~70 msecs, we need to set OLED entry mode within first 300 msecs before Roland will start its own initialization
  delay(150);
  
  // set OLED entry mode no shift, increment (left to right)
  PORTC = (0x6 & DDRC_MASK);
  PORTB = (0 & DDRB_MASK);
  // RS = 0 - this is command
  PORTD = (B00000001 & DDRD_MASK);
  wt(PCWAIT);
  PORTD = (B00000000 & DDRD_MASK);
  wt(PCWAIT);

  // now start copying input to output
  attachInterrupt(digitalPinToInterrupt(iEpin), intercept, FALLING);
}

// 350 nsec per loop
volatile void wt(uint8_t loop) {
  uint8_t wi;
  for (wi = 0; wi < loop; wi++)
  {
    asm volatile("nop\n\t");
  }
}

void intercept() {
  uint8_t d = iD;
  uint8_t rs = bitRead(iRS, iRSpin);
  PORTC = (d & DDRC_MASK);
  PORTB = ((d >> 2) & DDRB_MASK);
  PORTD = ((1 | (rs << 1)) & DDRD_MASK);
  wt(PCWAIT);
  PORTD = ((0 | (rs << 1)) & DDRD_MASK);
  wt(PCWAIT);
}

void loop() {
  // everything is done in ISR
}
