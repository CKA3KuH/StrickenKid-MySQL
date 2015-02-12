/*
	SA:MP MySQL Plugin v2.1
	Copyright (c) 2011, StrickenKid


	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __MAIN_H
#define __MAIN_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#	include <windows.h>
#	include <process.h>
#	define OS_WINDOWS
#else
#	include <pthread.h>
#endif

// standard headers
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <queue>
#include <list>
#include <iostream>
#include <fstream>

// namespaces
using std::queue;
using std::list;

// plugin sdk
#include "../SDK/amx/amx.h"
#include "../SDK/plugincommon.h"

// mysql headers
#include "../include/mysql.h"

// plugin headers
#include "Base.h"
#include "Mysql.h"
#include "Natives.h"
#include "Thread.h"

// check params originally by Kye
#define CHECK_PARAMS(f, n, r)													\
{																				\
	if ( params[ 0 ] != ( (n) * sizeof( cell ) ) )								\
	{																			\
		logprintf( "Function: %s called with incorrect parameter count.", (f) );\
		return (r);																\
	}																			\
}

#define CHECK_CONNECTION(v, n)														  \
{																					  \
	if ( (v) == NULL || (v)->mysql == NULL || !(v)->connected )						  \
	{																				  \
		char tmp[128];																  \
		sprintf( tmp, "Function: %s called when not connected to any database", (n) );\
		return Error( my[ handle ], tmp, P_ERROR_DBNOCONN );						  \
	}																				  \
}

#define SAFE_DELETE(a)			{ if ( (a) ) { delete(a); (a) = NULL; } }
#define SAFE_FCLOSE(a)			{ if ( (a) ) { fclose(a); (a) = NULL; } }

#ifdef OS_WINDOWS
#	define EXIT_THREAD()		{ _endthread(); }
#	define SLEEP_THREAD(a)		{ Sleep( (a) ); }
#else
#  include "unistd.h"
#	define EXIT_THREAD()		{ pthread_exit( NULL ); }
#	define SLEEP_THREAD(a)		{ usleep( (a) * 1000 ); }
#endif

const char*						GetString( AMX *amx, cell params );
void							SetString( AMX *amx, cell addr, const char *source );
void							Log( const char *format, ... );
int								Error( MySQL *handle, const char *error, int errid = -1 );

typedef void					(*logprintf_t)( char *format, ... );

extern logprintf_t				logprintf;
extern list<AMX *>				pAmx;
extern MySQL*					my[ MAX_CONNECTIONS ];

#endif
