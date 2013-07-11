#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 5;

use IPC::Run qw(run start);

my $test_file = "t/test.txt";
run("touch $test_file");
my $kqwait_command = "./kqwait";

# wait on single file
my @kqwait  = split (' ', "$kqwait_command $test_file");
my $handle = start( \@kqwait, \my( $in,$out,$err));
run("echo 'hello world' > $test_file");
ok(finish $handle, 'should return on write');;
is($err, '', "should be no output to stderr");
is($out,"$test_file\n", "should return filename on write");

run("touch $test_file.1");
@kqwait = split(' ', "$kqwait_command $test_file $test_file.1");
$handle = start( \@kqwait, \$in,\$out,\$err);
run("echo 'hello world' > $test_file.1");
finish $handle;
is($err, '', "should be no output to stderr");
is($out,"$test_file.1\n", "should return correct filename when waiting on multiple files");

run("rm t/test.txt*");

