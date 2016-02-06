/*
 * lightalarm.c
 *
 *  Created on: 06 february 2016 �.
 *      Author: D. Falko
 */

#include "lightalarm.h"

volatile uint8_t rec_buffer[RECEIVER_BUFFER_SIZE];
volatile uint16_t rec_buffer_it = 0;
volatile receiver_state_st reciver_state = REC_WAIT_TRANS;

 void set_color(uint8_t red, uint8_t green, uint8_t blue) {
	if(red <= 255 && green <= 255 && blue <= 255) {
		TA0CCR1 = 255 - red;
		TA1CCR1 = 255 - blue;
		TA1CCR2 = 255 - green;
	}
}

void set_color_hex(uint32_t hex) {
    // 0x00FFFFFF 0x00RRGGBB
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	red   = (uint8_t) (hex >> 16);
	green = (uint8_t) (hex >> 8);
	blue  = (uint8_t) (hex);

	set_color(red, green, blue);
}

int receiver_handler(uint8_t byte) {
	switch(reciver_state) {
	case REC_WAIT_TRANS: {
		if(byte == RECEIVER_START) {
			uint16_t i;
			rec_buffer_it = 0;
			for(i = RECEIVER_BUFFER_SIZE; i != 0 ; i--) {
				rec_buffer[i] = 0; // clean buffer
			}
			reciver_state = REC_TRANS;
		}
		break;
	}
	case REC_TRANS: {
		if(byte == RECEIVER_END) {
			//end of trans
			need_command_handle = 1;
			reciver_state = REC_END;
			return 1;
		} else if(rec_buffer_it != RECEIVER_BUFFER_SIZE) {
			rec_buffer[rec_buffer_it++] = byte;
		} else {
			// buffer overflow, reset trans
			reciver_state = REC_WAIT_TRANS;
		}
		break;
	}
	case REC_END: {
		__no_operation(); /* wait for handle command */
		break;
	}
	default: {
		/* TODO: error state, signal */
		break;
	}
	}
	return 0;
}

void handle_command(volatile uint8_t msg[], volatile uint16_t size) {
	/*TODO: write */

	// send result
	send_status(STATE_OK);
	// after all reset state
	need_command_handle = 0;
	reciver_state = REC_WAIT_TRANS;
}

inline void send_status(handle_state_st state) {
	UCA0TXBUF = state;
}