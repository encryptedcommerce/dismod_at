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
$begin check_rate_smooth$$
$spell
	ptr
	dismod
	vec
	const
	CppAD
	struct
	smoothings
	pini
$$

$section Check Rate Smoothing Grid Assumptions$$

$head syntax$$
$codei%check_rate_smooth(%rate_table%, %s_info_ptr%)%$$

$head Purpose$$
If the assumptions below do not hold, 
$code check_rate_smooth$$ exits with an error message.
$list number$$
The smoothings corresponding to
$cref/pini/rate_table/rate_name/pini/$$ must have
$cref/n_age/smooth_table/n_age/$$ equal to one.
$lnext
For each 
$cref/rate_id/rate_table/rate_id/$$
the corresponding
$cref/parent_smooth_id/rate_table/parent_smooth_id/$$ and
$cref/child_smooth_id/rate_table/child_smooth_id/$$ must have the
same 
$cref/age_id/smooth_grid_table/age_id/$$ and
$cref/time_id/smooth_grid_table/time_id/$$ values.
$lend

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

$head s_info_ptr$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info*> %smooth_info_ptr%
%$$
Let $icode n_smooth$$ be the number of rows in the
$cref smooth_table$$.
For $icode%smooth_id% = 0 , %...%, %n_smooth%-1%$$ the value
$codei%
	*(%smooth_info_ptr%[%smooth_id%])
%$$
is the $cref smooth_info$$ information for the corresponding
$cref/smooth_id/smooth_grid_table/smooth_id/$$.

$end
*/
# include <dismod_at/include/get_rate_table.hpp>
# include <dismod_at/include/smooth_info.hpp>
# include <dismod_at/include/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_rate_smooth(
	const CppAD::vector<rate_struct>& rate_table  ,
	const CppAD::vector<smooth_info*> s_info_ptr  )
{	assert( rate_table.size() == number_rate_enum );
	std::string message;

	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t parent_smooth_id = rate_table[rate_id].parent_smooth_id;
		size_t child_smooth_id  = rate_table[rate_id].child_smooth_id;
		//
		smooth_info* s_info_parent = s_info_ptr[parent_smooth_id];
		smooth_info* s_info_child  = s_info_ptr[child_smooth_id];
		//
		size_t n_age_parent = s_info_parent->age_size();
		size_t n_age_child  = s_info_child->age_size();
		//
		if( rate_id == pini_enum && n_age_parent != 1 )
		{	message = "parent_smooth_id corresponds to a smoothing"
				" with n_age not equal to one";
			table_error_exit("rate", rate_id, message);
		}
		if( n_age_parent != n_age_child )
		{	message = "parent_smooth_id and child_smooth_id correspond to"
				" differnt values of n_age in smooth table";
			table_error_exit("rate", rate_id, message);
		}
		//
		size_t n_time_parent = s_info_parent->time_size();
		size_t n_time_child  = s_info_child->time_size();
		if( n_time_parent != n_time_child )
		{	message = "parent_smooth_id and child_smooth_id correspond to"
				" differnt values of n_time in smooth table";
			table_error_exit("rate", rate_id, message);
		}
		//
		for(size_t i = 0; i < n_age_parent; i++) if(
			s_info_parent->age_id(i) != s_info_child->age_id(i)
		) 
		{	message = "parent_smooth_id and child_smooth_id correspond to"
				" age_id values in the smooth_grid table";
			table_error_exit("rate", rate_id, message);
		}
		//
		for(size_t j = 0; j < n_time_parent; j++) if(
			s_info_parent->time_id(j) != s_info_child->time_id(j)
		) 
		{	message = "parent_smooth_id and child_smooth_id correspond to"
				" time_id values in the smooth_grid table";
			table_error_exit("rate", rate_id, message);
		}
	}
}

} // END DISMOD_AT_NAMESPACE
