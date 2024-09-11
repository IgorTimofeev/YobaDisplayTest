#pragma once

#include <cstdlib>
#include <cstring>
#include "buffer.h"

enum class ColorDepth : uint8_t {
	One = 1,
	Two = 2,
	Four = 4,
	Eight = 8,
	Sixteen = 16,
	ThirtyTwo = 32
};

template<typename TValue>
class PaletteBuffer : public TypedBuffer<TValue> {
	public:
		explicit PaletteBuffer(uint16_t *palette);

		void setPaletteColor(TValue index, uint16_t value);

		void clear(TValue paletteIndex) override;
		void renderPixel(int32_t x, int32_t y, TValue value) override;

	protected:
		uint16_t* _palette;
};

template<typename TValue>
PaletteBuffer<TValue>::PaletteBuffer(uint16_t *palette) : _palette(palette)  {

}

template<typename TValue>
void PaletteBuffer<TValue>::setPaletteColor(TValue index, uint16_t value) {
	_palette[index] = value;
}

template<typename TValue>
void PaletteBuffer<TValue>::clear(TValue paletteIndex) {
	memset((uint16_t*) this->_buffer, (int) paletteIndex, this->_bufferLength);
}

template<typename TValue>
void PaletteBuffer<TValue>::renderPixel(int32_t x, int32_t y, TValue value) {

}
