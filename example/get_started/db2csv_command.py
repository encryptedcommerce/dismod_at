# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin db2csv_command.py$$ $newlinech #$$
# $spell
#	init
#	dismod
# $$
#
# $section init Command: Example and Test$$
#
# $code
# $srcfile%
#	example/get_started/db2csv_command.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import subprocess
import distutils.dir_util
import csv
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/db2csv_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# distribution directory
dist_dir  = os.getcwd()
#
# directory where test files are stored
test_dir  = dist_dir + '/build/example/get_started'
#
# import sandbox version of dismod_at
local_dir = dist_dir + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/get_started directory
distutils.dir_util.mkpath(test_dir)
os.chdir(test_dir)
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
file_name      = 'get_started.db'
for command in [ 'init', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
#
# change into distribution directory to run sandbox version of dismodat.py
# return to test_dir when done.
os.chdir(dist_dir)
program        = 'bin/dismodat.py'
file_name      = test_dir + '/get_started.db'
command        = 'db2csv'
cmd            = [ program, file_name, command ]
#
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at db2csv command failed')
os.chdir(test_dir)
# ---------------------------------------------------------------------------
# get_table
def get_table(name) :
	file_name = name + '.csv'
	file_ptr  = open(file_name, 'r')
	table     = list()
	reader    = csv.DictReader(file_ptr)
	for row in reader :
		table.append(row)
	file_ptr.close()
	return table
# --------------------------------------------------------------------------
# data.csv
data_table = get_table('data')
#
# check data.csv correspond to data_subset table
assert len(data_table) == 1
assert int( data_table[0]['data_id'] ) == 0
# -----------------------------------------------------------------------
print('db2csv_command: OK')
# END PYTHON
