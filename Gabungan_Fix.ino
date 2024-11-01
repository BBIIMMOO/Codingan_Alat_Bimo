#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define pHPin A0    // pH meter Analog output to Arduino Analog Input 0
#define TurbPin A3  
#define samplingInterval 20
#define printInterval 800
#define ArrayLength 40  // times of collection
int pHArray[ArrayLength];  // Store the average value of the sensor feedback
int pHArrayIndex = 0;
int TurbArray[ArrayLength];  // Store the average value of the sensor feedback
int TurbArrayIndex = 0;
#define ONE_WIRE_BUS A1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float Suhu;
float pHADC;
float pHADC2;
float pH;
float TurbADC;
float NTU;

const int Push1 = 8;
const int Push2 = 6;
const int Push3 = 4;
const int Push4 = 2;

int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup(void) {
  pinMode(Push1, INPUT);
  pinMode(Push2, INPUT);
  pinMode(Push3, INPUT);
  pinMode(Push4, INPUT);
  Serial.begin(115200);
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Pendeteksi Kualitas Air");
  lcd.setCursor(0, 1);
  lcd.print("1. Higiene Sanitasi");
  lcd.setCursor(0, 2);
  lcd.print("2. Kolam Renang");
  lcd.setCursor(0, 3);
  lcd.print("3. Tambak Udang");
}

void loop() {
  buttonState1 = digitalRead(Push1);
  buttonState2 = digitalRead(Push2);
  buttonState3 = digitalRead(Push3);
  buttonState4 = digitalRead(Push4);

  if (buttonState1 == HIGH) {
    lcd.clear();
    Sanitasi();
  }
  if (buttonState2 == HIGH) {
    lcd.clear();
    Kolam();
  }
  if (buttonState3 == HIGH) {
    lcd.clear();
    Tambak();
  }
}

void Sanitasi() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  while (buttonState1 == HIGH) {
    if (millis() - samplingTime > samplingInterval) {

      pHArray[pHArrayIndex++] = analogRead(pHPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      pHADC = averageArray(pHArray, ArrayLength);
      pHADC = round(pHADC);
if (pHADC < 620) {
      pH = (pHADC * 0.0628) - 28.128;
  }  else {
      pH = (pHADC * -0.0082) + 11.883;
    }
    

      TurbArray[TurbArrayIndex++] = analogRead(TurbPin);
      if (TurbArrayIndex == ArrayLength) TurbArrayIndex = 0;
      TurbADC = averageArray(TurbArray, ArrayLength);
      TurbADC = round(TurbADC / 1) * 1;

      if (TurbADC >= 570) {
        NTU = 0;
      } else {
        NTU = (TurbADC * -1.9521) + 1108.4;
      }
      samplingTime = millis();
    }
    sensors.requestTemperatures();
    Suhu = sensors.getTempCByIndex(0);

    float qualitySanitasi = FuzzySanitasi(pH, Suhu, NTU);
    String qualityResultSanitasi;
    if (qualitySanitasi > 0.6) {
      qualityResultSanitasi = "baik         ";
    } else if (qualitySanitasi > 0.3) {
      qualityResultSanitasi = "kurang baik";
    } else {
      qualityResultSanitasi = "tidak baik  ";
    }

    if (millis() - printTime > printInterval) {  

      Serial.print(pH, 1);
      Serial.print(",");
      Serial.print(Suhu, 2);
      Serial.print(",");
      Serial.print(NTU, 1);
      Serial.print(",");
      Serial.println(qualityResultSanitasi);

      lcd.setCursor(0, 0);
      lcd.print("pH  : ");
      lcd.print(pH, 1);



      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(Suhu, 2);
      lcd.print(" C");


      lcd.setCursor(0, 2);
      lcd.print("Turb: ");
      lcd.print(NTU, 1);


      lcd.setCursor(0, 3);
      lcd.print("Kual: ");
      lcd.print(qualityResultSanitasi);

      printTime = millis();
    }
  }
}


