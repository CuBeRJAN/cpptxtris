#include <iostream>
#include<string.h>
#include <vector>
#include <cmath>
#include<future>
#include <cstdlib> 
#include <ctime> 
#include <thread> 
#include<chrono>
#include <algorithm>

using std::string;
using std::cout;
using std::cin;

int pieces[] = {0,1,2,3,4,5,6};
int pindex = 0;

// TODO: game end, check collision for rotation, score counter, game speedup


#define WIDTH 10 // 10
#define HEIGHT 12 // 12
#define SHAPESIZE 5 // 5
#define QSHAPESIZE 25 // 5*5

// shapes must be square and 5x5
// shape will be rotated around central piece
const char i_shape[] = { '.', '.', 'O', '.', '.', // SHAPE "I"
                         '.', '.', 'O', '.', '.',
                         '.', '.', 'O', '.', '.',
                         '.', '.', 'O', '.', '.',
                         '.', '.', '.', '.', '.' };

const char s_shape[] = { '.', '.', '.', '.', '.', // SHAPE "S"
                         '.', '.', '.', 'O', '.',
                         '.', '.', 'O', 'O', '.',
                         '.', '.', 'O', '.', '.',
                         '.', '.', '.', '.', '.' };
const char z_shape[] = { '.', '.', '.', '.', '.', // SHAPE "Z"
                         '.', 'O', '.', '.', '.',
                         '.', 'O', 'O', '.', '.',
                         '.', '.', 'O', '.', '.',
                         '.', '.', '.', '.', '.' };
const char q_shape[] = { '.', '.', '.', '.', '.', // SHAPE "Q"
                         '.', '.', '.', '.', '.',
                         '.', '.', 'O', 'O', '.',
                         '.', '.', 'O', 'O', '.',
                         '.', '.', '.', '.', '.' };
const char l_shape[] = { '.', '.', '.', '.', '.', // SHAPE "L"
                         '.', '.', 'O', '.', '.',
                         '.', '.', 'O', '.', '.',
                         '.', '.', 'O', 'O', '.',
                         '.', '.', '.', '.', '.' };
const char rl_shape[] = { '.', '.', '.', '.', '.', // SHAPE "L (reverse)"
                          '.', '.', 'O', '.', '.',
                          '.', '.', 'O', '.', '.',
                          '.', 'O', 'O', '.', '.',
                          '.', '.', '.', '.', '.' };
const char t_shape[] = { '.', '.', '.', '.', '.', // SHAPE "T"
                         '.', '.', 'O', '.', '.',
                         '.', 'O', 'O', 'O', '.',
                         '.', '.', '.', '.', '.',
                         '.', '.', '.', '.', '.' };



