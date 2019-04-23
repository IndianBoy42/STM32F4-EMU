#include "lcd_main.h"
#include "lcd_font.h"
#include "lcd_init.h"
#include "strhelper.h"

#define TFT_SPI SPI1
#define TFT_DMA DMA2_Stream5

//private data
uint16_t curr_bg_color = BLACK;
uint16_t curr_highlight_color = BLACK;
uint16_t curr_text_color = BLACK;
uint16_t curr_text_color_sp = BLACK;

#ifdef MODEL_ST7735
static uint8_t col_shift;
static uint8_t row_shift;
#endif

uint8_t tft_orientation = 0, tft_enabled = 1;
uint8_t char_max_x, char_max_y;
uint16_t row_width;

uint8_t cur_screen, next_screen;
uint8_t pointer_to_curr_screen = 0;
char text_buf[2][CHAR_MAX_X][CHAR_MAX_Y] = {0};
uint16_t text_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y] = {0};
uint16_t bg_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y] = {0};

__forceinline void dma_transfer(DMA_Stream_TypeDef* dma, volatile void* buf, uint16_t len) {
	dma->M0AR = (uint32_t) buf;  
	dma->NDTR = len;
	dma->CR |= (uint32_t)DMA_SxCR_EN;
}
__forceinline void dma_circular_mode(DMA_Stream_TypeDef* dma, uint8_t enable) {
	if (enable) dma->CR |= DMA_SxCR_CIRC;
	else        dma->CR &= ~DMA_SxCR_CIRC;
}
__forceinline void dma_doublebuf_mode(DMA_Stream_TypeDef* dma, uint8_t enable) {
	if (enable) dma->CR |= DMA_SxCR_DBM;
	else        dma->CR &= ~DMA_SxCR_DBM;
}
#define DMA_FLAG_RESERVED_MASK ((uint32_t)0x0F7D0F7D)
#define DMA_FLAG_FE DMA_LISR_FEIF0
#define DMA_FLAG_DM DMA_LISR_DMEIF0
#define DMA_FLAG_TE DMA_LISR_TEIF0
#define DMA_FLAG_HT DMA_LISR_HTIF0
#define DMA_FLAG_TC DMA_LISR_TCIF0
__forceinline uint8_t dma_flag_status(DMA_Stream_TypeDef* dma, uint32_t flag) {
	uint32_t tmpreg = 0;

	/* Determine the DMA to which belongs the stream */
	if (dma < DMA2_Stream0) {
		/* dma belongs to DMA1 */
		if (dma < DMA1_Stream4) {
			/* Get DMA1 LISR register value */
			tmpreg = DMA1->LISR;
		} else {
			/* Get DMA1 HISR register value */
			tmpreg = DMA1->HISR;
		}
	} else {
		/* dma belongs to DMA2 */
		if (dma < DMA2_Stream4) {
			/* Get DMA2 LISR register value */
			tmpreg = DMA2->LISR;
		} else {
			/* Get DMA2 HISR register value */
			tmpreg = DMA2->HISR;
		}
	}  

	uint32_t chflag;
	switch ((uint32_t)dma) {
		case (uint32_t)DMA1_Stream0: 
		case (uint32_t)DMA1_Stream4: 
		case (uint32_t)DMA2_Stream0: 
		case (uint32_t)DMA2_Stream4: chflag = flag;
			break;
		
		case (uint32_t)DMA1_Stream1: 
		case (uint32_t)DMA1_Stream5: 
		case (uint32_t)DMA2_Stream1: 
		case (uint32_t)DMA2_Stream5: chflag = flag<<6;
			break;
		
		case (uint32_t)DMA1_Stream2: 
		case (uint32_t)DMA1_Stream6: 
		case (uint32_t)DMA2_Stream2: 
		case (uint32_t)DMA2_Stream6: chflag = flag<<16;
			break;
		
		case (uint32_t)DMA1_Stream3: 
		case (uint32_t)DMA1_Stream7: 
		case (uint32_t)DMA2_Stream3: 
		case (uint32_t)DMA2_Stream7: chflag = flag<<22;
			break;
	}
	// chflag &= DMA_FLAG_RESERVED_MASK;

	// return tmpreg & chflag;
	return (tmpreg & chflag) != 0;
}
__forceinline void dma_clear_flag(DMA_Stream_TypeDef* dma, uint32_t flag) {
	uint32_t chflag;
	switch ((uint32_t)dma) {
		case (uint32_t)DMA1_Stream0: 
		case (uint32_t)DMA1_Stream4: 
		case (uint32_t)DMA2_Stream0: 
		case (uint32_t)DMA2_Stream4: chflag = flag;
			break;
		
		case (uint32_t)DMA1_Stream1: 
		case (uint32_t)DMA1_Stream5: 
		case (uint32_t)DMA2_Stream1: 
		case (uint32_t)DMA2_Stream5: chflag = flag<<6;
			break;
		
		case (uint32_t)DMA1_Stream2: 
		case (uint32_t)DMA1_Stream6: 
		case (uint32_t)DMA2_Stream2: 
		case (uint32_t)DMA2_Stream6: chflag = flag<<16;
			break;
		
		case (uint32_t)DMA1_Stream3: 
		case (uint32_t)DMA1_Stream7: 
		case (uint32_t)DMA2_Stream3: 
		case (uint32_t)DMA2_Stream7: chflag = flag<<22;
			break;
	}

	/* Determine the DMA to which belongs the stream */
	if (dma < DMA2_Stream0) {
		/* dma belongs to DMA1 */
		if (dma < DMA1_Stream4) {
			/* Get DMA1 LISR register value */
			DMA1->LIFCR = chflag & DMA_FLAG_RESERVED_MASK;
		} else {
			/* Get DMA1 HISR register value */
			DMA1->HIFCR = chflag & DMA_FLAG_RESERVED_MASK;
		}
	} else {
		/* dma belongs to DMA2 */
		if (dma < DMA2_Stream4) {
			/* Get DMA2 LISR register value */
			DMA2->LIFCR = chflag & DMA_FLAG_RESERVED_MASK;
		} else {
			/* Get DMA2 HISR register value */
			DMA2->HIFCR = chflag & DMA_FLAG_RESERVED_MASK;
		}
	}  
}

