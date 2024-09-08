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

	320,
	240
);

void setup() {
	Serial.begin(115200);

	// Transceiver
	Serial.println("Offing Transceiver");
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	Serial.println("Beginning display");

	uint8_t govno = 0;

	for (int i = 0; i < 16; i++) {
		buffer.setPaletteColor(i, to16Bit(RgbColor(govno, govno, govno)));
		govno += 0x11;
	}

	display.begin();

	Serial.println("Draw");
}

int32_t color = 0;
int32_t colorStep = 1;

void loop() {
	auto startTime = esp_timer_get_time();

	buffer.clear(color);

//	for (int32_t y = 0; y < display.getHeight(); y++) {
//		for (int32_t x = 0; x < display.getWidth(); x++) {
//			buffer.renderPixel(x, y, color);
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

//	delay(100);
}