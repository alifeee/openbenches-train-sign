#include "Adafruit_GFX_mock.h"
#include "glcdfont.c"

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c)
{
    return gfxFont->glyph + c;
}
inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont)
{
    return gfxFont->bitmap;
}

size_t Print::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;
    while (size--)
    {
        if (write(*buffer++))
            n++;
        else
            break;
    }
    return n;
}

size_t Print::print(const char str[])
{
    return write(str);
}

size_t Print::print(char c)
{
    return write(c);
}

Adafruit_GFX::Adafruit_GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h)
{
    _width = WIDTH;
    _height = HEIGHT;
    rotation = 0;
    cursor_y = cursor_x = 0;
    textsize_x = textsize_y = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap = true;
    _cp437 = false;
    gfxFont = NULL;
}

void Adafruit_GFX::startWrite() {}
void Adafruit_GFX::writePixel(int16_t x, int16_t y, uint16_t color)
{
    drawPixel(x, y, color);
}
void Adafruit_GFX::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    fillRect(x, y, w, h, color);
}
void Adafruit_GFX::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    drawFastVLine(x, y, h, color);
}
void Adafruit_GFX::endWrite() {}

void Adafruit_GFX::setTextSize(uint8_t s) { setTextSize(s, s); }
void Adafruit_GFX::setTextSize(uint8_t s_x, uint8_t s_y)
{
    textsize_x = (s_x > 0) ? s_x : 1;
    textsize_y = (s_y > 0) ? s_y : 1;
}

void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                            uint16_t color)
{
    startWrite();
    for (int16_t i = x; i < x + w; i++)
    {
        writeFastVLine(i, y, h, color);
    }
    endWrite();
}

void Adafruit_GFX::setFont(const GFXfont *f)
{
    if (f)
    { // Font struct pointer passed in?
        if (!gfxFont)
        { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
            cursor_y += 6;
        }
    }
    else if (gfxFont)
    { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
        cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
}

GFXcanvas1::GFXcanvas1(uint16_t w, uint16_t h) : Adafruit_GFX(w, h)
{
    uint16_t bytes = ((w + 7) / 8) * h;
    if ((buffer = (uint8_t *)malloc(bytes)))
    {
        memset(buffer, 0, bytes);
    }
}
GFXcanvas1::~GFXcanvas1(void)
{
    if (buffer)
        free(buffer);
}

void GFXcanvas1::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (buffer)
    {
        if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
            return;

        int16_t t;
        switch (rotation)
        {
        case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
        }

        uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
        if (color)
            *ptr |= 0x80 >> (x & 7);
        else
            *ptr &= ~(0x80 >> (x & 7));
    }
}

void GFXcanvas1::fillScreen(uint16_t color)
{
    if (buffer)
    {
        uint16_t bytes = ((WIDTH + 7) / 8) * HEIGHT;
        memset(buffer, color ? 0xFF : 0x00, bytes);
    }
}

