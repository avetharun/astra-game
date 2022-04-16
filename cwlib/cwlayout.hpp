#pragma once
#include "_cwincludes.h"
#include "cwerror.h"
#include "cwasset.hpp"



struct cwLayout {
private:

	unsigned long long FS_SIZE;
	char* fs;
	static cwLayout* inst;
	std::map<unsigned short, std::function<int()>>::iterator ev_iterator = {};
	std::map<int, Sprite*> asset_sprite_array = {};
	unsigned char HEAD_SIZE;
	unsigned char VEC_SIZE = 2; // {x,y} &/ {w,h} &/ {vc0,vc1}
	unsigned char CWL_LINE_SIZE = 2; // {xs,ys},{xe,ye}
	unsigned char CWL_RECT_SIZE = 2; // {x,y},{w,h}
	unsigned long long CWL_COL_LINE_ROOT = INT64_MAX;
	unsigned long long CWL_COL_LINE_END = INT64_MAX;
	unsigned long long CWL_COL_RECT_ROOT = INT64_MAX;
	unsigned long long CWL_COL_RECT_END = INT64_MAX;
	unsigned long long CWL_ASSET_ROOT = INT64_MAX;
	unsigned long long CWL_ASSET_END = INT64_MAX;
	unsigned long long CWL_FUNCTIONS_ROOT = INT64_MAX;
	const unsigned long long CWL_COL_LINE_ROOT_OFFSET = 0x06;
	const unsigned long long CWL_COL_LINE_END_OFFSET = 0x0E;
	const unsigned long long CWL_COL_ASSET_ROOT_OFFSET = 0x17;
	const unsigned long long CWL_COL_ASSET_END_OFFSET = 0x1F;
	const unsigned long long CWL_COL_RECT_ROOT_OFFSET = 0x27;
	const unsigned long long CWL_COL_RECT_END_OFFSET = 0x2F;
public:
	std::string name;
	std::map<unsigned short, std::function<int()>> events = {};
	std::map<int, bool*> event_triggers = {};
	MeshCollider2d* lines = {};
	std::vector<RectCollider2d*> rects = {};

private:
	int _cwlLoadF(const char* fn) { // Load CWL file into memory
		this->name = fn;
		alib_read_file(fn, &fs, &FS_SIZE);
		if (this->fs == nullptr) {
			return 1;
		}
		// Size of the CWL header. Usually 10 bytes, but depending on application, some things used at runtime could be put in header, and fetched
		// by the game (add 1 byte of padding (0xFB) for readability (As there is currently no way to visualize it other than in hex))
		HEAD_SIZE = fs[0x00] + 1;
		if (HEAD_SIZE < 68) {
			const char* err = alib_va_arg_parse("CWL file header isn't 68 bytes long!\nSupplied: %d\nAttemting to use anyways, but strange things may happen!", (int)HEAD_SIZE);
			cwError::sstate(cwError::CW_WARN);
			cwError::serror(err);
		}
		CWL_RECT_SIZE = fs[0x03]; // Size of a pre-defined collision rect. (0-255 bytes per collider)
		VEC_SIZE = fs[0x04]; // Size of a vector (0-255 bytes per vector pair)
		CWL_LINE_SIZE = fs[0x05]; // Size of a pre-defined collision line. (0-255 bytes per collider)
		memcpy(&CWL_COL_LINE_ROOT, fs + CWL_COL_LINE_ROOT_OFFSET, 8); // 8 byte int (64 bit) pointing to the offset mesh colliders start.
		memcpy(&CWL_COL_LINE_END, fs + CWL_COL_LINE_END_OFFSET, 8); // 8 byte int (64 bit) pointing to the offset mesh colliders end.
		memcpy(&CWL_COL_RECT_ROOT, fs + CWL_COL_RECT_ROOT_OFFSET, 8); // 8 byte int (64 bit) pointing to the offset rect colliders start.
		memcpy(&CWL_COL_RECT_END, fs + CWL_COL_RECT_END_OFFSET, 8); // 8 byte int (64 bit) pointing to the offset rect colliders end.
		memcpy(&CWL_ASSET_ROOT, fs + CWL_COL_ASSET_ROOT_OFFSET, 8); // 8 byte int pointing to the offset asset paths are defined.
		memcpy(&CWL_ASSET_END, fs + CWL_COL_ASSET_END_OFFSET, 8); // 8 byte int pointing to offset assets path declaration ends


#ifdef CWL_DEBUG
		cwError::sstate(cwError::CW_DEBUG);
		cwError::serrof(R"(Reading CWL file %s,
header size: %d at 0x00,
vector size %d at 0x04,
rect size (in vectors) %d at 0x03
line size (in vectors) %d at 0x05,
line root: %lld at 0x06,
line end: %lld at 0x0E,
assetf root: %lld at 0x%X,
assetf end: %lld at 0x%X,
rect root: %lld at 0x%X,
rect end: %lld at 0x%X,
)",
fn,
HEAD_SIZE,
VEC_SIZE,
CWL_RECT_SIZE,
CWL_LINE_SIZE,
CWL_COL_LINE_ROOT,
CWL_COL_LINE_END,
CWL_ASSET_ROOT,
CWL_COL_ASSET_ROOT_OFFSET,
CWL_ASSET_END,
CWL_COL_ASSET_END_OFFSET,
CWL_COL_RECT_ROOT,
CWL_COL_RECT_ROOT_OFFSET,
CWL_COL_RECT_END,
CWL_COL_RECT_END_OFFSET
);
#endif

