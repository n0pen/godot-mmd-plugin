#pragma once

#include <godot_cpp/variant/string.hpp>

#include <string>

namespace godot {
class mmd_helpers {
public:
	static String convert_string(const std::string &p_string, uint8_t p_encoding);
};

} //namespace godot