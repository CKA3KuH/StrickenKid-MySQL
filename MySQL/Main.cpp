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

void**			ppPluginData;
extern void*	pAMXFunctions;

logprintf_t		logprintf;
FILE*			mLogFile;
list<AMX *>		pAmx;
MySQL*			my[ MAX_CONNECTIONS ];


PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions	= ppData[ PLUGIN_DATA_AMX_EXPORTS ];
	logprintf		= (logprintf_t)ppData[ PLUGIN_DATA_LOGPRINTF ];

	if( mysql_library_init( 0, NULL, NULL ) )
	{
		logprintf( "[MySQL] Error: Could not initialize MySQL library." );
		exit( 0 );
	}

	logprintf( " SA:MP MySQL Plugin v%s Loaded.", PLUGIN_VERSION );

	Log( "-------------------------" );
	Log( "     Logging Started" );
	Log( "-------------------------" );

#ifdef OS_WINDOWS
	_beginthread( QueryThread, 0, NULL );
#else
	pthread_t thread;
	pthread_create( &thread, NULL, QueryThread, NULL );
#endif

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload( )
{
	Log( "-------------------------" );
	Log( "      Logging Ended" );
	Log( "-------------------------" );

	SAFE_FCLOSE( mLogFile );

	logprintf( " SA:MP MySQL Plugin v%s Unloaded.", PLUGIN_VERSION );
}

AMX_NATIVE_INFO PluginNatives[ ] =
{
	{ "mysql_init", Natives::n_mysql_init },
	{ "mysql_connect", Natives::n_mysql_connect },
	{ "mysql_close", Natives::n_mysql_close },
	{ "mysql_refresh", Natives::n_mysql_refresh },
	{ "mysql_select_db", Natives::n_mysql_select_db },
	{ "mysql_query", Natives::n_mysql_query },
	{ "mysql_query_array", Natives::n_mysql_query_array },
	{ "mysql_store_result", Natives::n_mysql_store_result },
	{ "mysql_free_result", Natives::n_mysql_free_result },
	{ "mysql_result_stored", Natives::n_mysql_result_stored },
	{ "mysql_fetch_field", Natives::n_mysql_fetch_field	},
	{ "mysql_fetch_field_num", Natives::n_mysql_fetch_field_num },
	{ "mysql_fetch_row", Natives::n_mysql_fetch_row },
	{ "mysql_fetch_row_data", Natives::n_mysql_fetch_row_data },
	{ "mysql_real_escape_string", Natives::n_mysql_real_escape_string },
	{ "mysql_num_rows", Natives::n_mysql_num_rows },
	{ "mysql_num_fields", Natives::n_mysql_num_fields },
	{ "mysql_affected_rows", Natives::n_mysql_affected_rows },
	{ "mysql_insert_id", Natives::n_mysql_insert_id },
	{ "mysql_ping", Natives::n_mysql_ping },
	{ "mysql_error", Natives::n_mysql_error },
	{ "mysql_errno", Natives::n_mysql_errno },
	{ "mysql_warning_count", Natives::n_mysql_warning_count },
	{ "mysql_info", Natives::n_mysql_info },
	{ "mysql_stat", Natives::n_mysql_stat },
	{ "mysql_get_server_info", Natives::n_mysql_get_server_info },
	{ "mysql_get_host_info", Natives::n_mysql_get_host_info },
	{ "mysql_data_seek", Natives::n_mysql_data_seek },
	{ "mysql_set_character_set", Natives::n_mysql_set_character_set },
	{ "mysql_get_character_set", Natives::n_mysql_get_character_set },
	{ "mysql_fetch_int", Natives::n_mysql_fetch_int },
	{ "mysql_fetch_float", Natives::n_mysql_fetch_float },
	{ "mysql_fetch_string", Natives::n_mysql_fetch_string },
	{ 0, 0 }
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	pAmx.push_back( amx );
	return amx_Register( amx, PluginNatives, -1 );
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	for ( list<AMX *>::iterator i = pAmx.begin(); i != pAmx.end(); ++i )
	{
		if ( (*i) == amx )
		{
			pAmx.erase( i );
			break;
		}
	}

	return AMX_ERR_NONE;
}

const char *GetString( AMX *amx, cell params )
{
	cell* phys_addr;
	int length;
	char* dest;
    amx_GetAddr( amx, params, &phys_addr );
    amx_StrLen( phys_addr, &length );
	// Fix for null string crash
	if ( length <= 0 )
	{
		dest = new char[ 5 ];
		sprintf( dest, "NULL" );
		dest[ 5 ] = '\0';
		return dest;
	}
	dest = new char[ length + 1 ];
	amx_GetString( dest, phys_addr, 0, UNLIMITED );
	dest[ length ] = '\0';
	return dest;
}

void SetString( AMX *amx, cell addr, const char *source )
{
	cell* dest;
	amx_GetAddr( amx, addr, &dest );
	amx_SetString( dest, source, 0, 0, strlen( source ) + 1 );
}

void Log( const char *format, ... )
{
	time_t		rawtime;
	struct tm * timeinfo;
	va_list		args;
	if ( mLogFile == NULL )
	{
		mLogFile = fopen( "mysql_log.txt", "a" );

		if ( mLogFile == NULL )
			return;
	}
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	char *timestr = asctime( timeinfo );
	// We don't want that new line
	timestr[ strlen( timestr ) - 1 ] = '\0';
	fprintf( mLogFile, "[%s] ", timestr );
	va_start( args, format );
	vfprintf( mLogFile, format, args );
	va_end( args );
	fprintf( mLogFile, "\n" );
	fflush( mLogFile );
}

int Error( MySQL *handle, const char *error, int errid )
{
	const char *my_error = mysql_error( handle->mysql );
	int index;
	if ( errid == -1 )
		errid = mysql_errno( handle->mysql );
	if ( handle->printerrs )
	{
		if ( strlen( my_error ) > 1 )
			logprintf( "[MySQL] Error (%d): %s. %s.", handle->id, error, my_error );
		else
			logprintf( "[MySQL] Error (%d): %s.", handle->id, error );
	}
	if ( handle->logtype == LOG_ALL || handle->logtype == LOG_ONLY_ERRORS )
	{
		if ( strlen( my_error ) > 1 )
			Log( "Error (%d): %s. %s.", handle->id, error, my_error );
		else
			Log( "Error (%d): %s.", handle->id, error );
	}
	for ( list<AMX *>::iterator i = pAmx.begin(); i != pAmx.end(); ++i )
	{
		if( amx_FindPublic( (*i), "OnMysqlError", &index ) == AMX_ERR_NONE )
		{
			char *dump;
			cell amx_addr, retval, *phys_addr;
			amx_Push( (*i), handle->id );
			amx_Push( (*i), errid );
			if ( strlen( my_error ) > 1 )
			{
				dump = new char[ strlen( error ) + strlen( my_error ) + 4 ];
				sprintf( dump, "%s. %s.", error, my_error );
				amx_PushString( (*i), &amx_addr, &phys_addr, dump, 0, 0 );
				SAFE_DELETE( dump );
			}
			else
			{
				amx_PushString( (*i), &amx_addr, &phys_addr, error, 0, 0 );
			}
			amx_Exec( (*i), &retval, index );
			amx_Release( (*i), amx_addr );
		}
	}
	return 0;
}