# This file is part of boydemdb.
#
# Copyright (c) 2020 Dima Krasner
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

project('boydemdb', 'c')

sqlite = custom_target('sqlite',
                       command: [join_paths(meson.current_source_dir(), 'amalgamate.sh'), meson.current_source_dir(), meson.current_build_dir()],
                       output: ['sqlite3.c', 'sqlite3.h'])

sqlite_cflags = [
	'-DSQLITE_OMIT_LOAD_EXTENSION',
	'-DSQLITE_THREADSAFE=0',
	'-DSQLITE_DEFAULT_AUTOVACUUM=1',
	'-DSQLITE_TEMP_STORE=3',
]

boydemdb_includes = include_directories('.')

libboydemdb = static_library('boydemdb',
                             'boydemdb.c', sqlite,
                             c_args: sqlite_cflags + ['-DBOYDEMDB_INTERNAL'],
                             include_directories: boydemdb_includes)

boydemdb_dependency = declare_dependency(link_with: libboydemdb,
                                         include_directories: boydemdb_includes)
