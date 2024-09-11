#pragma once

#include <cstdlib>
#include <cstring>
#include "renderBuffer.h"

template<typename TValue>
class PaletteBuffer : public RenderBuffer<TValue> {
	public:
		explicit PaletteBuffer(uint16_t *palette);
		void setPaletteColor(TValue index, uint16_t color);
		void clear(TValue paletteIndex) override;

	protected:
		uint16_t* _palette;
};

template<typename TValue>
PaletteBuffer<TValue>::PaletteBuffer(uint16_t *palette) : _palette(palette)  {

}

template<typename TValue>
void PaletteBuffer<TValue>::setPaletteColor(TValue index, uint16_t color) {
	_palette[index] = color;
}

template<typename TValue>
void PaletteBuffer<TValue>::clear(TValue paletteIndex) {
	memset((uint16_t*) this->_buffer, (int) paletteIndex, this->_bufferLength);
}
