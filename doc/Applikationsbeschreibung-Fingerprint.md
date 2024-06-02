<!-- 
cSpell:words knxprod EEPROM Ausgangstrigger Sonnenstandsbezogene Sonnenauf vollzumüllen Enocean Pieptönen platformio
cSpell:words softwareseitig untergangszeit Urlaubsinfo Feiertagsinfo Konverterfunktionen Vergleicher Geokoordinaten
cSpell:words Konstantenbelegung vorzubelegen Intervallvergleich Hysteresevergleich Uebersicht Logiktrigger priorität
cSpell:words Szenenkonverter Szenennummern Zahlenbasierte Intervallgrenzen Hystereseschalter Ganzzahlbasierte
cSpell:words erwartungskonform hardwareabhängig Rueckkopplung eingabebereit maliges AUSschaltverzögerung EINschaltverzögerung
cSpell:words Triggersignal expample runterladen Wiregateway updatefähige Updatefunktion Auskühlalarm Zaehler tagestrigger
cSpell:words mgeramb ambiente Ambientenbeleuchtung
-->

# Applikationsbeschreibung Fingerprint

Die Applikation Fingerprint erlaubt eine Parametrisierung einer Zutrittskontrolle per Fingerabdruck mit der ETS.

Es gibt eine kleinere Applikationsversion mit bis zu 200 Aktionen/Fingerzuordnungen und eine größere mit bis zu 1500 Aktionen/Fingerzuordnungen. Während die größere hauptsächlich für das "großen" Lesegerät R503Pro angeboten wird, kann sie bei Bedarf an beispeilsweise mehr als 200 Fingerzuordnungen auch für das "kleinere" Lesegerät R503 verwendet werden.

## Änderungshistorie

Im folgenden werden Änderungen an dem Dokument erfasst, damit man nicht immer das Gesamtdokument lesen muss, um Neuerungen zu erfahren.

02.06.2024: Firmware 0.2.1, Applikation 0.2

* KO "Scan: Erfolg" liefert nun eine "0", wenn ein Finger nicht erkannt wurde.

01.06.2024: Firmware 0.2.0, Applikation 0.2

* Es gibt nun eine Personensuche, mit der man nach den zugewiesenen Fingern und Namen suchen kann.
* Die Synchronisation zwischen mehreren Fingerabdrucklesern ist nun vollständig implementiert. Ist diese aktiviert, erfolgt die Synchronisation automatisch, nachdem ein neuer Finger angelernt wurde. Über die ETS ist es aber auch möglich die Synchronisation einzelner Finger manuell anzustoßen. Auch das Löschen eines Fingers wird synchronisiert.
* Die Funktion und Farbe des Fingerprint LED-Rings kann nun über Rohdaten-KOs auch extern gesteuert werden.

18.05.2024: Firmware 0.1.0, Applikation 0.1

* Initiales Release als OpenKNX Fingerprint

## **Einleitung**

<!-- DOC HelpContext="Dokumentation" -->
Mit diesem Modul können Finger im Lesegerät angelernt, gelöscht, Aktion verknüft und Finger den Aktionen zugeordnet werden.

## **Allgemein**

Hier werden Einstellungen vorgenommen, die für das gesamte Logikmodul und alle Kanäle gelten.

### Hardware

<!-- DOC -->
#### **Fingerprint Scanner**

Hier kann die angeschlossene Fingerprint-Scanner-Hardware ausgewählt werden.

Aktuell werden folgende Scanner unterstützt:

* R503 (Standard): Speicherplatz für 200 Finger
* R503S: Speicherplatz für 150 Finger
* R503Pro: Speicherplatz für 1500 Finger

### Autorisierung

<!-- DOC -->
#### **Warten auf Autorisierung**

Ist eine Autorisierung für eine Aktion angefordert, wird die hier angegebene Zeit auf das Auflegen eines Fingers auf den Scanner gewartet, bis die Aktion abgebrochen wird.

### Neuen Finger anlernen

<!-- DOC -->
#### **Name der Person**

Der Name der Person, welcher zusammen mit den neu angelernten Fingerdaten gespeichert werden soll.

<!-- DOC -->
#### **Finger der Person**

Der Finger der Person, welcher zusammen mit den neu angelernten Fingerdaten gespeichert werden soll.

<!-- DOC -->
#### **Scanner Finger ID**

Die ID des Fingers (= der Speicherplatz), auf welche die neu angelernten Fingerdaten gespeichert werden soll.

Dabei sind die verfügbaren SPeicherplätze abhängig von der ausgewählten Hardware des Fingerprint-Scanners:
Sie werden dabei von 0 beginnned durchnummeriert. Hat der Scanner also beispielsweise 200 Speicherplätze, stehen die IDs 0-199 zur Verfügung.

### Finger löschen

<!-- DOC -->
#### **Scanner Finger ID**

