#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# cppad_mixed: C++ Laplace Approximation of Mixed Effects Models
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# parameters that can be changed
#
# non-zero so same random see each time.
random_seed='123'
#
# number of random effects is 2 * n_children
n_children='10'
#
# use quasi-Newton (otherwise Newton)
quasi_fixed='false'
#
# smaller increase work per function evaluation
ode_step_size='5.0'
#
# this branch name must not be empty
branch1='master'
#
# If non-empty, compare branches.
# If empty, and there is a new sub-directory, use new subdirectory for changes.
# If empty, and no new sub-directory, only run branch1 case.
branch2=''
#
# Assume that at beginning, installs correspond to master branch.
# If true, re-install release version corresponding to each version and
# at the end ensure installs correspond to master version.
install_cppad='true'
install_cppad_mixed='true'
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/speed.sh' ]
then
	echo 'bin/speed.sh must be run from its parent directory'
	exit 1
fi
current=`git branch | sed -n -e '/^\*/p' | sed -e 's|^\* *||'`
if [ "$current" != 'master' ]
then
	echo 'bin/speed.sh must be run from master branch'
	exit 1
fi
if [ "$branch1" == '' ]
then
	echo 'bin/speed.sh: branch1 cannot be empty'
	exit 1
fi
if [ "$branch2" == 'master' ]
then
	echo 'bin/speed.sh: branch2 cannot be master'
	exit 1
fi
if [ "$branch1" == "$branch2" ]
then
	echo 'bin/speed.sh: branch1 and branch2 must be different'
	echo 'branch2 can be empty in which case new is used to replace soruce'
	exit 1
fi
# -----------------------------------------------------------------------------
if [ "$branch2" != '' ] || [ -d new ]
then
	list='one two'
else
	list='one'
fi
for version in $list
do
	if [ "$version" == 'one' ]
	then
		git reset --hard
		git checkout $branch1
		name="$branch1"
	else
		if [ "$branch2" == '' ]
		then
			echo 'git_new.sh from > /dev/null'
			git_new.sh from > /dev/null
			name='new'
		else
			git checkout $branch2
			name="$branch2"
		fi
	fi
	#
	# build release version
	sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|"
	#
	# run cmake
	echo "bin/run_cmake.sh > build/$name.log"
	bin/run_cmake.sh > build/$name.log
	#
	if [ "$name" != 'master' ]
	then
		#
		# install cppad
		if [ "$install_cppad" == 'true' ]
		then
			echo "bin/install_cppad.sh >> build/$name.log"
			bin/install_cppad.sh >> build/$name.log
		fi
		#
		# install cppad_mixed
		if [ "$install_cppad_mixed" == 'true' ]
		then
			echo "bin/install_cppad_mixed.sh >> build/$name.log"
			bin/install_cppad_mixed.sh >> build/$name.log
		fi
	fi
	#
	# build dismod_at
	cd build
	make clean
	echo "make dismod_at >> build/$name.log"
	make dismod_at >> $name.log
	cd ..
	#
	# create database
	arguments="$random_seed $n_children $quasi_fixed"
	echo "python3 example/user/speed.py $arguments >> build/$name.log"
	python3 example/user/speed.py $arguments >> build/$name.log
	#
	# change into database directory
	pushd build/example/user > /dev/null
	#
	# run timeing test
	program="../../devel/dismod_at"
	echo "time $program example.db fit both 0 >& build/$name.out"
	( time $program example.db fit both 0 ) >& ../../$name.out
	#
	# run memory test
	echo "massif.sh $program example.db fit both 0 >& /dev/null"
	massif.sh $program example.db fit both 0 >& /dev/null
	#
	echo "mv massif.out build/$name.massif"
	mv massif.out ../../$name.massif
	#
	popd > /dev/null
	#
	# undo changes in bin/run_cmake.sh
	git checkout bin/run_cmake.sh
done
# ---------------------------------------------------------------------------
# restore master
# ---------------------------------------------------------------------------
git checkout master
if [ "$name" != 'master' ]
then
	#
	# install cppad
	if [ "$install_cppad" == 'true' ]
	then
		echo "bin/install_cppad.sh >> build/$name.log"
		bin/install_cppad.sh >> build/$name.log
	fi
	#
	# install cppad_mixed
	if [ "$install_cppad_mixed" == 'true' ]
	then
		echo "bin/install_cppad_mixed.sh >> build/$name.log"
		bin/install_cppad_mixed.sh >> build/$name.log
	fi
fi
