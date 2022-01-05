# Usage

`./vengi-voxconvert --merge --scale --input infile --output outfile`

* `--crop`: reduces the volume sizes to their voxel boundaries.
* `--export-palette`: will save the included palette as png next to the source file. Use in combination with `--src-palette`.
* `--filter <filter>`: will filter out layers not mentioned in the expression. E.g. `1-2,4` will handle layer 1, 2 and 4. It is the same as `1,2,4`. The first layer is `0`. See the layers note below.
* `--force`: overwrite existing files
* `--input <file>`: allows to specify input files. You can specify more than one file
* `--merge`: will merge a multi layer volume (like `vox`, `qb` or `qbt`) into a single volume of the target file
* `--mirror <x|y|z>`: allows you to mirror the volumes at x, y and z axis
* `--output <file>`: allows you to specify the output filename
* `--pivot <x:y:z>`: change the pivots of the volume layers. Not all voxel formats support this.
* `--rotate <x|y|z>`: allows you to rotate the volumes by 90 degree at x, y and z axis
* `--scale`: perform lod conversion of the input volume (50% scale per call)
* `--script "<script> <args>"`: execute the given script - see [scripting support](../LUAScript.md) for more details
* `--split <x:y:z>`: slices the volumes into pieces of the given size
* `--src-palette`: will use the included [palette](../Palette.md) and doesn't perform any quantization to the default palette
* `--translate <x:y:z>`: translates the volumes by x (right), y (up), z (back)

Just type `vengi-voxconvert` to get a full list of commands and options.

Using a different target palette is also possible by setting the `palette` [cvar](../Configuration.md).

`./vengi-voxconvert -set palette /path/to/palette.png --input infile outfile`

The palette file has to be in the dimensions 1x256. It is also possible to just provide the basename of the palette.
This is e.g. `nippon`. The tool will then try to look up the file `palette-nippon.png` in the file search paths.

You can convert to a different palette with this command. The closest possible color will be chosen for each
color from the source file palette to the specified palette.

## The order of execution is:

* filter
* merge
* scale
* mirror
* rotate
* translate
* script
* pivot
* crop
* split

## Layers

Some formats also have layer support. Our layers are maybe not the layers you know from your favorite editor. Each layer can currently only have one object or volume in it. To get the proper layer ids (starting from 0) for your voxel file, you should load it once in [voxedit](../voxedit/Index.md) and check the layer panel.

Especially magicavoxel supports more objects in one layer. This might be confusing to get the right numbers for `voxconvert`. See [this issue](https://github.com/mgerhardy/engine/issues/68) for a few more details.
