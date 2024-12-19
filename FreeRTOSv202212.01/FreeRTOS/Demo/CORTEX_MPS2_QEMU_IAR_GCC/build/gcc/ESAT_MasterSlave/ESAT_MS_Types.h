/*
 * ESAT_MS_Types.h
 *
 *  Created on: 16.12.2024 г.
 *      Author: MLM1SF
 */

#ifndef ESAT_MASTERSLAVE_ESAT_MS_TYPES_H_
#define ESAT_MASTERSLAVE_ESAT_MS_TYPES_H_

#include <stdint.h>

#define ESAT_S_OK	     (0u)
#define ESAT_S_NOT_OK	 (1u)
#define ESAT_ERROR_SYNC  (0xAD)
#define ESAT_ERROR_OP    (0xAE)
#define ESAT_ERROR_INV   (0xAF)

enum  en_MasterStates {
	M_INIT = 0,
	IDLE,
	PROCESSING,
	ERROR
};

enum  en_SlaveStates {
	S_INIT,
	SLEEP,
	ACTIVE,
	FAULT
};

enum en_MasterSync
{
	REQUEST_STATUS = 10,
	REQUEST_OP_RESULT,
	REQUEST_ERROR
};

enum en_SlaveOperations
{
	GET_STATUS = 10,
	GET_OP_RESULT,
	GET_ERROR
};

struct s_MasterRequestData
{
	enum en_MasterSync en_requestStatus;
	enum  en_SlaveStates en_slaveStatus;
	uint32_t u32_operationResult;
};
#endif /* ESAT_MASTERSLAVE_ESAT_MS_TYPES_H_ */
