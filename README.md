# Sisop-FP-2025-IT-B10

# Final Project Sistem Operasi IT

## Peraturan
1. Waktu pengerjaan dimulai hari Kamis (19 Juni 2025) setelah soal dibagikan hingga hari Rabu (25 Juni 2025) pukul 23.59 WIB.
2. Praktikan diharapkan membuat laporan penjelasan dan penyelesaian soal dalam bentuk Readme(github).
3. Format nama repository github “Sisop-FP-2025-IT-[Kelas][Kelompok]” (contoh:Sisop-FP-2025-IT-A01).
4. Setelah pengerjaan selesai, seluruh source code dan semua script bash, awk, dan file yang berisi cron job ditaruh di github masing - masing kelompok, dan link github dikumpulkan pada form yang disediakan. Pastikan github di setting ke publik.
5. Commit terakhir maksimal 10 menit setelah waktu pengerjaan berakhir. Jika melewati maka akan dinilai berdasarkan commit terakhir.
6. Jika tidak ada pengumuman perubahan soal oleh asisten, maka soal dianggap dapat diselesaikan.
7. Jika ditemukan soal yang tidak dapat diselesaikan, harap menuliskannya pada Readme beserta permasalahan yang ditemukan.
8. Praktikan tidak diperbolehkan menanyakan jawaban dari soal yang diberikan kepada asisten maupun praktikan dari kelompok lainnya.
9. Jika ditemukan indikasi kecurangan dalam bentuk apapun di pengerjaan soal final project, maka nilai dianggap 0.
10. Pengerjaan soal final project sesuai dengan modul yang telah diajarkan.

## Kelompok 10

| Nama                              | NRP         |
|-----------------------------------|-------------|
| Erlangga Valdhio Putra Sulistio  | 5027241030  |
| Bima Aria Perthama               | 5027241060  |
| Muhammad Ardiansyah Tri Wibowo  | 5027241091  |
| Muhammad Ziddan Habibi          | 5027241122  |


## Deskripsi Soal

#### Logging File System

![image](https://github.com/user-attachments/assets/f67752b4-f101-44ba-b77f-2857c4cda6c7)


Memahami konsep file system virtual dan monitoring I/O menggunakan FUSE. Program akan membuat sebuah file system pengguna (FUSE) yang bertindak sebagai lapisan pengawas terhadap direktori target. Setiap operasi file yang dilakukan (seperti `open`, `read`, `write`, dan `mkdir`) di direktori target akan dicatat ke dalam sebuah file log. Program tidak menyimpan file sendiri, melainkan meneruskan seluruh operasi ke sistem file asli sambil mencatat aktivitasnya. Tujuan dari soal ini adalah untuk mempraktikkan bagaimana FUSE dapat digunakan untuk membuat sistem pemantauan file system sederhana.

### Catatan

---

#### **Deskripsi Umum**

Kode ini merupakan implementasi file system virtual menggunakan **FUSE (Filesystem in Userspace)** yang berfungsi **mencatat semua operasi file** ke dalam sebuah log file. Tujuan utamanya adalah **monitoring dan audit trail** terhadap operasi seperti `read`, `write`, `open`, `mkdir`, `unlink`, dan sebagainya yang dilakukan di dalam direktori yang di-*mount*.

---

#### **Struktur Komponen**

1. **`logging_fs.c`**: Implementasi utama sistem file berbasis FUSE dan pencatatan log.
2. **`tes_logging_fs.sh`**: Script bash untuk menguji fungsionalitas filesystem secara otomatis.
3. **`README.md`**: Template dokumentasi, berisi informasi umum tim dan struktur repo.
4. **`makefile`**: Tidak dianalisis karena tidak terbaca langsung, namun diasumsikan digunakan untuk kompilasi `logging_fs`.

---

#### **Fungsi Utama yang Diimplementasikan**

Semua fungsi FUSE yang di-*override* digunakan untuk mencatat log setiap kali ada aktivitas filesystem:

* `getattr`, `access`, `readlink`, `readdir`, `mkdir`, `unlink`, `rmdir`, `rename`, `chmod`, `chown`, `truncate`, `utimens`, `open`, `read`, `write`, `create`

Setiap operasi:

* Menggunakan `get_full_path()` untuk menggabungkan `target_dir` + `path`.
* Dilaporkan ke log melalui `log_operation()`, termasuk error bila terjadi.
* Thread-safe menggunakan mutex `log_mutex`.

Contoh isi log:

```
[2025-06-25 14:05:22] READ: /file1.txt - bytes=23 offset=0
```

---

####  **Testing (`tes_logging_fs.sh`)**

Script `tes_logging_fs.sh`:

* Membangun struktur direktori untuk pengujian (`./test_target`)
* Mem-*mount* filesystem ke direktori `./test_mount`
* Menjalankan serangkaian operasi (`ls`, `cat`, `echo`, `mv`, `chmod`, `rm`, dll.)
* Menampilkan isi log dan rekap jumlah masing-masing operasi

Mode eksekusi:

* `setup`: siapkan environment saja
* `start`: mount filesystem saja
* `test`: jalankan tes (filesystem harus aktif)
* `logs`: tampilkan log
* `full` *(default)*: semua langkah dari awal sampai akhir
* `cleanup`: unmount dan hapus proses terkait

---

####  **Keamanan dan Sinkronisasi**

* Menggunakan `pthread_mutex` untuk **menghindari race condition** dalam penulisan log.
* Ada penanganan sinyal `SIGINT` dan `SIGTERM` untuk memastikan file log ditutup dengan benar saat unmount atau terminate.

---

####  **Catatan Tambahan**

* Program memerlukan minimal **3 argumen saat dijalankan**:

  ```
  ./logging_fs <target_dir> <log_file> <mount_point> [fuse_options...]
  ```
* Fungsi `cleanup()` akan membebaskan memori heap dan menutup file log.
* Penggunaan `realpath()` dan `strdup()` meningkatkan keandalan validasi direktori/log.

---


Struktur repository:
```
logging-fuse/
├── logging_fs.c           # Source code utama
├── Makefile              # Build configuration
├── test_logging_fs.sh    # Script testing

```

## Pengerjaan

> Insert poin soal...

**Teori**

Menurut Yerragudi et al (2011) Filesystem in USErspace atau dikenal FUSE merupakan suatu modul yang memuat untuk sistem UNIX dan menyediakan API (Antarmuka Pemrograman Aplikasi) kepada pogram tanpa mengubah kode kernel sama sekali. Design and Development of A User Based Secure File System for Linux Using FUSE. 10(1),9. Disisi lain Kanaujia et al (2012) menyatakan bahwasannya FUSE merupakan modul yang sifatnya terbuka. Pada modul ini FUSE menfasilitasi di ruang pengguna dan eksplore seperangkat API untuk kembangkan ruang pengguna yang dapat di mount dan berfungsi dengan penuh tanpa memperlukan hak istimewa. FUSEing Python for Development of Storage Efficient Filesystem 7(4), 4.

**Solusi**



> Insert poin soal...

**Teori**

...

**Solusi**

...

**Video Menjalankan Program**


https://github.com/user-attachments/assets/66d39df2-ee7d-443d-b8a7-54cb1dcfdfb2




## Daftar Pustaka

Sitasi 1
Sitasi 2
Sitasi 3
