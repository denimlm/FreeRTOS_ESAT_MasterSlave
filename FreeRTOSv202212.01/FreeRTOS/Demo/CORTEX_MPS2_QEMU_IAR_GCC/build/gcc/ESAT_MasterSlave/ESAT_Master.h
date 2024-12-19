/*
 * ESAT_Master.h
 *
 *  Created on: 16.12.2024 г.
 *      Author: MLM1SF
 */

#ifndef ESAT_MASTERSLAVE_ESAT_MASTER_H_
#define ESAT_MASTERSLAVE_ESAT_MASTER_H_

#include "ESAT_MS_Types.h"

/*
 * ESAT_Master()
 * Represent Device A
 *
 * \param    u32_ActionTrigger
 * \param    s_InOutParam
 *
 * \return   value
 * */
void ESAT_MasterMain(const uint32_t u32_ActionTrigger, struct s_MasterRequestData* s_InOutParam);


#endif /* ESAT_MASTERSLAVE_ESAT_MASTER_H_ */
