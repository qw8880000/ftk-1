SUBDIRS=data xul icons
include $(abs_top_srcdir)/src/rules.mk

LOCAL_CFLAGS=-DAPP_DATA_DIR="\"$(datadir)/ftk/orange\"" -DAPP_LOCAL_DATA_DIR="\"$(srcdir)\"" \
	-I$(abs_top_srcdir)/apps/common

lib_LTLIBRARIES=libapp-orange.la
libapp_orange_la_SOURCES=main.c ftk_app_orange.c ftk_orange_xul.c
libapp_orange_la_LDFLAGS=$(MYLDFLAGS)
libapp_orange_la_CFLAGS=$(LOCAL_CFLAGS)

noinst_PROGRAMS=orange
orange_SOURCES=main.c ftk_app_orange.c ftk_orange_xul.c
orange_LDFLAGS=$(MYLDFLAGS)
orange_CFLAGS=$(LOCAL_CFLAGS) -DHAS_MAIN

EXTRA_DIST=ftk_app_orange.h
