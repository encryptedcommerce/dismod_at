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
# $begin user_jump_at_age.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Case with a Jump in Rate at an Age$$
#
# $code
# $srcfile%
#	example/user/jump_at_age.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# true values used to simulate data
iota_20        = 1e-4
iota_100       = 1e-1
iota_age_list  = [ 0.0, 20.0, 21.0, 100.0 ]
data_age_list  = [ 0.0, 1.0, 5.0, 10.0, 20.0, 40.0, 80.0, 100.0 ]
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/jump_at_age.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
# Note that the a, t values are used for this example
def constant_weight_fun(a, t) :
	return 1.0
#
def fun_iota_parent(a, t) :
	if 19.5 <= a and a <= 20.5 :
		return ('prior_rate_parent', 'prior_none', 'prior_difference')
	else :
		return ('prior_rate_parent', 'prior_difference', 'prior_difference')
#
def iota_true(age) :
	if age <= 20.0 :
		return iota_20
	else :
		return iota_100
#
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table (in age_list above)
	age_list = sorted( set( iota_age_list  ) )
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_list = [
		'Sincidence',
		'mtother'
	]
	#
	# node table: world
	node_table = [ { 'name':'world',         'parent':'' } ]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# avgint table: empty
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	#
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'log_gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   time_list[0],
		'time_upper':   time_list[-1]
	}
	# Sincidence data (exclude data at 100)
	for age in data_age_list :
		#
		meas_value = iota_true(age)
		row['age_lower']    = age
		row['age_upper']    = age
		row['integrand']    = 'Sincidence'
		row['meas_value']   = meas_value
		row['meas_std']     = meas_value * 0.1
		row['eta']          = 1e-6;
		data_table.append( copy.copy(row) )
		#
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_none
			'name':     'prior_none',
			'density':  'uniform',
			'mean':     0.0,
		},{ # prior_difference
			'name':     'prior_difference',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      1.0,
			'eta':      1e-4
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-5,
			'upper':    1.0,
			'mean':     0.1,
		},{ # prior_iota_20
			'name':     'prior_iota_20',
			'density':  'uniform',
			'lower':    iota_20,
			'upper':    iota_20,
			'mean':     iota_20,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	#
	iota_age_id = list()
	for age in iota_age_list :
		iota_age_id.append( age_list.index(age) )
	#
	smooth_table = [
		{ # smooth_iota_parent
			'name':                     'smooth_iota_parent',
			'age_id':                   iota_age_id,
			'time_id':                  range(len(time_list)),
			'fun':                       fun_iota_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'ode_step_size',          'value':'1.0'               },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'200'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-6'              },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-10'             }
	]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_list,
		node_table,
		weight_table,
		covariate_table,
		avgint_table,
		data_table,
		prior_table,
		smooth_table,
		nslist_table,
		rate_table,
		mulcov_table,
		option_table
	)
	# ----------------------------------------------------------------------
	n_smooth  = len( smooth_table )
	return
# ===========================================================================
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# Results for fitting with no noise
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
parent_node_id = 1
eps            = 1e-4
#
# check rates values
count             = 0
iota_rate_id      = 1
max_err           = 0.0
tolerance         = 1e-3
age_list          = sorted( set( iota_age_list ) )
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	assert row['var_type'] == 'rate'
	assert row['node_id']  == 0
	age    = age_list[ row['age_id'] ]
	rate_id = row['rate_id']
	value  = fit_var_table[var_id]['fit_var_value']
	assert rate_id == iota_rate_id
	value_true = iota_true(age)
	rate       = 'iota'
	max_err = max(max_err, abs( value / value_true - 1.0 ) )
	if( abs(value / value_true - 1.0) > tolerance ) :
		print(rate, age, value / value_true - 1.0 )
assert max_err <= tolerance
# -----------------------------------------------------------------------------
print('jump_at_age.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON