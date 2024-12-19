/*
 * ESAT_Slave.c
 *
 *  Created on: 16.12.2024 г.
 *      Author: MLM1SF
 */

#include <stdlib.h>
/* Standard includes. */
#include <stdio.h>
#include <time.h>
#include "ESAT_Slave.h"

static enum en_SlaveStates SlaveState = S_INIT;
static uint32_t ESAT_Error = ESAT_S_OK;

static enum en_SlaveStates ESAT_StateDecision(uint32_t Condition)
{
	srand(Condition);
	uint32_t randValue = rand()%10u;
	enum en_SlaveStates lState = S_INIT;

	if((2u <= randValue) && 8u >= randValue)
	{
		lState = ACTIVE;
	}
	else if (0u == randValue)
	{
		lState = FAULT;
	}
	else
	{
		lState = SLEEP;
	}
	return lState;
}

static enum en_SlaveStates ESAT_ReturnState(void)
{
	return SlaveState;
}

static uint32_t ESAT_DoOperation(void* Op)
{
	uint32_t lResult = ESAT_S_NOT_OK;

	if(NULL!= Op)
	{
		if(1u == (*((uint32_t*)Op)) % 3)
		{
			lResult = ESAT_S_OK;
		}
	}

	return lResult;
}

uint8_t ESAT_SlaveOperation(struct s_MasterRequestData* ReqOp, void* Op)
{
	uint8_t u8_retVal = ESAT_S_OK;

	if( GET_STATUS == ReqOp->en_requestStatus)
	{
		ReqOp->en_slaveStatus = ESAT_ReturnState();
	}
	else if(GET_OP_RESULT == ReqOp->en_requestStatus)
	{
		ReqOp->u32_operationResult = ESAT_DoOperation(Op);
	}
	else if(GET_ERROR == ReqOp->en_requestStatus)
	{
		ReqOp->u32_operationResult = ESAT_Error;
	}
	else
	{
		u8_retVal = ESAT_S_NOT_OK;
	}
	return u8_retVal;
}



/*
 * ESAT_Slave()
 * Represent Device B
 *
 * */
uint8_t ESAT_Slave(uint32_t Trigger, struct s_MasterRequestData* s_RequestOp)
{
	uint8_t u8_reVal = 0u;
	SlaveState = ESAT_StateDecision(Trigger);

	switch(SlaveState)
	{
	case ACTIVE:
		u8_reVal = ESAT_SlaveOperation(s_RequestOp, (void*)&Trigger);
	break;

	case SLEEP:
		s_RequestOp->en_slaveStatus = SLEEP;
		u8_reVal = ESAT_S_OK;
		ESAT_Error = u8_reVal;
	break;

	case FAULT:
		s_RequestOp->en_slaveStatus = FAULT;
		ESAT_Error = ESAT_ERROR_SYNC;
		u8_reVal = ESAT_S_OK;
	break;

	default:
		printf( "ESAT_Slave no matching case \r\n" );
		u8_reVal = ESAT_S_NOT_OK;
		ESAT_Error = ESAT_ERROR_INV;
	break;
	}
	return u8_reVal;
}

