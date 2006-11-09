/*************************************************************************\
* Copyright (c) 2006 UChicago Argonne LLC, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/
/* epicsExitTest.cpp */

/* Author:  Marty Kraimer Date:    09JUL2004*/

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "epicsThread.h"
#include "epicsAssert.h"
#include "epicsEvent.h"
#include "epicsExit.h"
#include "epicsUnitTest.h"
#include "testMain.h"


typedef struct info {
    const char *name;
    epicsEventId terminate;
    epicsEventId terminated;
}info;

static void atExit(void *pvt)
{
    info *pinfo = (info *)pvt;
    testPass("%s reached atExit", pinfo->name);
    epicsEventSignal(pinfo->terminate);
    /*Now wait for thread to terminate*/
    epicsEventMustWait(pinfo->terminated);
    testPass("%s destroying pinfo", pinfo->name);
    epicsEventDestroy(pinfo->terminate);
    epicsEventDestroy(pinfo->terminated);
    free(pinfo);
}

static void thread(void *arg)
{
    info *pinfo = (info *)arg;

    pinfo->name = epicsThreadGetNameSelf();
    testDiag("%s starting", pinfo->name);
    pinfo->terminate = epicsEventMustCreate(epicsEventEmpty);
    pinfo->terminated = epicsEventMustCreate(epicsEventEmpty);
    epicsAtExit(atExit, pinfo);
    testDiag("%s waiting for atExit", pinfo->name);
    epicsEventMustWait(pinfo->terminate);
    testPass("%s terminating", pinfo->name);
    epicsEventSignal(pinfo->terminated);
}

static void mainExit(void *pvt)
{
    testPass("Reached mainExit");
    testDone();
}

MAIN(epicsExitTest)
{
    unsigned int stackSize = epicsThreadGetStackSize(epicsThreadStackSmall);
    info *pinfoA = (info *)calloc(1, sizeof(info));
    info *pinfoB = (info *)calloc(1, sizeof(info));

    testPlan(7);

    epicsAtExit(mainExit, NULL);

    epicsThreadCreate("threadA", 50, stackSize, thread, pinfoA);
    epicsThreadSleep(0.1);
    epicsThreadCreate("threadB", 50, stackSize, thread, pinfoB);
    epicsThreadSleep(1.0);

    testDiag("Calling epicsExit\n");
    epicsExit(0);
    return 0;
}
