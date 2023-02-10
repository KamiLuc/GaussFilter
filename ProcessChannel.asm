
;	Kamil £uc Gliwice SSI grupa 1
;	10.02.2023
;	Rok akademicki: 2022/2023
;	Temat projektu: Filtr Gaussa


OPTION CASEMAP:NONE

.data
align 16

ones dq 0101010101010101h, 0101010101010101h, 0101010101010101h, 0101010101010101h
zeros dq 0000000000000000h, 0000000000000000h, 0000000000000000h, 0000000000000000h

.code

;	Procedura now¹ wartoœæ kana³u na podstawie przekazanych do niej dwóch tablic:
;	- tablica kana³ów otaczaj¹cych aktualnie obliczany kana³ wraz z tym kana³em
;	- tablica kolejnych wartoœci j¹dra (filtru) Gaussa

;   INPUT
;
;   RCX - adres tablicy kana³ów w której znajduj¹ siê 32 liczby bez znaku 8 bitowe
;   RDX - adtes tablicy kolejnych wartoœci j¹dra Gaussa w której znajduj¹ siê 32 liczby bez znaku 8 bitowe

;	OUTPUT
;
;	RAX - nowa wartoœæ kana³u znajduj¹cego siê w œrodku tablicy kana³ów

processChannel PROC
	
	VMOVDQU ymm1, YMMWORD PTR [RCX]			; za³aduj do ymm1 32 bajty znajduj¹ce siê pod adresem z rejestru RCX
	VMOVDQU ymm2, YMMWORD PTR [RDX]			; za³aduj do ymm2 32 bajty znajduj¹ce siê pod adresem z rejestru RDX

	VMOVDQU ymm3, YMMWORD PTR [ones]		; za³aduj do ymm3 32 8 bitowych liczb, z których ka¿da do 1
	VMOVDQU ymm4, YMMWORD PTR [zeros]		; wype³nij rejestr ymm4 zerami

	VPMADDUBSW ymm1, ymm1, ymm2				; wymnó¿ parami 8 bitowe liczby znajduj¹ce siê w rejestrach ymm1 i ymm2,
											; dodaj do siebie wymno¿one pary i wynik zapisz jako liczby 16 bitowe bez znaku,
											; wynik zapisz do ymm1

	VPHADDW ymm1, ymm1, ymm4				
	VPHADDW ymm1, ymm1, ymm4
	VPHADDW ymm1, ymm1, ymm4				; po wykonaniu trzech operacji dodawania horyzontalnego liczb 16 bitowych
											; jedna czêœæ sumy znajdujê siê na 16 najm³odszych bitach rejestru ymm1 czyli
											; w rejestrze xmm1, natomiast druga czêœæ sumy znajdujê siê
											; na 16 najm³odszych bitach drugiej po³owy rejestru ymm1

	VEXTRACTI128 xmm5, ymm1, 1				; przenieœ do xmm5 m³odsz¹ po³owê rejestru ymm1, teraz w xmm5 znajduje siê
											; druga czêœæ sumy
	
	PEXTRW RAX, xmm1, 0						; przenieœ 16 najm³odszych bitów z rejestru xmm1 do RAX
	PEXTRW RBX, xmm5, 0						; przenieœ 16 najm³odszych bitów z rejestru xmm5 do RBX

	ADD RAX, RBX							; dodaj do siebie RAX i RBX, wynik zapisz w RAX,
											; wartoœæ znajduj¹ca siê w RAX to dodane do siebie wszystkie pomno¿one
											; elementy tablicy kana³ów przez wartoœci z tablicy j¹dra Gaussa
	
	VPMADDUBSW ymm2, ymm2, ymm3				; wymnó¿ parami 8 bitowe liczby znajduj¹ce siê w rejestrach ymm2 i ymm3,
											; dodaj do siebie wymno¿one pary i wynik zapisz jako liczby 16 bitowe bez znaku,
											; wynik zapisz do ymm2, w ymm3 znajduj¹ siê same 8 bitowe jedynki, wiêc
											; wynikiem jest tak naprawdê suma kolejnych 8 bitowych par z rejestru ymm2
											; zrzutowana na liczby 16 bitowe
	
	VPHADDW ymm2, ymm2, ymm4
	VPHADDW ymm2, ymm2, ymm4
	VPHADDW ymm2, ymm2, ymm4				; patrz linia 34

	VEXTRACTI128 xmm5, ymm2, 1				; patrz linia 39
	
	PEXTRW RCX, xmm2, 0						; patrz linia 42
	PEXTRW R9, xmm5, 0						; patrz linia 43

	ADD RCX, R9								; dodaj do siebie RCX i R9, wynik zapisz w RCX,
											; wartoœæ znajduj¹ca siê w RCX to suma wszystkich elementów tablicy 
											; j¹dra Gaussa

	XOR RDX, RDX							; wyzeruj RDX aby unikn¹æ wyj¹tku "Integer overflow"

	CMP RCX, 0								; porównaj sumê RCX z 0
	JE div_by_0								; jeœli RCX == 0 to skocz do div_by_0 aby unikn¹æ dzielenia przez zero

	DIV RCX									; jeœli  RCX != 0 to pomnó¿ RAX przez RCX, wynik w RAX

	RET									

div_by_0:
	MOV RAX, 0								; jeœli RCX == 0 to zwróæ 0
	RET

processChannel ENDP

END