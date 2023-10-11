/* This is truncated down to feed to chat GPT
 */ 

#include "light_ws2812.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void ws2812_sendarray_mask(uint8_t *, uint16_t , uint8_t);

void ws2812_sendarray(uint8_t *data,uint16_t datlen)
{
	ws2812_sendarray_mask(data,datlen,_BV(ws2812_pin));
}

void ws2812_sendarray_single(uint16_t N, uint8_t R, uint8_t G, uint8_t B)//, uint16_t datlen)
{
	ws2812_sendarray_mask_single(N, R, G, B,_BV(ws2812_pin));
}

void ws2812_sendarray_mask(uint8_t *data,uint16_t datlen,uint8_t maskhi)
{
	uint8_t curbyte,ctr,masklo;
	masklo	=~maskhi&ws2812_port;
	maskhi |=ws2812_port;
	
	while (datlen--) {
		curbyte=*data++;
		
		asm volatile(
		
		"		ldi	%0,8		\n\t"		// 0
		"loop%=:out	%2,	%3		\n\t"		// 1
		"		lsl	%1			\n\t"		// 2
		"		dec	%0			\n\t"		// 3

		"		rjmp .+0		\n\t"		// 5
		
		"		brcs .+2		\n\t"		// 6l / 7h
		"		out	%2,	%4		\n\t"		// 7l / -

		"		rjmp .+0		\n\t"		// 9
		
		"		nop				\n\t"		// 10
		"		out	%2,	%4		\n\t"		// 11
		"		breq end%=		\n\t"		// 12      nt. 13 taken

		"		rjmp .+0		\n\t"		// 14
		"		rjmp .+0		\n\t"		// 16
		"		rjmp .+0		\n\t"		// 18
		"		rjmp loop%=		\n\t"		// 20
		"end%=:					\n\t"
		:	"=&d" (ctr)
		:	"r" (curbyte), "I" (_SFR_IO_ADDR(ws2812_port)), "r" (maskhi), "r" (masklo)
		);
	}
}

void ws2812_sendarray_mask_single(uint16_t N, uint8_t R, uint8_t G, uint8_t B, uint8_t maskhi)
{
    uint8_t curbyte,ctr,masklo;
    masklo  =~maskhi&ws2812_port;
    maskhi |=ws2812_port;

    // Assuming the order is GRB for a single WS2812 LED, typical for many WS2812 LEDs.
    uint8_t colors[3] = {G, R, B};

    for(uint16_t i = 0; i < N; i++) {
        for(uint8_t j = 0; j < 3; j++) { // For each of the R, G, B values
            curbyte = colors[j];

            asm volatile(
                "       ldi %0,8        \n\t"
                "loop%=:out %2, %3      \n\t"
                "       lsl %1          \n\t"
                "       dec %0          \n\t"
                "       rjmp .+0        \n\t"
                "       brcs .+2        \n\t"
                "       out %2, %4      \n\t"
                "       rjmp .+0        \n\t"
                "       nop             \n\t"
                "       out %2, %4      \n\t"
                "       breq end%=      \n\t"
                "       rjmp .+0        \n\t"
                "       rjmp .+0        \n\t"
                "       rjmp .+0        \n\t"
                "       rjmp loop%=     \n\t"
                "end%=:                 \n\t"
                :   "=&d" (ctr)
                :   "r" (curbyte), "I" (_SFR_IO_ADDR(ws2812_port)), "r" (maskhi), "r" (masklo)
            );
        }
    }
}
