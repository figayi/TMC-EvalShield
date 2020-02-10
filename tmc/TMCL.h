/*
 * TMCL.h
 *
 *  Created on: 05.02.2020
 *      Author: LK
 */

#ifndef TMC_TMCL_H_
#define TMC_TMCL_H_

#include "Definitions.h"
#include "boards/Board.h"

#ifndef TMC_TMCL_DATAGRAM_LENGTH
#define TMC_TMCL_DATAGRAM_LENGTH 9
#endif

#define TMC_TMCL_TIMEOUT 1000 // [ms]

void tmcl_init();
void tmcl_process();

#endif /* TMC_TMCL_H_ */
