#include "Arduino_LED_Matrix.h"

#define MAX_X 12
#define MAX_Y  8

uint8_t grid[MAX_Y][MAX_X] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

ArduinoLEDMatrix matrix;

struct point_t {
    double x, y, dx, dy;

    point_t() : x(0.0), y(0.0), dx(0.0), dy(0.0) {}
    point_t(int _x, int _y, int _dx, int _dy) : 
        x(_x), y(_y), dx(_dx), dy(_dy) {}

    point_t(double _x, double _y, double _dx, double _dy) :
        x(_x), y(_y), dx(_dx), dy(_dy) {}

    void set() { grid[(int) y][(int) x] = 1; }

    void reset() { grid[(int) y][(int) x] = 0; }

    void update() {
        if ((x+dx) < 0 || (x-dx) < 0 || 
        (x+dx) >= MAX_X || (x-dx) >= MAX_X) { dx *= -1; }

        if ((y+dy) < 0 || (y-dy) < 0 || 
        (y+dy) >= MAX_Y || (y-dy) >= MAX_Y) { dy *= -1; }

        x += dx;
        y += dy;
    }
};

#define   DELAY       20
#define   MAX_POINTS   4

point_t points[MAX_POINTS];

void setup() {
    delay(1000);
    Serial.begin(9600);
    while(!Serial){}
    delay(1000);
    
    Serial.println("Arduino LED Matrix");
    matrix.begin();

    pinMode(A0, INPUT);
    randomSeed(analogRead(A0));

    int const min_num = 2;
    int const max_num = 4;

    auto rand_lambda = []() -> double { 
        return (1.0 / (double) random(min_num, max_num));
    };

    for (point_t &pt : points) {
        pt.x = random(0, MAX_X);
        pt.y = random(0, MAX_Y);
        pt.dx = random(2) ? -rand_lambda() : +rand_lambda();
        pt.dy = random(2) ? -rand_lambda() : +rand_lambda();
    }
    pinMode(21, OUTPUT);
    digitalWrite(21, LOW);
}

void loop() {
    for (point_t &pt : points) { pt.set(); }
    matrix.renderBitmap(grid, 8, 12);

    delay(DELAY);

    for (point_t &pt : points) { pt.reset(); }
    matrix.renderBitmap(grid, 8, 12);

    for (point_t &pt : points) { pt.update(); }
}
