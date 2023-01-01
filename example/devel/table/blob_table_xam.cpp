// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin blob_table_xam.cpp$$
$spell
   xam
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Writing and Reading Blobs: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cassert>
# include <dismod_at/blob_table.hpp>
# include <dismod_at/open_connection.hpp>

bool blob_table_xam(void)
{  bool   ok = true;
   //
   struct my_struct {int x; double y;};
   //
   // db
   std::string  file_name = "example.db";
   bool         new_file  = true;
   sqlite3*     db        = dismod_at::open_connection(file_name, new_file);
   //
   // one
   my_struct one;
   one.x = 1;
   one.y = 2.0;
   //
   // write from one -> blob
   std::string table_name  = "blob";
   std::string col_name    = "my_struct";
   size_t      sizeof_data = sizeof(one);
   void* data              = reinterpret_cast<void*>(&one);
   dismod_at::write_blob_table(db, table_name, col_name, sizeof_data, data);
   //
   // two
   my_struct two;
   assert( sizeof(two) == sizeof_data);
   //
   // read blob -> two
   data = reinterpret_cast<void*>(&two);
   dismod_at::read_blob_table(db, table_name, col_name, sizeof_data, data);
   //
   // check result
   ok &= one.x == two.x;
   ok &= one.y == two.y;
   //
   return ok;
}
// END C++
