# Decomment Project

Name: James Kyoung Ho Kim

Student ID: [2017-13897]

Assignment: Assignment #1 - Decomment Program


## Overview
This project processes a C source file (decomment.c) and removes all comments from it, producing an output file without comments.

## Files
- decomment.c: The source code for the decomment program.
- decomment.i: Preprocessed file, generated from decomment.c.
- decomment.s: Assembly file, generated from decomment.c.
- decomment.o: Object file, generated from decomment.c.

## Collaborations
I bounced ideas off of Kim GeunYoung and Kim GeunWoo to check for edge cases, however, did not collaborate with anyone regarding my final code and implementation. 

## Possible Bugs
According to Requirement 8 (EOF before a comment is terminated) of the assignment, I decided to return EXIT_FAILURE when a single-line comment reached EOF before the state was exited with \n. Based off C documentation (https://www.ibm.com/docs/en/zos/2.4.0?topic=elements-comments), a new line not immediately preceded by a backslash terminates this form of comment. Hence, a single line comment without a newline at the end of a c file is, by definition, still within the comment state. 