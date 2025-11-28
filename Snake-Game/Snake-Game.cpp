#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

// --- GLOBAL DEĞİŞKENLER ---
bool gameOver;
const int width = 40;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[1000], tailY[1000];
int nTail;

// HIZ AYARI: 110ms
const int GAME_SPEED = 110;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

// --- YARDIMCI FONKSİYONLAR ---
void TextColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
    HideCursor();
}

// --- EKRAN ÇİZİMİ ---
void Draw() {
    gotoxy(0, 0);

    // --- ÜST DUVAR ---
    TextColor(8);
    for (int i = 0; i < width + 2; i++) cout << (char)219;
    cout << endl;

    // --- HARİTA İÇİ ---
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) { TextColor(8); cout << (char)219; } // Sol Duvar

            if (i == y && j == x) {
                TextColor(10); cout << "O"; // Yılan Başı
            }
            else if (i == fruitY && j == fruitX) {
                TextColor(12); cout << "+"; // Meyve
            }
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        TextColor(2); cout << "o"; // Kuyruk
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) { TextColor(8); cout << (char)219; } // Sağ Duvar
        }
        cout << endl;
    }

    // --- ALT DUVAR ---
    TextColor(8);
    for (int i = 0; i < width + 2; i++) cout << (char)219;
    cout << endl;

    // --- ALT BİLGİ PANELİ ---
    TextColor(14);
    cout << " SKOR: " << score << endl;
    TextColor(8);
    for (int i = 0; i < width + 2; i++) cout << "-";
    cout << endl;
    TextColor(7);
    cout << " [W,A,S,D] Yonet | [X] Cikis" << endl;
}

// --- ÖZEL BAŞLANGIÇ MANTIĞI ---
void WaitForStartAndCountdown() {
    // 1. Önce statik olarak map'i ve yılanı çiz (Donmuş görüntü)
    Draw();

    // 2. Map'in ortasına mesaj yaz
    TextColor(15); // Parlak Beyaz
    gotoxy(width / 2 - 11, height / 2);
    cout << "BASLAMAK ICIN YON SEC";
    gotoxy(width / 2 - 4, height / 2 + 1);
    cout << "[W,A,S,D]";

    // 3. Geçerli bir yön tuşu bekleniyor...
    bool validKey = false;
    char key;
    while (!validKey) {
        if (_kbhit()) {
            key = _getch();
            // Sadece yön tuşlarını kabul et
            switch (key) {
            case 'w': case 'W': dir = UP; validKey = true; break;
            case 's': case 'S': dir = DOWN; validKey = true; break;
            case 'a': case 'A': dir = LEFT; validKey = true; break;
            case 'd': case 'D': dir = RIGHT; validKey = true; break;
            case 'x': case 'X': exit(0); break; // Çıkış imkanı
            }
        }
    }

    // 4. Tuşa basıldı, şimdi map içindeki yazıyı silip geri sayım yapalım
    // Önceki yazıyı temizlemek için map'i tekrar çiziyoruz (daha temiz görünür)
    Draw();

    TextColor(12); // Kırmızı
    for (int i = 3; i > 0; i--) {
        gotoxy(width / 2 - 2, height / 2);
        cout << " " << i << " "; // Sayıyı yaz
        Sleep(1000); // 1 saniye bekle
    }

    // 5. Geri sayımdan sonra map'i son kez temizle ki "1" yazısı ekranda kalmasın
    Draw();
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a': case 'A': if (dir != RIGHT) dir = LEFT; break;
        case 'd': case 'D': if (dir != LEFT) dir = RIGHT; break;
        case 'w': case 'W': if (dir != DOWN) dir = UP; break;
        case 's': case 'S': if (dir != UP) dir = DOWN; break;
        case 'x': case 'X': gameOver = true; break;
        }
    }
}

void Logic() {
    if (dir == STOP) return;

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
    case LEFT: x--; break;
    case RIGHT: x++; break;
    case UP: y--; break;
    case DOWN: y++; break;
    default: break;
    }

    // Duvara Çarpma
    if (x >= width || x < 0 || y >= height || y < 0) gameOver = true;

    // Kuyruğa Çarpma
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y) gameOver = true;

    // Yem Yeme
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

int main() {
    char secim;

    do {
        Setup(); // Yılanı ve yemi oluştur

        // --- YENİ BAŞLANGIÇ FONKSİYONU ---
        // Bu fonksiyon tuş bekler, sayım yapar ve 'dir' değişkenini ayarlar
        WaitForStartAndCountdown();

        // Oyun Döngüsü
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            Sleep(GAME_SPEED);
        }

        // --- OYUN SONU MENÜSÜ ---
        system("cls");
        TextColor(12);
        cout << "\n\n\t###########################" << endl;
        cout << "\t#      OYUN BITTI!        #" << endl;
        cout << "\t###########################" << endl;

        TextColor(14);
        cout << "\n\tTOPLAM SKORUNUZ: " << score << endl;

        TextColor(7);
        cout << "\n\t[R] - Yeniden Basla" << endl;
        cout << "\t[C] - Cikis Yap" << endl;

        while (true) {
            if (_kbhit()) {
                secim = _getch();
                if (secim == 'r' || secim == 'R' || secim == 'c' || secim == 'C')
                    break;
            }
        }

    } while (secim == 'r' || secim == 'R');

    return 0;
}