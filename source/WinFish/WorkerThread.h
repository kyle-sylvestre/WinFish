#ifndef __SEXY_WORKER_THREAD_H__
#define __SEXY_WORKER_THREAD_H__

#include <SexyAppFramework/SexyAppBase.h>
#include <SDL3/SDL.h>
#include <vector>
#include <list>

namespace Sexy
{
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	class WorkerThread
	{
	public:
        SDL_Thread *mTaskProc;
		void DoTask(SexyThreadCallback *func, void* param);
        void WaitForTask();
	};
};

#endif

