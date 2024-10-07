#include <z80e/debugging/command.h>
#include <z80e/debugging/debugger.h>

#include <z80e/hardware/t6a04.h>
#include <z80e/ti/asic.h>

//MARK: - Print Screen Command

static void dump_lcd_unicode_to_utf8(char *b, uint32_t c) {
	if (c<0x80) *b++=c;
	else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
	else if (c-0xd800u<0x800) return;
	else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
	else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
}

static int command_lcd(debugger_state_t state, void *data, int argc, char **argv) {
	lcd_t6a04_t lcd = &state->asic->lcd;
	int cY;
	int cX;

	#define LCD_BRAILLE
	#ifndef LCD_BRAILLE
		for (cX = 0; cX < 64; cX++) {
			for (cY = 0; cY < 120; cY++) {
				state->print(state, "%c", bw_lcd_read_screen(lcd, cY, cX) ? 'O' : ' ');
			}
			state->print(state, "\n");
		}
	#else
		for (cX = 0; cX < 64; cX += 4) {
			for (cY = 0; cY < 120; cY += 2) {
				bool a = lcd_t6a04_read(lcd, cY + 0, cX + 0);
				bool b = lcd_t6a04_read(lcd, cY + 0, cX + 1);
				bool c = lcd_t6a04_read(lcd, cY + 0, cX + 2);
				bool d = lcd_t6a04_read(lcd, cY + 1, cX + 0);
				bool e = lcd_t6a04_read(lcd, cY + 1, cX + 1);
				bool f = lcd_t6a04_read(lcd, cY + 1, cX + 2);
				bool g = lcd_t6a04_read(lcd, cY + 0, cX + 3);
				bool h = lcd_t6a04_read(lcd, cY + 1, cX + 3);
				uint32_t byte_value = 0x2800;
				byte_value += (
					(a << 0) |
					(b << 1) |
					(c << 2) |
					(d << 3) |
					(e << 4) |
					(f << 5) |
					(g << 6) |
					(h << 7));
				char buff[5] = {0};
				dump_lcd_unicode_to_utf8(buff, byte_value);
				state->print(state, "%s", buff);
			}
			state->print(state, "\n");
		}
	#endif
	state->print(state, "C: 0x%02X X: 0x%02X Y: 0x%02X Z: 0x%02X\n", lcd->contrast, lcd->X, lcd->Y, lcd->Z);
	state->print(state, "   %c%c%c%c  O1: 0x%01X 02: 0x%01X\n", lcd->up ? 'V' : '^', lcd->counter ? '-' : '|',
		lcd->word_length ? '8' : '6', lcd->display_on ? 'O' : ' ', lcd->op_amp1, lcd->op_amp2);
	return 0;
}

const struct debugger_command DumpScreenCommand = {
	.name = "lcd",
	.summary = "Dumps the LCD and its registers",
	.callback = command_lcd,
};