Die ID des Fingers (= der Speicherplatz), die gelöscht werden soll.

### Zusatzfunktionen

<!-- DOC -->
#### **Touch-Frontplatine vorhanden**

Optional kann die Basisplatine auf eine Touch-Frontplatine aufgesteckt werden.

Ist diese vorhanden und wurde die Option hier aktiviert, stehen einige zusätzliche Kommunikationsobjekte zur Verfügung. 

<!-- DOC -->
#### **Rohdaten auf den Bus senden**

Der Fingerprint-Scanner kann seine Daten direkt auf den Bus senden, ohne jegliche Aktionszuordnungen "dazwischen".

Bei Aktivierung werden entsprechende Kommunikationsobjekte freigeschaltet.

<!-- DOC -->
##### **Zutrittsdaten-KOs aktivieren**

Werden die speziellen Kommunikationsobjekte für Zutrittsdaten benötigt (DPT 15), können diese hier aktiviert werden.

<!-- DOC -->
#### **Synchronisation mehrerer Scanner**

Sind mehrere Fingerprint-Scanner vorhanden und sollen die Fingerdaten unter diesen Scanner synchronisiert werden, sollte diese Option aktiviert werden.

Es stehen daraufhin zusätzliche Kommunikationsobjekte zur Synchronisation zur Verfügung.

<!-- DOC -->
#### **Verzögerung zwischen Sync-Telegrammen**

Um eine zu hohe Busbelastung zu vermeiden, wird hier die Verzögerung zwischen Sync-Telegrammen in Millisekunden festgelegt.

<!-- DOC -->
#### **Finger ID synchronisieren**

Grundsätzlich erfolgt die Synchronisation nach einem Anlernvorgang automatisch.

Zusätzlich kann hier die Synchronisation eines bestimmten Fingers auch manuell angestoßen werden.

<!-- DOC -->
#### **Externe Kontrolle ermöglichen**

Bei Aktivierung werden entsprechende Kommunikationsobjekte freigeschaltet, die dazu verwendet werden können den Scanner extern zu steuern (wie ein Anlernvorgang extern anzustoßen).

### **Gefährliche Funktionen**

<!-- DOC -->
#### **Passwort**

Wenn Sie hier ein Passwort vergeben und dieses vergessen, wird das Fingerprint-Lesegerät unbrauchbar. Das Passwort kann nicht wiederhergestellt werden!

Sie haben die Möglichkeit ein Passwort erstmalig festzulegen oder ein bereits vorhandenes zu ändern. Im letzteren Fall muss auch das alte Passwort eingegeben werden.

<!-- DOC -->
##### **Altes Passwort**

Das bereits vorhandene Passwort, welches geändert werden soll.

<!-- DOC -->
##### **Neues Passwort**

Das neue, zu setzende Passwort.

<!-- DOC -->
#### **Alle Finger löschen?**

Mit dieser Funktion werden sämtliche gespeicherte Fingerdaten inklusive Personenzuordnung unwiderruflich gelöscht.

## Aktionen

<!-- DOC -->
### **Verfügbare Aktionen**

--ToDo--

Die ETS ist auch schneller in der Anzeige, wenn sie weniger (leere) Aktionen darstellen muss. Insofern macht es Sinn, nur so viele Aktionen anzuzeigen, wie man wirklich braucht.

--ToDo--



## **Unterstützte Hardware**

Die Software für dieses Release wurde auf folgender Hardware getestet und läuft damit "out-of-the-box":

* **AB-SmartHouse Fingerprint-Leser** [www.ab-smarthouse.com](https://www.ab-smarthouse.com/produkt/openknx-fingerprint-leser/) als Basisplatine mit Finger-Lesegeräte R503, R503S und R503Pro

Andere Hardware kann genutzt werden, jedoch muss das Projekt dann neu kompiliert und gegebenenfalls angepasst werden. Alle notwendigen Teile für ein Aufsetzen der Build-Umgebung inklusive aller notwendigen Projekte finden sich im [OpenKNX-Projekt](https://github.com/OpenKNX).

Interessierte sollten auch die Beiträge im [OpenKNX-Forum](https://knx-user-forum.de/forum/projektforen/openknx) studieren.

## **Übersicht der vorhandenen Kommunikationsobjekte**

Hier werden nur Kommunikationsobjekte (KO) des Fingerprint-Moduls beschrieben, die KO anderer Module sind in der jeweiligen Applikationsbeschreibung dokumentiert.

KO | Name | DPT | Bedeutung
:---:|:---|---:|:--
1 | in Betrieb | 1.002 | Meldet zyklisch auf den Bus, dass das Gerät noch funktioniert. Das KO steht nicht zur Verfügung, wenn kein Sendezyklus eingestellt wurde.
2 | Uhrzeit | 10.001 | Eingang zum empfangen der Uhrzeit
3 | Datum | 11.001 | Eingang zum empfangen des Datums