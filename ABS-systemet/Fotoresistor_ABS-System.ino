//Beskrivning:  Funktion som mäter varvtal och hastighet på två hjul med en analog foto-resistor. samt Jämför dessa hastigheter och varnar användaren om de skiljer sig åt mer än en bestämmd procentsats (tollerans), den ger information både genom seriell-monnitorn och med led-lampor.


// By:           Isak Jonsson Öhström
// Date:         2022-03-28




void setup() {
  Serial.begin(9600);   //definerar baud raten för seriell porten eftersom jag kommer använda seriall-monitorn.
  pinMode(13, OUTPUT);  //definerar pin 13 som output (grön LED)
  pinMode(12, OUTPUT);  //definerar pin 12 som output (röd LED)
}

  float intervall = 0.5;  //initsierar och definerar värdet för intervallen som bestämmer hur ofta den skall ta den samlade mätdatan, utföra beräkningar och ge användaren ett värde. (lägre värde ger resultat oftare men ökar risken för störningar och mätfel som kan försummas om en högre intervall används) 0.5s var ganska lagom.
  float antalhol = 10;    //initsierar och definerar antalet hål det finns i "mäthjulet" detta behövs för att ekvationen skall veta hur många hög och låg värden det är per varje varv. (högst upp i koden så att använaren skall kunna ändra lätt.)
  int diameter = 8;     //hjulets diamter i cm
  float speedkoeicient = diameter * 0.01 * 3.1415 /60 * 3.6 * 2; //ekvation för att omvandla rpm till km/h,
  int antalhjul = 2;  //öantalet hjul som skall ta mätvärden ifrån, dock inte det enda som behöver ändras om man vill ha fler eller färre hjul, detta är en utvecklingspotential.
  int hjulsensor1 = A1;   // pin för fotosensorn på hjul1
  int hjulsensor2 = A2;    // pin för fotosensorn på hjul2
  float tolleransprocent = 5;   // Tollerans i procent som hjulen kan skilja sig åt och inte lösa ut ABS-varningen.
  
  
  int varvvariabel = 0;   //initsierar den varaibel som kommer inkrementeras upp för jarve gång som sensorn har fått en signal innom intervallet. (hjul 1)
  int varvvariabel2 = 0;    //samma fast för hjul nummer 2.
  int rep = 0;    // initsierar en variablen som krävs för att se till att ovan avariable inkrementeras endast en gång per uppnått mätvärde och inte fera gånger (se if-sats nedan.)
  int rep2 = 0;   // samma för hjul 2.
  float rpm = 0;  //initsierar varaibeln för varvtalet (rpm) för hjulet.
  float rpm2 = 0; // samma för hjul 2.
  float rpmtot = 0;   // en varaibeln som används för summan av rpm och rpm2
  float klockstart = 0; //Initsierar varaibeln för hur mycket som skall subtraheras från totala tiden sedan arduinon startade (millis()). detta skall var float o inte interger, det skapade mysko fel.
  float helavarv = 0;   //initsierar varaiblen för hur många hela varv som hjulet snurrat.
  float helavarv2 = 0;  // samma för hjul2


void loop() {

  ABS_systemet();   //en stor funktion för hela mitt program. (lite svårt att använda del och hjälpfunktioner när jag jobbar med tid och loopar.) men fullt möjligt och en utvecklingspotential.

}


