
/*
 * Projekti: Autonominen esteitä väistävä robotti (4WD)
 * Tekijä: Topias Kujanpää
 * Päiväys: Tammikuu 2026
 * 
 * Kuvaus:
 * Tämä ohjelma ohjaa autonomista robottia, joka väistelee esteitä
 * ultraäänianturin (HC-SR04) ja kääntyvän servon avulla.
 * Koodi sisältää tällä hetkellä logiikan esteentunnistukseen, ympäristön skannaukseen
 * ja reitinvalintaan.
 * 
 * Laitteisto / Kytkennät (Pinout):
 * - Arduino Uno
 * - Servo (SG90)
 * - Ultraääni anturi (HC-SR04)
 * - Moottoriajurit: L298N
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
const int MinEtaisyys = 20;
const int MittaustenMaara = 4;
const int MinEste = 25; // Kun etäisyys on tätä pienempi, este havaittu sivuilla
int KaantymisIndexi = 0; // joka toinen käännös on pidempi -> ei pyöritä paikallaan


// Moottoriohjaus 
// vasen puoli:
const int IN1 = 8;
const int IN2 = 9;

// oikea puoli:
const int IN3 = 5;
const int IN4 = 6;

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
      if (x > 0 and x <= MinEste) {
        delay(500);
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
    if (MittausIndexi == MinEste) {
      int x = mittaaEtaisyys();
      // Jos este löytyy, palauta false heti
      // Etäisyyden täytyy olla suurempi kuin 0, koska HC-SR04 palauttaa 0 jos este on liian kauakana 
      if (x > 0 and x <= 10) {
        delay(500);
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

void pysahdy() {
  // Pysäytys molemmille puolille
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void ajaTaakse() {
  // Vasen taaksepäin
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  // Oikea taaksepäin
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(300); // liikutaan 700 ms ajan taaksepäin
  pysahdy();
  delay(500); // pieni tauko peruutuksen jälkeen
}

void ajaEteen() {
// Vasen eteenpäin
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Oikea eteenpäin
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
 
}
void kaannyVasemmalle() {
  // "Tankkikäännös": Vasen pakittaa, Oikea menee eteen
  
  // Vasen pakittaa
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Oikea eteen
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  if (KaantymisIndexi % 2 == 0) {
    delay(600); // käännytään 600 ms ajan
  } else {
    delay(400); // käännytään 400 ms ajan
  }
  pysahdy();
  delay(500); // pieni tauko käännöksen jälkeen
  KaantymisIndexi += 1;
}
  

void kaannyOikealle() {
  // "Tankkikäännös": Oikea pakittaa, Vasen menee eteen
  
  // Oikea pakittaa
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Vasen eteen
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  if (KaantymisIndexi % 2 == 0) {
    delay(600); // käännytään 600 ms ajan
  } else {
    delay(400); // käännytään 400 ms ajan
  }
  pysahdy();
  delay(500); // pieni tauko käännöksen jälkeen
  KaantymisIndexi += 1;
}




void setup() {
  myservo.attach(servoPin);
  myservo.write(90);
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);



  


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
    delay(200);
      // Ensin peruutetaan vähän
    ajaTaakse();
    delay(200);
    pysahdy();
    delay(50); // pieni tauko peruutuksen jälkeen
    bool v = VasenOK();
    delay(60);
    bool o = OikeaOK();
    delay(1000);
    // jos molemmat sivut tukossa, peruutetaan ja yritetään uudestaan
    if (o == false and v == false) {
      Serial.println("-----Molemmat sivut tukossa, peruutetaan-----");
      ajaTaakse();
      // peruutetaan niin kauan kunnes toinen sivu on vapaa
      while (o == false and v == false) {
        v = VasenOK();
        if (v == true) {
          Serial.println("-----Vasen puoli vapaa, käännytään vasemmalle-----");
          delay(400);
          kaannyVasemmalle();
          break;
        }
        delay(300);
        o = OikeaOK();
        if (o == true) {
          Serial.println("-----Oikea puoli vapaa, käännytään oikealle-----");
          delay(400);
          kaannyOikealle();
          break;
        }
        delay(500);
        Serial.println("-----Molemmat sivut edelleen tukossa, peruutetaan lisää-----");
        ajaTaakse();
      }
      // Jos oikea on vapaa, käännytään oikealle
    } else if (o == true and v == false) {
      Serial.println("-----Kaannytään OIKEALLE-----");
      kaannyOikealle();
      // jos vasen on vapaa, käännytään vasemmalle
    } else if (v == true and o == false) {
      Serial.println("-----Kaannytään VASEMMALLE-----");
      kaannyVasemmalle();
      // jos molemmat vapaat, jatketaan oikealle
    } else if (o == true and v == true) {
      Serial.println("-----Molemmat sivut vapaat, jatketaan oikealle-----");
      kaannyOikealle();
      
    }
 
  }
}


 