void Kolam() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  while (buttonState2 == HIGH) {
    if (millis() - samplingTime > samplingInterval) {

      pHArray[pHArrayIndex++] = analogRead(pHPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      pHADC = averageArray(pHArray, ArrayLength);
      pHADC = round(pHADC);
if (pHADC < 620) {
      pH = (pHADC * 0.0628) - 28.128;
  }  else {
      pH = (pHADC * -0.0082) + 11.883;
    }

      TurbArray[TurbArrayIndex++] = analogRead(TurbPin);
      if (TurbArrayIndex == ArrayLength) TurbArrayIndex = 0;
      TurbADC = averageArray(TurbArray, ArrayLength);
      TurbADC = round(TurbADC / 1.0) * 1;

      if (TurbADC >= 570) {
        NTU = 0;
      } else {
        NTU = (TurbADC * -1.9521) + 1108.4;
      }
      samplingTime = millis();
    }
    sensors.requestTemperatures();
    Suhu = sensors.getTempCByIndex(0);

    float qualityKolam = FuzzyKolam(pH, Suhu, NTU);
    String qualityResultKolam;
    if (qualityKolam > 0.6) {
      qualityResultKolam = "baik         ";
    } else if (qualityKolam > 0.3) {
      qualityResultKolam = "kurang baik";
    } else {
      qualityResultKolam = "tidak baik  ";
    }

    if (millis() - printTime > printInterval) {  

      Serial.print(pH, 1);
      Serial.print(",");
      Serial.print(Suhu, 2);
      Serial.print(",");
      Serial.print(NTU);
      Serial.print(",");
      Serial.println(qualityResultKolam);

      lcd.setCursor(0, 0);
      lcd.print("pH  : ");
      lcd.print(pH, 1);

      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(Suhu, 2);
      lcd.print(" C");

      lcd.setCursor(0, 2);
      lcd.print("Turb: ");
      lcd.print(NTU, 1);

      lcd.setCursor(0, 3);
      lcd.print("Kual: ");
      lcd.print(qualityResultKolam);

      printTime = millis();
    }
  }
}


void Tambak() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  while (buttonState3 == HIGH) {
    if (millis() - samplingTime > samplingInterval) {


      pHArray[pHArrayIndex++] = analogRead(pHPin);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      pHADC = averageArray(pHArray, ArrayLength);
      pHADC2 = round(pHADC);
if (pHADC < 620) {
      pH = (pHADC * 0.0628) - 28.128;
  }  else {
      pH = (pHADC * -0.0082) + 11.883;
    }

      TurbArray[TurbArrayIndex++] = analogRead(TurbPin);
      if (TurbArrayIndex == ArrayLength) TurbArrayIndex = 0;
      TurbADC = averageArray(TurbArray, ArrayLength);
      TurbADC = round(TurbADC / 1) * 1;

      if (TurbADC >= 570) {
        NTU = 0;
      } else {
        NTU = (TurbADC * -1.9521) + 1108.4;
      }
      samplingTime = millis();
    }
    sensors.requestTemperatures();
    Suhu = sensors.getTempCByIndex(0);

    float qualityTambak = FuzzyTambak(pH, Suhu, NTU);
    String qualityResultTambak;
    if (qualityTambak > 0.6) {
      qualityResultTambak = "baik         ";
    } else if (qualityTambak > 0.3) {
      qualityResultTambak = "kurang baik";
    } else {
      qualityResultTambak = "tidak baik  ";
    }

    if (millis() - printTime > printInterval) {  // Every 800 milliseconds, print a numerical, convert the state of the LED indicator

      Serial.print(pH, 1);
      Serial.print(",");
      Serial.print(Suhu, 1);
      Serial.print(",");
      Serial.print(NTU);
      Serial.print(",");
      Serial.print(qualityResultTambak);

      // Menampilkan nilai pH pada LCD dengan satu angka di belakang koma
      lcd.setCursor(0, 0);
      lcd.print("pH  : ");
      lcd.print(pH, 1);

      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(Suhu, 2);
      lcd.print(" C");

      lcd.setCursor(0, 2);
      lcd.print("Turb: ");
      lcd.print(NTU, 1);

      lcd.setCursor(0, 3);
      lcd.print("Kual: ");
      lcd.print(qualityResultTambak);

      printTime = millis();
    }
  }
}


float membershipFunction(float x, float a, float b, float c, float d) {
  if (x <= a || x >= d) return 0;
  else if (x >= b && x <= c) return 1;
  else if (x > a && x < b) return (x - a) / (b - a);
  else return (d - x) / (d - c);
}

