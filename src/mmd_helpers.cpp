#include "mmd_helpers.h"

#include <godot_cpp/variant/packed_byte_array.hpp>

namespace godot {

String mmd_helpers::convert_string(const std::string &p_string, uint8_t p_encoding) {
	if (!p_string.empty()) {
		auto d_string = PackedByteArray();
		size_t str_len = p_string.size();
		d_string.resize(str_len);
		const char *str_data = p_string.c_str();
		if (str_data != nullptr) {
			memcpy(d_string.ptrw(), str_data, str_len / 2 * sizeof(char16_t));
		}
		if (!p_encoding) {
			return d_string.get_string_from_utf16();
		}
		return d_string.get_string_from_utf8();
	}
	return "";
}

}

