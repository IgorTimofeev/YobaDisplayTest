#pragma once
#include "buffer.h"
#include "../display.h"

template<typename TValue>
class RenderBuffer : public Buffer {
	public:
		virtual void clear(TValue value) = 0;

		void renderPixel(const Point &point, TValue value);
		void renderHorizontalLine(const Point &point, uint16_t width, TValue value);
		void renderFilledRectangle(const Bounds& bounds, TValue value);

	protected:
		virtual void renderPixelUnchecked(const Point &point, TValue value) = 0;
		virtual void renderHorizontalLineUnchecked(const Point &point, uint16_t width, TValue value) = 0;
		virtual void renderFilledRectangleUnchecked(const Bounds& bounds, TValue value) = 0;
};

template<typename TValue>
void RenderBuffer<TValue>::renderPixel(const Point &point, TValue value) {
	if (_display->getViewport().intersects(point))
		renderPixelUnchecked(point, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderHorizontalLine(const Point &point, uint16_t width, TValue value) {
	const auto& viewport = _display->getViewport();

	if (point.getX() > viewport.getWidth() || point.getX() + width < viewport.getX())
		return;

	uint16_t x1 = max(point.getX(), viewport.getX());
	width = min(point.getX() + width, (int32_t) viewport.getWidth()) - x1;

	renderHorizontalLineUnchecked(Point(x1, point.getY()), width, value);
}

template<typename TValue>
void RenderBuffer<TValue>::renderFilledRectangle(const Bounds &bounds, TValue value) {
	const auto& viewport = _display->getViewport();

	if (viewport.intersects(bounds))
		renderFilledRectangleUnchecked(viewport.getIntersection(bounds), value);
}