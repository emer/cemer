// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef taImageProc_h
#define taImageProc_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class float_Matrix; // 
class int_Matrix; // 
class taVector2f; //
class taVector2i; //


taTypeDef_Of(taImageProc);

class TA_API taImageProc : public taNBase {
  // #STEM_BASE ##CAT_Image basic image processing operations; images are either rgb = 3 dimensional matrix with 3rd dim = 3 or greyscale (2 dim matrix)
  INHERITED(taNBase)
public:

  enum EdgeMode {		// how to deal with image edges
    CLIP,			// just hard clip edges, nothing fancy
    BORDER,			// render/preserve a 1 pixel border around everything
    WRAP,			// wrap the image around to the other side: no edges!
  };

  static bool	GetBorderColor_float(float_Matrix& img_data, float& r, float& g, float& b, float& a);
  // #CAT_Render get the average color around a 1 pixel border region of the image -- if grey-scale image, r,g,b are all set to the single grey value
  static bool	GetBorderColor_float_rgb(float_Matrix& img_data, float& r, float& g, float& b, float& a);
    // #CAT_Render get the average color around a 1 pixel border region of the image -- specifically for rgb(a) image
    static bool	GetBorderColor_float_grey(float_Matrix& img_data, float& grey);
    // #CAT_Render get the average color around a 1 pixel border region of the image -- specifically for grey scale image

  static bool	RenderBorder_float(float_Matrix& img_data);
  // #CAT_Render make a uniform border 1 pixel wide around image, containing average value for that border region in original image: this value is what gets filled in when image is translated "off screen"

  static bool	FadeEdgesToBorder_float(float_Matrix& img_data, int fade_width = 8);
  // #CAT_Render fade edges of image uniformly to the border color, over fade_width pixels

  static bool	RenderOccluderBorderColor_float(float_Matrix& img_data,
						float llx, float lly, float urx, float ury);
  // #CAT_Render render an occluder rectangle of given normalized size (ll = lower left corner (0,0 = farthest ll), ur = upper right (1,1 = farthest ur) using the border color
  
  static bool	RenderFill(float_Matrix& img_data, float r, float g, float b, float a);
  // #CAT_Render render a "blank" image at a specified color

  static void	GetWeightedPixels_float(float coord, int size, int* pc, float* pw);
  // #IGNORE helper function: get pixel coordinates (pc[0], pc[1]) with norm weights (pw[0], [1]) for given floating coordinate coord

