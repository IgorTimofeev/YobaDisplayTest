#include "buffer.h"
#include "display/display.h"

void Buffer::flush() {

}


void Buffer::setDisplay(Display* display) {
	_display = display;
}

template<typename TColor>
void TypedBuffer<TColor>::clear(TColor value) {

}

template<typename TColor>
void TypedBuffer<TColor>::renderPixel(int32_t x, int32_t y, TColor value) {

}