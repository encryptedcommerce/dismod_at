// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

/*
$begin pack_var_ctor$$
$spell
	mulcov
	CppAD
	struct
	dismod
	var
	const
	integrands
$$

$section Pack Variables Constructor$$

$head Syntax$$
$codei%dismod_at::pack_var %var%(
	%n_integrand%, %smooth_table%, %mulcov_table%
)
%$$
$icode%size%  = %var%.size()
%$$

$head n_integrand$$
This argument has prototype
$codei%
	size_t %n_integrand%
%$$
and is the number of integrands; i.e., the size of
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head smooth_table$$
This argument has prototype
$codei%
	CppAD::vector<smooth_struct>& %smooth_table%
%$$
and is the
$cref/smooth_table/get_smooth_table/smooth_table/$$.
Only the following fields of this table are used:
$code n_age$$, $code n_time$$.

$head mulcov_table$$
This argument has prototype
$codei%
	CppAD::vector<mulcov_struct>& %mulcov_table%
%$$
and is the
$cref/mulcov_table/get_mulcov_table/mulcov_table/$$.

$head size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %size%
%$$
and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

$end
*/

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

pack_var::pack_var(
	size_t                               n_integrand  ,
	const CppAD::vector<smooth_struct>&  smooth_table ,
	const CppAD::vector<mulcov_struct>&  mulcov_table 
) :
n_smooth_( smooth_table.size() ) ,
n_integrand_( n_integrand ) 
{	using std::string;

	// mulstd
	size_t offset = 0;
	offset_value_mulstd_  = offset; offset += n_smooth_;
	offset_dage_mulstd_   = offset; offset += n_smooth_;
	offset_dtime_mulstd_  = offset; offset += n_smooth_;

	// meas_mean_mulcov
	meas_mean_mulcov_info_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t mulcov_id;
		for(mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == meas_mean_enum;
			match &= mulcov_table[mulcov_id].integrand_id == int(integrand_id);
			if( match )
			{	size_t covariate_id = size_t(
					mulcov_table[mulcov_id].covariate_id
				); 
				CppAD::vector<mulcov_info>& info_vec = 
					meas_mean_mulcov_info_[integrand_id];
				for(size_t j = 0; j < info_vec.size(); j++)
				{	if( info_vec[j].covariate_id == covariate_id )
					{	string msg = "covariate_id appears twice with "
							"mulcov_type equal to 'meas_mean'";
						string table_name = "mulcov";
						table_error_exit(table_name, mulcov_id, msg);
					}
				}
				size_t smooth_id = mulcov_table[mulcov_id].smooth_id;
				size_t n_age     = smooth_table[smooth_id].n_age;
				size_t n_time    = smooth_table[smooth_id].n_time;
				//
				mulcov_info info;
				info.covariate_id = covariate_id;
				info.smooth_id    = smooth_id;
				info.n_var        = n_age * n_time;
				info.offset       = offset; 
				offset           += info.n_var;
			}
		}
	}
	size_ = offset;
};

// size
size_t pack_var::size(void) const
{	return size_; }

/*
$begin pack_var_mulstd$$
$spell
	var
	mulstd
	dage
	dtime
	const
	dismod
$$

$section Pack Variables Standard Deviations Multipliers$$

$head Syntax$$
$icode%index% = %var%.value_mulstd(%smooth_id%)
%$$
$icode%index% = %var%.dage_mulstd(%smooth_id%)
%$$
$icode%index% = %var%.dtime_mulstd(%smooth_id%)
%$$

$head var$$
This object has prototype
$codei%
	const dismod_at::pack_var %var%
%$$.

$head smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the 
$cref/smooth_id/smooth_table/smooth_id/$$.

$subhead index$$
The return value has prototype
$codei%
	size_t index
%$$
and is the unique index for the correspond smoothing standard
deviation multiplier in a packed variable vector.


$end

*/
size_t pack_var::value_mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_value_mulstd_ + smooth_id;
}
size_t pack_var::dage_mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_dage_mulstd_ + smooth_id;
}
size_t pack_var::dtime_mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_dtime_mulstd_ + smooth_id;
}

/*
$begin pack_var_meas_mean$$
$spell
	cov
	var
	mulcov
	dismod
	const
	covariate
$$

$section Pack Variables Measurement Mean Multipliers$$

$head Syntax$$
$icode%n_cov% = %var%.meas_mean_mulcov_n_cov(%integrand_id%)
%$$
$icode%info% = %var%.meas_mean_mulcov_info(%integrand_id%, %j%)
%$$

$head mulcov_info$$
The type $code dismod_at::pack-var::mulcov_info$$ is defined as follows:
$code
$verbatim%dismod_at/include/pack_var.hpp
%5%// BEGIN MULCOV_INFO%// END MULCOV_INFO%$$
$$
$head var$$
This object has prototype
$codei%
	const dismod_at::pack_var %var%
%$$.

$head integrand_id$$
This argument has prototype 
$codei%
	size_t %integrand_id%
%$$
and it specifies the 
$cref/integrand_id/integrand_table/integrand_id/$$ the covariate
multiplier.

$head n_cov$$
This return value has prototype
$codei%
	size_t %n_cov%
%$$ 
and is the number of covariate multipliers for the specified 
$icode integrand_id$$.
This is referred to as $codei%n_cov(%integrand_id%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%integrand_id%)%$$.

$head info$$
this return value has prototype
$codei%
	mulcov_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead smooth_id$$
is the $cref/smooth_id/smooth_table/smooth_id/$$ for the
$th j$$ covariate multiplier fro this $icode integrand_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number are age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset of the $icode n_var$$ variables in the packed variable vector.

$end
*/
size_t 
pack_var::meas_mean_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id].size();
}
//
pack_var::mulcov_info 
pack_var::meas_mean_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id][j];
}

} // END DISMOD_AT_NAMESPACE
