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

MySQL::MySQL( int handle_id ) :
	result( NULL ),
	id( handle_id ),
	state( -1 ),
	logtype( 0 ),
	printerrs( false ),
	parsing( false ),
	connected( false )
{
}

MySQL::~MySQL( void )
{
	if ( connected )
	{
		close();
	}
}

int MySQL::init( int log, bool print )
{
	mysql		= mysql_init( NULL );
	logtype		= log;
	printerrs	= print;

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_init executed with result: \"%d\".", ( mysql ? id : -1 ) );

	if ( mysql )
		return id;

	Error( this, "Failed to initialize" );
	return -1;
}

int MySQL::connect( const char *host, const char *user, const char *pass, const char *db, my_bool auto_reconnect )
{
	if ( mysql_real_connect( mysql, host, user, pass, db, 0, NULL, 0 ) )
	{
		mysql_options( mysql, MYSQL_OPT_RECONNECT, &auto_reconnect );

		logprintf( "[MySQL] Connected (%d) to '%s'@'%s'.", id, user, mysql_get_host_info( mysql ) );
		logprintf( "[MySQL] Server Version %s.", mysql_get_server_info( mysql ) );

		if ( logtype > 0 )
		{
			Log( "Connected (%d) to %s @ %s.", id, user, mysql_get_host_info( mysql ) );
			Log( "MySQL Server Version %s.", mysql_get_server_info( mysql ) );
		}

		connected = true;
		return 1;
	}
	return Error( this, "Failed to connect" );
}

void MySQL::close( void )
{
	if ( result )
	{
		mysql_free_result( result );
		result = NULL;
	}

	if ( row )
		row = NULL;
	if ( field )
		field = NULL;

	if ( mysql )
	{
		if ( logtype > 0 )
			Log( "MySQL closed. Logging ended." );

		logprintf( "[MySQL] Closed (%d). Logging ended.", id );

		mysql_close( mysql );
	}

	connected = false;
}

int MySQL::refresh( unsigned int options )
{
	int res = mysql_refresh( mysql, options );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_reload executed with result: \"%d\".", ( res != 0 ? 1 : 0 ) );
	if ( result != 0 )
		return Error( this, "Failed to reload database" );

	return res;
}

int MySQL::select_db( const char *dbname )
{
	int res = mysql_select_db( mysql, dbname );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_select_db executed with result: \"%d\".", ( res != 0 ? 1 : 0 ) );
	if ( res != 0 )
		return Error( this, "Failed to select database" );

	return res;
}

int MySQL::query( const char *query, int resultid, int extra )
{
	if ( result )
	{
		mysql_free_result( result );
		result = NULL;
	}

	if ( resultid != -1 )
	{
		queryInfo mysqlQueue;

		mysqlQueue.query	= query;
		mysqlQueue.resultId = resultid;
		mysqlQueue.extraId	= extra;
		mysqlQueue.type		= TYPE_ID;

		queryQueue.push( mysqlQueue );
		return 1;
	}

	state = mysql_real_query( mysql, query, strlen( query ) );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_query executed: \"%s\" with result: \"%d\".", query, state );

	SAFE_DELETE( query );

    if ( state != 0 )
         return Error( this, "Failed to exeute query" );

	return state;
}

int MySQL::query_array( AMX *amx, const char *query, int resultid, const char *extra )
{
	if ( result )
	{
		mysql_free_result( result );
		result = NULL;
	}

	if ( resultid != -1 )
	{
		queryInfo mysqlQueue;

		mysqlQueue.query 		= query;
		mysqlQueue.resultId		= resultid;
		mysqlQueue.extraVars	= extra;
		mysqlQueue.type 		= TYPE_ARRAY;

		queryQueue.push( mysqlQueue );

		return 1;
	}

	state = mysql_real_query( mysql, query, strlen( query ) );

	if ( logtype == LOG_ALL )
         Log( "Function: mysql_query_array executed: \"%s\" with result: \"%d\".", query, state );

	SAFE_DELETE( query );

    if ( state != 0 )
         return Error( this, "Failed to exeute query array" );

	return 1;
}

