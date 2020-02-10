/*
 * Connection.c
 *
 *  Created on: 07.02.2020
 *      Author: LK
 */

#include "Connection.h"

TMC_Connection_Status TMC_Connection_getStatus(TMC_Connection *con)
{
	return con->status;
}

void TMC_Connection_setContinuous(TMC_Connection *con, uint8_t continuous)
{
	con->continuous = continuous;
}

uint8_t TMC_Connection_isContinuous(TMC_Connection *con)
{
	return con->continuous;
}
