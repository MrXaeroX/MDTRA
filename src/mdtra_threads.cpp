/***************************************************************************
* Copyright (C) 2011-2016 Alexander V. Popov.
* 
* This file is part of Molecular Dynamics Trajectory 
* Reader & Analyzer (MDTRA) source code.
* 
* MDTRA source code is free software; you can redistribute it and/or 
* modify it under the terms of the GNU General Public License as 
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
* 
* MDTRA source code is distributed in the hope that it will be 
* useful, but WITHOUT ANY WARRANTY; without even the implied 
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software 
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
***************************************************************************/
#include "mdtra_main.h"
#include "mdtra_threads.h"
#include "mdtra_progressDialog.h"

#include <QtGui/QApplication>

MDTRA_ProgressDialog *pProgressDialog = NULL;
MDTRA_ProgressBarWrapper gProgressBarWrapper;

static int dispatch = 0;
static int workcount = 0;
static bool thread_interrupt = false;

#ifdef _DEBUG
#define THREAD_DEBUG
#endif

#if !defined(WIN32)
void OutputDebugString( const char *s )
{
	puts(s);
}
#endif

int GetThreadWork( void )
{
    ThreadLock();

	if (thread_interrupt) {
#ifdef THREAD_DEBUG
        OutputDebugString("GetThreadWork: thread interrupted\n");
#endif
        ThreadUnlock();
        return -1;
	}
    if (dispatch > workcount) {
#ifdef THREAD_DEBUG
        OutputDebugString("GetThreadWork: dispatch > workcount!\n");
#endif
        ThreadUnlock();
        return -1;
    }
    if (dispatch == workcount) {
#ifdef THREAD_DEBUG
        OutputDebugString("GetThreadWork: dispatch == workcount, work is complete\n");
#endif
        ThreadUnlock();
        return -1;
    }
    if (dispatch < 0) {
#ifdef THREAD_DEBUG
        OutputDebugString("GetThreadWork: negative dispatch!\n");
#endif
        ThreadUnlock();
        return -1;
    }

    int r = dispatch;
    dispatch++;

    ThreadUnlock();
    return r;
}

static MDTRA_ThreadFunc workfunction;

static void ThreadWorkerFunction( int threadnum, int unused )
{
    int work;

    while ((work = GetThreadWork()) != -1) {
#ifdef THREAD_DEBUG
		char msgBuf[256];
		memset( msgBuf, 0, sizeof(msgBuf) );
		sprintf_s(msgBuf, sizeof(msgBuf), "Thread %i: work %i\n", threadnum, work);
		OutputDebugString(msgBuf);
#endif
		workfunction(threadnum, work);
    }

#ifdef THREAD_DEBUG
        OutputDebugString("ThreadWorkerFunction: exit!\n");
#endif
}

void InterruptThreads( void )
{
	thread_interrupt = true;
}

#if defined(USE_WIN32_THREADS)

static int numthreads = -1;
static int threadpriority = 1;
static int oldpriority = 0;
static bool threaded = false;

void ThreadSetDefault( int count, int priority )
{
 	threadpriority = priority;
	numthreads = count;

    if (numthreads == -1) {
		SYSTEM_INFO info;
        GetSystemInfo(&info);
        numthreads = info.dwNumberOfProcessors;
    }

	if (numthreads < 1)
		numthreads = 1;
	else if (numthreads > MDTRA_MAX_THREADS)
		numthreads = MDTRA_MAX_THREADS;
}

int CountThreads( void )
{
	return numthreads;
}

void ThreadSetPriority( void )
{
	int val;

    switch (threadpriority) {
    case 0:
        val = IDLE_PRIORITY_CLASS;
        break;
    case 1:
    default:
        val = NORMAL_PRIORITY_CLASS;
        break;
    }

	oldpriority = GetPriorityClass( GetCurrentProcess() );
	SetPriorityClass(GetCurrentProcess(), val);
}

void ThreadResetPriority( void )
{
	SetPriorityClass(GetCurrentProcess(), oldpriority);
}

static CRITICAL_SECTION crit;
static int enter;

void ThreadLock( void )
{
	if (!threaded)
        return;

    EnterCriticalSection(&crit);

#ifdef THREAD_DEBUG
	if (enter)
        OutputDebugString("ThreadLock: recursive thread lock!\n");
#endif
	enter++;
}

void ThreadUnlock( void )
{
	if (!threaded)
		return;

#ifdef THREAD_DEBUG
	if (!enter)
        OutputDebugString("ThreadUnlock: no lock!\n");
#endif
    enter--;
    LeaveCriticalSection(&crit);
}

