#pragma once

#include <driver/spi_master.h>

typedef struct RgbColor
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RgbColor;

static uint16_t to16Bit(const RgbColor& color) {
	return (color.r >> 3) << 3 | (color.g >> 5) | ((color.g >> 2) << 13) | ((color.b >> 3) << 8);
}

typedef struct HsbColor
{
	uint8_t h;
	uint8_t s;
	uint8_t b;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv)
{
	RgbColor rgb;
	unsigned char region, remainder, p, q, t;

	if (hsv.s == 0)
	{
		rgb.r = hsv.b;
		rgb.g = hsv.b;
		rgb.b = hsv.b;
		return rgb;
	}

	region = hsv.h / 43;
	remainder = (hsv.h - (region * 43)) * 6;

	p = (hsv.b * (255 - hsv.s)) >> 8;
	q = (hsv.b * (255 - ((hsv.s * remainder) >> 8))) >> 8;
	t = (hsv.b * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
		case 0:
			rgb.r = hsv.b; rgb.g = t; rgb.b = p;
			break;
		case 1:
			rgb.r = q; rgb.g = hsv.b; rgb.b = p;
			break;
		case 2:
			rgb.r = p; rgb.g = hsv.b; rgb.b = t;
			break;
		case 3:
			rgb.r = p; rgb.g = q; rgb.b = hsv.b;
			break;
		case 4:
			rgb.r = t; rgb.g = p; rgb.b = hsv.b;
			break;
		default:
			rgb.r = hsv.b; rgb.g = p; rgb.b = q;
			break;
	}

	return rgb;
}

