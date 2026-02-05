#include "editor_scene_importer_mmd_vmd.h"

#include "godot_cpp/classes/resource_importer.hpp"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "thirdparty/ksy/mmd_vmd.h"
#include "thirdparty/shift_jis.h"

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "mmd_helpers.h"

#include <fstream>

namespace godot {

PackedStringArray EditorSceneImporterMMDVMD::_get_extensions() const {
	auto list = PackedStringArray();
	list.push_back("vmd");
	return list;
}

Node *EditorSceneImporterMMDVMD::_import_scene(const String &p_path, uint32_t p_flags, const Dictionary &p_options) {
	Ref<VMDMMDState> state;
	state.instantiate();
	NodePath option = p_options["Skeleton"];
	const auto *editor = EditorInterface::get_singleton();
	const auto *edited_node = editor->get_edited_scene_root();
	auto *node = edited_node->get_node<Skeleton3D>(option);

	if (!node) {
		return nullptr;
	}
	print_line(node->get_path());
	return import_mmd_vmd_scene(p_path, node, p_flags, state);
}

void EditorSceneImporterMMDVMD::_get_import_options(const String &p_path) {
	add_import_option_advanced(Variant::NODE_PATH,
			"Skeleton",
			nullptr,
			PROPERTY_HINT_NODE_PATH_VALID_TYPES,
			"Skeleton3D",
			PROPERTY_USAGE_NODE_PATH_FROM_SCENE_ROOT | PROPERTY_USAGE_DEFAULT

	);
	EditorSceneFormatImporter::_get_import_options(p_path);
}
Quaternion EditorSceneImporterMMDVMD::mmd_vec4_to_quat(const mmd_vmd_t::vec4_t *p) {
	return Quaternion(-p->x(), -p->y(), p->z(), p->w());
}

Vector3 EditorSceneImporterMMDVMD::mmd_vec3_to_vector3d(const mmd_vmd_t::vec3_t *vec3) const {
	return {
		vec3->x() * mmd_unit_conversion,
		vec3->y() * mmd_unit_conversion,
		vec3->z() * mmd_unit_conversion * -1
	};
}

struct AnimationKeyIdx {
	double key_time;
	int32_t track_idx;