  static bool   TranslateImagePix_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
					int move_x, int move_y, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform move (translate) image by pixel move_x, move_y factors
  static bool   TranslateImage_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
				   float move_x, float move_y, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform move (translate) image by normalized move_x, move_y factors: 1 = center of image moves to right/top edge, -1 center moves to bottom/left
  static bool	RotateImage_float(float_Matrix& rotated_img, float_Matrix& orig_img,
				  float rotation, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform rotate the image: rotation = normalized 0-1 = 0-360 degrees 
  static bool	ScaleImage_float(float_Matrix& scaled_img, float_Matrix& orig_img, float scale,
				 EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform change the size of the image by normalized scaling factor (either rgb=3 dim or grey=2 dim)
  static bool   CropImage_float(float_Matrix& crop_img, float_Matrix& orig_img, 
				int crop_width, int crop_height, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform crop image to given size (-1 = use original image size), centered on the center of the image; border color of original image is used to fill in missing values

  static bool	TransformImage_float(float_Matrix& xformed_img, float_Matrix& orig_img,
				     float move_x=0.0f, float move_y=0.0f, float rotate=0.0f,
				     float scale=1.0f, int crop_width=-1, int crop_height=-1,
				     EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform Transform an image by translation, rotation, scaling, and cropping, as determined by parameters (calls above functions; only if needed; if crop < 0 then no cropping); does RenderBorder for each step to preserve uniform background color
  static bool	SampleImageWindow_float(float_Matrix& out_img, float_Matrix& in_img,
					int win_width=320, int win_height=320,
					float ctr_x=0.5f, float ctr_y=0.5f,
					float rotate=0.0f, float scale=1.0f,
					EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform Sample a window of given width and height centered on given normalized coordinate location in input image, with scaling and rotation (in normalized 0-1 units) as specified

  static bool	SampleImageWindow_float_wrap_mono
    (float_Matrix& out_img, float_Matrix& in_img, float_Matrix& sc_ary,
     taVector2i& win_size, taVector2i& img_size, taVector2f& win_ctr, taVector2f& img_ctr,
     int n_orig_pix, float rotate, float scale);
  // #IGNORE
  static bool	SampleImageWindow_float_wrap_rgb
    (float_Matrix& out_img, float_Matrix& in_img, float_Matrix& sc_ary,
     taVector2i& win_size, taVector2i& img_size, taVector2f& win_ctr, taVector2f& img_ctr,
     int n_orig_pix, float rotate, float scale);
  // #IGNORE
  static bool	SampleImageWindow_float_clip_mono
    (float_Matrix& out_img, float_Matrix& in_img, float_Matrix& sc_ary,
     taVector2i& win_size, taVector2i& img_size, taVector2f& win_ctr, taVector2f& img_ctr,
     int n_orig_pix, float rotate, float scale, EdgeMode edge);
  static bool	SampleImageWindow_float_clip_rgb
    (float_Matrix& out_img, float_Matrix& in_img, float_Matrix& sc_ary,
     taVector2i& win_size, taVector2i& img_size, taVector2f& win_ctr, taVector2f& img_ctr,
     int n_orig_pix, float rotate, float scale, EdgeMode edge);
  // #IGNORE
  
  static bool	AttentionFilter(float_Matrix& mat, float radius_pct);
  // #CAT_Filter #MENU_BUTTON #MENU_ON_Filter apply an "attentional" filter to the matrix data: outside of radius, values are attenuated in proportion of squared distance outside of radius (r_sq / dist_sq) -- radius_pct is normalized proportion of maximum half-size of image (e.g., 1 = attention bubble extends to furthest edge of image; only corners are attenuated)

  static bool	BlobBlurOcclude(float_Matrix& img, float pct_occlude,
				float circ_radius=0.05, float gauss_sig=0.05,
				EdgeMode edge=BORDER, bool use_border_clr=true);
  // #CAT_Noise #MENU_BUTTON #MENU_ON_Noise occlude the image (in place -- affects the img matrix itself) by blurring gaussian blobs into the image -- blobs are uniform circles with gaussian blur around the edges -- radius and gaussian sigma are in image width normalized units (e.g., .05 = 5% of the width of the image), pct_occlude is proportion of total image to occlude computed as a proportion of (1/gauss_sig)^2 as the total number of different blobs possible -- actual blob locations are drawn at random, so complete occlusion is not ensured even at 1, though there is an extra 2x factor at 1 to really try to occlude as completely as possible -- if use_border_clr, then the 1 pixel border around image provides the color for all the blobs -- otherwise color is weighted local average using same gaussian blobo kernel

  static bool	BubbleMask(float_Matrix& img, int n_bubbles, float bubble_sig=.05f,
			   float_Matrix* foreground=NULL, int_Matrix* bubble_coords=NULL);
  // #CAT_Noise #MENU_BUTTON #MENU_ON_Noise Simplified version of Gosselin & Schyn's bubble paradigm which creates a mask from Gaussians Bubbles through which information is let through. Conceptually just the inverse of BlobBlurOcclude, but parameterization allows titration on number of bubbles. n_bubbles is the number of bubbles to create in the mask, bubble_sig controls the width of the bubble in image width normalized units (e.g., .05 = 5% of the width of the image). foreground specifies a foreground to bubble through (default = border color). if bubble_coords is specified, saves the coordinates of the bubble centers for analysis in an Nx2 xy matrix 
  
  static bool	AdjustContrast(float_Matrix& img, float new_contrast, float bg_color=-1.0f);
  // #CAT_ImageProc #MENU_BUTTON #MENU_ON_ImageProc Adjust the contrast of the image (in place -- affects the img matrix itself) using new_contrast as a scalar. Holds background color constant at passed in value or if not specified, checks border. new_contrast is a scalar in range [0 1] and bg_color is an integer in range [0 1]
  
  static bool	CompositeImages(float_Matrix& img1, float_Matrix& img2);
  // #CAT_ImageProc #MENU_BUTTON #MENU_ON_ImageProc Combine img1 and img2 using img1's alpha channel. Operation is done in place on img1. Assumes img1 is RGBA format (img2 alpha channel unused) and images are same size.  img2 can be greyscale or rgb
  
  static bool	OverlayImages(float_Matrix& img1, float_Matrix& img2);
  // #CAT_ImageProc #MENU_BUTTON #MENU_ON_ImageProc overlay img2 onto img1. if img2 is smaller than img1, then overlay is done on the center of img1. both images should have the same number of dims (i.e., both grayscale or both rgb)

  static bool Blur(float_Matrix& img, int kernel_size);

  String 	GetTypeDecoKey() const override { return "DataTable"; }
  TA_BASEFUNS_NOCOPY(taImageProc);
private:
  void 	Initialize();
  void	Destroy();
};

#endif // taImageProc_h
