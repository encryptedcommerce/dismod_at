/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <cppad/mixed/exception.hpp>
# include <dismod_at/adj_integrand.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/grid2line.hpp>
# include <dismod_at/cohort_ode.hpp>

/*
$begin adj_integrand$$
$spell
	vec
	const
	CppAD
$$

$section Compute Adjusted Integrand On a Cohort$$

$head Syntax$$
$icode%adjust_integrand% = adj_integrand(
	%rate_case%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%s_info_vec%,
	%integrand_id%,
	%n_child%,
	%child%,
	%x%,
	%line_age%,
	%line_time%,
	%pack_object%,
	%pack_vec%
)%$$

$head Prototype$$
$srcfile%devel/model/adj_integrand.cpp%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head n_line$$
We use the notation $icode n_line$$ for the number
points at which the approximate solution is returned.

$head rate_case$$
This is the value of
$cref/rate_case/option_table/rate_case/$$ in the option table.

$head age_table$$
This argument is the $cref age_table$$.

$head time_table$$
This argument is the $cref time_table$$.

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
None of the prior information in $icode s_info_vec$$ is used.

$head integrand_table$$
This argument is the $cref integrand_table$$.

$head integrand_id$$
This is the $cref/integrand_id/integrand_table/integrand_id/$$
in the integrand table.

$head n_child_$$
set to number of $cref/children/option_table/parent_node_id/Children/$$.

$head child$$
Is the $cref/child/child_info/table_id2child/child/$$ corresponding
to this adjustment of the integrand.

$head line_age$$
This vector has size $icode n_line$$ and is
the age points at which the adjusted integrand is computed.
It must be monotone non-decreasing; i.e.,
$codei%
	%line_age%[%k%] <= %line_age%[%k%+1]
%$$

$head line_time$$
This vector has size $icode n_line$$ and is
the time points at which the adjusted integrand is computed.
It must be monotone non-decreasing; i.e.,
$codei%
	%line_time%[%k%] <= %line_time%[%k%+1]
%$$

$subhead ODE$$
In the case where the integrand (specified by $icode integrand_id$$)
requires solving the
$cref/ODE/integrand_table/integrand_name/ODE/$$,
the line must be a cohort; i.e.,
for $icode%k% = 1 , %...%, %n_line%-1%$$
$codei%
	%line_time%[%k%] - %line_age%[%k%] == %line_time%[0] - %line_age%[0]
%$$
In this case
$icode cohort_age$$ and $icode cohort_time$$ are better names for the
arguments $icode line_age$$ and $icode line_time$$.

$head pack_object$$
This is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $icode pack_object$$.

$head adjust_integrand$$
The return value is a vector with size $icode n_line$$
and $icode%adjust_integrand%[%i%]%$$ is the
$cref/adjusted integrand/avg_integrand/Adjusted Integrand/$$
at age $icode%line_age%[%i%]%$$
and time $icode%line_time%[%i%]%$$.

$children%example/devel/model/adj_integrand_xam.cpp
%$$
$head Example$$
The file $cref adj_integrand_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
template <class Float>
CppAD::vector<Float> adj_integrand(
	const std::string&                        rate_case        ,
	const CppAD::vector<double>&              age_table        ,
	const CppAD::vector<double>&              time_table       ,
	const CppAD::vector<integrand_struct>&    integrand_table  ,
	const CppAD::vector<smooth_info>&         s_info_vec       ,
	size_t                                    integrand_id     ,
	size_t                                    n_child          ,
	size_t                                    child            ,
	const CppAD::vector<double>&              x                ,
	const CppAD::vector<double>&              line_age         ,
	const CppAD::vector<double>&              line_time        ,
	const pack_info&                          pack_object      ,
	const CppAD::vector<Float>&               pack_vec         )
// END_PROTOTYPE
{	using CppAD::vector;
	//
	// some temporaries
	pack_info::subvec_info info;
	vector<Float> smooth_value;
	vector< vector<Float> > rate(number_rate_enum);
	// ---------------------------------------------------------------------
	// integrand for this data point
	integrand_enum integrand = integrand_table[integrand_id].integrand;
	bool need_ode = false;
	vector<bool> need_rate(number_rate_enum);
	for(size_t k = 0; k < number_rate_enum; ++k)
		need_rate[k] = false;
	switch( integrand )
	{
		// -----------------------------------------------------------------
		// need_ode = true;
		case susceptible_enum:
		case withC_enum:
		case prevalence_enum:
		case Tincidence_enum:
		case mtspecific_enum:
		case mtall_enum:
		case mtstandard_enum:
		need_ode = true;
		//
		// need_rate = true
		for(size_t k = 0; k < number_rate_enum; ++k)
			need_rate[k] = true;
		break;

		// -----------------------------------------------------------------
		case Sincidence_enum:
		need_rate[iota_enum] = true;
		break;

		case remission_enum:
		need_rate[rho_enum] = true;
		break;

		case mtexcess_enum:
		need_rate[chi_enum] = true;
		break;

		case mtother_enum:
		need_rate[omega_enum] = true;
		break;

		case mtwith_enum:
		need_rate[omega_enum] = true;
		need_rate[chi_enum]   = true;
		break;

		case relrisk_enum:
		need_rate[chi_enum]   = true;
		need_rate[omega_enum] = true;
		break;

		// -----------------------------------------------------------------
		default:
		need_ode = false;
	}
	// number of points in line
	size_t n_line = line_age.size();
	//
	// vector of effects
	vector<Float> effect(n_line), temp(n_line);
	// -----------------------------------------------------------------------
	// get value for each rate that is needed
	for(size_t rate_id = 0; rate_id < number_rate_enum; ++rate_id)
	if( need_rate[rate_id] )
	{	rate[rate_id].resize(n_line);
		//
		// parent rate for each point in the line
		info             = pack_object.rate_info(rate_id, n_child);
		size_t smooth_id = info.smooth_id;
		//
		if( smooth_id == DISMOD_AT_NULL_SIZE_T )
		{	for(size_t k = 0; k < n_line; ++k)
				rate[rate_id][k] = 0.0;
		}
		else
		{	// interpolate this rate from smoothing grid to line
			smooth_value.resize(info.n_var);
			for(size_t k = 0; k < info.n_var; ++k)
				smooth_value[k] = pack_vec[info.offset + k];
			const smooth_info& s_info = s_info_vec[smooth_id];
			rate[rate_id] = grid2line(
				line_age,
				line_time,
				age_table,
				time_table,
				s_info,
				smooth_value
			);
		}
		//
		// initialize effect as zero
		for(size_t k = 0; k < n_line; ++k)
			effect[k] = 0.0;
		//
		// include the child effect
		if( child < n_child )
		{	// child effect rate for each point in the line
			info      = pack_object.rate_info(rate_id, child);
			smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	// interpolate from smoothing grid to line
				smooth_value.resize(info.n_var);
				for(size_t k = 0; k < info.n_var; ++k)
					smooth_value[k] = pack_vec[info.offset + k];
				const smooth_info& s_info = s_info_vec[smooth_id];
				temp = grid2line(
					line_age,
					line_time,
					age_table,
					time_table,
					s_info,
					smooth_value
				);
				for(size_t k = 0; k < n_line; ++k)
					effect[k] += temp[k];
			}
		}
		//
		// include the covariate effects on this rate
		size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; ++j)
		{	info        = pack_object.mulcov_rate_value_info(rate_id, j);
			smooth_id   = info.smooth_id;
			double x_j  = x[ info.covariate_id ];
			// interpolate from smoothing grid to line
			smooth_value.resize(info.n_var);
			for(size_t k = 0; k < info.n_var; ++k)
				smooth_value[k] = pack_vec[info.offset + k];
			const smooth_info& s_info = s_info_vec[smooth_id];
			temp = grid2line(
				line_age,
				line_time,
				age_table,
				time_table,
				s_info,
				smooth_value
			);
			for(size_t k = 0; k < n_line; ++k)
				effect[k] += temp[k] * x_j;
		}
		//
		// multiply parent rate by exponential of the total effect
		for(size_t k = 0; k < n_line; ++k)
			rate[rate_id][k] *= exp( effect[k] );
	}
	// -----------------------------------------------------------------------
	// solve the ode on the cohort specified by line_age and line_time[0]
	vector<Float> s_out(n_line), c_out(n_line);
	if( need_ode )
	{
# ifndef NDEBUG
		Float eps99  = 99.0 * CppAD::numeric_limits<Float>::epsilon();
		Float check  = line_time[0] - line_age[0];
		for(size_t k = 1; k < n_line; ++k)
		{	Float diff = line_time[k] - line_age[k];
			CppAD::NearEqual(diff, check, eps99, eps99);
		}
# endif
		Float pini = rate[pini_enum][0];
		cohort_ode(
			rate_case,
			line_age,
			pini,
			rate[iota_enum],
			rate[rho_enum],
			rate[chi_enum],
			rate[omega_enum],
			s_out,
			c_out
		);
	}
# ifndef NDEBUG
	else
	{	for(size_t k = 0; k < n_line; ++k)
		{	s_out[k] = CppAD::numeric_limits<Float>::quiet_NaN();
			c_out[k] = CppAD::numeric_limits<Float>::quiet_NaN();
		}
	}
# endif
	// -----------------------------------------------------------------------
	// value of the integrand on the line
	vector<Float> result(n_line);
	Float infinity = std::numeric_limits<double>::infinity();
	Float zero     =  0.0;
	for(size_t k = 0; k < n_line; ++k)
	{	Float P = c_out[k] / ( s_out[k] + c_out[k] );
		bool ok = zero <= P && P < infinity;
		ok     |= integrand == susceptible_enum;
		ok     |= integrand == withC_enum;
		ok     |= ! need_ode;
		if( ! ok )
		{	std::string message = "Numerical integration error.\n"
			"Need Prevalence and it is negative or infinite or Nan.";
			throw CppAD::mixed::exception( "avg_integrand", message);
		}
		switch(integrand)
		{	// ----------------------------------------------------------------
			// no ode cases
			// ----------------------------------------------------------------

			case Sincidence_enum:
			result[k] = rate[iota_enum][k];
			break;

			case remission_enum:
			result[k] = rate[rho_enum][k];
			break;

			case mtexcess_enum:
			result[k] = rate[chi_enum][k];
			break;

			case mtother_enum:
			result[k] = rate[omega_enum][k];
			break;

			case mtwith_enum:
			result[k] = rate[omega_enum][k] + rate[chi_enum][k];
			break;

			case relrisk_enum:
			result[k] = 1.0 + rate[chi_enum][k] / rate[omega_enum][k];
			break;

			// ----------------------------------------------------------------
			// need_ode cases
			// ----------------------------------------------------------------

			case susceptible_enum:
			result[k] = s_out[k];
			break;

			case withC_enum:
			result[k] = c_out[k];
			break;

			case prevalence_enum:
			result[k] = P;
			break;

			case Tincidence_enum:
			result[k] = rate[iota_enum][k] * (1.0 - P);
			break;

			case mtspecific_enum:
			result[k] = rate[chi_enum][k] * P;
			break;

			case mtall_enum:
			result[k] = rate[omega_enum][k] + rate[chi_enum][k] * P;
			break;

			case mtstandard_enum:
			result[k]  = rate[omega_enum][k] + rate[chi_enum][k];
			result[k] /= rate[omega_enum][k] + rate[chi_enum][k] * P;
			break;

			// ---------------------------------------------------------------
			default:
			assert(false);
			// ---------------------------------------------------------------
		}
	}
	// -----------------------------------------------------------------------
	// include measurement effects
	//
	// initialize effect as zero
	for(size_t k = 0; k < n_line; ++k)
		effect[k] = 0.0;
	size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
	for(size_t j = 0; j < n_cov; ++j)
	{	info             = pack_object.mulcov_meas_value_info(integrand_id, j);
		size_t smooth_id = info.smooth_id;
		double x_j       = x[ info.covariate_id ];
		// interpolate from smoothing grid to cohort
		smooth_value.resize(info.n_var);
		for(size_t k = 0; k < info.n_var; ++k)
			smooth_value[k] = pack_vec[info.offset + k];
		const smooth_info& s_info = s_info_vec[smooth_id];
		temp = grid2line(
			line_age,
			line_time,
			age_table,
			time_table,
			s_info,
			smooth_value
		);
		for(size_t k = 0; k < n_line; ++k)
			effect[k] += temp[k] * x_j;
	}
	// multiply by exponential of total effect
	for(size_t k = 0; k < n_line; ++k)
		 result[k] *= exp( effect[k] );
	//
	return result;
}

# define DISMOD_AT_INSTANTIATE_ADJ_INTEGTAND(Float)                     \
    template                                                            \
	CppAD::vector<Float> adj_integrand(                                 \
		const std::string&                        rate_case        ,    \
		const CppAD::vector<double>&              age_table        ,    \
		const CppAD::vector<double>&              time_table       ,    \
		const CppAD::vector<integrand_struct>&    integrand_table  ,    \
		const CppAD::vector<smooth_info>&         s_info_vec       ,    \
		size_t                                    integrand_id     ,    \
		size_t                                    n_child          ,    \
		size_t                                    child            ,    \
		const CppAD::vector<double>&              x                ,    \
		const CppAD::vector<double>&              line_age         ,    \
		const CppAD::vector<double>&              line_time        ,    \
		const pack_info&                          pack_object      ,    \
		const CppAD::vector<Float>&               pack_vec              \
	);

// instantiations
DISMOD_AT_INSTANTIATE_ADJ_INTEGTAND( double )
DISMOD_AT_INSTANTIATE_ADJ_INTEGTAND( a1_double )

} // END_DISMOD_AT_NAMESPACE
