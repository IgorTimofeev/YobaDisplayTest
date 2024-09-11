#pragma once


#include <cstdint>
#include <driver/spi_master.h>
#include <functional>

class Display;

class Driver {
	public:
		Driver(uint8_t chipSelectPin, uint8_t dataCommandPin, uint8_t resetPin);

		void begin(Display *display);

		virtual void writeInitializationCommands();


		/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
 * until the transfer is complete.
 *
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
		void sendCommand(uint8_t command);

		/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
		 * transfer is complete.
		 *
		 * Since data transactions are usually small, they are handled in polling
		 * mode for higher speed. The overhead of interrupt transactions is more than
		 * just waiting for the transaction to complete.
		 */
		void sendData(const uint8_t *data, int length);

		void sendCommandAndData(uint8_t command, const uint8_t *data, int length);

		//This function is called (in irq context!) just before a transmission starts. It will
		//set the D/C line to the value indicated in the user field.
		static void SPIPreCallback(spi_transaction_t* transaction);

		/* To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
		 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
		 * because the D/C line needs to be toggled in the middle.)
		 * This routine queues these commands up as interrupt transactions so they get
		 * sent faster (compared to calling spi_device_transmit several times), and at
		 * the mean while the lines for next transactions can get calculated.
		 */
		void flushTransactionBuffer(Display *display, int y);
		uint16_t *getTransactionBuffer() const;
		size_t getTransactionBufferLength() const;

		void setTransactionBufferHeight(uint8_t transactionBufferHeight);
		uint8_t getTransactionBufferHeight() const;

	protected:
		uint8_t _chipSelectPin;
		uint8_t _dataCommandPin;
		uint8_t _resetPin;

		spi_device_handle_t _spi;

		//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
		//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
		uint8_t _transactionBufferHeight = 120;
		uint16_t* _transactionBuffer = nullptr;
		size_t _transactionBufferLength = 0;
};

struct DriverSPIPreCallbackUserData {
	DriverSPIPreCallbackUserData(Driver *driver, bool dataCommandPinState);

	Driver* driver;
	bool dataCommandPinState;
};