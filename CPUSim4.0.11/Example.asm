; This program reads in integers until -999
; is read in.  Then it outputs the numbers in reverse order.
;
; A0 holds the new numbers as they are read in.
; A1 holds the sentinel value -999
; A2 holds the constant 1
; A3 counts the number of elements read in, initially 0

	   load A1 sentinel  	;initialize A1 to sentinel value -999
	   load A2 one		; loadi constant 1 into A2
	   load A3 zero 	; initialize A3 to 0

	   ;;read in the numbers and push them on the stack
_Beginning: read A0  		; read num -> A0

	   ;;test for sentinel
	   subtract A0 A1	; subtract -999
           jmpz A0 _FinishUp  	; jump if difference was zero
	   add A0 A1	 	; add -999 back to restore the number

           push A0 		; push A0 onto the stack
	   add A3 A2  		; increment the counter
	   jump _Beginning	; go back and read in the next number

	   ;;pop and write out the numbers
_FinishUp:  jmpz A3 _done 	; quit if length is 0
  	   pop A0  		; pop the stack and put it in A0
	   write A0		; output the next value
	   subtract A3 A2  	; subtract one from the command in A1
	   jump _FinishUp	; go back and print out the next number

_done:      end			; stop

_one:	   .data 2 1 		; constant 1
_zero:	   .data 2 0		; constant 0
_sentinel:  .data 2 -999		; constant -999
