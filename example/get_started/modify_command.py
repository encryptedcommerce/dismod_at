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
# $begin modify_command.py$$ $newlinech #$$
# $spell
#	init
#	dismod
# $$
#
# $section init Command: Example and Test$$
#
# $code
# $srcfile%
#	example/get_started/modify_command.py
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
import math
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/modify_command.py'
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
# change into distribution directory to run sandbox version of dismodat.py
# return to test_dir when done.
os.chdir(dist_dir)
print( os.getcwd() )
program          = 'bin/dismodat.py'
file_name        = test_dir + '/get_started.db'
command          = 'modify'
table_name       = 'option'
column_name      = 'option_value'
row_expression   = 'option_name==\'ode_step_size\''
value_expression = '5.0'
cmd            = [
	program,
	file_name,
	command,
	table_name,
	column_name,
	row_expression,
	value_expression
]
#
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at modify command failed')
os.chdir(test_dir)
# -----------------------------------------------------------------------
# option.csv
new        = False
connection = dismod_at.create_connection(file_name, new)
option_table = dismod_at.get_table_dict( connection, 'option')
connection.close()
found = False
for row in option_table :
	if row['option_name'] == 'ode_step_size' :
		found = True
		assert float( row['option_value'] ) == 5.0
assert found
# -----------------------------------------------------------------------
print('modify_command.py: OK')
# END PYTHON
