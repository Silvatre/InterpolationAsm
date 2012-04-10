.686
.mmx
.xmm
.model flat, stdcall
.data
	POLOWA REAL4 0.5
	DWA REAL4 2.0
.code

ConvertYtoX proc uses eax ecx ebx edx, y: REAL8, b: REAL8, a: REAL8, pi2: REAL8
	
	LOCAL temp: REAL8

	finit
	movhpd xmm0, y			; Zaladowanie y do starszych 64 bitow rejestru XMM0
	movlpd xmm0, b			; Zaladowanie b do mlodszych 64 bitow rejestru XMM0
	movhpd xmm1, a			; Zaladowanie a do starszych 64 bitow rejestru XMM0
	movlpd xmm1, a			; Zaladowanie a do mlodszych 64 bitow rejestru XMM0
	subpd xmm0, xmm1		; Odejmowanie - wynik w xmm0.

	movlpd temp, xmm0
	fld temp
	movhpd temp, xmm0
	fld temp
	fdiv ST(0), ST(1)
	fld pi2
	fmul
	ret						

ConvertYtoX endp

ValuesInitialization proc uses eax ecx ebx edx, m: ptr DWORD, n: DWORD, b: REAL8, a: REAL8, ddelta: ptr DWORD, y: ptr REAL8
	
	LOCAL delta: REAL8
	LOCAL temp: DWORD

	finit
	fld a					; Zaladowanie do ST(0) zmiennej a
	fld b					; Zaladowanie do ST(0) zmiennej b
	fsub ST(0), ST(1)		; Wykonanie (b-a)

	mov ecx, n				; Zaladowanie n do ecx
	inc ecx					; Inkrementacja ecx
	mov temp, ecx			; Zaladowanie do temp wartosci n+1

	fidiv temp				; Podzielenie (b-a) przez (n+1)
	fstp delta				; Zaladowanie wyniku do delta

	mov ebx, y				; Zaladowanie adresu tablicy y do ebx
	fld b					; Zaladowanie do ST(0) zmiennej calkowitej b
	fstp real8 ptr [ebx][ecx*8] ; Zaladowanie b do y[n+1]

	fild n					; Zaladowanie n
	fdiv DWA				; Podzielenie n przez 2

	mov ebx, m				; Zaladowanie adresu zmiennej m do ebx
	fist DWORD ptr [ebx]		; Wpisanie wartosci do zmiennej m
	fmul DWA				; Pomnozenie m przez 2
	fild n					; Zaladowanie n
	fsub ST(0), ST(1)		; Wykonanie odejmowania: n - (2*m)

	mov ebx, ddelta			; Zaladowanie adresu ddelta
	fistp dword ptr [ebx]	; Wpisanie wyniku do ddelta
	
	fld delta				; Zaladowanie wartosci zmiennej delta do ST(0)
	ret						; Zwrocenie tej wartosci poprzez rejestr ST(0)

ValuesInitialization endp

FactorsGeneration proc uses eax ecx ebx edx, endVal: DWORD, n: DWORD, begin: DWORD, x: ptr REAL8, f:ptr REAL8, aa: ptr REAL8, bb:ptr REAL8
	
	LOCAL u: REAL8
	LOCAL u1: REAL8
	LOCAL u2: REAL8
	LOCAL beta: REAL8
	LOCAL tempZew: DWORD
	LOCAL j: DWORD
	LOCAL tempVal: DWORD
	LOCAL tempReal8: REAL8
	LOCAL tempTryg: REAL8
	
	;----------------------- Inicjalizacja zmiennych i rejestrow

	mov ecx, begin			; Ustawienie poczatkowej wartosci kroku petli
	mov ebx, x				; EBX przechowuje x	
	mov edx, f				; EDX przechowuje f
	finit					; Inicjalizacja FPU

	;-----------------------
	
petlaZew:

	lea eax, [ebx + ecx*8]
	fldz					; ST(0) = 0 [ST = 1]
	fst u1					; Zerowanie u1 oraz u2
	fstp u2					; [ST = 0]
	fld real8 ptr [eax]		; ST(0) = x[j] [ST = 1]
	fstp beta				; Zaladowanie do beta wartosci x[j] [ST = 0]
	mov tempZew, ecx
	mov ecx, n				; Zaladowanie n do ECX, rejestr kroku petli
	fld beta				; Cos beta do zmiennej [ST = 1]
	lea eax, [edx + ecx*8 + 8]	; Zaladowanie adresu f[n+1]
	fcos
	mov tempVal, 2
	fstp tempTryg			; [ST = 0]
	
