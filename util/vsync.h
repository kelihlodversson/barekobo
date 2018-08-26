#pragma once
#include <circle/types.h>
#include <circle/sched/synchronizationevent.h>

namespace hfh3
{
    /** A class used to wait for a VSync interrupt using Circle's
      * task scheduler.
      * Using this class will allow other tasks to run while the main
      * thread is waiting as opposed to the method implemented in
      * CBcmFrameBuffer::WaitForVerticalSync.
      *
      * It does however require that we put the following line into
      * config.txt:
      *      fake_vsync_isr=1
      */
    class VSync
    {
    public:
        VSync();
        ~VSync();

        bool Initialize();

        void Wait();

    private:
        static void VsyncIntStub(void* param);
        void VsyncInt();

        CSynchronizationEvent syncEvent;
        volatile bool isWaiting;
    };
}