		char* header_data = (char*)malloc(HEAD_SIZE + 1);
		int HEAD_ITER = 1; // Start at 1.
		memcpy(header_data, fs, HEAD_SIZE);
		if (header_data == nullptr) {
			cwError::sstate(cwError::CW_ERROR);
			cwError::serror("Unable to read header of CWL file- Assumed to be empty.");
			return 1;
		}

		// File decompilation pass
		// Asset file definition format: <path_str_local> x00 <id_char> [NEXT FILE]
		for (size_t ait = CWL_ASSET_ROOT; ait < FS_SIZE && ait < CWL_ASSET_END;) {
			size_t _len = strlen(fs + ait);
			unsigned short path_id;
			char* path_str = (char*)malloc(_len + 1);
			memcpy(path_str, fs + ait, _len);
			path_str[_len] = '\0'; // Append null-terminator. If one isn't already there, then that's not an issue.
			memcpy(&path_id, fs + ait + _len + 1, 2); // Next 2 bytes after null terminator are the asset ID (0-USHORT_MAX)
			// *should* only output the current string
			//cwError::serrof("Importing file %s, offset: 0x%zX, with ID of %i\n", path_str, ait+_len, path_id);
			if (
				alib_endswith(path_str, ".png") |
				alib_endswith(path_str, ".jpg") |
				alib_endswith(path_str, ".bmp")
				) {
				Sprite* _s = new Sprite(path_str);
				_s->enabled = true;
				_s->transform.position = { 0,0 };
				_s->transform.angle = 0;
				_s->transform.scale = { 0,0 };
				_s->center = false;
				_s->layer = 0;
				_s->origin = { 0,0 };
				asset_sprite_array.insert({ path_id, _s });
			}
			if (alib_endswith(path_str, ".cwa")) {


			}

			free(path_str);
			ait += _len + 3;
			if (fs[ait] == '\0') {
				break;
				// No more strings found!
			}
		}
		// Collider (rect) Decompilation Pass
		// 
		for (size_t it = CWL_COL_RECT_ROOT; (it < FS_SIZE && it < CWL_COL_RECT_END);) {
			Vector2 xy = _cwlVectorPair(it);
			Vector2 wh = _cwlVectorPair(it + VEC_SIZE);
			RectCollider2d* r = new RectCollider2d();

			cwError::sstate(cwError::CW_DEBUG);
			cwError::serrof("Created rect with with %d, %d at %d, %d at offset 0x%X", wh.x, wh.y, xy.x, xy.y, it);

			r->layer = fs[it + VEC_SIZE * 8];
			r->rect = new SDL_Rect{
				xy.x, xy.y,
				wh.x, wh.y
			};
			rects.push_back(r);
			it += VEC_SIZE * 8 + CWL_RECT_SIZE;//Assume there's padding after a vector for the collider ID. If not, well oops!
		}


