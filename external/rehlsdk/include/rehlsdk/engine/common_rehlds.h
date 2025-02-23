/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/
#pragma once

#include "rehlsdk/common/const.h"
#include "rehlsdk/common/qlimits.h"

#ifdef REHLDS_FIXES
#define COM_TOKEN_LEN	2048
#else
#define COM_TOKEN_LEN	1024
#endif

// Don't allow overflow
#define SIZEBUF_CHECK_OVERFLOW	0
#define SIZEBUF_ALLOW_OVERFLOW	BIT(0)
#define SIZEBUF_OVERFLOWED		BIT(1)

#define MAX_NUM_ARGVS	50
#define NUM_SAFE_ARGVS	7

#define COM_COPY_CHUNK_SIZE 1024
#define COM_MAX_CMD_LINE 256

typedef struct sizebuf_s
{
	const char *buffername;
	uint16 flags;
	byte *data;
	int maxsize;
	int cursize;
} sizebuf_t;

typedef struct downloadtime_s
{
	qboolean bUsed;
	float fTime;
	int nBytesRemaining;
} downloadtime_t;

typedef struct incomingtransfer_s
{
	qboolean doneregistering;
	int percent;
	qboolean downloadrequested;
	downloadtime_t rgStats[8];
	int nCurStat;
	int nTotalSize;
	int nTotalToTransfer;
	int nRemainingToTransfer;
	float fLastStatusUpdate;
	qboolean custom;
} incomingtransfer_t;
