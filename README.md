# Parallel-Bruteforce
Bruteforce-Algorithm with parallel computing

Ablauf:

- Es existiert eine Liste von Hashes
- Vordefinierter Suchraum, d.h. definierte Länge des möglichen Passwortbereiches
- Master teilt Suchraum in n-Teile 
- Client berechnet für jedes Wort in seinem Suchraum einen Hash
- Client vergleicht Hashes mit Liste
- Jeder Client erhält die Liste aller zu berechnenden Hashes



Datenübertragung:

- Gefundene Hashes sollen direkt an den Server übertragen werden
- Client async send / Server async receive 

Zeit-/Performance-Messung:

- Vergleich sequentielle vs parallele Lösung
- Vergleich des theoretischen gegen den realen Speed-Up
- Ermittelung des optimalen Systems (Grenzen der Parallelisierung)
- Ablaufdiagramme (Server/Client Seite)

Weiterführende Ideen:

- lokale parallelisierung der Clients über OpenMP

Ablaufdiagramm
![Bild](http://i58.tinypic.com/2vjd0rq.png "Ablaufdiagramm")