	bool operator==(const AnimationKeyIdx &other) const {
		return key_time == other.key_time && track_idx == other.track_idx;
	}
};

struct KeyHasher {
	static uint32_t hash(const AnimationKeyIdx &p_key) {
		uint32_t h = godot::hash_murmur3_one_32(*(uint32_t *)&p_key.key_time);
		return godot::hash_murmur3_one_32(static_cast<uint64_t>(p_key.track_idx), h);
	}
};

Vector4 EditorSceneImporterMMDVMD::get_rotation_interpolation(std::vector<uint8_t> *interpolation_data) {
	Vector4 r_bezier;
	for (int32_t i = 0; i < 4; i++) {
		r_bezier[i] = interpolation_data->at(3 * 16 + i * 4) / 127.0f;
		;
	}
	return r_bezier;
}
Node *EditorSceneImporterMMDVMD::import_mmd_vmd_scene(const String &p_path, Skeleton3D *skeleton, uint32_t p_flags, Ref<VMDMMDState> r_state) {
	if (r_state.is_null()) {
		r_state.instantiate();
	}
	double frame_time = 1.0 / 30.0;
	std::ifstream ifs(
			ProjectSettings::get_singleton()->globalize_path(p_path).utf8().get_data(), std::ifstream::binary);
	kaitai::kstream ks(&ifs);
	mmd_vmd_t vmd = mmd_vmd_t(&ks);
	AnimationPlayer *root = memnew(AnimationPlayer);

	AnimationLibrary *library = memnew(AnimationLibrary);
	Animation *animation = memnew(Animation);
	Array blendshapes;
	TypedArray<String> morph_tracks;
	uint32_t max_frame = 0;

	if (skeleton) {
		auto children = skeleton->get_children();
		MeshInstance3D *mesh;
		for (const auto &child : children) {
			if ((mesh = cast_to<MeshInstance3D>(child))) {
				break;
			}
		}
		if (mesh) {
			for (uint32_t vmd_bs_idx = 0; vmd_bs_idx < vmd.morph_count(); ++vmd_bs_idx) {
				auto morph = vmd.morphs()->at(vmd_bs_idx).get();
				auto morph_name = mmd_helpers::convert_string(sj2utf8(morph->morph_name()), 1);
				if (mesh->find_blend_shape_by_name(morph_name) < 0) {
					continue;
				}
				max_frame = MAX(max_frame, morph->frame_number());
				int32_t current_track;
				if (morph_tracks.find(morph_name) < 0) {
					current_track = animation->add_track(Animation::TYPE_BLEND_SHAPE);
					animation->track_set_path(current_track, NodePath("Mesh:" + morph_name));
					morph_tracks.append(morph_name);
				} else {
					current_track = morph_tracks.find(morph_name);
				}
				animation->blend_shape_track_insert_key(current_track, frame_time * morph->frame_number(), morph->weight());
			}
		}

		auto animation_cache = HashMap<AnimationKeyIdx, mmd_vmd_t::motion_t *, KeyHasher>();
		TypedArray<String> motion_tracks;
		for (uint32_t vmd_motion_idx = 0; vmd_motion_idx < vmd.motion_count(); ++vmd_motion_idx) {
			auto motion = vmd.motions()->at(vmd_motion_idx).get();
			//if (motion->frame_number()>20) continue;
			max_frame = MAX(max_frame, motion->frame_number());

			auto bone_name = mmd_helpers::convert_string(sj2utf8(motion->bone_name()), 1);
			auto bone = skeleton->find_bone(bone_name);
			if (bone < 0)
				continue;
			int32_t rotation_track_v, rotation_track_b;

			int32_t translation_track;
			Transform3D bone_transform = skeleton->get_bone_rest(bone);
			auto rot_path1 = vformat(".:bones/%d/bone_meta/mmd_rot_1", bone);
			auto rot_bezier_path = vformat(".:bones/%d/bone_meta/mmd_rot_bezier", bone);

			if (motion_tracks.find(bone_name) < 0) {
				rotation_track_v = animation->add_track(Animation::TYPE_VALUE);
				animation->track_set_path(rotation_track_v, NodePath(rot_path1));
				animation->value_track_set_update_mode(rotation_track_v, Animation::UpdateMode::UPDATE_DISCRETE);

				rotation_track_b = animation->add_track(Animation::TYPE_BEZIER);
				animation->track_set_path(rotation_track_b, NodePath(rot_bezier_path));

				translation_track = animation->add_track(Animation::TYPE_POSITION_3D);
				animation->track_set_path(translation_track, NodePath(".:" + bone_name));
				motion_tracks.append(bone_name);
			} else {
				rotation_track_v = animation->find_track(NodePath(rot_path1), Animation::TYPE_VALUE);
				rotation_track_b = animation->find_track(NodePath(rot_bezier_path), Animation::TYPE_BEZIER);
				translation_track = animation->find_track(NodePath(".:" + bone_name), Animation::TYPE_POSITION_3D);
			}

			Quaternion quat = mmd_vec4_to_quat(motion->rotation());

			Vector2 p_in_handle, p_out_handle;
			double p_time = frame_time * motion->frame_number();
			animation_cache.insert(AnimationKeyIdx{ p_time, rotation_track_b }, motion);

			animation->bezier_track_insert_key(rotation_track_b, frame_time * motion->frame_number(), 0);
			animation->track_insert_key(
					rotation_track_v,
					p_time,
					quat);

			animation->position_track_insert_key(
					translation_track,
					p_time,
					bone_transform.origin + mmd_vec3_to_vector3d(motion->position()));
		}

		for (int track_idx = 0; track_idx < animation->get_track_count(); ++track_idx) {
			if (animation->track_get_type(track_idx) == Animation::TYPE_BEZIER) {
				auto pathb = animation->track_get_path(track_idx);
				print_line(pathb);
				auto track_path =  pathb.get_concatenated_names() + StringName(":") + pathb.get_concatenated_subnames();
				print_line(track_path);
				auto tphase = animation->add_track(Animation::TYPE_VALUE);
				auto trot2 = animation->add_track(Animation::TYPE_VALUE);
				auto trot1 = animation->find_track(NodePath(track_path.replace("bezier", "1")), Animation::TYPE_VALUE);

				auto path = track_path.replace("bezier", "phase");
				auto path2 = track_path.replace("bezier", "2");

				animation->track_set_path(tphase, path);
				animation->track_set_path(trot2, path2);
				animation->value_track_set_update_mode(tphase, Animation::UpdateMode::UPDATE_DISCRETE);
				animation->value_track_set_update_mode(trot2, Animation::UpdateMode::UPDATE_DISCRETE);

				for (int key_idx = 0; key_idx < animation->track_get_key_count(track_idx) - 1; ++key_idx) {
					if (key_idx == 0) {
						animation->bezier_track_set_key_value(track_idx, key_idx, 0);
					}

					auto t1 = animation->track_get_key_time(track_idx, key_idx);
					auto t2 = animation->track_get_key_time(track_idx, key_idx + 1);
					auto v1 = animation->bezier_track_get_key_value(track_idx, key_idx);
					animation->bezier_track_set_key_value(track_idx, key_idx + 1, !v1);

					animation->track_insert_key(trot2, t1, animation->track_get_key_value(trot1, key_idx + 1));
					animation->track_insert_key(tphase, animation->track_get_key_time(track_idx, key_idx), v1);
					auto v2 = animation->bezier_track_get_key_value(track_idx, key_idx + 1);
					if (!animation_cache.has({ t1, track_idx }) || !animation_cache.has({ t2, track_idx })) {
						continue;
					}
					auto *motion1 = animation_cache.get({ t1, track_idx });
					auto *motion2 = animation_cache.get({ t2, track_idx });
					auto delta_time = t1 - t2;
					Vector4 r_bezier = get_rotation_interpolation(motion2->interpolation());
					animation->bezier_track_set_key_in_handle(
							track_idx,
							key_idx + 1,
							{ static_cast<real_t>((1 - r_bezier[2]) * delta_time), (r_bezier[3] - 1) * (v2 - v1) });
					animation->bezier_track_set_key_out_handle(
							track_idx,
							key_idx,
							{ -static_cast<real_t>(r_bezier[0] * delta_time), r_bezier[1] * (v2 - v1) });
				}
			}
		}
	}

	animation->set_length(frame_time * max_frame);
	library->add_animation("dance", animation);
	root->add_animation_library("vmd", library);

	return root;
}
} //namespace godot