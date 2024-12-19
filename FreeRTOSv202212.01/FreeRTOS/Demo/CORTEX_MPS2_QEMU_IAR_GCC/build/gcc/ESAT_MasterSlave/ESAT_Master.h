/*
 * ESAT_Master.h
 *
 *  Created on: 16.12.2024 г.
 *      Author: Mladen Mladenov
 */

#ifndef ESAT_MASTERSLAVE_ESAT_MASTER_H_
#define ESAT_MASTERSLAVE_ESAT_MASTER_H_

#include "ESAT_MS_Types.h"

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
void ESAT_MasterMain(const uint32_t u32_ActionTrigger, struct s_MasterRequestData* s_InOutParam);


#endif /* ESAT_MASTERSLAVE_ESAT_MASTER_H_ */
