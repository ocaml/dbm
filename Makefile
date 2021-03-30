#########################################################################
#                                                                       #
#                                 CamlDBM                               #
#                                                                       #
#            Xavier Leroy, projet Gallium, INRIA Rocquencourt           #
#                                                                       #
#   Copyright 2011 Institut National de Recherche en Informatique et    #
#   en Automatique.  All rights reserved.  This file is distributed     #
#   under the terms of the GNU Library General Public License, with     #
#   the special exception on linking described in file ../../LICENSE.   #
#                                                                       #
#########################################################################

JBUILDER?=jbuilder
RM?=rm

# Legacy items kept to allow installation
LIBDIR=`ocamlc -where`
STUBLIBDIR=$(LIBDIR)/stublibs
A=`ocamlc -config | sed -ne 's/ext_lib: //p'`
SO=`ocamlc -config | sed -ne 's/ext_dll: //p'`
ROOT=_build/install/default

all:
	$(JBUILDER) build @install

install:
	if test -f $(ROOT)/lib/stublibs/dlldbm_stubs$(SO); then mkdir $(STUBLIBDIR) || echo Ok; cp $(ROOT)/lib/stublibs/dlldbm_stubs$(SO) $(STUBLIBDIR)/; fi
	cp $(ROOT)/lib/dbm/libdbm_stubs$(A) $(LIBDIR)/
#	cd $(LIBDIR) && ranlib libdbm_stubs$(A)
	cp $(ROOT)/lib/dbm/dbm.cma $(ROOT)/lib/dbm/dbm.cmxa $(ROOT)/lib/dbm/dbm.cmi $(ROOT)/lib/dbm/dbm.mli $(LIBDIR)/
	cp $(ROOT)/lib/dbm/dbm$(A) $(LIBDIR)/
#	cd $(LIBDIR) && ranlib dbm$(A)
	if test -f $(ROOT)/lib/dbm/dbm.cmxs; then cp $(ROOT)/lib/dbm/dbm.cmxs $(LIBDIR)/; fi

clean:
	$(RM) -f *.sexp
	$(JBUILDER) clean

test:
	$(JBUILDER) runtest --force