/**
  * @brief  Initialization of SPI for TFT
  * @param  None
  * @retval None
  */
void tft_spi_init(void) {
	LL_SPI_EnableDMAReq_TX(TFT_SPI);
	TFT_DMA->PAR = (uint32_t) &TFT_SPI->DR;
	// TFT_DMA->CR |= DMA_SxCR_DIR_0;
	LL_SPI_Enable(TFT_SPI);

	gpio_set(TFT_DC);
	gpio_reset(TFT_LED);
	
	dma_transfer(TFT_DMA, (void*)0x20000000, 16);
}

/**
  * @brief  Sending a command
  * @param  command: one byte command to be sent
  * @retval None
  */
__forceinline void tft_write_command(uint8_t command) {
	__disable_irq();
	while(TFT_SPI->SR & SPI_SR_BSY);
	
	TFT_DC_GPIO_Port->BSRR = TFT_DC_Pin<<16;

	while (!(TFT_SPI->SR & SPI_SR_TXE));
	TFT_SPI->DR = command;
	while (!(TFT_SPI->SR & SPI_SR_BSY)); //Wait for transmission to start
	while (TFT_SPI->SR & SPI_SR_BSY); //And then to end
	
	TFT_DC_GPIO_Port->BSRR = TFT_DC_Pin;
	__enable_irq();
}

/**
  * @brief  Sending a data
  * @param  data: one byte data to be sent
  * @retval None
  */
__forceinline void tft_write_data(uint8_t data) {
	while (!(TFT_SPI->SR & SPI_SR_TXE));
	TFT_SPI->DR = data;
}

/**
  * @brief  Hardware reset for TFT
  * @param  None
  * @retval None
  */
void tft_reset(void)
{
	gpio_set(TFT_RST);
	delay(100);
	gpio_reset(TFT_RST);
	delay(100);
	gpio_set(TFT_RST);
	delay(100);
}

