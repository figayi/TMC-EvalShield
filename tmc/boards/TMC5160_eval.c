/*
 * TMC5160_eval.c
 *
 *  Created on: 07.02.2020
 *      Author: LK
 */

#include "Board.h"
#include "tmc/ic/TMC5160/TMC5160.h"
#include "Definitions.h"
#include "SPI.h"

#define ERRORS_VM        (1<<0)
#define ERRORS_VM_UNDER  (1<<1)
#define ERRORS_VM_OVER   (1<<2)

#define VM_MIN         50   // VM[V/10] min
#define VM_MAX         660  // VM[V/10] max

#define DEFAULT_MOTOR  0

#define TIMEOUT 1000
#define TMC5160_READ 0
#define TMC5160_WRITE 1

static uint8_t vMaxModified = 0;
//static uint32_t vMax		   = 1;

static uint32_t right(uint8_t motor, int32_t velocity);
static uint32_t left(uint8_t motor, int32_t velocity);
static uint32_t rotate(uint8_t motor, int32_t velocity);
static uint32_t stop(uint8_t motor);
static uint32_t moveTo(uint8_t motor, int32_t position);
static uint32_t moveBy(uint8_t motor, int32_t *ticks);
static uint32_t GAP(uint8_t type, uint8_t motor, int32_t *value);
static uint32_t SAP(uint8_t type, uint8_t motor, int32_t value);
static void readRegister(uint8_t motor, uint8_t address, int32_t *value);
static void writeRegister(uint8_t motor, uint8_t address, int32_t value);
static uint32_t getMeasuredSpeed(uint8_t motor, int32_t *value);

