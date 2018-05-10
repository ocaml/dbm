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

include Makefile.config

OCAMLC=ocamlc
OCAMLOPT=ocamlopt
OCAMLMKLIB=ocamlmklib
OCAMLDEP=ocamldep
OCAMLRUN=ocamlrun
O=o
A=a
SO=so
LIBDIR=`ocamlc -where`
STUBLIBDIR=$(LIBDIR)/stublibs


all: libcamldbm.$(A) dbm.cma dbm.cmxa dbm.cmxs 

dbm.cma: dbm.cmo
	$(OCAMLMKLIB) -o dbm -oc camldbm -linkall dbm.cmo $(DBM_LINK)

dbm.cmxa: dbm.cmx
	$(OCAMLMKLIB) -o dbm -oc camldbm -linkall dbm.cmx $(DBM_LINK)

dbm.cmxs: dbm.cmxa libcamldbm.$(A)
	$(OCAMLOPT) -shared -o dbm.cmxs -I . dbm.cmxa

libcamldbm.$(A): cldbm.$(O)
	$(OCAMLMKLIB) -oc camldbm cldbm.$(O) $(DBM_LINK)

.SUFFIXES: .ml .mli .cmi .cmo .cmx .$(O)

.mli.cmi:
	$(OCAMLC) -c $(COMPFLAGS) $<

.ml.cmo:
	$(OCAMLC) -c $(COMPFLAGS) $<

.ml.cmx:
	$(OCAMLOPT) -c $(COMPFLAGS) $<

.c.$(O):
	$(OCAMLC) -c -ccopt "$(DBM_INCLUDES)" -ccopt "$(DBM_DEFINES)" $<

depend:
	$(OCAMLDEP) *.ml *.mli > .depend

install::
	if test -f dllcamldbm.$(SO); then mkdir $(STUBLIBDIR) || echo Ok; cp dllcamldbm.$(SO) $(STUBLIBDIR)/; fi 
	cp libcamldbm.$(A) $(LIBDIR)/
	cd $(LIBDIR) && ranlib libcamldbm.$(A)
	cp dbm.cma dbm.cmxa dbm.cmi dbm.mli $(LIBDIR)/
	cp dbm.$(A) $(LIBDIR)/
	cd $(LIBDIR) && ranlib dbm.$(A)
	if test -f dbm.cmxs; then cp dbm.cmxs $(LIBDIR)/; fi

clean::
	rm -f *.cm* *.$(O) *.$(A) *.$(SO)

testdbm.byte: dbm.cma testdbm.ml
	$(OCAMLC) -o $@ dbm.cma testdbm.ml

testdbm.opt: dbm.cmxa testdbm.ml
	$(OCAMLOPT) -ccopt -L. -o $@ dbm.cmxa testdbm.ml

clean::
	rm -f testdbm.byte testdbm.opt testdatabase.*

test: testdbm.byte testdbm.opt
	rm -f testdatabase.*
	ocamlrun -I . ./testdbm.byte
	rm -f testdatabase.*
	./testdbm.opt
	rm -f testdatabase.*
	

include .depend
