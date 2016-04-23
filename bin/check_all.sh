#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_all.sh" ]
then
	echo "bin/check_all.sh: must be executed from its parent directory"
	exit 1
fi
# ---------------------------------------------------------------------------
input=''
while [ "$input" != 'd' ] \
&&    [ "$input" != 'r' ] \
&&    [ "$input" != 'ds' ] \
&&    [ "$input" != 'rs' ]
do
	msg='Debug + Speed [ds], Release + Speed [rs], Debug [d], Release [s] ?'
	read -p "$msg" input
done
bin/check_devel_xam.sh
bin/check_include.sh
bin/check_srcfile.sh
bin/check_configure.sh
#
bin/run_omhelp.sh xml
#
if [ "$input" == 'r' ] || [ "$input" == 'rs' ]
then
	bin/run_cmake.sh --release
else
	bin/run_cmake.sh
fi
#
cd build
make check
if [ "$input" == 'ds' ] || [ "$input" == 'rs' ]
then
	make speed
fi
echo 'check_all.sh: OK'