#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
void scr_clear() {
    system("cls");
}
void game_quit() {
    cout << "Game over!\n";
    exit(0);
}
int key_press(char* mv) { // not working: F11 (-122, toggles fullscreen)
    KEY_EVENT_RECORD keyevent;
    INPUT_RECORD irec;
    DWORD events;
    while (true) {
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &irec, 1, &events);
        if (irec.EventType == KEY_EVENT && ((KEY_EVENT_RECORD&)irec.Event).bKeyDown) {
            keyevent = (KEY_EVENT_RECORD&)irec.Event;
            const int ca = (int)keyevent.uChar.AsciiChar;
            const int cv = (int)keyevent.wVirtualKeyCode;
            const int key = ca == 0 ? -cv : ca + (ca > 0 ? 0 : 256);
            switch (key) {
            case  -16: continue; // disable Shift
            case  -17: continue; // disable Ctrl / AltGr
            case  -18: continue; // disable Alt / AltGr
            case -220: continue; // disable first detection of "^" key (not "^" symbol)
            case -221: continue; // disable first detection of "`" key (not "`" symbol)
            case -191: continue; // disable AltGr + "#"
            case  -52: continue; // disable AltGr + "4"
            case  -53: continue; // disable AltGr + "5"
            case  -54: continue; // disable AltGr + "6"
            case  -12: continue; // disable num block 5 with num lock deactivated
            case   13: return  10; // enter
            case  -46: return 127; // delete
            case  -49: return 251; // ¹
            case    0: continue;
            case    1: continue; // disable Ctrl + a (selects all text)
            case    2: continue; // disable Ctrl + b
            case    3: continue; // disable Ctrl + c (terminates program)
            case    4: continue; // disable Ctrl + d
            case    5: continue; // disable Ctrl + e
            case    6: continue; // disable Ctrl + f (opens search)
            case    7: continue; // disable Ctrl + g
            //case    8: continue; // disable Ctrl + h (ascii for backspace)
            //case    9: continue; // disable Ctrl + i (ascii for tab)
            case   10: continue; // disable Ctrl + j
            case   11: continue; // disable Ctrl + k
            case   12: continue; // disable Ctrl + l
            //case   13: continue; // disable Ctrl + m (breaks console, ascii for new line)
            case   14: continue; // disable Ctrl + n
            case   15: continue; // disable Ctrl + o
            case   16: continue; // disable Ctrl + p
            case   17: continue; // disable Ctrl + q
            case   18: continue; // disable Ctrl + r
            case   19: continue; // disable Ctrl + s
            case   20: continue; // disable Ctrl + t
            case   21: continue; // disable Ctrl + u
            case   22: continue; // disable Ctrl + v (inserts clipboard)
            case   23: continue; // disable Ctrl + w
            case   24: continue; // disable Ctrl + x
            case   25: continue; // disable Ctrl + y
            case   26: continue; // disable Ctrl + z
            default: *mv = (char)key; // any other ASCII/virtual character
            }
        }
    }
}
#elif defined(__linux__)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
void scr_clear() {
    system("clear");
}
void game_quit() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
    cout << "Game over!\n";
    exit(0);
}
int key_press(char* mv) { // not working: ¹ (251), num lock (-144), caps lock (-20), windows key (-91), kontext menu key (-93)
    struct termios term;
    tcgetattr(0, &term);
    while (true) {
        term.c_lflag &= ~(ICANON | ECHO); // turn off line buffering and echoing
        tcsetattr(0, TCSANOW, &term);
        int nbbytes;
        ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
        while (!nbbytes) {
            sleep(0.01);
            fflush(stdout);
            ioctl(0, FIONREAD, &nbbytes); // 0 is STDIN
        }
        int key = (int)getchar();
        if (key == 27 || key == 194 || key == 195) { // escape, 194/195 is escape for °ß´äöüÄÖÜ
            key = (int)getchar();
            if (key == 91) { // [ following escape
                key = (int)getchar(); // get code of next char after \e[
                if (key == 49) { // F5-F8
                    key = 62 + (int)getchar(); // 53, 55-57
                    if (key == 115) key++; // F5 code is too low by 1
                    getchar(); // take in following ~ (126), but discard code
                }
                else if (key == 50) { // insert or F9-F12
                    key = (int)getchar();
                    if (key == 126) { // insert
                        key = 45;
                    }
                    else { // F9-F12
                        key += 71; // 48, 49, 51, 52
                        if (key < 121) key++; // F11 and F12 are too low by 1
                        getchar(); // take in following ~ (126), but discard code
                    }
                }
                else if (key == 51 || key == 53 || key == 54) { // delete, page up/down
                    getchar(); // take in following ~ (126), but discard code
                }
            }
            else if (key == 79) { // F1-F4
                key = 32 + (int)getchar(); // 80-83
            }
            key = -key; // use negative numbers for escaped keys
        }
        term.c_lflag |= (ICANON | ECHO); // turn on line buffering and echoing
        tcsetattr(0, TCSANOW, &term);
        switch (key) {
        case  127: return   8; // backspace
        case  -27: return  27; // escape
        case  -51: return 127; // delete
        case -164: return 132; // ä
        case -182: return 148; // ö
        case -188: return 129; // ü
        case -132: return 142; // Ä
        case -150: return 153; // Ö
        case -156: return 154; // Ü
        case -159: return 225; // ß
        case -181: return 230; // µ
        case -167: return 245; // §
        case -176: return 248; // °
        case -178: return 253; // ²
        case -179: return 252; // ³
        case -180: return 239; // ´
        case  -65: return -38; // up arrow
        case  -66: return -40; // down arrow
        case  -68: return -37; // left arrow
        case  -67: return -39; // right arrow
        case  -53: return -33; // page up
        case  -54: return -34; // page down
        case  -72: return -36; // pos1
        case  -70: return -35; // end
        case    0: continue;
        case    1: continue; // disable Ctrl + a
        case    2: continue; // disable Ctrl + b
        case    3: continue; // disable Ctrl + c (terminates program)
        case    4: continue; // disable Ctrl + d
        case    5: continue; // disable Ctrl + e
        case    6: continue; // disable Ctrl + f
        case    7: continue; // disable Ctrl + g
        case    8: continue; // disable Ctrl + h
        //case    9: continue; // disable Ctrl + i (ascii for tab)
        //case   10: continue; // disable Ctrl + j (ascii for new line)
        case   11: continue; // disable Ctrl + k
        case   12: continue; // disable Ctrl + l
        case   13: continue; // disable Ctrl + m
        case   14: continue; // disable Ctrl + n
        case   15: continue; // disable Ctrl + o
        case   16: continue; // disable Ctrl + p
        case   17: continue; // disable Ctrl + q
        case   18: continue; // disable Ctrl + r
        case   19: continue; // disable Ctrl + s
        case   20: continue; // disable Ctrl + t
        case   21: continue; // disable Ctrl + u
        case   22: continue; // disable Ctrl + v
        case   23: continue; // disable Ctrl + w
        case   24: continue; // disable Ctrl + x
        case   25: continue; // disable Ctrl + y
        case   26: continue; // disable Ctrl + z (terminates program)
        default: *mv = (char)key; // any other ASCII character
        }
    }
}
#endif // Windows/Linux