void GFXcanvas1::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{

    if (h < 0)
    { // Convert negative heights to positive equivalent
        h *= -1;
        y -= h - 1;
        if (y < 0)
        {
            h += y;
            y = 0;
        }
    }

    // Edge rejection (no-draw if totally off canvas)
    if ((x < 0) || (x >= width()) || (y >= height()) || ((y + h - 1) < 0))
    {
        return;
    }

    if (y < 0)
    { // Clip top
        h += y;
        y = 0;
    }
    if (y + h > height())
    { // Clip bottom
        h = height() - y;
    }

    if (getRotation() == 0)
    {
        drawFastRawVLine(x, y, h, color);
    }
    else if (getRotation() == 1)
    {
        int16_t t = x;
        x = WIDTH - 1 - y;
        y = t;
        x -= h - 1;
        drawFastRawHLine(x, y, h, color);
    }
    else if (getRotation() == 2)
    {
        x = WIDTH - 1 - x;
        y = HEIGHT - 1 - y;

        y -= h - 1;
        drawFastRawVLine(x, y, h, color);
    }
    else if (getRotation() == 3)
    {
        int16_t t = x;
        x = y;
        y = HEIGHT - 1 - t;
        drawFastRawHLine(x, y, h, color);
    }
}
void GFXcanvas1::drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    // x & y already in raw (rotation 0) coordinates, no need to transform.
    int16_t row_bytes = ((WIDTH + 7) / 8);
    uint8_t *buffer = this->getBuffer();
    uint8_t *ptr = &buffer[(x / 8) + y * row_bytes];

    if (color > 0)
    {
        uint8_t bit_mask = (0x80 >> (x & 7));
        for (int16_t i = 0; i < h; i++)
        {
            *ptr |= bit_mask;
            ptr += row_bytes;
        }
    }
    else
    {
        uint8_t bit_mask = ~(0x80 >> (x & 7));
        for (int16_t i = 0; i < h; i++)
        {
            *ptr &= bit_mask;
            ptr += row_bytes;
        }
    }
}
void GFXcanvas1::drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                  uint16_t color)
{
    // x & y already in raw (rotation 0) coordinates, no need to transform.
    int16_t rowBytes = ((WIDTH + 7) / 8);
    uint8_t *buffer = this->getBuffer();
    uint8_t *ptr = &buffer[(x / 8) + y * rowBytes];
    size_t remainingWidthBits = w;

    // check to see if first byte needs to be partially filled
    if ((x & 7) > 0)
    {
        // create bit mask for first byte
        uint8_t startByteBitMask = 0x00;
        for (int8_t i = (x & 7); ((i < 8) && (remainingWidthBits > 0)); i++)
        {
            startByteBitMask |= (0x80 >> i);
            remainingWidthBits--;
        }
        if (color > 0)
        {
            *ptr |= startByteBitMask;
        }
        else
        {
            *ptr &= ~startByteBitMask;
        }

        ptr++;
    }

    // do the next remainingWidthBits bits
    if (remainingWidthBits > 0)
    {
        size_t remainingWholeBytes = remainingWidthBits / 8;
        size_t lastByteBits = remainingWidthBits % 8;
        uint8_t wholeByteColor = color > 0 ? 0xFF : 0x00;

        memset(ptr, wholeByteColor, remainingWholeBytes);

        if (lastByteBits > 0)
        {
            uint8_t lastByteBitMask = 0x00;
            for (size_t i = 0; i < lastByteBits; i++)
            {
                lastByteBitMask |= (0x80 >> i);
            }
            ptr += remainingWholeBytes;

            if (color > 0)
            {
                *ptr |= lastByteBitMask;
            }
            else
            {
                *ptr &= ~lastByteBitMask;
            }
        }
    }
}

bool GFXcanvas1::getPixel(int16_t x, int16_t y) const
{
    int16_t t;
    switch (rotation)
    {
    case 1:
        t = x;
        x = WIDTH - 1 - y;
        y = t;
        break;
    case 2:
        x = WIDTH - 1 - x;
        y = HEIGHT - 1 - y;
        break;
    case 3:
        t = x;
        x = y;
        y = HEIGHT - 1 - t;
        break;
    }
    return getRawPixel(x, y);
}

bool GFXcanvas1::getRawPixel(int16_t x, int16_t y) const
{
    if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
        return 0;
    if (this->getBuffer())
    {
        uint8_t *buffer = this->getBuffer();
        uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];

        return ((*ptr) & (0x80 >> (x & 7))) != 0;
    }
    return 0;
}

size_t Adafruit_GFX::write(uint8_t c)
{
    if (!gfxFont)
    { // 'Classic' built-in font

        if (c == '\n')
        {                               // Newline?
            cursor_x = 0;               // Reset x to zero,
            cursor_y += textsize_y * 8; // advance y one line
        }
        else if (c != '\r')
        { // Ignore carriage returns
            if (wrap && ((cursor_x + textsize_x * 6) > _width))
            {                               // Off right?
                cursor_x = 0;               // Reset x to zero,
                cursor_y += textsize_y * 8; // advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                     textsize_y);
            cursor_x += textsize_x * 6; // Advance x one char
        }
    }
    else
    { // Custom font

        if (c == '\n')
        {
            cursor_x = 0;
            cursor_y +=
                (int16_t)textsize_y * (uint8_t)gfxFont->yAdvance;
        }
        else if (c != '\r')
        {
            uint8_t first = gfxFont->first;
            if ((c >= first) && (c <= (uint8_t)gfxFont->last))
            {
                GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
                uint8_t w = glyph->width,
                        h = glyph->height;
                if ((w > 0) && (h > 0))
                {                                        // Is there an associated bitmap?
                    int16_t xo = (int8_t)glyph->xOffset; // sic
                    if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width))
                    {
                        cursor_x = 0;
                        cursor_y += (int16_t)textsize_y *
                                    (uint8_t)gfxFont->yAdvance;
                    }
                    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                             textsize_y);
                }
                cursor_x +=
                    (uint8_t)glyph->xAdvance * (int16_t)textsize_x;
            }
        }
    }
    return 1;
}

