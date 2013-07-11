#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 12;

use IPC::Run qw(run start timeout);

my $test_dir = "t";
my $test_file = "$test_dir/test.txt";
my $kqwait_command = "./kqwait";

sub do_test {
  my ($cmd, $action, $expectation, $msg) = @_;
  my @kqwait  = split (' ', $cmd);
  my $handle = start( \@kqwait, \my( $in,$out,$err),timeout(2));
  run($action);
  ok(finish $handle, 'should return');;
  is($err, '', "should produce no output to stderr");
  is($out,$expectation, $msg);
}

# wait on single file
run("touch $test_file");
do_test(
  "$kqwait_command $test_file",
  "echo 'hello world' > $test_file",
  "$test_file\n",
  "should return filename on write"
);

# wait on multiple files
run("touch $test_file.1");
do_test(
  "$kqwait_command $test_file $test_file.1",
  "echo 'hello world' > $test_file.1",
  "$test_file.1\n",
  "should return correct filename when waiting on multiple files"
);

# wait on directory and create one file
do_test(
  "$kqwait_command $test_dir/",
  "echo 'hello world' > $test_file.2",
  "+ $test_file.2\n",
  "should return correct filename when creating a file in dir"
);

# wait on directory and delete one file
do_test(
  "$kqwait_command $test_dir/",
  "rm $test_file",
  "- $test_file\n",
  "should return correct filename when deleting a file in dir"
);


run("rm t/test.txt*");

