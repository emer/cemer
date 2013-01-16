// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef taBrainAtlas_h
#define taBrainAtlas_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// smartptr, ref:
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <iColor>

// declare all other types mentioned but not required to include:
class int_Matrix; //
class String_Matrix; //
class float_Matrix; //
class taColor_List; //
class BrainAtlasInfo; // #IGNORE
class ColorScale; //
class BrainAtlas; // #IGNORE

TypeDef_Of(taBrainAtlas);

class EMERGENT_API taBrainAtlas : public taNBase {
  // a single brain atlas, with methods to access and manipulate the associated labels, colors, and voxels
  INHERITED(taNBase)
public:
  String  filepath;		// #FILE_DIALOG_LOAD #FILETYPE_BrainAtlas #EXT_xml path to the brain atlas .xml file
  String  description;		// #EDIT_DIALOG description of this atlas -- where did it come from, what areas does it contain?
  String  image_filepath;	// #FILE_DIALOG_LOAD #FILETYPE_BrainAtlas #EXT_nii path to the nifti brain image that contains indexes into the .xml brain atlas file
  
  virtual String	Label(int index);
  // #CAT_BrainAtlas get the label at a given index in the atlas
  virtual int    	Index(const String& label);
  // #CAT_BrainAtlas get the index for a given label in the atlas
  virtual iColor   	ColorForIndex(int index);
  // #CAT_BrainAtlas get the color at a given index
  virtual iColor   	ColorForLabel(const String& label);
  // #CAT_BrainAtlas get the color at a given label
  virtual void       	Colors(taColor_List& colors,
                                 const String& labels_regexp=".*");
  // #CAT_BrainAtlas fill in the colors for the labels that match the given regexp (default is all)
  virtual void		Labels(String_Matrix& labels, const String& labels_regexp=".*");
  // #CAT_BrainAtlas fill in the labels that match the given regexp (default is all)
  virtual void		Indexes(int_Matrix& indexes, const String& labels_regexp=".*");
  // #CAT_BrainAtlas fill in the indexes that match the given regexp (default is all)
  virtual void		VoxelCoordinates(float_Matrix& voxels, const String& label_regexp);
  // #CAT_BrainAtlas fill in the voxels for all the areas that match the given regexp (default is all) -- voxels are a 2d matrix with inner dimension = 3d coord and outer dim is number of voxels

  virtual void	      SetColorValue(const String& labels_regexp, float val,
				    ColorScale* color_scale);
  // #CAT_BrainAtlas set color of all areas matching regexp to color given by numerical value as given by provided color scale (which must be initialized to include value range and color spec)
  virtual void	      SetColorString(const String& labels_regexp, const String& color);
  // #CAT_BrainAtlas set color of all areas matching regexp to given color, specified as a string (#rrggbb or a common color name)
  virtual void	      SetColor(const String& labels_regexp, iColor color);
  // #CAT_BrainAtlas set color of all areas matching regexp to given color, specified with an internal color object

  virtual void        EditAtlas();
  // #BUTTON #NO_BUSY #CAT_BrainAtlas edit the atlas contents, including the colors associated with regions in this atlas
  virtual void	      SaveAtlas();
  // #BUTTON #CONFIRM #CAT_BrainAtlas save the .xml file to the same file that it was loaded from, overwriting it with current data (e.g., new colors)
  virtual void	      SaveAtlasAs(const String& file_name);
  // #BUTTON #FILE_DIALOG_SAVE #FILETYPE_BrainAtlas #EXT_xml #CAT_BrainAtlas save the .xml file to the given file name -- note it is typically a good idea to set a different name in the name field before doing this, so the atlas can be recognized as unique in a list
  virtual void	      RevertAtlas();
  // #BUTTON #CONFIRM #CAT_BrainAtlas revert the atlas data to the .xml file on disk -- this will overwrite any current changes to the atlas data!
  virtual void	      LoadAtlas(const String& file_name);
  // #BUTTON #FILE_DIALOG_LOAD #FILETYPE_BrainAtlas #EXT_xml #CAT_BrainAtlas load atlas data from the given .xml file, overwriting any current atlas data

#ifndef __MAKETA__
  BrainAtlas&  Atlas();
#endif
  
  explicit taBrainAtlas(const BrainAtlasInfo& info);
  TA_BASEFUNS_SC(taBrainAtlas);
protected:
  bool  m_have_atlas_instance;
  BrainAtlas*  m_atlas;

private:
  void  Initialize();
  void  Destroy();
};

TA_SMART_PTRS(taBrainAtlas);

#endif // taBrainAtlas_h
