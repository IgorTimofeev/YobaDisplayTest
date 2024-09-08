#include <Arduino.h>
#include "display/display.h"
#include "display/drivers/ILI9341Driver.h"
#include "display/buffers/fourBitsPaletteBuffer.h"

ILI9341Driver driver = ILI9341Driver(
	5,
	16,
	17
);

FourBitsPaletteBuffer buffer = FourBitsPaletteBuffer();

Display display = Display(
	driver,
	buffer,

	320,
	240
);

void setup() {
	Serial.begin(115200);
	Serial.printf("Free heap: %d\n", (ESP.getFreeHeap()));
	Serial.println("Offing Transceiver");

	// Transceiver
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	Serial.println("Beginning display");

	uint8_t govno = 0;

	for(uint8_t i = 0; i < 16; i++) {
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

	auto depth = (uint8_t) ColorDepth::Four;
	auto heightPart = 240 /depth;

	for (int32_t y = 0; y < display.getHeight(); y++) {
		for (int32_t x = 0; x < display.getWidth(); x++) {
			buffer.renderPixel(x, y, color);
		}
	}

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
	Serial.printf("FPS: %lld, color: %d\n", 60000000 / delta, color);

//	delay(100);
}