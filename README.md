# SISOP-5-2026-IT-054
Nabila Sharliz Sigit (5027251054)<br>
Berikut adalah Reporting dari Praktikum SISOP Modul 5:

---
## **Soal 2**
**Penjelasan** <br>
<br>
Pertama-tama mengisi fungsi getChar_ dan _putInMemory pada kernel.asm sebagai penghubung antara kernel.c dengan hardware/BIOS. Diawali dengan deklarasi dan konfigurasi awal. Lalu selanjutnya _start sebagai titik awal eksekusi kernel setelah dimuat oleh bootloader. Dilanjut dengan .hang (infinite loop) untuk menjaga CPU tetap berada di kernel setelah main() selesai dieksekusi.

```c
bits 16

global _start
global _putInMemory
global _getChar
extern _main

_start:

    cli

    mov ax, cs
    mov ds, ax
    mov es, ax

    sti

    call _main

.hang:
    jmp .hang


```
Untuk fungsi intinya ada _putInMemory yaitu untuk menulis satu byte data ke alamat memori tertentu sehingga dapat memberikan akses langsung ke memori dari program C. Lalu, _getChar agar huruf-huruf yang di ditulis bisa tampil di layar, atau menyediakan input keyboard untuk kernel.

```c
_putInMemory:
    push bp
    mov bp, sp

    push ds

    mov ax, [bp+4]
    mov si, [bp+6]
    mov cl, [bp+8]

    mov ds, ax
    mov [si], cl

    pop ds

    pop bp
    ret

_getChar:
    push bp
    mov bp, sp

    mov ah, 00h
    int 16h

    mov ah, 00h     

    pop bp
    ret
```
Program pada file kernel.asm sudah lengkap, maka lanjut ke file kernel.c yaitu program inti kernel yang mengatur interaksi antara user dengan komputer setelah booting selesai. <br>
Diawali dengan variabel deklarasi cursor untuk menyimpan posisi karakter saat ini dan colour sebagai warna text saat ini (0x07 = putih). Lalu, dilanjut dengan 2 fungsi yaitu putInMemory() dan getChar() yang dimana implementasinya berada di kernel.asm

```c
int cursor = 0;
char color = 0x07;

void putInMemory(int segment, int address, char character);
int getChar();
```
Fungsi printChar() digunakan untuk menampilkan char ke layar dengan cara menulis langsung ke memori video VGA pada alamat 0xB800. Karakter yang diterima sebagai parameter disimpan pada posisi yang ditentukan oleh variabel cursor, kemudian atribut warnanya diambil dari variabel color. Setelah karakter berhasil ditampilkan, nilai cursor ditambah satu agar posisi penulisan berikutnya berpindah ke karakter selanjutnya.

```c
void printChar(char c) {
    putInMemory(0xB800, cursor * 2, c);
    putInMemory(0xB800, (cursor * 2) + 1, color);
    cursor++;
}
```

Fungsi newline() digunakan untuk memindahkan posisi kursor ke awal baris berikutnya. Fungsi ini menghitung baris saat ini berdasarkan nilai cursor, kemudian mengubah nilai cursor ke posisi kolom pertama pada baris selanjutnya. ("\n")
```c
void newline() {
    int current_line = 0;
    int temp = cursor;
    while (temp >= 80) {
        temp -= 80;
        current_line++;
    }
    cursor = (current_line + 1) * 80;
}
```

Fungsi printString() digunakan untuk menampilkan sebuah string ke layar satu karakter pada satu waktu, jadi saat string[i] (per karakter di string sesuai i atau index) bukan karakter akhir dan jika karakter newline maka memanggil fungsi newline(), else akan memanggil fungsi printChar.
```c
void printString(char *string) {
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == '\n') {
            newline();
        } else {
            printChar(string[i]);
        }
        i++;
    }
}
```

Fungsi clearScreen() digunakan untuk membersihkan seluruh tampilan layar dengan mengisi setiap posisi karakter pada memori video VGA dengan spasi (' ') dan warna default. Lalu, setelah layar bersih, cursor di set kembali menjadi 0.
```c
void clearScreen() {
    int i;
    cursor = 0;
    for (i = 0; i < 2000; i++) {
        putInMemory(0xB800, i * 2, ' ');
        putInMemory(0xB800, (i * 2) + 1, 0x07);
    }
    cursor = 0;
}
```

