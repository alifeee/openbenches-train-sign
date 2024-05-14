#ifdef _WIN32
#include <Adafruit_GFX_mock.h>
#else
#include <Adafruit_GFX.h>
#endif

void doGraphics(GFXcanvas1 *canvas)
{
    canvas->drawPixel(2, 2, 1);

    canvas->setCursor(5, 5);
    canvas->setTextSize(1);
    canvas->print("hi :0");
}
