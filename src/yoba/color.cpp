#include "color.h"
#include "Arduino.h"

const Color16 Color16::black = Color16(0x0000);
const Color16 Color16::white = Color16(0xFFFF);

Color24::Color24(uint8_t r, uint8_t g, uint8_t b) :
	_r(r),
	_g(g),
	_b(b)
{

}

Color24::Color24(const Color24 &source) {
	_r = source._r;
	_g = source._g;
	_b = source._b;
}

Color24::Color24(const ColorHSB &hsb) {
	auto hueSector = hsb._h * 6.0f;
	auto hueSectorIntegerPart = (uint8_t) hueSector;
	auto hueSectorFractionalPart = hueSector - (float) hueSectorIntegerPart;

	auto
		p = (uint8_t) (255.0f * hsb._b * (1 - hsb._s)),
		q = (uint8_t) (255.0f * hsb._b * (1 - hueSectorFractionalPart * hsb._s)),
		t = (uint8_t) (255.0f * hsb._b * (1 - (1 - hueSectorFractionalPart) * hsb._s)),
		v = (uint8_t) (255.0f * hsb._b);

	switch (hueSectorIntegerPart) {
		case 1:
			_r = q;
			_g = v;
			_b = p;
			break;

		case 2:
			_r = p;
			_g = v;
			_b = t;
			break;

		case 3:
			_r = p;
			_g = q;
			_b = v;
			break;

		case 4:
			_r = t;
			_g = p;
			_b = v;
			break;

		case 5:
			_r = v;
			_g = p;
			_b = q;
			break;

		default:
			_r = v;
			_g = t;
			_b = p;
			break;
	}
}

Color24::Color24() {
	_r = 0;
	_g = 0;
	_b = 0;
}

Color24::Color24(uint32_t value) {
	_r = value >> 16 & 0xFF;
	_g = value >> 8 & 0xFF;
	_b = value & 0xFF;
}

void Color24::add(const Color24 &color) {
	add(color._r, color._g, color._b);
}

void Color24::add(uint8_t r, uint8_t g, uint8_t b) {
	_r = (uint8_t) constrain(_r + r, 0, 255);
	_g = (uint8_t) constrain(_g + g, 0, 255);
	_b = (uint8_t) constrain(_b + b, 0, 255);
}

void Color24::multiply(float factor) {
	_r = (uint8_t) constrain((float) _r * factor, 0.0f, 255.0f);
	_g = (uint8_t) constrain((float) _g * factor, 0.0f, 255.0f);
	_b = (uint8_t) constrain((float) _b * factor, 0.0f, 255.0f);
}

uint32_t Color24::to24Bit() const {
	return _r << 16 | _g << 8 | _b;
}

uint16_t Color24::toUint16() const {
	return (_r >> 3) << 3 | (_g >> 5) | ((_g >> 2) << 13) | ((_b >> 3) << 8);
}

uint8_t Color24::interpolateChannel(uint8_t first, uint8_t second, float position) {
	return (uint8_t) ((float) first + (float) (second - first) * position);
}

void Color24::interpolateTo(Color24 &second, float position) {
	_r = interpolateChannel(_r, second._r, position);
	_g = interpolateChannel(_g, second._g, position);
	_b = interpolateChannel(_b, second._b, position);
}

uint8_t Color24::getR() const {
	return _r;
}

void Color24::setR(uint8_t r) {
	_r = r;
}

uint8_t Color24::getG() const {
	return _g;
}

void Color24::setG(uint8_t g) {
	_g = g;
}

uint8_t Color24::getB() const {
	return _b;
}

void Color24::setB(uint8_t b) {
	_b = b;
}

// ------------------------------------------------------------------------------------

ColorHSB::ColorHSB(float h, float s, float b) : _h(h), _s(s), _b(b) {

}

// ------------------------------------------------------------------------------------

Color16::Color16(uint16_t value) : _value(value) {

}

uint16_t Color16::toUint16() const {
	return _value;
}

// ------------------------------------------------------------------------------------

ColorPalette::ColorPalette(uint8_t index) : _index(index) {

}

uint16_t ColorPalette::toUint16() const {
	return _index;
}