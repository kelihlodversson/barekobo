//
// macros.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
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
#ifndef _circle_macros_h
#define _circle_macros_h

#define PACKED		__attribute__ ((packed))
#define	ALIGN(n)	__attribute__ ((aligned (n)))
#define NORETURN	__attribute__ ((noreturn))
#define NOOPT		__attribute__ ((optimize (0)))
#define MAXOPT		__attribute__ ((optimize (3)))
#define WEAK		__attribute__ ((weak))

// big endian (to be used for constants only)
#define BE(value)	((((value) & 0xFF00) >> 8) | (((value) & 0x00FF) << 8))

#endif
