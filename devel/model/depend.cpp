// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/depend.hpp>
# include <dismod_at/a1_double.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-- ---------------------------------------------------------------------------
{xrst_begin data_depend dev}
{xrst_spell
   sparsity
}

Which Variables The Data Model Depends On
#########################################

Syntax
******
*depend* = ``data_depend`` ( *pack_vec* , *data_object* )

Prototype
*********
{xrst_literal
   // BEGIN DATA_DEPEND PROTOTYPE
   // END DATA_DEPEND PROTOTYPE
}

pack_vec
********
This can be any value for the :ref:`model_variables-name`
at which one can evaluate the likelihood for the data.

data_object
***********
This object contains and evaluates the likelihood for the data.
It is effectively const.

depend
******
The return value *depend* has the same size as *pack_vec*
and is a sparsity pattern for the data likelihood as a function of
the model variables; i.e., if it is false, the derivatives of the
data likelihood w.r.t. the corresponding model variable is always zero.

{xrst_end data_depend}
*/
// BEGIN DATA_DEPEND PROTOTYPE
CppAD::vector<bool> data_depend(
   const CppAD::vector<double>& pack_vec     ,
   data_model&                  data_object  )
// END DATA_DEPEND PROTOTYPE
{
   typedef CppAD::vector<a1_double> a1_vector;
   //
   // declare a1_pack_vec to be independent variable vector
   size_t n_var = pack_vec.size();
   a1_vector a1_pack_vec(n_var);
   for(size_t i = 0; i < n_var; i++)
      a1_pack_vec[i] = pack_vec[i];
   CppAD::Independent( a1_pack_vec );
   //
   // initialize sum of the likelihood of all the data
   a1_double a1_sum = 0.0;
   for(size_t random = 0; random < 2; random++)
   {  // compute likelihood for fixed or random effects part
      bool random_depend = bool( random );
      bool hold_out      = true;
      CppAD::vector< residual_struct<a1_double> > a1_like_vec =
         data_object.like_all(hold_out, random_depend, a1_pack_vec);
      //
      // add this part to the sum
      size_t n_like = a1_like_vec.size();
      for(size_t i = 0; i < n_like; i++)
      {  a1_sum += a1_like_vec[i].logden_smooth;
         density_enum density = a1_like_vec[i].density;
         if( density == laplace_enum || density == log_laplace_enum )
            a1_sum -= fabs( a1_like_vec[i].logden_sub_abs );
      }
   }
   //
   // declare dependent variable
   a1_vector a1_like_sum(1);
   a1_like_sum[0] = a1_sum;
   CppAD::ADFun<double> f;
   f.Dependent(a1_pack_vec, a1_like_sum);
   //
   // 2DO: test to see if this makes release version faster
# ifndef NDEBUG
   f.optimize("no_conditional_skip");
# endif
   typedef CppAD::sparse_rc< CppAD::vector<size_t> > sparsity_pattern;
   //
   // sparsity pattern for 1 by 1 identity matrix
   size_t nr = 1, nc = 1, nnz = 1;
   sparsity_pattern pattern_in(nr, nc, nnz);
   pattern_in.set(0, 0, 0);
   //
   // only interested in non-zero derivatives so set dependency false
   bool dependency    = false;
   // vectors of integers instead of bools for CppAD internal representation
   bool internal_bool = false;
   // no need to traspose input and output patterns
   bool transpose     = false;
   // result returned here
   sparsity_pattern pattern_out;
   // compute pattern_out
   f.rev_jac_sparsity(
      pattern_in, transpose, dependency, internal_bool, pattern_out
   );
   assert( pattern_out.nr() == 1 );
   assert( pattern_out.nc() == n_var );
   //
   // compute the dependency vector
   CppAD::vector<bool> result(n_var);
   for(size_t i = 0; i < n_var; i++)
      result[i] = false;
   for(size_t k = 0; k < pattern_out.nnz(); k++)
   {  size_t c  = pattern_out.col()[k];
      result[c] = true;
   }
   //
   return result;
}
/*
-- ---------------------------------------------------------------------------
{xrst_begin prior_depend dev}
{xrst_spell
   sparsity
}

Which Variables The Prior Depends On
####################################

Syntax
******
*depend* = ``prior_depend`` ( *pack_vec* , *prior_object* )

Prototype
*********
{xrst_literal
   // BEGIN PRIOR_DEPEND PROTOTYPE
   // END PRIOR_DEPEND PROTOTYPE
}

pack_vec
********
This can be any value for the :ref:`model_variables-name`
at which one can evaluate the likelihood for the prior.

prior_object
************
This object contains and evaluates the prior.

depend
******
The return value *depend* has the same size as *pack_vec*
and is a sparsity pattern for the prior as a function of
the model variables; i.e., if it is false, the derivatives of the
prior w.r.t. the corresponding model variable is always zero.

{xrst_end prior_depend}
*/
// BEGIN PRIOR_DEPEND PROTOTYPE
CppAD::vector<bool> prior_depend(
   const CppAD::vector<double>&   pack_vec     ,
   const prior_model&            prior_object  )