void tmc5160_writeDatagram(uint8_t motor, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
void tmc5160_writeInt(uint8_t motor, uint8_t address, int value);
int tmc5160_readInt(uint8_t motor, uint8_t address);

static void periodicJob(uint32_t tick);
static void checkErrors(uint32_t tick);
static void deInit(void);
static uint32_t userFunction(uint8_t type, uint8_t motor, int32_t *value);

static uint8_t reset();
static void enableDriver(DriverState state);

static TMC5160TypeDef TMC5160[TMC_AXES_COUNT];

void tmc5160_writeDatagram(uint8_t motor, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	if(motor >= TMC_AXES_COUNT)
		return;

	address = TMC_ADDRESS(address);
	uint8_t data[5] = { address|0x80, x1, x2, x3, x4 };
	TMC_SPI_Channel[motor].con.txRequest(&data[0], 5, TIMEOUT);
}

void tmc5160_writeInt(uint8_t motor, uint8_t address, int value)
{
	if(motor >= TMC_AXES_COUNT)
		return;

	tmc5160_writeDatagram(motor, address, 0xFF & (value>>24), 0xFF & (value>>16), 0xFF & (value>>8), 0xFF & (value>>0));
}

int tmc5160_readInt(uint8_t motor, uint8_t address)
{
	if(motor >= TMC_AXES_COUNT)
		return -1;

	address = TMC_ADDRESS(address);

	// Register not readable -> shadow register copy
	if(!TMC_IS_READABLE(TMC5160[motor].registerAccess[address]))
		return board[motor].config->shadowRegister[address];

	uint8_t data_tx[5] = { address, 0, 0, 0, 0 };
	uint8_t data_rx[5] = { 0, 0, 0, 0, 0 };
	TMC_SPI_Channel[motor].con.txrx(&data_tx[0], &data_rx[0], 5, TIMEOUT);

	return _8_32(data_rx[0], data_rx[1], data_rx[2], data_rx[3]);
}

static uint32_t rotate(uint8_t motor, int32_t velocity)
{
	if(motor >= TMC_AXES_COUNT)
		return TMC_ERROR_MOTOR;

	vMaxModified = 1;

	// set absolute velocity, independant from direction
	tmc5160_writeInt(motor, TMC5160_VMAX, abs(velocity));

	// signdedness defines velocity mode direction bit in rampmode register
	tmc5160_writeDatagram(motor, TMC5160_RAMPMODE, 0, 0, 0, (velocity >= 0)? 1 : 2);

	return TMC_ERROR_NONE;
}

static uint32_t right(uint8_t motor, int32_t velocity)
{
	return rotate(motor, velocity);
}

static uint32_t left(uint8_t motor, int32_t velocity)
{
	return rotate(motor, -velocity);
}

static uint32_t stop(uint8_t motor)
{
	return rotate(motor, 0);
}

static uint32_t moveTo(uint8_t motor, int32_t position)
{
	if(motor >= TMC_AXES_COUNT)
		return TMC_ERROR_MOTOR;

	if(vMaxModified)
	{
		tmc5160_writeInt(motor, TMC5160_VMAX, board[motor].config->shadowRegister[TMC5160_VMAX]);
		vMaxModified = 0;
	}

	// set position
	tmc5160_writeInt(motor, TMC5160_XTARGET, position);

	// change to positioning mode
	tmc5160_writeDatagram(motor, TMC5160_RAMPMODE, 0, 0, 0, 0);

	return TMC_ERROR_NONE;
}

static uint32_t moveBy(uint8_t motor, int32_t *ticks)
{
	// determine actual position and add numbers of ticks to move
	*ticks = tmc5160_readInt(motor, TMC5160_XACTUAL) + *ticks;

	return moveTo(motor, *ticks);
}

static uint32_t handleParameter(uint8_t readWrite, uint8_t motor, uint8_t type, int32_t *value)
{
	uint32_t buffer;
	uint32_t errors = TMC_ERROR_NONE;

	if(motor >= TMC_AXES_COUNT)
		return TMC_ERROR_MOTOR;

	switch(type)
	{
	case 0:
		// Target position
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_XTARGET);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_XTARGET, *value);
		}
		break;
	case 1:
		// Actual position
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_XACTUAL);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_XACTUAL, *value);
		}
		break;
	case 2:
		// Target speed
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_VMAX);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_VMAX, abs(*value));
			vMaxModified = 1;
		}
		break;
	case 3:
		// Actual speed
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_VACTUAL);
			*value = CAST_Sn_TO_S32(*value, 24);
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 4:
		// Maximum speed
		if(readWrite == TMC5160_READ) {
			*value = board[motor].config->shadowRegister[TMC5160_VMAX];
		} else if(readWrite == TMC5160_WRITE) {
			board[motor].config->shadowRegister[TMC5160_VMAX] = abs(*value);
			if(tmc5160_readInt(motor, TMC5160_RAMPMODE) == TMC5160_MODE_POSITION)
				tmc5160_writeInt(motor, TMC5160_VMAX, abs(*value));
		}
		break;
	case 5:
		// Maximum acceleration
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_AMAX);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_AMAX, *value);
		}
		break;
	case 6:
		// Maximum current
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_IHOLD_IRUN, TMC5160_IRUN_MASK, TMC5160_IRUN_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_IHOLD_IRUN, TMC5160_IRUN_MASK, TMC5160_IRUN_SHIFT, *value);
		}
		break;
	case 7:
		// Standby current
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_IHOLD_IRUN, TMC5160_IHOLD_MASK, TMC5160_IHOLD_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_IHOLD_IRUN, TMC5160_IHOLD_MASK, TMC5160_IHOLD_SHIFT, *value);
		}
		break;
	case 8:
		// Position reached flag
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_RAMPSTAT, TMC5160_POSITION_REACHED_MASK, TMC5160_POSITION_REACHED_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 10:
		// Right endstop
		if(readWrite == TMC5160_READ) {
			*value = !TMC5160_FIELD_READ(motor, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_R_MASK, TMC5160_STATUS_STOP_R_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 11:
		// Left endstop
		if(readWrite == TMC5160_READ) {
			*value = !TMC5160_FIELD_READ(motor, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 12:
		// Automatic right stop
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_SWMODE, TMC5160_STOP_R_ENABLE_MASK, TMC5160_STOP_R_ENABLE_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_SWMODE, TMC5160_STOP_R_ENABLE_MASK, TMC5160_STOP_R_ENABLE_SHIFT, *value);
		}
		break;
	case 13:
		// Automatic left stop
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_SWMODE, TMC5160_STOP_L_ENABLE_MASK, TMC5160_STOP_L_ENABLE_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_SWMODE, TMC5160_STOP_L_ENABLE_MASK, TMC5160_STOP_L_ENABLE_SHIFT, *value);
		}
		break;
	case 14:
		// SW_MODE Register
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_SWMODE);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_SWMODE, *value);
		}
		break;
	case 15:
		// Acceleration A1
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_A1);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_A1, *value);
		}
		break;
	case 16:
		// Velocity V1
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_V1);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_V1, *value);
		}
		break;
	case 17:
		// Maximum Deceleration
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_DMAX);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_DMAX, *value);
		}
		break;
	case 18:
		// Deceleration D1
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_D1);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_D1, *value);
		}
		break;
	case 19:
		// Velocity VSTART
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_VSTART);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_VSTART, *value);
		}
		break;
	case 20:
		// Velocity VSTOP
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_VSTOP);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_VSTOP, *value);
		}
		break;
	case 21:
		// Waiting time after ramp down
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_TZEROWAIT);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_TZEROWAIT, *value);
		}
		break;
	case 23:
		// Speed threshold for high speed mode
		if(readWrite == TMC5160_READ) {
			buffer = tmc5160_readInt(motor, TMC5160_THIGH);
			*value = MIN(0xFFFFF, (1 << 24) / ((buffer)? buffer : 1));
		} else if(readWrite == TMC5160_WRITE) {
			*value = MIN(0xFFFFF, (1 << 24) / ((*value)? *value:1));
			tmc5160_writeInt(motor, TMC5160_THIGH, *value);
		}
		break;
	case 24:
		// Minimum speed for switching to dcStep
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_VDCMIN);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_VDCMIN, *value);
		}
		break;
	case 27:
		// High speed chopper mode
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_VHIGHCHM_MASK, TMC5160_VHIGHCHM_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_VHIGHCHM_MASK, TMC5160_VHIGHCHM_SHIFT, *value);
		}
		break;
	case 28:
		// High speed fullstep mode
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_VHIGHFS_MASK, TMC5160_VHIGHFS_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_VHIGHFS_MASK, TMC5160_VHIGHFS_SHIFT, *value);
		}
		break;
	case 29:
		// Measured Speed
		if(readWrite == TMC5160_READ) {
			*value = TMC5160[motor].velocity;
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 33:
		// Analog I Scale
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_GCONF, TMC5160_RECALIBRATE_MASK, TMC5160_RECALIBRATE_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_GCONF, TMC5160_RECALIBRATE_MASK, TMC5160_RECALIBRATE_SHIFT, *value);
		}
		break;
	case 34:
		// Internal RSense
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_GCONF, TMC5160_REFR_DIR_MASK, TMC5160_REFR_DIR_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_GCONF, TMC5160_REFR_DIR_MASK, TMC5160_REFR_DIR_SHIFT, *value);
		}
		break;
	case 140:
		// Microstep Resolution
		if(readWrite == TMC5160_READ) {
			*value = 0x100 >> TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			switch(*value)
			{
				case 1:    *value = 8;   break;
				case 2:    *value = 7;   break;
				case 4:    *value = 6;   break;
				case 8:    *value = 5;   break;
				case 16:   *value = 4;   break;
				case 32:   *value = 3;   break;
				case 64:   *value = 2;   break;
				case 128:  *value = 1;   break;
				case 256:  *value = 0;   break;
				default:   *value = -1;  break;
			}

			if(*value != -1)
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT, *value);
			}
			else
			{
				errors |= TMC_ERROR_VALUE;
			}
		}
		break;
	case 162:
		// Chopper blank time
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_TBL_MASK, TMC5160_TBL_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_TBL_MASK, TMC5160_TBL_SHIFT, *value);
		}
		break;
	case 163:
		// Constant TOff Mode
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_CHM_MASK, TMC5160_CHM_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_CHM_MASK, TMC5160_CHM_SHIFT, *value);
		}
		break;
	case 164:
		// Disable fast decay comparator
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_DISFDCC_MASK, TMC5160_DISFDCC_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_DISFDCC_MASK, TMC5160_DISFDCC_SHIFT, *value);
		}
		break;
	case 165:
		// Chopper hysteresis end / fast decay time
		buffer = tmc5160_readInt(motor, TMC5160_CHOPCONF);
		if(readWrite == TMC5160_READ) {
			if(buffer & (1 << TMC5160_CHM_SHIFT))
			{
				*value = (buffer >> TMC5160_HEND_SHIFT) & TMC5160_HEND_MASK;
			}
			else
			{
				*value = (tmc5160_readInt(motor, TMC5160_CHOPCONF) >> TMC5160_TFD_ALL_SHIFT) & TMC5160_TFD_ALL_MASK;
				if(buffer & TMC5160_TFD_3_SHIFT)
					*value |= 1<<3; // MSB wird zu value dazugefügt
			}
		} else if(readWrite == TMC5160_WRITE) {
			if(tmc5160_readInt(motor, TMC5160_CHOPCONF) & (1<<14))
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_HEND_MASK, TMC5160_HEND_SHIFT, *value);
			}
			else
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_TFD_3_MASK, TMC5160_TFD_3_SHIFT, (*value & (1<<3))); // MSB wird zu value dazugefügt
				TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_TFD_ALL_MASK, TMC5160_TFD_ALL_SHIFT, *value);
			}
		}
		break;
	case 166:
		// Chopper hysteresis start / sine wave offset
		buffer = tmc5160_readInt(motor, TMC5160_CHOPCONF);
		if(readWrite == TMC5160_READ) {
			if(buffer & (1 << TMC5160_CHM_SHIFT))
			{
				*value = (buffer >> TMC5160_HSTRT_SHIFT) & TMC5160_HSTRT_MASK;
			}
			else
			{
				*value = (buffer >> TMC5160_OFFSET_SHIFT) & TMC5160_OFFSET_MASK;
				if(buffer & (1 << TMC5160_TFD_3_SHIFT))
					*value |= 1<<3; // MSB wird zu value dazugefügt
			}
		} else if(readWrite == TMC5160_WRITE) {
			if(buffer & (1 << TMC5160_CHM_SHIFT))
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_HSTRT_MASK, TMC5160_HSTRT_SHIFT, *value);
			}
			else
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_OFFSET_MASK, TMC5160_OFFSET_SHIFT, *value);
			}
		}
		break;
	case 167:
		// Chopper off time
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_TOFF_MASK, TMC5160_TOFF_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_TOFF_MASK, TMC5160_TOFF_SHIFT, *value);
		}
		break;
	case 168:
		// smartEnergy current minimum (SEIMIN)
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SEIMIN_MASK, TMC5160_SEIMIN_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SEIMIN_MASK, TMC5160_SEIMIN_SHIFT, *value);
		}
		break;
	case 169:
		// smartEnergy current down step
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SEDN_MASK, TMC5160_SEDN_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SEDN_MASK, TMC5160_SEDN_SHIFT, *value);
		}
		break;
	case 170:
		// smartEnergy hysteresis
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SEMAX_MASK, TMC5160_SEMAX_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SEMAX_MASK, TMC5160_SEMAX_SHIFT, *value);
		}
		break;
	case 171:
		// smartEnergy current up step
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SEUP_MASK, TMC5160_SEUP_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SEUP_MASK, TMC5160_SEUP_SHIFT, *value);
		}
		break;
	case 172:
		// smartEnergy hysteresis start
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SEMIN_MASK, TMC5160_SEMIN_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SEMIN_MASK, TMC5160_SEMIN_SHIFT, *value);
		}
		break;
	case 173:
		// stallGuard2 filter enable
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SFILT_MASK, TMC5160_SFILT_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SFILT_MASK, TMC5160_SFILT_SHIFT, *value);
		}
		break;
	case 174:
		// stallGuard2 threshold
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_COOLCONF, TMC5160_SGT_MASK, TMC5160_SGT_SHIFT);
			*value = CAST_Sn_TO_S32(*value, 7);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_COOLCONF, TMC5160_SGT_MASK, TMC5160_SGT_SHIFT, *value);
		}
		break;
	case 180:
		// smartEnergy actual current
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_DRVSTATUS, TMC5160_CS_ACTUAL_MASK, TMC5160_CS_ACTUAL_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 181:
		// smartEnergy stall velocity
		//this function sort of doubles with 182 but is necessary to allow cross chip compliance
		if(readWrite == TMC5160_READ) {
			if(TMC5160_FIELD_READ(motor, TMC5160_SWMODE, TMC5160_SG_STOP_MASK, TMC5160_SG_STOP_SHIFT))
			{
				buffer = tmc5160_readInt(motor, TMC5160_TCOOLTHRS);
				*value = MIN(0xFFFFF, (1<<24) / ((buffer)? buffer:1));
			}
			else
			{
				*value = 0;
			}
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_SWMODE, TMC5160_SG_STOP_MASK, TMC5160_SG_STOP_SHIFT, (*value)? 1:0);

			*value = MIN(0xFFFFF, (1<<24) / ((*value)? *value:1));
			tmc5160_writeInt(motor, TMC5160_TCOOLTHRS, *value);
		}
		break;
	case 182:
		// smartEnergy threshold speed
		if(readWrite == TMC5160_READ) {
			buffer = tmc5160_readInt(motor, TMC5160_TCOOLTHRS);
			*value = MIN(0xFFFFF, (1<<24) / ((buffer)? buffer:1));
		} else if(readWrite == TMC5160_WRITE) {
			*value = MIN(0xFFFFF, (1<<24) / ((*value)? *value:1));
			tmc5160_writeInt(motor, TMC5160_TCOOLTHRS, *value);
		}
		break;
	case 184:
		// Random TOff mode
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_CHOPCONF, TMC5160_RNDTF_MASK, TMC5160_RNDTF_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_CHOPCONF, TMC5160_RNDTF_MASK, TMC5160_RNDTF_SHIFT, *value);
		}
		break;
	case 185:
		// Chopper synchronization
		if(readWrite == TMC5160_READ) {
			*value = (tmc5160_readInt(motor, TMC5160_CHOPCONF) >> 20) & 0x0F;
		} else if(readWrite == TMC5160_WRITE) {
			buffer = tmc5160_readInt(motor, TMC5160_CHOPCONF);
			buffer &= ~(0x0F<<20);
			buffer |= (*value & 0x0F) << 20;
			tmc5160_writeInt(motor, TMC5160_CHOPCONF,buffer);
		}
		break;
	case 186:
		// PWM threshold speed
		if(readWrite == TMC5160_READ) {
			buffer = tmc5160_readInt(motor, TMC5160_TPWMTHRS);
			*value = MIN(0xFFFFF, (1<<24) / ((buffer)? buffer:1));
		} else if(readWrite == TMC5160_WRITE) {
			*value = MIN(0xFFFFF, (1<<24) / ((*value)? *value:1));
			tmc5160_writeInt(motor, TMC5160_TPWMTHRS, *value);
		}
		break;
	case 187:
		// PWM gradient
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_PWMCONF, TMC5160_PWM_GRAD_MASK, TMC5160_PWM_GRAD_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			// Set gradient
			TMC5160_FIELD_UPDATE(motor, TMC5160_PWMCONF, TMC5160_PWM_GRAD_MASK, TMC5160_PWM_GRAD_SHIFT, *value);
			// Enable/disable stealthChop accordingly
			TMC5160_FIELD_UPDATE(motor, TMC5160_GCONF, TMC5160_EN_PWM_MODE_MASK, TMC5160_EN_PWM_MODE_SHIFT, (*value) ? 1 : 0);
		}
		break;
	case 188:
		// PWM amplitude
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_PWMCONF, TMC5160_PWM_OFS_MASK, TMC5160_PWM_OFS_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_PWMCONF, TMC5160_GLOBAL_SCALER_MASK, TMC5160_GLOBAL_SCALER_SHIFT, *value);
		}
		break;
	case 191:
		// PWM frequency
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_PWMCONF, TMC5160_PWM_FREQ_MASK, TMC5160_PWM_FREQ_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			if(*value >= 0 && *value < 4)
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_PWMCONF, TMC5160_PWM_FREQ_MASK, TMC5160_PWM_FREQ_SHIFT, *value);
			}
			else
			{
				errors |= TMC_ERROR_VALUE;
			}
		}
		break;
	case 192:
		// PWM autoscale
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_PWMCONF, TMC5160_PWM_AUTOSCALE_MASK, TMC5160_PWM_AUTOSCALE_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			if(*value >= 0 && *value < 2)
			{
				TMC5160_FIELD_UPDATE(motor, TMC5160_PWMCONF, TMC5160_PWM_AUTOSCALE_MASK, TMC5160_PWM_AUTOSCALE_SHIFT, *value);
			}
			else
			{
				errors |= TMC_ERROR_VALUE;
			}
		}
		break;
	case 204:
		// Freewheeling mode
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_PWMCONF, TMC5160_FREEWHEEL_MASK, TMC5160_FREEWHEEL_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			TMC5160_FIELD_UPDATE(motor, TMC5160_PWMCONF, TMC5160_FREEWHEEL_MASK, TMC5160_FREEWHEEL_SHIFT, *value);
		}
		break;
	case 206:
		// Load value
		if(readWrite == TMC5160_READ) {
			*value = TMC5160_FIELD_READ(motor, TMC5160_DRVSTATUS, TMC5160_SG_RESULT_MASK, TMC5160_SG_RESULT_SHIFT);
		} else if(readWrite == TMC5160_WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 209:
		// Encoder position
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_XENC);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_XENC, *value);
		}
		break;
	case 210:
		// Encoder Resolution
		if(readWrite == TMC5160_READ) {
			*value = tmc5160_readInt(motor, TMC5160_ENC_CONST);
		} else if(readWrite == TMC5160_WRITE) {
			tmc5160_writeInt(motor, TMC5160_ENC_CONST, *value);
		}
		break;
	default:
		errors |= TMC_ERROR_TYPE;
		break;
	}
	return errors;
}

