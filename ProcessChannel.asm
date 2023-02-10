; sorry for polish language in comments

OPTION CASEMAP:NONE

.data
align 16

ones dq 0101010101010101h, 0101010101010101h, 0101010101010101h, 0101010101010101h
zeros dq 0000000000000000h, 0000000000000000h, 0000000000000000h, 0000000000000000h

.code

;	Procedura nową wartość kanału na podstawie przekazanych do niej dwóch tablic:
;	- tablica kanałów otaczających aktualnie obliczany kanał wraz z tym kanałem
;	- tablica kolejnych wartości jądra (filtru) Gaussa

;   INPUT
;
;   RCX - adres tablicy kanałów w której znajdują się 32 liczby bez znaku 8 bitowe
;   RDX - adtes tablicy kolejnych wartości jądra Gaussa w której znajdują się 32 liczby bez znaku 8 bitowe

;	OUTPUT
;
;	RAX - nowa wartość kanału znajdującego się w środku tablicy kanałów

processChannel PROC
	
	VMOVDQU ymm1, YMMWORD PTR [RCX]			; załaduj do ymm1 32 bajty znajdujące się pod adresem z rejestru RCX
	VMOVDQU ymm2, YMMWORD PTR [RDX]			; załaduj do ymm2 32 bajty znajdujące się pod adresem z rejestru RDX

	VMOVDQU ymm3, YMMWORD PTR [ones]		; załaduj do ymm3 32 8 bitowych liczb, z których każda do 1
	VMOVDQU ymm4, YMMWORD PTR [zeros]		; wypełnij rejestr ymm4 zerami

	VPMADDUBSW ymm1, ymm1, ymm2				; wymnóż parami 8 bitowe liczby znajdujące się w rejestrach ymm1 i ymm2,
											; dodaj do siebie wymnożone pary i wynik zapisz jako liczby 16 bitowe bez znaku,
											; wynik zapisz do ymm1

	VPHADDW ymm1, ymm1, ymm4				
	VPHADDW ymm1, ymm1, ymm4
	VPHADDW ymm1, ymm1, ymm4				; po wykonaniu trzech operacji dodawania horyzontalnego liczb 16 bitowych
											; jedna część sumy znajduję się na 16 najmłodszych bitach rejestru ymm1 czyli
											; w rejestrze xmm1, natomiast druga część sumy znajduję się
											; na 16 najmłodszych bitach drugiej połowy rejestru ymm1

	VEXTRACTI128 xmm5, ymm1, 1				; przenieś do xmm5 młodszą połowę rejestru ymm1, teraz w xmm5 znajduje się
											; druga część sumy
	
	PEXTRW RAX, xmm1, 0						; przenieś 16 najmłodszych bitów z rejestru xmm1 do RAX
	PEXTRW RBX, xmm5, 0						; przenieś 16 najmłodszych bitów z rejestru xmm5 do RBX

	ADD RAX, RBX							; dodaj do siebie RAX i RBX, wynik zapisz w RAX,
											; wartość znajdująca się w RAX to dodane do siebie wszystkie pomnożone
											; elementy tablicy kanałów przez wartości z tablicy jądra Gaussa
	
	VPMADDUBSW ymm2, ymm2, ymm3				; wymnóż parami 8 bitowe liczby znajdujące się w rejestrach ymm2 i ymm3,
											; dodaj do siebie wymnożone pary i wynik zapisz jako liczby 16 bitowe bez znaku,
											; wynik zapisz do ymm2, w ymm3 znajdują się same 8 bitowe jedynki, więc
											; wynikiem jest tak naprawdę suma kolejnych 8 bitowych par z rejestru ymm2
											; zrzutowana na liczby 16 bitowe
	
	VPHADDW ymm2, ymm2, ymm4
	VPHADDW ymm2, ymm2, ymm4
	VPHADDW ymm2, ymm2, ymm4				; patrz linia 34

	VEXTRACTI128 xmm5, ymm2, 1				; patrz linia 39
	
	PEXTRW RCX, xmm2, 0						; patrz linia 42
	PEXTRW R9, xmm5, 0						; patrz linia 43

	ADD RCX, R9								; dodaj do siebie RCX i R9, wynik zapisz w RCX,
											; wartość znajdująca się w RCX to suma wszystkich elementów tablicy 
											; jądra Gaussa

	XOR RDX, RDX							; wyzeruj RDX aby uniknąć wyjątku "Integer overflow"

	CMP RCX, 0								; porównaj sumę RCX z 0
	JE div_by_0								; jeśli RCX == 0 to skocz do div_by_0 aby uniknąć dzielenia przez zero

	DIV RCX									; jeśli  RCX != 0 to pomnóż RAX przez RCX, wynik w RAX

	RET									

div_by_0:
	MOV RAX, 0								; jeśli RCX == 0 to zwróć 0
	RET

processChannel ENDP

END
