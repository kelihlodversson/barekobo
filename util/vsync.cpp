#include <circle/bcm2835.h>
#include <circle/interrupt.h>
#include <circle/memio.h>
#include <circle/sched/scheduler.h>
#include <circle/synchronize.h>

#include "util/vsync.h"
#include "util/log.h"

using namespace hfh3;

// Note config.txt needs to have the line "fake_vsync_isr=1" for this to work
const unsigned VSYNC_IRQ  = ARM_IRQ_SMI;
const uintptr  ARM_SMI_CS =(ARM_IO_BASE + 0x600000);

VSync::VSync()
    : syncEvent(false)
    , isWaiting(false)
{}

bool VSync::Initialize()
{
    CInterruptSystem* interrupts = CInterruptSystem::Get();
    assert(interrupts);
    interrupts->ConnectIRQ(VSYNC_IRQ, VSync::VsyncIntStub, this);
}

VSync::~VSync()
{
    CInterruptSystem* interrupts = CInterruptSystem::Get();
    assert(interrupts);
    interrupts->DisconnectIRQ(VSYNC_IRQ);
}

void VSync::Wait()
{
    isWaiting = true;
	DataSyncBarrier();

    syncEvent.Wait();
    syncEvent.Clear(); // immediately clear the event so the next call will block again
}

void VSync::VsyncIntStub(void* param)
{
    assert(param);
    VSync* vsync = static_cast<VSync*>(param);
    vsync->VsyncInt();
}

/* Handle the SMI interrupt, that if fake_vsync_isr is set to 1 in config.txt
 * is generated on every VSYNC by the GPU core.
 * See: https://github.com/raspberrypi/firmware/issues/67
 */
void VSync::VsyncInt()
{
    // Clear the SMI interrupt status
    write32(ARM_SMI_CS, 0);

    // Wake up the screen manager task if it's waiting
    if (isWaiting)
    {
        isWaiting = false;
        syncEvent.Set();
    }
}
