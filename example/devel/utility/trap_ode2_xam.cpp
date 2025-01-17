// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin trap_ode2_xam.cpp dev}
{xrst_spell
   trapezoidal
}

Example and Test of Trapezoidal Method of Solving ODE
#####################################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end trap_ode2_xam.cpp}
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/trap_ode2.hpp>
# include <dismod_at/eigen_ode2.hpp>

namespace {
   bool run_case(size_t case_number, const CppAD::vector<double>& b)
   {  bool ok = true;
      double tf = 0.01;
      CppAD::vector<double>  yi(2), yf(2), xf(2);
      yi[0] = 1.0;
      yi[1] = 2.0;
      //
      // compare result with eigen_ode2
      xf  = dismod_at::eigen_ode2(case_number, b, yi, tf);
      yf  = dismod_at::trap_ode2(b, yi, tf);
      double rel_error;
      rel_error = yf[0] / xf[0] - 1.0;
      ok       &= fabs( rel_error ) < 1e-5;
      rel_error = yf[1] / xf[1] - 1.0;
      ok       &= fabs( rel_error ) < 1e-5;
      //
      // check trapeziodal equalt for y^f_1
      double term = b[0] * (yi[0] + yf[0])  + b[1] * (yi[1] + yf[1]);
      double check = yi[0] + term * tf / 2.0;
      rel_error = yf[0] / check - 1.0;
      ok       &= fabs( rel_error ) < 1e-5;
      //
      // check trapeziodal equalt for y^f_2
      term     = b[2] * (yi[0] + yf[0])  + b[3] * (yi[1] + yf[1]);
      check     = yi[1] + term * tf / 2.0;
      rel_error = yf[1] / check - 1.0;
      ok       &= fabs( rel_error ) < 1e-5;
      return ok;
   }
}

bool trap_ode2_xam(void)
{  bool ok = true;
   typedef CppAD::vector<double> vector;
   vector b(4);
   size_t case_number;
   // -------------------------------------------------------------------
   // b1 = 0, b2 = 0
   b[0] = -3.0;  b[1] = 0.0;
   b[2] = 0.0;   b[3] = -2.0;
   case_number = 1;
   ok &= run_case(case_number, b);
   // -------------------------------------------------------------------
   // b1 != 0, b2 = 0, b0 != b3
   b[0] = -3.0 , b[1] =  1.0;
   b[2] =  0.0 , b[3] = -2.0;
   case_number = 2;
   ok &= run_case(case_number, b);
   // -------------------------------------------------------------------
   // b1 = 0, b2 != 0, b0 != b3
   b[0] = -3.0 , b[1] =  0.0;
   b[2] =  1.0 , b[3] = -2.0;
   case_number = 3;
   ok &= run_case(case_number, b);
   // -------------------------------------------------------------------
   // b1 != 0, b2 != 0
   b[0] = -3.0 , b[1] =  1.0;
   b[2] =  1.0 , b[3] = -3.0;
   case_number = 4;
   ok &= run_case(case_number, b);
   // -------------------------------------------------------------------
   return ok;
}
// END C++
