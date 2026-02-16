#pragma once
#include "godot_cpp/classes/skeleton_modifier3d.hpp"

namespace godot {

class MMDAnimatorModifier3D : public SkeletonModifier3D {
	GDCLASS(MMDAnimatorModifier3D, SkeletonModifier3D)

	public:
	void _process_modification_with_delta(double p_delta) override;

	static void _bind_methods(){};
};
} //namespace godot