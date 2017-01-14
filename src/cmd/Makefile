# Filename: src/cmd/Makefile
# Project: lc
# Brief: categorize files in a directory; list multi-column for each category
#
# Copyright (C) 2016 Guy Shaw
# Written by Guy Shaw <gshaw@acm.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PROGRAM := lc
SRCS = $(PROGRAM).c
OBJS = $(PROGRAM).o
LIBS := ../liblc/liblc.a  ../libmc/libmc.a  ../libcscript/libcscript.a -lcurses

CC := gcc
CONFIG :=
CFLAGS := -g -Wall -Wextra
CPPFLAGS := -I../inc

.PHONY: all test clean-test clean

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(CONFIG) $(OBJS) $(LIBS)

test: $(PROGRAM)
	@cd test && make test

clean-test:
	cd test && make clean

clean: clean-test
	rm -f $(PROGRAM) core a.out mmv *.o *.a
	rm -f test_?? T.??
	rm -f *,FAILED

show-targets:
	@show-makefile-targets

show-%:
	@echo $*=$($*)
