#include <Arduino.h>
#include "yoba/hardware/screen/buffer.h"
#include "yoba/hardware/screen/drivers/ILI9341Driver.h"
#include "yoba/hardware/screen/eightBitsPaletteBuffer.h"
#include "yoba/color.h"

ILI9341Driver driver = ILI9341Driver(ILI9341DriverSettings(
	5,
	16,
	17
));

EightBitsPaletteBuffer buffer = EightBitsPaletteBuffer(
	&driver,
	Size(320, 240)
);

void setup() {
	Serial.begin(115200);

	// Transceiver
	Serial.println("Disabling transceiver");
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	// Display
	Serial.println("Updating palette");

	// Grayscale
	uint8_t govno = 0;

	for (int i = 0; i < 16; i++) {
		buffer.setPaletteColor(i, Color24(govno, govno, govno).toUint16());
		govno += 0x11;
	}

	// RGB
	buffer.setPaletteColor(16, Color24(0xFF, 0x00, 0x00).toUint16());
	buffer.setPaletteColor(17, Color24(0x00, 0xFF, 0x00).toUint16());
	buffer.setPaletteColor(18, Color24(0x00, 0x00, 0xFF).toUint16());

	Serial.println("Beginning display");
	buffer.begin();
//	display.getDriver()->setTransactionBufferHeight(display.getResolution().getHeight() / 2);
}

int32_t paletteIndex = 0;
int32_t paletteIndexInc = 1;
Point pivot = Point(5, 5);
int32_t pivotInc = 1;

void clearDisplay() {
	buffer.clear(paletteIndex);

	paletteIndex += paletteIndexInc;

	if (paletteIndex > 15) {
		paletteIndex = 15;
		paletteIndexInc = -1;
	}
	else if (paletteIndex < 0) {
		paletteIndex = 0;
		paletteIndexInc = 1;
	}
}

void renderPrimitives() {
	Point point = pivot;

	// Dots
	for (int32_t x = point.getX(); x < point.getX() + 100; x += 5)
		buffer.renderPixel(Point(x, point.getY()), 16);

	point.setY(point.getY() + 10);

	// Line
	buffer.renderHorizontalLine(Point(point.getX(), point.getY() + 10), 100, 17);
	point.setY(point.getY() + 10);

	// Rect
	buffer.renderFilledRectangle(Bounds(point.getX(), point.getY() + 15, 100, 30), 18);
	point.setY(point.getY() + 40);

	pivot.setX(pivot.getX() + 1);

	if (pivot.getX() > buffer.getViewport().getX2())
		pivot.setX(0);
}

void loop() {
	auto startTime = esp_timer_get_time();

	clearDisplay();
	renderPrimitives();

	buffer.flush();

	auto deltaTime = esp_timer_get_time() - startTime;
	Serial.printf("FPS: %lld, color: %d, free heap: %d kb, max alloc heap: %d kb\n", 60000000 / deltaTime, paletteIndex, ESP.getFreeHeap() / 1024, ESP.getMaxAllocHeap() / 1024);

	// svit slip....
	uint32_t desiredDeltaTime = 1000 / 60;
	if (deltaTime < desiredDeltaTime)
		delay(desiredDeltaTime - deltaTime);

	//	delay(1000);
}