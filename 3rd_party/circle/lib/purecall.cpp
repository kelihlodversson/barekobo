//
// purecall.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#include <assert.h>
#include <circle/string.h>

extern "C" void __cxa_pure_virtual (void)
{
	register unsigned link_register;
	asm volatile ("mov %0, %%lr\n" : "=r" (link_register) );
	CString error;
	error.Format("Pure virtual call from %x", link_register);
	assertion_failed(error, __FILE__, __LINE__);
//	assert (0);
}
