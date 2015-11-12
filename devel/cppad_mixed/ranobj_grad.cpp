// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/cppad_mixed.hpp>
# include <Eigen/Sparse>
/*
$begin ranobj_grad$$
$spell
	ranobj
	cppad
	hes
	vec
	const
	Cpp
	xam
$$

$section Derivative of Random Objective$$

$head Syntax$$
$icode%mixed_object%.ranobj_grad(%fixed_vec%, %random_vec%, %r_fixed%)%$$

$head Purpose$$
This routine computes the
$cref/derivative of the random objective
	/cppad_mixed_theory/
	Derivative of Random Objective
/$$.

$head mixed_object$$
We use $cref/mixed_object/cppad_mixed_derived_ctor/mixed_object/$$
to denote an object of a class that is
derived from the $code cppad_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
and is the value of fixed effects $latex \theta$$.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
and is the value of fixed effects $latex u$$.
It is assumed that these random effects are optimal for the specified
fixed effects and hence $latex f_u^{(1)} ( \theta , u ) = 0$$.

$head r_fixed$$
This argument has prototype
$codei%
	CppAD::vector<double>& %r_fixed%
%$$
If the input size must be equal to $code n_fixed_$$.
Upon return, it contains the value of the derivative w.r.t
the fixed effects; i.e. $latex r^{(1)} ( \theta )$$.

$children%
	example/devel/cppad_mixed/private/ranobj_grad_xam.cpp
%$$
$head Example$$
The file $cref ranobj_grad_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
void cppad_mixed::ranobj_grad(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec ,
	d_vector&       r_fixed    )
{
	assert( fixed_vec.size() == n_fixed_ );
	assert( random_vec.size() == n_random_ );
	assert( r_fixed.size() == n_fixed_ );

	// declare eigen matrix types
	typedef Eigen::SparseMatrix<double>                sparse_matrix;
	typedef Eigen::SparseMatrix<double>::InnerIterator inner_itr;

	// number of non-zeros in Hessian
	size_t K = hes_ran_.row.size();
	assert( K == hes_ran_.col.size() );

	// evaluate the hessian f_{uu}^{(2)} (theta, u)
	d_vector both(n_fixed_ + n_random_), val_out(K);
	pack(fixed_vec, random_vec, both);
	val_out = hes_ran_fun_.Forward(0, both);

	// create a lower triangular eigen sparse matrix representation of Hessian
	// 2DO: only do analyze pattern once and store in chol
	// 2DO: same hessian point is factorized here as well as in logdet_grad.
	sparse_matrix hessian(n_random_, n_random_);
	for(size_t k = 0; k < K; k++)
	{	assert( n_fixed_        <= hes_ran_.col[k]  );
		assert( hes_ran_.col[k] <= hes_ran_.row[k] );
		size_t row = hes_ran_.row[k] - n_fixed_;
		size_t col = hes_ran_.col[k] - n_fixed_;
		assert( row < n_random_ );
		assert( col < n_random_ );
		hessian.insert(row, col) = val_out[k];
	}
	// compute an LDL^T Cholesky factorization of f_{uu}^{(2)}(theta, u)
	Eigen::SimplicialLDLT<sparse_matrix, Eigen::Lower> chol;
	chol.analyzePattern(hessian);
	chol.factorize(hessian);
	//
	// Compute derivative of logdet of f_{uu}^{(2)} ( theta , u )
	d_vector logdet_fix(n_fixed_), logdet_ran(n_random_);
	logdet_grad(fixed_vec, random_vec, logdet_fix, logdet_ran);
	//
	// Compute derivative of f(theta , u ) w.r.t theta and u
	d_vector w(1), f_both(n_fixed_ + n_random_);
	w[0] = 1.0;
	ran_like_fun_.Forward(0, both);
	f_both = ran_like_fun_.Reverse(1, w);
	d_vector f_fixed(n_fixed_), f_random(n_random_);
	unpack(f_fixed, f_random, f_both);
	//
	// Compute the Hessian cross terms f_{u theta}^{(2)} ( theta , u )
	// 2DO: another ran_like_fun_.Forward(0, both) is done by SparseHessian
	CppAD::vector< std::set<size_t> > not_used;
	K = hes_cross_.row.size();
	val_out.resize(K);
	ran_like_fun_.SparseHessian(
		both,
		w,
		not_used,
		hes_cross_.row,
		hes_cross_.col,
		val_out,
		hes_cross_.work
	);
	// initialize index in hes_cross_.row, hes_cross_.col
	size_t k = 0;
	size_t col = n_fixed_;
	if( k < K )
		col = hes_cross_.col[k];
	assert( col <= n_fixed_ );
	//
	// Loop over fixed effects and compute r_fixed one component at a time
	for(size_t j = 0; j < n_fixed_; j++)
	{	// parial w.r.t fixed effects contribution to total derivative
		r_fixed[j] =  f_fixed[j] + 0.5 * logdet_fix[j];
		//
		// set b_i = - f_{u theta}^{(2)} (theta , u) ]_{i,j}
		sparse_matrix b(n_random_, 1);
		while( col < j )
		{	k++;
			if( k >= K )
				col = n_fixed_;
			else
			{	col = hes_cross_.col[k];
				assert( col < n_fixed_ );
			}
		}
		while( col == j )
		{	assert( hes_cross_.row[k] >= n_fixed_ );
			size_t row = hes_cross_.row[k] - n_fixed_;
			assert( row < n_random_ );
			b.insert(row, 0) = - val_out[k];
			k++;
			if( k >= K )
				col = n_fixed_;
			else
			{	col = hes_cross_.col[k];
				assert( col < n_fixed_ );
			}
		}
		//
		// compute the partial of uhat(theta) w.r.t theta[j]
		sparse_matrix x = chol.solve(b);
		//
		// compute effect on the total derivative
		assert( x.outerSize() == 1 );
		for(inner_itr itr(x, 0); itr; ++itr)
		{	// random effect index
			size_t i = itr.row();
			// partial of optimal random effect for this (i, j)
			double ui_thetaj = itr.value();
			// partial of random part of objective w.r.t this random effect
			double h_ui = f_random[i] + 0.5 * logdet_ran[i];
			// contribution to total derivative
			r_fixed[j] += h_ui * ui_thetaj;
		}
	}
	//
	return;
}

} // END_DISMOD_AT_NAMESPACE