/**
  * @brief  Configuration of TFT
  * @param  None
  * @retval None
  */
inline static void tft_config(void)
{
	tft_spi_init();
	tft_reset();

	uint32_t i=0;
	while (tft_init_code[++i] != TFT_EOF_MARKER) {
		tft_write_command(tft_init_code[i]);
		
		if (tft_init_code[i] == TFT_ORIENTATION_CMD) tft_write_data(tft_orientation_init_code[tft_orientation]);

		while (tft_init_code[++i] != TFT_EOL_MARKER) {
			if (tft_init_code[i] == TFT_DELAY_MARKER) delay(tft_init_code[++i]);
			else                                      tft_write_data(tft_init_code[i]);
		}
	}
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp, uint16_t in_highlight_color) { //, DISPLAY_MODE mode) {
	//cur_display_mode = mode;
	
	cur_screen = 0;
	next_screen = 1;
	tft_orientation = orientation;

	#ifdef MODEL_ST7735
	switch (tft_orientation) {
		case 0:
		col_shift = 2;
		row_shift = 1;
		break;
		case 1:
		col_shift = 1;
		row_shift = 2;
		break;
		case 2:
		col_shift = 2;
		row_shift = 2;
		break;
		case 3:
		col_shift = 1;
		row_shift = 2;
		break;
	}
	#endif

	tft_config();
	
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	tft_set_highlight_color(in_highlight_color);

	tft_fill_color(in_bg_color);
	delay(1);

	char_max_x = (orientation % 2) ? CHAR_MAX_X_HORIZONTAL : CHAR_MAX_X_VERTICAL;
	char_max_y = (orientation % 2) ? CHAR_MAX_Y_HORIZONTAL : CHAR_MAX_Y_VERTICAL;

	row_width = (orientation%2) ? MAX_HEIGHT : MAX_WIDTH;
	
	tft_clear();
	cur_screen = 1;
	tft_clear();
	cur_screen = 0;
	// tft_update();
}

/**
  * @brief  Set the current background color
  * @param  None
  * @retval None
  */
void tft_set_bg_color(uint16_t in_bg_color) {
	curr_bg_color = in_bg_color;
}

/**
  * @brief  Set the current special background color
  * @param  None
  * @retval None
  */
