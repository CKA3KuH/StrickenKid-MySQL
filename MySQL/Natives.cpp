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

Natives::Natives( void )
{
	// Nothing to do here :)
}

Natives::~Natives( void )
{
	// Nothing to do here either :)
}

cell AMX_NATIVE_CALL Natives::n_mysql_init( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_init", 2, -1 );
	int handle = -1;
	for ( int i = 0; i < MAX_CONNECTIONS; i++ )
	{
		if ( my[ i ] == NULL )
		{
			handle = i;
			break;
		}
	}
	if ( handle == -1 )
	{
		logprintf( "[MySQL] Error: Max connections (%d) reached, could not connect.", MAX_CONNECTIONS );
		Log( "Max connections (%d) reached, could not connect.", MAX_CONNECTIONS );
		return -1;
	}
	my[ handle ] = new MySQL( handle );
	return my[ handle ]->init( params[ 1 ], ( params[ 2 ] != 0 ? true : false ) );
}

cell AMX_NATIVE_CALL Natives::n_mysql_connect( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_connect", 6, -1 );
	int handle = params[ 5 ];
	if ( my[ handle ] == NULL )
	{
		Log( "Error (%d): Function: mysql_connect failed, mysql was not initialized on this handle.", handle );
		logprintf( "[MySQL] Error (%d): Function: mysql_connect failed, mysql was not initialized on this handle.", handle );
		return 0;
	}
	const char
		*host	= GetString( amx, params[ 1 ] ),
		*user	= GetString( amx, params[ 2 ] ),
		*pass	= GetString( amx, params[ 3 ] ),
		*db		= GetString( amx, params[ 4 ] );
	int result = my[ handle ]->connect( host, user, pass, db, (my_bool)params[ 6 ] );
	SAFE_DELETE( host );
	SAFE_DELETE( user );
	SAFE_DELETE( pass );
	SAFE_DELETE( db );
	return result;
}

cell AMX_NATIVE_CALL Natives::n_mysql_close( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_close", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_close" );
	SAFE_DELETE( my[ handle ] );
	return 1;
}

cell AMX_NATIVE_CALL Natives::n_mysql_refresh( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_refresh", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_reload" );
	return my[ handle ]->refresh( params[ 1 ] );
}

cell AMX_NATIVE_CALL Natives::n_mysql_select_db( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_select_db", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_select_db" );
	const char *dbname = GetString( amx, params[ 1 ] );
	int result = my[ handle ]->select_db( dbname );
	SAFE_DELETE( dbname );
	return result;
}

cell AMX_NATIVE_CALL Natives::n_mysql_query( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_query", 4, 0 );
	int handle = params[ 4 ];
	CHECK_CONNECTION( my[ handle ], "mysql_query" );
	const char *query = GetString( amx, params[ 1 ] );
	return my[ handle ]->query( query, params[ 2 ], params[ 3 ] );
}

cell AMX_NATIVE_CALL Natives::n_mysql_query_array( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_query_array", 4, 0 );
	int handle = params[ 4 ];
	CHECK_CONNECTION( my[ handle ], "mysql_query_array" );
	const char *query = GetString( amx, params[ 1 ] );
	const char *extra = GetString( amx, params[ 3 ] );
	return my[ handle ]->query_array( amx, query, params[ 2 ], extra );
}

cell AMX_NATIVE_CALL Natives::n_mysql_store_result( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_store_result", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_store_result" );
	return my[ handle ]->store_result();
}

cell AMX_NATIVE_CALL Natives::n_mysql_free_result( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_free_result", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_free_result" );
	return my[ handle ]->free_result();
}

