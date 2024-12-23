/*
 * ESAT_Slave.c
 *
 *  Created on: 16.12.2024 г.
 *      Author: Mladen Mladenov
 */

#include <stdlib.h>
/* Standard includes. */
#include <stdio.h>
#include <time.h>
#include "ESAT_Slave.h"

#define ESAT_SLAVE_DIVIDER                  50U
#define ESAT_SLAVE_DECREMENT_OP_CONSTANT	10000U

static enum en_SlaveStates SlaveState = S_INIT;
static uint32_t ESAT_Error = ESAT_S_OK;

/****************************************************************************/
/*   FUNCTION:   ESAT_StateDecision                                         */
/****************************************************************************/
/*!
    \brief    Random switching between Slave states

    \details  Using pseudo random number generator to change slave states

    \param    Condition

    \return   new slave state

 */
/****************************************************************************/
static enum en_SlaveStates ESAT_StateDecision(uint32_t Condition)
{
	enum en_SlaveStates lState = S_INIT;
	uint32_t randValue;

	if(S_INIT != SlaveState)
	{
		srand(Condition);
		randValue = rand() % ESAT_SLAVE_DIVIDER;

		if( 5U == randValue)
		{
			lState = SLEEP;
		}
		else if (0U == randValue)
		{
			lState = FAULT;
		}
		else
		{
			lState = ACTIVE;
		}
	}

	return lState;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_ReturnState                                           */
/****************************************************************************/
/*!
    \brief    Get current slave state

    \details  Get current slave state

    \return   current slave state

 */
/****************************************************************************/
static enum en_SlaveStates ESAT_ReturnState(void)
{
	return SlaveState;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_DoOperation                                           */
/****************************************************************************/
/*!
    \brief    Slave operation execution

    \details  Simulation of operation execution with simple calculations

    \param[in/out]    Op operand value received from timer or other source

    \return   result status

 */
/****************************************************************************/
static uint32_t ESAT_DoOperation(void* Op)
{
	uint32_t lResult = ESAT_S_NOT_OK;

	if(NULL!= Op)
	{
		if(ESAT_SLAVE_DECREMENT_OP_CONSTANT <= (*((uint32_t*)Op)))
		{
			lResult = (*((uint32_t*)Op) - (*((uint32_t*)Op) >> 4U));
		}
		else
		{
			lResult = (*((uint32_t*)Op));
		}

	}

	return lResult;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_SlaveOperation                                        */
/****************************************************************************/
/*!
    \brief    Return slave status or result from operation

    \details  Can return Slave Status, Error details or result from operation
    \param    ReqOp
    \param    Op
    \return   result

 */
/****************************************************************************/
uint8_t ESAT_SlaveOperation(struct s_MasterRequestData* ReqOp, void* Op)
{
	uint8_t u8_retVal = ESAT_S_NOT_OK;

	if((NULL != ReqOp) && (NULL != Op))
	{
		if( REQUEST_STATUS == ReqOp->en_requestStatus)
		{
			ReqOp->en_slaveStatus = ESAT_ReturnState();
		}
		else if(REQUEST_OP_RESULT == ReqOp->en_requestStatus)
		{
			ReqOp->u32_operationResult = ESAT_DoOperation(Op);
		}
		else if(REQUEST_ERROR == ReqOp->en_requestStatus)
		{
			ReqOp->u32_operationResult = ESAT_Error;
		}
		else if(REQUEST_TGT_RESET == ReqOp->en_requestStatus)
		{
			SlaveState = S_INIT;
		}
		else
		{
			u8_retVal = ESAT_S_OK;
		}
	}

	return u8_retVal;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_Slave                                                 */
/****************************************************************************/
/*!
    \brief    Represent Device B

    \details  Function representing Device B from ESAT task

    \param    Trigger

    \param    s_RequestOp

    \return   result status

 */
/****************************************************************************/
uint8_t ESAT_Slave(uint32_t Trigger, struct s_MasterRequestData* s_RequestOp)
{
	uint8_t u8_reVal = ESAT_S_NOT_OK;

	if(NULL != s_RequestOp)
	{
		SlaveState = ESAT_StateDecision(Trigger);

		switch(SlaveState)
		{
		case S_INIT:
			srand(Trigger);
			s_RequestOp->en_slaveStatus = SlaveState;
			SlaveState = ACTIVE;
		break;
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
	}

	return u8_reVal;
}


