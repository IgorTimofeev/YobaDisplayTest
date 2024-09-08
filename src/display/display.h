#pragma once

#include "display/drivers/driver.h"
#include "display/buffers/buffer.h"
#include <driver/spi_master.h>
#include "Arduino.h"

class RgbColor {
	public:
		RgbColor() : r(0), g(0), b(0) {}
		RgbColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

		uint8_t r;
		uint8_t g;
		uint8_t b;
};

static uint16_t to16Bit(const RgbColor& color) {
	return (color.r >> 3) << 3 | (color.g >> 5) | ((color.g >> 2) << 13) | ((color.b >> 3) << 8);
}

class Display {
	public:
		Display(
			uint8_t chipSelectPin,
			uint8_t dataCommandPin,
			uint8_t resetPin,

			Driver &driver,
			Buffer &buffer,

			uint16_t width,
			uint16_t height
		);

		void begin();

		/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
		 * until the transfer is complete.
		 *
		 * Since command transactions are usually small, they are handled in polling
		 * mode for higher speed. The overhead of interrupt transactions is more than
		 * just waiting for the transaction to complete.
		 */
		void sendCommand(uint8_t command, bool keepChipSelectActive);

		/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
		 * transfer is complete.
		 *
		 * Since data transactions are usually small, they are handled in polling
		 * mode for higher speed. The overhead of interrupt transactions is more than
		 * just waiting for the transaction to complete.
		 */
		void sendData(const uint8_t *data, int len);

		//This function is called (in irq context!) just before a transmission starts. It will
		//set the D/C line to the value indicated in the user field.
		static void SPIPreTransferCallback(spi_transaction_t* transaction);

		/* To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
		 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
		 * because the D/C line needs to be toggled in the middle.)
		 * This routine queues these commands up as interrupt transactions so they get
		 * sent faster (compared to calling spi_device_transmit several times), and at
		 * the mean while the lines for next transactions can get calculated.
		 */
		void pushTransactions(int ypos);

		uint16_t getWidth() const;
		uint16_t getHeight() const;
		Buffer &getBuffer() const;
		Driver &getDriver() const;

		uint8_t getTransactionScanlines() const;
		uint16_t *getTransactionBuffer() const;
		size_t getTransactionBufferSize() const;

	private:
		uint16_t _width;
		uint16_t _height;


		Buffer& _buffer;
		Driver& _driver;

		uint8_t _chipSelectPin;
		uint8_t _dataCommandPin;
		uint8_t _resetPin;

		spi_device_handle_t _spi;

		//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
		//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
		uint8_t _transactionScanlines = 16;
		uint16_t* _transactionBuffer = nullptr;
		size_t _transactionBufferSize = 0;
};