void init_grid(char* grid, int size, const char fill) {
    for (int i = 0; i < size; i++) {
        grid[i] = fill;
    }
}

void print_grid(const char* grid, const int width, const int height) {
    const char border = '#';
    string cstr;
    cstr += "\n";
    for (int i = 0; i < width; i++) {
        cstr += border;
    }
    cstr += "\n";
    //cout << grid[width * height];
    for (int i = SHAPESIZE; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (j == 1) cstr += border;
            cstr += grid[(height * j) + i];
            if (j == width - 2) cstr += border;
        }
        cstr += "\n";
    }
    for (int i = 0; i < width; i++) {
        cstr += border;
    }
    cout << cstr << "\n";
}

bool collision_check(const char* grid, const int width, const int height, const char shape[], const int size, const int xpos, const int ypos, const bool toRight) {
    bool toReturn = true;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (toRight && (ypos + (height * j) + i + ((xpos + 1) * height)) < (width * height) && (ypos + (height * j) + i + ((xpos + 1) * height)) > 1 && (ypos + (height * j) + i + (xpos * height) < width * height && ypos + (height * j) + i + (xpos * height) > 0)) { // Very readable code ahead
                if (grid[ypos + (height * j) + i + ((xpos + 1) * height)] == 'O' && grid[ypos + (height * j) + i + ((xpos)*height)] == 'O' && shape[(size * i) + j] == 'O' && shape[(size * i) + j + 1] != 'O') {
                    toReturn = false;
                }
                if (shape[(size * i) + j] == 'O') {
                    if (xpos + (((size * (i)) + j + 1) % SHAPESIZE) >= width - 1) toReturn = false;
                }
            }
            if (!toRight && (ypos + (height * j) + i + ((xpos - 1) * height)) < (width * height) && (ypos + (height * j) + i + ((xpos - 1) * height)) > 1 && (ypos + (height * j) + i + (xpos * height) < width * height && ypos + (height * j) + i + (xpos * height) > 0)) {
                if (grid[ypos + (height * j) + i + ((xpos - 1) * height)] == 'O' && grid[ypos + (height * j) + i + ((xpos)*height)] == 'O' && shape[(size * i) + j] == 'O' && shape[(size * i) + j - 1] != 'O') {
                    toReturn = false;
                }
                if (shape[(size * i) + j] == 'O') {
                    if (xpos + (((size * i) + j) % SHAPESIZE) <= 0 + 1) toReturn = false;
                }
            }

        }
    }
    return toReturn;
}

bool drop_check(const char* grid, const int width, const int height, const char shape[], const int size, const int xpos, const int ypos) {
    int toReturn = true;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if ((ypos - 1 + (height * j) + i + ((xpos) * height)) < (width * height) && (ypos - 1 + (height * j) + i + ((xpos) * height)) > 1 && (ypos + (height * j) + i + (xpos * height) < (width * height)-1 && ypos + (height * j) + i + (xpos * height) > 0)) {
                if (grid[ypos + 1 + (height * j) + i + ((xpos) * height)] == 'O' && grid[ypos + (height * j) + i + ((xpos)*height)] == 'O' && shape[(size * i) + j] == 'O' && shape[(size * (i + 1)) + j] != 'O') {
                    toReturn = false;
                }
                if (shape[(size * i) + j] == 'O') {
                    if ((ypos + (((size * (j - 1)) + i) % SHAPESIZE)) >= height-2) toReturn = false;
                }
            }
        }
    }
    return toReturn;
}

