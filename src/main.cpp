#include <Arduino.h>
#include "display/display.h"
#include "display/drivers/ILI9341Driver.h"
#include "display/buffers/eightBitsPaletteBuffer.h"

ILI9341Driver driver = ILI9341Driver(
	5,
	16,
	17
);

EightBitsPaletteBuffer buffer;

Display display = Display(
	&driver,
	&buffer,
	Size(320, 240)
);

void setup() {
	Serial.begin(115200);

	// Transceiver
	Serial.println("Disabling Transceiver");
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	// Display
	Serial.println("Updating palette");

	uint8_t govno = 0;

	for (int i = 0; i < 16; i++) {
		buffer.setPaletteColor(i, Color24(govno, govno, govno).toUint16());
		govno += 0x11;
	}

	Serial.println("Beginning display");
	display.begin();
}

int32_t color = 0;
int32_t colorStep = 1;

void loop() {
	auto startTime = esp_timer_get_time();

	buffer.clear(color);

//	for (int32_t y = 0; y < display.getSize().getHeight(); y++) {
//		for (int32_t x = 0; x < display.getSize().getWidth(); x++) {
//			buffer.renderPixel(Point(x, y), color);
//		}
//	}

	color += colorStep;

	if (color > 15) {
		color = 15;
		colorStep = -1;
	}
	else if (color < 0) {
		color = 0;
		colorStep = 1;
	}

	buffer.flush();

	auto delta = esp_timer_get_time() - startTime;
	Serial.printf("FPS: %lld, color: %d, free heap: %d kb, max alloc heap: %d kb\n", 60000000 / delta, color, ESP.getFreeHeap() / 1024, ESP.getMaxAllocHeap() / 1024);

	uint32_t desiredTime = 1000 / 60;
	if (delta < desiredTime)
		delay(desiredTime - delta);

	delay(1000);
}