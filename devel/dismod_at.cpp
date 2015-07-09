// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <map>
# include <cassert>
# include <string>
# include <iostream>
# include <cppad/vector.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/put_table_row.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/get_column_max.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/manage_gsl_rng.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/sim_random.hpp>
# include <dismod_at/get_simulate_table.hpp>
# include <dismod_at/avg_case_subset.hpp>

namespace { // BEGIN_EMPTY_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin init_command$$
$spell
	init
	var
	dismod
$$

$section The Variable Command$$

$head Syntax$$
$codei%dismod_at init %file_name%$$

$head Purpose$$
This command should be executed whenever any of the
$cref input$$ tables change.

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head var_table$$
A new $cref var_table$$ is created with the information
that maps a $cref/var_id/var_table/var_id/$$
to its meaning in terms of the
$cref/model variables/model_variable/$$.

$head data_subset_table$$
A new $cref data_subset_table$$ is created.
This makes explicit exactly which rows of the data table are used.

$head avg_case_subset_table$$
A new $cref avg_case_subset_table$$ is created.
This makes explicit exactly which rows of the avg_case table are used.

$head Deleted Tables$$
If any of the following tables exist, they are deleted:
$list number$$
$cref/fit_var_table/fit_command/fit_var_table/$$
$lnext
$cref/truth_var_table/truth_command/truth_var_table/$$
$lnext
$cref/simulate_table/simulate_command/simulate_table/$$
$lnext
$cref/sample_table/sample_command/sample_table/$$
$lend

$children%example/get_started/init_command.py%$$
$head Example$$
The file $cref init_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void init_command(
sqlite3*                                                db                  ,
const CppAD::vector<dismod_at::data_subset_struct>&     data_subset_obj     ,
const CppAD::vector<dismod_at::avg_case_subset_struct>& avg_case_subset_obj ,
const dismod_at::pack_info&                             pack_object         ,
const dismod_at::db_input_struct&                       db_input            ,
const size_t&                                           parent_node_id      ,
const dismod_at::child_info&                            child_object
)
{	using CppAD::vector;
	using std::string;
	using dismod_at::to_string;

	// -----------------------------------------------------------------------
	const char* drop_list[] = {
		"subset", "var", "fit_var", "truth_var", "simulate", "sample"
	};
	size_t n_drop = sizeof( drop_list ) / sizeof( drop_list[0] );
	string sql_cmd;
	for(size_t i = 0; i < n_drop; i++)
	{	sql_cmd = "drop table if exists ";
		sql_cmd += drop_list[i];
		dismod_at::exec_sql_cmd(db, sql_cmd);
	}
	// -----------------------------------------------------------------------
	// create data_subset_table
	sql_cmd = "create table data_subset("
		" data_subset_id  integer primary key,"
		" data_id         integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	string table_name = "data_subset";
	size_t n_subset   = data_subset_obj.size();
	CppAD::vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0] = "data_id";
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int data_id    = data_subset_obj[subset_id].data_id;
		row_val_vec[0] = dismod_at::to_string( data_id );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// -----------------------------------------------------------------------
	// create avg_case_subset_table
	sql_cmd = "create table avg_case_subset("
		" avg_case_subset_id  integer primary key,"
		" avg_case_id         integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "avg_case_subset";
	n_subset   = avg_case_subset_obj.size();
	col_name_vec[0] = "avg_case_id";
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int avg_case_id    = avg_case_subset_obj[subset_id].avg_case_id;
		row_val_vec[0] = dismod_at::to_string( avg_case_id );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// -----------------------------------------------------------------------
	sql_cmd = "create table var("
		" var_id         integer primary key,"
		" var_type       text,"
		" smooth_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" node_id        integer,"
		" rate_id        integer,"
		" integrand_id   integer,"
		" covariate_id   integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	table_name = "var";
	//
	col_name_vec.resize(8);
	row_val_vec.resize(8);
	col_name_vec[0]   = "var_type";
	col_name_vec[1]   = "smooth_id";
	col_name_vec[2]   = "age_id";
	col_name_vec[3]   = "time_id";
	col_name_vec[4]   = "node_id";
	col_name_vec[5]   = "rate_id";
	col_name_vec[6]   = "integrand_id";
	col_name_vec[7]   = "covariate_id";
	//
	// mulstd variables
	size_t n_smooth = db_input.smooth_table.size();
	size_t offset, var_id;
	for(size_t i = 2; i < row_val_vec.size(); i++)
		row_val_vec[i] = "null"; // these columns are null for mulstd variables
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	offset      = pack_object.mulstd_offset(smooth_id);
		for(size_t i = 0; i < 3; i++)
		{	var_id                   = offset + i;
			// var_type
			if( i == 0 )
				row_val_vec[0] = "mulstd_value";
			else if( i == 1 )
				row_val_vec[0] = "mulstd_dage";
			else
				row_val_vec[0] = "mulstd_dtime";
			//
			// smooth_id
			row_val_vec[1] = to_string( smooth_id );
			//
			dismod_at::put_table_row(
				db,
				table_name,
				col_name_vec,
				row_val_vec,
				var_id
			);
		}
	}
	//
	// rate variables
	size_t n_rate  = db_input.rate_table.size();
	size_t n_child = child_object.child_size();
	size_t smooth_id, n_var, n_age, n_time, node_id;
	dismod_at::pack_info::subvec_info info;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	info      = pack_object.rate_info(rate_id, child_id);
			offset    = info.offset;
			smooth_id = info.smooth_id;
			n_var     = info.n_var;
			n_age     = db_input.smooth_table[smooth_id].n_age;
			n_time    = db_input.smooth_table[smooth_id].n_time;
			if( child_id == n_child )
				node_id = parent_node_id;
			else
				node_id = child_object.child_id2node_id(child_id);
			assert( n_var == n_age * n_time );
			for(size_t index = 0; index < n_var; index++)
			{	size_t age_id   = index % n_age;
				size_t time_id  = index / n_age;
				var_id          = offset + index;
				//
				// variable_value
				row_val_vec[0]  = "rate";     // var_type
				row_val_vec[1]  = "null";     // smooth_id
				row_val_vec[2]  = to_string( age_id );
				row_val_vec[3]  = to_string( time_id );
				row_val_vec[4]  = to_string( node_id );
				row_val_vec[5]  = to_string( rate_id );
				row_val_vec[6]  = "null";     // integrand_id
				row_val_vec[7]  = "null";     // covariate_id
				dismod_at::put_table_row(
					db,
					table_name,
					col_name_vec,
					row_val_vec,
					var_id
				);
			}
		}
	}
	//
	// covariate multiplers
	const CppAD::vector<dismod_at::mulcov_struct>&
		mulcov_table( db_input.mulcov_table );
	size_t n_mulcov        = mulcov_table.size();
	size_t count_rate_mean  = 0;
	size_t count_meas_value = 0;
	size_t count_meas_std   = 0;
	for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
	{	dismod_at::mulcov_type_enum mulcov_type;
		mulcov_type     = mulcov_table[mulcov_id].mulcov_type;
		size_t rate_id  = mulcov_table[mulcov_id].rate_id;
		size_t integrand_id = mulcov_table[mulcov_id].integrand_id;
		size_t covariate_id = mulcov_table[mulcov_id].covariate_id;
		size_t smooth_id    = mulcov_table[mulcov_id].smooth_id;
		//
		if( mulcov_type == dismod_at::rate_mean_enum ) info =
		pack_object.mulcov_rate_mean_info(rate_id, count_rate_mean++);
		//
		else if( mulcov_type == dismod_at::meas_value_enum ) info =
		pack_object.mulcov_meas_value_info(integrand_id, count_meas_value++);
		//
		else if( mulcov_type == dismod_at::meas_std_enum ) info =
		pack_object.mulcov_meas_std_info(integrand_id, count_meas_std++);
		//
		else assert(false);
		//
		offset    = info.offset;
		assert( smooth_id == info.smooth_id);
		n_var     = info.n_var;
		n_age     = db_input.smooth_table[smooth_id].n_age;
		n_time    = db_input.smooth_table[smooth_id].n_time;
		assert( n_var == n_age * n_time );
		for(size_t index = 0; index < n_var; index++)
		{	size_t age_id   = index % n_age;
			size_t time_id  = index / n_age;
			var_id          = offset + index;
			//
		// var_type
			if( mulcov_type == dismod_at::rate_mean_enum )
				row_val_vec[0]  = "mulcov_rate_mean";
			else if( mulcov_type == dismod_at::meas_value_enum )
				row_val_vec[0]  = "mulcov_meas_value";
			else if( mulcov_type == dismod_at::meas_std_enum )
				row_val_vec[0]  = "mulcov_meas_std";
			else assert(false);
			//
			row_val_vec[1]  = "null";     // smooth_id
			row_val_vec[2]  = to_string( age_id );
			row_val_vec[3]  = to_string( time_id );
			row_val_vec[4]  = "null";     // node_id
			row_val_vec[5]  = "null";     // rate_id
			row_val_vec[6]  = to_string( integrand_id );
			row_val_vec[7]  = to_string( covariate_id );
			dismod_at::put_table_row(
				db,
				table_name,
				col_name_vec,
				row_val_vec,
				var_id
			);
		}
	}
	return;
}

