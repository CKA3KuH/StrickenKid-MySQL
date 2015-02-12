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

#ifndef __BASE_H
#define __BASE_H

#include "Main.h"

#define PLUGIN_VERSION "2.1.1"
#define MAX_CONNECTIONS 4

struct queryInfo
{
	const char* query;
	cell		resultId;
	cell		extraId;
	const char* extraVars;
	int			type;
};

struct threadInfo
{
	cell		resultId;
	cell		extraId;
	const char* extraVars;
	int			type;
};

enum plugin_errors
{
	P_ERROR_NONE,
	P_ERROR_INCPARAMCNT,
	P_ERROR_INCVALUE,
	P_ERROR_MAXCONN,
	P_ERROR_DBNOCONN,
	P_ERROR_NORES,
	P_ERROR_RES,
	P_ERROR_NOFIELD,
	P_ERROR_NOFIELDDATA,
	P_ERROR_NOROWDATA,
	P_ERROR_CANTALLOC,
	P_ERROR_NOCALLBACK,
	P_ERROR_NOQUERY
};

enum log_types
{
	LOG_OFF,
	LOG_ALL,
	LOG_ONLY_ERRORS
};

enum query_types
{
	TYPE_ARRAY,
	TYPE_ID
};
#endif