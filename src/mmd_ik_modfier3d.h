#pragma once
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/skeleton_modifier3d.hpp"

#define MMD_IK_TARGET_DISTANCE 1e-6f
#define MMD_IK_TARGET_ANGLE 1e-6f

namespace godot {
class MMDIKChain : public Resource {
	GDCLASS(MMDIKChain, Resource);

public:
	int32_t bone_index = -1;
	bool is_limited = false;
	AABB limits = AABB();
	Vector3 rotation_axis = Vector3();
	bool is_axis_locked = false;
	EulerOrder rotation_order;
// Setters
	void set_bone_index(int32_t p_index) { bone_index = p_index; }
	void set_is_limited(bool p_limited) { is_limited = p_limited; }
	void set_limits(AABB p_limits) { limits = p_limits; }
	void set_rotation_axis(Vector3 p_axis) { rotation_axis = p_axis; }
	void set_is_axis_locked(bool p_locked) { is_axis_locked = p_locked; }
	void set_rotation_order(int p_order) { rotation_order = (EulerOrder)p_order; }

	// Getters
	int32_t get_bone_index() const { return bone_index; }
	bool get_is_limited() const { return is_limited; }
	AABB get_limits() const { return limits; }
	Vector3 get_rotation_axis() const { return rotation_axis; }
	bool get_is_axis_locked() const { return is_axis_locked; }
	int get_rotation_order() const { return (int)rotation_order; }

	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_bone_index"), &MMDIKChain::get_bone_index);
		ClassDB::bind_method(D_METHOD("set_bone_index", "p_index"), &MMDIKChain::set_bone_index);
		ClassDB::bind_method(D_METHOD("get_is_limited"), &MMDIKChain::get_is_limited);
		ClassDB::bind_method(D_METHOD("set_is_limited", "p_limited"), &MMDIKChain::set_is_limited);
		ClassDB::bind_method(D_METHOD("get_limits"), &MMDIKChain::get_limits);
		ClassDB::bind_method(D_METHOD("set_limits", "p_limits"), &MMDIKChain::set_limits);
		ClassDB::bind_method(D_METHOD("get_rotation_axis"), &MMDIKChain::get_rotation_axis);
		ClassDB::bind_method(D_METHOD("set_rotation_axis", "p_axis"), &MMDIKChain::set_rotation_axis);
		ClassDB::bind_method(D_METHOD("get_is_axis_locked"), &MMDIKChain::get_is_axis_locked);
		ClassDB::bind_method(D_METHOD("set_is_axis_locked", "p_locked"), &MMDIKChain::set_is_axis_locked);
		ClassDB::bind_method(D_METHOD("get_rotation_order"), &MMDIKChain::get_rotation_order);
		ClassDB::bind_method(D_METHOD("set_rotation_order", "p_order"), &MMDIKChain::set_rotation_order);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "bone_index"), "set_bone_index", "get_bone_index");
		ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_limited"), "set_is_limited", "get_is_limited");
		ADD_PROPERTY(PropertyInfo(Variant::AABB, "limits"), "set_limits", "get_limits");
		ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "rotation_axis"), "set_rotation_axis", "get_rotation_axis");
		ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_axis_locked"), "set_is_axis_locked", "get_is_axis_locked");
		ADD_PROPERTY(PropertyInfo(Variant::INT, "rotation_order"), "set_rotation_order", "get_rotation_order");
	}

};

class MMDIKModifierConfig : public Resource {
	GDCLASS(MMDIKModifierConfig, Resource);

public:
	int32_t iterations = 5;
	real_t iteration_angle_limit = 0.01;

	int32_t effector_bone = -1;
	int32_t target_bone = -1;

	TypedArray<MMDIKChain> chain = {};
	// Setters
	void set_iterations(int32_t p_iters) { iterations = p_iters; }
	void set_iteration_angle_limit(real_t p_limit) { iteration_angle_limit = p_limit; }
	void set_effector_bone(int32_t p_bone) { effector_bone = p_bone; }
	void set_target_bone(int32_t p_bone) { target_bone = p_bone; }
	void set_chain(const TypedArray<MMDIKChain> &p_chain) { chain = p_chain; }

	// Getters
	int32_t get_iterations() const { return iterations; }
	real_t get_iteration_angle_limit() const { return iteration_angle_limit; }
	int32_t get_effector_bone() const { return effector_bone; }
	int32_t get_target_bone() const { return target_bone; }
	TypedArray<MMDIKChain> get_chain() const { return chain; }

	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_iterations"), &MMDIKModifierConfig::get_iterations);
		ClassDB::bind_method(D_METHOD("set_iterations", "p_iters"), &MMDIKModifierConfig::set_iterations);
		ClassDB::bind_method(D_METHOD("get_iteration_angle_limit"), &MMDIKModifierConfig::get_iteration_angle_limit);
		ClassDB::bind_method(D_METHOD("set_iteration_angle_limit", "p_limit"), &MMDIKModifierConfig::set_iteration_angle_limit);
		ClassDB::bind_method(D_METHOD("get_effector_bone"), &MMDIKModifierConfig::get_effector_bone);
		ClassDB::bind_method(D_METHOD("set_effector_bone", "p_bone"), &MMDIKModifierConfig::set_effector_bone);
		ClassDB::bind_method(D_METHOD("get_target_bone"), &MMDIKModifierConfig::get_target_bone);
		ClassDB::bind_method(D_METHOD("set_target_bone", "p_bone"), &MMDIKModifierConfig::set_target_bone);
		ClassDB::bind_method(D_METHOD("get_chain"), &MMDIKModifierConfig::get_chain);
		ClassDB::bind_method(D_METHOD("set_chain", "p_chain"), &MMDIKModifierConfig::set_chain);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "iterations"), "set_iterations", "get_iterations");
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "iteration_angle_limit"), "set_iteration_angle_limit", "get_iteration_angle_limit");
		ADD_PROPERTY(PropertyInfo(Variant::INT, "effector_bone"), "set_effector_bone", "get_effector_bone");
		ADD_PROPERTY(PropertyInfo(Variant::INT, "target_bone"), "set_target_bone", "get_target_bone");

		// Critical: Hint string tells Godot this is an Array of MMDIKChain objects
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "chain", PROPERTY_HINT_TYPE_STRING,
			vformat("%d/%d:MMDIKChain", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE)),
			"set_chain", "get_chain");
	}
};
class MMDIKModifier3D : public SkeletonModifier3D {
	GDCLASS(MMDIKModifier3D, SkeletonModifier3D);
	TypedArray<MMDIKModifierConfig> config;

	TypedArray<MMDIKModifierConfig> get_config() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
	void _process_modification_with_delta(double p_delta) override;
	void set_config(const TypedArray<MMDIKModifierConfig> &p_config);

	static void _bind_methods();
};

} //namespace godot
