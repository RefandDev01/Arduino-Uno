#include <Adafruit_LiquidCrystal.h>

// LCD berbasis MCP23008 (Tinkercad)
Adafruit_LiquidCrystal lcd(0);

const int buzzerPin = 8;

// ===== PENGATURAN TEMPO =====
// Partitur asli: 4/4, tempo 68 (terlalu lambat untuk buzzer). Dipakai 120.
// Ubah angka ini kalau mau lebih cepat/lambat (saran: 100 - 140).
const int BPM = 140;
// Celah diam di akhir tiap nada (ms) supaya setiap nada terdengar terpisah
const int CELAH_MS = 25;
// 1 "unit" = setengah ketukan (not pendek = 1 unit, not biasa = 2 unit)
const unsigned long UNIT_MS = 60000UL / BPM / 2;

// ===== Frekuensi nada (Hz), do = C =====
#define N_C4  262   // 1
#define N_D4  294   // 2
#define N_E4  330   // 3
#define N_F4  349   // 4
#define N_FS4 370   // 4# (fa kres)
#define N_G4  392   // 5
#define N_A4  440   // 6
#define N_B4  494   // 7
#define N_C5  523   // 1'
#define N_D5  587   // 2'
#define R     0     // jeda (diam)

// Kode khusus: ganti tampilan LCD di titik ini (tidak menambah durasi)
#define L1 -1
#define L2 -2
#define L3 -3
#define L4 -4

struct Nada {
  int  f;   // frekuensi (Hz), 0 = jeda, negatif = ganti layar
  byte u;   // durasi dalam unit (1 unit = setengah ketukan)
};

// Setiap baris = 1 birama (4 ketukan = 8 unit), sama seperti partitur
const Nada lagu[] = {
  // Pembuka: 3 ketukan jeda + "Ter"
  {L1, 0}, {R, 6}, {N_G4, 2},
  // Terpujilah wahai engkau ibu bapak guru
  {N_C5, 3}, {N_G4, 1}, {N_G4, 2}, {N_F4, 2},          // pu ji lah wa
  {N_F4, 3}, {N_E4, 1}, {N_E4, 2}, {N_E4, 2},          // hai eng kau i
  {N_F4, 2}, {N_E4, 2}, {N_D4, 2}, {N_C4, 2},          // bu ba pak gu
  {N_D4, 4}, {R, 2}, {N_E4, 1}, {N_F4, 1},             // ru (tahan) | Na ma
  // Namamu akan selalu hidup dalam sanubariku
  {N_G4, 2}, {N_G4, 2}, {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, // mu a kan se la
  {N_A4, 2}, {N_B4, 2}, {N_C5, 2}, {N_A4, 2},          // lu hi dup da
  {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, {N_F4, 2}, {N_D4, 2}, // lam sa nu ba ri
  {N_C4, 4}, {L2, 0}, {R, 2}, {N_C4, 2},               // ku (tahan) | Se
  // Semua baktimu akan kuukir di dalam hatiku
  {N_D4, 2}, {N_D4, 1}, {N_E4, 1}, {N_F4, 2}, {N_G4, 2}, // mua bak ti mu a
  {N_E4, 2}, {N_E4, 1}, {N_F4, 1}, {N_G4, 2}, {N_C4, 2}, // kan ku u kir di
  {N_D4, 2}, {N_D4, 1}, {N_E4, 1}, {N_F4, 2}, {N_A4, 2}, // da lam(2 nada) ha ti
  {N_G4, 4}, {R, 2}, {N_C4, 2},                        // ku (tahan) | S'ba
  // S'bagai prasasti terima kasihku tuk pengabdianmu
  {N_D4, 2}, {N_D4, 1}, {N_E4, 1}, {N_F4, 2}, {N_G4, 2}, // gai pra sas ti t'ri
  {N_E4, 2}, {N_E4, 1}, {N_F4, 1}, {N_G4, 2}, {N_G4, 2}, // ma ka sih ku tuk
  {N_FS4, 2}, {N_A4, 2}, {N_D5, 2}, {N_C5, 2},         // peng ab di an
  {N_B4, 4}, {L3, 0}, {R, 2}, {N_G4, 2},               // mu (tahan) | Eng
  // Engkau sebagai pelita dalam kegelapan
  {N_C5, 3}, {N_G4, 1}, {N_G4, 2}, {N_F4, 2},          // kau se ba gai
  {N_F4, 3}, {N_E4, 1}, {N_E4, 2}, {N_E4, 2},          // pe li ta da
  {N_F4, 2}, {N_E4, 2}, {N_D4, 2}, {N_C4, 2},          // lam ke ge la
  {N_D4, 4}, {R, 2}, {N_E4, 1}, {N_F4, 1},             // pan (tahan) | Eng kau
  // Engkau laksana embun penyejuk dalam kehausan
  {N_G4, 2}, {N_G4, 2}, {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, // lak sa na em bun
  {N_A4, 2}, {N_B4, 2}, {N_C5, 2}, {N_A4, 2},          // pe nye juk da
  {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, {N_F4, 2}, {N_G4, 2}, // lam ke(2 nada) ha us
  {N_E4, 4}, {L4, 0}, {R, 2}, {N_E4, 1}, {N_F4, 1},    // san (tahan) | Eng kau
  // Engkau patriot pahlawan bangsa tanpa tanda jasa
  {N_G4, 2}, {N_G4, 2}, {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, // pa tri ot pah la
  {N_A4, 2}, {N_B4, 2}, {N_C5, 6}, {N_A4, 2},          // wan bang sa(TAHAN) tan
  {N_G4, 2}, {N_F4, 1}, {N_E4, 1}, {N_F4, 2}, {N_D4, 2}, // pa tan da ja(2 nada)
  {N_C4, 6}, {R, 2}                                    // sa (tahan panjang)
};

#define JML_NADA (sizeof(lagu) / sizeof(lagu[0]))

// ===== Tampilan LCD (teks sama seperti sebelumnya) =====
void tulis(int kolom, int baris, const char* teks) {
  lcd.setCursor(kolom, baris);
  lcd.print(teks);
}

void tampilLayar(int nomor) {
  lcd.clear();
  switch (nomor) {
    case 1: tulis(2, 0, "SELAMAT HARI");   tulis(2, 1, "GURU NASIONAL");  break;
    case 2: tulis(1, 0, "Spesial Untuk:"); tulis(2, 1, "Guru-Guru TKJ");  break;
    case 3: tulis(2, 0, "Terima Kasih");   tulis(1, 1, "Atas Ilmunya!");  break;
    case 4: tulis(1, 0, "Kami Bangga! <3"); tulis(1, 1, "- Keluarga TKJ"); break;
  }
}

// ===== Pemutar lagu =====
// Musik jadi "jam utama". Layar diganti tepat di titik jeda,
// dan waktunya otomatis dikejar supaya irama tidak ikut melambat.
void mainkanLagu() {
  unsigned long target = millis();
  for (unsigned int i = 0; i < JML_NADA; i++) {
    int f = lagu[i].f;
    if (f < 0) {            // kode ganti layar
      tampilLayar(-f);
      continue;
    }
    unsigned long durasi = lagu[i].u * UNIT_MS;
    if (f > 0) tone(buzzerPin, f, durasi - CELAH_MS);  // bunyi, lalu celah 40 ms antar nada
    target += durasi;
    while ((long)(millis() - target) < 0) { }           // tunggu sampai nada selesai
  }
  noTone(buzzerPin);
}

void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  mainkanLagu();
  delay(2500);   // jeda sebelum lagu diulang
}