static uint32_t SAP(uint8_t type, uint8_t motor, int32_t value)
{
	return handleParameter(TMC5160_WRITE, motor, type, &value);
}

static uint32_t GAP(uint8_t type, uint8_t motor, int32_t *value)
{
	return handleParameter(TMC5160_READ, motor, type, value);
}

static uint32_t getMeasuredSpeed(uint8_t motor, int32_t *value)
{
	if(motor >= TMC_AXES_COUNT)
		return TMC_ERROR_MOTOR;

	*value = TMC5160[motor].velocity;

	return TMC_ERROR_NONE;
}

static void writeRegister(uint8_t motor, uint8_t address, int32_t value)
{
	tmc5160_writeInt(motor, address, value);
}

static void readRegister(uint8_t motor, uint8_t address, int32_t *value)
{
	*value = tmc5160_readInt(motor, address);
}

static void periodicJob(uint32_t tick)
{
	for(int motor = 0; motor < TMC_AXES_COUNT; motor++)
	{
		tmc5160_periodicJob(motor, tick, &TMC5160, board[motor].config);
	}
}

static void checkErrors(uint32_t tick)
{

}

static uint32_t userFunction(uint8_t type, uint8_t motor, int32_t *value)
{
	uint32_t buffer;
	uint32_t errors = 0;

	UNUSED(type);
	UNUSED(motor);
	UNUSED(value);

	switch(type)
	{
	default:
		errors |= TMC_ERROR_TYPE;
		break;
	}
	return errors;
}

