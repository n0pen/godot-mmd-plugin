#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "editor_scene_importer_mmd_pmx.h"
#include "editor_scene_importer_mmd_vmd.h"
#include "mmd_animator_modifier3d.h"
#include "mmd_ik_modfier3d.h"

using namespace godot;

void initialize_mmd_plugin_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(EditorSceneImporterMMDPMX);
		GDREGISTER_CLASS(EditorSceneImporterMMDVMD);
		GDREGISTER_CLASS(PMXMMDState);
		GDREGISTER_CLASS(VMDMMDState);
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(MMDAnimatorModifier3D);
		GDREGISTER_CLASS(MMDIKChain);
		GDREGISTER_CLASS(MMDIKModifierConfig);
		GDREGISTER_CLASS(MMDIKModifier3D);
	}
}

void uninitialize_mmd_plugin_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT mmd_plugin_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_mmd_plugin_module);
	init_obj.register_terminator(uninitialize_mmd_plugin_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

	return init_obj.init();
}
}