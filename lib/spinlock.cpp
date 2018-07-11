//
// spinlock.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2017  R. Stange <rsta2@o2online.de>
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
#include <circle/spinlock.h>

#ifdef ARM_ALLOW_MULTI_CORE

#include <circle/multicore.h>
#include <assert.h>

#define SPINLOCK_SAVE_POWER

boolean CSpinLock::s_bEnabled = FALSE;

CSpinLock::CSpinLock (unsigned nTargetLevel)
:	m_nTargetLevel (nTargetLevel),
	m_nLocked (0)
{
	assert (nTargetLevel <= FIQ_LEVEL);
}

CSpinLock::~CSpinLock (void)
{
	assert (m_nLocked == 0);
}

void CSpinLock::Acquire (void)
{
	if (m_nTargetLevel >= IRQ_LEVEL)
	{
		asm volatile
		(
			"mrs %0, cpsr\n"
			"cpsid i\n"

			: "=r" (m_nCPSR[CMultiCoreSupport::ThisCore ()])
		);

		if (m_nTargetLevel == FIQ_LEVEL)
		{
			DisableFIQs ();
		}
	}

	if (s_bEnabled)
	{
		// See: ARMv7-A Architecture Reference Manual, Section D7.3
		asm volatile
		(
			"mov r1, %0\n"
			"mov r2, #1\n"
			"1: ldrex r3, [r1]\n"
			"cmp r3, #0\n"
#ifdef SPINLOCK_SAVE_POWER
			"wfene\n"
#endif
			"strexeq r3, r2, [r1]\n"
			"cmpeq r3, #0\n"
			"bne 1b\n"
			"dmb\n"

			: : "r" ((uintptr) &m_nLocked)
		);
	}
}

void CSpinLock::Release (void)
{
	if (s_bEnabled)
	{
		// See: ARMv7-A Architecture Reference Manual, Section D7.3
		asm volatile
		(
			"mov r1, %0\n"
			"mov r2, #0\n"
			"dmb\n"
			"str r2, [r1]\n"
#ifdef SPINLOCK_SAVE_POWER
			"dsb\n"
			"sev\n"
#endif

			: : "r" ((uintptr) &m_nLocked)
		);
	}

	if (m_nTargetLevel >= IRQ_LEVEL)
	{
		asm volatile
		(
			"msr cpsr_c, %0\n"

			: : "r" (m_nCPSR[CMultiCoreSupport::ThisCore ()])
		);
	};
}

void CSpinLock::Enable (void)
{
	assert (!s_bEnabled);
	s_bEnabled = TRUE;
}

#endif