int MySQL::store_result( void )
{
	if ( result )
		return Error( this, "Function: mysql_store_result called when result already stored. Use mysql_free_result first", P_ERROR_RES );
	if ( state != 0 )
		return Error( this, "Function: mysql_store_result called when no prior successful query executed", P_ERROR_NOQUERY );

	result = mysql_store_result( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_store_result executed with result: \"%d\"", ( result ? 1 : 0 ) );
	if ( result == NULL )
		return Error( this, "Failed to store result" );

	return 1;
}

int MySQL::free_result( void )
{
	if ( result )
	{
		mysql_free_result( result );
		result = NULL;
	}
	if ( logtype == LOG_ALL )
         Log( "Function: mysql_free_result executed." );

	state = -1;

	if ( row )
		row = NULL;
	if ( field )
		field = NULL;

	return 1;
}

int MySQL::result_stored( void )
{
	if ( logtype == LOG_ALL )
         Log( "Function: mysql_result_stored executed." );

	return ( result ? 1 : 0 );
}

const char *MySQL::fetch_field( const char *fieldname )
{
	if ( result == NULL )
	{
		Error( this, "Function: mysql_fetch_field called when no result stored", P_ERROR_NORES );
		return NULL;
	}

	if ( logtype == LOG_ALL )
         Log( "Function: mysql_fetch_field executed." );
	if ( row == NULL )
		row = mysql_fetch_row( result );

	if ( row )
	{
		for ( unsigned int i = 0, j = mysql_num_fields( result ); i < j; i++ )
		{
			field = mysql_fetch_field_direct( result, i );

			if ( !strcmp( fieldname, field->name ) )
			{
				if ( row[ i ] )
				{
					return row[ i ];
				}
				else
				{
					Error( this, "Function: mysql_fetch_field failed, no field data", P_ERROR_NOFIELDDATA );
					return NULL;
				}
			}
		}
		char buffer[ 128 ];
		sprintf( buffer, "Function: mysql_fetch_field could not find field name: \"%s\"", fieldname );
		Error( this, buffer, P_ERROR_NOFIELD );
		return NULL;
	}
	return NULL;
}

const char *MySQL::fetch_field_num( int fieldnum )
{
	if ( result == NULL )
	{
		Error( this, "Function: mysql_store_result called when no result stored", P_ERROR_NORES );
		return NULL;
	}
	if ( logtype == LOG_ALL )
         Log( "Function: mysql_fetch_field_num executed." );

	if ( row == NULL )
		row = mysql_fetch_row( result );

	if ( row )
		return ( row[ fieldnum ] ? row[ fieldnum ] : NULL );

	Error( this, "Function: mysql_fetch_field failed, no field data", P_ERROR_NOFIELDDATA );
	return NULL;
}

const char *MySQL::fetch_row( const char *splitter )
{
	if ( result == NULL )
	{
		Error( this, "Function: mysql_store_result called when no result stored", P_ERROR_NORES );
		return NULL;
	}

	if ( row = mysql_fetch_row( result ) )
	{
		unsigned long *lengths = mysql_fetch_lengths( result );
		unsigned int numfields = mysql_num_fields( result );

		int bufflen = numfields + 1;

		for ( unsigned int i = 0; i < numfields; i++ )
			bufflen += lengths[ i ];

		char *buffer = new char[ bufflen ];

		memset( buffer, 0, bufflen );

		for ( unsigned int i = 0; i < numfields; i++ )
		{
			strcat( buffer, ( row[ i ] ? row[ i ] : "NULL" ) );
			if ( i != numfields - 1 )
				strcat( buffer, splitter );
		}

		buffer[ strlen( buffer ) ] = '\0';

		if ( logtype == LOG_ALL )
			Log( "Function: mysql_fetch_row executed with result: \"%s\".", buffer );

		return buffer;
	}
	return NULL;
}

int MySQL::fetch_row_data( void )
{
	if ( result == NULL )
		return Error( this, "Function: mysql_store_result called when no result stored", P_ERROR_NORES );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_fetch_row_data executed." );

	row = mysql_fetch_row( result );

	if ( row )
		return 1;

	return Error( this, "Function: mysql_fetch_row_data failed, no field data", P_ERROR_NOFIELDDATA );
}

const char *MySQL::real_escape_string( const char *string )
{
	char *buffer = new char[ strlen( string ) * 3 ];

	mysql_real_escape_string( mysql, buffer, string, strlen( string ) );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_real_esacpe_string executed: \"%s\" with result: \"%s\".", string, buffer );

	return buffer;
}

