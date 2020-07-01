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

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "boydemdb.h"

#define SELECT_QUERY "SELECT data FROM blobs WHERE id = ?"
#define ONE_QUERY "SELECT id, data FROM blobs WHERE type = ? LIMIT 1"
#define INSERT_QUERY "INSERT INTO blobs(type, data) VALUES (?, ?)"
#define DELETE_QUERY "DELETE FROM blobs WHERE id = ?"
#define CREATE_TABLE_QUERY "CREATE TABLE IF NOT EXISTS blobs (id INTEGER PRIMARY KEY, type INTEGER, data BLOB)"

boydemdb_id boydemdb_set(boydemdb db,
                         const boydemdb_type type,
                         void *blob,
                         size_t size)
{
	sqlite3_int64 id = -1;

	if (size > INT_MAX)
		return id;

	if (sqlite3_bind_int64(db->insert, 1, type) != SQLITE_OK)
		return id;

	if (sqlite3_bind_blob(db->insert,
	                      2,
	                      blob,
	                      (int)size,
	                      SQLITE_STATIC) != SQLITE_OK)
		goto reset;

	if (sqlite3_step(db->insert) != SQLITE_DONE)
		goto reset;

	id = sqlite3_last_insert_rowid(db->db);

reset:
	sqlite3_clear_bindings(db->insert);
	sqlite3_reset(db->insert);

	return id;
}

void *boydemdb_get(boydemdb db, boydemdb_id id, size_t *size)
{
	const void *p;
	unsigned char *copy = NULL;

	if (sqlite3_bind_int64(db->select, 1, id) != SQLITE_OK)
		return copy;

	if (sqlite3_step(db->select) != SQLITE_ROW)
		goto reset;

	p = sqlite3_column_blob(db->select, 0);
	*size = (size_t)sqlite3_column_bytes(db->select, 0);

	copy = malloc(*size + 1);
	if (!copy)
		goto reset;

	memcpy(copy, p, *size);
	copy[*size] = '\0';

reset:
	sqlite3_clear_bindings(db->select);
	sqlite3_reset(db->select);

	return copy;
}

void *boydemdb_one(boydemdb db,
                   const boydemdb_type type,
                   boydemdb_id *id,
                   size_t *size)
{
	const void *p;
	unsigned char *copy = NULL;

	if (sqlite3_bind_int(db->one, 1, type) != SQLITE_OK)
		return copy;

	if (sqlite3_step(db->one) != SQLITE_ROW)
		goto reset;

	*id = sqlite3_column_int64(db->one, 0);

	p = sqlite3_column_blob(db->one, 1);
	*size = (size_t)sqlite3_column_bytes(db->one, 1);

	copy = malloc(*size + 1);
	if (!copy)
		goto reset;

	memcpy(copy, p, *size);
	copy[*size] = '\0';

reset:
	sqlite3_clear_bindings(db->one);
	sqlite3_reset(db->one);

	return copy;
}

void boydemdb_delete(boydemdb db, boydemdb_id id)
{
	if (sqlite3_bind_int64(db->delete, 1, id) != SQLITE_OK)
		return;

	sqlite3_step(db->delete);

	sqlite3_clear_bindings(db->delete);
	sqlite3_reset(db->delete);
}

boydemdb boydemdb_open(const char *path)
{
	boydemdb db;

	db = malloc(sizeof(*db));
	if (!db)
		return NULL;

	if (sqlite3_open_v2(path,
	                    &db->db,
	                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
	                    NULL) != SQLITE_OK) {
		free(db);
		return NULL;
	}

	if (sqlite3_exec(db->db,
	                 CREATE_TABLE_QUERY,
	                 NULL,
	                 NULL,
	                 NULL) != SQLITE_OK) {
		sqlite3_close(db->db);
		free(db);
		return NULL;
	}

	if (sqlite3_prepare(db->db,
	                    INSERT_QUERY,
	                    -1,
	                    &db->insert,
	                    NULL) != SQLITE_OK) {
		sqlite3_close(db->db);
		free(db);
		return NULL;
	}

	if (sqlite3_prepare(db->db,
	                    SELECT_QUERY,
	                    -1,
	                    &db->select,
	                    NULL) != SQLITE_OK) {
		sqlite3_finalize(db->insert);
		sqlite3_close(db->db);
		free(db);
		return NULL;
	}

	if (sqlite3_prepare(db->db,
	                    ONE_QUERY,
	                    -1,
	                    &db->one,
	                    NULL) != SQLITE_OK) {
		sqlite3_finalize(db->select);
		sqlite3_finalize(db->insert);
		sqlite3_close(db->db);
		free(db);
		return NULL;
	}

	if (sqlite3_prepare(db->db,
	                    DELETE_QUERY,
	                    -1,
	                    &db->delete,
	                    NULL) != SQLITE_OK) {
		sqlite3_finalize(db->one);
		sqlite3_finalize(db->select);
		sqlite3_finalize(db->insert);
		sqlite3_close(db->db);
		free(db);
		return NULL;
	}

	return db;
}

void boydemdb_close(boydemdb db)
{
	sqlite3_finalize(db->delete);
	sqlite3_finalize(db->one);
	sqlite3_finalize(db->select);
	sqlite3_finalize(db->insert);

	sqlite3_close(db->db);

	free(db);
}