#include <Servo.h>

// Sistem Bilgileri
#define CURRENT_DATE "2025-05-26 06:20:28"
#define CURRENT_USER "Bunyaminoksuz"

// Motor kontrol pinleri
#define IN1 6  // Sol motor ileri/geri
#define IN2 7  // Sol motor ileri/geri
#define IN3 8  // Sağ motor ileri/geri
#define IN4 9  // Sağ motor ileri/geri

// Röle pini
#define RELAY_PIN 10

// Nem sensörü pini
#define MOISTURE_PIN A0

// HC-SR04 pinleri
#define TRIG_PIN 11
#define ECHO_PIN 12

// Servo motor pini
#define SERVO_PIN 5

// Nem eşik değerleri
#define THRESHOLD_KURU 700
#define THRESHOLD_NEMLI 300

// Mesafe eşik değeri (15 cm)
#define MESAFE_ESIK 15

// Diğer parametreler
#define HAREKET_SURESI 25000    // 25 saniye hareket - GÜNCELLENDİ
#define SERVO_HAREKET_HIZI 50   // Servo hareket hızı
#define SULAMA_SURESI 18000     // 18 saniye sulama - GÜNCELLENDİ
#define YON_DEGISIM_SURESI 10000 // 10 saniyede bir yön değiştirme
#define DONUS_SURESI 800        // Dönüş süresi (milisaniye)

// Servo motor nesnesi
Servo servo;

// Durum değişkenleri
enum SistemDurumu {
  HAREKET_EDIYOR,
  OLCUM_YAPIYOR,
  SULAMA_YAPIYOR
};

// Hareket durumları
enum HareketDurumu {
  ILERI_GIT,
  DONUS_YAP,
  YON_DEGISTIR
};

SistemDurumu simdikiDurum = HAREKET_EDIYOR;
HareketDurumu hareketDurumu = ILERI_GIT;
unsigned long durumBaslangicZamani = 0;
unsigned long sonYonDegisimZamani = 0;
unsigned long hareketZamani = 0;

void setup() {
  // Seri haberleşmeyi başlat
  Serial.begin(9600);
  
  // Sistem ve kullanıcı bilgilerini görüntüle
  Serial.println("\n=== Sulama ve Duzgun Gezinme Sistemi ===");
  Serial.print("Tarih/Saat: ");
  Serial.println(CURRENT_DATE);
  Serial.print("Kullanici: ");
  Serial.println(CURRENT_USER);
  Serial.println("=======================================");

  // Motor pinlerini çıkış olarak ayarla
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Röle pinini çıkış olarak ayarla
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Röle başlangıçta kapalı
  
  // Nem sensör pini giriş olarak ayarlandı
  pinMode(MOISTURE_PIN, INPUT);

  // HC-SR04 pinlerini ayarla
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo motoru tanımla
  servo.attach(SERVO_PIN);
  servo.write(0); // Servo başlangıç pozisyonu (0 derece)
  delay(1000);
  
  // Rastgele sayı üreteci için seed
  randomSeed(analogRead(A5));
  
  Serial.println("Sistem hazir! Duzgun hareket modu aktif.");
  durumBaslangicZamani = millis();
  sonYonDegisimZamani = millis();
  hareketZamani = millis();
}

void loop() {
  // Durum makinesine göre işlemleri yönet
  switch (simdikiDurum) {
    case HAREKET_EDIYOR:
      hareketEt();
      break;
      
    case OLCUM_YAPIYOR:
      nemOlcumuYap();
      break;
      
    case SULAMA_YAPIYOR:
      sulamaYap();
      break;
  }
}

