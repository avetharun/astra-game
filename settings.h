#pragma once
#ifndef CW_SETTINGS_HEADER
#define CW_SETTINGS_HEADER


#include "input.h"
#include "cwlib/cwabt.hpp"
struct CWSettings {

	struct KeyBinds {
		static inline int UP = Input::K_W;
		static inline int DOWN = Input::K_S;
		static inline int LEFT = Input::K_A;
		static inline int RIGHT = Input::K_D;
		static inline int USE = Input::K_E;
		static inline int PAUSE = Input::K_escape;
		static inline int INVENTORY = Input::K_Q;
	};
	struct Rendering {
		static inline bool ALWAYS_SHOW_BORDERS = false;
	};
	static void Parse() {
		if (!alib_file_exists) { Save(); return; }
		ABTDataStub m_abt;
		m_abt.deserialize_json(alib_file_read("data/settings.abt"));
		KeyBinds::UP =   Input::ParseKey(m_abt.get_or_default_string("up", "k_w"));
		KeyBinds::DOWN = Input::ParseKey(m_abt.get_or_default_string("down", "k_s"));
		KeyBinds::LEFT = Input::ParseKey(m_abt.get_or_default_string("left", "k_a"));
		KeyBinds::RIGHT = Input::ParseKey(m_abt.get_or_default_string("right", "k_d"));
		KeyBinds::USE = Input::ParseKey(m_abt.get_or_default_string("use", "k_e"));
		KeyBinds::PAUSE = Input::ParseKey(m_abt.get_or_default_string("pause", "k_escape"));
		KeyBinds::INVENTORY = Input::ParseKey(m_abt.get_or_default_string("inventory", "k_q"));
		Rendering::ALWAYS_SHOW_BORDERS = m_abt.get_or_default_bool("always_show_borders", false);
	}
	static void Save() {
		ABTDataStub s;
		s.set_string("up", Input::GetKeyName(KeyBinds::UP));
		s.set_string("down", Input::GetKeyName(KeyBinds::DOWN));
		s.set_string("left", Input::GetKeyName(KeyBinds::LEFT));
		s.set_string("right", Input::GetKeyName(KeyBinds::RIGHT));
		s.set_string("use", Input::GetKeyName(KeyBinds::USE));
		s.set_string("pause", Input::GetKeyName(KeyBinds::PAUSE));
		s.set_string("inventory", Input::GetKeyName(KeyBinds::INVENTORY));
		s.set_bool("always_show_borders", Rendering::ALWAYS_SHOW_BORDERS);
		alib_string _d = s.dump();
		alib_file_write("data/settings.abt", _d, _d.size());
	}
};


#endif