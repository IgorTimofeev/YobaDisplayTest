#pragma once

#include "Arduino.h"
#include <cstdint>
#include "point.h"
#include "size.h"

class Bounds {
	public:
		Bounds(int32_t x, int32_t y, int32_t width, int32_t height) :
			_x(x),
			_y(y),
			_width(width),
			_height(height)
		{

		}

		Bounds() : Bounds(0, 0, 0, 0) {

		}

		Bounds(const Point &position, const Size &size) : Bounds(position.getX(), position.getY(), size.getWidth(), size.getHeight()) {

		}

		explicit Bounds(const Size &size) : Bounds(Point(), size) {

		}

		int32_t getX() const {
			return _x;
		}

		void setX(int32_t value) {
			_x = value;
		}

		int32_t getY() const {
			return _y;
		}

		void setY(int32_t value) {
			_y = value;
		}

		uint16_t getWidth() const {
			return _width;
		}

		void setWidth(uint16_t value) {
			_width = value;
		}

		uint16_t getHeight() const {
			return _height;
		}

		void setHeight(uint16_t value) {
			_height = value;
		}

		Point getPosition() const {
			return {getX(), getY()};
		}

		Point getSize() const {
			return {getWidth(), getHeight()};
		}

		int32_t getX2() const {
			return _x + _width;
		}

		int32_t getY2() const {
			return _y + _height;
		}

		int32_t getXCenter() const {
			return _x + _width / 2;
		}

		int32_t getYCenter() const {
			return _y + _height / 2;
		}

		bool intersects(const Point& point) const {
			return
				point.getX() >= _x
				&& point.getY() >= _y
				&& point.getX() < getX2()
				&& point.getY() < getY2();
		}

		bool intersects(const Bounds& bounds) const {
			return !(
				getX() > bounds.getX2()
				|| bounds.getX() > getX2()
				|| getY() > bounds.getY2()
				|| bounds.getY() > getY2()
			);
		}

		Bounds getIntersection(const Bounds& bounds) const {
			Bounds result = Bounds();

			result.setX(max(getX(), bounds.getX()));
			result.setY(max(getY(), bounds.getY()));
			result.setWidth(min(getX2(), bounds.getX2()) - result.getX());
			result.setHeight(min(getY2(), bounds.getY2()) - result.getY());

			return result;
		}

	private:
		int32_t _x = 0;
		int32_t _y = 0;
		uint16_t _width = 0;
		uint16_t _height = 0;
};