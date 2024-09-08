#include "display.h"

Display::Display(
	uint8_t chipSelectPin,
	uint8_t dataCommandPin,
	uint8_t resetPin,

	Driver &driver,
	Buffer &buffer,

	uint16_t width,
	uint16_t height
) :
	_chipSelectPin(chipSelectPin),
	_dataCommandPin(dataCommandPin),
	_resetPin(resetPin),

	_buffer(buffer),
	_driver(driver),

	_width(width),
	_height(height)
{
	_buffer.setDisplay(this);
}

void Display::begin() {
	pinMode(_resetPin, OUTPUT);

	esp_err_t ret;
	spi_bus_config_t buscfg = {
		.mosi_io_num = MOSI,
		.miso_io_num = MISO,
		.sclk_io_num = SCK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = _transactionScanlines * _width * 2 + 8
	};

	spi_device_interface_config_t devcfg = {
		.mode = 0,                              //SPI mode 0
		.clock_speed_hz = SPI_MASTER_FREQ_26M,     //Clock out at required MHz
		.spics_io_num = _chipSelectPin,             //CS pin
		.queue_size = 7,                        //We want to be able to queue 7 transactions at a time
		.pre_cb = SPIPreTransferCallback, //Specify pre-transfer callback to handle D/C line
	};

	//Initialize the SPI bus
	auto SPIHost = SPI2_HOST;

	ret = spi_bus_initialize(SPIHost, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(ret);

	//Attach the LCD to the SPI bus
	ret = spi_bus_add_device(SPIHost, &devcfg, &_spi);
	ESP_ERROR_CHECK(ret);

	//Initialize the LCD

	//Initialize non-SPI GPIOs
	gpio_config_t io_conf = {};
	io_conf.pin_bit_mask = ((1ULL << _dataCommandPin) | (1ULL << _resetPin));
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	gpio_config(&io_conf);

	// Reset the display
	gpio_set_level((gpio_num_t) _resetPin, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);

	gpio_set_level((gpio_num_t) _resetPin, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);

	Serial.println("writeInitializationCommands()");
	_driver.writeInitializationCommands(*this);

	//Allocate memory for the pixel buffers
	Serial.println("heap_caps_malloc()");

	_transactionBufferSize = _width * _transactionScanlines * sizeof(uint16_t);
	_transactionBuffer = (uint16_t*) heap_caps_malloc(_transactionBufferSize, MALLOC_CAP_DMA);
	assert(_transactionBuffer != nullptr);
}


void Display::sendCommand(uint8_t command, bool keepChipSelectActive) {
	spi_transaction_t transaction;
	memset(&transaction, 0, sizeof(transaction));       //Zero out the transaction
	transaction.length = 8;                   //Command is 8 bits
	transaction.tx_buffer = &command;             //The data is the cmd itself
	transaction.user = (void*) 0;              //D/C needs to be set to 0

	if (keepChipSelectActive)
		transaction.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer

	auto result = spi_device_polling_transmit(_spi, &transaction); //Transmit!
	assert(result == ESP_OK);          //Should have had no issues.
}

void Display::sendData(const uint8_t *data, int len) {
	if (len == 0)
		return;    //no need to send anything

	spi_transaction_t transaction;
	memset(&transaction, 0, sizeof(transaction));       //Zero out the transaction
	transaction.length = len * 8;             //Len is in bytes, transaction length is in bits.
	transaction.tx_buffer = data;             //Data
	transaction.user = (void*)1;              //D/C needs to be set to 1

	auto result = spi_device_polling_transmit(_spi, &transaction); //Transmit!
	assert(result == ESP_OK);          //Should have had no issues.
}

void Display::SPIPreTransferCallback(spi_transaction_t *transaction) {
	auto DCValue = (int) transaction->user;
	gpio_set_level((gpio_num_t) 16, DCValue);
}

void Display::pushTransactions(int ypos) {
	//Transaction descriptors. Declared static, so they're not allocated on the stack; we need this memory even when this
	//function is finished because the SPI driver needs access to it even while we're already calculating the next line.
	static spi_transaction_t transactions[6];

	//In theory, it's better to initialize trans and data only once and hang on to the initialized
	//variables. We allocate them on the stack, so we need to re-init them each call.
	for (uint8_t i = 0; i < 6; i++) {
		memset(&transactions[i], 0, sizeof(spi_transaction_t));

		if ((i & 1) == 0) {
			//Even transfers are commands
			transactions[i].length = 8;
			transactions[i].user = (void*) 0;
		}
		else {
			//Odd transfers are data
			transactions[i].length = 8 * 4;
			transactions[i].user = (void*) 1;
		}

		transactions[i].flags = SPI_TRANS_USE_TXDATA;
	}

	transactions[0].tx_data[0] = 0x2A;         //Column Address Set

	transactions[1].tx_data[0] = 0;            //Start Col High
	transactions[1].tx_data[1] = 0;            //Start Col Low
	transactions[1].tx_data[2] = (_width - 1) >> 8;   //End Col High
	transactions[1].tx_data[3] = (_width - 1) & 0xff; //End Col Low

	transactions[2].tx_data[0] = 0x2B;         //Page address set

	transactions[3].tx_data[0] = ypos >> 8;    //Start page high
	transactions[3].tx_data[1] = ypos & 0xff;  //start page low
	transactions[3].tx_data[2] = (ypos + _transactionScanlines - 1) >> 8; //end page high
	transactions[3].tx_data[3] = (ypos + _transactionScanlines - 1) & 0xff; //end page low

	transactions[4].tx_data[0] = 0x2C;         //memory write

	transactions[5].tx_buffer = _transactionBuffer;      //finally send the line data
	transactions[5].length = _width * _transactionScanlines * 2 * 8;  //Data length, in bits
	transactions[5].flags = 0; //undo SPI_TRANS_USE_TXDATA flag

	// Enqueue all transactions
	esp_err_t result;

	for (uint8_t i = 0; i < 6; i++) {
		result = spi_device_queue_trans(_spi, &transactions[i], portMAX_DELAY);
		assert(result == ESP_OK);
	}

	//When we are here, the SPI driver is busy (in the background) getting the transactions sent. That happens
	//mostly using DMA, so the CPU doesn't have much to do here. We're not going to wait for the transaction to
	//finish because we may as well spend the time calculating the next line. When that is done, we can call
	//send_line_finish, which will wait for the transfers to be done and check their status.

	spi_transaction_t* transaction;

	//Wait for all 6 transactions to be done and get back the results.
	for (uint8_t i = 0; i < 6; i++) {
		result = spi_device_get_trans_result(_spi, &transaction, portMAX_DELAY);
		assert(result == ESP_OK);

		//We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
	}
}

uint16_t Display::getWidth() const {
	return _width;
}

uint16_t Display::getHeight() const {
	return _height;
}

Buffer &Display::getBuffer() const {
	return _buffer;
}

Driver &Display::getDriver() const {
	return _driver;
}

uint8_t Display::getTransactionScanlines() const {
	return _transactionScanlines;
}

uint16_t *Display::getTransactionBuffer() const {
	return _transactionBuffer;
}

size_t Display::getTransactionBufferSize() const {
	return _transactionBufferSize;
}
