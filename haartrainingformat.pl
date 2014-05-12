#!/usr/bin/perl
use strict;
use File::Basename;
################################################################################
# The MIT License
# 
# Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
################################################################################

sub usage {
    print 'Helper tool to convert a specific format into a haartraining format.' . "\n";
    print 'Usage: perl haartrainingformat.pl [option]... [filename]' . "\n";
    print '    filename' . "\n";
    print '        file to be read. STDIN is also supported. ' . "\n";
    print 'Options:' . "\n";
    print '    -l' . "\n";
    print '    --ls' . "\n";
    print '        input is an output of ls.' . "\n";
    print '    -t' . "\n";
    print '    --trim' . "\n";
    print '        trim heading 0 of numbers like from 00xx to xx.' . "\n";
    print '    -b' . "\n";
    print '    --basename' . "\n";
    print '        get basenames from filenames.' . "\n";
    print '    -h' . "\n";
    print '    --help' . "\n";
    print '        show this help.' . "\n";
    print "\n";
    print 'Example:' . "\n";
    print '  $ cat a.txt' . "\n";
    print '    image.jpg 68 47 89 101' . "\n";
    print '    image.jpg 87 66 90 80' . "\n";
    print '    image.jpg 95 105 33 32' . "\n";
    print '  $ perl haartrainingformal.pl a.txt' . "\n";
    print '    image.jpg 3 68 47 89 101 87 66 90 80 95 105 33 32' . "\n";
    print "\n";
    print '  $ \ls images/' . "\n";
    print '    images/image.jpg_0068_0047_0089_0101.png' . "\n";
    print '    images/image.jpg_0087_0066_0090_0080.png' . "\n";
    print '    images/image.jpg_0095_0105_0033_0032.png' . "\n";
    print '  $ \ls images/ | perl --ls --trim --basename haartrainingformal.pl' . "\n";
    print '    image.jpg 3 68 47 89 101 87 66 90 80 95 105 33 32' . "\n";
}

sub parse_args {
    my @ARGV = @_;
    my $infile = "";
    my $lsformat = 0;
    my $trim = 0; # trim heading 0 or not (0010 => 10)
    my $basename = 0;
    for (my $i = 0; $i <= $#ARGV; $i++) {
        if ($ARGV[$i] eq "-t" || $ARGV[$i] eq "--trim" || $ARGV[$i] eq "-trim") {
            $trim = 1;
        } elsif ($ARGV[$i] eq "-b" || $ARGV[$i] eq "--basename" || $ARGV[$i] eq "-basename") {
            $basename = 1;
        } elsif ($ARGV[$i] eq "-l" || $ARGV[$i] eq "-ls" || $ARGV[$i] eq "--ls") {
            $lsformat = 1;
        } elsif ($ARGV[$i] eq "-h" || $ARGV[$i] eq "--help") {
            &usage();
            exit;
        } elsif (substr($ARGV[$i],0,1) eq "-") {
            print STDERR $ARGV[$i] . ': No such option exist' . "\n";
            exit;
        } else {
            $infile = $ARGV[$i];
            if (!-f $infile) {
                print STDERR $infile . ': No such file exist' . "\n";
            }
        }
    }
    return ($infile, $lsformat, $trim, $basename);
}

sub ls2typical {
    my $trim = pop(@_);
    my @lines = @_;
    for (my $i = 0; $i <= $#lines; $i++) {
        if ($trim) {
            $lines[$i] =~ s/([^_]*).*_0*(\d+)_0*(\d+)_0*(\d+)_0*(\d+)\.[^.]*$/$1 $2 $3 $4 $5\n/g;
        } else {
            $lines[$i] =~ s/([^_]*).*_(\d+)_(\d+)_(\d+)_(\d+)\.[^.]*$/$1 $2 $3 $4 $5\n/g;
        }
    }
    return @lines;
}

sub typical2haartraining {
    my $basename = pop(@_);
    my $trim = pop(@_);
    my @lines = @_;
    my %counts = ();
    my %coords = ();
    foreach my $line (@lines) {
        $line =~ s/\s+$//;
        my @list = split(/ /, $line, 5);
        my $fname = shift(@list);
        if ($basename) { $fname = basename($fname); }
        if ($trim) {
            for (my $i = 0; $i <= $#list; $i++) {
                $list[$i] =~ s/^0*//;
            }
        }
        my $coord = ' ' . join(' ', @list);
        if (exists($counts{$fname})) {
            $counts{$fname}++;
            $coords{$fname} .= $coord;
        } else {
            $counts{$fname} = 1;
            $coords{$fname} = $coord;
        }
    }
    my @newlines = ();
    foreach my $fname (sort(keys(%counts))) {
        push(@newlines, $fname . ' ' . $counts{$fname} . $coords{$fname} . "\n");
    }
    return @newlines;
}
    
sub main {
    ## arguments
    my ($infile, $lsformat, $trim, $basename) = &parse_args(@_);
    ## read
    my @lines;
    if ($infile ne "") {
        open(INPUT, $infile); @lines = <INPUT>; close(INPUT);
    } else {
        @lines = <STDIN>;
    }
    ## body
    if ($lsformat) {
        @lines = &ls2typical(@lines, $trim);
    }
    @lines = &typical2haartraining(@lines, $trim, $basename);

    ## output
    print @lines;
}

&main(@ARGV);