// Hareket durumunu yöneten fonksiyon
void hareketEt() {
  unsigned long simdikiZaman = millis();
  
  // İlk kez hareket durumuna girdiğinde
  if (durumBaslangicZamani == 0) {
    Serial.println("\n--- HAREKET DURUMU ---");
    durumBaslangicZamani = simdikiZaman;
    hareketDurumu = ILERI_GIT;
    hareketZamani = simdikiZaman;
    ileriHareket();
  }
  
  // Engel tespiti ve kaçma
  int mesafe = mesafeOlc();
  if (mesafe <= MESAFE_ESIK) {
    motorDur();
    delay(200);
    Serial.println("ENGEL ALGILANDI! Yön değiştiriliyor...");
    
    // Rasgele sağa veya sola dön
    if (random(2) == 0) {
      Serial.println("Engelden kaçmak için sağa dönülüyor...");
      sagaDon();
    } else {
      Serial.println("Engelden kaçmak için sola dönülüyor...");
      solaDon();
    }
    
    delay(DONUS_SURESI); // Belirli süre dön
    motorDur();
    delay(100);
    ileriHareket(); // Düz ileri git
    hareketDurumu = ILERI_GIT;
    hareketZamani = simdikiZaman;
  }
  
  // Düzenli rastgele yön değişimi
  if (simdikiZaman - sonYonDegisimZamani >= YON_DEGISIM_SURESI && hareketDurumu == ILERI_GIT) {
    motorDur();
    delay(200);
    
    Serial.println("Rastgele yön değişimi yapılıyor...");
    hareketDurumu = YON_DEGISTIR;
    
    // Rastgele sağa veya sola dön
    if (random(2) == 0) {
      Serial.println("Sağa dönülüyor...");
      sagaDon();
    } else {
      Serial.println("Sola dönülüyor...");
      solaDon();
    }
    
    delay(random(DONUS_SURESI, DONUS_SURESI * 2)); // Rastgele süre dön
    motorDur();
    delay(100);
    ileriHareket(); // Düz ileri git
    
    sonYonDegisimZamani = simdikiZaman;
    hareketDurumu = ILERI_GIT;
    hareketZamani = simdikiZaman;
  }
  
  // Belirli süre hareket ettikten sonra ölçüm durumuna geç
  if (simdikiZaman - durumBaslangicZamani >= HAREKET_SURESI) {
    motorDur();
    Serial.println("Hareket tamamlandi, olcum yapilacak.");
    
    // Ölçüm durumuna geç
    simdikiDurum = OLCUM_YAPIYOR;
    durumBaslangicZamani = 0; // Sıfırla, yeni durumda tekrar başlatılacak
  }
}

// Nem ölçümü yapan fonksiyon
void nemOlcumuYap() {
  // İlk kez ölçüm durumuna girdiğinde
  if (durumBaslangicZamani == 0) {
    Serial.println("\n--- OLCUM DURUMU ---");
    durumBaslangicZamani = millis();
    
    // 1. ADIM: Servo motoru 90 dereceye getir (sensörü toprağa batır)
    Serial.println("ADIM 1: Servo motor sensoru topraga batiriyor");
    servoAsagi();
    
    // 2. ADIM: Nem ölçümü yap
    Serial.println("ADIM 2: Toprak nem olcumu yapiliyor");
    int nemDegeri = nemOlcumu();
    String nemDurumu = nemDurumunuBelirle(nemDegeri);
    
    // 3. ADIM: Servo motoru başlangıç pozisyonuna getir
    Serial.println("ADIM 3: Servo motor sensor basi pozisyonuna getiriliyor");
    servoYukari();
    
    // 4. ADIM: Nem durumuna göre işlem belirle
    if (nemDurumu == "KURU") {
      Serial.println("ADIM 4: Toprak KURU! Sulama yapilacak.");
      simdikiDurum = SULAMA_YAPIYOR;
    } else {
      Serial.print("ADIM 4: Toprak ");
      Serial.print(nemDurumu);
      Serial.println(". Sulamaya gerek yok.");
      simdikiDurum = HAREKET_EDIYOR;
    }
    
    durumBaslangicZamani = 0; // Sıfırla, yeni durumda tekrar başlatılacak
  }
}

