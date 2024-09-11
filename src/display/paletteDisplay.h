#pragma once

#include <cstdlib>
#include <cstring>
#include "renderDisplay.h"

template<typename TValue>
class PaletteDisplay : public RenderDisplay<TValue> {
	public:
		PaletteDisplay(Driver *driver, const Size &size, uint16_t *palette);

		void setPaletteColor(TValue index, uint16_t color);
		void clear(TValue paletteIndex) override;

	protected:
		uint16_t* _palette;
};

template<typename TValue>
PaletteDisplay<TValue>::PaletteDisplay(Driver *driver, const Size &size, uint16_t *palette) : RenderDisplay<TValue>(driver, size), _palette(palette) {

}

template<typename TValue>
void PaletteDisplay<TValue>::setPaletteColor(TValue index, uint16_t color) {
	_palette[index] = color;
}

template<typename TValue>
void PaletteDisplay<TValue>::clear(TValue paletteIndex) {
	memset((uint16_t*) this->_buffer, (int) paletteIndex, this->_bufferLength);
}