MDTRA_ThreadFunc thread_entry;

static DWORD WINAPI ThreadEntryStub(LPVOID pParam)
{
    thread_entry((int)pParam, 0);
    return 0;
}

void RunThreadsOn( int workcnt, MDTRA_ThreadFunc func )
{
	DWORD threadid[MDTRA_MAX_THREADS];
    HANDLE threadhandle[MDTRA_MAX_THREADS];

	dispatch = 0;
    workcount = workcnt;
	thread_interrupt = false;

	ThreadSetPriority();

	if (CountThreads() <= 1) {
		//single-threaded
		func(0, 0);
		ThreadResetPriority();
		return;
	}

	threaded = true;
	thread_entry = func;

	QApplication::flush();
	QApplication::processEvents();

	InitializeCriticalSection(&crit);

	for (int i = 0; i < numthreads; i++) {
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadEntryStub, (LPVOID) i, CREATE_SUSPENDED, &threadid[i]);
        if (hThread != NULL) {
            threadhandle[i] = hThread;
        } else {
#ifdef THREAD_DEBUG
		OutputDebugString("Unable to create thread!\n");
#endif
        }
    }

	 // Start all the threads
    for (int i = 0; i < numthreads; i++)
    {
        if (ResumeThread(threadhandle[i]) == 0xFFFFFFFF) {
#ifdef THREAD_DEBUG
			OutputDebugString("Unable to resume thread!\n");
#endif
        }
    }

#ifdef THREAD_DEBUG
	char msgBuf[256];
	memset( msgBuf, 0, sizeof(msgBuf) );
	sprintf_s(msgBuf, sizeof(msgBuf), "Started %i threads\n", numthreads);
	OutputDebugString(msgBuf);
#endif

	// Wait for threads to complete
	while (1) {
		DWORD dwWaitResult = WaitForMultipleObjects( numthreads, threadhandle, TRUE, 200 );
		if (dwWaitResult == WAIT_TIMEOUT) {
			bool bPendingEvents = QApplication::hasPendingEvents();
			bool bUpdateDialogGUI = pProgressDialog ? pProgressDialog->needUpdateGUI() : false;
			bool bUpdateWrapperGUI = gProgressBarWrapper.needUpdateGUI();

			if (bUpdateDialogGUI || bUpdateWrapperGUI || bPendingEvents) {
				ThreadLock();
#ifdef THREAD_DEBUG
				OutputDebugString("Timeout lock: QApplication::processEvents\n");
#endif
				//update progress bar
				if (bUpdateDialogGUI) {
					pProgressDialog->updateGUI();
					QApplication::processEvents();
				} 
				if (bUpdateWrapperGUI) {
					gProgressBarWrapper.updateGUI();
					QApplication::processEvents();
				} 
				
				if (bPendingEvents) {
					QApplication::processEvents();
				}

				ThreadUnlock();
			}
		} else break;
	}

	DeleteCriticalSection(&crit);

	thread_entry = NULL;
    threaded = false;
	ThreadResetPriority();
}

#elif defined(USE_POSIX_THREADS)

static int numthreads = -1;
static int threadpriority = 1;
static bool threaded = false;

void ThreadSetDefault( int count, int priority )
{
	threadpriority = priority;
	numthreads = count;

    if (numthreads == -1) {
        //poll /proc/cpuinfo
		FILE *fp = NULL;
		if (fopen_s( &fp, "/proc/cpuinfo", "r" )) {
			numthreads = 1;
		} else {
			char buf[1024];
			memset(buf,0,sizeof(buf));
			numthreads = 0;
			while (!feof(fp)) {
				if (!fgets(buf, 1023, fp))
					break;
				if (!_strnicmp(buf, "processor", 9))
					numthreads++;
			}
			fclose(fp);
		}
    }

	if (numthreads < 1)
		numthreads = 1;
	else if (numthreads > MDTRA_MAX_THREADS)
		numthreads = MDTRA_MAX_THREADS;
}

int CountThreads( void )
{
	return numthreads;
}

void ThreadSetPriority( void )
{
	int val;

	if (threadpriority == 1)
		return;

    switch (threadpriority) {
    case 0:
        val = PRIO_MIN;
        break;
    default:
        val = 0;
        break;
    }

	setpriority( PRIO_PROCESS, 0, val );
}

void ThreadResetPriority( void )
{
	setpriority( PRIO_PROCESS, 0, 0 );
}

pthread_mutex_t* pth_mutex = NULL;
static int enter;

