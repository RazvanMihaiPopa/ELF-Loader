# ELF-Loader
Homework assignment for the Operating Systems Course. Basically, an ELF Loader implemented as a dynamic library for Linux.

# Implementation
Functia segv_handler este un handler de tratare a semnalului SIGSEGV.
Verificam daca pagina se gaseste intr-un segment sau nu; daca nu se gaseste,
sau campul info->si_code este setat cu valoarea SEGV_ACCERR (permisiuni
invalide), rulam handlerul default, folosind signal. Altfel, daca campul
info->si_code sugereaza ca adresa nu este mapata, aflam offset-ul la care
va trebui sa mapam.

Pentru mapare, avem 3 cazuri. Daca intreaga pagina mapata depaseste file_size-ul,
se mapeaza in zona respectiva folosind MAP_ANONYMOUS(zona va fi initializata 
direct cu 0), ultimele 2 campuri fiind automat ignorate. Daca nu depaseste 
file_size-ul, se mapeaza setand file descriptorul si campul offset, de la 
care se va citi din fisier. Daca ne aflam intr-o pagina care este "intermediara"
(adresa de start este inainte de file_size, insa pagina depaseste file_size),
se mapeaza la fel ca in cazul anterior, insa va trebui sa initializam cu 0 
zona de dupa file_size. La sfarsit se foloseste mprotect pentru schimbarea 
permisiunilor potrivite segmentului respectiv.
