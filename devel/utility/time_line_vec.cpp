// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/time_line_vec.hpp>
# include <dismod_at/a1_double.hpp>

/*
$begin time_line_vec$$
$spell
	vec
	struct
$$

$section Creating a Vector of Time Lines For Sampling a Function$$

$head Syntax$$
$codei%time_line_vec<%Float%>::near_equal(double %x%, double %y%)
%$$
$codei%time_line_vec %vec%(%age_grid%)
%$$
$icode%vec%.specialize(
	%age_lower%, %age_upper%, %time_lower%, %time_upper%
)
%$$
$icode%extend_grid% = %vec%.extend_grid()
%$$
$icode%sub_lower% = %vec%.sub_lower()
%$$
$icode%sub_upper% = %vec%.sub_upper()
%$$
$icode%vec%.add_point(%age_index%, %point%)
%$$
$icode%time_line% = %vec%.time_line(%age_index%)
%$$
$icode%avg% = %vec%.age_time_avg()
%$$

$head Prototype$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_CONSTRUCTOR_PROTOTYPE%// END_CONSTRUCTOR_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_SPECIALIZE_PROTOTYPE%// END_SPECIALIZE_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_ADD_POINT_PROTOTYPE%// END_ADD_POINT_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_EXTEND_GRID_PROTOTYPE%// END_EXTEND_GRID_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_SUB_LOWER_PROTOTYPE%// END_SUB_LOWER_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_SUB_UPPER_PROTOTYPE%// END_SUB_UPPER_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_TIME_LINE_PROTOTYPE%// END_TIME_LINE_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_AGE_TIME_AVG_PROTOTYPE%// END_AGE_TIME_AVG_PROTOTYPE%1
%$$
$pre
$$

$head Purpose$$
The $code time_line_vec$$ class is used to create a vector of
time lines for sampling a function of age and time.

$head Float$$
The type $icode Float$$ is $code double$$ or $cref a1_double$$.

$head time_point$$
This structure is defined in the $codei%time_line_vec<%Float%>%$$ class
as follows:
$codei%
	struct time_point {double time; Float value; };
%$$

$head near_equal$$
Checks if $icode x$$ and $icode y$$ are nearly equal
to multiple of numerical precision that is greater than 10
and less than 100.

$head time_line_vec$$
This constructs the object $icode vec$$ for managing vectors of time lines.

$head age_grid$$
This vector specifies the grid for averaging w.r.t. age.
This vector is monotone increasing; i.e.,
$codei%
	%age_grid%[%j%] < %age_grid%[%j%+1]
%$$

$head specialize$$
This creates an extended age grid for averaging between
the specified lower and upper ages and times where
$codei%
	%age_lower%  <= %age_upper%
	%time_lower% <= %time_upper%
%$$
There is a time line for each sub grid point
and it is initialized as empty.

$head extend_grid$$
This return value is an extended age grid and is monotone increasing.
It include all the points in the original $icode age_grid$$
and the values $icode age_lower$$ and $icode age_upper$$.

$head sub_lower$$
This return value is the index in $icode extend_grid$$
where the sub grid starts;
$codei%
	true == near_equal( %age_lower%, %extend_grid%[ %sub_lower% ] )
%$$

$head sub_upper$$
This return value is the index in $icode extend_grid$$
where the sub grid ends;
$codei%
	true == near_equal( %age_upper%, %extend_grid%[ %sub_upper% ] )
%$$

$head age_index$$
This is the index, for the time line, in the extended age grid
$codei%
	%sub_lower% <= %age_index% <= %sub_upper%
%$$.

$head add_point$$
This adds a time point to the specified time line.

$subhead point$$
This is the time point that is added to the time line.
The value $icode%point%.value%$$ is the value of the function
that we are sampling at age $icode%extend_grid[%age_index%]%$$
and time $icode%point.time%$$.
The time must satisfy
$codei%
	%time_lower% <= %point%.time <= %time_upper%
%$$
In addition, two calls to $code add_point$$ cannot have the
same $icode age_index$$ and $icode%point%.time%$$.

$head time_line$$
This vector contains the points in the time line
that corresponds to the specified $icode age_index$$.
The vector monotone non-decreasing in time; i.e.,
$codei%
	%time_line%[%i%].time <= %time_line%[%i%+1].time
%$$

$head age_time_avg$$
This uses the
$cref numeric_average$$ method to approximate the average
of the sampled function with respect to the specified age and time limits.
Each time line corresponding to
$codei%
	%sub_lower% <= %age_index% <= %sub_upper%
%$$.
must have a point with time nearly equal to
$icode time_lower$$ and a point with time nearly equal to $icode time_upper$$.
If the upper and lower time limits are nearly equal,
only one call to $code add_point$$ for each time line is necessary.


$children%example/devel/utility/time_line_vec_xam.cpp
%$$
$head Example$$
The file $cref time_line_vec_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
bool time_line_vec<Float>::near_equal(double x, double y)
{	// some constants for near equal calculations
	double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
	double min99 = 99.0 * std::numeric_limits<double>::min();
	//
	double abs_x = std::fabs(x);
	double abs_y = std::fabs(y);
	//
	if( abs_x <= min99 && abs_y <= min99 )
		return true;
	//
	return std::abs(1.0 - x / y) <= eps99;
}

// BEGIN_CONSTRUCTOR_PROTOTYPE
template <class Float>
time_line_vec<Float>::time_line_vec(
	const CppAD::vector<double>& age_grid
)
// END_CONSTRUCTOR_PROTOTYPE
: age_grid_(age_grid)
{
# ifndef NDEBUG
	assert( 2 <= age_grid_.size() );
	for(size_t j = 1; j < age_grid.size(); ++j)
		assert( age_grid_[j-1] < age_grid_[j] );
# endif
}

// BEGIN_SPECIALIZE_PROTOTYPE
template <class Float>
void time_line_vec<Float>::specialize(
	const double& age_lower  ,
	const double& age_upper  ,
	const double& time_lower ,
	const double& time_upper )
// END_SPECIALIZE_PROTOTYPE
{	assert( time_lower <= time_upper );
	//
	assert( age_lower <= age_upper );
	assert( age_grid_[0] <= age_lower );
	assert( age_upper <= age_grid_[age_grid_.size() - 1] );
	// -----------------------------------------------------------------
	time_lower_ = time_lower;
	time_upper_ = time_upper;
	// -----------------------------------------------------------------
	// extend_grid_
	//
	size_t n_age     = age_grid_.size();
	size_t age_index = 0;
	extend_grid_.resize(0);
	//
	// ages < age_lower
	while( age_grid_[age_index] < age_lower )
	{	if( ! near_equal( age_grid_[age_index], age_lower ) )
			extend_grid_.push_back( age_grid_[age_index] );
		++age_index;
	}
	//
	// age_lower
	sub_lower_ = extend_grid_.size();
	sub_upper_ = sub_lower_;
	extend_grid_.push_back( age_lower );
	if( near_equal( age_grid_[age_index], age_lower ) )
		++age_index;
	//
	if( ! near_equal( age_lower, age_upper ) )
	{	//
		// ages < age_upper
		assert( age_index < n_age );
		while( age_grid_[age_index] < age_upper )
		{	if( ! near_equal( age_grid_[age_index], age_upper ) )
				extend_grid_.push_back( age_grid_[age_index] );
			++age_index;
		}
		//
		// age_upper
		sub_upper_ = extend_grid_.size();
		extend_grid_.push_back( age_upper );
	}
	if( near_equal( age_grid_[age_index], age_upper ) )
		++age_index;
	while( age_index < n_age )
	{	extend_grid_.push_back( age_grid_[age_index] );
		++age_index;
	}
	// -----------------------------------------------------------------
	// vec_
	size_t n_sub = sub_upper_ - sub_lower_ + 1;
	vec_.resize(n_sub);
	for(size_t j = 0; j < n_sub; ++j)
		vec_[j].resize(0);
}
// BEGIN_EXTEND_GRID_PROTOTYPE
template <class Float>
const CppAD::vector<double>& time_line_vec<Float>::extend_grid(void) const
// END_EXTEND_GRID_PROTOTYPE
{	return extend_grid_; }

// BEGIN_SUB_LOWER_PROTOTYPE
template <class Float>
size_t time_line_vec<Float>::sub_lower(void) const
// END_SUB_LOWER_PROTOTYPE
{	return sub_lower_; }

// BEGIN_SUB_UPPER_PROTOTYPE
template <class Float>
size_t time_line_vec<Float>::sub_upper(void) const
// END_SUB_UPPER_PROTOTYPE
{	return sub_upper_; }


// BEGIN_ADD_POINT_PROTOTYPE
template <class Float>
void time_line_vec<Float>::add_point(
	const size_t&     age_index ,
	const time_point& point     )
// END_ADD_POINT_PROTOTYPE
{	assert( time_lower_ <= point.time );
	assert( point.time <= time_upper_ );
	assert( sub_lower_ <= age_index );
	assert( age_index <= sub_upper_ );
	//
	// this time line
	CppAD::vector<time_point>& time_line = vec_[age_index - sub_lower_];
	size_t n_time = time_line.size();
	//
	// time index at which to insert this point
	size_t time_index = 0;
	while( time_index < n_time && time_line[time_index].time < point.time )
		++time_index;
	//
	// case where this point goes at the end of the line
	if( time_index == n_time )
	{	time_line.push_back(point);
		return;
	}
	//
	// make sure two calls do not have the same time
	assert( ! near_equal( point.time, time_line[time_index].time ) );
	//
	// case where this point is inserted at time_index
	time_line.push_back( time_line[n_time-1] );
	for(size_t i = n_time - 1; i > time_index; --i)
		time_line[i] = time_line[i-1];
	time_line[time_index] = point;
	//
	return;
}

// BEGIN_TIME_LINE_PROTOTYPE
template <class Float>
const CppAD::vector<typename time_line_vec<Float>::time_point>&
time_line_vec<Float>::time_line(const size_t& age_index) const
// END_TIME_LINE_PROTOTYPE
{
	assert( sub_lower_ <= age_index );
	assert( age_index <= sub_upper_ );
	return vec_[age_index - sub_lower_];
}

// BEGIN_AGE_TIME_AVG_PROTOTYPE
template <class Float>
Float time_line_vec<Float>::age_time_avg(void) const
// END_AGE_TIME_AVG_PROTOTYPE
{	size_t n_sub = sub_upper_ - sub_lower_ + 1;
	//
	// compute average w.r.t time for each age
	CppAD::vector<Float> time_avg(n_sub);
	for(size_t i = 0; i < n_sub; ++i)
	{	const CppAD::vector<time_point>& line( vec_[i] );
		size_t n_time = line.size();
		//
		assert( n_time >= 1 );
		assert( near_equal( line[0].time, time_lower_ ) );
		assert( near_equal( line[n_time - 1].time, time_upper_ ) );
		//
		if( n_time == 1 )
			time_avg[i] = line[0].value;
		else
		{	Float sum(0);
			for(size_t j = 1; j < n_time; ++j )
			{	Float value = (line[j].value + line[j-1].value) / Float(2);
				sum        += value * (line[j].time - line[j-1].time);
			}
			time_avg[i] = sum / (time_upper_ - time_lower_);
		}
	}
	// compute average w.r.t age
	Float result(0);
	if( n_sub == 1 )
		result = time_avg[0];
	else
	{	for(size_t i = 1; i < n_sub; ++i)
		{	Float  value = (time_avg[i] + time_avg[i-1]) / Float(2);
			size_t k     = i + sub_lower_;
			result      += value * (extend_grid_[k] - extend_grid_[k-1]);
		}
		result = result/(extend_grid_[sub_upper_] - extend_grid_[sub_lower_]);
	}
	return result;
}


// instantiation
template class time_line_vec<double>;
template class time_line_vec<a1_double>;


} // END_DISMOD_AT_NAMESPACE
