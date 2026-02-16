#pragma once

#include <godot_cpp/variant/string.hpp>

#include <string>

#define mmd_unit_conversion 0.079f

namespace godot {
class mmd_helpers {
public:
	static String convert_string(const std::string &p_string, uint8_t p_encoding);
};

} //namespace godot