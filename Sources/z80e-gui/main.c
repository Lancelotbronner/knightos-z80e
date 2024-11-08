#include <z80e/app.h>
#include <z80e/hardware/t6a04.h>

#include <SDL2/SDL.h>

static SDL_Window *Window;
static SDL_Renderer *Renderer;
static float Scale = 5;

#define COLOR_ON    0x00, 0x00, 0x00, 0xff
#define COLOR_OFF   0x99, 0xb1, 0x99, 0xff
#define COLOR_CLEAR 0xc6, 0xe6, 0xc6, 0xff

static void lcd_update(void *data, lcd_t6a04_t lcd) {
	SDL_SetRenderDrawColor(Renderer, COLOR_CLEAR);
	SDL_RenderClear(Renderer);

	for (int cX = 0; cX < 64; cX++) {
		for (int cY = 0; cY < 96; cY++) {
			int cXZ = (cX + lcd->Z) % 64;
			if (lcd->display_on && lcd_t6a04_read(lcd, cY, cXZ))
				SDL_SetRenderDrawColor(Renderer, COLOR_ON);
			else
				SDL_SetRenderDrawColor(Renderer, COLOR_OFF);
			SDL_RenderDrawPoint(Renderer, cY, cX);
		}
	}
	SDL_RenderPresent(Renderer);

//	if (context.log_level >= L_INFO) {
//		printf("C: 0x%02X X: 0x%02X Y: 0x%02X Z: 0x%02X\n", lcd->contrast, lcd->X, lcd->Y, lcd->Z);
//		printf("   %c%c%c%c  O1: 0x%01X 02: 0x%01X\n", lcd->up ? 'V' : '^', lcd->counter ? '-' : '|',
//				lcd->word_length ? '8' : '6', lcd->display_on ? 'O' : ' ', lcd->op_amp1, lcd->op_amp2);
//	}
}

static void log_handler(void *data, loglevel_t level, const char *part, const char *message) {
	printf("%s\n", message);
}

static unsigned char z80e_keycode(SDL_Keycode keycode) {
	switch (keycode) {
	case SDLK_LSHIFT: return K_2nd;
	case SDLK_RSHIFT:
	case SDLK_BACKSPACE: return K_CLEAR;
	case SDLK_LCTRL:
	case SDLK_RCTRL: return K_ALPHA;
	case SDLK_ESCAPE: return K_MODE;
	case SDLK_F1: return K_FUNCTIONS;
	case SDLK_F2: return K_WINDOW;
	case SDLK_F3: return K_ZOOM;
	case SDLK_F4: return K_TRACE;
	case SDLK_F5: return K_GRAPH;
	case SDLK_F7: return K_VARS;
	case SDLK_F8: /* F8 = Toggle 400% Speed */
		break;
	case SDLK_F10: /* F10 = Load File */
		break;
	case SDLK_F11: /* F11 = Debugger */
		break;
	case SDLK_SPACE: return K_0;
	case SDLK_INSERT: return K_VARS;
	case SDLK_HOME: return K_MATH;
	case SDLK_PAGEUP: return K_APPS;
	case SDLK_PAGEDOWN: return K_PRGM;
	case SDLK_END: return K_STAT;
	case SDLK_PLUS:
	case SDLK_EQUALS: return K_XTON;
	case SDLK_UNDERSCORE:
	case SDLK_MINUS:
	case SDLK_QUESTION:
	case SDLK_SLASH: return K_NEGATE;
	case SDLK_KP_0: return K_VARS;
	case SDLK_KP_1: return K_STAT;
	case SDLK_KP_2: return K_DOWN;
	case SDLK_KP_3: return K_PRGM;
	case SDLK_KP_4: return K_RIGHT;
	case SDLK_KP_6: return K_LEFT;
	case SDLK_KP_7: return K_MATH;
	case SDLK_KP_8: return K_UP;
	case SDLK_KP_9: return K_APPS;
	case SDLK_KP_PLUS: return K_PLUS;
	case SDLK_KP_MINUS: return K_SUBTRACT;
	case SDLK_KP_MULTIPLY: return K_MULTIPLY;
	case SDLK_KP_DIVIDE: return K_DIVIDE;
	case SDLK_a: return K_MATH;
	case SDLK_b: return K_APPS;
	case SDLK_c: return K_PRGM;
	case SDLK_d: return K_INVERT;
	case SDLK_e: return K_SIN;
	case SDLK_f: return K_COS;
	case SDLK_g: return K_TAN;
	case SDLK_h: return K_POW;
	case SDLK_i: return K_SQUARE;
	case SDLK_j: return K_COMMA;
	case SDLK_k: return K_LPAREN;
	case SDLK_l: return K_RPAREN;
	case SDLK_m: return K_DIVIDE;
	case SDLK_n: return K_LOG;
	case SDLK_o: return K_7;
	case SDLK_p: return K_8;
	case SDLK_q: return K_9;
	case SDLK_r: return K_MULTIPLY;
	case SDLK_s: return K_LN;
	case SDLK_t: return K_4;
	case SDLK_u: return K_5;
	case SDLK_v: return K_6;
	case SDLK_w: return K_SUBTRACT;
	case SDLK_x: return K_STORE;
	case SDLK_y: return K_1;
	case SDLK_z: return K_2;
	case SDLK_0: return K_0;
	case SDLK_1: return K_1;
	case SDLK_2: return K_2;
	case SDLK_3: return K_3;
	case SDLK_4: return K_4;
	case SDLK_5: return K_5;
	case SDLK_6: return K_6;
	case SDLK_7: return K_7;
	case SDLK_8: return K_8;
	case SDLK_9: return K_9;
	case SDLK_UP: return K_UP;
	case SDLK_LEFT: return K_LEFT;
	case SDLK_RIGHT: return K_RIGHT;
	case SDLK_DOWN: return K_DOWN;
	case SDLK_RETURN: return K_ENTER;
	case SDLK_DELETE: return K_DEL;
	case SDLK_PERIOD:
	case SDLK_SEMICOLON: return K_DOT;
	default: return 0xFF;
	}
}

static void sdl_update(asic_t asic, void *data) {
	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_F12: /* F12 = On */
				asic_power_press(asic);
				break;
			default:
				keyboard_press(&asic->keyboard, z80e_keycode(event.key.keysym.sym));
				break;
			}
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_F12: /* F12 = On */
				asic_power_release(asic);
				break;
			default:
				keyboard_release(&asic->keyboard, z80e_keycode(event.key.keysym.sym));
				break;
			}
		case SDL_WINDOWEVENT:
			z80e_dirty_lcd();
			break;
		case SDL_QUIT:
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
}

int main(int argc, char **argv) {
	z80e_args(argc, argv);
	z80e_log_callback(log_handler, nullptr);

	if (!z80e_init())
		return EXIT_FAILURE;

	SDL_Init(SDL_INIT_EVERYTHING);
	atexit(SDL_Quit);

	SDL_CreateWindowAndRenderer(Scale * 96, Scale * 64, SDL_WINDOW_RESIZABLE, &Window, &Renderer);
	SDL_SetWindowTitle(Window, "z80e");
	SDL_RenderSetLogicalSize(Renderer, 96, 64);

	z80e_hook_lcd(60, lcd_update);
	asic_add_timer(z80e_asic(), 0, 100, sdl_update, nullptr);

	z80e_start();

	z80e_deinit();
	return EXIT_SUCCESS;
}
