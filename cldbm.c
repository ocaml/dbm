/***********************************************************************/
/*                                                                     */
/*                                OCaml                                */
/*                                                                     */
/*            Francois Rouaix, projet Cristal, INRIA Rocquencourt      */
/*                                                                     */
/*  Copyright 1996 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the GNU Library General Public License, with    */
/*  the special exception on linking described in file ../../LICENSE.  */
/*                                                                     */
/***********************************************************************/

/* $Id: cldbm.c 11156 2011-07-27 14:17:02Z doligez $ */

#include <string.h>
#include <fcntl.h>
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/callback.h>

#ifdef DBM_USES_GDBM_NDBM
#include <gdbm-ndbm.h>
#elif defined DBM_COMPAT
#include <ndbm.h>
#else
#include <gdbm.h>
#endif

#ifndef DBM_COMPAT
typedef struct gdbm_file_info DBM;
#endif

/* Quite close to sys_open_flags, but we need RDWR */
static int dbm_open_flags[] = {
  O_RDONLY, O_WRONLY, O_RDWR, O_CREAT
};

static void raise_dbm (char *errmsg) Noreturn;

static void raise_dbm(char *errmsg)
{
  static value * dbm_exn = NULL;
  if (dbm_exn == NULL)
    dbm_exn = caml_named_value("dbmerror");
  raise_with_string(*dbm_exn, errmsg);
}

#define DBM_val(v) *((DBM **) &Field(v, 0))
#define DBM_db_memory_val(v) *((datum **) &Field(v, 1))

static value alloc_dbm(DBM * db)
{
  value res = alloc_small(2, Abstract_tag);
  datum *db_mem = malloc(sizeof(datum));
  if (db_mem == NULL)
    caml_raise_out_of_memory();
  DBM_val(res) = db;
  DBM_db_memory_val(res) = db_mem;
  db_mem->dptr = NULL;
  db_mem->dsize = 0;
  return res;
}

static DBM * extract_dbm(value vdb)
{
  if (DBM_val(vdb) == NULL) raise_dbm("DBM has been closed");
  return DBM_val(vdb);
}

/* Dbm.open : string -> Sys.open_flag list -> int -> t */
value caml_dbm_open(value vfile, value vflags, value vmode) /* ML */
{
  char *file = String_val(vfile);
  int flags = convert_flag_list(vflags, dbm_open_flags);
  int mode = Int_val(vmode);
#ifdef DBM_COMPAT
  DBM *db = dbm_open(file,flags,mode);
#else
  char *filename = String_val(caml_stat_strconcat(2, file, ".pag"));
  DBM *db = gdbm_open(filename,0,flags,mode,NULL);
#endif

  if (db == NULL)
    raise_dbm("Can't open file");
  else
    return (alloc_dbm(db));
}

/* Dbm.close: t -> unit */
value caml_dbm_close(value vdb)       /* ML */
{
#ifdef DBM_COMPAT
  dbm_close(extract_dbm(vdb));
#else
  gdbm_close(extract_dbm(vdb));
#endif
  DBM_val(vdb) = NULL;
  return Val_unit;
}

/* Dbm.fetch: t -> string -> string */
value caml_dbm_fetch(value vdb, value vkey)  /* ML */
{
  datum key,answer;
  key.dptr = String_val(vkey);
  key.dsize = string_length(vkey);
#ifdef DBM_COMPAT
  answer = dbm_fetch(extract_dbm(vdb), key);
#else
  answer = gdbm_fetch(extract_dbm(vdb), key);
#endif
  if (answer.dptr) {
    value res = alloc_string(answer.dsize);
    memmove (String_val (res), answer.dptr, answer.dsize);
    return res;
  }
  else raise_not_found();
}

value caml_dbm_insert(value vdb, value vkey, value vcontent) /* ML */
{
  datum key, content;

  key.dptr = String_val(vkey);
  key.dsize = string_length(vkey);
  content.dptr = String_val(vcontent);
  content.dsize = string_length(vcontent);

#ifdef DBM_COMPAT
  switch(dbm_store(extract_dbm(vdb), key, content, DBM_INSERT)) {
#else
  switch(gdbm_store(extract_dbm(vdb), key, content, GDBM_INSERT)) {
#endif
  case 0:
    return Val_unit;
  case 1:                       /* DBM_INSERT and already existing */
    raise_dbm("Entry already exists");
  default:
    raise_dbm("dbm_store failed");
  }
}

value caml_dbm_replace(value vdb, value vkey, value vcontent) /* ML */
{
  datum key, content;

  key.dptr = String_val(vkey);
  key.dsize = string_length(vkey);
  content.dptr = String_val(vcontent);
  content.dsize = string_length(vcontent);

#ifdef DBM_COMPAT
  switch(dbm_store(extract_dbm(vdb), key, content, DBM_REPLACE)) {
#else
  switch(gdbm_store(extract_dbm(vdb), key, content, GDBM_REPLACE)) {
#endif
  case 0:
    return Val_unit;
  default:
    raise_dbm("dbm_store failed");
  }
}

value caml_dbm_delete(value vdb, value vkey)         /* ML */
{
  datum key;
  key.dptr = String_val(vkey);
  key.dsize = string_length(vkey);

#ifdef DBM_COMPAT
  if (dbm_delete(extract_dbm(vdb), key) < 0)
#else
  if (gdbm_delete(extract_dbm(vdb), key) < 0)
#endif
    raise_dbm("dbm_delete");
  else return Val_unit;
}

value caml_dbm_firstkey(value vdb)            /* ML */
{
#ifdef DBM_COMPAT
  datum key = dbm_firstkey(extract_dbm(vdb));
#else
  datum key = gdbm_firstkey(extract_dbm(vdb));
  datum *db_mem = DBM_db_memory_val(vdb);
  if (db_mem->dptr != NULL)
    free(db_mem->dptr);
  (DBM_db_memory_val(vdb))->dptr = key.dptr;
  (DBM_db_memory_val(vdb))->dsize = key.dsize;
#endif

  if (key.dptr) {
    value res = alloc_string(key.dsize);
    memmove (String_val (res), key.dptr, key.dsize);
    return res;
  }
  else raise_not_found();
}

value caml_dbm_nextkey(value vdb)             /* ML */
{
#ifdef DBM_COMPAT
  datum key = dbm_nextkey(extract_dbm(vdb));
#else
  datum key = {NULL, 0};
  datum *db_mem = DBM_db_memory_val(vdb);
  if (db_mem->dptr != NULL) {
    key = gdbm_nextkey(extract_dbm(vdb), *db_mem);
    free(db_mem->dptr);
    (DBM_db_memory_val(vdb))->dptr = key.dptr;
    (DBM_db_memory_val(vdb))->dsize = key.dsize;
  }
#endif

  if (key.dptr) {
    value res = alloc_string(key.dsize);
    memmove (String_val (res), key.dptr, key.dsize);
    return res;
  }
  else raise_not_found();
}