petlaWew:

	;------------------- Obliczanie wartosci:  u = f[k] + 2.0*u1*cos(beta) - u2

	fld tempTryg			; [ST = 1]	
	sub eax, 8	
	fld u1					; [ST = 2]
	fmul					; Obliczenie u1*cos(beta)		[ST = 1]
	fimul tempVal			; Obliczenie 2.0*u1*cos(beta)	[ST = 1]
	fld u2					; [ST = 2]
	fsubp st(1), st(0)		; [ST = 1]
	;fxch st(1)
	fld real8 ptr [eax]		; [ST = 2]
	sub ecx, 1
	fadd					; [ST = 1]
	
	;------------------- Wykonanie przypisan: u2 = u1, u1 = u;
	
	fld u1					; [ST = 1]	
	fstp u2					; [ST = 0]
	fst u					; [ST = 0]
	fstp u1

	;----------------------- Warunkowanie skoku petli for (k = n; k >= 1; k--)
	
	cmp ecx, 1
	jae petlaWew

	;----------------------- Obliczenie wartosci wyrazen: u1*cos(beta), u1 * sin(beta)

	movhpd xmm0, u1			; Zaladowanie u1 do starszych 64 bitow rejestru XMM0
	movlpd xmm0, u1			; Zaladowanie u1 do mlodszych 64 bitow rejestru XMM0
	movhpd xmm1, tempTryg	; Zaladowanie cos(beta) do starszych 64 bitow rejestru XMM0
	fld beta				; [ST = 1]
	fsin
	fstp tempTryg			; Zaladowanie do tempTryg wartosci sin(beta)	[ST = 0]
	movlpd xmm1, tempTryg	; Zaladowanie sin do mlodszych 64 bitow rejestru XMM0
	mulpd xmm0, xmm1		; Wynik w xmm0. Na mlodszych 64 bitach: u1*sin(beta); na starszych: u1*cos(beta)

	;----------------------- Obliczenie wartosci wyrazenia: f[0] + u1*cos(beta) - u2

	movhpd tempReal8, xmm0
	fld tempReal8			; Zaladowanie u1*cos(beta) do ST(0)					[ST = 1]
	fld real8 ptr [edx]		; Zaladowanie f[0]									[ST = 2]
	fadd					; Obliczenie wyrazenia: f[0] + u1*cos(beta)			[ST = 1]
	fld u2					; Zaladowanie u2									[ST = 2]
	fsubp st(1), st(0)		; Obliczenie wyrazenia: f[0] + u1*cos(beta) - u2	[ST = 1]
	fstp tempReal8			; Wynik w tempReal8									[ST = 0]
	
	;----------------------- Obliczenie: 2.0/(n+1)

	movhpd xmm0, tempReal8	; Zaladowanie tempReal8 do starszych 64 bitow rejestru XMM0, w mlodszych nadal znajduje sie u1*sin(beta)
	fld DWA					; [ST = 1]
	mov eax, n
	add eax, 1
	mov j, eax
	fidiv j					; [ST = 1]
	fstp tempReal8			; Wynik w zmiennej tempReal8	[ST = 0]

	;----------------------- Obliczenie wartosci wyrazen koncowych

	movhpd xmm1, tempReal8
	movlpd xmm1, tempReal8
	mulpd xmm1, xmm0		; Wartosc dla bb[j] w mlodszych 64 bitach, natomiast w starszych dla aa[j]

	;----------------------- Przypisanie wartosci do zmiennych aa[j] praz bb[j]

	mov ecx, tempZew		; Przywrocenie wartosci kroku petli zewnetrznej
	mov eax, aa
	movhpd real8 ptr [eax][ecx*8], xmm1	; Przypisanie do aa[j]
	mov eax, bb
	movlpd real8 ptr [eax][ecx*8], xmm1	; Przypisanie do bb[j]

	;------------------- Warunkowanie skoku petli for (j = 0; j <= m+1; j++)

	mov eax, endVal
	mov j, eax				; Zmienna j ma wartosc konca kroku petlu
	inc ecx
	cmp ecx, j
	jle petlaZew			; Skok jesli ecx <= m+1

	;-------------------

	ret
FactorsGeneration endp

PolynomialValue proc uses eax ecx ebx edx, t:REAL8, m:DWORD, ddelta:DWORD, aa:ptr REAL8, bb:ptr REAL8

	LOCAL pp: REAL8
	LOCAL tt: REAL8
	LOCAL j: DWORD
	LOCAL temp: REAL8
	LOCAL temp2: REAL8 ; j bedzie jako ecx

	;------------------- Inicjalizacja zmiennych i rejestrow

	finit
	mov ecx, 1			; Ustawienie poczatkowej wartosci kroku petli
	mov ebx, aa			; EBX przechowuje aa	
	mov edx, bb			; EDX przechowuje bb
	fldz				; Zaladowanie stalej 0 do ST(0)
	fst pp				; Ustawienie zmiennej pp na wartosc 0
	fst tt				; Ustawienie zmiennej tt na wartosc 0

	;-------------------
	
