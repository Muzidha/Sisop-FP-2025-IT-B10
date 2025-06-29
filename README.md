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

Struktur repository:
```
logging-fuse/
├── logging_fs.c           # Source code utama
├── Makefile              # Build configuration
├── test_logging_fs.sh    # Script testing

```

## Pengerjaan


**Teori**
1. Filesystem in Userspace

   Menurut Yerragudi et al (2011) Filesystem in USErspace atau dikenal FUSE merupakan suatu modul yang memuat untuk sistem UNIX dan menyediakan API (Antarmuka Pemrograman Aplikasi) kepada pogram tanpa mengubah kode kernel sama sekali. Design and Development of A User Based Secure File System for Linux Using FUSE. 10(1),9. Disisi lain Kanaujia et al (2012) menyatakan bahwasannya FUSE merupakan modul yang sifatnya terbuka. Pada modul ini FUSE menfasilitasi di ruang pengguna dan eksplore seperangkat API untuk kembangkan ruang pengguna yang dapat di mount dan berfungsi dengan penuh tanpa memperlukan hak istimewa. FUSEing Python for Development of Storage Efficient Filesystem 7(4), 4.

2. System Call Wrapping

    Menurut Silberschatz, Galvin, dan Gagne dalam buku Operating System Concepts (9th Edition), system call wrapping adalah teknik membungkus pemanggilan system call dengan fungsi tambahan untuk tujuan seperti logging, debugging, modifikasi perilaku, keamanan, atau manipulasi data. Teknik ini memungkinkan developer menambahkan perilaku baru di sekitar pemanggilan system call, misalnya mencatat setiap kali fungsi seperti open() atau read() dipanggil, tanpa mengubah kode asli dari system call itu sendiri.
   
3. Thread Safety & Logging
   
   Dikutip pada jurnal Efficiently Detecting Use-after-Free Exploits in Multi-Threaded Applications. Sebagian besar bahasa yang digunakan terdapat kontrol langsung dengan perangkat keras demi mendapatkan hasil yang maksimal. Salah satunya yang dapat kita rasakan yaitu penggunakan pointer. Namun penggunaan ini apabila tidak benar dapat menyebabkan kerentanan keamanan yang mengakibatkan kerusakan fragmen memori, Nigade Vijay (2019).
   
4. Virtual File System
   
   dalam buku "UnderStanding The Linux Kernel 3rd Edition" yang ditulis oleh Daniel P. Bovet dan Marco Cesati menyebutkan "The Virtual Filesystem (also known as Virtual Filesystem Switch or VFS) is a kernel software layer that handles all system calls related to a standard Unix filesystem. Its main strength is providing a common interface to several kinds of filesystems."(456) artinya Sistem File Virtual (juga dikenal sebagai Virtual Filesystem Switch atau VFS) adalah sebuah kernel
lapisan perangkat lunak yang menangani semua panggilan sistem yang terkait dengan sistem file Unix standar. Dia
kekuatan utamanya adalah menyediakan antarmuka umum untuk beberapa jenis sistem file. Dalam pengimplementasian yang dibuat Karena sistem ini tidak langsung beroperasi pada path absolut di sistem, path relatif dari FUSE perlu diterjemahkan ke path absolut (`real path`) di `target_dir`.

5. Mutual Exclusion (Mutex)
   
   Menurut Silberschatz, Galvin, dan Gagne dalam buku Operating System Concepts (10th Edition), mutex (mutual exclusion) adalah mekanisme sinkronisasi yang digunakan dalam pemrograman multithread untuk mencegah lebih dari satu thread mengakses bagian kritis kode secara bersamaan. Teknik ini memungkinkan developer untuk mengatur eksekusi eksklusif terhadap sumber daya bersama, seperti file atau variabel global, sehingga dapat menghindari kondisi balapan (race condition).

   Selain itu, Kerrisk (2010) dalam bukunya The Linux Programming Interface menyatakan bahwa "mutex locks provide a way to protect shared data from being simultaneously accessed by multiple threads." Mutex bekerja dengan cara mengunci (lock) akses terhadap bagian kritis sebelum digunakan, dan membukanya kembali (unlock) setelah selesai, sehingga menjamin integritas data tetap terjaga di lingkungan multithreaded.



