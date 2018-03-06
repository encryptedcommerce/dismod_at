// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <iostream>
# include <cassert>
# include <cstring>

// this directory
extern bool age_time_order(void);
extern bool data_model_subset(void);
extern bool meas_mulcov(void);
extern bool rate_mulcov(void);
extern bool cppad_mixed_xam(void);

// anonymous namespace
namespace {
	using std::cout;
	using std::endl;

	// function that runs one test
	static size_t Run_ok_count    = 0;
	static size_t Run_error_count = 0;
	void Run(bool test_fun(void), const char* test_name)
	{
		std::streamsize width = 30;
		cout.width( width );
		cout.setf( std::ios_base::left );
		cout << test_name << ':';
		assert( std::strlen(test_name) < size_t(width) );
		//
		bool ok = test_fun();
		if( ok )
		{	cout << "OK" << endl;
			Run_ok_count++;
		}
		else
		{	cout << "Error" << endl;
			Run_error_count++;
		}
	}
}
// macro for calls Run
# define RUN(test_name) Run( test_name, #test_name )

// main program that runs all the tests
int main(void)
{
	// this directory
	RUN(age_time_order);
	RUN(data_model_subset);
	RUN(meas_mulcov);
	RUN(rate_mulcov);
	RUN(cppad_mixed_xam);

	// summary report
	int return_flag;
	if( Run_error_count == 0 )
	{	cout << "All " << Run_ok_count << " tests passed." << endl;
		return_flag = 0;
	}
	else
	{	cout << Run_error_count << " tests failed." << endl;
		return_flag = 1;
	}
	return return_flag;
}