void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
    drawChar(x, y, c, color, bg, size, size);
}
void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y)
{

    if (!gfxFont)
    { // 'Classic' built-in font

        if ((x >= _width) ||              // Clip right
            (y >= _height) ||             // Clip bottom
            ((x + 6 * size_x - 1) < 0) || // Clip left
            ((y + 8 * size_y - 1) < 0))   // Clip top
            return;

        if (!_cp437 && (c >= 176))
            c++; // Handle 'classic' charset behavior

        startWrite();
        for (int8_t i = 0; i < 5; i++)
        { // Char bitmap = 5 columns
            uint8_t line = font[c * 5 + i];
            for (int8_t j = 0; j < 8; j++, line >>= 1)
            {
                if (line & 1)
                {
                    if (size_x == 1 && size_y == 1)
                        writePixel(x + i, y + j, color);
                    else
                        writeFillRect(x + i * size_x, y + j * size_y, size_x, size_y,
                                      color);
                }
                else if (bg != color)
                {
                    if (size_x == 1 && size_y == 1)
                        writePixel(x + i, y + j, bg);
                    else
                        writeFillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
                }
            }
        }
        if (bg != color)
        { // If opaque, draw vertical line for last column
            if (size_x == 1 && size_y == 1)
                writeFastVLine(x + 5, y, 8, bg);
            else
                writeFillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
        }
        endWrite();
    }
    else
    { // Custom font

        // Character is assumed previously filtered by write() to eliminate
        // newlines, returns, non-printable characters, etc.  Calling
        // drawChar() directly with 'bad' characters of font may cause mayhem!

        c -= (uint8_t)gfxFont->first;
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
        uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

        uint16_t bo = glyph->bitmapOffset;
        uint8_t w = glyph->width, h = glyph->height;
        int8_t xo = glyph->xOffset,
               yo = glyph->yOffset;
        uint8_t xx, yy, bits = 0, bit = 0;
        int16_t xo16 = 0, yo16 = 0;

        if (size_x > 1 || size_y > 1)
        {
            xo16 = xo;
            yo16 = yo;
        }

        // Todo: Add character clipping here

        // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
        // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
        // has typically been used with the 'classic' font to overwrite old
        // screen contents with new data.  This ONLY works because the
        // characters are a uniform size; it's not a sensible thing to do with
        // proportionally-spaced fonts with glyphs of varying sizes (and that
        // may overlap).  To replace previously-drawn text when using a custom
        // font, use the getTextBounds() function to determine the smallest
        // rectangle encompassing a string, erase the area with fillRect(),
        // then draw new text.  This WILL infortunately 'blink' the text, but
        // is unavoidable.  Drawing 'background' pixels will NOT fix this,
        // only creates a new set of problems.  Have an idea to work around
        // this (a canvas object type for MCUs that can afford the RAM and
        // displays supporting setAddrWindow() and pushColors()), but haven't
        // implemented this yet.

        startWrite();
        for (yy = 0; yy < h; yy++)
        {
            for (xx = 0; xx < w; xx++)
            {
                if (!(bit++ & 7))
                {
                    bits = bitmap[bo++];
                }
                if (bits & 0x80)
                {
                    if (size_x == 1 && size_y == 1)
                    {
                        writePixel(x + xo + xx, y + yo + yy, color);
                    }
                    else
                    {
                        writeFillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
                                      size_x, size_y, color);
                    }
                }
                bits <<= 1;
            }
        }
        endWrite();

    } // End classic vs custom font
}
