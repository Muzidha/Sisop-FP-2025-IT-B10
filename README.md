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
1. Filesystem in Userspace

Menurut Yerragudi et al (2011) Filesystem in USErspace atau dikenal FUSE merupakan suatu modul yang memuat untuk sistem UNIX dan menyediakan API (Antarmuka Pemrograman Aplikasi) kepada pogram tanpa mengubah kode kernel sama sekali. Design and Development of A User Based Secure File System for Linux Using FUSE. 10(1),9. Disisi lain Kanaujia et al (2012) menyatakan bahwasannya FUSE merupakan modul yang sifatnya terbuka. Pada modul ini FUSE menfasilitasi di ruang pengguna dan eksplore seperangkat API untuk kembangkan ruang pengguna yang dapat di mount dan berfungsi dengan penuh tanpa memperlukan hak istimewa. FUSEing Python for Development of Storage Efficient Filesystem 7(4), 4.

2. .....
3. Thread Safety & Logging
   
   Dikutip pada jurnal Efficiently Detecting Use-after-Free Exploits in Multi-Threaded Applications. Sebagian besar bahasa yang digunakan terdapat kontrol langsung dengan perangkat keras demi mendapatkan hasil yang maksimal. Salah satunya yang dapat kita rasakan yaitu penggunakan pointer. Namun penggunaan ini apabila tidak benar dapat menyebabkan kerentanan keamanan yang mengakibatkan kerusakan fragmen memori, Nigade Vijay (2019).
   
4. Virtual File System
   
   dalam buku "UnderStanding The Linux Kernel 3rd Edition" yang ditulis oleh Daniel P. Bovet dan Marco Cesati menyebutkan "The Virtual Filesystem (also known as Virtual Filesystem Switch or VFS) is a kernel software layer that handles all system calls related to a standard Unix filesystem. Its main strength is providing a common interface to several kinds of filesystems."(456) artinya Sistem File Virtual (juga dikenal sebagai Virtual Filesystem Switch atau VFS) adalah sebuah kernel
lapisan perangkat lunak yang menangani semua panggilan sistem yang terkait dengan sistem file Unix standar. Dia
kekuatan utamanya adalah menyediakan antarmuka umum untuk beberapa jenis sistem file. Dalam pengimplementasian yang dibuat Karena sistem ini tidak langsung beroperasi pada path absolut di sistem, path relatif dari FUSE perlu diterjemahkan ke path absolut (`real path`) di `target_dir`.

**Solusi**
1. Filesystem in Userspace

    didalam kode yang dibuat pengimplementasian FUSE sendiri dengan mengimplementasikan callback dalam `struct fuse_operations`. Program mengimplementasikan berbagai fungsi callback FUSE seperti `.getattr`, `.access`, `.read`, `.write`, `.mkdir`, `.unlink`, dll. Kemudian struktur operasi FUSE dideklarasikan di `static struct fuse_operations logging_oper = { ... };` Program dimulai dengan `ret = fuse_main(argc - 2, fuse_argv, &logging_oper, NULL);`

2. .....
3. Thread Safety & Logging

  - fungsi `log_operation()`:
    ```c
    static void log_operation(const char *operation, const char *path, const char *details) {
    pthread_mutex_lock(&log_mutex);
    
    if (log_fp) {
        time_t now;
        struct tm *timeinfo;
        char timestamp[80];
        
        time(&now);
        timeinfo = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        fprintf(log_fp, "[%s] %s: %s", timestamp, operation, path);
        if (details) {
            fprintf(log_fp, " - %s", details);
        }
        fprintf(log_fp, "\n");
        fflush(log_fp);
    }
    
    pthread_mutex_unlock(&log_mutex);
    }
    ```

    - Contoh penggunaan operasinya:
    ```c
    log_operation("RMDIR", path, "OK");
    return 0;
    ```

    
4. Virtual File System

   - Fungsi `get_full_path()` menyusun path absolut:
     ```c
     static void get_full_path(char fpath[PATH_MAX], const char *path) {
     strcpy(fpath, target_dir);
     strncat(fpath, path, PATH_MAX - strlen(target_dir));
     }
     ```
   - Semua operasi file menggunakan path fpath hasil terjemahan, misal:
     ```c
     get_full_path(fpath, path);
     res = open(fpath, fi->flags);
     ```

   



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
