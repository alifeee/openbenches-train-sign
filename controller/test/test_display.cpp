// #include <Adafruit_GFX_mock.h>
#include <graphics.cpp>
#include <iostream>
using namespace std;

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

int xlim = 96;
int ylim = 26;
GFXcanvas1 *canvas = NULL;

int main(int argc, char **argv)
{
    canvas = new GFXcanvas1(xlim, ylim);
    doGraphics(canvas);

    // uint8_t *buffer = canvas->getBuffer();

    cout << "Hello\n";

    int x;
    int y;
    for (y = 0; y <= ylim; y++)
    {
        // printf("%d", y);
        for (x = 0; x <= xlim; x++)
        {
            if (canvas->getPixel(x, y))
                printf("#");
            else
                printf(" ");
        }
        printf("\n");
    }

    cout << "\n"; // otherwise above does not print
}
