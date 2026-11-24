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

// ===== Animasi Baris Atas ("~ HYMNE GURU ~" -> "X TKJ 1" -> Jeda 1.5s -> Normal) =====
const char* teksAnimAtas = " ~ HYMNE GURU ~       X TKJ 1";
const int SPEED_ATAS_MS = 180;
const int JEDA_ATAS_MS  = 1500; // jeda 1,5 detik setelah "X TKJ 1" menghilang di kiri

int statusAtas = 0; // 0 = diam, 1 = running ke kiri, 2 = jeda kosong sebelum muncul lagi
int posAtas = 0;
int panjangAnimAtas = 0;
unsigned long waktuGeserAtas = 0;

void mulaiAnimasiAtas() {
  panjangAnimAtas = strlen(teksAnimAtas);
  posAtas = 0;
  statusAtas = 1;
  waktuGeserAtas = millis() + SPEED_ATAS_MS;
}

void updateAnimasiAtas() {
  if (statusAtas == 1) {
    if ((long)(millis() - waktuGeserAtas) >= 0) {
      posAtas++;
      char jendelaAtas[17];
      for (int k = 0; k < 16; k++) {
        int idx = posAtas + k;
        if (idx >= 0 && idx < panjangAnimAtas) {
          jendelaAtas[k] = teksAnimAtas[idx];
        } else {
          jendelaAtas[k] = ' ';
        }
      }
      jendelaAtas[16] = '\0';
      tulis(0, 0, jendelaAtas);

      // Jika "X TKJ 1" sudah sepenuhnya hilang di sebelah kiri
      if (posAtas >= panjangAnimAtas) {
        statusAtas = 2;
        waktuGeserAtas = millis() + JEDA_ATAS_MS;
      } else {
        waktuGeserAtas += SPEED_ATAS_MS;
      }
    }
  } else if (statusAtas == 2) {
    // Setelah jeda 1,5 detik selesai, tampilkan kembali "~ HYMNE GURU ~" secara normal
    if ((long)(millis() - waktuGeserAtas) >= 0) {
      tulis(0, 0, " ~ HYMNE GURU ~ ");
      statusAtas = 0;
    }
  }
}

// ===== Variabel & Fungsi Sliding Window Lirik Baris Bawah =====
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

// Mapping indeks nada awal tiap bait ke kalimat lirik
void cekJadwalLirik(unsigned int i) {
  switch (i) {
    case 0:
      lcd.clear();
      statusAtas = 0;
      tulis(1, 0, "~ HYMNE GURU ~");
      setKalimatLirik("  (Mode Lirik)  ", 0, 190);
      break;

    case 2:
      setKalimatLirik("Terpujilah wahai engkau ibu bapak guru", POS_MULAI_KANAN, 200);
      break;

    case 17:
      // Mulai animasi baris atas ("~ HYMNE GURU ~" jalan ke kiri disusul "X TKJ 1")
      mulaiAnimasiAtas();
      setKalimatLirik("Namamu akan selalu hidup dalam sanubariku", POS_MULAI_KANAN, 190);
      break;

    case 36:
      setKalimatLirik("Semua baktimu akan kuukir di dalam hatiku", POS_MULAI_KANAN, 190);
      break;

    case 54:
      setKalimatLirik("S'bagai prasasti terima kasihku tuk pengabdianmu", POS_MULAI_KANAN, 160);
      break;

    case 72:
      setKalimatLirik("Engkau sebagai pelita dalam kegelapan", POS_MULAI_KANAN, 205);
      break;

    case 87:
      setKalimatLirik("Engkau laksana embun penyejuk dalam kehausan", POS_MULAI_KANAN, 175);
      break;

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

// ===== Fungsi Cek Saklar OFF Secara Real-Time =====
bool cekSaklarMati() {
  if (digitalRead(switchPin) == HIGH) { // HIGH = Rocker Switch berada di posisi OFF
    noTone(buzzerPin);
    lcd.clear();
    lcd.noBacklight();
    statusAtas = 0;
    sudahMulai = false;
    return true;
  }
  return false;
}

// Fungsi pengganti delay() biasa agar tetap langsung mati jika saklar di-OFF-kan saat jeda
bool jedaDenganCekSaklar(unsigned long durasiMs) {
  unsigned long batas = millis() + durasiMs;
  while ((long)(millis() - batas) < 0) {
    if (cekSaklarMati()) return false;
    delay(10);
  }
  return true;
}

// ===== Pemutar lagu (modeLirik: false = Ucapan, true = Lirik Running Text) =====
void mainkanLagu(bool modeLirik) {
  unsigned long target = millis();

  for (unsigned int i = 0; i < JML_NADA; i++) {
    if (cekSaklarMati()) return; // Langsung berhenti jika saklar di-OFF-kan

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

    // Sambil menunggu durasi nada selesai, cek saklar & geser running text di latar belakang
    while ((long)(millis() - target) < 0) {
      if (cekSaklarMati()) return;
      if (modeLirik) {
        updateRunningText();
        updateAnimasiAtas();
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
  if (!jedaDenganCekSaklar(2000)) return;

  // Putaran 2: Melodi + Running Text Lirik & Animasi "X TKJ 1"
  mainkanLagu(true);
  if (!sudahMulai) return;

  // Tampilan penutup saat jeda akhir sebelum mengulang ke awal
  lcd.clear();
  tulis(1, 0, "Sehat Selalu,");
  tulis(2, 1, "Guru-Guruku!");
  jedaDenganCekSaklar(2500);
}
