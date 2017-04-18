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
# $begin user_asymptotic.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section Sample from Asymptotic Distribution for Model Variables$$
#
# $head Purpose$$
# The command
# $codei%
#	dismod_at %database% sample asymptotic
# %$$
# samples form an asymptotic approximation for the posterior distribution
# of the $cref model_variables$$.
#
# $head Notation$$
# $table
# $icode iota_n$$ $cnext model incidence for $code north_america$$ $rnext
# $icode u_m$$ $cnext incidence random effect for $code mexico$$ $rnext
# $icode u_c$$ $cnext incidence random effect for $code canada$$ $rnext
# $icode y_n$$ $cnext measured incidence for $code north_america$$ $rnext
# $icode y_m$$ $cnext measured incidence for $code mexico$$ $rnext
# $icode y_c$$ $cnext measured incidence for $code canada$$ $rnext
# $icode s_n$$ $cnext standard deviation for $icode y_n$$   $rnext
# $icode s_m$$ $cnext standard deviation for $icode y_m$$   $rnext
# $icode s_c$$ $cnext standard deviation for $icode y_c$$   $rnext
# $icode s_r$$ $cnext standard deviation for random effects $rnext
# $tend
#
# $head Likelihood$$
# We define $latex h(y, \mu , \sigma)$$
# to be the log-density for a $latex \B{N}(0, 1)$$ distribution; i.e.,
# $latex \[
#	h(y, \mu, \sigma) =
#		- \frac{ ( y - \mu )^2 }{ \sigma^2 }
#		- \log \left( \sigma \sqrt{ 2 \pi } \right)
# \] $$
# The total log-likelihood for this example is
# $latex \[
#	h[ y_n, \iota_n, s_n ] +
#	h[ y_m, \exp( u_m ) \iota_m, s_m ] +
#	h[ y_c, \exp( u_c ) \iota_c, s_c ] +
#	h( u_m, 0, s_r ) + h( u_c , 0 , s_r ) +
# \] $$
#
# $code
# $srcfile%
#	example/user/asymptotic.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
measure = {
	'north_america' : 1.0e-2 ,
	'mexico'        : 2.0e-2 ,
	'canada'        : 0.5e-2
}
standard = {
	'north_america' : 1.0e-3 ,
	'mexico'        : 2.0e-3 ,
	'canada'        : 0.5e-3
}
standard_random_effect = 1.0
number_sample          = 500
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import math
import numpy
import sys
import os
import copy
import distutils.dir_util
import subprocess
test_program = 'example/user/asymptotic.py'
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
# ---------------------------------------------------------------------------
# no need to include sqrt{2 \pi} term (it does not depend on model variables)
def h(y, mu, sigma ) :
	if sigma <= 0.0 :
		return - float("inf")
	res = (y - mu ) / sigma
	return - res * res - math.log( sigma )
#
def log_f(x) :
	iota_n = x[0]
	u_m    = x[1]
	u_c    = x[2]
	#
	y_n    = measure['north_america']
	y_m    = measure['mexico']
	y_c    = measure['canada']
	#
	s_n    = standard['north_america']
	s_m    = standard['mexico']
	s_c    = standard['canada']
	s_r    = standard_random_effect
	#
	ret    = h(y_n, iota_n, s_n)
	ret   += h(y_m, math.exp(u_m) * iota_n, s_m )
	ret   += h(y_c, math.exp(u_c) * iota_n, s_c )
	ret   += h(u_m, 0.0, s_r ) + h(u_c, 0.0, s_r)
	return ret
# ---------------------------------------------------------------------------
# note that the a, t values are not used for this example
# note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_zero(a, t) :
	return ('prior_zero', 'prior_zero', 'prior_zero')
