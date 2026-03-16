#include "WorkerThread.h"

using namespace Sexy;


void WorkerThread::WaitForTask()
{
    SDL_WaitThread(mTaskProc, NULL);
    mTaskProc = NULL;
}
void WorkerThread::DoTask(SexyThreadCallback *func, void *param)
{
    WaitForTask();
    mTaskProc = SexyAppBase::xCreateThread(func, param);
}
