# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin argument_table.py$$ $newlinech #$$
#
# $section argument_table: Example and Test$$
#
# $code
# $verbatim%example/table/argument_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
def argument_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the argument table
	col_name = [ 'argument_name', 'argument_value'  ]
	col_type = [ 'text unique'  , 'text' ]
	row_list = [
		['parent_node_id', '0'           ] ,
		['ode_step_size',  '20.0'        ] ,
		['tolerance',      '1e-8'        ] ,
		['max_num_iter',   '100'         ] ,
		['print_level',    '0'           ] ,
		['rate_info',      'chi_positive']
	]
	tbl_name = 'argument'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
	assert row_list == check_list
	# ------------------------------------------------------------------------
	connection.close()
	print('argument_table: OK')
# END PYTHON
