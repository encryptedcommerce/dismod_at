#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# =============================================================================
# $OMhelpKeyCharacter=&
# &begin run_cmake.sh&& &newlinech #&&
# &spell
#	cmake makefile cxx std dismod libdir lmixed cholmod xam cpp
#	usr eigen ipopt cppad bools suitesparse devel hpp
# &&
#
# &section bin/run_cmake.sh: User Configuration Options&&
#
# &head cmake_verbose_makefile&&
# Use '0' for normal and '1' for verbose make output:
# &codep
cmake_verbose_makefile='0'
# &&
#
# &head cmake_build_type&&
# Use either 'DEBUG' or 'RELEASE' for the type of this build:
# &codep
cmake_build_type='DEBUG'
# &&
#
# &head python_three_command&&
# Command used to execute python3 on this machine:
# &codep
python_three_command='python3'
# &&
#
# &head extra_cxx_flags&&
# Extra C++ flags used during compilation:
# &codep
extra_cxx_flags='-std=c++11 -Wall'
# &&
#
# &head dismod_at_prefix&&
# Prefix where dismod_at will be installed:
# &codep
dismod_at_prefix="$HOME/prefix/dismod_at"
# &&
#
# &head Other Prefixes&&
# Prefixes where the required packages were installed:
# &codep
eigen_prefix="$HOME/prefix/dismod_at"
ipopt_prefix="$HOME/prefix/dismod_at"
cppad_prefix="$HOME/prefix/dismod_at"
# &&
#
# &head mixed_cppad_newton&&
# &index MIXED_CPPAD_NEWTON&&
# Use a second order method when optimizing the fixed effects (YES/NO).
# The default is NO because the Newton method requires a lot more memory:
# &codep
mixed_cppad_newton="NO"
# &&
# Note that the include file &code dismod_at/configure.hpp&& contains the
# symbol &code MIXED_CPPAD_NEWTON&& which is zero or one depending on the
# value of &icode mixed_cppad_newton&&.
#
# &head mixed_cppad_set_sparsity&&
# If YES, use sets of indices for sparsity patterns.
# If NO use arrays of bools:
# &codep
mixed_cppad_set_sparsity="NO"
# &&
#
# &head mixed_cppad_libdir&&
# Sub-directory of dismod_at_prefix where mixed_cppad libraries are installed.
# The eigen part of the library is separate so different flags can be used
# to compile the part of the code that uses eigen.
# The following will properly link the &code mixed_cppad&& library:
# &codep
#	-lmixed_cppad -lmixed_cppad_eigen -lmixed_cppad
# &&
# If you do not need to install mixed_cppad, use NOTFOUND for this setting:
# &codep
mixed_cppad_libdir='lib64'
# &&
#
# &head suitesparse_prefix&&
# Prefix where optional package was installed (use NOTFOUND if not installed).
# This is only required by example/devel/mixed_cppad/cholmod_xam.cpp.
# &codep
suitesparse_prefix="$HOME/prefix/suitesparse"
# &&
#
# &head IHME Cluster Settings&&
# Here are some example changes that are used for the IHME cluster
# &codep
# suitesparse_prefix="NOTFOUND"
# python_three_command='/usr/local/anaconda3-current/bin/python'
# extra_cxx_flags='-Wall'
# &&
# &end
# ============================================================================
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
if [ "$1" == '--help' ]
then
	cat << EOF
usage: bin/run_cmake.sh \\
	[--help] \\
	[--verbose] \\
	[--newton] \\
	[--set_sparsity]
EOF
	exit 0
else
	user_option="$1"
fi
if [ "$user_option" == '--verbose' ]
then
	cmake_verbose_makefile='1'
elif [ "$user_option" == '--newton' ]
then
	mixed_cppad_newton="YES"
elif [ "$user_option" == '--set_sparsity' ]
then
	mixed_cppad_set_sparsity="YES"
elif [ "$user_option" != '' ]
then
	echo "'$1' is an invalid option"
	bin/run_cmake.sh --help
	exit 1
fi
# ---------------------------------------------------------------------------
if [ ! -e build ]
then
	echo_eval mkdir build
fi
echo_eval cd build
cmake \
	-Wno-dev \
	-D CMAKE_VERBOSE_MAKEFILE=$cmake_verbose_makefile \
	-D CMAKE_BUILD_TYPE=$cmake_build_type \
	\
	-D python_three_command=$python_three_command \
	-D extra_cxx_flags="$extra_cxx_flags" \
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D cppad_prefix="$cppad_prefix" \
	-D ipopt_prefix="$cppad_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	\
	-D mixed_cppad_newton="$mixed_cppad_newton" \
	-D mixed_cppad_set_sparsity="$mixed_cppad_set_sparsity" \
	-D mixed_cppad_libdir="$mixed_cppad_libdir" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	..