static void deInit(void)
{
};

static uint8_t reset()
{
	for(size_t i = 0; i < TMC_AXES_COUNT; i++)
		if(!tmc5160_readInt(i, TMC5160_VACTUAL))
			tmc5160_reset(board[i].config);

	return 1;
}

static uint8_t restore()
{
	for(size_t i = 0; i < TMC_AXES_COUNT; i++)
		if(!tmc5160_readInt(i, TMC5160_VACTUAL))
			tmc5160_restore(board[i].config);

	return 1;
}

static void enableDriver(DriverState state)
{
	if(state ==  DRIVER_DISABLE)
		__NOP();
	else if(state == DRIVER_ENABLE)
		__NOP();
}


void TMC5160_init(TMC_Board *board, uint8_t axis)
{
	tmc5160_initConfig(&TMC5160[axis]);

	board->config->reset = reset;
	board->config->restore = restore;
	board->config->state = CONFIG_RESET;
	board->config->configIndex = 0;

	board->rotate               = rotate;
	board->right                = right;
	board->left                 = left;
	board->stop                 = stop;
	board->GAP                  = GAP;
	board->SAP                  = SAP;
	board->moveTo               = moveTo;
	board->moveBy               = moveBy;
	board->writeRegister        = writeRegister;
	board->readRegister         = readRegister;
	board->periodicJob          = periodicJob;
	board->userFunction         = userFunction;
	board->getMeasuredSpeed     = getMeasuredSpeed;
	board->enableDriver         = enableDriver;
	board->checkErrors          = checkErrors;
	board->numberOfMotors       = TMC5160_MOTORS;
	board->VMMin                = VM_MIN;
	board->VMMax                = VM_MAX;
	board->deInit               = deInit;

	enableDriver(DRIVER_USE_GLOBAL_ENABLE);
}
