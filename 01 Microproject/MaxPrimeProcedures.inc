;----Function checking if number is prime-----------------------------------------------------------
; The number to test is in bx register
; Result puts in eax register (1 if number is prime, 0 otherwise)
macro IsPrime {
        sub cx, 1          ; put first possible divisor (number - 1) in cx

findDivisor:
        ; check if its the last possible divisor (no need to check 1 as a divisor)
        cmp cx, 1
        je prime           ; number is prime

        mov ax, bx         ; put the number in ax
        xor dx, dx         ; dx = 0
        div cx             ; ax / cx, (remainder of the division is in dx)
        cmp dx, 0          ; check if cx is a divisor (the remainder = 0)
        je notPrime        ; number is not prime (cx is a divisor)

        loopw findDivisor  ; to the next divisor

prime:
        ; put result in eax (number is prime)
        mov eax, 1
        jmp  endIsPrime
notPrime:
        ; put result in eax (number is not prime)
        mov eax, 0
        jmp  endIsPrime
endIsPrime:
}
;----End of function--------------------------------------------------------------------------------


;----Function looking for the maximum prime number in the range from 1 to an unsigned machine word--
; Result is in ecx register
macro FindMaxPrime {
        ; find an unsigned machine word value
        xor ecx, ecx
        sub cx, word 1

nextNumber:
        mov bx, cx            ; store the current number in bx register
        IsPrime               ; check if the value in cx(bx) is prime

        mov cx, bx            ; restore the current number in cx register

        cmp eax, 1            ; if the number is prime go to the end
        je endFindMaxPrime

        loopw nextNumber

endFindMaxPrime:
}
;----End of function--------------------------------------------------------------------------------

;----Procedure printing information to the user-----------------------------------------------------
macro Start {
        xor ecx, ecx
        sub cx, word 1        ; find an unsigned machine word value

        push ecx              ; print information about the search range
        push strRange
        call [printf]
        add esp, 8             ; move the stack pointer

        push strStart          ; print start line
        call [printf]
        call [scanf]           ; wait for user to press enter
        add esp, 4             ; move the stack pointer


endPrintInfo:
}
;----End of procedure-------------------------------------------------------------------------------

;----Procedure printing program result--------------------------------------------------------------
; Result is in ecx register
macro End {
        push ecx               ; print program result
        push strResult
        call [printf]
        add esp, 8             ; move the stack pointer

        push strResInfo        ; print additional information
        call [printf]
        add esp, 4             ; move the stack pointer

        push strEnd            ; print exit prompt
        call [printf]
        add esp, 4             ; move the stack pointer

endPrintResult:
}
;----End of procedure-------------------------------------------------------------------------------