**Solusi**
1. Filesystem in Userspace

    didalam kode yang dibuat pengimplementasian FUSE sendiri dengan mengimplementasikan callback dalam `struct fuse_operations`. Program mengimplementasikan berbagai fungsi callback FUSE seperti `.getattr`, `.access`, `.read`, `.write`, `.mkdir`, `.unlink`, dll. Kemudian struktur operasi FUSE dideklarasikan di `static struct fuse_operations logging_oper = { ... };` Program dimulai dengan `ret = fuse_main(argc - 2, fuse_argv, &logging_oper, NULL);`

2. System Call Wrapping

- Wrapping system call `open()`
```bash
static int logging_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char fpath[PATH_MAX];
    char flags_str[64];
    
    get_full_path(fpath, path);
    res = open(fpath, fi->flags);  
    
    if (res == -1) {
        log_operation("OPEN_ERROR", path, strerror(errno)); 
        return -errno;
    }
    
    sprintf(flags_str, "flags=%d fd=%d", fi->flags, res);
    log_operation("OPEN", path, flags_str);  
    
    close(res);
    return 0;
}

```

- Wrapping system call `read()`
```bash
static int logging_read(const char *path, char *buf, size_t size, off_t offset,
                        struct fuse_file_info *fi) {
    FILE *fp;
    int res;
    char fpath[PATH_MAX];
    char read_info[64];

    get_full_path(fpath, path);
    fp = fopen(fpath, "r");  
    
    if (fp == NULL) {
        log_operation("READ_ERROR", path, strerror(errno));
        return -errno;
    }

    fseek(fp, offset, SEEK_SET);
    res = fread(buf, 1, size, fp);  
    ...
}

```
- Wrapping system call `mkdir()`
```bash
  static int logging_mkdir(const char *path, mode_t mode) {
    int res;
    char fpath[PATH_MAX];
    char mode_str[32];
    
    get_full_path(fpath, path);
    res = mkdir(fpath, mode);  
    
    if (res == -1) {
        log_operation("MKDIR_ERROR", path, strerror(errno));
        return -errno;
    }
    
    sprintf(mode_str, "mode=%o", mode);
    log_operation("MKDIR", path, mode_str);
    return 0;}
```
- Wrapping system call `write()`
```bash
  static int logging_write(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi) {
    FILE *fp;
    int res;
    char fpath[PATH_MAX];
    char write_info[64];
    
    (void) fi;
    get_full_path(fpath, path);  

    fp = fopen(fpath, "r+");    
    
    if (fp == NULL) {
        log_operation("WRITE_ERROR", path, strerror(errno));  
        return -errno;
    }
    
    fseek(fp, offset, SEEK_SET); 
    res = fwrite(buf, 1, size, fp);  
    
    if (res == -1) {
        res = -errno;
        log_operation("WRITE_ERROR", path, strerror(errno));  
    } else {
        sprintf(write_info, "bytes=%d offset=%ld", res, offset);
        log_operation("WRITE", path, write_info);  
    }
    
    fclose(fp);
    return res;
}

```
  


   
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

5. Mutual Exclusion (Mutex)

```bash
static void log_operation(const char *operation, const char *path, const char *details) {
    pthread_mutex_lock(&log_mutex);   // Lock sebelum akses log_fp

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

    pthread_mutex_unlock(&log_mutex); // Unlock setelah selesai akses log
}

```

   




**Video Menjalankan Program**


https://github.com/user-attachments/assets/66d39df2-ee7d-443d-b8a7-54cb1dcfdfb2




## Daftar Pustaka

Sitasi 1

- Yerragudi, V. S., N. D. Gangadhar, dan Dipayan M. Design and Development of A User Based Secure File System for Linux Using FUSE

- Kanaujia, V., dan Chetan G. FUSE’ing Python for Development of Storage Efficient Filesystem 

Sitasi 2

- Silberschatz, A., Galvin, P. B., & Gagne, G. (2013). Operating System Concepts (9th ed.)


Sitasi 3

- Nigade, V. V. Efficiently Detecting Use-after-Free Exploits in Multi-Threaded Applications

Sitasi 4

- Bovet, D. P., dan Cesati, M. 2006. Understanding the Linux Kernel. 3rd ed. Sebastopol: O’Reilly Media.

Sitasi 5
- Silberschatz, A., Galvin, P. B., & Gagne, G. (2020). Operating System Concepts (10th ed.)
- Kerrisk, M. (2010). The Linux Programming Interface: A Linux and UNIX System Programming Handbook.
