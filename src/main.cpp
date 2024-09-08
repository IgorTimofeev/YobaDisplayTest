#include <Arduino.h>
#include "display/display.h"
#include "display/drivers/ILI9341Driver.h"
#include "display/buffers/fourBitsPaletteBuffer.h"

ILI9341Driver driver = ILI9341Driver();

FourBitsPaletteBuffer buffer = FourBitsPaletteBuffer();

Display display = Display(
	5,
	16,
	17,

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

	buffer.setPaletteColor(0, to16Bit(RgbColor(0xFF, 0x00, 0x00)));
	buffer.setPaletteColor(1, to16Bit(RgbColor(0x00, 0xFF, 0x00)));
	buffer.setPaletteColor(2, to16Bit(RgbColor(0x00, 0x00, 0xFF)));
	buffer.setPaletteColor(3, to16Bit(RgbColor(0xFF, 0xFF, 0x00)));

	display.begin();

	Serial.println("Draw");

	auto depth = (uint8_t) ColorDepth::Four;
	auto heightPart = 240 /depth;

	for (int32_t i = 0; i < depth; i++) {
		for (int32_t y = i * heightPart; y < (i + 1) * heightPart; y++) {
			for (int32_t x = 0; x < 320; x++) {
				buffer.setPixel(x, y, i);
			}
		}
	}
}

void loop() {
	Serial.println("Loop");

	auto startTime = esp_timer_get_time();

	buffer.flush();

	auto delta = esp_timer_get_time() - startTime;
	Serial.printf("FPS: %lld\n", 60000000 / delta);
}