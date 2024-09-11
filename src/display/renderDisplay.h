#pragma once
#include "display.h"
#include "display.h"

template<typename TValue>
class RenderDisplay : public Display {
	public:
		RenderDisplay(Driver *driver, const Size &resolution);

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
RenderDisplay<TValue>::RenderDisplay(Driver *driver, const Size &resolution) : Display(driver, resolution) {

}

template<typename TValue>
void RenderDisplay<TValue>::renderPixel(const Point &point, TValue value) {
	if (getViewport().intersects(point))
		renderPixelUnchecked(point, value);
}

template<typename TValue>
void RenderDisplay<TValue>::renderHorizontalLine(const Point &point, uint16_t width, TValue value) {
	const auto& viewport = getViewport();

	if (point.getX() > viewport.getWidth() || point.getX() + width < viewport.getX())
		return;

	uint16_t x1 = max(point.getX(), viewport.getX());
	width = min(point.getX() + width, (int32_t) viewport.getWidth()) - x1;

	renderHorizontalLineUnchecked(Point(x1, point.getY()), width, value);
}

template<typename TValue>
void RenderDisplay<TValue>::renderFilledRectangle(const Bounds &bounds, TValue value) {
	const auto& viewport = getViewport();

	if (viewport.intersects(bounds))
		renderFilledRectangleUnchecked(viewport.getIntersection(bounds), value);
}