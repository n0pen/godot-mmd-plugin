#include "mmd_ik_modfier3d.h"

#include "godot_cpp/classes/skeleton3d.hpp"
#include "mmd_helpers.h"

namespace godot {
TypedArray<MMDIKModifierConfig> MMDIKModifier3D::get_config() const {
	return config;
}

void MMDIKModifier3D::set_config(const TypedArray<MMDIKModifierConfig> &p_config) {
	if (config == p_config) {
		return;
	}
	config = p_config;
	notify_property_list_changed();
}

void MMDIKModifier3D::_get_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < config.size(); ++i) {
		String base = "config/" + itos(i) + "/";

		// 1. Group Header (NIL type, used only for UI grouping)
		p_list->push_back(PropertyInfo(Variant::NIL, "config " + itos(i), PROPERTY_HINT_NONE, base, PROPERTY_USAGE_GROUP));

		p_list->push_back(PropertyInfo(Variant::INT, base + "iterations", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
		p_list->push_back(PropertyInfo(Variant::FLOAT, base + "iteration_angle_limit", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));

		// Use INT for bone indices; Bone selection UI is usually handled via custom InspectorPlugins in GDExtension
		p_list->push_back(PropertyInfo(Variant::INT, base + "effector_bone", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
		p_list->push_back(PropertyInfo(Variant::INT, base + "target_bone", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));

		p_list->push_back(PropertyInfo(Variant::INT,  base + "chain/size", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));

		Ref<MMDIKModifierConfig> conf = config[i];
		if (conf.is_valid()) {
			for (int j = 0; j < conf->chain.size(); ++j) {
				String c_base = base + "chain/" + itos(j) + "/";

				// 2. Subgroup Header
				p_list->push_back(PropertyInfo(Variant::NIL, "Chain " + itos(j), PROPERTY_HINT_NONE, c_base, PROPERTY_USAGE_SUBGROUP));

				// 3. CHANGE FROM PROPERTY_USAGE_GROUP TO PROPERTY_USAGE_DEFAULT
				p_list->push_back(PropertyInfo(Variant::INT, c_base + "bone_index", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
				p_list->push_back(PropertyInfo(Variant::BOOL, c_base + "is_limited", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
				p_list->push_back(PropertyInfo(Variant::AABB, c_base + "limits", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
			}
		}
	}
}

bool MMDIKModifier3D::_get(const StringName &p_name, Variant &r_ret) const {
	PackedStringArray parts = String(p_name).split("/");
	if (parts.size() < 3 || parts[0] != "config")
		return false;

	int config_idx = parts[1].to_int();
	if (config_idx < 0 || config_idx >= config.size())
		return false;

	Ref<MMDIKModifierConfig> conf = config[config_idx];
	if (conf.is_null())
		return false;

	String field = parts[2];

	if (parts.size() == 3) {
		if (field == "iterations") {
			r_ret = conf->iterations;
			return true;
		}
		if (field == "iteration_angle_limit") {
			r_ret = conf->iteration_angle_limit;
			return true;
		}
		if (field == "effector_bone") {
			r_ret = conf->effector_bone;
			return true;
		}
		if (field == "target_bone") {
			r_ret = conf->target_bone;
			return true;
		}
	}
	if (parts.size() == 4 && field == "chain" && parts[3] == "size") {
		r_ret = conf->chain.size();
		return true;
	}

	if (parts.size() == 5 && field == "chain") {
		int chain_idx = parts[3].to_int();
		if (chain_idx < 0 || chain_idx >= conf->chain.size())
			return false;

		Ref<MMDIKChain> chain_item = conf->chain[chain_idx];
		if (chain_item.is_valid()) {
			String chain_field = parts[4];
			if (chain_field == "bone_index") {
				r_ret = chain_item->bone_index;
				return true;
			}
			if (chain_field == "is_limited") {
				r_ret = chain_item->is_limited;
				return true;
			}
			if (chain_field == "limits") {
				r_ret = chain_item->limits;
				return true;
			}
		}
	}

	return false;
}

bool MMDIKModifier3D::_set(const StringName &p_name, const Variant &p_value) {
	PackedStringArray parts = String(p_name).split("/");
	if (parts.size() < 3 || parts[0] != "config")
		return false;

	int config_idx = parts[1].to_int();
	if (config_idx < 0 || config_idx >= config.size())
		return false;

	Ref<MMDIKModifierConfig> conf = config[config_idx];
	if (conf.is_null())
		return false;

	String field = parts[2];

	if (parts.size() == 3) {
		if (field == "iterations") {
			conf->iterations = p_value;
			return true;
		}
		if (field == "iteration_angle_limit") {
			conf->iteration_angle_limit = p_value;
			return true;
		}
		if (field == "effector_bone") {
			conf->effector_bone = p_value;
			return true;
		}
		if (field == "target_bone") {
			conf->target_bone = p_value;
			return true;
		}
	}

	if (parts.size() == 4 && field == "chain" && parts[3] == "size") {
		int old_size = conf->chain.size();
		conf->chain.resize(p_value);
		for (int i = old_size; i < conf->chain.size(); ++i) {
			conf->chain[i] = Ref(memnew(MMDIKChain));
		}
		notify_property_list_changed();
		return true;
	}

	if (parts.size() == 5 && field == "chain") {
		int chain_idx = parts[3].to_int();
		if (chain_idx < 0 || chain_idx >= conf->chain.size())
			return false;

		Ref<MMDIKChain> chain_item = conf->chain[chain_idx];
		if (chain_item.is_null())
			return false;

		String chain_field = parts[4];
		if (chain_field == "bone_index") {
			chain_item->bone_index = p_value;
			return true;
		}
		if (chain_field == "is_limited") {
			chain_item->is_limited = p_value;
			return true;
		}
		if (chain_field == "limits") {
			chain_item->limits = p_value;
			auto abs_limit = chain_item->limits.get_position().abs() + chain_item->limits.get_end().abs();
			if (!Math::is_zero_approx(abs_limit.x) && Math::is_zero_approx(abs_limit.y) && Math::is_zero_approx(abs_limit.z))
				chain_item->rotation_axis = Vector3(1, 0, 0);
			else if (!Math::is_zero_approx(abs_limit.y) && Math::is_zero_approx(abs_limit.x) && Math::is_zero_approx(abs_limit.z))
				chain_item->rotation_axis = Vector3(0, 1, 0);
			else if (!Math::is_zero_approx(abs_limit.z) && Math::is_zero_approx(abs_limit.x) && Math::is_zero_approx(abs_limit.y))
				chain_item->rotation_axis = Vector3(0, 0, 1);

			if (!Math::is_zero_approx(chain_item->limits.size.length_squared()))
				chain_item->is_axis_locked = true;

			if (chain_item->limits.get_position().x > -Math_PI * 0.5 && chain_item->limits.get_end().x < Math_PI * 0.5)
				chain_item->rotation_order = EULER_ORDER_ZXY;
			else if (chain_item->limits.get_position().y > -Math_PI * 0.5 && chain_item->limits.get_end().y < Math_PI * 0.5)
				chain_item->rotation_order = EULER_ORDER_XYZ;
			else
				chain_item->rotation_order = EULER_ORDER_YZX;

			notify_property_list_changed();
			return true;
		}
	}

	return false;
}

Vector3 limit_angle(Vector3 p_angle, bool p_axis_lim, Vector3 p_low, Vector3 p_high) {
	if (!p_axis_lim) {
		return Vector3(
				Math::clamp(p_angle.x, p_low.x, p_high.x),
				Math::clamp(p_angle.y, p_low.y, p_high.y),
				Math::clamp(p_angle.z, p_low.z, p_high.z));
	}

	Vector3 vec_l1 = 2.0f * p_low - p_angle;
	Vector3 vec_h1 = 2.0f * p_high - p_angle;

	if (p_angle.x < p_low.x) {
		p_angle.x = (vec_l1.x <= p_high.x) ? vec_l1.x : p_low.x;
	} else if (p_angle.x > p_high.x) {
		p_angle.x = (vec_h1.x >= p_low.x) ? vec_h1.x : p_high.x;
	}

	if (p_angle.y < p_low.y) {
		p_angle.y = (vec_l1.y <= p_high.y) ? vec_l1.y : p_low.y;
	} else if (p_angle.y > p_high.y) {
		p_angle.y = (vec_h1.y >= p_low.y) ? vec_h1.y : p_high.y;
	}

	if (p_angle.z < p_low.z) {
		p_angle.z = (vec_l1.z <= p_high.z) ? vec_l1.z : p_low.z;
	} else if (p_angle.z > p_high.z) {
		p_angle.z = (vec_h1.z >= p_low.z) ? vec_h1.z : p_high.z;
	}

	return p_angle;
}

void MMDIKModifier3D::_process_modification_with_delta(double p_delta) {
	auto skeleton = get_skeleton();
	for (int config_idx = 0; config_idx < config.size(); ++config_idx) {
		Ref<MMDIKModifierConfig> conf = config[config_idx];

		if (conf.is_null()) {
			continue;
		}
		if (conf->effector_bone < 0 || conf->target_bone < 0) {
			continue;
		}
		bool skip = false;
		for (int i = 0; i < conf->chain.size(); ++i) {
			if (cast_to<MMDIKChain>(conf->chain[i])->bone_index < 0) {
				skip = true;
				break;
			}
		}
		if (skip) {
			continue;
		}
		auto effector_pose = skeleton->get_bone_global_pose(conf->effector_bone);
		auto target_pose = skeleton->get_bone_global_pose(conf->target_bone);
		if (effector_pose.origin.distance_squared_to(target_pose.origin) < MMD_IK_TARGET_DISTANCE) {
			continue;
		}

		for (int i = 0; i < conf->iterations; ++i) {
			bool axis_limited = i < conf->iterations / 2;
			auto tip = skeleton->get_bone_global_pose(conf->effector_bone);

			for (int chain_idx = 0; chain_idx < conf->chain.size(); ++chain_idx) {
				Ref<MMDIKChain> chain = conf->chain[chain_idx];
				if (!chain.is_valid()) {
					continue;
				}
				auto link = skeleton->get_bone_global_pose(chain->bone_index);
				auto target_vec = target_pose.origin - link.origin;
				auto ik_vec = tip.origin - link.origin;

				auto j_rot = Quaternion();

				auto fixed_axis = chain->rotation_axis;

				if (axis_limited && fixed_axis.length_squared() > 0) {
					auto global_fixed_axis = (link.basis.xform(fixed_axis)).normalized();
					auto proj_target = (target_vec - global_fixed_axis * target_vec.dot(global_fixed_axis)).normalized();
					auto proj_ik = (ik_vec - global_fixed_axis * ik_vec.dot(global_fixed_axis)).normalized();

					auto dot_v = CLAMP(proj_ik.dot(proj_target), -1.0, 1.0);
					auto angle = acos(dot_v);
					if (fabs(angle) > MMD_IK_TARGET_ANGLE) {
						auto cross_v = proj_ik.cross(proj_target);
						auto sign_v = cross_v.dot(global_fixed_axis) > 0 ? 1.0 : -1.0;
						auto limit = conf->iteration_angle_limit * (1 + i);
						j_rot = Quaternion(fixed_axis, CLAMP(angle * sign_v, -limit, limit)).normalized();
					}
				} else {
					auto target_dir = target_vec.normalized();
					auto ik_dir = ik_vec.normalized();
					auto dot_v = CLAMP(ik_dir.dot(target_dir), -1.0, 1.0);
					auto angle = acos(dot_v);

					if (abs(angle) > MMD_IK_TARGET_ANGLE) {
						auto axis = link.basis.xform_inv(ik_dir.cross(target_dir)).normalized();
						auto limit = conf->iteration_angle_limit * (1 + i);
						j_rot = Quaternion(axis, CLAMP(angle, -limit, limit)).normalized();

					} else {
						j_rot = Quaternion();
					}
				}
				if (j_rot != Quaternion()) {
					auto j_res = (skeleton->get_bone_pose_rotation(chain->bone_index) * j_rot).normalized();

					if (chain->is_limited) {
						auto euler = j_res.get_euler(chain->rotation_order);
						euler = limit_angle(euler, axis_limited, chain->limits.get_position(), chain->limits.get_end());
						j_res = Basis::from_euler(euler, chain->rotation_order).get_rotation_quaternion();
					}

					skeleton->set_bone_pose_rotation(chain->bone_index, j_res);
					skeleton->force_update_all_bone_transforms();
					tip = skeleton->get_bone_global_pose(conf->effector_bone);
				}
			}
		}
	}
}

void MMDIKModifier3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_config"), &MMDIKModifier3D::get_config);
	ClassDB::bind_method(D_METHOD("set_config", "p_config"), &MMDIKModifier3D::set_config);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "config", PROPERTY_HINT_ARRAY_TYPE, "MMDIKModifierConfig"), "set_config", "get_config");
}

} //namespace godot