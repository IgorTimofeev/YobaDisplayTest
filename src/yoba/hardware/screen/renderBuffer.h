#pragma once
#include "buffer.h"
#include "../../font.h"

template<typename TValue>
class RenderBuffer : public Buffer {
	public:
		RenderBuffer(Driver *driver, const Size &resolution);

		virtual void clear(TValue value) = 0;

		void renderPixel(const Point &point, TValue value);
		void renderHorizontalLine(const Point &point, uint16_t width, TValue value);
		void renderFilledRectangle(const Bounds& bounds, TValue value);
		void renderText(const Point& point, Font* font, TValue value, const char* text, uint8_t scale = 1);

	protected:
		virtual void renderPixelUnchecked(const Point &point, TValue value) = 0;
		virtual void renderHorizontalLineUnchecked(const Point &point, uint16_t width, TValue value) = 0;
		virtual void renderFilledRectangleUnchecked(const Bounds& bounds, TValue value) = 0;
};

template<typename TValue>
RenderBuffer<TValue>::RenderBuffer(Driver *driver, const Size &resolution) : Buffer(driver, resolution) {

}

template<typename TValue>
void RenderBuffer<TValue>::renderPixel(const Point &point, TValue value) {
	if (getViewport().intersects(point))
		renderPixelUnchecked(point, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderHorizontalLine(const Point &point, uint16_t width, TValue value) {
	const auto& viewport = getViewport();

	if (point.getX() > viewport.getWidth() || point.getX() + width < viewport.getX())
		return;

	uint16_t x1 = max(point.getX(), viewport.getX());
	width = min(point.getX() + width, (int32_t) viewport.getWidth()) - x1;

	renderHorizontalLineUnchecked(Point(x1, point.getY()), width, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderFilledRectangle(const Bounds &bounds, TValue value) {
	const auto& viewport = getViewport();

	if (viewport.intersects(bounds))
		renderFilledRectangleUnchecked(viewport.getIntersection(bounds), value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderText(const Point &point, Font* font, TValue value, const char* text, uint8_t scale) {
	const char* charPtr;
	size_t charIndex = 0;
	const Glyph* glyph;

	uint32_t bitmapBitIndex;
	uint8_t bitmapByte;

	int32_t x = point.getX();

	while (true) {
		charPtr = text + charIndex;

		// End of text
		if (*charPtr == '\0')
			break;

		// Trying to find glyph matched to char
		glyph = font->getGlyph(*charPtr);

		if (glyph) {
			bitmapBitIndex = glyph->getBitmapBitIndex();

			for (int j = 0; j < font->getHeight(); j++) {
				for (int i = 0; i < glyph->getWidth(); i++) {
					bitmapByte = font->getBitmap()[bitmapBitIndex / 8];

					// We have pixel!
					if ((bitmapByte >> bitmapBitIndex % 8) & 0b1) {
						renderPixel(Point(x + i, point.getY() + j), value);
					}

					bitmapBitIndex++;
				}
			}

			x += glyph->getWidth();
		}
		// For non-existing glyphs we can just simulate whitespace
		else {
			x += 10;
		}

		charIndex++;
	}
}