cell AMX_NATIVE_CALL Natives::n_mysql_result_stored( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_result_stored", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_result_stored" );
	return my[ handle ]->result_stored();
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_field( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_fetch_field", 3, 0 );
	int handle = params[ 3 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_field" );
	const char *fieldname = GetString( amx, params[ 1 ] );
	const char *field = my[ handle ]->fetch_field( fieldname );
	SAFE_DELETE( fieldname );
	if ( field )
	{
		SetString( amx, params[ 2 ], field );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_field_num( AMX *amx, cell *params)
{
	CHECK_PARAMS( "mysql_fetch_field_num", 3, 0 );
	int handle = params[ 3 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_field_num" );
	const char *field = my[ handle ]->fetch_field_num( params[ 1 ] );
	if ( field )
	{
		SetString( amx, params[ 2 ], field );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_row( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_fetch_row", 3, 0 );
	int handle = params[ 3 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_row" );
	const char *splitter = GetString( amx, params[ 2 ] );
	const char *row = my[ handle ]->fetch_row( splitter );
	if ( row )
	{
		SetString( amx, params[ 1 ], row );
		SAFE_DELETE( row );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_row_data( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_fetch_row_data", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_row_data" );
	return my[ handle ]->fetch_row_data();
}

cell AMX_NATIVE_CALL Natives::n_mysql_real_escape_string( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_real_escape_string", 3, 0 );
	int handle = params[ 3 ];
	CHECK_CONNECTION( my[ handle ], "mysql_real_escape_string" );
	const char *in = GetString( amx, params[ 1 ] );
	const char *out = my[ handle ]->real_escape_string( in );
	SAFE_DELETE( in );
	if ( out )
	{
		SetString( amx, params[ 2 ], out );
		SAFE_DELETE( out );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL Natives::n_mysql_num_rows( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_num_rows", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_num_rows" );
	return my[ handle ]->num_rows();
}

cell AMX_NATIVE_CALL Natives::n_mysql_num_fields( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_num_fields", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_num_fields" );
	return my[ handle ]->num_fields();
}

cell AMX_NATIVE_CALL Natives::n_mysql_affected_rows( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_affected_rows", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_affected_rows" );
	return my[ handle ]->affected_rows();
}

cell AMX_NATIVE_CALL Natives::n_mysql_insert_id( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_insert_id", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_insert_id" );
	return my[ handle ]->insert_id();
}

cell AMX_NATIVE_CALL Natives::n_mysql_ping( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_ping", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_ping" );
	return my[ handle ]->ping();
}

cell AMX_NATIVE_CALL Natives::n_mysql_error( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_error", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_error" );
	const char *error = my[ handle ]->error();
	SetString( amx, params[ 1 ], error );
	return ( strlen( error ) > 1 ? 1 : 0 );
}

cell AMX_NATIVE_CALL Natives::n_mysql_errno( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_errno", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_errno" );
	return my[ handle ]->errorno();
}

cell AMX_NATIVE_CALL Natives::n_mysql_warning_count( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_warning_count", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_warning_count" );
	return my[ handle ]->warning_count();
}

cell AMX_NATIVE_CALL Natives::n_mysql_info( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_info", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_info" );
	const char *info = my[ handle ]->info();
	SetString( amx, params[ 1 ], info );
	return ( strlen( info ) > 1 ? 1 : 0 );
}


cell AMX_NATIVE_CALL Natives::n_mysql_stat( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_stat", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_stat" );
	const char *stat = my[ handle ]->stat();
	SetString( amx, params[ 1 ], stat );
	return ( strlen( stat ) > 1 ? 1 : 0 );
}

cell AMX_NATIVE_CALL Natives::n_mysql_get_server_info( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_get_server_info", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_get_server_info" );
	const char *server_info = my[ handle ]->get_server_info();
	SetString( amx, params[ 1 ], server_info );
	return ( strlen( server_info ) > 1 ? 1 : 0 );
}

cell AMX_NATIVE_CALL Natives::n_mysql_get_host_info( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_get_host_info", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_get_host_info" );
	const char *host_info = my[ handle ]->get_host_info();
	SetString( amx, params[ 1 ], host_info );
	return ( strlen( host_info ) > 1 ? 1 : 0 );
}

cell AMX_NATIVE_CALL Natives::n_mysql_data_seek( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_data_seek", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_data_seek" );
	return my[ handle ]->data_seek( params[ 1 ] );
}

cell AMX_NATIVE_CALL Natives::n_mysql_set_character_set( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_set_character_set", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_set_character_set" );
	const char *charset = GetString( amx, params[ 1 ] );
	int result = my[ handle ]->set_character_set( charset );
	SAFE_DELETE( charset );
	return result;
}

cell AMX_NATIVE_CALL Natives::n_mysql_get_character_set( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_get_character_set", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_get_character_set" );
	const char *charset = my[ handle ]->get_character_set();
	SetString( amx, params[ 1 ], charset );
	return ( strlen( charset ) > 1 ? 1 : 0 );
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_int( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_fetch_int", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_int" );
	return my[ handle ]->fetch_int();
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_float( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_fetch_float", 1, 0 );
	int handle = params[ 1 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_float" );
	float value = my[ handle ]->fetch_float();
	return amx_ftoc( value );
}

cell AMX_NATIVE_CALL Natives::n_mysql_fetch_string( AMX *amx, cell *params )
{
	CHECK_PARAMS( "mysql_fetch_string", 2, 0 );
	int handle = params[ 2 ];
	CHECK_CONNECTION( my[ handle ], "mysql_fetch_string" );
	const char *string = my[ handle ]->fetch_string();
	if ( string )
	{
		SetString( amx, params[ 1 ], string );
		return 1;
	}
	return 0;
}