
/*
 * Projekti: Autonominen esteitä väistävä robotti (4WD)
 * Tekijä: Topias Kujanpää
 * Päiväys: Joulukuu 2025
 * 
 * Kuvaus:
 * Tämä ohjelma ohjaa autonomista robottia, joka väistelee esteitä
 * ultraäänianturin (HC-SR04) ja kääntyvän servon avulla.
 * Koodi sisältää tällä hetkellä logiikan esteentunnistukseen, ympäristön skannaukseen
 * ja reitinvalintaan.
 * HUOM: Robotin liikkeet on tällä hetkellä simuloitu LED-merkkivaloilla
 * (Visual Debugging), koska moottoriajurit odottaa päivitystä.
 * Itse ohjauslogiikka on kuitenkin täysin toimiva ja testattu.
 * 
 * Laitteisto / Kytkennät (Pinout):
 * - Arduino Uno
 * - Servo (SG90): Pin 7
 * - Ultraääni anturi (HC-SR04): Trig 12, Echo 11
 * - Moottoriajurit: 2kpl L9110S
 * 
 * kehitysympäristö: VS Code + PlatformIO
 */
#include <Arduino.h>
#include <Servo.h>

Servo myservo;
// Servo
const int servoPin = 7;
// HC-SR04
const int trigPin = 12;
const int echoPin = 11;
const int MinEtaisyys = 15;
const int MittaustenMaara = 4;

// Moottoriohjaus (LEDit simuloivat moottoreita tällä hetkellä)

const int LEDvas = 2;
const int LEDoik = 3;
const int LEDeteen = 4;
const int LEDtaakse = 5;


// servon vakiot
const int viive = 10; // viive asentojen välillä
const int kulmaA = 20; // servon kulma 1. ääriasento
const int kulmaB = 160; // servon kulma 2. ääriasento


// Funktiot etäisyyden mittaamiseen ja esteiden tarkastukseen

// Mittaa "MittaustenMaara" vakion määrämän kertaa ja palauttaa niiden keskiarvon, hylkää "0" mittaukset.
long mittaaEtaisyys() {
  long summa = 0;
  int onnistuneetMittaukset = 0;

  // Otetaan 4 mittausta
  for (int i = 0; i < (MittaustenMaara-1); i++) {
    
    // --- 1. Lähetä ping ---
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // --- 2. Lue kaiku (AIKAKATKAISULLA!) ---
    // 15000 mikrosekuntia = n. 15ms odotus (max etäisyys n. 2,5 metriä)

    long duration = pulseIn(echoPin, HIGH, 15000);

    // --- 3. Suodata virheet ---
    if (duration == 0) {
      continue; // Hylkää nollat, yritä uudestaan
    }

    // --- 4. Lisää summaan ---
    summa += duration;
    onnistuneetMittaukset++;
    
    // --- 5. Tärkeä pieni tauko ---
    // Ultraääni kaikuu huoneessa. Pieni tauko estää "haamukaiut".
    delay(5); 
  }

  // Jos kaikki mittaukset epäonnistui (esim. johto irti), palauta 0
  if (onnistuneetMittaukset == 0) return 0;

  // Laske keskiarvo
  long keskiarvoDuration = summa / onnistuneetMittaukset;
  int cm = keskiarvoDuration * 0.034 / 2;
  Serial.print("Keskiarvo: ");
  Serial.println(cm); 
  return cm;
  
} 





// tarkastaa onko Vasemmalla puolella esteitä:
// palauttaa false jos on, true jos ei
// MittausIndexi = kuinka usein mittaus tehdään
bool VasenOK() {
  int MittausIndexi = 0;
  // Viive niin ei mitata samaa estettäkahta kertaa
  delay(1000);

  for (int kulma = 0; kulma <= 70; kulma += 1) {
    
    // Mitataan joka kymmenes askel
    if (MittausIndexi == 10) {
      int x = mittaaEtaisyys();
      // Jos este löytyy, palauta false heti
      if (x > 0 and x <= 10) {
        delay(750);
        myservo.write(90);
        Serial.println("--------ESTE HAVAITTU VASEMMALLA----------");
        return false; 
      }    
      // Jos ei estettä, nollataan laskuri ja JATKETAAN NORMAALISTI
      MittausIndexi = 0; 
    }
    // Nämä rivit pitää suorittaa joka kerta (paitsi jos este löytyi ja poistuttiin)
    myservo.write(kulma);
    MittausIndexi += 1;
    delay(viive);
  }

  // Jos päästiin loppuun asti ilman törmäystä:
  Serial.print("----------VASEN VAPAA-----------");
  myservo.write(90);
  return true;
}

// tarkastaa onko Oikealla puolella esteitä:
// palauttaa false jos on, true jos ei
// MittausIndexi = kuinka usein mittaus tehdään

