#include <z80e/app.h>
#include <z80e/hardware/t6a04.h>

#include <stdio.h>
#include <stdlib.h>

static void tui_unicode_to_utf8(char *b, uint32_t c) {
	if (c<0x80) *b++=c;
	else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
	else if (c-0xd800u<0x800) return;
	else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
	else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
}

static void lcd_update(void *data, lcd_t6a04_t lcd) {
#define LCD_BRAILLE
#ifndef LCD_BRAILLE
	for (int cX = 0; cX < 64; cX++) {
		for (int cY = 0; cY < 120; cY++)
			printf("%c", bw_lcd_read_screen(lcd, cY, cX) ? 'O' : ' ');
		printf("\n");
	}
#else
	for (int cX = 0; cX < 64; cX += 4) {
		for (int cY = 0; cY < 120; cY += 2) {
			uint32_t byte_value;
			byte_value |= lcd_t6a04_read(lcd, cY + 0, cX + 0) << 0;
			byte_value |= lcd_t6a04_read(lcd, cY + 0, cX + 1) << 1;
			byte_value |= lcd_t6a04_read(lcd, cY + 0, cX + 2) << 2;
			byte_value |= lcd_t6a04_read(lcd, cY + 1, cX + 0) << 3;
			byte_value |= lcd_t6a04_read(lcd, cY + 1, cX + 1) << 4;
			byte_value |= lcd_t6a04_read(lcd, cY + 1, cX + 2) << 5;
			byte_value |= lcd_t6a04_read(lcd, cY + 0, cX + 3) << 6;
			byte_value |= lcd_t6a04_read(lcd, cY + 1, cX + 3) << 7;
			byte_value += 0x2800;
			char buff[5] = {0};
			tui_unicode_to_utf8(buff, byte_value);
			printf("%s", buff);
		}
		printf("\n");
	}
#endif
//	if (context.log_level >= L_INFO) {
//		printf("C: 0x%02X X: 0x%02X Y: 0x%02X Z: 0x%02X\n", lcd->contrast, lcd->X, lcd->Y, lcd->Z);
//		printf("   %c%c%c%c  O1: 0x%01X 02: 0x%01X\n", lcd->up ? 'V' : '^', lcd->counter ? '-' : '|',
//				lcd->word_length ? '8' : '6', lcd->display_on ? 'O' : ' ', lcd->op_amp1, lcd->op_amp2);
//	}
}

static void log_handler(void *data, loglevel_t level, const char *part, const char *message) {
	printf("%s\n", message);
}

int main(int argc, char **argv) {
	z80e_args(argc, argv);
	z80e_log_callback(log_handler, nullptr);

	if (!z80e_init())
		return EXIT_FAILURE;

	z80e_hook_lcd(60, lcd_update);
	z80e_start();

	z80e_deinit();
	return EXIT_SUCCESS;
}
