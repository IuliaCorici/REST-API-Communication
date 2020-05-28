# REST-API-Communication

In primul rand, am inceput prin a mi reaminti ce s-a facut mai exact in
cadrul laboratorului 10, intelegand mai bine conceptele, ulterior preluand
anumite fisiere cu functii de acolo pe care le-am modificat conform cerinte-
lor temei.

MOTIVATIE UTILIZARE bibliotca de parsare JSON : PARSON.C:
    -> initial incercasem sa creez stringul ce simuleaza JSON-ul manual =>
        mare FAIL
    -> apoi am gasit un exemplu de utilizare a bibliotecii PARSON pentru a
       crea un obiect cu anumite campuri si apoi sa-l transforme in char*.
    -> prin crearea cu anumite campuri fac referire la functii prin care se
       seteaza obiectul de tip JSON si prin transformare in char* o functie
       care il "printeaza" intr-un char*
    -> desi stiu ca este pentru C, mi s-a parut ca a facut ce aveam nevoie
       si in C++, neavand vreo dificultate, cu mentiunea ca am modificat
       extensia fisierelor.

Am ales sa implementez tema in C++ datorita utilizarii STL-ului, asadar am
modificat fisierele mentionate mai sus: buffer.c, requests.c, helpers.c,
parson.c.

In ceea ce priveste implementarea temei, am inceput prin a ma asigura ca
cookie-ul si token-ul sunt goale, si am pornit conexiunea.
intr-un loop infinit, se citesc toate comenzile cu ajutorul lui 
std::getline(std::cin, string) pentru a parsa corect comenzile si a permite
utilizatorului sa adauge spre exemplu mai multe cuvinte pentru titlu sau autor.

COMANDA LOGIN SI REGISTER
    -> se citesc username-ul si password-ul, se valideaza acestea, asigurandu-ma
       ca sunt formate dintr-un singur cuvant
    -> se creeaza prompt-ul necesar
    -> se formeaza JSON-ul si apoi se formateza intr-un string
    -> se trimite la server mesajul pentru POST requests
    -> daca nu exista raspuns, se repeta procedura de mai sus
    -> se verifica daca au existat erori sau a fost totul ok
    -> pentru login, retin cookie-ul obtinut din raspunsul de la server

COMANDA ENTER_LIBRARY
    -> se trimite la server mesajul pentru GET requests cu cookie-ul anterior
    -> daca nu exista raspuns, se repeta procedura de mai sus
    -> se verifica daca au existat erori sau a fost totul ok
    -> se extrage token-ul din raspuns in caz de succes parsand raspunsul

COMANDA GET_BOOKS SI GET_BOOKS
    -> se repeta procedura de trimitere mesajul pentru cererea de tip GET
    -> se afiseaza content-ul dorit sin raspunsul efectuat cu succes, parsand
       de asemenea partea cu JSON de care avem nevoie
    -> in caz de eroare se afiseaza eroarea
    -> se valideaza id-ul pentru GET_BOOK incat sa nu fie string sau numar negativ

COMANDA ADD_BOOK
    -> se valideaza datele de input incat page_count sa fie numar pozitiv
    -> se formeaza JSON-ul, setand campurile citite si apoi se formateza intr-un string
    -> se repeta procedura de trimitere mesajul pentru cererea de tip POST
    -> se verifica daca a fost succes sau eroare

COMANDA DELETE_BOOK
    -> foarte asemanatoare cu GET_BOOK, dar cererea e de tip DELETE_BOOK

COMANDA LOGOUT
    -> se repeta procedura de trimitere mesajul pentru cererea de tip GET
    -> se verifica daca a fost succes sau eroare
    -> se golesc token-ul si cookie-ul

INSTRUCTIUNI Makefile:
    -> make client => pentru compilare
    -> make run    => pentru rulare
    -> make clean

