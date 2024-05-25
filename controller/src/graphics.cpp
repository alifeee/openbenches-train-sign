#ifdef _WIN32
#include <Adafruit_GFX_mock.h>
#else
#include <Adafruit_GFX.h>
#endif

#include <Fonts/TomThumb.h>
#define FONT_HEIGHT 5
// #include <Fonts/Picopixel.h>
// #define FONT_HEIGHT 6
// #include <Fonts/Tiny3x3a2pt7b.h>
// #define FONT_HEIGHT 3

void doGraphics(GFXcanvas1 *canvas)
{
    canvas->drawPixel(2, 2, 1);

    // canvas->setFont(&TomThumb);
    canvas->setTextSize(1);
    canvas->setCursor(5, 5);
    canvas->setTextSize(1);
    canvas->print("hi :0");
}

void drawText(GFXcanvas1 *canvas, char *text)
{
    canvas->setFont(&TomThumb);
    canvas->setCursor(0, FONT_HEIGHT);
    canvas->setTextSize(1);
    canvas->print(text);
}
