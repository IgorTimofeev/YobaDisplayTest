#include <Arduino.h>
#include "display/display.h"
#include "display/drivers/ILI9341Driver.h"

Display display = Display(
	320,
	240,

	5,
	16,
	17
);

void setup() {
	Serial.begin(115200);
	Serial.printf("Free heap: %d\n", (ESP.getFreeHeap()));
	Serial.println("Offing Transceiver");

	// Transceiver
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	Serial.println("Beginning display");

	display.begin();
}

void loop() {
	auto startTime = esp_timer_get_time();

	display.tick();

	auto delta = esp_timer_get_time() - startTime;
	Serial.printf("FPS: %lld\n", 60000000 / delta);
}