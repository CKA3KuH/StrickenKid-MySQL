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

#ifndef __MYSQL_H
#define __MYSQL_H

class MySQL
{
	public:

						MySQL					( int handle_id );
					   ~MySQL					( void );

	int					init					( int log, bool print );
	int					connect					( const char *host, const char *user, const char *pass, const char *db, my_bool auto_reconnect );
	void				close					( void );
	int					refresh					( unsigned int options );
	int					select_db				( const char *dbname );
	int					query					( const char *query, int resultid, int extra );
	int					query_array				( AMX *amx, const char *query, int resultid, const char *extra );
	int					store_result			( void );
	int					free_result				( void );
	int					result_stored			( void );
	const char*			fetch_field				( const char *fieldname );
	const char*			fetch_field_num			( int fieldnum );
	const char*			fetch_row				( const char *splitter );
	int					fetch_row_data			( void );
	const char*			real_escape_string		( const char *string );
	long				num_rows				( void );
	int					num_fields				( void );
	long				affected_rows			( void );
	long				insert_id				( void );
	int					ping					( void );
	const char*			error					( void );
	int					errorno					( void );
	int					warning_count			( void );
	const char*			info					( void );
	const char*			stat					( void );
	const char*			get_server_info			( void );
	const char*			get_host_info			( void );
	int					data_seek				( unsigned long );
	int					set_character_set		( const char *charset );
	const char*			get_character_set		( void );
	int					fetch_int				( void );
	float				fetch_float				( void );
	const char*			fetch_string			( void );

	AMX*				parent;
	MYSQL*				mysql;
	MYSQL_RES*			result;
	MYSQL_FIELD*		field;
	MYSQL_ROW			row;
	bool				connected;
	int					state;
	unsigned int		id;
	unsigned int		logtype;
	bool				printerrs;
	bool				parsing;
	queue<queryInfo>	queryQueue;
	queue<threadInfo>	threadQueue;

};

#endif