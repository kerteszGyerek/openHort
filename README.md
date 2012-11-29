OpenHort kertészeti vezérlő rendszer
====================================

Alapvető célok:
 - Környezeti paraméterek, növény paraméterek, mérése, tárolása egy központi adatbázisban. (hőmérséklet, páratartalom, levélhőmérséklet, levélszín, stb.)
 - Növény paraméterek manuális méréseit be lehessen illeszteni az adatbázisba. (méret, terméstömeg, szín, stb.)
 - Az adatokon végezzen számításokat. (átlagok, effektív hőösszeg, optimumtól való eltérés, stb.)
 - Beavatkozó szervek működtetése. (pumpák, kapcsolók, ablaknyitók, stb.)
 - Legyen bármely kertész által összeépíthető és üzemeltethető!!

Arduino
-------

Jelenleg egy arduino mér, és kezeli a beavatkozó szerveket. Ennek a forrása található az Arduino mappában. 
TimeAlarm library használatával vannak időzítve az események, majd USB-n kiküldi logokat.

Rooter
------

A külvilággal egy Asus WL500g rooter köti össze, rajta OpenWRT Bbackfire 10.03.1 fut. Ennek az USB-portjára van dugva az arduino.  
Kommunikál az arduinoval. Beállítja az időt, szűri és továbbítja az arduino logjait a web felé.

Web
---

Az openhort.ártó.hu (igen, ékezetes) fut az uploadData.php
Most még nagyon ne nézd meg:) nem látsz semmit.