// END PRIOR_DEPEND PROTOTYPE
{
   typedef CppAD::vector<a1_double> a1_vector;
   //
   // declare a1_pack_vec to be independent variable vector
   size_t n_var = pack_vec.size();
   a1_vector a1_pack_vec(n_var);
   for(size_t i = 0; i < n_var; i++)
      a1_pack_vec[i] = pack_vec[i];
   CppAD::Independent( a1_pack_vec );
   //
   // initialize sum of the likelihood of all the priors
   a1_double a1_sum = 0.0;
   for(size_t random = 0; random < 2; random++)
   {
      // compute likelihood for fixed or random effects part
      CppAD::vector< residual_struct<a1_double> > a1_like_vec;
      if( random == 1 )
         a1_like_vec = prior_object.random(a1_pack_vec);
      else
         a1_like_vec = prior_object.fixed(a1_pack_vec);
      //
      // add this part to the sum
      size_t n_like = a1_like_vec.size();
      for(size_t i = 0; i < n_like; i++)
      {  a1_sum += a1_like_vec[i].logden_smooth;
         density_enum density = a1_like_vec[i].density;
         if( density == laplace_enum || density == log_laplace_enum )
            a1_sum -= fabs( a1_like_vec[i].logden_sub_abs );
      }
   }
   //
   // declare dependent variable
   a1_vector a1_like_sum(1);
   a1_like_sum[0] = a1_sum;
   CppAD::ADFun<double> f;
   f.Dependent(a1_pack_vec, a1_like_sum);
   //
   // 2DO: test to see if this makes release version faster
# ifndef NDEBUG
   f.optimize("no_conditional_skip");
# endif
   typedef CppAD::sparse_rc< CppAD::vector<size_t> > sparsity_pattern;
   //
   // sparsity pattern for 1 by 1 identity matrix
   size_t nr = 1, nc = 1, nnz = 1;
   sparsity_pattern pattern_in(nr, nc, nnz);
   pattern_in.set(0, 0, 0);
   //
   // only interested in non-zero derivatives so set dependency false
   bool dependency    = false;
   // vectors of integers instead of bools for CppAD internal representation
   bool internal_bool = false;
   // no need to traspose input and output patterns
   bool transpose     = false;
   // result returned here
   sparsity_pattern pattern_out;
   // compute pattern_out
   f.rev_jac_sparsity(
      pattern_in, transpose, dependency, internal_bool, pattern_out
   );
   assert( pattern_out.nr() == 1 );
   assert( pattern_out.nc() == n_var );
   //
   // compute the dependency vector
   CppAD::vector<bool> result(n_var);
   for(size_t i = 0; i < n_var; i++)
      result[i] = false;
   for(size_t k = 0; k < pattern_out.nnz(); k++)
   {  size_t c  = pattern_out.col()[k];
      result[c] = true;
   }
   //
   return result;
}

} // END_DISMOD_AT_NAMESPACE
