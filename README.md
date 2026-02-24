# MMD asset importer Plugin for Godot

## What is this?

A Godot 4.6 Plugin for importing Miku Miku Dance Files into Godot. Based on [Godot PMX importer](https://github.com/seanlynch/godot_pmx_importer) module

The objective is to import MMD files to be able to render and work with MMD assets inside godot

### PMX:
- [x] Skeleton
- [x] Mesh
- [x] Morphs
  - Only vertex morphs are imported
- [x] Materials
  - Only basic textured materials are created
  - This will be improved in the future
- [ ] IK
  - Currently, IK is imported using Godot's CCDIK
  - A custom IK system is under development to better match MMD IK results
- [x] Bone Parent
  - Implemented using bone modifiers.
- [ ] Physics
  - For now, use bone modifiers. See below

### VMD (under development):
- Morphs and Motion data has been imported
- Interpolation has been implemented using a custom skeleton modifier

## How do I use it?

Build, then copy the Plugin `addons` folder into your godot `addons` folder,
Activate the plugin in project settings,
then import your MMD files

## Physics

To implement physics simulation you can add bone modifiers (like SpringBones)
This may require tuning

Eventually, a custom bone modifier system could be added to replicate MMD Rigid Body system


## Credits and thanks
- Sean Lynch and iFire
For working in the original PMX importer module

- Lyuma, Tokage and IFire from V-Sekai for their help


