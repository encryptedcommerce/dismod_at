var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_utility.htm',
'child_info.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'child_info.htm#Syntax',
'child_info.htm#child_object',
'child_info.htm#Constructor',
'child_info.htm#Constructor.parent_node_id',
'child_info.htm#Constructor.node_table',
'child_info.htm#Constructor.table',
'child_info.htm#child_size',
'child_info.htm#child_size.n_child',
'child_info.htm#child_id2node_id',
'child_info.htm#child_id2node_id.child_id',
'child_info.htm#child_id2node_id.node_id',
'child_info.htm#table_id2child',
'child_info.htm#table_id2child.table_id',
'child_info.htm#table_id2child.child'
];
var list_down3 = [
'math_abstract.htm',
'install_unix.htm',
'get_started.htm',
'user.htm',
'database.htm',
'model.htm',
'command.htm',
'python.htm',
'devel.htm',
'whats_new_2018.htm',
'wish_list.htm'
];
var list_down2 = [
'devel_table.htm',
'devel_utility.htm',
'devel_model.htm'
];
var list_down1 = [
'a1_double.htm',
'avgint_subset.htm',
'bilinear_interp.htm',
'child_info.htm',
'cohort_ode.htm',
'data_subset.htm',
'eigen_ode2.htm',
'error_exit.htm',
'fixed_effect.htm',
'get_prior_mean.htm',
'number_random_const.htm',
'age_avg_grid.htm',
'pack_info.htm',
'pack_prior.htm',
'residual_density.htm',
'random_number.htm',
'random_effect.htm',
'grid2line.htm',
'solve_ode.htm',
'split_space.htm',
'time_line_vec.htm'
];
function choose_up0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_up0[index-1];
}
function choose_across0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_across0[index-1];
}
function choose_current0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_current0[index-1];
}
function choose_down3(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down3[index-1];
}
function choose_down2(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down2[index-1];
}
function choose_down1(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down1[index-1];
}
function choose_down0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down0[index-1];
}
