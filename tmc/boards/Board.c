/*
 * Board.c
 *
 *  Created on: 07.02.2020
 *      Author: LK
 */

#include "Board.h"

extern void TMC5160_init(TMC_Board *board);

void Board_init(TMC_Board *board)
{
#ifdef BOARD
#ifdef TMC5160_shield
	//TMC5160_init(board);
	board_setDummyFunctions(board);
#endif
#else
	board_setDummyFunctions(board);
#endif
}

static void deInit(void) {}

// Evalboard channel function dummies
static uint32_t dummy_Motor(uint8_t motor)
{
	UNUSED(motor);
	return TMC_ERROR_FUNCTION;
}

static uint32_t dummy_MotorValue(uint8_t motor, int32_t value)
{
	UNUSED(motor);
	UNUSED(value);
	return TMC_ERROR_FUNCTION;
}

static void dummy_AddressRef(uint8_t motor, uint8_t address, int32_t *value)
{
	UNUSED(motor);
	UNUSED(address);
	UNUSED(value);
}

static void dummy_AddressValue(uint8_t motor, uint8_t address, int32_t value)
{
	UNUSED(motor);
	UNUSED(address);
	UNUSED(value);
}

static uint32_t dummy_MotorRef(uint8_t motor, int32_t *value)
{
	UNUSED(motor);
	UNUSED(value);
	return TMC_ERROR_FUNCTION;
}

static uint32_t dummy_TypeMotorValue(uint8_t type, uint8_t motor, int32_t value)
{
	UNUSED(type);
	UNUSED(motor);
	UNUSED(value);
	return TMC_ERROR_FUNCTION;
}

static uint32_t dummy_TypeMotorRef(uint8_t type, uint8_t motor, int32_t *value)
{
	UNUSED(type);
	UNUSED(motor);
	UNUSED(value);
	return TMC_ERROR_FUNCTION;
}

static uint32_t dummy_getLimit(uint8_t type, uint8_t motor, int32_t *value)
{
	UNUSED(type);
	UNUSED(motor);
	UNUSED(value);
	return TMC_ERROR_FUNCTION;
}

static uint8_t delegationReturn(void)
{
	return 1;
}

static void enableDriver(DriverState state)
{
	UNUSED(state);
}

static void periodicJob(uint32_t tick)
{
	UNUSED(tick);
}

void board_setDummyFunctions(TMC_Board *board)
{
	board->config->reset     = delegationReturn;
	board->config->restore   = delegationReturn;

	board->deInit            = deInit;
	board->periodicJob       = periodicJob;
	board->left              = dummy_MotorValue;
	board->stop              = dummy_Motor;
	board->moveTo            = dummy_MotorValue;
	board->moveBy            = dummy_MotorRef;
	board->moveProfile       = dummy_MotorValue;
	board->right             = dummy_MotorValue;
	board->GAP               = dummy_TypeMotorRef;
	board->readRegister      = dummy_AddressRef;
	board->writeRegister     = dummy_AddressValue;
	board->SAP               = dummy_TypeMotorValue;
	board->STAP              = dummy_TypeMotorValue;
	board->RSAP              = dummy_TypeMotorValue;
	board->userFunction      = dummy_TypeMotorRef;
	board->getMeasuredSpeed  = dummy_MotorRef;
	board->checkErrors       = periodicJob;
	board->enableDriver      = enableDriver;

	board->fullCover         = NULL;
	board->getMin            = dummy_getLimit;
	board->getMax            = dummy_getLimit;
}

void periodicJobDummy(uint32_t tick)
{
	UNUSED(tick);
}
