/*
 * Board.h
 *
 *  Created on: 07.02.2020
 *      Author: LK
 */

#ifndef TMC_BOARDS_BOARD_H_
#define TMC_BOARDS_BOARD_H_

#include "Definitions.h"
#include "tmc/helpers/API_Header.h"

#ifndef TMC_CFLAGS

////////////////////////////////////////////////////////////////////////////////
// User defines

#define TMC_AXES_COUNT   3
#define TMC_BOARD_COUNT  TMC_AXES_COUNT
#define BOARD_0          ID_TMC5160
#define BOARD_1          ID_NONE
#define BOARD_2          ID_NONE

////////////////////////////////////////////////////////////////////////////////

#endif

typedef enum {
	DRIVER_DISABLE,
	DRIVER_ENABLE,
	DRIVER_USE_GLOBAL_ENABLE
} DriverState;

typedef enum {
	TMC_ERROR_TYPE = 0x04,
	TMC_ERROR_ADDRESS = 0x04,
	TMC_ERROR_NOT_DONE = 0x20
} TMC_Error;

typedef struct tmc_board_t {
	void *type;
	uint16_t id;
	uint8_t axis;
	uint8_t alive;
	uint32_t errors;
	uint32_t VMMax;
	uint32_t VMMin;
	unsigned char numberOfMotors;
	ConfigurationTypeDef *config;
	uint32_t (*left)                (uint8_t motor, int32_t velocity);            // move left with velocity <velocity>
	uint32_t (*right)               (uint8_t motor, int32_t velocity);            // move right with velocity <velocity>
	uint32_t (*rotate)              (uint8_t motor, int32_t velocity);            // move right with velocity <velocity>
	uint32_t (*stop)                (uint8_t motor);                            // stop motor
	uint32_t (*moveTo)              (uint8_t motor, int32_t position);            // move to position <position>
	uint32_t (*moveBy)              (uint8_t motor, int32_t *ticks);              // move by <ticks>, changes ticks to absolute target
	uint32_t (*moveProfile)         (uint8_t motor, int32_t position);            // move profile <position>
	uint32_t (*SAP)                 (uint8_t type, uint8_t motor, int32_t value);   // set axis parameter -> TMCL conformance
	uint32_t (*GAP)                 (uint8_t type, uint8_t motor, int32_t *value);  // get axis parameter -> TMCL conformance
	uint32_t (*STAP)                (uint8_t type, uint8_t motor, int32_t value);   // store axis parameter -> TMCL conformance
	uint32_t (*RSAP)                (uint8_t type, uint8_t motor, int32_t value);   // restore axis parameter -> TMCL conformance
	void (*readRegister)          (uint8_t motor, uint8_t address, int32_t *value);  // Motor needed since some chips utilize it as a switch between low and high values
	void (*writeRegister)         (uint8_t motor, uint8_t address, int32_t value);   // Motor needed since some chips utilize it as a switch between low and high values
	uint32_t (*getMeasuredSpeed)    (uint8_t motor, int32_t *value);
	uint32_t (*userFunction)        (uint8_t type, uint8_t motor, int32_t *value);

	void (*periodicJob)           (uint32_t tick);
	void (*deInit)                (void);

	void (*checkErrors)           (uint32_t tick);
	void (*enableDriver)          (DriverState state);

	uint8_t (*cover)                (uint8_t data, uint8_t lastTransfer);
	void  (*fullCover)            (uint8_t *data, size_t length);

	uint32_t (*getMin)              (uint8_t type, uint8_t motor, int32_t *value);
	uint32_t (*getMax)              (uint8_t type, uint8_t motor, int32_t *value);

	DriverState driverState;
} TMC_Board;

TMC_Board board[TMC_AXES_COUNT];

void Boards_init(void);
void Board_init(TMC_Board *board);
void board_setDummyFunctions(TMC_Board *board);

#endif /* TMC_BOARDS_BOARD_H_ */
