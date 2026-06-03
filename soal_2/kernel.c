int cursor = 0;
char color = 0x07;

void putInMemory(int segment, int address, char character);
int getChar();

void printChar(char c) {
    putInMemory(0xB800, cursor * 2, c);
    putInMemory(0xB800, (cursor * 2) + 1, color);
    cursor++;
}

void newline() {
    int current_line = 0;
    int temp = cursor;
    while (temp >= 80) {
        temp -= 80;
        current_line++;
    }
    cursor = (current_line + 1) * 80;
}

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

void clearScreen() {
    int i;
    cursor = 0;
    for (i = 0; i < 2000; i++) {
        putInMemory(0xB800, i * 2, ' ');
        putInMemory(0xB800, (i * 2) + 1, 0x07);
    }
    cursor = 0;
}

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

int atoi(char *str) {
    int res = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return res;
}

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

int factorial(int n) {
    int res = 1;
    int i;
    for (i = 1; i <= n; i++) {
        res *= i;
    }
    return res;
}

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
