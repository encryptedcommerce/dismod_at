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
$begin check_pini_n_age$$
$spell
	const
	CppAD
	struct
	pini
$$

$section Check Initial Prevalence Grid Has One Age$$

$head syntax$$
$codei%check_pini_n_age(%rate_table%, %smooth_table%)%$$

$head rate_table$$
This argument has prototype
$codei%
	const CppAD::vector<rate_struct>& %rate_table%
%$$
and it is the 
$cref/rate_table/get_rate_table/rate_table/$$.
For this table,
only the fields $code parent_smooth_id$$ and $code child_smooth_id$$
are used.

$head smooth_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_struct>& %smooth_table%
%$$
and it is the 
$cref/smooth_table/get_smooth_table/smooth_table/$$.
For this table, only the $code n_age$$ field is used.

$end
*/
# include <dismod_at/include/get_rate_table.hpp>
# include <dismod_at/include/get_smooth_table.hpp>
# include <dismod_at/include/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_pini_n_age(
	const CppAD::vector<rate_struct>&   rate_table    ,
	const CppAD::vector<smooth_struct>& smooth_table  )
{	assert( rate_table.size()   == number_rate_enum );
	std::string message;
	//
	size_t rate_id = size_t( pini_enum );
	size_t parent_smooth_id = rate_table[rate_id].parent_smooth_id;
	size_t child_smooth_id  = rate_table[rate_id].child_smooth_id;
	//
	size_t n_age_parent = smooth_table[parent_smooth_id].n_age;
	size_t n_age_child  = smooth_table[child_smooth_id].n_age;
	//
	if( n_age_parent != 1 )
	{	message = "parent_smooth_id, for pini, corresponds to a smoothing"
			" with n_age not equal to one";
		table_error_exit("rate", rate_id, message);
	}
	//
	if( n_age_child != 1 )
	{	message = "child_smooth_id, for pini, corresponds to a smoothing"
			" with n_age not equal to one";
		table_error_exit("rate", rate_id, message);
	}
}

} // END DISMOD_AT_NAMESPACE