Fungsi readString() digunakan untuk membaca input pengguna dari keyboard dan menyimpannya ke dalam buffer sebagai sebuah string. Fungsi ini terus menerima karakter menggunakan getChar() hingga pengguna menekan tombol Enter (0x0D), yang menandai akhir input, ada juga fitur backspace (0x08).
```c
void readString(char *buffer) {
    int i = 0;
    while (1) {
        char c = getChar();
        if (c == 0x0D) {
            buffer[i] = '\0';
            break;
        } else if (c == 0x08) { 
            if (i > 0) {
                i--;
                cursor--;
                printChar(' ');
                cursor--;
            }
        } else if (c >= 32 && c <= 126) {
            buffer[i] = c;
            printChar(c);
            i++;
        }
    }
}
```

Fungsi strcmp() digunakan untuk membandingkan dua string karakter per karakter. Saat char string1 dan string2 bukan karakter akhir, jika char kedua string berbeda mengembalikan nilai 0. Namun jika char kedua string sama dan berakhir di posisi yang sama juga maka return 1.

```c
int strcmp(char *str1, char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return 0; 
        }
        i++;
    }
    if (str1[i] == '\0' && str2[i] == '\0') {
        return 1;
    }
    return 0;
}
```

Fungsi startsWith() digunakan untuk memeriksa apakah sebuah string diawali dengan awalan (prefix) tertentu, seperti add 5 3 atau fac 4. Dilakukan dengan membandingan karakter pada string dengan prefix.
```c
int startsWith(char *str, char *prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (str[i] != prefix[i]) {
            return 0; 
        }
        i++;
    }
    return 1;
}

```

Fungsi atoi() yaitu untuk string menjadi tipe integer. Fungsi membaca setiap karakter digit dari string, kemudian menggabungkannya menjadi sebuah bilangan dengan cara mengalikan hasil sebelumnya dengan 10 dan menambahkan nilai digit yang sedang dibaca.
'''c
int atoi(char *str) {
    int res = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return res;
}
'''

Fungsi intToString() adalah kebalikan dari fungsi atoi() dimana ia mengubah tipe data integer menjadi string dengan mengambil setiap digit dari angka, menyimpannya sementara dalam urutan terbalik, kemudian membalik kembali urutannya ke dalam string hasil.
```c
void intToString(int num, char *str) {
    char temp[16];
    int i = 0;
    int j = 0;
    int digit = 0;
    int t = 0;
    int quotient = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0) {
        digit = 0;
        t = num;
        while (t >= 10) {
            t -= 10;
        }
        digit = t;
        temp[i++] = digit + '0';
        
        quotient = 0;
        t = num;
        while (t >= 10) {
            t -= 10;
            quotient++;
        }
        num = quotient;
    }

    for (j = 0; j < i; j++) {
        str[j] = temp[i - 1 - j];
    }
    str[i] = '\0';
}
```
Fungsi factorial() digunakan untuk menghitung faktorial suatu angka bertipe data integer.
```c
int factorial(int n) {
    int res = 1;
    int i;
    for (i = 1; i <= n; i++) {
        res *= i;
    }
    return res;
}
```

Fungsi getArgs() digunakan untuk mengambil argumen tertentu dari sebuah command yang dimasukkan pengguna (memisahkan command dan parameternya). Fungsi ini mencari posisi argumen berdasarkan jumlah spasi pada string command, kemudian menyalin argumen tersebut ke dalam argBuffer.
```c
void getArgs(char *cmd, int argNum, char *argBuffer) {
    int i = 0, j = 0, spaceCount = 0;
    while (cmd[i] != '\0') {
        if (cmd[i] == ' ') {
            spaceCount++;
            while (cmd[i] == ' ') i++;
            if (spaceCount == argNum) {
                while (cmd[i] != ' ' && cmd[i] != '\0') {
                    argBuffer[j++] = cmd[i++];
                }
                argBuffer[j] = '\0';
                return;
            }
        }
        if (cmd[i] != '\0') i++;
    }
    argBuffer[0] = '\0';
}
```

