/*************************************************************************\
* Copyright (c) 2007 UChicago Argonne LLC, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* SPDX-License-Identifier: EPICS
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/
/* iocshRegisterCommon.h */
/* Author:  Marty Kraimer Date: 27APR2000 */

#ifndef INCiocshRegisterCommonH
#define INCiocshRegisterCommonH

#include "shareLib.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dbBase;

/* register many useful commands */
epicsShareFunc void iocshRegisterCommon(void);

epicsShareFunc
long
registerAllRecordDeviceDrivers(struct dbBase *pdbbase);

epicsShareFunc
void runRegistrarOnce(void (*reg_func)(void));

#ifdef EPICS_PRIVATE_API
epicsShareFunc
void clearRegistrarOnce(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /*INCiocshRegisterCommonH*/
