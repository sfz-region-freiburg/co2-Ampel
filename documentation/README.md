# Aufbau der CO2 Ampel
Die CO2 Ampel besteht aus relativ wenigen Komponenten.

![CO2 Ampel - Blockschaltbild](https://github.com/sfz-region-freiburg/co2-Ampel/blob/main/images/blockdiagram.png)

![CO2 Ampel - Bild](https://github.com/sfz-region-freiburg/co2-Ampel/blob/main/images/komponenten.png)


### CO2 Sensor 
Der CO2 Sensor SCD30 von Sensirion misst über Infrarotstrahlung in seiner Messkammer die Konzentration an CO2 Partikeln in der Luft. Vereinfacht gesagt wird Licht im Infrarotbereich ausgesendet und abhängig von der CO2 Konzentration kommt mehr oder weniger von diesem Licht an einem Empfänger an. Je mehr CO2 in der Luft desto weniger Licht kommt an.

### Arduino Nano
Der Arduino Nano ist sozusagen die Schaltzentrale der CO2 Ampel, er bekommt vom CO2 Sensor die aktuelle Raumluftkonzentration, wertet diese aus, zeigt diese auf dem Display an und abhängig von den eingestellten Grenzwerten werden die RGB-LED, der Piezo Buzzer sowie die Flash LEDs geschalten. Die Kommunikation zwischen Arduino, CO2 Sensor und Display erfolgt über I2C, die LEDs und der Buzzer werden über digitale Ausgänge angesteuert.

### Display
Bei dem Display handelt es sich um ein sogenanntes Punktmatrix-Display mit einer Auflösung von 128 Punkten horizontal und 32 Punkten vertikal. Jeder dieser Punkte kann nun ein oder ausgeschalten werden. Um auf diesem Display nun Text oder Zahlen anzuzeigen wird eine Bibliothek verwendet die aus dem jeweiligen Symbol ein Punktmuster erzeugt. 

![Display - Hallo](https://github.com/sfz-region-freiburg/co2-Ampel/blob/main/images/display.png)

Beispiel „Hallo!“

### RGB-LED
Bei der RGB-LED steht RGB für die drei Farben Rot, Grün und Blau. Es sind also drei LEDs in einem Gehäuse. Dies hat den Vorteil, dass man nicht nur die drei Farben hat, sondern auch diese drei Farben mischen kann. So erhält man zum Beispiel, wenn die Rote und die Grüne LED an sind die Farbe Gelb. 

![Display - Hallo](https://github.com/sfz-region-freiburg/co2-Ampel/blob/main/images/rgb.png)

### Piezo Buzzer
Ein Piezo Buzzer ist eine Art Lautsprecher. Durch Anlegen einer elektrischen Spannung schwingt der Piezo Kristall und bewegt so eine Membran, die diese mechanische Bewegung in Schallwellen umwandelt. Er dient als akustisches Signal bei Überschreiten des eingestellten Grenzwertes. Man muss also nicht immer auf die CO2 Ampel schauen.

### FLASH LEDs
Die beiden Flash LEDs sind sehr helle LEDs die rot leuchten. Sie werden immer nur kurz angeschaltet und erzeugen somit einen „Blitz“. Auch sie sollen auf die Überschreitung des Grenzwertes aufmerksam machen.

# Schaltplan
![CO2 Ampel - Schaltplan](https://github.com/sfz-region-freiburg/co2-Ampel/blob/main/images/schaltplan.png)


# Komponentenliste
| Bauteil 						| Beschreibung | Bezeichnung | Anzahl | Bestelllink | Preis (ca) |
| ---- | ---- | ---- | ---- | ---- | ---- |
| CO2 Sensor 					| | IC1 		| 1 | | |
| Arduino Nano					| | IC2 		| 1 | | |
| Display 						| | IC3 		| 1 | | |
| RGB-LED 						| | LED3 		| 1 | | |
| Piezo Buzzer 					| | SG1 		| 1 | | |
| FLASH LEDs 					| | LED1, LED2 	| 2 | | |
| Vorwiderstand 120hm 			| | R3,R4,R5	| 3 | | |
| Vorwiderstand 300Ohm 			| | R6,R7		| 2 | | |
| I2C Pullup Widerstand 3300Ohm | | R1,R2 		| 2 | | |
| Gehäuse 						| | 			| 1 | | |
| Powerbank 					| |  			| 1 | | |




# Gefördert durch:
[![Gisela und Erwin Sick Stiftung](https://sfz-region-freiburg.de/wp-content/uploads/2020/02/sfz-unterstuetzer-sick-stiftung.jpg)](https://www.sick-stiftung.org/)