Fungsi main() yaitu fungsi utama yang menjalankan shell pada kernel. Fungsi ini diawali dengan membersihkan layar dan menampilkan pesan "Welcome to Assistant's Last Gift", kemudian masuk ke dalam perulangan untuk terus menerima perintah dari pengguna. Setiap perintah yang dimasukkan akan dibaca, diperiksa, dan diproses sesuai fungsinya, seperti menampilkan bantuan (help), informasi sistem (about), membersihkan layar (clear), melakukan operasi aritmatika (add, sub, fac), mengubah warna teks (season), atau mencetak pola segitiga (triangle). Jika perintah yang dimasukkan tidak dikenali, sistem akan menampilkan pesan "Command not found". Jika menghitung fac lebih dari 7 juga akan memunculkan pesan limit.
```c
void main() {
    char cmd[64];
    char arg1[16], arg2[16];
    char resultStr[16];
    int n;
    int i;
    int j;
    int res;

    clearScreen();

    printString("Welcome to Assistant's Last Gift");
    newline();
    printString("type 'help'");
    newline();
    newline();

    while (1) {
        printString("> ");
        readString(cmd);
        newline();

        if (strcmp(cmd, "help")) {
            printString("check, add, sub, fac, season, triangle, clear, about");
        } 
        else if (strcmp(cmd, "about")) {
            printString("Kernel OS 16-bit v1.0");
        } 
        else if (strcmp(cmd, "clear")) {
            clearScreen();
            continue;
        } 
        else if (strcmp(cmd, "check")) {
            printString("ok");
        } 
        else if (startsWith(cmd, "add ")) {
            getArgs(cmd, 1, arg1);
            getArgs(cmd, 2, arg2);
            if (arg1[0] != '\0' && arg2[0] != '\0') {
                res = atoi(arg1) + atoi(arg2);
                intToString(res, resultStr);
                printString(resultStr);
            } else {
                printString("Usage: add <a> <b>");
            }
        } 
        else if (startsWith(cmd, "sub ")) {
            getArgs(cmd, 1, arg1);
            getArgs(cmd, 2, arg2);
            if (arg1[0] != '\0' && arg2[0] != '\0') {
                res = atoi(arg1) - atoi(arg2);
                if (res < 0) {
                    printString("-");
                    res = -res;
                }
                intToString(res, resultStr);
                printString(resultStr);
            } else {
                printString("Usage: sub <a> <b>");
            }
        } 
        else if (startsWith(cmd, "fac ")) {
            getArgs(cmd, 1, arg1);
            if (arg1[0] != '\0') {
                n = atoi(arg1);
                if (n > 7) {
                    printString("know your limit bro.");
                } else {
                    res = factorial(n);
                    intToString(res, resultStr);
                    printString(resultStr);
                }
            } else {
                printString("Usage: fac <n>");
            }
        } 
        else if (startsWith(cmd, "season ")) {
            getArgs(cmd, 1, arg1);
            if (strcmp(arg1, "winter")) {
                color = 0x01;
                printString("winter mode");
            } else if (strcmp(arg1, "spring")) {
                color = 0x02;
                printString("spring mode");
            } else if (strcmp(arg1, "summer")) {
                color = 0x0E;
                printString("summer mode");
            } else if (strcmp(arg1, "fall")) {
                color = 0x06;
                printString("fall mode");
            } else if (strcmp(arg1, "radiant")) {
                color = 0x05;
                printString("radiant mode");
            } else {
                printString("Unknown season.");
            }
        } 
        else if (startsWith(cmd, "triangle ")) {
            getArgs(cmd, 1, arg1);
            if (arg1[0] != '\0') {
                n = atoi(arg1);
                for (i = 1; i <= n; i++) {
                    for (j = 1; j <= i; j++) {
                        printChar('*');
                    }
                    if (i < n) {
                        newline();
                    }
                }
            } else {
                printString("Usage: triangle <n>");
            }
        } 
        else if (cmd[0] != '\0') {
            printString("Command not found");
        }

        newline();
    }
}
```

**Output:**
Belum ada karena saya error di bagian bochs nya. Sekian Terimakasih.
