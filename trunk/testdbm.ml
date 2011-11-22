(***********************************************************************)
(*                                                                     *)
(*                                CamlDBM                              *)
(*                                                                     *)
(*          Francois Rouaix, projet Cristal, INRIA Rocquencourt        *)
(*                                                                     *)
(*  Copyright 1996 Institut National de Recherche en Informatique et   *)
(*  en Automatique.  All rights reserved.  This file is distributed    *)
(*  under the terms of the GNU Library General Public License, with    *)
(*  the special exception on linking described in file ../../LICENSE.  *)
(*                                                                     *)
(***********************************************************************)

let optfind d k = try Some(Dbm.find d k) with Not_found -> None

let _ =
  let db = Dbm.opendbm "testdatabase" [Dbm.Dbm_rdwr;Dbm.Dbm_create] 0o666 in
  Dbm.add db "one" "un";
  Dbm.add db "two" "dos";
  Dbm.replace db "two" "deux";
  Dbm.add db "three" "trois";
  assert (optfind db "one" = Some "un");
  assert (optfind db "two" = Some "deux");
  assert (optfind db "three" = Some "trois");
  assert (optfind db "four" = None);
  Dbm.close db;
  let db = Dbm.opendbm "testdatabase" [Dbm.Dbm_rdonly] 0 in
  Dbm.iter (fun k d -> Printf.printf "key '%s' -> data '%s'\n" k d) db;
  Dbm.close db


  
  