std::vector<int> save_shape(char* grid, const int width, const int height, const char shape[], const int size, const int xpos, const int ypos, std::vector<int> filled) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (ypos + (height * j) + i + (xpos * height) < width * height && ypos + (height * j) + i + (xpos * height) > 0 && shape[(size * i) + j] == 'O') {
                filled.push_back(ypos+ (height * j) + i + (xpos * height));
            }
        }
    }
    return filled;
}


void render_shape(char* grid, const int width, const int height, const char shape[], const int size, const int xpos, const int ypos) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (ypos + (height * j) + i + (xpos * height) < width * height && ypos + (height * j) + i + (xpos * height) > 0)
                grid[ypos + (height * j) + i + (xpos * height)] = shape[(size * i) + j];
        }
    }
}

void rotate(char* shape, const int shape_grid){ // Could be more efficient, but it's just like 25 bytes so it's fine
    char* new_shape;
    new_shape = (char*)malloc(QSHAPESIZE * sizeof(char));
    memcpy(new_shape, shape, QSHAPESIZE);
    int diff;
    char* ptr = new_shape;
    for (int i = 0; i < shape_grid; i+=1) { // First reverse columns
        for (int j = 0; j < shape_grid; j++) {
            diff = (shape_grid - j - 1);
            new_shape[diff + (i * shape_grid)] = shape[(i * shape_grid) + j];
        }
    }
    memcpy(shape, new_shape, QSHAPESIZE);
    for (int i = 0; i < shape_grid; i+=1) { // Then transpose matrix
        for (int j = 0; j < shape_grid; j++) {
            new_shape[i + (j * shape_grid)] = shape[(i * shape_grid) + j];
        }
    }
    memcpy(shape, new_shape, QSHAPESIZE);
    free(new_shape);
}

int newpos(const char* shape, const int shape_size) {
    int defy = 0;
    bool found;
    for (int i = ((shape_size-1)/2); i < shape_size; i++){
        found = false;
        for (int j = 0; j < shape_size; j++) {
            if (shape[(i * shape_size) + j] == 'O') found = true;
        }
        if (found) defy++;
    }
    return defy;
}

std::vector<int> row_clear(char* grid, const int width, const int height, std::vector<int> filled) {
    bool toClear;
    int arr[width];
    int index = 0;
    std::vector<int> rows;
    for (int i = SHAPESIZE; i < height+SHAPESIZE+1; i++) {
        toClear = true;
        for (int j = 1; j < width - 1; j++) {
            if (grid[(height * j) + i] != 'O') toClear = false;
            else if (grid[(height * j) + i] == 'O' && i < SHAPESIZE+2) {game_quit();}
            arr[j] = (height * j) + i;
        }
        if (toClear) {
            for (int j = 0; j < width; j++) {
                filled.erase(std::remove(filled.begin(), filled.end(), (int)arr[j]), filled.end());
            }
            for (int j = 0; j < filled.size(); j++) {
                if (filled.at(j) % (height) < i)
                    filled.at(j) += 1;
            }
        }
    }
    return filled;
}

void piece_shuffle() {
    std::random_shuffle(&pieces[0], &pieces[6]);
}

int cpiece() {
    ++pindex;
    if (pindex > 6) {
        piece_shuffle();
        pindex = 0;
    }
    return(pieces[pindex]);
}

std::vector<int> getshape(char* grid, const int width, const int height, int* xpos, int* ypos, char* realshape, std::vector<int> rfilled, const int middle) {
    rfilled = save_shape(grid, (width), (height), realshape, SHAPESIZE, *xpos, *ypos, rfilled);
    //int random_range = 0 + (rand() % static_cast<int>(6 - 0 + 1));
    int whichPiece = cpiece();
    if (whichPiece == 0)
        memcpy(realshape, s_shape, QSHAPESIZE);
    else if (whichPiece == 1)
        memcpy(realshape, z_shape, QSHAPESIZE);
    else if (whichPiece == 2)
        memcpy(realshape, i_shape, QSHAPESIZE);
    else if (whichPiece == 3)
        memcpy(realshape, t_shape, QSHAPESIZE);
    else if (whichPiece == 4)
        memcpy(realshape, l_shape, QSHAPESIZE);
    else if (whichPiece == 5)
        memcpy(realshape, rl_shape, QSHAPESIZE);
    else if (whichPiece == 6)
        memcpy(realshape, q_shape, QSHAPESIZE);

    *xpos = middle;
    *ypos = newpos(realshape, SHAPESIZE);
    rfilled = row_clear(grid, width, height, rfilled);
    return rfilled;
}

