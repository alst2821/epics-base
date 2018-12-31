#!/usr/bin/perl

#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
#     National Laboratory.
# Copyright (c) 2014 The Regents of the University of California, as
#     Operator of Los Alamos National Laboratory.
# SPDX-License-Identifier: EPICS
# EPICS BASE is distributed subject to a Software License Agreement found
# in file LICENSE that is included with this distribution.
#*************************************************************************
#
#   makeStatTbl.pl - Create Error Symbol Table
#
#   Original Author: Kay-Uwe Kasemir, 1-31-97
#
# SYNOPSIS
# perl makeStatTbl.pl files.h...
#
# DESCRIPTION
# This tool creates a symbol table (ERRSYMTAB) structure which contains the
# names and values of all the status codes defined in the .h files named in
# its input arguments.  The status codes must be prefixed with "S_"
# in order to be included in this table.
# Module numbers definitions prefixed with "M_" are also read from the input
# files and included in the output.
#
# This tool's primary use is for creating an error status table used
# by errPrint, and errSymLookup.
#
# FILES
# errMdef.h   Module number file for each h directory
# errSymTbl.c Source file generated by tool in the cwd
#
# SEE ALSO: errnoLib(1), symLib(1)

use strict;
use Getopt::Std;

my $tool = 'makeStatTbl.pl';

our ($opt_h);
our $opt_o = 'errSymTbl.c';

$Getopt::Std::OUTPUT_HELP_VERSION = 1;

&HELP_MESSAGE unless getopts('ho:') && @ARGV;
&HELP_MESSAGE if $opt_h;

my (@syms, %vals, %msgs);

# Extract names, values and comments from all S_ and M_ symbol definitions
while (<>) {
    chomp;
    next unless m/^ \s* \# \s* define \s+ ([SM]_[A-Za-z0-9_]+)
        \s+ (.*?) \s* \/ \* \s* (.*?) \s* \* \/ \s* $/x;
    push @syms, $1;
    $vals{$1} = $2;
    $msgs{$1} = $3;
}

open my $out, '>', $opt_o or
    die "Can't create $opt_o: $!\n";

print $out <<"END";
/* Generated file $opt_o */

#include "errMdef.h"
#include "errSymTbl.h"
#include "dbDefs.h"

END

my @mods = grep {/^M_/} @syms;
my @errs = grep {/^S_/} @syms;

foreach my $mod (@mods) {
    my $val = $vals{$mod};
    my $msg = $msgs{$mod};
    print $out
        "#ifndef $mod\n",
        "#define $mod $val /* $msg */\n",
        "#endif\n";
}

print $out
    "\n",
    "static ERRSYMBOL symbols[] = {\n";

foreach my $err (@errs) {
    my $msg = escape($msgs{$err});
    my $val = $vals{$err};
    print $out
        "    { $val, \"$msg\"},\n";
}

print $out <<"END";
};

static ERRSYMTAB symTbl = {
    NELEMENTS(symbols), symbols
};

ERRSYMTAB_ID errSymTbl = &symTbl;

END

sub HELP_MESSAGE {
    print STDERR "Usage: $tool [-o file.c] files.h ...\n";
    exit 2;
}

sub escape {
    $_ = shift;
    s/"/\\"/g;
    return $_;
}
