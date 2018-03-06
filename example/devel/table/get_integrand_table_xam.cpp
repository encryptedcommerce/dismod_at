// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_integrand_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_integrand_table: Example and Test$$
$index example, C++ get_integrand_table$$
$index get_integrand_table, C++ example$$

$code
$srcfile%example/devel/table/get_integrand_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_integrand_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table integrand"
	"(integrand_id integer primary key, integrand_name text unique)",
	"insert into integrand values(0, 'mtall'       )",
	"insert into integrand values(1, 'prevalence'  )",
	"insert into integrand values(2, 'remission'   )",
	"insert into integrand values(3, 'Sincidence'  )",
	"insert into integrand values(4, 'susceptible' )",
	"insert into integrand values(5, 'withC'       )",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the integrand table
	vector<dismod_at::integrand_enum> integrand_table =
		dismod_at::get_integrand_table(db);
	ok  &= integrand_table.size() == 6;
	//
	ok  &= integrand_table[0] == dismod_at::mtall_enum;
	ok  &= integrand_table[1] == dismod_at::prevalence_enum;
	ok  &= integrand_table[2] == dismod_at::remission_enum;
	ok  &= integrand_table[3] == dismod_at::Sincidence_enum;
	ok  &= integrand_table[4] == dismod_at::susceptible_enum;
	ok  &= integrand_table[5] == dismod_at::withC_enum;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++