# 🤖 Autonominen Esteitä Väistävä Robotti (C++ / PlatformIO)

**Tekijä:** Topias Kujanpää  
**Status:** 🚧 Työn alla (Moottoriajureiden asennus seuraavaksi)  
**Ympäristö:** VS Code + PlatformIO

## 📖 Projektin kuvaus
Tämä on sulautettu järjestelmäprojekti, jossa rakennetaan autonomisesti navigoivaa robottia. Robotti havainnoi ympäristöään ultraäänianturilla ja tekee päätöksiä reitinvalinnasta reaaliaikaisesti.

Projektin tavoitteena on demonstroida C++-ohjelmointitaitoja, sensoridatan käsittelyä ja logiikan toteutusta Arduinolla

### 💡 Nykyinen tila: "Visual Debugging"
Koska moottoriajurit (L9110S) odottavat vielä asennusta, robotin aivot on ohjelmoitu **Visual Debugging** -tilaan. 
* Moottoreiden sijaan robotti ohjaa LED-merkkivaloja.
* Tämä mahdollistaa navigointilogiikan ja sensorien testaamisen turvallisesti pöydällä ennen liikkuvia osia.

---

## 🎥 Demo: Logiikkatesti
Alla olevalla videolla näkyy, kuinka robotti reagoi esteisiin, pysähtyy, skannaa ympäristön servolla ja päättää uuden suunnan.

[Katso video YouTubessa (Lue kuvaus)!] (https://youtube.com/shorts/HBnDYzbjyys)

**Värikoodit videolla:**
* 🟢 **Vihreä:** Reitti vapaa (Aja eteenpäin)
* 🔴 **Punainen:** Este edessä (Peruuta)
* 🟡 **Keltainen:** Käänny (Vasen/Oikea esteen sijainnista riippuen)
* 🟢🔴🟡🟡 **Kaikki:** Robotti on pysähtynyt.

---

## 🛠️ Laitteisto ja Mekaniikka

Robotti yhdistelee valmiita komponentteja ja itse valmistettuja osia.

### 🖨️ 3D-Tulostettu Sensoriteline
Ultraäänianturin (HC-SR04) kiinnitys servomoottoriin on toteutettu **3D-tulostamalla** kustomoitu teline. 
* **Tulostin:** Prusa MK4
* **Materiaali:** PLA/PETG
* Teline mahdollistaa anturin tarkan kääntymisen 180 astetta.
![3DTulostus](https://github.com/user-attachments/assets/a7333163-69d6-4e49-9fe9-82f02d315cb3)



### 🔌 Komponentit
* **MCU:** Arduino Uno
* **Virtalähde (Logiikka):** 9V Paristo (Arduino, sensorit, servo)
* **Virtalähde (Moottorit):** Fulbat 6V 4Ah Akku
* **Sensorit:** HC-SR04 Ultraääni (Trig: 12, Echo: 11)
* **Toimilaitteet:** SG90 Servo (Pin 7)
* **Runko:** 4WD Robottialusta (akryyli/metalli)
* **Moottoriajurit:** L9110S

---

## 📸 Rakennusvaiheet

### 1. Rungon kasaus
Alusta ja moottorit asennettuna.
![Rungon kasaus](https://github.com/user-attachments/assets/551fc064-0d9e-4008-ac16-9eb8c35c5de9)


### 2. Prototyypin johdotus
Kytkennät toteutettu koekytkentälevylle (Breadboard) testausta varten.
![Kytkenät](https://github.com/user-attachments/assets/529d43b6-553a-4650-afc7-2e78efef399a)

### 3. Moottoriajureiden asennus (Tulossa)...
Tämä vaihe suoritetaan heti, kun tilatut **L9110S-moottoriajurit** saapuvat. 
* Väliaikaiset LED-kytkennät puretaan.
* Moottoriajurit asennetaan runkoon ja kytketään DC-moottoreihin.
* Koodi päivitetään ohjaamaan moottoreita LEDien sijaan.