petla:
	movhpd xmm1, real8 ptr [ebx][ecx*8] ; aa[j]	; Zaladowanie elementu aa[j] do starszych 64 bitow rejestru XMM1
	movlpd xmm1, real8 ptr [edx][ecx*8] ; bb[j] ; Zaladowanie elementu bb[j] do mlodszych 64 bitow rejestru XMM1

	;------------------- Obliczanie wartosci:  sin(j*t), cos(j*t)

	mov j, ecx
	fld t				; Zaladowanie zmiennej t do rejestru ST[0]
	fimul j				; Pomnozenie zmiennej t przez zmienna j
	fsincos				; Obliczenie sinus i cosinus z wyniku mnozenia. Wynik funkcji cosinus znajduje sie w ST(0), natomias sinus w ST(1)
	fstp temp			; Zaladowanie do zmiennej temp wyniku operacji cosinus(j*t)
	fstp temp2			; Zaladowanie do zmiennej temp2 wyniku operacji sinus(j*t)

	;------------------- Wartosci przechowane w: temp = cos(j*t) oraz temp2 = sin(j*t)

	movhpd xmm0, temp	; Zaladowanie cos(j*t) do starszych 64 bitow rejestru XMM0
	movlpd xmm0, temp2	; Zaladowanie sin(j*t) do starszych 64 bitow rejestru XMM0
	mulpd xmm1, xmm0	; Wykonanie operacji mnozenia. Wynik znajduje sie w rejestrze XMM1
	movhpd temp, xmm1	; Zaladowanie wyniku aa[j]*cos(j*t) ze starszych 64 bitow rejestru XMM1 do zmiennej temp
	movlpd temp2, xmm1	; Zaladowanie wyniku bb[j]*cos(j*t) z mlodszych 64 bitow rejestru XMM1 do zmiennej temp2

	fld temp			
	fld temp2
	fadd				; Dodanie wynikow z poprzednich operacji
	fst temp			; Zaladowanie wyniku dodawania do zmiennej temp
	fld pp				; Zaladowanie zmiennej pp
	fadd				; Dodanie do pp wyniku
	fstp pp				; Zaladowanie wyniku dodawania do zmiennej pp

	inc ecx				; Zwiekszenie kroku
	cmp ecx, m			; Porownanie kroku z wartoscia zmiennej m
	jle petla			; Skok, gdy krok jest wiekszy od zmiennej m

	;------------------- Obliczanie wartosci wyrazenia:  0.5 * aa[0] + ddelta/2.0 * aa[m+1] * cos((m+1)*test2)
	
	fld POLOWA			; Zaladowanie stalej 0.5 do ST(0)
	fld real8 ptr [ebx]	; Zaladowanie aa[0] do ST(0)
	fmul				; Wykonanie mnozenia
	fstp temp			; Zaladowanie wyniku do zmiennej temp

	mov j, 2			
	fld ddelta
	fidiv j				; Podzielenie zmiennej ddelta przez liczbe calkowita 2

	mov ecx, m			; Zaladowanie do ecx wartosci zmiennej calkowitej m
	inc ecx				; Ustawienie ecx na wartosc m+1
	fld real8 ptr [ebx][ecx]
	fmul				; Wykonanie operacji: ddelta/2.0 * aa[m+1]
	fstp temp2			; Zaladowanie wynioku do zmiennej temp2

	mov eax, m
	mov j, eax
	add j, 1
	fld t
	fimul j				
	fcos				; Obliczenie wyniku wyrazenia: cos((m+1)*t). Wynik w ST(0)
	
	fld temp2			; Zaladowanie do ST(0) wyniku wyrazenia: ddelta/2.0 * aa[m+1]
	fmul				; Wykonanie operacji: ddelta/2.0 * aa[m+1] * cos((m+1)*t). Wynik w ST(0)

	fld temp			; Zaladowanie do ST(0) wyniku wyrazenia: 0.5 * aa[0]
	fadd				; Obliczenie wartosci calego wyrazenia

	;------------------- Wynik wyrazenia znajduje sie w rejestrze ST(0)

	fld pp				; Zaladowanie pp do ST(0)
	fadd				; Dodanie pp do poprzedniego wyniku
	ret					; Zwrocenie wyniku jako 64 bitowej liczby zmiennoprzecinkowej przechowywanej w rejestrze ST(0)
PolynomialValue endp

end