bool OikeaOK() {
  int MittausIndexi = 0;
  // Viive niin ei mitata samaa estettäkahta kertaa
  delay(1000);

  for (int kulma = 180; kulma >= 110; kulma -= 1) {

    // Mitataan joka kymmenes askel
    if (MittausIndexi == 10) {
      int x = mittaaEtaisyys();
      // Jos este löytyy, palauta false heti
      // Etäisyyden täytyy olla suurempi kuin 0, koska HC-SR04 palauttaa 0 jos este on liian kauakana 
      if (x > 0 and x <= 10) {
        delay(750);
        myservo.write(90);
        Serial.println("--------ESTE HAVAITTU OIKEALLA----------");
        return false; 
      }    
      // Jos ei estettä, nollataan laskuri ja JATKETAAN NORMAALISTI
      MittausIndexi = 0; 
    }
    // Nämä rivit pitää suorittaa joka kerta (paitsi jos este löytyi ja poistuttiin)
    myservo.write(kulma);
    MittausIndexi += 1;
    delay(viive);
  }

  // Jos päästiin loppuun asti ilman törmäystä:
  Serial.print("----------OIKEA VAPAA-----------");
  myservo.write(90);

  return true;
}




// AJOFUNKTIOT:

void ajaTaakse() {
  digitalWrite(LEDvas, LOW);
  digitalWrite(LEDoik, LOW);
  digitalWrite(LEDeteen, LOW);
  digitalWrite(LEDtaakse, HIGH);  
}
void pysahdy() {
  digitalWrite(LEDvas, HIGH);
  digitalWrite(LEDoik, HIGH);
  digitalWrite(LEDeteen, HIGH);
  digitalWrite(LEDtaakse, HIGH);
}
void ajaEteen() {
  digitalWrite(LEDvas, LOW);
  digitalWrite(LEDoik, LOW);
  digitalWrite(LEDeteen, HIGH);
  digitalWrite(LEDtaakse, LOW);
}
void kaannyVasemmalle() {
  digitalWrite(LEDvas, HIGH);
  digitalWrite(LEDoik, LOW);
  digitalWrite(LEDeteen, LOW);
  digitalWrite(LEDtaakse, LOW);
}
void kaannyOikealle() {
  digitalWrite(LEDvas, LOW);
  digitalWrite(LEDoik, HIGH);
  digitalWrite(LEDeteen, LOW);
  digitalWrite(LEDtaakse, LOW);
}




void setup() {
  myservo.attach(servoPin);
  myservo.write(90);
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(LEDeteen, OUTPUT);
  pinMode(LEDtaakse, OUTPUT);
  pinMode(LEDvas, OUTPUT);
  pinMode(LEDoik, OUTPUT);



  


}
// pääohjelma
void loop() {
  // Mittaa etäisyyden ja kuljetaan samalla eteenpäin
  int etaisyys = mittaaEtaisyys();
  ajaEteen();
  delay(100);
  // Jos etäisyys on liian pieni, pysähdytään ja tarkastetaan sivut
  if (etaisyys <= MinEtaisyys) {
    pysahdy();
    delay(500);
    bool v = VasenOK();
    delay(100);
    bool o = OikeaOK();
    delay(1000);
    // jos molemmat sivut tukossa, peruutetaan ja yritetään uudestaan
    if (o == false and v == false) {
      Serial.println("-----Molemmat sivut tukossa, peruutetaan-----");
      ajaTaakse();
      delay(1000); // liikutaan sekunnin ajan taaksepäin
      pysahdy();
      // peruutetaan niin kauan kunnes toinen sivu on vapaa
      while (o == false and v == false) {
        v = VasenOK();
        if (v == true) {
          Serial.println("-----Vasen puoli vapaa, käännytään vasemmalle-----");
          kaannyVasemmalle();
          delay(1000); // liikutaan sekunnin ajan vasemmalle
          pysahdy();
          break;
        }
        delay(100);
        o = OikeaOK();
        if (o == true) {
          Serial.println("-----Oikea puoli vapaa, käännytään oikealle-----");
          kaannyOikealle();
          delay(1000); // liikutaan sekunnin ajan oikealle
          pysahdy();
          break;
        }
        delay(1000);
        Serial.println("-----Molemmat sivut edelleen tukossa, peruutetaan lisää-----");
        ajaTaakse();
        delay(1000); // liikutaan sekunnin ajan taaksepäin
        pysahdy();
      }
      // Jos oikea on vapaa, käännytään oikealle
    } else if (o == true and v == false) {
      Serial.println("-----Kaannytään OIKEALLE-----");
      kaannyOikealle();
      delay(1000); // liikutaan sekunnin ajan oikealle
      // jos vasen on vapaa, käännytään vasemmalle
    } else if (v == true and o == false) {
      Serial.println("-----Kaannytään VASEMMALLE-----");
      kaannyVasemmalle();
      delay(1000); // liikutaan sekunnin ajan vasemmalle
      // jos molemmat vapaat, jatketaan oikealle
    } else if (o == true and v == true) {
      Serial.println("-----Molemmat sivut vapaat, jatketaan oikealle-----");
      kaannyOikealle();
      delay(1000); // liikutaan sekunnin ajan oikealle
      
    }
 
  }
}


 