/*
-----------------------------------------------------------------------------
$begin fit_command$$
$spell
	var
	arg
	num_iter
	dismod
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at fit %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head fit_var_table$$
A new $cref fit_var_table$$ is created each time this command is run.
It contains the results of the fit in its
$cref/fit_var_value/fit_var_table/fit_var_value/$$ column.

$children%example/get_started/fit_command.py%$$
$head Example$$
The file $cref fit_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void fit_command(
	sqlite3*                                     db               ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::db_input_struct&            db_input         ,
	const CppAD::vector<dismod_at::smooth_info>& s_info_vec       ,
	const dismod_at::data_model&                 data_object      ,
	const dismod_at::prior_model&                prior_object     ,
	const std::string&                           tolerance_arg    ,
	const std::string&                           max_num_iter_arg
)
{	using CppAD::vector;
	using std::string;
	using dismod_at::to_string;

	// ------------------ run fit_model ------------------------------------
	dismod_at::fit_model fit_object(
		pack_object          ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object
	);
	fit_object.run_fit(tolerance_arg, max_num_iter_arg);
	vector<double> solution = fit_object.get_solution();
	// -------------------- fit_var table --------------------------------------
	string sql_cmd = "drop table if exists fit";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table fit_var("
		" fit_var_id   integer primary key,"
		" fit_var_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	string table_name = "fit_var";
	//
	CppAD::vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "fit_var_value";
	for(size_t fit_var_id = 0; fit_var_id < solution.size(); fit_var_id++)
	{	double fit_var_value   = solution[fit_var_id];
		row_val_vec[0] = to_string( fit_var_value );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}
/*
-----------------------------------------------------------------------------
$begin truth_command$$
$spell
	var
	dismod
$$

$section The Truth Command$$

$head Syntax$$
$codei%dismod_at var %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head fit_var_table$$
In addition to the standard $cref input$$ tables,
there must be a $cref fit_var_table$$.

$head truth_var_table$$
A new $cref truth_var_table$$ is created with the information in the fit_var table;
to be specific,
$codei%
	%truth_var_id% = %fit_var_id% = %var_id%
	%truth_var_value% = %fit_var_value%
%$$

$children%example/get_started/truth_command.py%$$
$head Example$$
The file $cref truth_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void truth_command(sqlite3* db)
{	using CppAD::vector;
	using std::string;
	//
	// get fit_var table information
	CppAD::vector<double> fit_var_value;
	string table_name  = "fit_var";
	string column_name = "fit_var_value";
	dismod_at::get_table_column(db, table_name, column_name, fit_var_value);
	//
	// create fit_var table
	string sql_cmd = "drop table if exists truth";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table truth_var("
		" truth_var_id   integer primary key,"
		" truth_var_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "truth_var";
	CppAD::vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "truth_var_value";
	for(size_t fit_var_id = 0; fit_var_id < fit_var_value.size(); fit_var_id++)
	{	string truth_var_value = dismod_at::to_string( fit_var_value[fit_var_id] );
		row_val_vec[0]     = truth_var_value;
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}
/*
-----------------------------------------------------------------------------
$begin simulate_command$$

$section The Simulate Command$$
$spell
	var
	dismod
	arg
	std
	covariates
$$

$head Syntax$$
$codei%dismod_at simulate %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head truth_var_table$$
The $cref truth_var_table$$ is an addition input table for this command.
It specifies the true values for the
$cref/model_variables/model_variable/$$ used during the simulation.
This table can be create by the $cref truth_command$$,
or the user can create it directly with the aid of the
$cref var_table$$ (created by the $cref init_command$$).

$head simulate_table$$
A new $cref simulate_table$$ is created.
It contains simulated measurement values that can be used in place of
the data table $cref/meas_value/data_table/meas_value/$$ column.
Only the $cref/data_id/data_subset_table/data_id/$$ that are in the
data_subset table are included.

$children%example/get_started/simulate_command.py%$$
$head Example$$
The file $cref simulate_command.py$$ contains an example and test
of using this command.

$end
*/
void simulate_command
(	sqlite3*                                            db              ,
	const CppAD::vector<dismod_at::integrand_struct>&   integrand_table ,
	const CppAD::vector<dismod_at::data_subset_struct>& data_subset_obj ,
	const dismod_at::data_model&                        data_object     ,
	const size_t&                                       actual_seed     ,
	const size_t&                                       number_sample
)
{	using std::cerr;
	using std::endl;
	using std::string;
	using CppAD::vector;
	using dismod_at::to_string;
	// -----------------------------------------------------------------------
	// read truth_var table into pack_vec
	vector<double> pack_vec;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	dismod_at::get_table_column(db, table_name, column_name, pack_vec);
	// ----------------- simulate_table ----------------------------------
	table_name = "simulate";
	//
	string sql_cmd = "drop table if exists simulate";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	sql_cmd = "create table simulate("
		" simulate_id     integer primary key,"
		" sample_index    integer,"
		" data_subset_id  integer,"
		" meas_value      real"
	");";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	vector<string> col_name_vec(3), row_val_vec(3);
	col_name_vec[0]   = "sample_index";
	col_name_vec[1]   = "data_subset_id";
	col_name_vec[2]   = "meas_value";
	//
	size_t n_subset = data_subset_obj.size();
	for(size_t sample_index = 0; sample_index < number_sample; sample_index++)
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	size_t integrand_id =  data_subset_obj[subset_id].integrand_id;
		dismod_at::integrand_enum integrand =
			integrand_table[integrand_id].integrand;
		double avg;
		switch( integrand )
		{	case dismod_at::Sincidence_enum:
			case dismod_at::remission_enum:
			case dismod_at::mtexcess_enum:
			case dismod_at::mtother_enum:
			case dismod_at::mtwith_enum:
			case dismod_at::relrisk_enum:
			avg = data_object.avg_no_ode(subset_id, pack_vec);
			break;

			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, pack_vec);
			break;

			default:
			assert(false);
		}
		// need to simulate random noise with proper density
		dismod_at::density_enum density = dismod_at::density_enum(
			data_subset_obj[subset_id].density_id
		);
		double meas_std     = data_subset_obj[subset_id].meas_std;
		double eta          = integrand_table[integrand_id].eta;
		double meas_value   = dismod_at::sim_random(
			density, avg, meas_std, eta
		);
		row_val_vec[0] = to_string( sample_index );
		row_val_vec[1] = to_string( subset_id );
		row_val_vec[2] = to_string(meas_value);
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}

	return;
}
/*
-------------------------------------------------------------------------------
$begin sample_command$$
$spell
	dismod
	var
	arg
$$

$section The Sample Command$$

$head Syntax$$
$codei%dismod_at sample %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head simulate_table$$
This command has the extra input $cref  simulate_table$$
which was created by a previous $cref simulate_command$$.

$head sample_table$$
A new $cref sample_table$$ is created each time this command is run.
It contains the optimal $cref model_variable$$ values
for each simulated $cref/sample_index/simulate_table/sample_index/$$.

$children%example/get_started/sample_command.py%$$
$head Example$$
The file $cref sample_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void sample_command(
	sqlite3*                                             db               ,
	dismod_at::data_model&                               data_object      ,
	const CppAD::vector<dismod_at::data_subset_struct>&  data_subset_obj  ,
	const dismod_at::pack_info&                          pack_object      ,
	const dismod_at::db_input_struct&                    db_input         ,
	const CppAD::vector<dismod_at::simulate_struct>&     simulate_table   ,
	const CppAD::vector<dismod_at::smooth_info>&         s_info_vec       ,
	const dismod_at::prior_model&                        prior_object     ,
	const std::string&                                   tolerance_arg    ,
	const std::string&                                   max_num_iter_arg
)
{	using CppAD::vector;
	using std::string;
	using dismod_at::to_string;

	// create a new sample table
	string sql_cmd = "drop table if exists sample";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table sample("
		" sample_id        integer primary key,"
		" sample_index     integer,"
		" var_id           integer,"
		" var_value        real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	string table_name = "sample";
	CppAD::vector<string> col_name_vec(3), row_val_vec(3);
	col_name_vec[0]   = "sample_index";
	col_name_vec[1]   = "var_id";
	col_name_vec[2]   = "var_value";

	// n_subset, n_sample
	size_t n_var    = pack_object.size();
	size_t n_subset = data_subset_obj.size();
	size_t n_sample = simulate_table.size() / n_subset;
	assert( simulate_table.size() == n_sample * n_subset );
	for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
	{	// set the measurement values for this simulation subset
		data_object.change_meas_value(sample_index, simulate_table);

		// fit_model
		dismod_at::fit_model fit_object(
			pack_object          ,
			db_input.prior_table ,
			s_info_vec           ,
			data_object          ,
			prior_object
		);
		fit_object.run_fit(tolerance_arg, max_num_iter_arg);
		vector<double> solution = fit_object.get_solution();
		assert( solution.size() == n_var );
		//
		// write out solution for this sample_index
		row_val_vec[0] = to_string( sample_index );
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	row_val_vec[1] = to_string( var_id );
			row_val_vec[2] = to_string( solution[var_id] );
			dismod_at::put_table_row(
				db, table_name, col_name_vec, row_val_vec
			);
		}
	}
	return;
}
} // END_EMPTY_NAMESPACE

int main(int n_arg, const char** argv)
{	// ---------------- using statements ----------------------------------
	using std::cerr;
	using std::endl;
	using CppAD::vector;
	using std::string;
	// ---------------- command line arguments ---------------------------
	string program = "dismod_at-";
	program       += DISMOD_AT_VERSION;
	if( n_arg != 3 )
	{	cerr << program << endl;
		cerr << "usage: dismod_at command file_name" << endl;
		std::exit(1);
	}
	size_t i_arg = 0;
	const string command_arg    = argv[++i_arg];
	const string file_name_arg  = argv[++i_arg];
	bool ok = false;
	ok     |= command_arg == "init";
	ok     |= command_arg == "fit";
	ok     |= command_arg == "truth";
	ok     |= command_arg == "simulate";
	ok     |= command_arg == "sample";
	if( ! ok )
	{	cerr << "dismod_at: command is not one of the following:" << endl
		<< "\tinit, fit, truth, simulate, sample" << endl;
		std::exit(1);
	}
	// --------------- get the input tables ---------------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	// ----------------------------------------------------------------------
	// argument_map
	std::map<string, string> argument_map;
	size_t n_argument = db_input.argument_table.size();
	for(size_t id = 0; id < n_argument; id++)
	{	string name  = db_input.argument_table[id].argument_name;
		string value = db_input.argument_table[id].argument_value;
		argument_map[name] = value;
	}
	// ---------------------------------------------------------------------
	// ode_step_size
	double ode_step_size  = std::atof( argument_map["ode_step_size"].c_str() );
	assert( ode_step_size > 0.0 );
	// ---------------------------------------------------------------------
	// initialize random number generator
	// 2DO: need to put actual seed in some output table or std::cout
	size_t random_seed = std::atoi( argument_map["random_seed"].c_str() );
	size_t actual_seed = dismod_at::new_gsl_rng(random_seed);
	// ---------------------------------------------------------------------
	// n_age_ode
	double age_min    = db_input.age_table[0];
	double age_max    = db_input.age_table[ db_input.age_table.size() - 1 ];
	size_t n_age_ode  = size_t( (age_max - age_min) / ode_step_size + 1.0 );
	assert( age_max  <= age_min  + n_age_ode * ode_step_size );
	// ---------------------------------------------------------------------
	// n_time_ode
	double time_min   = db_input.time_table[0];
	double time_max   = db_input.time_table[ db_input.time_table.size() - 1 ];
	size_t n_time_ode = size_t( (time_max - time_min) / ode_step_size + 1.0 );
	assert( time_max <= time_min  + n_time_ode * ode_step_size );
	// ---------------------------------------------------------------------
	// child_object and some more size_t values
	size_t parent_node_id = std::atoi(
		argument_map["parent_node_id"].c_str()
	);
	dismod_at::child_info child_object(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.data_table     ,
		db_input.avg_case_table
	);
	size_t n_child     = child_object.child_size();
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_weight    = db_input.weight_table.size();
	size_t n_smooth    = db_input.smooth_table.size();
	// ---------------------------------------------------------------------
	// data_subset_obj
	vector<dismod_at::data_subset_struct> data_subset_obj = data_subset(
		db_input.data_table,
		db_input.covariate_table,
		child_object
	);
	// ---------------------------------------------------------------------
	// avg_case_subset_obj
	vector<dismod_at::avg_case_subset_struct> avg_case_subset_obj =
		avg_case_subset(
			db_input.avg_case_table,
			db_input.covariate_table,
			child_object
	);
	// w_info_vec
	vector<dismod_at::weight_info> w_info_vec(n_weight);
	for(size_t weight_id = 0; weight_id < n_weight; weight_id++)
	{	w_info_vec[weight_id] = dismod_at::weight_info(
			db_input.age_table,
			db_input.time_table,
			weight_id,
			db_input.weight_table,
			db_input.weight_grid_table
		);
	}
	// s_info_vec
	vector<dismod_at::smooth_info> s_info_vec(n_smooth);
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	s_info_vec[smooth_id] = dismod_at::smooth_info(
			db_input.age_table         ,
			db_input.time_table        ,
			smooth_id                  ,
			db_input.smooth_table      ,
			db_input.smooth_grid_table
		);
	}
	// pack_object
	dismod_at::pack_info pack_object(
		n_integrand           ,
		n_child               ,
		db_input.smooth_table ,
		db_input.mulcov_table ,
		db_input.rate_table
	);
	// prior_object
	dismod_at::prior_model prior_object(
		pack_object           ,
		db_input.age_table    ,
		db_input.time_table   ,
		db_input.prior_table  ,
		s_info_vec
	);
	// data_object
	dismod_at::data_model data_object(
		parent_node_id           ,
		n_age_ode                ,
		n_time_ode               ,
		ode_step_size            ,
		db_input.age_table       ,
		db_input.time_table      ,
		db_input.integrand_table ,
		db_input.node_table      ,
		data_subset_obj          ,
		w_info_vec               ,
		s_info_vec               ,
		pack_object              ,
		child_object
	);
	string rate_info = argument_map["rate_info"];
	data_object.set_eigen_ode2_case_number(rate_info);
	// ---------------------------------------------------------------------
	if( command_arg == "init" )
	{	init_command(
			db,
			data_subset_obj,
			avg_case_subset_obj,
			pack_object,
			db_input,
			parent_node_id,
			child_object
		);
	}
	else if( command_arg == "fit" )
	{	string tolerance    = argument_map["tolerance"];
		string max_num_iter = argument_map["max_num_iter"];
		fit_command(
			db               ,
			pack_object      ,
			db_input         ,
			s_info_vec       ,
			data_object      ,
			prior_object     ,
			tolerance        ,
			max_num_iter
		);
	}
	else if( command_arg == "truth" )
	{	truth_command(db);
	}
	else if( command_arg == "simulate" )
	{	size_t number_sample = std::atoi(
			argument_map["number_sample"].c_str()
		);
		simulate_command(
			db                       ,
			db_input.integrand_table ,
			data_subset_obj          ,
			data_object              ,
			actual_seed              ,
			number_sample
		);
	}
	else if( command_arg == "sample" )
	{	CppAD::vector<dismod_at::simulate_struct> simulate_table =
			dismod_at::get_simulate_table(db);
		string tolerance    = argument_map["tolerance"];
		string max_num_iter = argument_map["max_num_iter"];
		sample_command(
			db               ,
			data_object      ,
			data_subset_obj  ,
			pack_object      ,
			db_input         ,
			simulate_table   ,
			s_info_vec       ,
			prior_object     ,
			tolerance        ,
			max_num_iter
		);
	}
	else
		assert(false);
	// ---------------------------------------------------------------------
	sqlite3_close(db);
	return 0;
}
