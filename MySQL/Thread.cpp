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

#include "Main.h"

#ifdef OS_WINDOWS
	void QueryThread( void * )
#else
	void* QueryThread( void * )
#endif
{	
	if ( mysql_thread_init() != 0 )
	{
		logprintf( "[MySQL] Warning: Query thread failed to initialize mysql library. Query Thread not running." );
		EXIT_THREAD();
	}
	
	logprintf( "[MySQL] Thread running. Threadsafe: %s.", ( mysql_thread_safe() ? "Yes" : "No" ) );

	while ( true )
	{
		for( unsigned int handle = 0; my[ handle ] != NULL; handle++ )
		{
			if ( !my[ handle ]->queryQueue.empty() && !my[ handle ]->parsing )
			{
				queryInfo	mysqlQueryQueue = my[ handle ]->queryQueue.front();
				threadInfo	mysqlThreadQueue;

				my[ handle ]->state = mysql_real_query( my[ handle ]->mysql, mysqlQueryQueue.query, strlen( mysqlQueryQueue.query) );

				if ( my[ handle ]->logtype == LOG_ALL )
					Log( "Function: mysql_query (threaded) executed: \"%s\" with result: \"%d\".", mysqlQueryQueue.query, my[ handle ]->state );

				if ( my[ handle ]->state == 0 )
				{
					mysqlThreadQueue.type		= mysqlQueryQueue.type;
					mysqlThreadQueue.resultId	= mysqlQueryQueue.resultId;

					if ( mysqlQueryQueue.type == TYPE_ID )
						mysqlThreadQueue.extraId = mysqlQueryQueue.extraId;

					else if ( mysqlQueryQueue.type == TYPE_ARRAY )
						mysqlThreadQueue.extraVars = mysqlQueryQueue.extraVars;

					my[ handle ]->parsing = true;
					my[ handle ]->threadQueue.push( mysqlThreadQueue );
				}
				else
				{
					my[ handle ]->parsing = false;
					Error( my[ handle ], "Could not execute query" );
				}

				if ( my[ handle ]->logtype == LOG_ALL )
					Log("Function: mysql_query executed: \"%s\" with result: \"%d\".", mysqlQueryQueue.query, my[ handle ]->state);

				SAFE_DELETE( mysqlQueryQueue.query );

				my[ handle ]->queryQueue.pop();
			}
		}
		SLEEP_THREAD( 4 );
	}

	mysql_thread_end();
	EXIT_THREAD();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick( void )
{
	for( unsigned int handle = 0; my[ handle ] != NULL; handle++ )
	{
		if( !my[ handle ]->threadQueue.empty() && my[ handle ]->parsing )
		{
			threadInfo	mysqlThreadQueue = my[ handle ]->threadQueue.front();
			int			index;
			cell		amx_addr, retval, *phys_addr;

			for ( list<AMX *>::iterator i = pAmx.begin(); i != pAmx.end(); ++i )
			{
				if ( mysqlThreadQueue.type == TYPE_ARRAY )
				{
					if( amx_FindPublic( (*i), "OnMysqlQueryArray", &index ) == AMX_ERR_NONE )
					{
						amx_Push( (*i), handle );
						amx_PushString( (*i), &amx_addr, &phys_addr, mysqlThreadQueue.extraVars, 0, 0 );
						amx_Push( (*i), mysqlThreadQueue.resultId );

						amx_Exec( (*i), &retval, index );
						amx_Release( (*i), amx_addr );

						if ( my[ handle ]->logtype == LOG_ALL )
							Log( "Callback: OnMysqlQueryArray(%d, %s, %d) called.", mysqlThreadQueue.resultId, mysqlThreadQueue.extraVars, handle );

						SAFE_DELETE( mysqlThreadQueue.extraVars );
					}
				}
				else if ( mysqlThreadQueue.type == TYPE_ID )
				{
					if( amx_FindPublic( (*i), "OnMysqlQuery", &index ) == AMX_ERR_NONE )
					{
						amx_Push( (*i), handle );
						amx_Push( (*i), mysqlThreadQueue.extraId );
						amx_Push( (*i), mysqlThreadQueue.resultId );

						amx_Exec( (*i), &retval, index );

						if ( my[ handle ]->logtype == LOG_ALL )
							Log( "Callback: OnMysqlQuery(%d, %d, %d) called.", mysqlThreadQueue.resultId, mysqlThreadQueue.extraId, handle );
					}
				}
			}

			my[ handle ]->threadQueue.pop();
			my[ handle ]->parsing = false;
		}
	}
}