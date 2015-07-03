// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

/*
$begin pack_prior$$
$spell
	vec
	const
	CppAD
$$

$section Prior Ids In Same Order As Packed Variables$$

$head Syntax$$
$icode%value_prior% = pack_value_prior(%pack_object%, %s_info_vec%)%$$

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref devel_pack_info$$ information corresponding to
the $cref/model_variables/model_variable/$$.

$head s_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info>& %s_info_vec%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.

$head value_prior$$
The return value has prototype
$codei%
	CppAD::vector<size_t> %value_prior%
%$$
The element $icode%value_prior%[%i%]%$$ is the
$cref/prior_id/prior_table/prior_id/$$ corresponding to the
$th i$$ model variable in the order determined by $icode pack_object$$.

$children%
	example/devel/utility/pack_prior_xam.cpp
%$$
$head Example$$
The file $cref pack_prior_xam.cpp$$
contains an example and test that uses this routine.

$end
*/
# include <dismod_at/pack_prior.hpp>

namespace {

	void set_value_prior(
		CppAD::vector<size_t>&            ret_val   ,
		size_t                            offset    ,
		const dismod_at::smooth_info&     s_info  )
	{
		size_t n_age     = s_info.age_size();
		size_t n_time    = s_info.time_size();
		for(size_t i = 0; i < n_age; i++)
		{	for(size_t j = 0; j < n_time; j++)
			{	size_t index   = offset + i * n_time + j;
				ret_val[index] = s_info.value_prior_id(i, j);
			}
		}
	}

}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

CppAD::vector<size_t> pack_value_prior(
	const pack_info&                     pack_object  ,
	const CppAD::vector<smooth_info>&    s_info_vec   )
{	pack_info::subvec_info info;

	// size the return value
	CppAD::vector<size_t> ret_val( pack_object.size() );

	// some parameters
	size_t n_smooth    = s_info_vec.size();
	size_t n_child     = pack_object.child_size();
	size_t n_integrand = pack_object.integrand_size();

	// get priors for smoothing multipliers
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	// mulstd
		size_t offset     = pack_object.mulstd_offset(smooth_id);
		size_t prior_id   = s_info_vec[smooth_id].mulstd_value();
		ret_val[offset+0] = prior_id;
		prior_id          = s_info_vec[smooth_id].mulstd_dage();
		ret_val[offset+1] = prior_id;
		prior_id          = s_info_vec[smooth_id].mulstd_dtime();
		ret_val[offset+2] = prior_id;
	}

	// get priors for rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	for(size_t j = 0; j <= n_child; j++)
		{	info             = pack_object.rate_info(rate_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_value_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
	}

	// get priors for rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_mean_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_rate_mean_info(rate_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_value_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
	}

	// get prioirs for measurement covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	// measurement mean covariates for this integrand
		size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_value_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_value_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
		// measurement std covariates for this integrand
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_std_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_value_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
	}

	// done
	return ret_val;
}

} // END_DISMOD_AT_NAMESPACE
