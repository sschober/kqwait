#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 15;

use IPC::Run qw(run start timeout);

my $test_dir = "t";
my $test_file = "$test_dir/test.txt";
my $kqwait_command = "./kqwait";

sub do_test {
  my ($cmd, $action, $expectation, $msg) = @_;
  my @kqwait  = split (' ', $cmd);
  my $handle = start( \@kqwait, \my( $in,$out,$err),timeout(5));
  run($action);
  ok(finish $handle, 'should return');;
  is($err, '', "produces no output to stderr");
  warn $err if $err;
  is($out,$expectation, $msg);
}

# wait on single file
run("touch $test_file");
do_test(
  "$kqwait_command $test_file",
  "echo 'hello world' > $test_file",
  "$test_file\n",
  "returns filename on write"
);

# wait on multiple files
run("touch $test_file.1");
do_test(
  "$kqwait_command $test_file $test_file.1",
  "echo 'hello world' > $test_file.1",
  "$test_file.1\n",
  "returns correct filename when waiting on multiple files"
);

# wait on directory and create one file
do_test(
  "$kqwait_command $test_dir/",
  "echo 'hello world' > $test_file.2",
  "+ $test_file.2\n",
  "returns correct filename when creating a file in dir"
);

# wait on directory and delete one file
do_test(
  "$kqwait_command $test_dir/",
  "rm $test_file",
  "- $test_file\n",
  "returns correct filename when deleting a file in dir"
);

# wait on directory and file
my $test_dir_2    =   "$test_dir/test_dir";
my $test_file_2   =   "$test_dir_2/test.file";
run("touch $test_file");
run("mkdir $test_dir_2");
do_test(
  "$kqwait_command $test_file $test_dir_2/",
  "touch $test_file_2",
  "+ $test_file_2\n",
  "returns correct filename when waiting for file and dir"
);
run("rm -rf $test_dir_2");

run("rm t/test.txt*");