/*
The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
struct InitializationCommand{
	uint8_t cmd;
	uint8_t data[16];
	uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
};

DRAM_ATTR static const InitializationCommand ILI9341InitializationCommands[] = {
	/* Power contorl B, power control = 0, DC_ENA = 1 */
	{0xCF, {0x00, 0x83, 0X30}, 3},
	/* Power on sequence control,
	 * cp1 keeps 1 frame, 1st frame enable
	 * vcl = 0, ddvdh=3, vgh=1, vgl=2
	 * DDVDH_ENH=1
	 */
	{0xED, {0x64, 0x03, 0X12, 0X81}, 4},
	/* Driver timing control A,
	 * non-overlap=default +1
	 * EQ=default - 1, CR=default
	 * pre-charge=default - 1
	 */
	{0xE8, {0x85, 0x01, 0x79}, 3},
	/* Power control A, Vcore=1.6V, DDVDH=5.6V */
	{0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
	/* Pump ratio control, DDVDH=2xVCl */
	{0xF7, {0x20}, 1},
	/* Driver timing control, all=0 unit */
	{0xEA, {0x00, 0x00}, 2},
	/* Power control 1, GVDD=4.75V */
	{0xC0, {0x26}, 1},
	/* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
	{0xC1, {0x11}, 1},
	/* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
	{0xC5, {0x35, 0x3E}, 2},
	/* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
	{0xC7, {0xBE}, 1},

	/* Memory access control, MX=0, MY=0, MV=1, ML=0, BGR=1, MH=0, X, X */
//	{0x36, {0x28}, 1},
	{0x36, {0 << 7 | 0 << 6 | 1 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 0 << 1 | 0}, 1},

	// INVERSION
	{0x21, {0x01}, 1},

	/* Pixel format, 16bits/pixel for RGB/MCU interface */
	{0x3A, {0x55}, 1},

	/* Frame rate control, f=fosc, 70Hz fps */
	{0xB1, {0x00, 0x1B}, 2},
	/* Enable 3G, disabled */
	{0xF2, {0x08}, 1},
	/* Gamma set, curve 1 */
	{0x26, {0x01}, 1},
	/* Positive gamma correction */
	{0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
	/* Negative gamma correction */
	{0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
	/* Column address set, SC=0, EC=0xEF */
	{0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
	/* Page address set, SP=0, EP=0x013F */
	{0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
	/* Memory write */
	{0x2C, {0}, 0},
	/* Entry mode set, Low vol detect disabled, normal display */
	{0xB7, {0x07}, 1},
	/* Display function control */
	{0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
	/* Sleep out */
	{0x11, {0}, 0x80},
	/* Display on */
	{0x29, {0}, 0x80},
	{0, {0}, 0xff},
};

class Display {
	public:
		Display(
			uint16_t width,
			uint16_t height,
			uint8_t chipSelectPin,
			uint8_t dataCommandPin,
			uint8_t resetPin
		) :
		_width(width),
		_height(height),
		_chipSelectPin(chipSelectPin),
		_dataCommandPin(dataCommandPin),
		_resetPin(resetPin) {

		}

		/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
		 * until the transfer is complete.
		 *
		 * Since command transactions are usually small, they are handled in polling
		 * mode for higher speed. The overhead of interrupt transactions is more than
		 * just waiting for the transaction to complete.
		 */
		void sendCommand(const uint8_t command, bool keepChipSelectActive) {
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

		/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
		 * transfer is complete.
		 *
		 * Since data transactions are usually small, they are handled in polling
		 * mode for higher speed. The overhead of interrupt transactions is more than
		 * just waiting for the transaction to complete.
		 */
		void sendData(const uint8_t *data, int len) {
			spi_transaction_t transaction;

			if (len == 0)
				return;    //no need to send anything

			memset(&transaction, 0, sizeof(transaction));       //Zero out the transaction
			transaction.length = len * 8;             //Len is in bytes, transaction length is in bits.
			transaction.tx_buffer = data;             //Data
			transaction.user = (void*)1;              //D/C needs to be set to 1

			auto result = spi_device_polling_transmit(_spi, &transaction); //Transmit!
			assert(result == ESP_OK);          //Should have had no issues.
		}

		//This function is called (in irq context!) just before a transmission starts. It will
		//set the D/C line to the value indicated in the user field.
		static void SPIPreTransferCallback(spi_transaction_t* transaction) {
			auto DCValue = (int) transaction->user;
			gpio_set_level((gpio_num_t) 16, DCValue);
		}

		/* To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
		 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
		 * because the D/C line needs to be toggled in the middle.)
		 * This routine queues these commands up as interrupt transactions so they get
		 * sent faster (compared to calling spi_device_transmit several times), and at
		 * the mean while the lines for next transactions can get calculated.
		 */
		void pushTransactions(int ypos) {
			sending = true;

			int x;
			//Transaction descriptors. Declared static so they're not allocated on the stack; we need this memory even when this
			//function is finished because the SPI driver needs access to it even while we're already calculating the next line.
			static spi_transaction_t trans[6];

			//In theory, it's better to initialize trans and data only once and hang on to the initialized
			//variables. We allocate them on the stack, so we need to re-init them each call.
			for (x = 0; x < 6; x++) {
				memset(&trans[x], 0, sizeof(spi_transaction_t));

				if ((x & 1) == 0) {
					//Even transfers are commands
					trans[x].length = 8;
					trans[x].user = (void*) 0;
				}
				else {
					//Odd transfers are data
					trans[x].length = 8 * 4;
					trans[x].user = (void*) 1;
				}

				trans[x].flags = SPI_TRANS_USE_TXDATA;
			}

			trans[0].tx_data[0] = 0x2A;         //Column Address Set
			trans[1].tx_data[0] = 0;            //Start Col High
			trans[1].tx_data[1] = 0;            //Start Col Low
			trans[1].tx_data[2] = (_width - 1) >> 8;   //End Col High
			trans[1].tx_data[3] = (_width - 1) & 0xff; //End Col Low
			trans[2].tx_data[0] = 0x2B;         //Page address set
			trans[3].tx_data[0] = ypos >> 8;    //Start page high
			trans[3].tx_data[1] = ypos & 0xff;  //start page low
			trans[3].tx_data[2] = (ypos + _transactionScanlines - 1) >> 8; //end page high
			trans[3].tx_data[3] = (ypos + _transactionScanlines - 1) & 0xff; //end page low
			trans[4].tx_data[0] = 0x2C;         //memory write
			trans[5].tx_buffer = _transactionBuffer;      //finally send the line data
			trans[5].length = _width * _transactionScanlines * 2 * 8;  //Data length, in bits
			trans[5].flags = 0; //undo SPI_TRANS_USE_TXDATA flag

			// Enqueue all transactions
			esp_err_t result;

			for (x = 0; x < 6; x++) {
				result = spi_device_queue_trans(_spi, &trans[x], portMAX_DELAY);
				assert(result == ESP_OK);
			}

			//When we are here, the SPI driver is busy (in the background) getting the transactions sent. That happens
			//mostly using DMA, so the CPU doesn't have much to do here. We're not going to wait for the transaction to
			//finish because we may as well spend the time calculating the next line. When that is done, we can call
			//send_line_finish, which will wait for the transfers to be done and check their status.
		}

		void pollTransactions() {
			spi_transaction_t* transaction;
			esp_err_t result;

			//Wait for all 6 transactions to be done and get back the results.
			for (int x = 0; x < 6; x++) {
				result = spi_device_get_trans_result(_spi, &transaction, portMAX_DELAY);
				assert(result == ESP_OK);

				//We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
			}

			sending = false;
		}

		void begin() {
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

			//Send all the commands
			int cmd = 0;

			while (ILI9341InitializationCommands[cmd].databytes != 0xff) {
				sendCommand(ILI9341InitializationCommands[cmd].cmd, false);
				sendData(ILI9341InitializationCommands[cmd].data, ILI9341InitializationCommands[cmd].databytes & 0x1F);

				if (ILI9341InitializationCommands[cmd].databytes & 0x80)
					vTaskDelay(100 / portTICK_PERIOD_MS);

				cmd++;
			}

			//Allocate memory for the pixel buffers
			_transactionBufferSize = _width * _transactionScanlines * sizeof(uint16_t);
			_transactionBuffer = (uint16_t*) heap_caps_malloc(_transactionBufferSize, MALLOC_CAP_DMA);
			assert(_transactionBuffer != nullptr);
		}

		uint8_t color = 0;

		void tick() {
//			if (sending)
//				return;


			float sStep = 1.0f / (float) _width;
			float bStep = 1.0f / (float) _height;
			float s = 0;
			float b = 1;

			int ib = 0;

//
//			for (int i = 0; i < _transactionBufferSize; i++) {
//				_transactionBuffer[i] = to16Bit(RgbColor {
//					.r = color,
//					.g = 0,
//					.b = color
//				});
//			}

			// Sending
			for (int y = 0; y < _height; y += _transactionScanlines) {

				// Moving screen buffer data to transaction buffer
				ib = 0;

				for (int yPizda = 0; yPizda < _transactionScanlines; yPizda++) {
					s = 0;

					for (int xPizda = 0; xPizda < _width; xPizda++) {
						_transactionBuffer[ib++] = to16Bit(HsvToRgb(HsvColor({
							.h = color,
							.s = (uint8_t) (s * 255.0f),
							.b = (uint8_t) (b * 255.0f)
						})));

						s += sStep;
					}

					b -= bStep;
				}

				pushTransactions(y);
				pollTransactions();
			}

			color += 4;

		}

	private:
		uint16_t _width;
		uint16_t _height;

		uint8_t _chipSelectPin;
		uint8_t _dataCommandPin;
		uint8_t _resetPin;

		spi_device_handle_t _spi;

		//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
		//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
		uint8_t _transactionScanlines = 16;
		uint16_t* _transactionBuffer;
		size_t _transactionBufferSize;

		bool sending = false;
};