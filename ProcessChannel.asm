
;	Kamil �uc Gliwice SSI grupa 1
;	10.02.2023
;	Rok akademicki: 2022/2023
;	Temat projektu: Filtr Gaussa


OPTION CASEMAP:NONE

.data
align 16

ones dq 0101010101010101h, 0101010101010101h, 0101010101010101h, 0101010101010101h
zeros dq 0000000000000000h, 0000000000000000h, 0000000000000000h, 0000000000000000h

.code

;	Procedura now� warto�� kana�u na podstawie przekazanych do niej dw�ch tablic:
;	- tablica kana��w otaczaj�cych aktualnie obliczany kana� wraz z tym kana�em
;	- tablica kolejnych warto�ci j�dra (filtru) Gaussa

;   INPUT
;
;   RCX - adres tablicy kana��w w kt�rej znajduj� si� 32 liczby bez znaku 8 bitowe
;   RDX - adtes tablicy kolejnych warto�ci j�dra Gaussa w kt�rej znajduj� si� 32 liczby bez znaku 8 bitowe

;	OUTPUT
;
;	RAX - nowa warto�� kana�u znajduj�cego si� w �rodku tablicy kana��w

processChannel PROC
	
	VMOVDQU ymm1, YMMWORD PTR [RCX]			; za�aduj do ymm1 32 bajty znajduj�ce si� pod adresem z rejestru RCX
	VMOVDQU ymm2, YMMWORD PTR [RDX]			; za�aduj do ymm2 32 bajty znajduj�ce si� pod adresem z rejestru RDX

	VMOVDQU ymm3, YMMWORD PTR [ones]		; za�aduj do ymm3 32 8 bitowych liczb, z kt�rych ka�da do 1
	VMOVDQU ymm4, YMMWORD PTR [zeros]		; wype�nij rejestr ymm4 zerami

	VPMADDUBSW ymm1, ymm1, ymm2				; wymn� parami 8 bitowe liczby znajduj�ce si� w rejestrach ymm1 i ymm2,
											; dodaj do siebie wymno�one pary i wynik zapisz jako liczby 16 bitowe bez znaku,
											; wynik zapisz do ymm1

	VPHADDW ymm1, ymm1, ymm4				
	VPHADDW ymm1, ymm1, ymm4
	VPHADDW ymm1, ymm1, ymm4				; po wykonaniu trzech operacji dodawania horyzontalnego liczb 16 bitowych
											; jedna cz�� sumy znajduj� si� na 16 najm�odszych bitach rejestru ymm1 czyli
											; w rejestrze xmm1, natomiast druga cz�� sumy znajduj� si�
											; na 16 najm�odszych bitach drugiej po�owy rejestru ymm1

	VEXTRACTI128 xmm5, ymm1, 1				; przenie� do xmm5 m�odsz� po�ow� rejestru ymm1, teraz w xmm5 znajduje si�
											; druga cz�� sumy
	
	PEXTRW RAX, xmm1, 0						; przenie� 16 najm�odszych bit�w z rejestru xmm1 do RAX
	PEXTRW RBX, xmm5, 0						; przenie� 16 najm�odszych bit�w z rejestru xmm5 do RBX

	ADD RAX, RBX							; dodaj do siebie RAX i RBX, wynik zapisz w RAX,
											; warto�� znajduj�ca si� w RAX to dodane do siebie wszystkie pomno�one
											; elementy tablicy kana��w przez warto�ci z tablicy j�dra Gaussa
	
	VPMADDUBSW ymm2, ymm2, ymm3				; wymn� parami 8 bitowe liczby znajduj�ce si� w rejestrach ymm2 i ymm3,
											; dodaj do siebie wymno�one pary i wynik zapisz jako liczby 16 bitowe bez znaku,
											; wynik zapisz do ymm2, w ymm3 znajduj� si� same 8 bitowe jedynki, wi�c
											; wynikiem jest tak naprawd� suma kolejnych 8 bitowych par z rejestru ymm2
											; zrzutowana na liczby 16 bitowe
	
	VPHADDW ymm2, ymm2, ymm4
	VPHADDW ymm2, ymm2, ymm4
	VPHADDW ymm2, ymm2, ymm4				; patrz linia 34

	VEXTRACTI128 xmm5, ymm2, 1				; patrz linia 39
	
	PEXTRW RCX, xmm2, 0						; patrz linia 42
	PEXTRW R9, xmm5, 0						; patrz linia 43

	ADD RCX, R9								; dodaj do siebie RCX i R9, wynik zapisz w RCX,
											; warto�� znajduj�ca si� w RCX to suma wszystkich element�w tablicy 
											; j�dra Gaussa

	XOR RDX, RDX							; wyzeruj RDX aby unikn�� wyj�tku "Integer overflow"

	CMP RCX, 0								; por�wnaj sum� RCX z 0
	JE div_by_0								; je�li RCX == 0 to skocz do div_by_0 aby unikn�� dzielenia przez zero

	DIV RCX									; je�li  RCX != 0 to pomn� RAX przez RCX, wynik w RAX

	RET									

div_by_0:
	MOV RAX, 0								; je�li RCX == 0 to zwr�� 0
	RET

processChannel ENDP

END