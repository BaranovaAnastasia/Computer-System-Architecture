;Option 3.
;Develop a program that enters a one-dimensional array A[N],
;forms a new array B of the sum of neighboring elements
;of array A (B = {A[0] + A[1], A[1] + A[2], ...}) and outputs it.
;Memory for arrays can be allocated either statically or dynamically
;at the developer's choice.
;
;Divide the solution of the problem into functions as follows:
;       1. Input and output of arrays make as subroutines
;       2. Execution of the task make as a procedure
;       3. Organize the output of both the source and generated arrays
;
;These procedures can use data directly (simulating procedures without parameters).
;Imitation of working with parameters is also allowed.

format PE console
entry start

include 'win32a.inc'
include 'MACRO\PROC32.INC'

section '.data' data readable writable

        strVecSize      db 'size of vector A? ', 0
        strIncorSize    db 'Incorrect size of vector = %d', 10, 0
        strVecElemI     db 'A[%d]? ', 0
        strScanInt      db '%d', 0
        strVecA         db 10, 10, 'Original vector A:', 10, 0
        strVecB         db 'Built vector B = {A[0] + A[1], ..., A[n - 1] + A[n]}:', 10, 0
        strEmptyVec     db 'Empty vector', 10, 0
        strVecElemOutA  db 'A[%d] = %d', 10, 0
        strVecElemOutB  db 'B[%d] = %d', 10, 0

        vec_size     dd 0
        sum          dd 0
        i            dd ?
        tmp          dd ?
        tmp_1        dd ?
        tmp_2        dd ?
        tmpStack     dd ?
        vec_a        rd 100
        vec_b        rd 100


;----------------------------------------------------------------
section '.code' code readable executable
start:
;1) vector A input
    call VectorInput
;2) vector B building
    call GetVectorB
;3) vectors output
    ;print A
    push strVecA
    call [printf]
    mov ebx, vec_a     ; ebx = &vec_a
    mov edx, strVecElemOutA
    call PrintVector

    ;print B
    push strVecB
    call [printf]
    mov ebx, vec_b     ; ebx = &vec_b
    mov edx, strVecElemOutB
    dec [vec_size]
    call PrintVector


finish:
    call [getch]

    push 0
    call [ExitProcess]


;----------------------------------------------------------------
VectorInput:
        ;1) A size input
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
        cmp eax, 0
        jge getVector

        ;wrong size
        push eax
        push strIncorSize
        call [printf]
        add esp, 8

        call [getch]
        push 0
        call [ExitProcess]

        ;2) A elements input
    getVector:
        xor ecx, ecx      ;ecx = 0
        mov ebx, vec_a    ;ebx = &vec_a
    getVectorLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endVectorInput

        ;element input
        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVectorLoop
endVectorInput:
        ret
;----------------------------------------------------------------
proc GetVectorB
    xor ecx, ecx       ;ecx = 0
    mov ebx, vec_a     ;ebx = &vec_a
    mov edx, vec_b     ;ebp = &vec_b
    mov eax, [ebx]     ;eax = A[0]
    add ebx, 4         ;move to the next element of A

getVectorBLoop:
    mov [tmp_2], edx
    mov [tmp_1], ebx
    cmp ecx, [vec_size]
    jge endGetVectorB

    add eax, [ebx]    ;eax = A[i] + A[i + 1]
    mov [edx], eax    ;B[i] =  eax

    mov eax, [ebx]    ;eax = A[i + 1]

    inc ecx
    mov ebx, [tmp_1]
    add ebx, 4
    mov edx, [tmp_2]
    add edx, 4
    jmp getVectorBLoop
endGetVectorB:
ret
endp
;----------------------------------------------------------------
PrintVector:
        mov [tmpStack], esp
        xor ecx, ecx       ;ecx = 0

        cmp [vec_size], 0  ;check if vector is empty
        jg printVectorLoop
        push strEmptyVec
        call [printf]
        jmp endPrintVector

    printVectorLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endPrintVector
        mov [i], ecx
        mov [tmp_1], edx

        ;print element
        push dword [ebx]
        push ecx
        push edx
        call [printf]
        mov edx, [tmp_1]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp printVectorLoop

endPrintVector:
        mov esp, [tmpStack]
        ret
;----------------------------------------------------------------

                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'