def fun_one(a, t) :
	return ('prior_one', 'prior_one', 'prior_one')
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0,   100.0 ]
	#
	# time table
	time_list   = [ 1995.0,  2015.0 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence',  'eta':1e-6 }
	]
	#
	# node table: world -> north_america
	#             north_america -> (mexico, canada)
	node_table = [
		{ 'name':'world',         'parent':''              },
		{ 'name':'north_america', 'parent':'world'         },
		{ 'name':'mexico',        'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
	]
	#
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	# --------------------------------------------------------------------------
	# data table:
	data_table = list()
	row = {
		'node':        'north_america',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0,
		'age_upper':    100.0,
		'integrand':   'Sincidence',
		'meas_value':   measure['north_america'],
		'meas_std':     standard['north_america'],
		'eta':          None
	}
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'mexico';
	row['meas_value'] = measure['mexico']
	row['meas_std']   = standard['mexico']
	row['eta']        = None;
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'canada';
	row['meas_value'] = measure['canada']
	row['meas_std']   = standard['canada']
	row['eta']        = None;
	data_table.append( copy.copy(row) )
	#
	for data_id in range( len( data_table ) ) :
		data_table[data_id]['data_name'] = 'd' + str(data_id)
	# --------------------------------------------------------------------------
	# prior_table
	prior_table = [
		{   # prior_zero
			'name':     'prior_zero',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-2 * measure['north_america'],
			'upper':    1e+2 * measure['north_america'],
			'mean':     measure['north_america'],
			'std':      None,
			'eta':      None
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      standard_random_effect,
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_parent
		}, { # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
			'child_nslist':  None
		},{
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'chi',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'omega',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ------------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america'      },
		{ 'name':'ode_step_size',          'value':'10.0'               },
		{ 'name':'random_seed',            'value':'0'                  },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'  },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-12'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-12'        }
	]
	# --------------------------------------------------------------------------
	# avgint table:
	avgint_table = list()
	# --------------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
	# -----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_table,
		node_table,
		weight_table,
		covariate_table,
		data_table,
		prior_table,
		smooth_table,
		nslist_table,
		rate_table,
		mulcov_table,
		option_table,
		avgint_table
	)
# ===========================================================================
file_name             = 'example.db'
example_db(file_name)
program               = '../../devel/dismod_at'
for command in [ 'init', 'start', 'fit', 'sample' ] :
	cmd = [ program, file_name, command ]
	if command == 'start' :
		cmd.append('prior_mean')
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	if command == 'sample' :
		cmd.append('asymptotic')
		cmd.append( str(number_sample) )
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table     = dismod_at.get_table_dict(connection, 'var')
node_table  = dismod_at.get_table_dict(connection, 'node')
rate_table  = dismod_at.get_table_dict(connection, 'rate')
sample_dict  = dismod_at.get_table_dict(connection, 'sample')
# -----------------------------------------------------------------------
# map from node name to variable id
node_name2var_id = dict()
for var_id in range(len(var_table) ) :
	assert var_id < 3
	row = var_table[var_id]
	assert row['var_type'] == 'rate'
	assert rate_table[row['rate_id']]['rate_name']  == 'iota'
	node_name = node_table[row['node_id']]['node_name']
	node_name2var_id[node_name] = var_id
#
# convert samples to a numpy array
sample_array = numpy.zeros( (number_sample, 3), dtype = float )
for row in sample_dict :
	var_id                              = row['var_id']
	sample_index                        = row['sample_index']
	sample_array[sample_index, var_id ] = row['var_value']
#
# compute statistics
var_avg = numpy.average(sample_array, axis=0);
var_std = numpy.std(sample_array, axis=0);
# -----------------------------------------------------------------------
# now use MCMC to calculate the same values
m          = 10 * number_sample
x0         = numpy.array( [ 1e-2, 0.0, 0.0 ] )
s          = numpy.array( [ 1e-3, 1e-1, 1e-1] )
(a, c)     = dismod_at.metropolis(log_f, m, x0, s)
burn_in    = int( 0.1 * m )
c          = c[burn_in :, :]
x_avg_mcmc = numpy.average(c, axis=0)
x_std_mcmc = numpy.std(c, axis=0)
mcmc_order = [ 'north_america', 'mexico', 'canada' ]
# -----------------------------------------------------------------------
# now check values
for i in range(3) :
	node_name = mcmc_order[i]
	value     = var_avg[ node_name2var_id[node_name] ]
	check     = x_avg_mcmc[i]
	avg_diff  = check / value - 1.0
	assert abs(avg_diff) < 0.05
	value     = var_std[ node_name2var_id[node_name] ]
	check     = x_std_mcmc[i]
	std_diff  = check / value - 1.0
	# This is a small sample case (only three data points)
	# so we do not expect the asymptotic statistics to be correct.
	# Note that in this case, the asymptotics are an over estimate.
	assert std_diff < 0.0 and abs(std_diff) < 0.5

print('asymptotic.py: OK')
# END PYTHON