		// Collider (mesh) Decompilation Pass
		for (size_t it = CWL_COL_LINE_ROOT; (it < FS_SIZE && it < CWL_COL_LINE_END);) {
			Vector2 l1 = _cwlVectorPair(it);
			Vector2 l2 = _cwlVectorPair(it + VEC_SIZE);
			MeshLine l{
				l1,
				l2
			};
			l.layer = fs[it + VEC_SIZE * 4];
			// Line is blank, (0x00), so skip.
			if (l.layer == 0 && l.end == 0 && l.start == 0) {
				it += VEC_SIZE * 4 + CWL_LINE_SIZE;//Assume there's padding after a vector for the collider ID. If not, well oops!

				continue;
			}
#ifdef CWL_DEBUG
			printf("Created line at (%i, %i) to (%i, %i) at offset %zi. Collider ID: ",
				l.start.x, l.start.y,
				l.end.x, l.end.y,
				it
			);
			alib_print_byte(fs[it + VEC_SIZE * 4]);
			std::cout << '\n';
#endif
			lines->lines.push_back(l);

			it += VEC_SIZE * 4 + CWL_LINE_SIZE;//Assume there's padding after a vector for the collider ID. If not, well oops!
		}

		return 0;
	}
	Vector2 _cwlVectorPair(size_t off) {
		Vector2 v{};
		memcpy(&v.x, fs + off, VEC_SIZE);
		memcpy(&v.y, fs + off + VEC_SIZE, VEC_SIZE);
		return v;
	}
	void _cwlFree() {
		free(fs); // We created the [fs] object using malloc, so free() should free it.
	}
public:
	int cwlLoad(const char* fn) {
		lines = new MeshCollider2d();
		return _cwlLoadF(fn);
	}
	cwLayout() {
		cwLayout::inst = this;
	};
	cwLayout(const char* c) {

		cwLayout::inst = this;
		cwlLoad(c);
	};
	//char* getHeader() {}


	/** \brief Register scene event under name ev_strint (using 64-bit char literal. example: 'event_name') */
	void registerEvent(unsigned short ev_id, std::function<int()> ev_func) {
		events.insert({ ev_id,ev_func });
	}

	void __registerEventTrigger(unsigned short ev_id, bool* boolptr) {
		std::cout << "cwl::__registerEventTrigger is not usable, please use cwl::callEvent" << std::endl;
		/*ev_iterator = events.find(ev_strint);
		if (ev_iterator == events.end()) {
			return;
		}
		event_triggers.insert({ev_strint, boolptr});*/
	}
	void callEvent(unsigned short ev_strint) {
		ev_iterator = events.find(ev_strint);
		if (ev_iterator != events.end()) {
			events[ev_strint]();
		}
	}
	void removeEvent(unsigned short ev_strint) {
		ev_iterator = events.find(ev_strint);
		if (ev_iterator != events.end()) {
			events.erase(ev_iterator);
		}
	}

	std::function<void(long long evid, long long evbits)> onEvent = [](long long _, long long __) { noop; };
	void Update() {
		for (int i = 0; i < events.size(); i++) {
		}
	}

	static cwLayout* Instance() { return inst; }
	static void Discard() {
		if (Instance() == NULL || Instance() == nullptr) { return; }
		if (&Instance()->asset_sprite_array) {
			for (const auto& SpritePair : Instance()->asset_sprite_array) {
				SpritePair.second->operator~();
			}
		}
		Instance()->lines->free();
	}

	void operator ~ () {
		this->Discard();
	}

};
typedef cwLayout CWLScene;
cwLayout* cwLayout::inst = nullptr;
