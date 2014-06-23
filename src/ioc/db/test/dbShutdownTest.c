/*************************************************************************\
* Copyright (c) 2013 Brookhaven National Laboratory.
* Copyright (c) 2013 ITER Organization.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
 \*************************************************************************/

/*
 *  Author: Michael Davidsaver <mdavidsaver@bnl.gov>
 *          Ralph Lange <Ralph.Lange@gmx.de>
 */

#include <string.h>

#include "dbUnitTest.h"
#include "epicsThread.h"
#include "iocInit.h"
#include "dbBase.h"
#include "dbAccess.h"
#include "registry.h"
#include "dbStaticLib.h"
#include "osiFileName.h"
#include "dbmf.h"

#include "testMain.h"

void dbShutdownTest_registerRecordDeviceDriver(struct dbBase *);

static struct threadItem {
    char *name;
    char found;
} commonThreads[] = {
    { "errlog", 0 },
    { "taskwd", 0 },
    { "timerQueue", 0 },
    { "cbLow", 0 },
    { "scanOnce", 0 },
    { NULL, 0 }
};

static
void findCommonThread (epicsThreadId id) {
    struct threadItem *thr;
    char name[32];

    epicsThreadGetName(id, name, 32);

    for (thr = commonThreads; thr->name; thr++) {
        if (strcasecmp(thr->name, name) == 0) {
            thr->found = 1;
        }
    }
}

static
void checkCommonThreads (void) {
    struct threadItem *thr;

    for (thr = commonThreads; thr->name; thr++) {
        testOk(thr->found, "Thread %s is running", thr->name);
        thr->found = 0;
    }
}

static
void cycle(void) {

    testdbPrepare();

    testdbReadDatabase("dbShutdownTest.dbd", NULL, NULL);

    dbShutdownTest_registerRecordDeviceDriver(pdbbase);

    testdbReadDatabase("sRecord.db", NULL, NULL);

    testOk1(!testiocInit());

    epicsThreadMap(findCommonThread);
    checkCommonThreads();

    testOk1(testiocShutdown()==0);

    testdbCleanup();
}

MAIN(dbShutdownTest)
{
    testPlan(14);

    cycle();
    cycle();

    return testDone();
}
