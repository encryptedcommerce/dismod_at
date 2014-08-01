// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_smooth_table$$
$spell
	mulstd
	dage
	dtime
	struct
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Smoothing Table Information$$
$index get, smooth table$$
$index smooth, get table$$
$index table, get smooth$$

$head Syntax$$
$icode%smooth_table% = get_smooth_table(%db%)%$$

$head Purpose$$
To read the $cref smooth_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head smooth_table$$
The return value $icode smooth_table$$ has prototype
$codei%
	CppAD::vector<smooth_struct>  %smooth_table%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%smooth_table%[%smooth_id%]
%$$
is the information for the corresponding
$cref/smooth_id/smooth_table/smooth_id/$$.

$head smooth_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $pre  $$ $cnext $code smooth_name$$ $cnext
	The $cref/smooth_name/smooth_table/smooth_name/$$ for this smoothing.
$rnext
$code int$$ $cnext $code n_age$$ $cnext
	The $cref/n_age/smooth_table/n_age/$$ for this smoothing.
$rnext
$code int$$ $cnext $code n_time$$ $cnext
	The $cref/n_time/smooth_table/n_time/$$ for this smoothing.
$rnext
$code int$$ $cnext $code mulstd_value$$ $cnext
	The $cref/mulstd_value/smooth_table/mulstd_value/$$ for this smoothing.
$rnext
$code int$$ $cnext $code mulstd_dage$$ $cnext
	The $cref/mulstd_dage/smooth_table/mulstd_dage/$$ for this smoothing.
$rnext
$code int$$ $cnext $code mulstd_dtime$$ $cnext
	The $cref/mulstd_dtime/smooth_table/mulstd_dtime/$$ for this smoothing.
$tend
$comment%example/devel/table/get_smooth_grid_xam.cpp in included by smooth_grid.omh
%$$

$head Example$$
The file $cref get_smooth_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth_struct> get_smooth_table(sqlite3* db)
{	using std::string;

	string table_name  = "smooth";
	size_t n_smooth = check_table_id(db, table_name);

	std::string column_name =  "smooth_name";
	CppAD::vector<string>       smooth_name;
	get_table_column(db, table_name, column_name, smooth_name);
	assert( smooth_name.size() == n_smooth );

	column_name        =  "n_age";
	CppAD::vector<int>     n_age;
	get_table_column(db, table_name, column_name, n_age);
	assert( n_age.size() == n_smooth );

	column_name        =  "n_time";
	CppAD::vector<int>     n_time;
	get_table_column(db, table_name, column_name, n_time);
	assert( n_time.size() == n_smooth );

	column_name        =  "mulstd_value";
	CppAD::vector<int>     mulstd_value;
	get_table_column(db, table_name, column_name, mulstd_value);
	assert( mulstd_value.size() == n_smooth );

	column_name        =  "mulstd_dage";
	CppAD::vector<int>     mulstd_dage;
	get_table_column(db, table_name, column_name, mulstd_dage);
	assert( mulstd_dage.size() == n_smooth );

	column_name        =  "mulstd_dtime";
	CppAD::vector<int>     mulstd_dtime;
	get_table_column(db, table_name, column_name, mulstd_dtime);
	assert( mulstd_dtime.size() == n_smooth );

	CppAD::vector<smooth_struct> smooth_table(n_smooth);
	for(size_t i = 0; i < n_smooth; i++)
	{	smooth_table[i].smooth_name   = smooth_name[i];
		smooth_table[i].n_age         = n_age[i];
		smooth_table[i].n_time        = n_time[i];
		smooth_table[i].mulstd_value   = mulstd_value[i];
		smooth_table[i].mulstd_dage    = mulstd_dage[i];
		smooth_table[i].mulstd_dtime   = mulstd_dtime[i];
	}
	return smooth_table;
}

} // END DISMOD_AT_NAMESPACE
