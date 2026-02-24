#pragma once
#include "godot_cpp/classes/skeleton_modifier3d.hpp"

#define MMD_IK_TARGET_DISTANCE 1e-6f
#define MMD_IK_TARGET_ANGLE 1e-6f

namespace godot {
class MMDIKChain : public RefCounted {
	GDCLASS(MMDIKChain, RefCounted);

public:
	int32_t bone_index = -1;
	bool is_limited = false;
	AABB limits = AABB();
	Vector3 rotation_axis = Vector3();
	bool is_axis_locked = false;
	EulerOrder rotation_order;
	static void _bind_methods(){};


};

class MMDIKModifierConfig : public RefCounted {
	GDCLASS(MMDIKModifierConfig, RefCounted);

public:
	int32_t iterations = 5;
	real_t iteration_angle_limit = 0.01;

	int32_t effector_bone = -1;
	int32_t target_bone = -1;

	TypedArray<MMDIKChain> chain = {};
	static void _bind_methods(){};
};
class MMDIKModifier3D : public SkeletonModifier3D {
	GDCLASS(MMDIKModifier3D, SkeletonModifier3D);
	TypedArray<MMDIKModifierConfig> config;

	TypedArray<MMDIKModifierConfig> get_config() const;
	void set_config(const TypedArray<MMDIKModifierConfig> &p_config);
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
	void _process_modification_with_delta(double p_delta) override;

	static void _bind_methods();
};

} //namespace godot