void tft_set_highlight_color(uint16_t highlight_color) {
	curr_highlight_color = highlight_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(uint16_t in_text_color) {
	curr_text_color = in_text_color;
}

/**
  * @brief  Set the current special text color
  * @param  None	
  * @retval None
  */
void tft_set_special_color(uint16_t text_color_sp) {
	curr_text_color_sp = text_color_sp;
}

uint8_t tft_get_orientation() {
	return tft_orientation;
}

__forceinline void tft_set_region(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	uint16_t startx = x;
	uint16_t starty = y;
	uint16_t endx = x+w;
	uint16_t endy = y+h;

	#ifdef MODEL_ST7735
	startx += col_shift;
	starty += row_shift;
	endx += col_shift;
	endy += row_shift;
	#endif

	tft_write_command(0x2a);		// Column addr set
	tft_write_data((startx>>8 & 0xFF));
	tft_write_data((startx)   & 0xFF); 				// X START
	tft_write_data((endx>>8   & 0xFF));
	tft_write_data((endx)     & 0xFF);

	tft_write_command(0x2b);		// Row addr set
	tft_write_data((starty>>8 & 0xFF));
	tft_write_data((starty)   & 0xFF);				// Y START
	tft_write_data((endy>>8   & 0xFF));
	tft_write_data((endy)     & 0xFF);

	tft_write_command(0x2c);
}

#define buf_coord(x, y) (((uint16_t)(y)) * ((uint16_t)char_max_x) + ((uint16_t)(x)))

/**
  * @brief  Clear one line in the text buffer
  * @param  line: the line to be cleared
  * @retval None
  */
void tft_clear_line(uint8_t y){
	for (uint8_t x = 0; x < char_max_x; x++) {
		text_buf[cur_screen][x][y] = ' ';
		text_color_buf[cur_screen][x][y] = curr_text_color;
		bg_color_buf[cur_screen][x][y] = curr_bg_color;
	}
}

/**
  * @brief  Clear the whole text buffer
  * @param  None
  * @retval None
  */
void tft_clear(void){
	for(uint8_t y = 0; y < char_max_y; y++){
		tft_clear_line(y);
	}
}

static void tft_write_color(uint16_t color) {
	#ifdef MODEL_ILI9486 //RGB666
	tft_write_data((color & 0xF800) >> (11-3));
	tft_write_data((color & 0x7E0) >> (5-2));
	tft_write_data((color & 0x1F) << 3);
	#else
	tft_write_data(color >> 8);
	tft_write_data(color);
	#endif
}

/**
  * @brief  Print a single pixel on screen
  * @param  x: x-coordinate
  * @param  y: y-coordinate
  * @param  color: color of the pixel
  * @retval None
  */
void tft_put_pixel(uint16_t x, uint16_t y, uint16_t color) {
	tft_set_region(x, y, 1, 1);
	tft_write_color(color);
}

/**
  * @brief  Fill the whole screen with a color
  * @param  color: color to be filled with
  * @retval None
  */
void tft_fill_color(uint16_t color) {
	if (tft_orientation % 2)
		tft_set_region(0, 0, MAX_HEIGHT, MAX_WIDTH);
	else
		tft_set_region(0, 0, MAX_WIDTH, MAX_HEIGHT);
	
	for (uint32_t i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		tft_write_color(color);
	}
}

/**
  * @brief  Clear every pixel on screen
  * @param  None
  * @retval None
  */
void tft_force_clear(void) {
	for (uint8_t i = 0; i < CHAR_MAX_X; i++) {
		for (uint8_t j = 0; j < CHAR_MAX_Y; j++) {
			text_buf[0][i][j] = ' ';
			text_buf[1][i][j] = ' ';
		}
	}
	tft_fill_color(curr_bg_color);
}

void tft_printc(uint8_t x, uint8_t y, char * fp) {
	uint8_t i = x;
	uint8_t j = y;
	uint8_t is_special = 0;
	uint8_t is_highlighted = 0;
	uint8_t is_underlined = 0;
	// uint8_t escaped = 0;
	
	while (*fp && i < char_max_x && j < char_max_y) {
		switch(*fp) {
			case '\r':
			case '\n':
			j++;
			i = x;
			break;
			case '[':
			case ']':
			is_special = (*fp == '[');
			break;
			case '{':
			case '}':
			is_highlighted = (*fp == '{');
			break;
			// case '|':
			// is_underlined = !is_underlined;
			// break;
			default:
			text_buf[cur_screen][i][j] = ((*fp) | (is_underlined ? 0x80 : 0x00));
			text_color_buf[cur_screen][i][j] = is_special     ? curr_text_color_sp   : curr_text_color;
			bg_color_buf[cur_screen][i++][j] = is_highlighted ? curr_highlight_color : curr_bg_color;
			break;
		}
		fp++;
	}
}

/**
  * @brief  Print a string at certain position, use [] to indicate special words
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  */
void tft_prints(uint8_t x, uint8_t y, const char * fmt, ...) {
	char buf[255]={0}, *fp=buf;
	
	va_list args;
	va_start( args, fmt );
	int c = print( &fp, fmt, args );

	tft_printc(x, y, buf);
}

void tft_printi(uint8_t x, uint8_t y, int32_t num) {
	char buf[12]={0};

	itos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printb(uint8_t x, uint8_t y, uint32_t b, uint8_t bits) {
	char buf[33]={0};

	for(int i=0; i<bits; i++) {
		buf[i] = '0' + ((b & (1<<i)) != 0);
	}
	
	tft_printc(x, y, buf);
}
void tft_printh(uint8_t x, uint8_t y, uint32_t b, uint8_t bits) {
	char buf[9]={0};
	int c = bits/4-1;
	for(int i=0; i<bits; i+=4) {
		uint8_t tmp = (b & (0xF<<i))>>i;
		if (tmp < 10) 
			buf[c] = '0' + tmp;
		else 
			buf[c] = 'A' + tmp-10;
		c--;
	}
	
	tft_printc(x, y, buf);
}

void tft_printl(uint8_t x, uint8_t y, int64_t num) {
	char buf[12]={0};

	ltos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printu(uint8_t x, uint8_t y, uint32_t num) {
	char buf[12]={0};

	utos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printf(uint8_t x, uint8_t y, float num, uint32_t dp) {
	char buf[40];

	ftos(buf, num, dp);

	tft_printc(x, y, buf);
}

__forceinline uint8_t tft_char_is_changed(uint8_t x, uint8_t y){
	// return true;
	return (text_buf[cur_screen][x][y] != text_buf[next_screen][x][y] \
		|| (text_color_buf[cur_screen][x][y] != text_color_buf[next_screen][x][y]) \
		|| (bg_color_buf[cur_screen][x][y] != bg_color_buf[next_screen][x][y]));
}

/**
  * @brief  Refresh the whole screen
  * @param  None
  * @retval None
  */
void tft_update(void) {
	int16_t adj_cnt = 0;
	uint16_t color;
	uint8_t char_cnt = 0;
	uint8_t y = 0;

	for (uint8_t j = 0; j < char_max_y; j++) {
		for (uint8_t i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(i, j)) {
				adj_cnt = 1;
				while ((i + adj_cnt < char_max_x) && tft_char_is_changed(i + (adj_cnt), j)) adj_cnt++; //Search for diff char
				tft_set_region(i*CHAR_WIDTH, j*CHAR_HEIGHT, adj_cnt*CHAR_WIDTH-1, CHAR_HEIGHT-1);
				
				y = 0;
				while (y < CHAR_HEIGHT) {
					uint8_t char_line = CHAR_PTR(text_buf[cur_screen][i + char_cnt][j])[y];
					for (uint8_t x = CHAR_WIDTH; x > 0; x--) {
						color = (char_line >> (x - 1) & 0x01) ? text_color_buf[cur_screen][i + char_cnt][j] : bg_color_buf[cur_screen][i + char_cnt][j];
						tft_write_color(color);
					}
					if (++char_cnt == adj_cnt) {
						char_cnt = 0;
						y++;
					}
				}
				i += adj_cnt-1;
			}
		}
	}
	
	// Swap pointers
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}

void tft_dma_tx(void* ptr, uint16_t size) {
	dma_transfer(TFT_DMA, ptr, size);
}
void tft_dma_wait(void) {
	// while (dma_flag_status(TFT_DMA,DMA_FLAG_TC) == 0);
	// dma_clear_flag(TFT_DMA,DMA_FLAG_TC);
	while (LL_DMA_IsActiveFlag_TC5(DMA2)==0);
	LL_DMA_ClearFlag_TC5(DMA2);
}

void tft_circ_push_pxbuf(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	tft_dma_wait();
	
	tft_set_region(x, y, w-1, h-1);
	
	dma_circular_mode(TFT_DMA, 1);
	dma_transfer(TFT_DMA, buf, w*h*2);
}
void tft_double_push_pxbuf(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	tft_dma_wait();
	// TFT_DMA->CR &= (uint32_t)~DMA_SxCR_EN;

	tft_set_region(x, y, w-1, h-1);

	uint32_t ptr = (uint32_t)buf;
	TFT_DMA->M0AR = ptr;
	TFT_DMA->M1AR = ptr+(w*h);  
	TFT_DMA->NDTR = w*h;
	dma_doublebuf_mode(TFT_DMA, 1);
	TFT_DMA->CR |= (uint32_t)DMA_SxCR_EN;
}
void tft_push_pxbuf(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	tft_dma_wait();
	
	tft_set_region(x, y, w-1, h-1);
	
	dma_transfer(TFT_DMA, buf, w*h*2);
}
#define CHAR_BYTES            CHAR_WIDTH*CHAR_HEIGHT*2
#define THREADED_TFT_BUF_LEN  CHAR_MAX_X
void tft_update_dma(void) {
	uint8_t buf_ptr = 0;
	static uint8_t tft_dma_bufs[2][CHAR_BYTES * THREADED_TFT_BUF_LEN];
	
	for (uint8_t j = 0; j < char_max_y; j++) {
		for (uint8_t i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(i, j)) {
				uint8_t adj_cnt = 1;
				while ((i + adj_cnt < char_max_x) && tft_char_is_changed(i + (adj_cnt), j) && (adj_cnt < THREADED_TFT_BUF_LEN))
					adj_cnt++; //Search for diff char

				uint16_t px = 0;
				uint16_t y = 0;
				while (y < CHAR_HEIGHT) {
					uint8_t char_cnt = 0;
					while (char_cnt != adj_cnt) {
						const uint8_t char_ptr = (CHAR_PTR(text_buf[cur_screen][i + char_cnt][j]))[y];
						uint16_t fg = text_color_buf[cur_screen][i + char_cnt][j];
						uint16_t bg = bg_color_buf[cur_screen][i + char_cnt][j];
						for (uint8_t x = CHAR_WIDTH; x > 0; x--) {
							// uint16_t color = 0xFFFF;
							uint16_t color = ((char_ptr >> (x - 1)) & 0x01) ? fg : bg;
							tft_dma_bufs[buf_ptr][px++] = color >> 8;
							tft_dma_bufs[buf_ptr][px++] = color;
						}
						char_cnt++;
					}
					y++;
				}

				tft_push_pxbuf(&tft_dma_bufs[buf_ptr], i * CHAR_WIDTH, j * CHAR_HEIGHT, adj_cnt * CHAR_WIDTH, CHAR_HEIGHT);
				
				buf_ptr ^= 0x1;
				i += adj_cnt - 1;
			}
		}
	}

	// Swap pointers
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}

#define tft_px_index(py, px) (tft_px_buffer[px + py*row_width])
void tft_pxbuf_update(uint16_t* tft_px_buffer) {
	const uint8_t *char_ptr;
	uint16_t color;
	uint32_t px = 0, py = 0;
	uint16_t* pix;
	
	for (uint32_t j = 0; j < char_max_y; j++) {
		for (uint32_t i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(i, j)) {
				px = i * CHAR_WIDTH;
				py = j * CHAR_HEIGHT;
				pix = &tft_px_index(py, px);
				char_ptr = CHAR_PTR(text_buf[cur_screen][i][j]);
				uint16_t fg = text_color_buf[cur_screen][i][j];
				uint16_t bg = bg_color_buf[cur_screen][i][j];
				for (uint32_t y = 0; y < CHAR_HEIGHT; y++) {
					char_ptr++;
					for (uint32_t x = CHAR_WIDTH; x > 0; x--) {
						color = (*char_ptr >> (x - 1) & 0x01) ? fg : bg;
						*pix++ = color;
					}
					pix += row_width-CHAR_WIDTH;
				}
			}
		}
	}
	
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}
void tft_pxbuf_write(uint16_t* tft_px_buffer) {
	const uint8_t *char_ptr;
	uint16_t color;
	uint32_t px = 0, py = 0;
	uint16_t* pix;
	
	for (uint32_t j = 0; j < char_max_y; j++) {
		for (uint32_t i = 0; i < char_max_x; i++) {
			char txt = text_buf[cur_screen][i][j];
			if (txt != ' ') {
				px = i * CHAR_WIDTH;
				py = j * CHAR_HEIGHT;
				pix = &tft_px_index(py, px);
				char_ptr = CHAR_PTR(txt);
				uint16_t fg = text_color_buf[cur_screen][i][j];
				uint16_t bg = bg_color_buf[cur_screen][i][j];
				for (uint32_t y = 0; y < CHAR_HEIGHT; y++) {
					char_ptr++;
					for (uint32_t x = CHAR_WIDTH; x > 0; x--) {
						color = (*char_ptr >> (x - 1) & 0x01) ? fg : bg;
						*pix++ = color;
					}
					pix += row_width-CHAR_WIDTH;
				}
			}
		}
	}
	
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}

void tft_print_nes_line(int y, uint16_t* buf) {
	tft_dma_wait();

	tft_set_region(32, y, 256, 1);
	
	dma_transfer(TFT_DMA, buf+8, 256*2);
}
