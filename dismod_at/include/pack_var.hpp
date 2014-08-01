// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_VARIABLE_VEC_HPP
# define DISMOD_AT_VARIABLE_VEC_HPP

# include <cppad/vector.hpp>
# include "get_node_table.hpp"
# include "get_data_table.hpp"
# include "get_smooth_table.hpp"
# include "get_mulcov_table.hpp"
# include "get_rate_table.hpp"
# include "smooth_info.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

class pack_var {
	// BEGIN MULCOV_INFO
	typedef struct { 
		size_t covariate_id;
		size_t smooth_id; 
		size_t n_var; 
		size_t offset; 
	} mulcov_info;
	// END MULCOV_INFO
private:
	// number of smoothings
	const size_t n_smooth_;

	// number of integrands
	const size_t n_integrand_;

	// offsets for mulstd variables
	size_t offset_value_mulstd_;
	size_t offset_dage_mulstd_;
	size_t offset_dtime_mulstd_;

	// meas_mean_mulcov
	CppAD::vector< CppAD::vector<mulcov_info> > meas_mean_mulcov_info_;

	// total number of elements in the packed vector
	size_t size_;
public:
	pack_var(
		size_t                              n_integrand  ,
		const CppAD::vector<smooth_struct>& smooth_table ,
		const CppAD::vector<mulcov_struct>& mulcov_table 
	);
	// size
	size_t size(void) const;

	// mulstd
	size_t value_mulstd(size_t smooth_id) const;
	size_t  dage_mulstd(size_t smooth_id) const;
	size_t dtime_mulstd(size_t smooth_id) const;

	// meas_mean_mulcov_
	size_t      meas_mean_mulcov_n_cov(size_t integrand_id) const;
	mulcov_info meas_mean_mulcov_info(size_t integrand_id, size_t j) const;
};

} // END DISMOD_AT_NAMESPACE:
# endif
