@tool
extends EditorPlugin
var pmx: EditorSceneImporterMMDPMX
var vmd: EditorSceneImporterMMDVMD
func _enter_tree() -> void:
	pmx = EditorSceneImporterMMDPMX.new()
	add_scene_format_importer_plugin(pmx)
	vmd = EditorSceneImporterMMDVMD.new()
	add_scene_format_importer_plugin(vmd)

func _exit_tree() -> void:
	remove_scene_format_importer_plugin(pmx)
	remove_scene_format_importer_plugin(vmd)
