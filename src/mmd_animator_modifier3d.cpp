#include "mmd_animator_modifier3d.h"
#include "godot_cpp/classes/skeleton3d.hpp"

#define MMD_ROT1 "mmd_rot_1"
#define MMD_ROT2 "mmd_rot_2"
#define MMD_PHASE "mmd_rot_phase"
#define MMD_BEZIER "mmd_rot_bezier"

void godot::MMDAnimatorModifier3D::_process_modification_with_delta(double p_delta) {
	const auto skeleton = get_skeleton();
	for (int i = 0; i < skeleton->get_bone_count(); ++i) {
		if (
				skeleton->has_bone_meta(i, MMD_ROT1) &&
				skeleton->has_bone_meta(i, MMD_ROT2) &&
				skeleton->has_bone_meta(i, MMD_PHASE) &&
				skeleton->has_bone_meta(i, MMD_BEZIER)) {
			auto rot1 = skeleton->get_bone_meta(i, MMD_ROT1);
			auto rot2 = skeleton->get_bone_meta(i, MMD_ROT2);
			auto phase = skeleton->get_bone_meta(i, MMD_PHASE);
			auto bezier = skeleton->get_bone_meta(i, MMD_BEZIER);

			if (rot1.get_type() == Variant::QUATERNION &&
					rot2.get_type() == Variant::QUATERNION &&
					phase.get_type() == Variant::FLOAT &&
					bezier.get_type() == Variant::FLOAT) {
			}
			skeleton->set_bone_pose_rotation(i,
					Quaternion(rot1).slerp(Quaternion(rot2), Math::abs(float(phase) - float(bezier))));
		}
	}
}
