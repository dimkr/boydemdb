/*
 * This file is part of boydemdb.
 *
 * Copyright (c) 2020 Dima Krasner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _BOYDEMDB_H_INCLDED
#	define _BOYDEMDB_H_INCLDED

#	include <stddef.h>
#	include <sys/types.h>

#	define BOYDEMDB_INIT NULL

#	ifdef BOYDEMDB_INTERNAL
#		include <sqlite3.h>
typedef sqlite3_int64 boydemdb_id;
#	else
#		include <inttypes.h>
typedef int64_t boydemdb_id;
#	endif

typedef struct {
#	ifdef BOYDEMDB_INTERNAL
	sqlite3 *db;
	sqlite3_stmt *select, *insert, *delete, *one;
#	else
	void *boydemdb, *select, *insert, *delete, *one;
#	endif
} *boydemdb;

typedef boydemdb_id boydemdb_type;

boydemdb_id boydemdb_set(boydemdb db,
                         const boydemdb_type type,
                         void *blob,
                         size_t size);
void *boydemdb_get(boydemdb db, boydemdb_id id, size_t *size);
void *boydemdb_one(boydemdb db,
                   const boydemdb_type type,
                   boydemdb_id *id,
                   size_t *size);
void boydemdb_delete(boydemdb db, boydemdb_id id);
boydemdb boydemdb_open(const char *path);
void boydemdb_close(boydemdb db);

#endif
