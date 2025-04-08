# Comparing Bloom Filters vs Blocked Bloom Filters vs Sectorized Bloom Filters

A C++ implementation of three types of bloom filters and experiments to compare them.

# Running Code

To run experiments:

```Bash
$ make
$ ./bin/csc2525-project
```

Each experiment outputs to a semicolon-separated file. The FPR results are given as a number of false positives, not a rate.

To run tests:

First comment out `runExps()` in main.

```Bash
$ make
$ ./bin/csc2525-project-test
```
