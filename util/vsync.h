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

        /** Initialization routine that will register an interrupt handler
          * with the interrupt system for the GPU SMI interrupt.
          */
        bool Initialize();

        /** Calling Wait will block the current thread until the next vertical
          * sync interrupt request is received.
          */
        void Wait();

        /** Returns the number of vsync interrupts received while no threads
          * were waiting. This can be used as an indication of missed frames
          * during rendering
          */
        unsigned GetMissed() { return missedFrames; }

    private:

        /** The interrupt service routine that handles the SMI interrupt
          * sent from the GPU.
          */
        void VsyncInt();

        /** Static stub needed to forward calls to VsyncInt, as the intterrupt
          * subsystem only can call static methods.
          */
        static void VsyncIntStub(void* param);

        // The synchronization event used to block and then re-enable the
        // thread of execution when waiting for vertical sync
        CSynchronizationEvent syncEvent;

        // The following two are accessed from interrupt context and are marked
        // as volatile to prevent the compiler from optimizing access to them.
        volatile unsigned missedFrames;
    };
}
