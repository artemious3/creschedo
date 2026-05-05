# CreSchedo -- OS Scheduler Simulator

*Work in progress*

Simulator of an OS scheduler, using Round Robin algorithm with multiple priorities (one queue per priority).

## Build

Use `make` for building (`bear` is required).

For optimized build use `make release`.

You can specify number of CPU available in simulation
with `make NCPU=8`.

## Usage

```
@<filename>      -- execute all commands from a file <filename>
spawn <prio>     -- spawn process with priority <prio>;
                    You will be asked to input a program;
                    program consists of lines `<N> <Action>`,
                    where <N> is a positive number,
                    <Action> is one of `r` - run, `w` - wait.
                    End program with an empty line.
kill <prid>      -- kill process with id <prid>
cpu              -- show table of CPUs
ps               -- show table of processess
tick             -- invoke simulation tick
run <log_option> -- invoke ticks until all processes are finished
                    log options are:
                    `show_cpu` -- show state of cpus after each tick
                    `show_proc` -- show state of processes after each tick
clear            -- reset simulation
metrics          -- show metrics
help             -- show this help message
```

Example script for simulator:

```
spawn 2
2 r
5 w
3 r

spawn 2
10 r
2 w
3 r

spawn 4
10 r
2 w
3 r

spawn 4
3 r
2 w
2 r

spawn 4
5 r
2 w
5 r

spawn 13
5 r
2 w
5 r
```


Example output of `run show_proc`:

```
> run show_proc
<id> -active on cpu <id>;
w - waiting;. - ready; F - finished
1   2   3   4   5   6
0   1   .   .   .   .
0   1   .   .   .   .
w   1   0   .   .   .
w   1   0   .   .   .
w   1   0   .   .   .
w   1   0   .   .   .
w   1   0   .   .   .
.   1   0   .   .   .
.   1   0   .   .   .
.   1   0   .   .   .
1   w   0   .   .   .
1   w   0   .   .   .
1   0   w   .   .   .
F   0   w   1   .   .
F   0   .   1   .   .
F   F   .   1   0   .
F   F   1   w   0   .
F   F   1   w   0   .
F   F   1   .   0   .
F   F   F   1   0   .
F   F   F   1   w   0
F   F   F   F   w   0
F   F   F   F   1   0
F   F   F   F   1   0
F   F   F   F   1   0
F   F   F   F   1   w
F   F   F   F   1   w
F   F   F   F   F   0
F   F   F   F   F   0
F   F   F   F   F   0
F   F   F   F   F   0
F   F   F   F   F   0
F   F   F   F   F   F
simulation finished in 33 ticks
```

Example output of `run show_cpu`:

```
> run show_cpu
0     1
------------
1     2
1     2
3     2
3     2
3     2
3     2
3     2
3     2
3     2
3     2
3     1
3     1
2     1
2     4
2     4
5     4
5     3
5     3
5     3
5     4
6     4
6     -
6     5
6     5
6     5
-     5
-     5
6     -
6     -
6     -
6     -
6     -
-     -
simulation finished in 33 ticks
```
