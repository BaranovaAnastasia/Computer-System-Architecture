; Baranova Anastasia BSE196
; Course: Computer System Architecture

; Microproject:
; Develop a program calculating the maximum prime number
; in the range from 1 to an unsigned machine word

format PE console
entry start

include 'win32a.inc'
include 'MaxPrimeProcedures.inc'

section '.data' data readable
        strRange    db 'Search range: [1; %d] (from 1 to an unsigned machine word)', 10, 0
        strStart    db 'Press ENTER to calculate the maximum prime number...', 0
        strResult   db 10, '|  Result: %d  |', 10, 0
        strResInfo  db 10, 'Calculated number is prime and the biggest for the given range.', 10, 0
        strEnd      db 10, 'Press any key to exit...', 0

;==================================================================================================

section '.code' code readable executable

start:
        Start          ; Prints information about the process
        FindMaxPrime   ; Finds the maximum prime number in the range
        End            ; Prints result + info

finish:
        call [getch]
        push 0
        call [ExitProcess]

;==================================================================================================
                                                 
section '.idata' import data readable

        library kernel, 'kernel32.dll',\
                msvcrt, 'msvcrt.dll'
        import kernel,\
               ExitProcess, 'ExitProcess'
        import msvcrt,\
               printf, 'printf',\
               scanf, 'scanf',\
               getch, '_getch'