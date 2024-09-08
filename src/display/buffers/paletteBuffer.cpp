#include "paletteBuffer.h"
#include "display/display.h"

PaletteBuffer::PaletteBuffer(uint16_t *palette, ColorDepth colorDepth) : _palette(palette), _colorDepth(colorDepth) {

}

void PaletteBuffer::setPaletteColor(size_t index, uint16_t value) {
	_palette[index] = value;
}

void PaletteBuffer::setDisplay(Display *display) {
	Buffer::setDisplay(display);

	_bufferLength = _display->getWidth() * _display->getHeight() * (uint8_t) _colorDepth / 8;
	_buffer = new uint8_t[_bufferLength];
}

void PaletteBuffer::clear(size_t paletteIndex) {
	memset((uint16_t*) _buffer, _palette[paletteIndex], _bufferLength);
}