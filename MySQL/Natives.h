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

#ifndef __NATIVES_H
#define __NATIVES_H

class Natives
{
	public:
									Natives						( void );
								   ~Natives						( void );

	static cell AMX_NATIVE_CALL		n_mysql_init				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_connect				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_close				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_refresh				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_select_db			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_query				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_query_array			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_store_result		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_free_result			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_result_stored		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_field			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_field_num		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_row			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_row_data		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_real_escape_string	( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_num_rows			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_num_fields			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_affected_rows		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_insert_id			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_ping				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_error				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_errno				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_warning_count		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_info				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_stat				( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_get_server_info		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_get_host_info		( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_data_seek			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_set_character_set	( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_get_character_set	( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_int			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_float			( AMX *amx, cell *params );
	static cell AMX_NATIVE_CALL		n_mysql_fetch_string		( AMX *amx, cell *params );
};

#endif