float FuzzySanitasi(float pH, float suhu, float kekeruhan) {
  // Himpunan fuzzy untuk Keasaman
  float asam = membershipFunction(pH, 0, 0, 6, 6.7);
  float netral = membershipFunction(pH, 6.5, 7, 8, 8.5);
  float basa = membershipFunction(pH, 8.3, 9, 14, 14);

  // Himpunan fuzzy untuk Suhu
  float rendah = membershipFunction(suhu, -25, 0, 22, 24);
  float normal = membershipFunction(suhu, 22, 26, 34, 38);
  float tinggi = membershipFunction(suhu, 36, 40, 100, 100);

  // Himpunan fuzzy untuk Kekeruhan
  float jernih = membershipFunction(kekeruhan, -25, 0, 20, 25);
  float sedang = membershipFunction(kekeruhan, 23, 30, 60, 65);
  float keruh = membershipFunction(kekeruhan, 60, 65, 100, 150);

  float tidak_baik = 0, kurang_baik = 0, baik = 0;

  // Aturan fuzzy yang telah ditentukan
  if (asam > 0 && rendah > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && rendah > 0 && sedang > 0) tidak_baik = 1;
  if (asam > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (asam > 0 && normal > 0 && jernih > 0) kurang_baik = 1;
  if (asam > 0 && normal > 0 && sedang > 0) kurang_baik = 1;
  if (asam > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (asam > 0 && tinggi > 0 && jernih > 0) kurang_baik = 1;
  if (asam > 0 && tinggi > 0 && sedang > 0) kurang_baik = 1;
  if (asam > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && rendah > 0 && jernih > 0) kurang_baik = 1;
  if (netral > 0 && rendah > 0 && sedang > 0) kurang_baik = 1;
  if (netral > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && normal > 0 && jernih > 0) baik = 1;
  if (netral > 0 && normal > 0 && sedang > 0) kurang_baik = 1;
  if (netral > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && tinggi > 0 && jernih > 0) baik = 1;
  if (netral > 0 && tinggi > 0 && sedang > 0) kurang_baik = 1;
  if (netral > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && rendah > 0 && jernih > 0) kurang_baik = 1;
  if (basa > 0 && rendah > 0 && sedang > 0) kurang_baik = 1;
  if (basa > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && normal > 0 && jernih > 0) baik = 1;
  if (basa > 0 && normal > 0 && sedang > 0) kurang_baik = 1;
  if (basa > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && tinggi > 0 && jernih > 0) baik = 1;
  if (basa > 0 && tinggi > 0 && sedang > 0) kurang_baik = 1;
  if (basa > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  // Defuzzifikasi dengan menggunakan metode rata-rata terbobot
  float total_membership = baik + kurang_baik + tidak_baik;
  float qualitySanitasi = (baik * 0.9 + kurang_baik * 0.6 + tidak_baik * 0.3) / total_membership;

  return qualitySanitasi;
}

float FuzzyKolam(float pH, float suhu, float kekeruhan) {
  // Himpunan fuzzy untuk Keasaman
  float asam = membershipFunction(pH, 0, 0, 6, 6.9);
  float netral = membershipFunction(pH, 6.8, 7, 7.8, 8);
  float basa = membershipFunction(pH, 7.9, 9, 14, 14);

  // Himpunan fuzzy untuk Suhu
  float rendah = membershipFunction(suhu, -25, 0, 13, 17);
  float normal = membershipFunction(suhu, 16, 20, 35, 40);
  float tinggi = membershipFunction(suhu, 38, 40, 100, 100);

  // Himpunan fuzzy untuk Kekeruhan
  float jernih = membershipFunction(kekeruhan, -25, 0, 3, 5);
  float sedang = membershipFunction(kekeruhan, 4, 7, 40, 45);
  float keruh = membershipFunction(kekeruhan, 40, 45, 100, 150);

  float tidak_baik = 0, kurang_baik = 0, baik = 0;

  // Aturan fuzzy yang telah ditentukan
  if (asam > 0 && rendah > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && rendah > 0 && sedang > 0) tidak_baik = 1;
  if (asam > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (asam > 0 && normal > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && normal > 0 && sedang > 0) tidak_baik = 1;
  if (asam > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (asam > 0 && tinggi > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && tinggi > 0 && sedang > 0) tidak_baik = 1;
  if (asam > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && rendah > 0 && jernih > 0) kurang_baik = 1;
  if (netral > 0 && rendah > 0 && sedang > 0) tidak_baik = 1;
  if (netral > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && normal > 0 && jernih > 0) baik = 1;
  if (netral > 0 && normal > 0 && sedang > 0) tidak_baik = 1;
  if (netral > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && tinggi > 0 && jernih > 0) kurang_baik = 1;
  if (netral > 0 && tinggi > 0 && sedang > 0) tidak_baik = 1;
  if (netral > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && rendah > 0 && jernih > 0) tidak_baik = 1;
  if (basa > 0 && rendah > 0 && sedang > 0) tidak_baik = 1;
  if (basa > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && normal > 0 && jernih > 0) tidak_baik = 1;
  if (basa > 0 && normal > 0 && sedang > 0) tidak_baik = 1;
  if (basa > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && tinggi > 0 && jernih > 0) tidak_baik = 1;
  if (basa > 0 && tinggi > 0 && sedang > 0) tidak_baik = 1;
  if (basa > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  // Defuzzifikasi dengan menggunakan metode rata-rata terbobot
  float total_membership = baik + kurang_baik + tidak_baik;
  float qualityKolam = (baik * 0.9 + kurang_baik * 0.6 + tidak_baik * 0.3) / total_membership;

  return qualityKolam;
}

float FuzzyTambak(float pH, float suhu, float kekeruhan) {
  // Himpunan fuzzy untuk Keasaman
  float asam = membershipFunction(pH, 0, 0, 7, 7.4);
  float netral = membershipFunction(pH, 7.3, 7.5, 8.5, 8.7);
  float basa = membershipFunction(pH, 8.6, 9, 14, 14);

  // Himpunan fuzzy untuk Suhu
  float rendah = membershipFunction(suhu, -25, 0, 20, 24);
  float normal = membershipFunction(suhu, 22, 24, 32, 34);
  float tinggi = membershipFunction(suhu, 32, 35, 100, 100);

  // Himpunan fuzzy untuk Kekeruhan
  float jernih = membershipFunction(kekeruhan, -25, 0, 4, 7);
  float sedang = membershipFunction(kekeruhan, 5, 9, 15, 19);
  float keruh = membershipFunction(kekeruhan, 17, 20, 100, 150);

  float tidak_baik = 0, kurang_baik = 0, baik = 0;

  // Aturan fuzzy yang telah ditentukan
  if (asam > 0 && rendah > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && rendah > 0 && sedang > 0) tidak_baik = 1;
  if (asam > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (asam > 0 && normal > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && normal > 0 && sedang > 0) kurang_baik = 1;
  if (asam > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (asam > 0 && tinggi > 0 && jernih > 0) tidak_baik = 1;
  if (asam > 0 && tinggi > 0 && sedang > 0) tidak_baik = 1;
  if (asam > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && rendah > 0 && jernih > 0) tidak_baik = 1;
  if (netral > 0 && rendah > 0 && sedang > 0) kurang_baik = 1;
  if (netral > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (netral > 0 && normal > 0 && jernih > 0) kurang_baik = 1;
  if (netral > 0 && normal > 0 && sedang > 0) baik = 1;
  if (netral > 0 && normal > 0 && keruh > 0) kurang_baik = 1;

  if (netral > 0 && tinggi > 0 && jernih > 0) tidak_baik = 1;
  if (netral > 0 && tinggi > 0 && sedang > 0) kurang_baik = 1;
  if (netral > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && rendah > 0 && jernih > 0) tidak_baik = 1;
  if (basa > 0 && rendah > 0 && sedang > 0) tidak_baik = 1;
  if (basa > 0 && rendah > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && normal > 0 && jernih > 0) tidak_baik = 1;
  if (basa > 0 && normal > 0 && sedang > 0) kurang_baik = 1;
  if (basa > 0 && normal > 0 && keruh > 0) tidak_baik = 1;

  if (basa > 0 && tinggi > 0 && jernih > 0) tidak_baik = 1;
  if (basa > 0 && tinggi > 0 && sedang > 0) tidak_baik = 1;
  if (basa > 0 && tinggi > 0 && keruh > 0) tidak_baik = 1;

  // Defuzzifikasi dengan menggunakan metode rata-rata terbobot
  float total_membership = baik + kurang_baik + tidak_baik;
  float qualityTambak = (baik * 0.9 + kurang_baik * 0.6 + tidak_baik * 0.3) / total_membership;

  return qualityTambak;
}

double averageArray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;

  if (number <= 0) {
    Serial.println("Error number for the array to averaging!");
    return 0;
  }

  if (number < 5) {  
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0];
      max = arr[1];
    } else {
      min = arr[1];
      max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;  
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  
          max = arr[i];
        } else {
          amount += arr[i];  
        }
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}