/*
 * ESAT_Master.c
 *
 *  Created on: 16.12.2024 г.
 *      Author: MLM1SF
 */

/* Standard includes. */
#include <stdio.h>
#include "ESAT_Slave.h"

enum en_MasterStates  MasterState = M_INIT;

/*
 * ESAT_Master_Request()
 * Represent Device A
 *
 * \param    u32_Request
 *
 * \return   result
 * */
uint32_t ESAT_Master_Request(enum en_MasterSync en_Request)
{
	uint32_t lResult = 0u;

	if(REQUEST_STATUS == en_Request)
	{
		lResult = 0u;
	}
	else if(REQUEST_OP_RESULT == en_Request)
	{
		lResult = 1u;
	}
	else if(REQUEST_ERROR == en_Request)
	{
		lResult = 2u;
	}
	else
	{

	}
	return lResult;
}

/*
 * ESAT_MasterTrigger()
 * Represent Device A
 *
 * \param    u32_Source
 *
 * \return   en_MasterStates
 * */
enum en_MasterStates ESAT_MasterTrigger(uint32_t u32_Source)
{
	enum en_MasterStates lReturn = M_INIT;

	if(0u != (u32_Source % 10))
	{
		lReturn = PROCESSING;
	}
	else
	{
		lReturn = IDLE;
	}
	return lReturn;
}

/*
 * ESAT_MasterRespond()
 * Represent Device A
 *
 * \param    u32_Trigger
 * \param    u32_SlaveResponse
 *
 * \return   value
 * */
static enum en_MasterStates ESAT_MasterRespond(const struct s_MasterRequestData* s_Response)
{
	enum en_MasterStates en_UpdateStatus = M_INIT;

	if(FAULT == s_Response->en_slaveStatus)
	{
		en_UpdateStatus = ERROR;
	}
	else if(ACTIVE == s_Response->en_slaveStatus)
	{
		printf( "ESAT_Master report SLAVE is in ACTIVE state \r\n" );
		en_UpdateStatus = PROCESSING;
	}
	else if(SLEEP == s_Response->en_slaveStatus)
	{
		printf( "ESAT_Master report SLAVE is in SLEEP state \r\n" );
		en_UpdateStatus = IDLE;
	}

	return en_UpdateStatus;
}
/*
 * ESAT_MasterSM()
 * Represent Device A
 *
 * \param    u32_SlaveResponse
 *
 * \return   value
 * */
static enum en_MasterStates ESAT_MasterSM(struct s_MasterRequestData* s_SlaveResponse)
{
	switch(MasterState)
	{
	case PROCESSING:
		s_SlaveResponse->en_requestStatus = REQUEST_STATUS;//ESAT_Master_Request(u32_SlaveResponse);
		printf( "ESAT_Master PROCESSING state \r\n" );
	break;

	case IDLE:
		printf( "ESAT_Master IDLE state \r\n" );
		s_SlaveResponse->en_slaveStatus = S_INIT;
		MasterState = PROCESSING;
	break;

	case ERROR:
		printf( "ESAT_Master report ERROR on SLAVE site \r\n" );
		if( FAULT == s_SlaveResponse->en_slaveStatus)
		{
			s_SlaveResponse->en_requestStatus = REQUEST_ERROR;

			if(0u != s_SlaveResponse->u32_operationResult)
			{
				printf( "ESAT_Master report SLAVE ERROR ID: %d \r\n", s_SlaveResponse->u32_operationResult );
				MasterState = PROCESSING;
				s_SlaveResponse->u32_operationResult = 0u;
			}

		}
	break;

	default:
		printf( "ESAT_Master no matching case \r\n" );
	break;
	}

	return MasterState;
}


/*
 * ESAT_Master()
 * Represent Device A
 *
 * \param    u32_ActionTrigger
 * \param    s_InOutParam
 *
 * \return   value
 * */
void ESAT_MasterMain(const uint32_t u32_ActionTrigger, struct s_MasterRequestData* s_InOutParam)
{
	enum en_MasterStates en_UpdateStatus = M_INIT;

	en_UpdateStatus = ESAT_MasterRespond(s_InOutParam);

	if(M_INIT == en_UpdateStatus)
	{
		MasterState = ESAT_MasterTrigger(u32_ActionTrigger);
	}
	else
	{
		MasterState = en_UpdateStatus;
	}
	/*! ESAT Master State Machine call */
	(void)ESAT_MasterSM(s_InOutParam);
}

