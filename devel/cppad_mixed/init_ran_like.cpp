// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <cppad/mixed/cppad_mixed.hpp>

/*
$begin init_ran_like$$
$spell
	init
	cppad
	vec
	const
	Cpp
$$

$section cppad_mixed: Initialize Random Likelihood$$

$head Syntax$$
$codei%init_ran_like(%fixed_vec%, %random_vec%)%$$

$head Private$$
This function is $code private$$ to the $code cppad_mixed$$ class
and cannot be used by a derived
$cref/mixed_object/cppad_mixed_derived_ctor/mixed_object/$$.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/cppad_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which the initialization is done.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/cppad_mixed/Random Effects, u/$$
vector $latex u$$ at which the initialization is done.

$head ran_like_fun_$$
The input value of the member variable
$codei%
	CppAD::ADFun<double> ran_like_fun_
%$$
does not matter.
Upon return it contains a recording of the function
$cref/ran_like/cppad_mixed_ran_like/$$.

$head ran_like_a1fun_$$
The input value of the member variable
$codei%
	CppAD::ADFun<double> ran_like_a1fun_
%$$
does not matter.
Upon return it contains a recording of the function
$cref/ran_like/cppad_mixed_ran_like/$$.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void cppad_mixed::init_ran_like(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( ! init_ran_like_done_ );
	//
	using CppAD::AD;
	using CppAD::ADFun;
	using CppAD::vector;
	using CppAD::Independent;
	//
	// ------------------------------------------------------------------
	// record ran_like_a1fun_
	// ------------------------------------------------------------------
	// combine into one vector
	a2d_vector a2_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a2_both);

	// start recording a2_double operations
	Independent(a2_both);

	// extract the fixed and random effects
	a2d_vector a2_theta(n_fixed_), a2_u(n_random_);
	unpack(a2_theta, a2_u, a2_both);

	// compute ran_like using a2_double operations
	a2d_vector a2_vec = ran_like(a2_theta, a2_u);
	if( a2_vec.size() == 0 )
	{	std::string error_message =
		"cppad_mixed: number of random effects > 0 and ran_like has size 0";
		fatal_error(error_message);
	}
	if( a2_vec.size() != 1 )
	{	std::string error_message =
		"cppad_mixed: ran_like does not have size zero or one.";
		fatal_error(error_message);
	}

	// save the recording
	ran_like_a1fun_.Dependent(a2_both, a2_vec);

	// optimize the recording
# ifndef NDEBUG
	ran_like_a1fun_.optimize();
# endif
	// ------------------------------------------------------------------
	// record ran_like_fun_
	// ------------------------------------------------------------------
	// combine into one vector
	a1d_vector a1_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a1_both);

	// start recording a1_double operations
	Independent(a1_both);

	// extract the fixed and random effects
	a1d_vector a1_theta(n_fixed_), a1_u(n_random_);
	unpack(a1_theta, a1_u, a1_both);

	// compute ran_like using a1_double operations
	a1d_vector a1_vec = ran_like(a1_theta, a1_u);
	if( a1_vec.size() == 0 )
	{	std::string error_message =
		"cppad_mixed: number of random effects > 0 and ran_like has size 0";
		fatal_error(error_message);
	}
	if( a1_vec.size() != 1 )
	{	std::string error_message =
		"cppad_mixed: ran_like does not have size zero or one.";
		fatal_error(error_message);
	}

	// save the recording
	ran_like_fun_.Dependent(a1_both, a1_vec);

	// optimize the recording
# ifndef NDEBUG
	ran_like_fun_.optimize();
# endif
	// ------------------------------------------------------------------
	init_ran_like_done_ = true;
	return;
}


} // END_DISMOD_AT_NAMESPACE
