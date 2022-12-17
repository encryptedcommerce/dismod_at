#! /usr/bin/python3
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin dismodat.py$$ $newlinech #$$
# $spell
#  dismodat.py
# $$
#
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Python Program Syntax$$
#
# $head Syntax$$
# dismodat.py database command ...
#
# $head Log Table$$
#
# $subhead Start$$
# At the start, a $code dismodat.py$$
# command adds a
# $cref/command/log_table/message_type/command/$$
# log entry with
# $cref/message/log_table/message/$$
# equal to
# $codei%
#      begin %command% ...
# %$$
#
# $subhead End$$
# At the end,
# a $code dismodat.py$$ command adds a command log entry with message equal to
# $codei%
#  end %command%
# %$$
# $end
# -----------------------------------------------------------------------------
import sys
import os
import time
#
# import dismod_at
sandbox = 'python/dismod_at'
if os.path.isdir(sandbox) :
   print('using ' + sandbox)
   sys.path.insert(0,  os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
# check usage
usage  = 'dismodat.py database db2csv\n'
usage += 'dismodat.py database pertrub tbl_name sigma\n'
usage += 'dismodat.py database plot_rate_fit pdf_file plot_title rate_set\n'
usage += 'dismodat.py database plot_data_fit pdf_file plot_title max_plot'
#
# deprecated
# usage += 'dismodat.py database csv2db configure_csv measure_csv\n'
# usage += 'dismodat.py database modify\\\n'
# usage += '\ttable_name column_name row_expression value_expression\\\n'
# usage += '\t[statement_one [statement_two ...] ]'
#
if len(sys.argv) < 3 :
   sys.exit(usage)
#
database_file_arg = sys.argv[1]
command_arg       = sys.argv[2]
arg_list          = sys.argv[3:]
# ---------------------------------------------------------------------------
# begin_time
begin_time     = int( time.time() )
# ---------------------------------------------------------------------------
# deprecated
# special case where we execute the command first to create the database
if command_arg == 'csv2db' :
   if len(sys.argv) != 5 :
      sys.exit(usage)
   configure_csv = sys.argv[3]
   measure_csv   = sys.argv[4]
   dismod_at.csv2db_command(database_file_arg, configure_csv, measure_csv)
# ---------------------------------------------------------------------------
# connection
new           = False
connection    = dismod_at.create_connection(database_file_arg, new)
# ---------------------------------------------------------------------------
# create log table if it does not yet exist
cmd  = 'create table if not exists log('
cmd += ' log_id        integer primary key,'
cmd += ' message_type  text               ,'
cmd += ' table_name    text               ,'
cmd += ' row_id        integer            ,'
cmd += ' unix_time     integer            ,'
cmd += ' message       text               )'
dismod_at.sql_command(connection, cmd)
# ---------------------------------------------------------------------------
# write begin for this command
log_table     = dismod_at.get_table_dict(connection, 'log')
log_id        = len( log_table )
#
# message
message       = 'begin'
for i in range(len(sys.argv) - 2) :
   message += ' ' + sys.argv[i+2]
#
cmd  = 'insert into log'
cmd += ' (log_id,message_type,table_name,row_id,unix_time,message) values('
cmd += str(log_id) + ','     # log_id
cmd += '"command",'          # message_type
cmd += 'null,'               # table_name
cmd += 'null,'               # row_id
cmd += str(begin_time) + ',' # unix_time
cmd += '"' + message + '")'  # message
dismod_at.sql_command(connection, cmd)
connection.close()
# ---------------------------------------------------------------------------
# execute command
if command_arg == 'db2csv' :
   if len(sys.argv) != 3 :
      sys.exit(usage)
   dismod_at.db2csv_command(database_file_arg)
elif command_arg == 'perturb' :
   if len(sys.argv) != 5 :
      sys.exit(usage)
   tbl_name = arg_list[0]
   sigma    = arg_list[1]
   dismod_at.perturb_command(
      database_file_arg, tbl_name, sigma
   )
elif command_arg == 'plot_rate_fit' :
   if len(sys.argv) != 6 :
      sys.exit(usage)
   pdf_file   = arg_list[0]
   plot_title = arg_list[1]
   rate_set   = set( arg_list[2].split() )
   dismod_at.plot_rate_fit(
      database_file_arg, pdf_file, plot_title, rate_set
   )
elif command_arg == 'plot_data_fit' :
   if len(sys.argv) != 6 :
      sys.exit(usage)
   pdf_file   = arg_list[0]
   plot_title = arg_list[1]
   max_plot   = int( arg_list[2] )
   dismod_at.plot_data_fit(
      database_file_arg, pdf_file, plot_title, max_plot
   )
elif command_arg == 'csv2db' :
   # deprecated
   pass # already executed this command
elif command_arg == 'modify' :
   # deprecated
   if len(sys.argv) < 7 :
      sys.exit(usage)
   dismod_at.modify_command(database_file_arg, arg_list)
else :
   sys.exit(usage)
# ---------------------------------------------------------------------------
# write end for this command
end_time     = int( time.time() )
message      = 'end ' + sys.argv[2]
cmd  = 'insert into log'
cmd += ' (log_id,message_type,table_name,row_id,unix_time,message) values('
cmd += str(log_id+1) + ','   # log_id
cmd += '"command",'          # message_type
cmd += 'null,'               # table_name
cmd += 'null,'               # row_id
cmd += str(end_time) + ','   # unix_time
cmd += '"' + message + '")'  # message
connection = dismod_at.create_connection(database_file_arg, new)
dismod_at.sql_command(connection, cmd)
connection.close()
# ---------------------------------------------------------------------------
print('dismodat.py: OK')