void ABS_systemet(){  //void funktion har ingen return. // lite svårt med hjälpfunktioner eftersom vi jämför med tid och många olika variabler som hela tiden påverkas.

  
  float tid = millis()*0.001-klockstart;    //Räknar tiden sedan arduinon startade, gör om det till hela sekunder och subtraherar variabeln klockstart för att återställa "tid" till noll varje gång intervallet har uppnåtts. tror millis är den enda tidräknaren såm kan gå samtidigt som andra processer, men går inte att reseta.   
  int value = analogRead(hjulsensor1);      //tilldelar variablen value mätvärdet från foto sensorn på hjul1 (A1), (analog read, 0-1024).
  int value2 = analogRead(hjulsensor2);     // samma för hjul2.

  
 if (value > 900 && rep == 0)   // ifsats med gränsvärde 900 och rep som förhindrar flera upprepningar för varje "high" värde.
  {
    digitalWrite(13, HIGH);   //visar att mätvärde har tagits in.
    varvvariabel += 1;    //inkrementera varvariabel med 1 för varje high värde.
    rep = 1;  // kräver att vi måste mäta ett "low-value" innan vi kan inkrementera ett nytt "high-value" annars kommer arduino bara att inkrementera på upp "varvvariabl" med hög hastighet.
  }

 else if (value > 900)   //om rep inte är lika med noll skal den bara tända lampan.
  {
    digitalWrite(13, HIGH);
  }
  

 else        // om mätvärdet är under 900
  {
    digitalWrite(13, LOW);   
    rep = 0;    // rep = 0, kräver ett uppmätt low value innan vi kan imkrementera varvvariabel igen.
  }


//---
// nedan följer samma ifsats för hjhul nummer 2.
// man skulle kunna göra detta till funktioner, utvecklingspotential.

  if (value2 > 900 && rep2 == 0)    
  {
    digitalWrite(13, HIGH);
    varvvariabel2 += 1;
    rep2 = 1;
  }

  else if (value2 > 900)
  {
    digitalWrite(13, HIGH);
  }
  

  else 
  {
    digitalWrite(13, LOW);
    rep2 = 0;
  }

//---

  
  if (tid > intervall)    // om min tidvariabel har uppnått intervallet / om det har gått 0.5 sekunder.
  {
    
    klockstart += intervall;   //inkrementerar klockstart för att återställa tid till noll då intervallet har uppnåtts.
    
    helavarv = varvvariabel/antalhol;   // omvandalr antalet hög och låg värden under 0.5s till antalet varv som hjulet snurrat.
    
    rpm = helavarv/intervall*60;        // använder tiden 0.5s för att omvandal omvandla antalet varv till varvtal/varv per minut. (RPM).

    //samma för hjul nummer två:

    helavarv2 = varvvariabel2/antalhol;
    rpm2 = helavarv2/intervall*60;

    rpmtot = rpm + rpm2;  //summan av varvtalen, använs senare i koden.
  
    varvvariabel = 0;   // återställer varvariablerna till noll för att kunna ikrementeras på nytt då nästa intervall börjar.
    varvvariabel2 = 0;   
      
    Serial.print("Hjul1" "RPM :  ");    
    Serial.println(rpm);          //skriver ut uppmätta varvtalet för hjul 1
  
    Serial.print("Hjul2" "RPM :  ");
    Serial.println(rpm2);          //skriver ut uppmätta varvtalet för hjul 2
  
    Serial.print("hastighet: ");
    Serial.print((rpm + rpm2)/2 * speedkoeicient);    // omvandlar rpm till km/h och skriver ut det i seriell-monitorn.
    Serial.println(" km/h");



  if ((abs((rpm+rpm2)/antalhjul - rpm2) > rpm * (tolleransprocent * 0.01)) && (rpmtot > 3)){   //om absolutbeloppet av genomsnittshastigheten av varvtalen är mer än 5% större än varvtalet för hjul 1 (5% tollerans, bestämmt av en variabel högst upp). och det totala varvtalet är över 3 (förhindrar störningar och märfel vis stillastående.)

    //denna if sats är vad som jämför hastighterna och gör två hastighetsmätare till ett ABS-system

    digitalWrite(12, HIGH);     //tänd varningslampan för ABS-systemet (röd).
    Serial.println("ABS-Varning!");     //skriv ABS varning i seriell-monitorn.
    //return true;
    //hade en tanke på att gör detta till en int funktion istället, och returnera true/false beroende på om ABS.systemet löst ut, men det är utvecklingspotential.
  }
  

  else 
  {
    digitalWrite(12, LOW);   //är inte systemet utlöst skall den röda lampan vara släckt.
  }


// Isak Jonsson Öhström - 28 mars 2022

  
}
}
