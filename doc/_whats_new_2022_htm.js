var list_up0 = [
'dismod_at.htm',
'release_notes.htm',
'whats_new_2022.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'whats_new_2022.htm#mm-dd',
'whats_new_2022.htm#mm-dd.11-03',
'whats_new_2022.htm#mm-dd.10-14',
'whats_new_2022.htm#mm-dd.09-16',
'whats_new_2022.htm#mm-dd.08-27',
'whats_new_2022.htm#mm-dd.08-26',
'whats_new_2022.htm#mm-dd.08-15',
'whats_new_2022.htm#mm-dd.05-19',
'whats_new_2022.htm#mm-dd.05-18',
'whats_new_2022.htm#mm-dd.05-02',
'whats_new_2022.htm#mm-dd.05-01',
'whats_new_2022.htm#mm-dd.04-21',
'whats_new_2022.htm#mm-dd.04-19',
'whats_new_2022.htm#mm-dd.04-05',
'whats_new_2022.htm#mm-dd.API Change',
'whats_new_2022.htm#mm-dd.04-04',
'whats_new_2022.htm#mm-dd.03-23',
'whats_new_2022.htm#mm-dd.03-19',
'whats_new_2022.htm#mm-dd.03-11',
'whats_new_2022.htm#mm-dd.03-08',
'whats_new_2022.htm#mm-dd.03-04',
'whats_new_2022.htm#mm-dd.02-17',
'whats_new_2022.htm#mm-dd.02-08',
'whats_new_2022.htm#mm-dd.02-05',
'whats_new_2022.htm#mm-dd.01-23',
'whats_new_2022.htm#mm-dd.01-22'
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
function choose_current0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_current0[index-1];
}
