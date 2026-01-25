# MMD asset importer Plugin for Godot

## What is it?

A Godot 4 Plugin under development for importing Miku Miku Dance Files into Godot.

The objective is to import MMD files to be able to render and work with MMD assets inside godot

### PMX:
- [x] Skeleton
- [x] Mesh
- [ ] Morphs
  - Only vertex morphs are imported at the moment
- [x] Materials
  - For the moment, only basic textured materials are created
- [ ] IK
  - This is Under development.
- [ ] Bone Parent
  - This is Under development.
- [ ] Physics
  - For now, use bone modifiers. See below

### VMD (under development):
- Morphs and Motion data has been imported
  - Interpolation is under development

## How do I use it?

Build, then copy the Plugin `addons` folder into your godot `addons` folder, then import your MMD files

## Physics

To implement physics simulation you can add bone modifiers (like SpringBones)
This may require tuning

Eventually, a custom bone modifier system could be added to replicate MMD Rigid Body system


## Credits and thanks
- Sean Lynch and iFire
For working in the original PMX importer module

- Lyuma, Tokage and IFire from the V-Sekai discord for their help


