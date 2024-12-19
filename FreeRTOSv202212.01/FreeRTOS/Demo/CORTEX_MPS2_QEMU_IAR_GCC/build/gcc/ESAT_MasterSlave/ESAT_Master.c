/*
 * ESAT_Master.c
 *
 *  Created on: 16.12.2024 г.
 *      Author: Mladen Mladenov
 */

/* Standard includes. */
#include <stdio.h>
#include "ESAT_Slave.h"

enum en_MasterStates  MasterState = M_INIT;

/****************************************************************************/
/*   FUNCTION:   ESAT_MasterTrigger                                         */
/****************************************************************************/
/*!
    \brief    Pseudo random trigger for switching states of Master

    \details  Device A states trigger

    \param    u32_Source - external source value for calculation trigger

    \return   master status

 */
/****************************************************************************/
enum en_MasterStates ESAT_MasterTrigger(uint32_t u32_Source)
{
	enum en_MasterStates lReturn = M_INIT;

	if(2u <= (u32_Source % 10))
	{
		lReturn = PROCESSING;
	}
	else
	{
		lReturn = IDLE;
	}
	return lReturn;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_MasterRespond                                         */
/****************************************************************************/
/*!
    \brief    Trigger action against slave state

    \details  Device A reaction relative to state of Device B

    \param    s_Response

    \return   master status

 */
/****************************************************************************/
static enum en_MasterStates ESAT_MasterRespond(struct s_MasterRequestData* s_Response)
{
	enum en_MasterStates en_UpdateStatus = M_INIT;

	if(NULL != s_Response)
	{
		if(FAULT == s_Response->en_slaveStatus)
		{
			s_Response->en_requestStatus = REQUEST_ERROR;

			if(0u != s_Response->u32_operationResult)
			{
				printf( "ESAT_Master report Slave ERROR ID: %d \r\n", s_Response->u32_operationResult );
				s_Response->u32_operationResult = 0u;
				s_Response->en_requestStatus = REQUEST_STATUS;
			}
			en_UpdateStatus = ERROR;
		}
		else if(ACTIVE == s_Response->en_slaveStatus)
		{
			printf( "ESAT_Master report SLAVE is in ACTIVE state \r\n" );
			if(REQUEST_OP_RESULT == s_Response->en_requestStatus)
			{
				printf( "ESAT_Master report SLAVE operation RESULT= %d \r\n", s_Response->u32_operationResult );
				s_Response->u32_operationResult = 0u;
				s_Response->en_requestStatus = REQUEST_STATUS;
			}
			else
			{
				s_Response->en_requestStatus = REQUEST_OP_RESULT;
				s_Response->u32_operationResult = 0u;
			}
			en_UpdateStatus = PROCESSING;
		}
		else if(SLEEP == s_Response->en_slaveStatus)
		{
			printf( "ESAT_Master report SLAVE is in SLEEP state \r\n" );
		}
	}

	return en_UpdateStatus;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_MasterSM                                              */
/****************************************************************************/
/*!
    \brief    Master State Machine

    \details  Device A state machine

    \param    s_SlaveResponse

    \return   master status

 */
/****************************************************************************/
static enum en_MasterStates ESAT_MasterSM(struct s_MasterRequestData* s_SlaveResponse)
{
	if(NULL != s_SlaveResponse)
	{
		switch(MasterState)
		{
		case PROCESSING:
			printf( "ESAT_Master PROCESSING state \r\n" );
		break;

		case IDLE:
			printf( "ESAT_Master IDLE state \r\n" );
			MasterState = PROCESSING; /* Do not stay in IDLE too long - pull requests must do */
		break;

		case ERROR:
			printf( "ESAT_Master report ERROR on Slave site \r\n" );
			if( REQUEST_ERROR != s_SlaveResponse->en_requestStatus)
			{
				MasterState = PROCESSING;
			}
		break;

		default:
			printf( "ESAT_Master no matching case \r\n" );
		break;
		}
	}

	return MasterState;
}

/****************************************************************************/
/*   FUNCTION:   ESAT_MasterMain                                            */
/****************************************************************************/
/*!
    \brief    Master State Machine

    \details  Device A state machine

    \param[in]        u32_ActionTrigger
    \param[in/out] 	  s_InOutParam

    \return

 */
/****************************************************************************/
void ESAT_MasterMain(const uint32_t u32_ActionTrigger, struct s_MasterRequestData* s_InOutParam)
{
	if(NULL != s_InOutParam)
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
}

