#ifndef AST_E_KEYBINDINGS
#define AST_E_KEYBINDINGS
#include "utils.hpp"
#include "utils.hpp"

class Input {
	public:
		struct Mouse {
			static inline float sy, sd{ 0 };
			static inline int x, y, dx, dy{ 0 };
			static inline bool m1d, m2d, m3d, mt1d, mt2d{ false };
			static inline bool m1df, m2df, m3df, mt1df, mt2df{ false };

			static inline std::function <void(int, int, bool, bool, bool, bool, bool)> onClick = [](int x, int y, bool l, bool r, bool m, bool t1, bool t2) {};
			static inline std::function <void(float)> onScroll = [](float y) {};
			static inline std::function <void(int, int)> onMove = [](int x, int y) {};
			static inline bool input_stolen;
		};
		struct Keyboard {
			static inline bool input_stolen;
			static inline char keys[1024]{};
			static inline char key_frame[1024]{};
			static inline std::function <void(unsigned char)> onKeyPress = [](unsigned char) {};
			static inline std::function <void(unsigned char)> onKeyRelease = [](unsigned char) {};
			static inline std::function <void(unsigned char)> onKeyHold = [](unsigned char) {};

			static void EmulateKeyboardDown(unsigned char key, SDL_Event ev) {
				keys[key] = true;
				if (key_frame[key] == -12) {
					key_frame[key] = true;
					onKeyPress(key);
					return;
				}
				onKeyHold(key);
			}
			static void EmulateKeyboardUp(unsigned char key, SDL_Event ev) {
				keys[key] = false;
				key_frame[key] = -12;
				onKeyRelease(key);
			}
			static bool GetKey(int k) { // Is Key pressed?
				return keys[k];
			}
			static bool GetKeyPressed(int k) { // Key pressed this frame?
				return key_frame[k];
			}
			static void flush() {
				memset(keys, 0, 1024);
			}
		};
		static inline int K_UNKNOWN = 0;

		static inline int K_A;
		static inline int K_B;
		static inline int K_C;
		static inline int K_D;
		static inline int K_E;
		static inline int K_F;
		static inline int K_G;
		static inline int K_H;
		static inline int K_I;
		static inline int K_J;
		static inline int K_K;
		static inline int K_L;
		static inline int K_M;
		static inline int K_N;
		static inline int K_O;
		static inline int K_P;
		static inline int K_Q;
		static inline int K_R;
		static inline int K_S;
		static inline int K_T;
		static inline int K_U;
		static inline int K_V;
		static inline int K_W;
		static inline int K_X;
		static inline int K_Y;
		static inline int K_Z;


