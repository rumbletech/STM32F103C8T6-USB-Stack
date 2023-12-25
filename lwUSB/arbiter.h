/*
 * Arbiter.h
 *
 *  Created on: Jul 20, 2023
 *      Author: Garmoosh
 */

#ifndef ARBITER_H_
#define ARBITER_H_

struct Event_s * getEventbyID (uint32_t evid);

void Arbiter_Main( void );
void Arbiter_Init ( void );

#endif /* ARBITER_H_ */
