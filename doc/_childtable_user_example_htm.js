// Child table for section user_example
document.write('\
<select onchange="user_example_child(this)">\
<option>user_example-&gt;</option>\
<option>user_age_avg_split.py</option>\
<option>user_bilevel_random.py</option>\
<option>user_bnd_mulcov.py</option>\
<option>user_cascade.py</option>\
<option>user_censor.py</option>\
<option>user_change_grid.py</option>\
<option>user_compress.py</option>\
<option>user_const_random.py</option>\
<option>user_const_value.py</option>\
<option>user_continue_fit.py</option>\
<option>user_covid_19.py</option>\
<option>user_csv2db.py</option>\
<option>user_data_density.py</option>\
<option>user_data_sim.py</option>\
<option>user_diabetes.py</option>\
<option>user_diff_constraint.py</option>\
<option>user_fit_fixed_both.py</option>\
<option>user_fit_meas_noise.py</option>\
<option>user_fit_random.py</option>\
<option>user_fit_sim.py</option>\
<option>user_group_mulcov.py</option>\
<option>user_hes_fixed_math.py</option>\
<option>user_hes_random.py</option>\
<option>user_hold_out.py</option>\
<option>user_ill_condition.py</option>\
<option>user_jump_at_age.py</option>\
<option>user_lasso_covariate.py</option>\
<option>user_mulstd.py</option>\
<option>user_no_children.py</option>\
<option>user_one_function.py</option>\
<option>user_predict_fit.py</option>\
<option>user_predict_mulcov.py</option>\
<option>user_re_scale.py</option>\
<option>user_residual.py</option>\
<option>user_sample_asy.py</option>\
<option>user_sample_asy_sim.py</option>\
<option>user_sample_sim.py</option>\
<option>user_sim_log.py</option>\
<option>user_speed.py</option>\
<option>user_subgroup_mulcov.py</option>\
<option>user_sum_residual.py</option>\
<option>user_trace_init.py</option>\
<option>user_warm_start.py</option>\
<option>user_zsum_child_rate.py</option>\
<option>user_zsum_mulcov_meas.py</option>\
<option>user_zsum_mulcov_rate.py</option>\
</select>\
');
function user_example_child(item)
{	var child_list = [
		'user_age_avg_split.py.htm',
		'user_bilevel_random.py.htm',
		'user_bnd_mulcov.py.htm',
		'user_cascade.py.htm',
		'user_censor.py.htm',
		'user_change_grid.py.htm',
		'user_compress.py.htm',
		'user_const_random.py.htm',
		'user_const_value.py.htm',
		'user_continue_fit.py.htm',
		'user_covid_19.py.htm',
		'user_csv2db.py.htm',
		'user_data_density.py.htm',
		'user_data_sim.py.htm',
		'user_diabetes.py.htm',
		'user_diff_constraint.py.htm',
		'user_fit_fixed_both.py.htm',
		'user_fit_meas_noise.py.htm',
		'user_fit_random.py.htm',
		'user_fit_sim.py.htm',
		'user_group_mulcov.py.htm',
		'user_hes_fixed_math.py.htm',
		'user_hes_random.py.htm',
		'user_hold_out.py.htm',
		'user_ill_condition.py.htm',
		'user_jump_at_age.py.htm',
		'user_lasso_covariate.py.htm',
		'user_mulstd.py.htm',
		'user_no_children.py.htm',
		'user_one_function.py.htm',
		'user_predict_fit.py.htm',
		'user_predict_mulcov.py.htm',
		'user_re_scale.py.htm',
		'user_residual.py.htm',
		'user_sample_asy.py.htm',
		'user_sample_asy_sim.py.htm',
		'user_sample_sim.py.htm',
		'user_sim_log.py.htm',
		'user_speed.py.htm',
		'user_subgroup_mulcov.py.htm',
		'user_sum_residual.py.htm',
		'user_trace_init.py.htm',
		'user_warm_start.py.htm',
		'user_zsum_child_rate.py.htm',
		'user_zsum_mulcov_meas.py.htm',
		'user_zsum_mulcov_rate.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}