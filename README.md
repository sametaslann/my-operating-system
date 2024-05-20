# MyOS

Default: Test for System Calls
$ make

Compile Options:
$make PART=-D<part_name> PT=-D<visibility_of_process_table> SPEED = -D<speed_of_interrupt>

part_name:
PARTA_1: For Part A strategy
PARTB_1: Part B 1. strategy
PARTB_2: Part B 2. Strategy
PARTB_3: Part B 3. Strategy
PARTB_4: Part B 4. Strategy
PARTC_1: Part C 1. Strategy
PARTC_2: Part C 2. Strategy

visibility_of_process_table:
SHOW: Print process table for each context switch

speed_of_interrupt:
SLOW: Implement context switches every 50 timer interrupts and print dummy in the process table print.
