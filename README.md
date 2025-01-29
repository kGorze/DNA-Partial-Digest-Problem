# Kompleksowy Raport Techniczny: Implementacja Problemu Częściowego Trawienia (PDP)

## Spis treści
1. [Streszczenie](#streszczenie)  
2. [Podstawy Teoretyczne](#podstawy-teoretyczne)  
   2.1 [Wprowadzenie do Problemu Częściowego Trawienia (PDP)](#wprowadzenie-do-problemu-częściowego-trawienia-pdp)  
   2.2 [Podstawy Matematyczne](#podstawy-matematyczne)  
       - 2.2.1 [Multizbiory i Obliczenia Odległości](#multizbiory-i-obliczenia-odległości)  
       - 2.2.2 [Formalne Sformułowanie Problemu](#formalne-sformułowanie-problemu)  
   2.3 [Zastosowania i Wyzwania](#zastosowania-i-wyzwania)  
3. [Analiza Algorytmów](#analiza-algorytmów)  
   3.1 [Podstawowy Solver Mapowania (BMP)](#podstawowy-solver-mapowania-bmp)  
   3.2 [Algorytm BBb](#algorytm-bbb)  
   3.3 [Algorytm BBb2](#algorytm-bbb2)  
4. [Komponenty Implementacyjne](#komponenty-implementacyjne)  
   4.1 [Podstawowa Struktura Projektu](#podstawowa-struktura-projektu)  
   4.2 [Implementacje Algorytmów](#implementacje-algorytmów)  
   4.3 [Wyzwania Implementacyjne i Rozwiązania](#wyzwania-implementacyjne-i-rozwiązania)  
   4.4 [Zarządzanie Systemem Plików i Katalogami](#zarządzanie-systemem-plików-i-katalogami)  
   4.5 [Framework Testowania i Debugowania](#framework-testowania-i-debugowania)  
   4.6 [Aspekty Wydajnościowe](#aspekty-wydajnościowe)  
5. [Metodologia Testowania](#metodologia-testowania)  
   5.1 [Dane Testowe i Kategorie](#dane-testowe-i-kategorie)  
   5.2 [Metody Generowania Instancji](#metody-generowania-instancji)  
   5.3 [Metryki Wydajności i Pomiary](#metryki-wydajności-i-pomiary)  
   5.4 [Implementacja Frameworka Testowego](#implementacja-frameworka-testowego)  
   5.5 [Walidacja Rozwiązań](#walidacja-rozwiązań)  
   5.6 [Instancje Dostarczone w Zadaniu](#instancje-dostarczone-w-zadaniu)  
   5.7 [Wyniki Programu dla Niepoprawnych Danych Wejściowych](#wyniki-programu-dla-niepoprawnych-danych-wejściowych)  
   5.8 [Wpływ Różnych Uporządkowań Danych Wejściowych na Wydajność Algorytmu](#wpływ-różnych-uporządkowań-danych-wejściowych-na-wydajność-algorytmu)  
6. [Wyniki i Analiza](#wyniki-i-analiza)  
   6.1 [Analiza Złożoności Czasowej](#analiza-złożoności-czasowej)  
   6.2 [Rozszerzona Analiza Porównawcza](#rozszerzona-analiza-porównawcza)  
   6.3 [Charakterystyka Użycia Pamięci](#charakterystyka-użycia-pamięci)  
   6.4 [Przypadki Brzegowe i Ograniczenia](#przypadki-brzegowe-i-ograniczenia)  
   6.5 [Podsumowanie Wyników](#podsumowanie-wyników)  
7. [Wizualizacje](#wizualizacje)  
8. [Optymalizacja i Udoskonalenia Implementacji](#optymalizacja-i-udoskonalenia-implementacji)  
   8.1 [Optymalizacja Kodu](#optymalizacja-kodu)  
   8.2 [Usprawnienia Bezpieczeństwa Pamięci](#usprawnienia-bezpieczeństwa-pamięci)  
   8.3 [Optymalizacje Wydajnościowe](#optymalizacje-wydajnościowe)  
9. [Bibliografia](#bibliografia)  

---

## Streszczenie
> **[To be completed when full content is available]**

---

## Podstawy Teoretyczne

### Wprowadzenie do Problemu Częściowego Trawienia (PDP)
Problem Częściowego Trawienia (ang. _Partial Digest Problem_, PDP) stanowi klasyczne wyzwanie obliczeniowe wywodzące się z mapowania restrykcyjnego DNA, w szczególności w scenariuszach, gdzie pojedynczy enzym restrykcyjny wykorzystywany jest do cięcia kopii cząsteczki DNA w różnych czasach reakcji, prowadząc do powstania różnorodnych częściowo strawionych fragmentów. Rezultatem tych reakcji jest multizbiór długości fragmentów, na podstawie których dąży się do rekonstrukcji pozycji miejsc restrykcyjnych wzdłuż cząsteczki DNA. Zadanie 3 na labolatoria do prof. Kasprzak - Bioinformatyka 3 semestr, Algorytmy kombinatoryczne w bioinformatyce.

Enzymy restrykcyjne to wyspecjalizowane białka rozpoznające i przecinające DNA w określonych krótkich sekwencjach (miejscach restrykcyjnych)[1,2]. W eksperymencie częściowego trawienia enzym nie zawsze przecina wszystkie możliwe miejsca w pojedynczej reakcji, ponieważ reakcja jest zatrzymywana w różnych momentach. W konsekwencji powstają liczne mniejsze fragmenty w różnych reakcjach, wspólnie tworząc multizbiór odległości. PDP polega na określeniu kolejności i pozycji miejsc restrykcyjnych wzdłuż DNA na podstawie tych zmierzonych odległości.

### Podstawy Matematyczne

#### Multizbiory i Obliczenia Odległości
Niech \\(X = \{x_0, x_1, x_2, \ldots, x_{n-1}\}\\) będzie zbiorem pozycji miejsc restrykcyjnych na liniowym fragmencie DNA. Bez utraty ogólności przyjmujemy:  
\\(x_0 = 0\\) oraz \\(x_{n-1} = L\\),  
gdzie \\(L\\) oznacza całkowitą długość fragmentu DNA (często określaną jako _szerokość_ w wielu implementacjach). Multizbiór odległości \\(\Delta X\\) jest wówczas zdefiniowany jako:  
\\[
\Delta X = \{|x_j - x_i| : 0 \le i < j \le n-1\}.
\\]
Ponieważ proces częściowego trawienia generuje wszystkie podfragmenty (i powtarza niektóre z nich) ze wszystkich możliwych częściowych cięć, obserwowany multizbiór wejściowy \\(D\\) powinien odpowiadać \\(\Delta X\\), ale z potencjalnie powtórzonymi wartościami, jeśli określone długości fragmentów występują wielokrotnie.

#### Formalne Sformułowanie Problemu
Mając dany multizbiór wejściowy \\(D\\) o rozmiarze:  
\\[
N = \frac{n(n-1)}{2},
\\]
należy znaleźć zbiór \\(X = \{x_0, x_1, \ldots, x_{n-1}\}\\) o rozmiarze \\(n\\) taki, że:  
\\[
\Delta X = D, \quad x_0 = 0, \quad x_{n-1} = L.
\\]
Tutaj \\(L\\) jest największą odległością w \\(D\\) (przy założeniu, że dane są poprawne). Jeśli nie można znaleźć takiego \\(X\\), instancję uznaje się za niespójną lub błędną.

### Zastosowania i Wyzwania
PDP został pierwotnie zainspirowany mapowaniem fizycznym genomów, gdzie identyfikacja lokalizacji miejsc restrykcyjnych pomaga w analizie DNA, klonowaniu genów i genomice porównawczej[3,4]. Dokładne rozwiązanie PDP jest często NP-trudne w pewnych wariantach z szumem[5], co wymaga podejść heurystycznych, aproksymacyjnych lub typu _branch-and-bound_. Podczas gdy istnieją heurystyki wielomianowe, nie gwarantują one dokładności[6,7]. Algorytmy dokładne – często oparte na backtrackingu lub metodzie _branch-and-bound_ – mogą niezawodnie rozwiązywać instancje PDP, ale z potencjalnie wykładniczą złożonością czasową w najgorszym przypadku[8,9].

> **[Figure 1]** Wizualizacja procesu częściowego trawienia DNA – schemat przedstawiający proces częściowego trawienia DNA, pokazujący kolejne etapy cięcia enzymatycznego i powstające fragmenty.

---

## Analiza Algorytmów

W ramach projektu zaimplementowano i porównano trzy główne algorytmy:

- **BMP** (*Basic Map Solver* – Podstawowy Solver Mapowania)  
- **BBb** (*Branch-and-Bound* oparty na BFS)  
- **BBb2** (Dwufazowy *Branch-and-Bound*)  

Nazewnictwo jest zgodne z konwencją przyjętą w literaturze omawiającej podejścia oparte na przeszukiwaniu wszerz (BBb) i ulepszone metody rozgałęziania (BBb2)[10,11].

### Podstawowy Solver Mapowania (BMP)

#### Podejście Implementacyjne
- Metoda wykorzystuje proste przeszukiwanie z nawrotami (przeszukiwanie w głąb), które próbuje umieszczać każde potencjalne miejsce cięcia w kolejności rosnącej wzdłuż DNA i weryfikuje spójność z wejściowym multizbiorem \\(D\\).
- Inicjalizacja obejmuje wyodrębnienie największej odległości \\(L\\) oraz ustalenie pozycji \\(x_0 = 0\\) i \\(x_{n-1} = L\\).
- Przeszukiwanie rekurencyjne realizowane jest przez funkcję `szukaj(ind)`, która próbuje przypisać \\(ind\\)-te miejsce poprzez iterację po zakresie możliwych pozycji.

#### Techniki Optymalizacyjne
- **Sprawdzanie odległości częściowych**: na każdym kroku nowo wprowadzone odległości są weryfikowane względem pozostałego multizbioru \\(D\\).  
- **Wczesne zakończenie**: po znalezieniu poprawnego rozwiązania rekurencja zostaje przerwana.

#### Zarządzanie Pamięcią
- Algorytm przechowuje bieżącą mapę (`currentMap`) oraz licznik odległości do śledzenia wykorzystanych odległości.  
- Relatywnie niskie zużycie pamięci dla małych \\(n\\), jednak może stać się znaczące i powolne dla \\(n > 13\\).

#### Ograniczenia i Wyzwania
- Wykładniczy wzrost złożoności: dla \\(n > 14\\) czas wykonania drastycznie rośnie.  
- Ograniczone wewnętrzne przycinanie: brak zaawansowanych mechanizmów ograniczających do pomijania dużych nieperspektywicznych gałęzi.

#### Ulepszenia Względem Wersji "Naiwnych"
- Zapewnia natychmiastowe wykrywanie niemożliwych rozwiązań częściowych.  
- Sortuje odległości dla szybkiego dostępu do największej odległości.

---

### Algorytm BBb

Algorytm **BBb** (*Branch-and-Bound oparty na BFS*) wykorzystuje strategię przeszukiwania wszerz, inspirowaną wcześniejszymi pracami nad problemami częściowego trawienia, które wykazały, że BFS może unikać pewnych zduplikowanych poddrzew[10].

#### Podejście Implementacyjne
Wykorzystywane są dwa warunki ograniczające (znane jako ograniczenia Skieny[9]):
1. **Sprawdzenie maksymalnej odległości**: największa odległość w pozostałym zbiorze musi odpowiadać albo nowemu miejscu, albo odległości komplementarnej \\(L - \text{(that site)}\\).  
2. **Eliminacja duplikatów**: podproblemy z tym samym częściowym zbiorem \\(X\\) są rozpoznawane i nie są rozwijane ponownie.

- **Ekspansja wszerz**: każdy poziom rozwija wszystkie podproblemy z bieżącego poziomu poprzez próby:  
  - \\(y = \max(D)\\)  
  - \\(L - y\\)  

  Jeśli nowe miejsce jest poprawne, tworzona jest nowa gałąź w drzewie BFS.

> **[Figure 2]** Schemat działania algorytmu BBb – diagram ilustrujący proces ekspansji drzewa przeszukiwania w algorytmie BBb, pokazujący węzły, ograniczenia i proces eliminacji duplikatów.

#### Techniki Optymalizacyjne
- **Deduplikacja podproblemów**: utrzymuje zbiór unikalnych par \\((D, X)\\) w celu pomijania powtarzających się stanów.  
- **Przycinanie**: jeśli nowo dodane miejsce nie może wyjaśnić odległości w \\(D\\), ta gałąź jest natychmiast porzucana.

#### Strategie Zarządzania Pamięcią
- Potencjalnie duża kolejka węzłów BFS, ale powtarzające się stany są przycinane.  
- Typowo przewyższa wydajnością **BasicMapSolver** dla umiarkowanych \\(n\\), ponieważ BFS łączy wiele identycznych podproblemów na wczesnym etapie.

#### Ograniczenia i Wyzwania
- Wciąż wykładniczy w najgorszym przypadku[9,10], ale bardziej efektywny niż **BasicMapSolver** dla przypadków przeciętnych.  
- Zużycie pamięci może stać się duże dla ekspansji BFS z powtarzającymi się stanami częściowymi.

---

### Algorytm BBb2

**BBb2** rozszerza **BBb**, łącząc dwufazowe BFS i lokalne ekspansje BBb[10,11]:

1. **Faza 1 ("Górne" BFS)**: buduje drzewo rozwiązań tylko do pewnego poziomu \\(\alpha\\), przechowując rozwiązania częściowe.  
2. **Faza 2 ("Lokalne" BBb)**: każde rozwiązanie częściowe na poziomie \\(\alpha\\) jest uzupełniane przy użyciu standardowego podejścia BBb lub DFS.

> **[Figure 3]** Dwufazowe podejście w BBb2 – schemat ilustrujący dwie fazy algorytmu BBb2, pokazujący przejście między fazą BFS a fazą lokalną.

#### Podejście Implementacyjne
- **Znalezienie \\(\alpha\\)**: formuła (lub podejście empiryczne) określa optymalną głębokość BFS \\(\alpha\\) w celu minimalizacji całkowitego zużycia pamięci[11].  
- **Kolejkowanie do poziomu \\(\alpha\\)**: wykorzystanie BFS do systematycznego generowania stanów częściowych \\((D_i^k, X_i^k)\\) dla \\(k \le \alpha\\).  
- **Przejście do przeszukiwania w głąb**: dla każdego węzła na poziomie \\(\alpha\\) wykorzystywany jest lokalny solver _branch-and-bound_ (podobny do BBb) do ukończenia rozwiązania.

#### Techniki Optymalizacyjne
- **Zredukowany zakres BFS**: unika eksplozji BFS poza poziomem \\(\alpha\\).  
- **Selektywne rozgałęzianie**: tylko najbardziej obiecujące rozwiązania częściowe na poziomie \\(\alpha\\) są dalej rozwijane.

#### Zarządzanie Pamięcią
- Hybrydowa koncepcja BFS-DFS kontroluje maksymalny rozmiar kolejki.  
- Znacząco przewyższa czysty BFS pod względem wykorzystania pamięci, gdy \\(n\\) rośnie[11].

#### Ograniczenia
- Wymaga starannego dostrojenia \\(\alpha\\). Jeśli \\(\alpha\\) jest zbyt małe, przycinanie BFS jest niewystarczające. Jeśli \\(\alpha\\) jest zbyt duże, część BFS staje się zbyt obszerna.

#### Ulepszenia Względem Poprzednich Wersji
- Typowo najszybszy spośród trzech algorytmów dla większych \\(n\\) zarówno pod względem czasu, jak i pamięci[10,11].  
- Osiąga do 75% poprawy w czasie wykonania w najgorszym przypadku w porównaniu do klasycznych podejść _branch-and-bound_[11].

> **[Figure 4]** Porównanie wydajności algorytmów – wykres porównujący czasy wykonania i zużycie pamięci dla trzech zaimplementowanych algorytmów w funkcji rozmiaru problemu \\(n\\).

---

## Komponenty Implementacyjne

### Podstawowa Struktura Projektu

#### Komponenty Główne
Implementacja składa się z kilku kluczowych komponentów zorganizowanych w modularne pliki źródłowe:

- **benchmark.h/cpp**: Implementuje standardowe środowisko testowania wydajności do pomiaru czasu wykonania i zużycia pamięci dla różnych algorytmów  
- **instance_generator.h/cpp**: Obsługuje tworzenie i zarządzanie instancjami testowymi PDP, w tym scenariusze losowe i przypadki szczególne  
- **map_solver.h/cpp**: Zawiera implementację **BasicMapSolver** wykorzystującą podejście z nawrotami  
- **bbd_algorithm.h/cpp, bbb_algorithm.h/cpp, bbb2_algorithm.h/cpp**: Implementują zoptymalizowane algorytmy _Branch-and-Bound_ z różnymi podejściami do eksploracji przestrzeni poszukiwań  

#### Framework Wspierający
- **test_framework.h/cpp**: Zapewnia interaktywne środowisko testowe do generowania instancji, walidacji rozwiązań i wykonywania testów wydajnościowych  
- **global_paths.h/cpp**: Zarządza operacjami na systemie plików i strukturą katalogów do przechowywania instancji, testów wydajnościowych i wyników  

> **[Figure 5]** Struktura projektu – diagram przedstawiający organizację komponentów projektu i ich wzajemne zależności.

---

### Implementacje Algorytmów

#### BasicMapSolver
Implementacja **BasicMapSolver** zawiera następujące kluczowe komponenty:
- Walidacja odległości przy użyciu mapy `remainingDistances` do śledzenia dostępnych odległości  
- Weryfikacja wykorzystania odległości w czasie rzeczywistym podczas konstrukcji rozwiązania  
- Prawidłowe wykonywanie nawrotów z inicjalizacją pozycji `(-1)`  
- Zoptymalizowane obliczenia granic dla `startVal` i `endVal`  
- Wczesne zakończenie po znalezieniu rozwiązania  

#### Algorytm BBb
Implementacja **BBb** (*Branch-and-Bound* z przeszukiwaniem wszerz) obejmuje:
- Systematyczne generowanie punktów przy użyciu pozostałych odległości  
- Solidną walidację kandydatów poprzez metodę `isValidCandidate`  
- Efektywną obsługę zduplikowanych odległości za pomocą klasy `MultisetD`  
- Kompleksową eksplorację przestrzeni rozwiązań z zachowaniem efektywności BFS  
- Odpowiednie sortowanie zbiorów \\(X\\) dla spójnego porównywania  

#### Algorytm BBb2
Dwufazowa implementacja **BBb2** zawiera:
- Obliczanie optymalnego poziomu \\(\alpha\\) dla zarządzania pamięcią  
- Wydajne łączenie rozwiązań częściowych  
- Ulepszone sprawdzanie unikalności z wykorzystaniem posortowanych wektorów  
- Wczesne sprawdzenia walidacyjne dla przycinania  
- Optymalizację pamięci poprzez semantykę przenoszenia  

> **[Figure 6]** Porównanie implementacji algorytmów – diagram porównujący kluczowe cechy implementacji trzech algorytmów.

---

### Wyzwania Implementacyjne i Rozwiązania

#### Zarządzanie Odległościami
Krytyczne wyzwanie dotyczyło właściwej obsługi multizbiorów odległości:
- Implementacja wydajnych struktur danych do śledzenia pozostałych odległości  
- Walidacja częstotliwości wykorzystania odległości  
- Zarządzanie zduplikowanymi odległościami w zbiorze wejściowym  
- Weryfikacja rozwiązań częściowych w czasie rzeczywistym  

#### Optymalizacja Pamięci
Zastosowano kilka technik efektywnego zarządzania pamięcią:
- Wykorzystanie semantyki przenoszenia dla dużych struktur danych  
- Minimalizacja operacji kopiowania wektorów  
- Wydajne przechowywanie unikalnych stanów w BBb i BBb2  
- Właściwe czyszczenie tymczasowych struktur danych  

#### Walidacja Algorytmów
Zaimplementowano solidne mechanizmy walidacji:
- Weryfikacja spójności odległości wejściowych  
- Sprawdzanie poprawności wykorzystania odległości w rozwiązaniach częściowych  
- Walidacja rozwiązań końcowych względem oryginalnego multizbioru odległości  
- Wykrywanie i obsługa nieprawidłowych przypadków wejściowych  

---

### Zarządzanie Systemem Plików i Katalogami

#### Struktura Katalogów
Implementacja utrzymuje spójną hierarchię katalogów:
- `instances/`: Przechowywanie instancji testowych PDP  
- `benchmark/`: Lokalizacja wyników testów wydajnościowych i analiz  
- `temp/`: Przechowywanie plików tymczasowych  
- `debug/`: Logi debugowania i informacje diagnostyczne  

#### Operacje na Plikach
Standardowe operacje na plikach obejmują:
- Spójną obsługę ścieżek w różnych systemach operacyjnych  
- Właściwe sprawdzanie istnienia i tworzenie plików  
- Obsługę błędów dla operacji plikowych  
- Czyszczenie plików tymczasowych  

---

### Framework Testowania i Debugowania

#### Narzędzia Debugowania
- **DebugMapSolver**: Rozszerzona wersja solvera z szczegółowym logowaniem  
  - Śledzenie rozwiązań krok po kroku  
  - Rejestrowanie historii unieważnień  
  - Możliwości profilowania wydajności  

> **[Figure 7]** Framework testowania – diagram ilustrujący architekturę frameworka testowego i jego główne komponenty.

#### Framework Walidacji
- Kompleksowa walidacja danych wejściowych  
- Weryfikacja rozwiązań względem oryginalnego multizbioru odległości  
- Zbieranie metryk wydajności  
- Walidacja krzyżowa między różnymi implementacjami algorytmów  

---

### Aspekty Wydajnościowe

#### Techniki Optymalizacji
- Wczesne przerywanie nieprawidłowych ścieżek rozwiązań  
- Efektywny dobór struktur danych dla częstych operacji  
- Optymalizacja zarządzania pamięcią  
- Usprawnienia wydajności specyficzne dla algorytmów  

#### Ulepszenia Skalowalności
- Właściwa obsługa dużych rozmiarów instancji  
- Optymalizacja wykorzystania pamięci dla rozległych przeszukiwań  
- Wydajna obsługa stanów zduplikowanych  
- Możliwości logowania i analizy wydajności  

> **[Figure 8]** Wykresy wydajności – zestaw wykresów przedstawiających metryki wydajności dla różnych aspektów implementacji.

---

## Metodologia Testowania

### Dane Testowe i Kategorie

W badaniach wykorzystano dwie główne kategorie instancji testowych:

1. **Dane Losowe (RD)**:
   - Równomiernie rozłożone pozycje miejsc cięcia do maksymalnej wartości \\(M\\)  
   - Zmienne \\(n\\) od małych (5, 8, 11) do umiarkowanie dużych (14 i więcej)  
   - Przetasowany porządek odległości dla uniknięcia podpowiedzi dla solverów[1,9]  

2. **Dane Specjalne/Ekstremalne (typu ZD)**:
   - Zainspirowane przykładami najgorszych przypadków z pracy Zhanga[9,10]  
   - Starannie skonstruowane, aby wywoływać wykładnicze rozgałęzienie w naiwnych podejściach DFS  

### Metody Generowania Instancji
`InstanceGenerator::generateInstance`:
- Wybiera pozycje miejsc losowo lub systematycznie  
- Oblicza multizbiór \\(\Delta X\\)  
- Opcjonalnie tasuje odległości dla losowej kolejności wejściowej  
- Zapisuje dane do pliku z rozszerzeniem `.txt`  

### Metryki Wydajności i Pomiary

1. **Czas Wykonania (ms)**:
   - Mierzony za pomocą zegarów wysokiej rozdzielczości C++11  
   - Agregowany z wielu przebiegów (5-100 powtórzeń w zależności od złożoności instancji)  

2. **Zużycie Pamięci (MB)**:
   - Obserwowane poprzez odczyt pamięci procesu systemowego (np. Linux `/proc` lub `top`)  
   - Logowane raz na przebieg dla oszacowania szczytowego zużycia  

> **[Figure 9]** Metodologia pomiarów – diagram ilustrujący proces zbierania i analizy metryk wydajnościowych.

### Implementacja Frameworka Testowego
- **Tryb interaktywny** do testowania ręcznego  
- **Kompleksowy benchmark** obejmujący:  
  - Iterację po rozmiarach i typach testów  
  - Uruchamianie każdego algorytmu (**BasicMapSolver**, **BBb**, **BBb2**)  
  - Logowanie wyników w CSV z kolumnami: `(algorytm, typ_testu, rozmiar, czas_ms, sukces, opis)`  

### Walidacja Rozwiązań
Wszystkie algorytmy zawierają końcowe sprawdzenie zapewniające, że wygenerowana mapa odtwarza ten sam multizbiór \\(D\\). W przypadku wykrycia niezgodności rozwiązanie jest oznaczane jako nieprawidłowe.

### Instancje Dostarczone w Zadaniu

| Nazwa instancji       | Rozmiar multizboru | Rozwiązanie                                           | Czas BMP    | Czas BBb2 | Walidacja |
|-----------------------|--------------------|--------------------------------------------------------|-------------|-----------|-----------|
| instance11aasc.txt    | 78                 | 0 4 10 15 23 26 35 40 42 46 53 61 67                  | 5700ms      | 0ms       | PASSED    |
| instance11basc.txt    | 78                 | 0 38 112 139 205 247 262 351 398 433 446 458 512      | 22884ms     | 0ms       | PASSED    |
| instance11bdesc.txt   | 78                 | 0 38 112 139 205 247 262 351 398 433 446 458 512      | 23154ms     | 0ms       | PASSED    |
| instance12aasc.txt    | 91                 | 0 4 10 15 23 26 35 40 42 46 53 61 67 73               | 85478ms     | 0ms       | PASSED    |
| instance13aasc.txt    | 105                | 0 3 9 15 23 30 34 36 41 50 53 61 66 72 76             | 142428ms    | 0ms       | PASSED    |
| instance14aasc.txt    | 120                | 0 4 10 15 23 26 35 40 42 46 53 61 67 73 76 81         | 1955950ms   | 1ms       | PASSED    |
| instance14basc.txt    | 120                | 0 79 136 161 199 273 300 366 408 423 512 559 594 607 619 673 | timeout | 1ms       | PASSED    |
| instance14bdesc.txt   | 120                | 0 79 136 161 199 273 300 366 408 423 512 559 594 607 619 673 | timeout | 1ms       | PASSED    |

> **[Figure 10]** Porównanie czasów wykonania – wykres porównujący czasy wykonania algorytmów **BMP** i **BBb2** dla różnych instancji testowych.

### Wyniki Programu dla Niepoprawnych Danych Wejściowych
Podczas testowania programu z nieprawidłowymi danymi wejściowymi (ekstremalnie duże liczby jak `999999999` i `1113123123123`), algorytm **BBb2** poprawnie zidentyfikował brak możliwego rozwiązania:
- Algorytm zakończył wykonanie w 14ms  
- Prawidłowo zaraportował "Nie znaleziono rozwiązania"  
- Nie uległ awarii ani nie wygenerował nieprawidłowego wyjścia  
- Zachował stabilność mimo nieprawidłowych danych wejściowych  

### Wpływ Różnych Uporządkowań Danych Wejściowych na Wydajność Algorytmu
Analiza różnych układów danych została przeprowadzona przy użyciu pliku `sequential_40.txt` zawierającego 861 odległości, z 1500 powtórzeniami dla każdego typu układu.

- **Wyniki dla różnych układów**:  
  - Kolejność oryginalna: średnio 13.94ms (zakres: 13.00–56.00ms)  
  - Kolejność rosnąca: średnio 13.65ms (zakres: 13.00–81.00ms)  
  - Kolejność malejąca: średnio 14.03ms (zakres: 13.00–88.00ms)  
  - Kolejność losowa: średnio 13.82ms (zakres: 13.00–76.00ms)  
  - Kolejność zgrupowana: średnio 14.13ms (zakres: 13.00–71.00ms)

**Kluczowe wnioski**:
- Wszystkie układy osiągnęły 100% skuteczności (1500/1500 udanych rozwiązań).  
- Minimalna zmienność w średnich czasach wykonania (zakres: 13.65–14.13ms).  
- Kolejność rosnąca wykazała nieznacznie lepszą wydajność.  
- Maksymalne czasy wykonania wykazywały większą zmienność niż średnie.  
- Algorytm wykazał spójną stabilność we wszystkich układach.

> **[Figure 11]** Analiza wydajności dla różnych uporządkowań – wykres przedstawiający porównanie czasów wykonania dla różnych uporządkowań danych wejściowych.

**Istotność statystyczna**:
- Różnica w wydajności między układami jest minimalna (mniej niż 0.5ms).  
- Wszystkie typy układów zachowały spójną wydajność bazową (minimum 13.00ms).  
- Algorytm wykazuje odporność na uporządkowanie danych wejściowych.

---

## Wyniki i Analiza

### Analiza Złożoności Czasowej
Przeprowadziliśmy szczegółową analizę porównawczą trzech algorytmów: **BasicMapSolver** (algorytm bazowy), **BBb** (ograniczanie oparte na BFS) oraz **BBb2** (dwufazowe ograniczanie BFS). Wyniki przedstawiają się następująco:

> **[Rysunek 1]** Porównanie wydajności algorytmów dla \\(n \le 400\\) – wykres przedstawia zależność czasu wykonania od rozmiaru problemu w skali liniowej (lewy) i logarytmicznej (prawy).

- Dla instancji losowych o rozmiarze \\(n \le 200\\), wszystkie trzy algorytmy wykazują podobną charakterystykę czasową (poniżej 2000 ms).  
- Dla \\(n > 200\\) obserwujemy gwałtowny wzrost czasu wykonania, co potwierdza złożoność wykładniczą algorytmów.  
- **BBb2** (oznaczony kolorem zielonym) wykazuje nieznacznie wyższe czasy wykonania niż **BBb** (niebieski) dla największych instancji, choć różnice pozostają umiarkowane.

### Rozszerzona Analiza Porównawcza
> **[Rysunek 2]** Szczegółowe porównanie algorytmów dla \\(4 \le n \le 14\\) – wykres przedstawia dokładne porównanie czasów wykonania w skali liniowej (lewy) i logarytmicznej (prawy).

- W zakresie mniejszych instancji (\\(n < 10\\)) wszystkie algorytmy utrzymują czas wykonania poniżej 1000 ms.  
- **BasicMapSolver** osiąga próg 1.000.000 ms (ponad 1000 sekund) przy \\(n \approx 14\\), podczas gdy **BBb** i **BBb2** zachowują znacznie wyższą efektywność w tym zakresie.

### Charakterystyka Użycia Pamięci
Wszystkie implementowane algorytmy wykorzystują struktury danych do przechowywania rozwiązań częściowych. Algorytmy oparte na BFS (**BBb**, **BBb2**) wykazują następujące charakterystyki:
- **BBb** często zużywa mniej pamięci niż **BasicMapSolver** dla danych losowych dzięki wczesnemu łączeniu powtarzających się stanów.  
- **BBb2** zazwyczaj osiąga najlepsze wyniki dla dużych \\(n\\), gdyż ekspansja BFS jest ograniczona do głębokości \\(\alpha\\).

### Przypadki Brzegowe i Ograniczenia
Zidentyfikowaliśmy następujące kluczowe aspekty działania algorytmów:
- **Błędne odległości**: Wszystkie algorytmy prawidłowo wykrywają sytuacje, gdy największa odległość nie występuje w multizbiorze.  
- **Powtarzające się odległości**: Poprawnie obsługiwane przez wewnętrzne liczniki.  
- **Duże wartości n**: Dla \\(n > 20\\) może wystąpić wyczerpanie pamięci w najgorszych przypadkach.

### Podsumowanie Wyników
Analiza wykazała, że:
- **BasicMapSolver** staje się niepraktyczny dla \\(n > 13\\).  
- **BBb** oferuje solidną alternatywę opartą na BFS, efektywnie łączącą powtarzające się stany.  
- **BBb2** łączy zalety BFS i DFS, osiągając do 75% poprawy czasu wykonania w najgorszych przypadkach [1].

---

## Wizualizacje
W raporcie przedstawiono następujące kluczowe wizualizacje:
- **[Rysunek 3]** Drzewo rozwiązań dla przykładowej instancji – diagram przedstawiający proces eksploracji przestrzeni rozwiązań przez algorytm **BBb2**.  
- **[Rysunek 4]** Porównanie zużycia pamięci – wykres przedstawiający zużycie pamięci w funkcji rozmiaru problemu dla wszystkich trzech algorytmów.

---

## Optymalizacja i Udoskonalenia Implementacji

### Optymalizacja Kodu
Wprowadzono następujące udoskonalenia:
- Usunięto redundantne komentarze  
- Zachowano kluczową dokumentację dotyczącą:  
  - Opisów algorytmów i analizy złożoności  
  - Specyfikacji interfejsów klas i metod  
  - Kluczowych decyzji implementacyjnych  

### Usprawnienia Bezpieczeństwa Pamięci
Zaimplementowano:
- Poprawną obsługę *const correctness*  
- Inicjalizację wektorów z rezerwacją pamięci  
- Sprawdzanie zakresów dostępu do wektorów  

### Optymalizacje Wydajnościowe
Wprowadzono:
- Zapobieganie przepełnieniu liczb całkowitych  
- Semantykę przenoszenia dla dużych struktur danych  
- Optymalizacje kontenerów  

---

## Bibliografia
> _(Numery [1]–[9] odwołują się do uzupełnienia bibliografii o szczegółowe źródła naukowe. Przykładowe pozycje zaznaczone poniżej.)_

1. [Abbas MM, Bahig HM, "A fast exact sequential algorithm for the partial digest problem", BMC Bioinformatics, 17(Suppl 19): 510, 2016.](#)  
2. [Zhang K, "Efficient algorithms for analyzing partial digest experiment", J Comp Biol, 1994.](#)  
3. **[Do uzupełnienia – szczegóły dotyczące enzymów restrykcyjnych]**  
4. **[Do uzupełnienia – mechanizmy działania enzymów]**  
5. **[Do uzupełnienia – zastosowania w mapowaniu genomów]**  
6. **[Do uzupełnienia – genomika porównawcza]**  
7. **[Do uzupełnienia – analiza złożoności obliczeniowej]**  
8. **[Do uzupełnienia – metody heurystyczne]**  
9. **[Do uzupełnienia – algorytmy aproksymacyjne]**  
10. **[Do uzupełnienia – metody dokładne]**  
11. **[Do uzupełnienia – analiza wydajności]**  

---

> **Uwaga**: Powyższy README w formacie Markdown powinien poprawnie wyświetlać się w serwisach i edytorach (np. GitHub, GitLab, VS Code).  
> Dzięki temu cały raport nie będzie jedną „wielką ścianą tekstu”.
