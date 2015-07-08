TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src
SUBDIRS += test

test.depends = src