//
// synchronizationevent.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2018  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <circle/sched/synchronizationevent.h>
#include <circle/sched/scheduler.h>
#include <circle/synchronize.h>
#include <circle/sysconfig.h>
#include <assert.h>

CSynchronizationEvent::CSynchronizationEvent (boolean bState)
:	m_bState (bState),
	m_pWaitTask (0)
{
}

CSynchronizationEvent::~CSynchronizationEvent (void)
{
	assert (m_pWaitTask == 0);
}

boolean CSynchronizationEvent::GetState (void)
{
	return m_bState;
}

void CSynchronizationEvent::Clear (void)
{
	m_bState = FALSE;

#ifdef ARM_ALLOW_MULTI_CORE
	DataSyncBarrier ();
#endif
}

#ifdef HFH3_PATCH
bool CSynchronizationEvent::Set (void)
#else
void CSynchronizationEvent::Set (void)
#endif
{
	if (!m_bState)
	{
		m_bState = TRUE;

#ifdef ARM_ALLOW_MULTI_CORE
		DataSyncBarrier ();
#endif

		if (m_pWaitTask != 0)
		{
			CScheduler::Get ()->WakeTask (&m_pWaitTask);
#ifdef HFH3_PATCH
			return TRUE;
#endif
		}
	}
#ifdef HFH3_PATCH
	return FALSE;
#endif
}

void CSynchronizationEvent::Wait (void)
{
	if (!m_bState)
	{
		assert (m_pWaitTask == 0);
		CScheduler::Get ()->BlockTask (&m_pWaitTask);
#ifndef HFH3_PATCH 
		assert (m_bState);
#endif
	}
}
