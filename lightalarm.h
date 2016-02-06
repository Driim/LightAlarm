/*
 * lightalarm.h
 *
 *  Created on: 06 february 2016 �.
 *      Author: D.Falko
 */

#ifndef LIGHTALARM_H_
#define LIGHTALARM_H_

#include <msp430.h>

/* Defines */
#define PWM_PERIOD           (256) /* rgb in hex max 255 */
#define RECEIVER_BUFFER_SIZE (30)
#define RECEIVER_START       (0xEF)
#define RECEIVER_END         (0xFE)

/* Types */
typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;

enum _receiver_state_st {
	REC_WAIT_TRANS = 0,
	REC_TRANS = 1,
	REC_END = 2,
};
typedef enum _receiver_state_st receiver_state_st;

enum _handle_state_st {
	STATE_OK = 0x59,
	STATE_ERROR = 0xEE,
};
typedef enum _handle_state_st handle_state_st;

extern volatile uint16_t need_command_handle;
extern volatile uint8_t rec_buffer[];
extern volatile uint16_t rec_buffer_it;

/* Functions */
/* Color */
void set_color(uint8_t red, uint8_t green, uint8_t blue);
void set_color_hex(uint32_t hex);

/*
 * Receiver functions:
 * 	 - [1byte] - start 0xEF
 * 	 - [1byte] - end 0xFE
 */
int receiver_handler(uint8_t byte);
void handle_command(volatile uint8_t msg[], volatile uint16_t size);

/* Sender */
inline void send_status(handle_state_st state);

#endif /* LIGHTALARM_H_ */
