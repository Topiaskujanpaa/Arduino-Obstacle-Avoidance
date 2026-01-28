# 🤖 Autonominen Esteitä Väistävä Robotti (C++ / PlatformIO)

**Tekijä:** Topias Kujanpää  
**Ympäristö:** VS Code + PlatformIO

## 📖 Projektin kuvaus
Tämä on sulautettu järjestelmäprojekti, jossa rakennetaan autonomisesti navigoivaa robottia. Robotti havainnoi ympäristöään ultraäänianturilla ja tekee päätöksiä reitinvalinnasta reaaliaikaisesti.
Projektin tavoitteena on demonstroida C++-ohjelmointitaitoja, sensoridatan käsittelyä ja logiikan toteutusta Arduinolla

## 🎥 Demo:
Alla olevalla videolla näkyy, kuinka robotti reagoi esteisiin, pysähtyy, skannaa ympäristön servolla ja päättää uuden suunnan.
[Katso video YouTubessa (Lue kuvaus)!] (https://youtube.com/shorts/HBnDYzbjyys)
**Värikoodit videolla:**
* 🟢 **Vihreä:** Reitti vapaa (Aja eteenpäin)
* 🔴 **Punainen:** Este edessä (Peruuta)
* 🟡 **Keltainen:** Käänny (Vasen/Oikea esteen sijainnista riippuen)
* 🟢🔴🟡🟡 **Kaikki:** Robotti on pysähtynyt.

Tässä vielä video robotin toiminnasta, kun ajuri on asennettu:
https://youtube.com/shorts/7eB4zP6gT3k?feature=share

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
* **Virtalähde (Logiikka):** 9V Paristo (Arduino, HC-SR04)
* **Virtalähde (Moottorit):** Fulbat 6V 4Ah Akku
* **Sensorit:** HC-SR04 Ultraääni (Trig: 12, Echo: 11)
* **Toimilaitteet:** SG90 Servo (Pin 7)
* **Runko:** 4WD Robottialusta (akryyli/metalli)
* **Moottoriajuri:** L298N

---

## 📸 Rakennusvaiheet

### 1. Rungon kasaus
Alusta ja moottorit asennettuna.
![Rungon kasaus](https://github.com/user-attachments/assets/551fc064-0d9e-4008-ac16-9eb8c35c5de9)


### 2. Prototyypin johdotus
Kytkennät toteutettu koekytkentälevylle (Breadboard) testausta varten.
![Kytkenät](https://github.com/user-attachments/assets/529d43b6-553a-4650-afc7-2e78efef399a)

### 3. Moottoriajureiden asennus
Projektissa siirryttiin käyttämään **L298N** -moottoriajuria, koska se kestää paremmin kuormaa ja mahdollistaa neljän moottorin ohjauksen luotettavasti.
* **Virranhallinta (Logic Power):** Järjestelmä käyttää 6V akkua. L298N-ajurin sisäinen 5V-regulaattori on ohitettu (jumper poistettu), ja ohjauselektroniikka ottaa 5V jännitteen suoraan Arduinosta. Tämä varmistaa vakaan toiminnan matalammalla akkujännitteellä.
![IMG_E2276](https://github.com/user-attachments/assets/708c404d-0f5a-48d2-b35d-6648796abe27)

## 🧠 Älykkyys ja Logiikka

1.  **Ajo eteenpäin:** Robotti etenee, kunnes ultraäänianturi havaitsee esteen alle 25 cm päässä.
2.  **Pysähtyminen:** Kun este havaitaan, moottorit pysähtyvät välittömästi. Tämän jälkeen robotti peruuttaa, jottei se jää umpikujaan jumiin.
3.  **Skannaus:** Servo kääntää sensoria vasemmalle ja oikealle mitatakseen etäisyydet sivuilta.
4.  **Päätöksenteko:** Algoritmi vertaa vasemman ja oikean puolen tilaa ja kääntää robotin sinne, missä on enemmän tilaa.
5.  **Peruutus:** Jos molemmat suunnat ovat tukossa, robotti peruuttaa lisää ja yrittää uudelleen.




