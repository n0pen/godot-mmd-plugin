#pragma once

#include <godot_cpp//classes/editor_scene_format_importer.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/node.hpp>

#include "thirdparty/ksy/mmd_vmd.h"

namespace godot {
class Animation;
class VMDMMDState;

class EditorSceneImporterMMDVMD : public EditorSceneFormatImporter {
	GDCLASS(EditorSceneImporterMMDVMD, EditorSceneFormatImporter)

	const real_t mmd_unit_conversion = 0.079;
	Vector3 mmd_vec3_to_vector3d(const mmd_vmd_t::vec3_t *vec3) const;
	Vector4 get_rotation_interpolation(std::vector<uint8_t> *interpolation_data);
	virtual Node *import_mmd_vmd_scene(const String &p_path, Skeleton3D *skeleton, uint32_t p_flags, Ref<VMDMMDState> r_state);

public:
	[[nodiscard]] PackedStringArray _get_extensions() const override;
	Node *_import_scene(const String &p_path, uint32_t p_flags, const Dictionary &p_options) override;
	void _get_import_options(const String &p_path) override;
	static Quaternion mmd_vec4_to_quat(const mmd_vmd_t::vec4_t *p_vec4);

	static void _bind_methods(){};
};

class VMDMMDState : public Resource {
	GDCLASS(VMDMMDState, Resource)

	static void _bind_methods(){};
};
}