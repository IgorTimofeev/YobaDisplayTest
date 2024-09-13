#pragma once
#include "buffer.h"
#include "../../font.h"

template<typename TValue>
class RenderBuffer : public Buffer {
	public:
		RenderBuffer(Driver *driver, const Size &resolution);

		void clear(TValue value);
		void renderPixel(const Point &point, TValue value);
		void renderHorizontalLine(const Point &point, uint16_t width, TValue value);
		void renderVerticalLine(const Point &point, uint16_t height, TValue value);
		void renderFilledRectangle(const Bounds& bounds, TValue value);
		void renderText(const Point& point, Font* font, TValue value, const char* text);
		Size getTextSize(Font* font, const char* text);

	protected:
		virtual void clearNative(TValue value) = 0;
		virtual void renderPixelNative(const Point &point, TValue value) = 0;
		virtual void renderVerticalLineNative(const Point &point, uint16_t width, TValue value) = 0;
		virtual void renderHorizontalLineNative(const Point &point, uint16_t width, TValue value) = 0;
		virtual void renderFilledRectangleNative(const Bounds& bounds, TValue value) = 0;
};

template<typename TValue>
RenderBuffer<TValue>::RenderBuffer(Driver *driver, const Size &resolution) : Buffer(driver, resolution) {

}

template<typename TValue>
void RenderBuffer<TValue>::clear(TValue value) {
	clearNative(value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderPixel(const Point &point, TValue value) {
	if (getViewport().intersects(point))
		renderPixelNative(point, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderHorizontalLine(const Point &point, uint16_t width, TValue value) {
	const auto& viewport = getViewport();

	if (
		point.getX() > viewport.getX2()
		|| point.getX() + width < viewport.getX()

		|| point.getY() < viewport.getY()
		|| point.getY() > viewport.getY2()
	)
		return;

	uint16_t x1 = max(point.getX(), viewport.getX());
	uint16_t x2 = min(point.getX() + width - 1, viewport.getX2());
	width = x2 - x1 + 1;

	renderHorizontalLineNative(Point(x1, point.getY()), width, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderVerticalLine(const Point &point, uint16_t height, TValue value) {
	const auto& viewport = getViewport();

	if (
		point.getX() < viewport.getX()
		|| point.getX() > viewport.getX2()

		|| point.getY() > viewport.getY2()
		|| point.getY() + height < viewport.getY()
	)
		return;

	uint16_t y1 = max(point.getY(), viewport.getY());
	uint16_t y2 = min(point.getY() + height - 1, viewport.getY2());
	height = y2 - y1 + 1;

	renderVerticalLineNative(Point(point.getX(), y1), height, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderFilledRectangle(const Bounds &bounds, TValue value) {
	const auto& viewport = getViewport();

	if (viewport.intersects(bounds))
		renderFilledRectangleNative(viewport.getIntersection(bounds), value);
}

template<typename TValue>
Size RenderBuffer<TValue>::getTextSize(Font *font, const char *text) {
	const char* charPtr;
	size_t charIndex = 0;
	const Glyph* glyph;

	uint16_t width = 0;

	while (true) {
		charPtr = text + charIndex;

		// End of text
		if (*charPtr == '\0')
			break;

		// Trying to find glyph matched to char
		glyph = font->getGlyph(*charPtr);

		if (glyph) {
			width += glyph->getWidth();
		}
		// For non-existing glyphs we can just simulate whitespace
		else {
			width += 10;
		}

		charIndex++;
	}

	return {
		width,
		font->getHeight()
	};
}

template<typename TValue>
void RenderBuffer<TValue>::renderText(const Point &point, Font* font, TValue value, const char* text) {
	const char* charPtr;
	size_t charIndex = 0;
	const Glyph* glyph;

	int32_t x = point.getX();

	uint32_t bitmapBitIndex;
	uint8_t bitmapByte;

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
					if ((bitmapByte >> bitmapBitIndex % 8) & 0b1)
						renderPixel(Point(x + i, point.getY() + j), value);

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