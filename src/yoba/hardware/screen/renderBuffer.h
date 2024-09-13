#pragma once

#include "buffer.h"
#include "../../font.h"

template<typename TColor>
class RenderBuffer : public Buffer {
	public:
		RenderBuffer(Driver *driver, const Size &resolution);

		void clear(TColor color);
		void renderPixel(const Point &point, TColor color);
		void renderHorizontalLine(const Point &point, uint16_t length, TColor color);
		void renderVerticalLine(const Point &point, uint16_t length, TColor color);
		void renderFilledRectangle(const Bounds& bounds, TColor color);
		void renderLine(const Point& from, const Point& to, TColor color);
		void renderText(const Point& point, Font* font, TColor color, const char* text);
		Size getTextSize(Font* font, const char* text);

	protected:
		virtual void clearNative(TColor color) = 0;
		virtual void renderPixelNative(const Point &point, TColor color) = 0;
		virtual void renderVerticalLineNative(const Point &point, uint16_t width, TColor color) = 0;
		virtual void renderHorizontalLineNative(const Point &point, uint16_t width, TColor color) = 0;
		virtual void renderFilledRectangleNative(const Bounds& bounds, TColor color) = 0;
};

template<typename TColor>
RenderBuffer<TColor>::RenderBuffer(Driver *driver, const Size &resolution) : Buffer(driver, resolution) {

}

template<typename TColor>
void RenderBuffer<TColor>::clear(TColor color) {
	clearNative(color);
}

template<typename TColor>
void RenderBuffer<TColor>::renderPixel(const Point &point, TColor color) {
	if (getViewport().intersects(point))
		renderPixelNative(point, color);
}

template<typename TColor>
void RenderBuffer<TColor>::renderHorizontalLine(const Point &point, uint16_t length, TColor color) {
	const auto& viewport = getViewport();

	if (
		point.getX() > viewport.getX2()
		|| point.getX() + length < viewport.getX()

		|| point.getY() < viewport.getY()
		|| point.getY() > viewport.getY2()
	)
		return;

	uint16_t x1 = max(point.getX(), viewport.getX());
	uint16_t x2 = min(point.getX() + length - 1, viewport.getX2());
	length = x2 - x1 + 1;

	renderHorizontalLineNative(Point(x1, point.getY()), length, color);
}

template<typename TColor>
void RenderBuffer<TColor>::renderVerticalLine(const Point &point, uint16_t length, TColor color) {
	const auto& viewport = getViewport();

	if (
		point.getX() < viewport.getX()
		|| point.getX() > viewport.getX2()

		|| point.getY() > viewport.getY2()
		|| point.getY() + length < viewport.getY()
	)
		return;

	uint16_t y1 = max(point.getY(), viewport.getY());
	uint16_t y2 = min(point.getY() + length - 1, viewport.getY2());
	length = y2 - y1 + 1;

	renderVerticalLineNative(Point(point.getX(), y1), length, color);
}

template<typename TColor>
void RenderBuffer<TColor>::renderLine(const Point &from, const Point &to, TColor color) {
	// Vertical line
	if (from.getX() == to.getX()) {
		renderVerticalLine(from, to.getY() - from.getY() + 1, color);
	}
	// Horizontal line
	else if (from.getY() == to.getY()) {
		renderHorizontalLine(from, to.getX() - from.getX() + 1, color);
	}
	// Meh...
	else {
		int32_t
			x1 = from.getX(),
			y1 = from.getY(),

			x2 = to.getX(),
			y2 = to.getY();

		bool deltaYGreater = abs(y2 - y1) > abs(x2 - x1);

		if (deltaYGreater) {
			std::swap(x1, y1);
			std::swap(x2, y2);
		}

		if (x1 > x2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		int32_t
			deltaX = x2 - x1,
			deltaY = abs(y2 - y1),

			partRemaining = deltaX,
			yStep = -1,
			partX = x1;

		uint16_t partLength = 0;

		if (y1 < y2)
			yStep = 1;

		// Split into steep and not steep for FastH/V separation
		if (deltaYGreater) {
			while (x1 <= x2) {
				partLength++;
				partRemaining -= deltaY;

				if (partRemaining < 0) {
					partRemaining += deltaX;

					if (partLength == 1) {
						renderPixel(Point(y1, partX), color);
					}
					else {
						renderVerticalLine(Point(y1, partX), partLength, color);
					}

					partLength = 0;
					partX = x1 + 1;
					y1 += yStep;
				}

				x1++;
			}

			if (partLength > 0)
				renderVerticalLine(Point(y1, partX), partLength, color);
		}
		else {
			while (x1 <= x2) {
				partLength++;
				partRemaining -= deltaY;

				if (partRemaining < 0) {
					partRemaining += deltaX;

					if (partLength == 1) {
						renderPixel(Point(partX, y1), color);
					}
					else {
						renderHorizontalLine(Point(partX, y1), partLength, color);
					}

					partLength = 0;
					partX = x1 + 1;
					y1 += yStep;
				}

				x1++;
			}

			if (partLength > 0)
				renderHorizontalLine(Point(partX, y1), partLength, color);
		}
	}
}

template<typename TColor>
void RenderBuffer<TColor>::renderFilledRectangle(const Bounds &bounds, TColor color) {
	const auto& viewport = getViewport();

	if (viewport.intersects(bounds))
		renderFilledRectangleNative(viewport.getIntersection(bounds), color);
}

template<typename TColor>
Size RenderBuffer<TColor>::getTextSize(Font *font, const char *text) {
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

template<typename TColor>
void RenderBuffer<TColor>::renderText(const Point &point, Font* font, TColor color, const char* text) {
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
						renderPixel(Point(x + i, point.getY() + j), color);

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