		static inline int K_a = K_A = 4;
		static inline int K_b = K_B = 5;
		static inline int K_c = K_C = 6;
		static inline int K_d = K_D = 7;
		static inline int K_e = K_E = 8;
		static inline int K_f = K_F = 9;
		static inline int K_g = K_G = 10;
		static inline int K_h = K_H = 11;
		static inline int K_i = K_I = 12;
		static inline int K_j = K_J = 13;
		static inline int K_k = K_K = 14;
		static inline int K_l = K_L = 15;
		static inline int K_m = K_M = 16;
		static inline int K_n = K_N = 17;
		static inline int K_o = K_O = 18;
		static inline int K_p = K_P = 19;
		static inline int K_q = K_Q = 20;
		static inline int K_r = K_R = 21;
		static inline int K_s = K_S = 22;
		static inline int K_t = K_T = 23;
		static inline int K_u = K_U = 24;
		static inline int K_v = K_V = 25;
		static inline int K_w = K_W = 26;
		static inline int K_x = K_X = 27;
		static inline int K_y = K_Y = 28;
		static inline int K_z = K_Z = 29;
		static inline int K_1 = 30;
		static inline int K_2 = 31;
		static inline int K_3 = 32;
		static inline int K_4 = 33;
		static inline int K_5 = 34;
		static inline int K_6 = 35;
		static inline int K_7 = 36;
		static inline int K_8 = 37;
		static inline int K_9 = 38;
		static inline int K_0 = 39;
		static inline int K_return = 40;
		static inline int K_escape = 41;
		static inline int K_backspace = 42;
		static inline int K_tab = 43;
		static inline int K_space = 44;
		static inline int K_minus = 45;
		static inline int K_equals = 46;
		static inline int K_leftbracket = 47;
		static inline int K_rightbracket = 48;
		static inline int K_backslash = 49;
		static inline int K_nonushash = 50;
		static inline int K_semicolon = 51;
		static inline int K_apostrophe = 52;
		static inline int K_grave = 53;
		static inline int K_tild = 53;
		static inline int K_comma = 54;
		static inline int K_period = 55;
		static inline int K_slash = 56;
		static inline int K_capslock = 57;
		static inline int K_F1 = 58;
		static inline int K_F2 = 59;
		static inline int K_F3 = 60;
		static inline int K_F4 = 61;
		static inline int K_F5 = 62;
		static inline int K_F6 = 63;
		static inline int K_F7 = 64;
		static inline int K_F8 = 65;
		static inline int K_F9 = 66;
		static inline int K_F10 = 67;
		static inline int K_F11 = 68;
		static inline int K_F12 = 69;
		static inline int K_printscreen = 70;
		static inline int K_scrolllock = 71;
		static inline int K_pause = 72;
		static inline int K_insert = 73;
		static inline int K_home = 74;
		static inline int K_pageup = 75;
		static inline int K_delete = 76;
		static inline int K_end = 77;
		static inline int K_pagedown = 78;
		static inline int K_right = 79;
		static inline int K_left = 80;
		static inline int K_down = 81;
		static inline int K_up = 82;
		static inline int K_lctrl = 224;
		static inline int K_lshift = 225;
		static inline int K_lalt = 226;
		static inline int K_lgui = 227;
		static inline int K_rctrl = 228;
		static inline int K_rshift = 229;
		static inline int K_ralt = 230;
		static inline int K_rgui = 231;
		static inline std::unordered_map<std::string, int> m_keys{
			{"k_a", K_a},
			{"k_b", K_b},
			{"k_c", K_c},
			{"k_d", K_d},
			{"k_e", K_e},
			{"k_f", K_f},
			{"k_g", K_g},
			{"k_h", K_h},
			{"k_i", K_i},
			{"k_j", K_j},
			{"k_k", K_k},
			{"k_l", K_l},
			{"k_m", K_m},
			{"k_n", K_n},
			{"k_o", K_o},
			{"k_p", K_p},
			{"k_q", K_q},
			{"k_r", K_r},
			{"k_s", K_s},
			{"k_t", K_t},
			{"k_u", K_u},
			{"k_v", K_v},
			{"k_w", K_w},
			{"k_x", K_x},
			{"k_y", K_y},
			{"k_z", K_z},
			{"k_1", K_1},
			{"k_2", K_2},
			{"k_3", K_3},
			{"k_4", K_4},
			{"k_5", K_5},
			{"k_6", K_6},
			{"k_7", K_7},
			{"k_8", K_8},
			{"k_9", K_9},
			{"k_0", K_0},
			{"k_return",K_return},
			{"k_escape",K_escape},
			{"k_backspace",K_backspace},
			{"k_tab",K_tab},
			{"k_space",K_space},
			{"k_minus",K_minus},
			{"k_equals",K_equals},
			{"k_leftbracket",K_leftbracket},
			{"k_rightbracket",K_rightbracket},
			{"k_backslash",K_backslash},
			{"k_nonushash",K_nonushash},
			{"k_semicolon",K_semicolon},
			{"k_apostrophe",K_apostrophe},
			{"k_grave",K_grave},
			{"k_tild",K_tild},
			{"k_comma",K_comma},
			{"k_period",K_period},
			{"k_slash",K_slash},
			{"k_capslock",K_capslock},
			{"k_f1",K_F1},
			{"k_f2",K_F2},
			{"k_f3",K_F3},
			{"k_f4",K_F4},
			{"k_f5",K_F5},
			{"k_f6",K_F6},
			{"k_f7",K_F7},
			{"k_f8",K_F8},
			{"k_f9",K_F9},
			{"k_f10",K_F10},
			{"k_f11",K_F11},
			{"k_f12",K_F12},
			{"k_printscreen",K_printscreen},
			{"k_scrolllock",K_scrolllock},
			{"k_pause",K_pause},
			{"k_insert",K_insert},
			{"k_home",K_home},
			{"k_pageup",K_pageup},
			{"k_delete",K_delete},
			{"k_end",K_end},
			{"k_pagedown",K_pagedown},
			{"k_right",K_right},
			{"k_left",K_left},
			{"k_down",K_down},
			{"k_up",K_up},
			{"k_lctrl",K_lctrl},
			{"k_lshift",K_lshift},
			{"k_lalt",K_lalt},
			{"k_lgui",K_lgui},
			{"k_rctrl",K_rctrl},
			{"k_rshift",K_rshift},
			{"k_ralt",K_ralt},
			{"k_rgui",K_rgui},
			{"k_unknown", K_UNKNOWN},
		}; 
		static inline std::unordered_map<int, std::string> m_keys__reverse = alib_reverse_hmap(m_keys);
		static int ParseKey(alib_string m_key) {
			alib_string k = alib_lowers(m_key);
			if (m_keys.contains(k)) {
				return m_keys.at(k);
			}
			return K_UNKNOWN;
		}
		static alib_string GetKeyName(int key) {
			if (m_keys__reverse.contains(key)) {
				return m_keys__reverse.at(key);
			}
			return "k_unknown";
		}
};
#endif