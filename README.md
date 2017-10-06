# BAMBAM

A System stress tester.

## Description

This is a very small program to stress you machine. You can specify the number

## Installation

To compile simply run `make` and you will get the binary `bambam` you can run.
You can also install this binary by running `make install`, but in most cases you do not have any need to do this.

## Usage

    $ bambam -h

    Usage: bambam [options]

    -- Options ----
    	-h              -- Show this help
	    -t [time]       -- Runtime in seconds (default: 10)
    	-m [memory]     -- Memory to use in Megabytes (default: 0)
	    -f [forks]      -- Forks to start (default: 0)
    	-b [busy forks] -- Busy Forks to start (default: 0)

    -- Example ----
    bambam -t 60 -m 1024 -f 30 -b 1