void prerun(char* grid, const int width, const int height, int* xpos, int* ypos, char* realshape, std::vector<int> rfilled) {
    init_grid(grid, (WIDTH + 2) * (HEIGHT + 1 + SHAPESIZE), '.');
        render_shape(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE), realshape, SHAPESIZE, *xpos, *ypos);
        for (int i = 0; i < (rfilled.size()); i++) {
            grid[rfilled.at(i)] = 'O';
        }
        scr_clear();
}

int main()
{
    srand((unsigned)time(0)); 
    piece_shuffle();
    char* grid;
    grid = new char[(WIDTH + 2) * (HEIGHT + 1 + SHAPESIZE)];
    struct {
        int xpos;
        int ypos;
    } shape;

    int* nxpos = &shape.xpos;
    int* nypos = &shape.ypos;

    const int middle = ((((int)WIDTH-2)-1) / 2);
    std::vector<int> rfilled;
    char* realshape;
    realshape = (char*)malloc(QSHAPESIZE);
    shape.xpos = middle;
    memcpy(realshape, i_shape, QSHAPESIZE);
    getshape(grid, WIDTH+2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled, middle);
    char *mv = new char;
    std::thread key_thread (key_press, std::ref(mv));
    int counter = 0;
    float float_ms=0;
    const int max_counter = 60;
    int toSleep;
    while (true) {
        *mv = 0;
        if (counter > max_counter)
            counter = 0;
        //usleep(10000-float_ms);
        toSleep = 10000-float_ms;
        std::this_thread::sleep_for(std::chrono::microseconds(toSleep));
        //prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
        //print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        auto start = std::chrono::high_resolution_clock::now();

        // Actions
        if (*mv == 'd') {
            if (collision_check(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE), realshape, SHAPESIZE, shape.xpos, shape.ypos, true))
                shape.xpos += 1;
                prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
                print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        }
        else if (*mv == 'a') {
            if (collision_check(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE), realshape, SHAPESIZE, shape.xpos, shape.ypos, false))
                shape.xpos -= 1;
                prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
                print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        }
        else if (*mv == 's' || *mv == 'f') {
            if (*mv == 's' && drop_check(grid, (WIDTH + 2), HEIGHT + 1 + SHAPESIZE, realshape, SHAPESIZE, shape.xpos, shape.ypos)) {
                shape.ypos += 1;
            }
            else if(*mv == 'f') {
                while (true) {
                    if (drop_check(grid, (WIDTH + 2), HEIGHT + 1 + SHAPESIZE, realshape, SHAPESIZE, shape.xpos, shape.ypos)) {
                        shape.ypos += 1;
                        prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
                    }
                    else {
                        rfilled = getshape(grid, WIDTH+2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled, middle);
                        break;
                    }
                }
            }
            else {
                rfilled = getshape(grid, WIDTH+2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled, middle);
            }
            prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
            print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        }
        else if (*mv == 'w') {
            //print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
            rotate(realshape,SHAPESIZE);
            prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
            print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        }
        
        else if (counter == max_counter && drop_check(grid, (WIDTH + 2), HEIGHT + 1 + SHAPESIZE, realshape, SHAPESIZE, shape.xpos, shape.ypos)) {
            shape.ypos += 1;
            prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
            print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        }
        else if (counter == max_counter) {
            rfilled = getshape(grid, WIDTH+2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled, middle);
            prerun(grid, WIDTH + 2, HEIGHT + 1 + SHAPESIZE, nxpos, nypos, realshape, rfilled);
            print_grid(grid, (WIDTH + 2), (HEIGHT + 1 + SHAPESIZE));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> float_ms = end - start;
        counter += 1;
    }

    delete[] grid;
    delete[] realshape;
    delete nypos;
    delete nxpos;
}
