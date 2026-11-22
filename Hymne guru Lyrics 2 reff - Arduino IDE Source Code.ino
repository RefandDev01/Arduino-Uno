#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // install: "LiquidCrystal I2C" (Frank de Brabander)

// ===== LCD 16x2 I2C (chip PCF8574) =====
// Alamat umum: 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Buzzer =====
const int buzzerPin = 8;   // kaki + buzzer
const int groundPin = 6;   // kaki - buzzer (dibuat LOW sebagai pengganti GND)

// ===== Pengaturan tempo =====
const int BPM = 120;
const int CELAH_MS = 40;
const unsigned long UNIT_MS = 60000UL / BPM / 2;

// ===== Frekuensi nada =====
#define N_C4  262
#define N_D4  294
#define N_E4  330
#define N_F4  349
#define N_FS4 370
#define N_G4  392
#define N_A4  440
#define N_B4  494
#define N_C5  523
#define N_D5  587
#define R     0

// Kode khusus: ganti tampilan LCD di titik ini (tidak menambah durasi)
#define L1 -1
#define L2 -2
#define L3 -3
#define L4 -4

struct Nada {
  int f;
  byte u;
};

// Partitur Hari Guru Nasional (TIDAK DIUBAH SAMA SEKALI)
const Nada lagu[] = {
  {L1, 0}, {R, 6}, {N_G4, 2},

  // Terpujilah wahai engkau ibu bapak guru
  {N_C5, 3}, {N_G4, 1}, {N_G4, 2}, {N_F4, 2},
  {N_F4, 3}, {N_E4, 1}, {N_E4, 2}, {N_E4, 2},
  {N_F4, 2}, {N_E4, 2}, {N_D4, 2}, {N_C4, 2},
  {N_D4, 4}, {R, 2}, {N_E4, 1}, {N_F4, 1},

  // Namamu akan selalu hidup dalam sanubariku
  {N_G4, 2}, {N_G4, 2}, {N_G4, 2}, {N_F4, 1}, {N_E4, 1},
  {N_A4, 2}, {N_B4, 2}, {N_C5, 2}, {N_A4, 2},
  {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, {N_F4, 2}, {N_D4, 2},
  {N_C4, 4}, {L2, 0}, {R, 2}, {N_C4, 2},

  // Semua baktimu akan kuukir di dalam hatiku
  {N_D4, 2}, {N_D4, 1}, {N_E4, 1}, {N_F4, 2}, {N_G4, 2},
  {N_E4, 2}, {N_E4, 1}, {N_F4, 1}, {N_G4, 2}, {N_C4, 2},
  {N_D4, 2}, {N_D4, 1}, {N_E4, 1}, {N_F4, 2}, {N_A4, 2},
  {N_G4, 4}, {R, 2}, {N_C4, 2},

  // S'bagai prasasti terima kasihku tuk pengabdianmu
  {N_D4, 2}, {N_D4, 1}, {N_E4, 1}, {N_F4, 2}, {N_G4, 2},
  {N_E4, 2}, {N_E4, 1}, {N_F4, 1}, {N_G4, 2}, {N_G4, 2},
  {N_FS4, 2}, {N_A4, 2}, {N_D5, 2}, {N_C5, 2},
  {N_B4, 4}, {L3, 0}, {R, 2}, {N_G4, 2},

  // Engkau sebagai pelita dalam kegelapan
  {N_C5, 3}, {N_G4, 1}, {N_G4, 2}, {N_F4, 2},
  {N_F4, 3}, {N_E4, 1}, {N_E4, 2}, {N_E4, 2},
  {N_F4, 2}, {N_E4, 2}, {N_D4, 2}, {N_C4, 2},
  {N_D4, 4}, {R, 2}, {N_E4, 1}, {N_F4, 1},

  // Engkau laksana embun penyejuk dalam kehausan
  {N_G4, 2}, {N_G4, 2}, {N_G4, 2}, {N_F4, 1}, {N_E4, 1},
  {N_A4, 2}, {N_B4, 2}, {N_C5, 2}, {N_A4, 2},
  {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, {N_F4, 2}, {N_G4, 2},
  {N_E4, 4}, {L4, 0}, {R, 2}, {N_E4, 1}, {N_F4, 1},

  // Engkau patriot pahlawan bangsa tanpa tanda jasa
  {N_G4, 2}, {N_G4, 2}, {N_G4, 2}, {N_F4, 1}, {N_E4, 1},
  {N_A4, 2}, {N_B4, 2}, {N_C5, 6}, {N_A4, 2},
  {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, {N_F4, 2}, {N_D4, 2},
  {N_C4, 6}, {R, 2}
};

#define JML_NADA (sizeof(lagu) / sizeof(lagu[0]))

// ===== Tampilan LCD =====
void tulis(int kolom, int baris, const char* teks) {
  lcd.setCursor(kolom, baris);
  lcd.print(teks);
}

// Teks Ucapan Utama (TIDAK DIUBAH SAMA SEKALI)
void tampilLayar(int nomor) {
  lcd.clear();
  switch (nomor) {
    case 1: tulis(2, 0, "SELAMAT HARI");    tulis(2, 1, "GURU NASIONAL");   break;
    case 2: tulis(1, 0, "Spesial Untuk:");  tulis(2, 1, "Guru-Guru TKJ");   break;
    case 3: tulis(2, 0, "Terima Kasih");    tulis(1, 1, "Atas Ilmunya!");   break;
    case 4: tulis(1, 0, "Kami Bangga! <3"); tulis(1, 1, "- Keluarga TKJ");  break;
  }
}

// Fungsi bantu cetak 2 baris lirik sekaligus
void cetakLirik(int kol1, const char* baris1, int kol2, const char* baris2) {
  lcd.clear();
  tulis(kol1, 0, baris1);
  tulis(kol2, 1, baris2);
}

// ===== Mapping Indeks Nada (i) ke Lirik Lagu =====
void tampilLirik(unsigned int i) {
  switch (i) {
    case 0:   cetakLirik(1, "~ HYMNE GURU ~",   2, "(Mode Lirik)");      break;

    // 1. Terpujilah wahai engkau ibu bapak guru
    case 2:   cetakLirik(3, "Terpujilah",       2, "wahai engkau");      break;
    case 10:  cetakLirik(3, "ibu bapak",        5, "guru...");           break;

    // 2. Namamu akan selalu hidup dalam sanubariku
    case 17:  cetakLirik(2, "Namamu akan",      2, "selalu hidup");      break;
    case 27:  cetakLirik(5, "dalam",            2, "sanubariku...");     break;

    // 3. Semua baktimu akan kuukir di dalam hatiku
    case 36:  cetakLirik(1, "Semua baktimu",    2, "akan kuukir");       break;
    case 47:  cetakLirik(4, "di dalam",         4, "hatiku...");         break;

    // 4. S'bagai prasasti terima kasihku tuk pengabdianmu
    case 54:  cetakLirik(0, "S'bagai prasasti", 1, "terima kasihku");    break;
    case 65:  cetakLirik(6, "tuk",              1, "pengabdianmu..");    break;

    // 5. Engkau sebagai pelita dalam kegelapan
    case 72:  cetakLirik(1, "Engkau sebagai",   5, "pelita");            break;
    case 80:  cetakLirik(5, "dalam",            2, "kegelapan...");      break;

    // 6. Engkau laksana embun penyejuk dalam kehausan
    case 87:  cetakLirik(1, "Engkau laksana",   1, "embun penyejuk");    break;
    case 97:  cetakLirik(5, "dalam",            3, "kehausan...");       break;

    // 7. Engkau patriot pahlawan bangsa tanpa tanda jasa
    case 106: cetakLirik(1, "Engkau patriot",   0, "pahlawan bangsa");   break;
    case 116: cetakLirik(2, "tanpa tanda",      5, "jasa...");           break;
  }
}

// ===== Pemutar lagu (modeLirik: false = Ucapan, true = Lirik) =====
void mainkanLagu(bool modeLirik) {
  unsigned long target = millis();

  for (unsigned int i = 0; i < JML_NADA; i++) {
    int f = lagu[i].f;

    if (modeLirik) {
      tampilLirik(i);         // Putaran 2: cek apakah indeks nada ini punya lirik
      if (f < 0) continue;    // Abaikan kode L1-L4 agar tidak menimpa lirik
    } else {
      if (f < 0) {            // Putaran 1: tampilkan ucapan L1-L4 seperti semula
        tampilLayar(-f);
        continue;
      }
    }

    unsigned long durasi = lagu[i].u * UNIT_MS;

    if (f > 0 && durasi > CELAH_MS) {
      tone(buzzerPin, f, durasi - CELAH_MS);
    } else {
      noTone(buzzerPin);
    }

    target += durasi;

    while ((long)(millis() - target) < 0) {
    }
  }

  noTone(buzzerPin);
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(groundPin, OUTPUT);
  digitalWrite(groundPin, LOW);   // pin 6 jadi "GND" buzzer

  lcd.init();
  lcd.backlight();
}

void loop() {
  // Putaran 1: Melodi + Tampilan Ucapan (L1 s/d L4)
  mainkanLagu(false);
  delay(2000);

  // Putaran 2: Melodi + Tampilan Lirik Sinkron
  mainkanLagu(true);
  delay(2500);
}