void ThreadLock( void )
{
	if (!threaded)
        return;

	pthread_mutex_lock(pth_mutex);

#ifdef THREAD_DEBUG
	if (enter)
        OutputDebugString("ThreadLock: recursive thread lock!\n");
#endif
	enter++;
}

void ThreadUnlock( void )
{
	if (!threaded)
        return;

#ifdef THREAD_DEBUG
	if (!enter)
        OutputDebugString("ThreadUnlock: no lock!\n");
#endif
    enter--;
	pthread_mutex_unlock(pth_mutex);
}

MDTRA_ThreadFunc thread_entry;

static void* ThreadEntryStub(void* pParam)
{
    thread_entry((int)pParam, 0);
    return NULL;
}

static unsigned long sys_timeBase = 0;
static int curtime;

int ThreadMilliseconds( void )
{
	struct timeval tp;

	gettimeofday(&tp, NULL);
	
	if (!sys_timeBase) {
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - sys_timeBase)*1000 + tp.tv_usec/1000;
	return curtime;
}

void RunThreadsOn( int workcnt, MDTRA_ThreadFunc func )
{
    pthread_t threadhandle[MDTRA_MAX_THREADS];
	pthread_attr_t threadattrib;

	dispatch = 0;
    workcount = workcnt;
	thread_interrupt = false;

	ThreadSetPriority();

	if (CountThreads() <= 1) {
		//single-threaded
		func(0, 0);
		ThreadResetPriority();
		return;
	}

	threaded = true;
	thread_entry = func;

	QApplication::flush();
	QApplication::processEvents();

	pthread_mutexattr_t pth_mutex_attr;
	if (!pth_mutex) {
		pth_mutex = (pthread_mutex_t*)malloc(sizeof(*pth_mutex));
		pthread_mutexattr_init(&pth_mutex_attr);
        pthread_mutex_init(pth_mutex, &pth_mutex_attr);
	}

	pthread_attr_init(&threadattrib);

	for (int i = 0; i < numthreads; i++) {
		if (pthread_create(&threadhandle[i], &threadattrib, ThreadEntryStub, (void*)i) == -1) {
#ifdef THREAD_DEBUG
			OutputDebugString("Unable to create thread!\n");
#endif
		}
    }

#ifdef THREAD_DEBUG
	char msgBuf[256];
	memset( msgBuf, 0, sizeof(msgBuf) );
	sprintf_s(msgBuf, sizeof(msgBuf), "Started %i threads\n", numthreads);
	OutputDebugString(msgBuf);
#endif

	// Wait for threads to complete
	int starttime = ThreadMilliseconds();

	while (1) {
		//check timeout 200ms
		int newtime = ThreadMilliseconds();
		if (newtime - starttime >= 200)	{
			starttime = newtime;

			//check if threads completed
			bool threads_done = true;
			for (int i = 0; i < numthreads; i++) {
				if (!pthread_kill(threadhandle[i], 0)) {
					//thread alive
					threads_done = false;
					break;
				}
			}

			if (threads_done)
				break;
		
			bool bPendingEvents = QApplication::hasPendingEvents();
			bool bUpdateDialogGUI = pProgressDialog ? pProgressDialog->needUpdateGUI() : false;
			bool bUpdateWrapperGUI = gProgressBarWrapper.needUpdateGUI();

			if (bUpdateDialogGUI || bUpdateWrapperGUI || bPendingEvents) {
				ThreadLock();
#ifdef THREAD_DEBUG
				OutputDebugString("Timeout lock: QApplication::processEvents\n");
#endif
				//update progress bars
				if (bUpdateDialogGUI) {
					pProgressDialog->updateGUI();
					QApplication::processEvents();
				}
				if (bUpdateWrapperGUI) {
					gProgressBarWrapper.updateGUI();
					QApplication::processEvents();
				}

				if (bPendingEvents)
					QApplication::processEvents();

				ThreadUnlock();
			}
		}
	}

	if (pth_mutex) {
		free(pth_mutex);
		pth_mutex = NULL;
	}

	thread_entry = NULL;
    threaded = false;
	ThreadResetPriority();
}

#else

void ThreadSetDefault( int count, int priority )
{
}

int CountThreads( void )
{
	return 1;
}

void ThreadLock( void )
{
}

void ThreadUnlock( void )
{
}

void RunThreadsOn( int workcnt, MDTRA_ThreadFunc func )
{
	dispatch = 0;
    workcount = workcnt;
	thread_interrupt = false;
	func(0, 0);
}

#endif

void RunThreadsOnIndividual( int workcnt, MDTRA_ThreadFunc func )
{
    workfunction = func;
    RunThreadsOn(workcnt, ThreadWorkerFunction);
}