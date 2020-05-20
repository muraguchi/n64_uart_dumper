/*
main.c

Copyright (c) 2020 muraguchi 

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

// 
// Target device: ATMEGA1284p
//

// 8MHz
#define F_CPU 8000000UL

//#define TX_NOINT_MODE

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// UART baudrate :1.000Mbps
#define BAUDRATE 1000000 

// receive buffer 2^USART_RX_SZ_BITS = USART_RX_LEN
#define USART_RX_SZ_BITS 5 
#define USART_RX_LEN     32 
// transmit buffer 2^USART_TX_SZ_BITS = USART_TX_LEN
#define USART_TX_SZ_BITS 5 
#define USART_TX_LEN     32 

#define PORTADL PORTA
#define DDRADL  DDRA
#define PINADL  PINA

#define PORTADH PORTB
#define DDRADH  DDRB
#define PINADH  PINB

#define PORTMISC PORTC
#define DDRMISC  DDRC
#define PINMISC  PINC
#define MISC_S_DAT       0
#define MISC_CLK         1
#define MISC_COLD_RESETB 2


#define PORTCTRL PORTD
#define DDRCTRL  DDRD
#define PINCTRL  PIND
#define CTRL_ALE_H  4
#define CTRL_ALE_L  5
#define CTRL_READB  6
#define CTRL_WRITEB 7




void USART_init();
char USART_rx_pop(uint8_t *rd);
void USART_tx_push(uint8_t wr);
uint8_t hex_to_bin(uint8_t hex);
uint8_t nibble_to_hex(uint8_t bin);

void n64_init();
void n64_reset(uint8_t value);
uint16_t n64_single_read(uint8_t a3, uint8_t a2, uint8_t a1,uint8_t a0);
void n64_page_read(uint8_t a3,uint8_t a2);


static volatile uint8_t USART_rx_wr_ptr;
static volatile uint8_t USART_rx_rd_ptr;
static volatile uint8_t USART_rx_b[USART_RX_LEN];
static volatile uint8_t USART_tx_wr_ptr;
static volatile uint8_t USART_tx_rd_ptr;
static volatile uint8_t USART_tx_b[USART_TX_LEN];

ISR(USART0_RX_vect){
  uint8_t next_rx_wr_ptr;
  next_rx_wr_ptr = ( USART_rx_wr_ptr + 1 ) & ( USART_RX_LEN - 1 );
  // If FIFO is full, discarded.
  if (next_rx_wr_ptr==USART_rx_rd_ptr)
    return;
  // else [Aread data and progress write pointer
  else {
    USART_rx_b[USART_rx_wr_ptr]=UDR0;
    USART_rx_wr_ptr=next_rx_wr_ptr;
  }
}


#ifndef TX_NOINT_MODE
ISR(USART0_UDRE_vect){
  // If fifo is empty, disable UDR intrrupt
  if (USART_tx_wr_ptr==USART_tx_rd_ptr){
    //UCR = (1 << RXCIE) | (0<< TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) |(0<<CHR9) ;
    UCSR0B = 1 << RXCIE0 | 0 << TXCIE0 | 0<<UDRIE0 | 1<< RXEN0 | 1<< TXEN0 | 0<<UCSZ02;
  }
  // pop tx data
  else {
    UDR0 = USART_tx_b[USART_tx_rd_ptr];
    USART_tx_rd_ptr = ( USART_tx_rd_ptr + 1 ) & ( USART_TX_LEN - 1 );
  }
}
#endif

void USART_init(){
  // Stop Interrupt
  cli();

  // Init pointers
  USART_rx_wr_ptr=0;
  USART_rx_rd_ptr=0;
  USART_tx_wr_ptr=0;
  USART_tx_rd_ptr=0;
  // Init UBRR
  UBRR0H = (uint8_t)((F_CPU/8/BAUDRATE-1)>>8);
  UBRR0L = (uint8_t)((F_CPU/8/BAUDRATE-1)&0xff);
  UCSR0A = 1 << U2X0 | 0<<MPCM0;
  UCSR0B = 1 << RXCIE0 | 0 << TXCIE0 | 0<<UDRIE0 | 1<< RXEN0 | 1<< TXEN0 | 0<<UCSZ02;
  UCSR0C = 0 << UMSEL00 | 0 << UPM01  | 0<<UPM00  | 0<< USBS0 | 1<<UCSZ01|1<<UCSZ00|0<<UCPOL0;

  // Enable Interrupt
  sei();  
}

// pop rxdata
char USART_rx_pop(uint8_t * rd)
{
  // If fifo is empty, return 0
  if (USART_rx_wr_ptr==USART_rx_rd_ptr){
    return 0;
  }
  // get read data and return 1
  else {
    *rd = USART_rx_b[USART_rx_rd_ptr];
    USART_rx_rd_ptr = ( USART_rx_rd_ptr + 1 ) & ( USART_RX_LEN - 1 );
    return 1;
  }
}

// push txdata
void USART_tx_push(uint8_t wr)
{
#ifdef  TX_NOINT_MODE
    while ( ~(UCSR0A & ((1<< TXC0)|(1 << UDRE0))))
    {
      __asm__ __volatile__ ("nop");	
    }
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    __asm__ __volatile__ ("nop");
    UDR0 = wr;
#endif
#ifndef TX_NOINT_MODE
  uint8_t next_tx_wr_ptr;
  next_tx_wr_ptr = ( USART_tx_wr_ptr + 1 ) & ( USART_TX_LEN - 1 );
  // If FIFO is full,
  while (next_tx_wr_ptr==USART_tx_rd_ptr){;}

  USART_tx_b[USART_tx_wr_ptr]=wr;
  USART_tx_wr_ptr = next_tx_wr_ptr;
  // Enable empty interrupt
  //UCR = (1 << RXCIE) | (0<< TXCIE) | (1<<UDRIE) | (1<<RXEN) | (1<<TXEN) |(0<<CHR9) ;
  UCSR0B = 1 << RXCIE0 | 0 << TXCIE0 | 1<<UDRIE0 | 1<< RXEN0 | 1<< TXEN0 | 0<<UCSZ02;
#endif

}

uint8_t hex_to_bin(uint8_t hex){
  if (hex>='0' && hex <='9')
    return hex - '0';
  else if (hex>='A' && hex <='F')
    return (hex - 'A') + 10;
  else if (hex>='a' && hex <='f')
    return (hex - 'a') + 10;
  else
    return 0;
}
uint8_t nibble_to_hex(uint8_t bin){
  if (bin<10)
    return '0'+bin;
  else if (bin<16)
    return 'A'-10+bin;
  else
    return 0;
}

void n64_init(){
  // AD7-0
  PORTADL = 0xff;
  DDRADL  = 0x00;
  // AD15-8
  PORTADH = 0xff;
  DDRADH  = 0x00;

  // S_DAT(0) input, CLK(1) out, COLD_RESET(2) output
  PORTMISC = 0x00;
  DDRMISC  = (1<<MISC_COLD_RESETB) | (1<<MISC_CLK) | (0<<MISC_S_DAT);

  // CTRL
  PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (1<<CTRL_ALE_H) ;
  DDRCTRL  = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (1<<CTRL_ALE_H) ;
}

void n64_reset(uint8_t value){
  if(value){
    PORTMISC |=  (1<<MISC_COLD_RESETB);
  } else {
    PORTMISC &= ~(1<<MISC_COLD_RESETB);
  }
}

uint16_t n64_single_read(uint8_t a3, uint8_t a2, uint8_t a1,uint8_t a0){
  uint16_t retval;

  // IDLE 
  PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (1<<CTRL_ALE_H) ;

  // Output ADDRESS hi word
  PORTADH  =a3;
  PORTADL  =a2;
  DDRADH   =0xff;
  DDRADL   =0xff;
  __asm__ __volatile__ ("nop");
  // ALE_H low
  PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;

  // Output ADDRESS low word
  PORTADH  =a1;
  PORTADL  =a0;
  __asm__ __volatile__ ("nop");
  // ALE_L low
  PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (0<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;

  // 1us
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");

  // input
  DDRADH   =0x00;
  DDRADL   =0x00;

  // READB low
  PORTCTRL = (1<<CTRL_WRITEB) | (0<<CTRL_READB) | (0<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;

  // 300us
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  retval = ((PINADH<<8)|PINADL);

  PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (1<<CTRL_ALE_H) ;

  return retval;
}

// 1Mbit 0x20000 page read
void n64_page_read(uint8_t a3,uint8_t a2){
  uint8_t a2_index;

  for (uint8_t a2_add=0;a2_add<2;a2_add++){
    a2_index = (a2&0xfe) + a2_add;
    for (uint8_t a1_index=0; a1_index<0x80;a1_index++){
      // IDLE 
      PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (1<<CTRL_ALE_H) ;
      
      // Output ADDRESS hi word
      PORTADH  =a3;
      PORTADL  =a2_index;
      DDRADH   =0xff;
      DDRADL   =0xff;
      __asm__ __volatile__ ("nop");
      // ALE_H low
      PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;
      
      // Output ADDRESS low word
      PORTADH  =a1_index<<1;
      PORTADL  =0x00;
      __asm__ __volatile__ ("nop");
      // ALE_L low
      PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (0<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;
      
      
      // 1us
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      __asm__ __volatile__ ("nop");
      
      // input
      DDRADH   =0x00;
      DDRADL   =0x00;
      
      
      for(int l=0;l<256;l++){
	// READB low
	PORTCTRL = (1<<CTRL_WRITEB) | (0<<CTRL_READB) | (0<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;
	// 300us
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");
	
	//USART_tx_push(nibble_to_hex((PINADL>>4)&0xf));
	//USART_tx_push(nibble_to_hex((PINADL   )&0xf));
	//USART_tx_push(nibble_to_hex((PINADH>>4)&0xf));
	//USART_tx_push(nibble_to_hex((PINADH   )&0xf));
	USART_tx_push(PINADL);
	USART_tx_push(PINADH);
	
	// READB high
	PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (0<<CTRL_ALE_L) | (0<<CTRL_ALE_H) ;
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");
      }
    }
  }
  // IDLE
  PORTCTRL = (1<<CTRL_WRITEB) | (1<<CTRL_READB) | (1<<CTRL_ALE_L) | (1<<CTRL_ALE_H) ;
}

void main(){

  // RX data
  uint8_t rxd;
  // state
  
  uint8_t  state;
  uint8_t  address[4];
  uint16_t rw_data; // used read write data buffer

  unsigned int  address_pgrd;

  // assert reset
  // Wait 100ms
  for(int l=0;l<100;l++) {
    _delay_ms(1);
  }
  // deassert reset

  USART_init();
  n64_init();
  state =0;
  while(1)
    {
      while (USART_rx_pop(&rxd)) {
	switch (state) {
	case 0:
	  if(rxd=='X') {
	    state=1;
	  }
	  if(rxd=='S'){
	    state=10;
	  }
	  if(rxd=='P'){
	    state=12;
	  }
	  break;
	case 1:
	  n64_reset(hex_to_bin(rxd)&0x1);
	  USART_tx_push(nibble_to_hex(hex_to_bin(rxd)&0x1));
	  state=0;
	  break;
	case 10:
	  if(rxd=='R'){
	    state=20;
	  } else {
	    state=0;
	  }
	  break;
	case 12:
	  if(rxd=='R'){
	    state=30;
	  } else{
	    state=0;
	  }
	  break;
	case 20:
	  address[3]  = hex_to_bin(rxd)<<4;
	  state = 21;
	  break;
	case 21:
	  address[3] |= hex_to_bin(rxd);
	  state = 22;
	  break;
	case 22:
	  address[2]  = hex_to_bin(rxd)<<4;
	  state = 23;
	  break;
	case 23:
	  address[2] |= hex_to_bin(rxd);
	  state = 24;
	  break;
	case 24:
	  address[1]  = hex_to_bin(rxd)<<4;
	  state = 25;
	  break;
	case 25:
	  address[1] |= hex_to_bin(rxd);
	  state = 26;
	  break;
	case 26:
	  address[0]  = hex_to_bin(rxd)<<4;
	  state = 27;
	  break;
	case 27:
	  address[0] |= hex_to_bin(rxd);
	  rw_data = n64_single_read(address[3],address[2],address[1],address[0]);
	  USART_tx_push(nibble_to_hex((rw_data>>12)&0xf));
	  USART_tx_push(nibble_to_hex((rw_data>> 8)&0xf));
	  USART_tx_push(nibble_to_hex((rw_data>> 4)&0xf));
	  USART_tx_push(nibble_to_hex(      rw_data&0xf));
	  state = 0;
	  break;

	case 30:
	  address[3]  = hex_to_bin(rxd)<<4;
	  state = 31;
	  break;
	case 31:
	  address[3] |= hex_to_bin(rxd);
	  state = 32;
	  break;
	case 32:
	  address[2]  = hex_to_bin(rxd)<<4;
	  state = 33;
	  break;
	case 33:
	  address[2] |= hex_to_bin(rxd);
	  n64_page_read(address[3],address[2]);
	  state = 0;
	  break;
	}
      } // RX POP
    } // INF
}
