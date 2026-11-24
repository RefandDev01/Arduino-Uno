#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // install: "LiquidCrystal I2C" (Frank de Brabander)
#include <string.h>

// ===== LCD 16x2 I2C (chip PCF8574) =====
// Alamat umum: 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Buzzer =====
const int buzzerPin = 8;   // kaki + buzzer
const int groundPin = 6;   // kaki - buzzer (dibuat LOW sebagai pengganti GND)

// ===== Pin Switch & Push Button =====
const int switchPin = 4;   // kaki rocker switch (kabel satunya ke GND)
const int buttonPin = 10;  // kaki push button hijau di Pin 10 (kabel satunya ke GND)
bool sudahMulai = false;   // penanda apakah tombol hijau sudah ditekan

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

// ===== Variabel & Fungsi Sliding Window (Muncul dari Kanan ke Kiri) =====
const int POS_MULAI_KANAN = -10; // posisi awal muncul dari sisi kanan layar

const char* lirikAktif = "";
int posScroll = 0;
int panjangLirik = 0;
unsigned long waktuGeser = 0;

int speedScrollAktif = 195;
int posisiJedaTengah = -99;
int durasiJedaTengah = 0;

// Cetak potongan 16 karakter ke baris bawah (baris 1)
void cetakPotonganLirik() {
  char jendela[17];
  for (int k = 0; k < 16; k++) {
    int idx = posScroll + k;
    if (idx >= 0 && idx < panjangLirik) {
      jendela[k] = lirikAktif[idx];
    } else {
      jendela[k] = ' ';
    }
  }
  jendela[16] = '\0';
  tulis(0, 1, jendela);
}

// Atur kalimat lirik baru untuk langsung berjalan dari kanan
void setKalimatLirik(const char* teksBaru, int posAwal, int speedMs, int titikJeda = -99, int lamaJedaMs = 0) {
  lirikAktif = teksBaru;
  panjangLirik = strlen(teksBaru);
  posScroll = posAwal;
  speedScrollAktif = speedMs;
  posisiJedaTengah = titikJeda;
  durasiJedaTengah = lamaJedaMs;

  cetakPotonganLirik();
  waktuGeser = millis() + speedMs;
}

// Mapping indeks nada awal tiap bait ke kalimat lirik (delay sudah dipercepat 15-20 ms)
void cekJadwalLirik(unsigned int i) {
  switch (i) {
    case 0:
      lcd.clear();
      tulis(1, 0, "~ HYMNE GURU ~");
      setKalimatLirik("  (Mode Lirik)  ", 0, 190);
      break;

    case 2:   // dikurangi dari 220 -> 200 ms
      setKalimatLirik("Terpujilah wahai engkau ibu bapak guru", POS_MULAI_KANAN, 200);
      break;

    case 17:  // dikurangi dari 205 -> 190 ms
      setKalimatLirik("Namamu akan selalu hidup dalam sanubariku", POS_MULAI_KANAN, 190);
      break;

    case 36:  // dikurangi dari 205 -> 190 ms
      setKalimatLirik("Semua baktimu akan kuukir di dalam hatiku", POS_MULAI_KANAN, 190);
      break;

    case 54:  // dikurangi dari 175 -> 160 ms
      setKalimatLirik("S'bagai prasasti terima kasihku tuk pengabdianmu", POS_MULAI_KANAN, 160);
      break;

    case 72:  // dikurangi dari 225 -> 205 ms
      setKalimatLirik("Engkau sebagai pelita dalam kegelapan", POS_MULAI_KANAN, 205);
      break;

    case 87:  // dikurangi dari 190 -> 175 ms
      setKalimatLirik("Engkau laksana embun penyejuk dalam kehausan", POS_MULAI_KANAN, 175);
      break;

    // Speed dikurangi dari 170 -> 155 ms, dan jeda "pahlawan bangsa" dikunci di 1150 ms
    case 106:
      setKalimatLirik("Engkau patriot pahlawan bangsa tanpa tanda jasa", POS_MULAI_KANAN, 155, 15, 1150);
      break;
  }
}

// Geser jendela 1 huruf ke kiri secara berkala
void updateRunningText() {
  if (posScroll + 16 < panjangLirik) {
    if ((long)(millis() - waktuGeser) >= 0) {
      posScroll++;
      cetakPotonganLirik();

      // Berhenti sejenak saat mencapai titik jeda tengah ("pahlawan bangsa ")
      if (posScroll == posisiJedaTengah) {
        waktuGeser += durasiJedaTengah;
      } else {
        waktuGeser += speedScrollAktif;
      }
    }
  }
}

// ===== Pemutar lagu (modeLirik: false = Ucapan, true = Lirik Running Text) =====
void mainkanLagu(bool modeLirik) {
  unsigned long target = millis();

  for (unsigned int i = 0; i < JML_NADA; i++) {
    int f = lagu[i].f;

    if (modeLirik) {
      cekJadwalLirik(i);      // Putaran 2: muat kalimat lirik sesuai indeks bait
      if (f < 0) continue;    // Abaikan kode L1-L4 agar tidak menghapus layar lirik
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

    // Sambil menunggu durasi nada selesai, geser running text di latar belakang
    while ((long)(millis() - target) < 0) {
      if (modeLirik) {
        updateRunningText();
      }
    }
  }

  noTone(buzzerPin);
}

// ===== Fungsi Tunggu Switch ON & Tombol Ditekan =====
void tungguTombolMulai() {
  bool layarSiap = false;

  while (!sudahMulai) {
    // Jika Rocker Switch posisi ON (terhubung ke GND -> bernilai LOW)
    if (digitalRead(switchPin) == LOW) {
      if (!layarSiap) {
        lcd.backlight();
        lcd.clear();
        tulis(0, 0, "Tekan tombol");       // Rata kiri (kolom 0, baris 0)
        tulis(0, 1, "untuk memulai...");   // Rata kiri (kolom 0, baris 1)
        layarSiap = true;
      }

      // Cek apakah Push Button Hijau di Pin 10 ditekan (LOW)
      if (digitalRead(buttonPin) == LOW) {
        delay(50); // anti-bounce singkat
        if (digitalRead(buttonPin) == LOW) {
          sudahMulai = true;
        }
      }
    } else {
      // Jika Rocker Switch posisi OFF (terputus -> bernilai HIGH)
      if (layarSiap) {
        lcd.clear();
        lcd.noBacklight();
        layarSiap = false;
      }
    }
    delay(20);
  }
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(groundPin, OUTPUT);
  digitalWrite(groundPin, LOW);   // pin 6 jadi "GND" buzzer

  pinMode(switchPin, INPUT_PULLUP); // aktifkan resistor internal untuk switch (Pin 4)
  pinMode(buttonPin, INPUT_PULLUP); // aktifkan resistor internal untuk push button (Pin 10)

  lcd.init();
  lcd.noBacklight(); // awalnya layar mati sampai switch di-ON-kan
}

void loop() {
  // Tunggu sampai Switch di-ON-kan dan Push Button ditekan
  if (!sudahMulai) {
    tungguTombolMulai();
  }

  // Putaran 1: Melodi + Tampilan Ucapan Asli
  mainkanLagu(false);
  delay(2000);

  // Putaran 2: Melodi + Running Text Lirik (Muncul dari Kanan ke Kiri)
  mainkanLagu(true);

  // Tampilan penutup saat jeda akhir sebelum mengulang ke awal
  lcd.clear();
  tulis(1, 0, "Sehat Selalu,");
  tulis(2, 1, "Guru-Guruku!");
  delay(2500);
}