long MySQL::num_rows( void )
{
	if ( result == NULL )
		return Error( this, "Function: mysql_num_rows called when no result stored", P_ERROR_NORES );

	long res = (long)mysql_num_rows( result );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_num_rows executed with result: \"%l\".", res );

	return res;
}

int MySQL::num_fields( void )
{
	if ( result == NULL )
		return Error( this, "Function: mysql_num_fields called when no result stored", P_ERROR_NORES );

	int res = mysql_num_fields( result );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_num_fields executed with result: \"%d\".", res );

	return res;
}

long MySQL::affected_rows( void )
{
	long res = (long)mysql_affected_rows( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_affected_rows executed with result: \"%l\".", res );

	return res;
}

long MySQL::insert_id( void )
{
	long res = (long)mysql_insert_id( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_insert_id executed with result: \"%l\".", res );

	return res;
}

int MySQL::ping( void )
{
	int res = mysql_ping( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_ping executed with result: \"%d\".", res );

	return res;
}

const char *MySQL::error( void )
{
	const char *res = mysql_error( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_error executed with result: \"%s\".", res );

	return res;
}

int MySQL::errorno( void )
{
	int res = mysql_errno( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_errno executed with result: \"%d\".", res );

	return res;
}

int MySQL::warning_count( void )
{
	int res = mysql_warning_count( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_warning_count executed with result: \"%d\".", res );

	return res;
}

const char *MySQL::info( void )
{
	const char *res = mysql_info( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_info executed with result: \"%s\".", res );

	return res;
}

const char *MySQL::stat( void )
{
	const char *res = mysql_stat( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_stat executed with result: \"%s\".", res );

	return res;
}

const char *MySQL::get_server_info( void )
{
	const char *res = mysql_get_server_info( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_get_server_info executed with result: \"%s\".", res );

	return res;
}

const char *MySQL::get_host_info( void )
{
	const char *res = mysql_get_host_info( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_get_host_info executed with result: \"%s\".", res );

	return res;
}

int MySQL::data_seek( unsigned long row )
{
	if ( result == NULL )
		return Error( this, "Function: mysql_data_seek called when no result stored", P_ERROR_NORES );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_data_seek executed." );

	mysql_data_seek( result, row );

	return 1;
}

int MySQL::set_character_set( const char *charset )
{
	int res = mysql_set_character_set( mysql, charset );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_set_character_set executed with result: \"%d\".", res );

	return res;
}

const char *MySQL::get_character_set( void )
{
	const char *res = mysql_character_set_name( mysql );

	if ( logtype == LOG_ALL )
		Log( "Function: mysql_get_character_set executed with result: \"%d\".", res );

	return res;
}

int MySQL::fetch_int( void )
{
	if ( result == NULL )
		return Error( this, "Function: mysql_fetch_int called when no result stored", P_ERROR_NORES );

	if ( row = mysql_fetch_row( result ) )
	{
		int res = atoi( ( row[ 0 ] ? row[ 0 ] : "0" ) );

		if ( logtype == LOG_ALL )
			Log( "Function: mysql_fetch_int executed with result: \"%d\".", res );

		return res;
	}
	return 0;
}

float MySQL::fetch_float( void )
{
	if ( result == NULL )
	{
		Error( this, "Function: mysql_fetch_float called when no result stored", P_ERROR_NORES );
		return 0.0;
	}

	if ( row = mysql_fetch_row( result ) )
	{
		float res = (float)atof( ( row[ 0 ] ? row[ 0 ] : "0.0" ) );

		if ( logtype == LOG_ALL )
			Log( "Function: mysql_fetch_float executed with result: \"%f\".", res );

		return res;
	}

	return 0.0;
}

const char *MySQL::fetch_string( void )
{
	if ( result == NULL )
	{
		Error( this, "Function: mysql_fetch_float called when no result stored", P_ERROR_NORES );
		return NULL;
	}

	if ( row = mysql_fetch_row( result ) )
	{
		const char *res = ( row[ 0 ] ? row[ 0 ] : NULL );

		if ( logtype == LOG_ALL )
			Log( "Function: mysql_fetch_string executed with result: \"%s\".", res );

		return res;
	}

	return NULL;
}