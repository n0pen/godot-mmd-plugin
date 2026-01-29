@tool
extends EditorPlugin
var plug: EditorSceneImporterMMDPMX
func _enter_tree() -> void:
	plug = EditorSceneImporterMMDPMX.new()
	add_scene_format_importer_plugin(plug)

func _exit_tree() -> void:
	remove_scene_format_importer_plugin(plug)