// Sulama işlemini yapan fonksiyon
void sulamaYap() {
  // İlk kez sulama durumuna girdiğinde
  if (durumBaslangicZamani == 0) {
    Serial.println("\n--- SULAMA DURUMU ---");
    durumBaslangicZamani = millis();
    
    // Röleyi aç, sulama başlat
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Sulama basladi...");
  }
  
  unsigned long simdikiZaman = millis();
  unsigned long gecenSure = simdikiZaman - durumBaslangicZamani;
  
  // Sulama süresince kalan süreyi göster
  if (gecenSure % 1000 == 0) { // Her saniye
    int kalanSaniye = (SULAMA_SURESI - gecenSure) / 1000;
    if (kalanSaniye >= 0) {
      Serial.print("Sulama devam ediyor: ");
      Serial.print(kalanSaniye);
      Serial.println(" saniye kaldi");
    }
  }
  
  // Sulama süresi dolduğunda
  if (gecenSure >= SULAMA_SURESI) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Sulama tamamlandi.");
    
    // Hareket durumuna geç
    simdikiDurum = HAREKET_EDIYOR;
    durumBaslangicZamani = 0; // Sıfırla, yeni durumda tekrar başlatılacak
  }
}

// Topraktan nem ölçümü yapan fonksiyon
int nemOlcumu() {
  // 3 ölçüm alıp ortalamasını hesapla (daha kararlı ölçüm için)
  int toplamNem = 0;
  
  for (int i = 0; i < 3; i++) {
    int hamNemDegeri = analogRead(MOISTURE_PIN);
    toplamNem += hamNemDegeri;
    
    Serial.print("  Olcum #");
    Serial.print(i+1);
    Serial.print(": Nem Degeri = ");
    Serial.println(hamNemDegeri);
    
    delay(500); // Ölçümler arası kısa bir bekleme
  }
  
  int ortNemDegeri = toplamNem / 3;
  
  String durum = nemDurumunuBelirle(ortNemDegeri);
  Serial.print("  Ortalama Nem Degeri: ");
  Serial.print(ortNemDegeri);
  Serial.print(" --> Toprak Durumu: ");
  Serial.println(durum);
  
  return ortNemDegeri;
}

// Servo motoru 90 dereceye indiren fonksiyon (sensörü toprağa batır)
void servoAsagi() {
  for (int aci = 0; aci <= 90; aci++) {
    servo.write(aci);
    delay(SERVO_HAREKET_HIZI);
  }
  delay(1000); // Sensörün toprağa yerleşmesi için bekle
}

// Servo motoru 0 dereceye çıkaran fonksiyon (sensörü topraktan çıkar)
void servoYukari() {
  for (int aci = 90; aci >= 0; aci--) {
    servo.write(aci);
    delay(SERVO_HAREKET_HIZI);
  }
}

// Mesafe ölçümü yapan fonksiyon
int mesafeOlc() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long sure = pulseIn(ECHO_PIN, HIGH);
  int mesafe = sure * 0.034 / 2;
  
  // 400 cm'den uzun mesafeleri filtrele (sensör sınırı)
  if (mesafe > 400 || mesafe <= 0) {
    return 400;
  }
  
  return mesafe;
}

// Nem değerine göre toprak durumunu belirleyen fonksiyon
String nemDurumunuBelirle(int nemDegeri) {
  if (nemDegeri > THRESHOLD_KURU) {
    return "KURU";
  } else if (nemDegeri > THRESHOLD_NEMLI) {
    return "NEMLI";
  } else {
    return "YAS";
  }
}

// MOTOR KONTROL FONKSİYONLARI //

// İleri hareket için her iki motor da ileri yönde çalışmalı
void ileriHareket() {
  // Sol motor ileri
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Sağ motor ileri
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  Serial.println("Motorlar ileri hareket ediyor...");
}

// Sağa dönüş için sol motor ileri, sağ motor geri gitmeli
void sagaDon() {
  // Sol motor ileri
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Sağ motor geri
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  
  Serial.println("Motorlar saga donuyor...");
}

// Sola dönüş için sol motor geri, sağ motor ileri gitmeli
void solaDon() {
  // Sol motor geri
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  // Sağ motor ileri
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  Serial.println("Motorlar sola donuyor...");
}

// Motorları durdur
void motorDur() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Motorlar durduruldu.");
}