# Aplikasi Kasir

Aplikasi Kasir ini dirancang untuk membantu pengelolaan transaksi pada toko kecil. Dibangun menggunakan **C++** dengan library **wxWidgets** untuk antarmuka pengguna, dan **SQLite** sebagai database lokal untuk menyimpan data barang serta riwayat transaksi.

---

## Fitur Utama
- **Manajemen Barang**: Menambahkan, mencari, dan memvalidasi data barang langsung dari database.
- **Keranjang Belanja**: Fitur untuk menambah barang ke keranjang belanja.
- **Proses Transaksi**: Menghitung total belanja, menerima pembayaran, menghitung kembalian, dan menyimpan log transaksi.
- **Cetak Struk**: Menyimpan struk ke file dan mencetaknya.
- **Database Terintegrasi**: Menggunakan SQLite untuk menyimpan data barang dan log transaksi.
- **Antarmuka Pengguna**: Antarmuka yang sederhana dan mudah digunakan dengan layout yang responsif.
- **Input Data**: Bisa Input Data dengan Aplikasi Terpisah
  
---

## Persyaratan Sistem
- **Sistem Operasi**: Windows, macOS, atau Linux.
- **Compiler**: GCC/Clang/MinGW (C++11 ke atas).
- **Library**: 
  - wxWidgets
  - SQLite3
  - CUPS (untuk pencetakan, opsional).

---

## Instalasi
1. **Kloning Repository**:
   ```bash
   git clone https://github.com/fajarjulyana/CashierAppPremium.git
   ```
2. **Persiapkan Lingkungan**:
   - Install **wxWidgets** sesuai panduan resmi.
   - Pastikan **SQLite3** sudah terpasang di sistem.
3. **Build dan Jalankan**:
   - Gunakan IDE favorit Anda (seperti Code::Blocks atau Visual Studio) atau build menggunakan terminal:
     
   - g++ src/AdminApp.cpp -o AdminApp `wx-config --cxxflags --libs` -lsqlite3 -lcups -lxlsxwriter
   - g++ src/CashierApp.cpp -o CashierApp `wx-config --cxxflags --libs` -lsqlite3 -lcups 
   - g++ src/ManageApp.cpp -o ManageApp `wx-config --cxxflags --libs` -lsqlite3 -lcups
     

---

## Struktur Proyek
- `main.cpp`: File utama yang berisi logika aplikasi.
- `kasir.db`: File database SQLite (dibuat otomatis jika tidak ada).
- `README.md`: Dokumentasi proyek.

---

## Cara Menggunakan
1. **Jalankan Aplikasi**:
   - Aplikasi akan tampil dalam mode layar penuh.
2. **Tambah Barang ke Keranjang**:
   - Masukkan **ID Barang** dan jumlah barang, lalu klik `Tambah ke Keranjang`.
3. **Proses Transaksi**:
   - Masukkan jumlah pembayaran, klik `Proses Transaksi`.
4. **Hapus Tabel**:
   - Klik tombol `Hapus Tabel` untuk mengosongkan daftar barang di keranjang.

---

## Pengembangan
Fungsi-fungsi utama aplikasi terdapat di kelas `KasirFrame`, antara lain:
- `OnTambahKeKeranjang`: Menambah barang ke keranjang.
- `HitungTotal`: Menghitung total belanja.
- `SimpanTransaksiKeLog`: Menyimpan transaksi ke log database.
- `CetakStruk`: Mencetak struk transaksi.

Untuk modifikasi lebih lanjut, Anda dapat menambahkan fitur atau memperbaiki desain antarmuka sesuai kebutuhan.

---

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).

---

## Kontribusi
Jika Anda ingin berkontribusi pada proyek ini, silakan buat **Pull Request** atau buka **Issue** untuk diskusi lebih lanjut.

---

Dikembangkan dengan ❤️ oleh **Fajar Julyana**

