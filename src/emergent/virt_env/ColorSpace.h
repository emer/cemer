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

#ifndef ColorSpace_h
#define ColorSpace_h 1

// parent includes:
#include <taOBase>
#include "network_def.h"

#ifndef __MAKETA__
# include <cmath>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class float_Matrix;


eTypeDef_Of(ColorSpace);

class E_API ColorSpace : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image transform RGB colors into CIE XYZ and LMS perceptual space, including color opponents, and back -- note that we use 0-1 normalized XYZ and LMS values throughout - not the 0-100 as is sometimes standard it seems..
INHERITED(taOBase)
public:
  enum  OppComp {               // opponent coding components
    L_C,                        // Long wavelength = Red component
    M_C,                        // Medium wavelength = Green component
    S_C,                        // Short wavelength = Blue component
    LM_C,                       // Long + Medium wavelength = Yellow component
    LvM_C,                      // L - M opponent contrast
    SvLM_C,                     // S - L+M opponent contrast
    GREY,                       // achromatic response (grey scale lightness)
    N_OP_C,                     // number of opponent components
  };
  
  static inline float sRGBvalToLinear(const float srgb) {
    if(srgb <= 0.04045f) return srgb / 12.92f;
    return powf((srgb + 0.055f) / 1.055f, 2.4f);
  }
  // #CAT_ColorSpace convert an sRGB rgb component to linear -- used in converting from sRGB to XYZ colors

  static inline float sRGBvalFromLinear(const float lin) {
    if(lin <= 0.0031308f) return 12.92f * lin;
    return (1.055f * powf(lin, 1.0f / 2.4f) + 0.055f);
  }
  // #CAT_ColorSpace convert an sRGB rgb linear component to non-linear sRGB value -- used in converting from XYZ to sRGB 
 
  static inline void sRGBtoLinear(float& r_lin, float& g_lin, float& b_lin,
                                  const float r_s, const float g_s, const float b_s) {
    r_lin = sRGBvalToLinear(r_s); g_lin = sRGBvalToLinear(g_s); b_lin = sRGBvalToLinear(b_s);
  }
  // #CAT_ColorSpace convert set of sRGB components to linear values (gamma correction)

  static inline void sRGBfromLinear(float& r_s, float& g_s, float& b_s,
                                  const float r_lin, const float g_lin, const float b_lin) {
    r_s = sRGBvalFromLinear(r_lin); g_s = sRGBvalFromLinear(g_lin); b_s = sRGBvalFromLinear(b_lin);
  }
  // #CAT_ColorSpace convert set of sRGB components to linear values (gamma correction)

  static inline void sRGBlinToXYZ(float& X, float& Y, float& Z,
                                  const float r_lin, const float g_lin, const float b_lin) {
    X = 0.4124f * r_lin + 0.3576f * g_lin + 0.1805f * b_lin;
    Y = 0.2126f * r_lin + 0.7152f * g_lin + 0.0722f * b_lin;
    Z = 0.0193f * r_lin + 0.1192f * g_lin + 0.9505f * b_lin;
  }
  // #CAT_ColorSpace convert sRGB linear into XYZ CIE standard color space

  static inline void XYZtosRGBlin(float& r_lin, float& g_lin, float& b_lin,
                                  const float X, const float Y, const float Z) {
    r_lin = 3.2406f * X + -1.5372f * Y + -0.4986f * Z;
    g_lin = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
    b_lin = 0.0557f * X + -0.2040f * Y + 1.0570f * Z;
  }
  // #CAT_ColorSpace convert XYZ CIE standard color space to sRGB linear

  static inline void sRGBtoXYZ(float& X, float& Y, float& Z,
                               const float r_s, const float g_s, const float b_s) {
    float r_lin, g_lin, b_lin;
    sRGBtoLinear(r_lin, g_lin, b_lin, r_s, g_s, b_s);
    sRGBlinToXYZ(X,Y,Z, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert sRGB into XYZ CIE standard color space

  static inline void XYZtosRGB(float& r_s, float& g_s, float& b_s,
                               const float X, const float Y, const float Z) {
    float r_lin, g_lin, b_lin;
    XYZtosRGBlin(r_lin, g_lin, b_lin, X,Y,Z);
    sRGBfromLinear(r_s, g_s, b_s, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert XYZ CIE standard color space into sRGB

  static inline void XYZrenormD65(float& X, float& Y, float& Z) {
    X *= (1.0f / 0.95047f); Z *= (1.0f / 1.08883);
  }
  // #CAT_ColorSpace renormalize XZY values relative to the D65 outdoor white light values
  
  static inline void XYZtoLMS_CAT02(float& L, float& M, float& S,
                                    const float X, const float Y, const float Z) {
    L = 0.7328f * X + 0.4296f * Y + -0.1624f * Z;
    M = -0.7036f * X + 1.6975f * Y + 0.0061f * Z;
    S = 0.0030f * X + 0.0136f * Y + 0.9834 * Z;
  }
  // #CAT_ColorSpace convert XYZ to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02)

  static inline void sRGBlinToLMS_CAT02(float& L, float& M, float& S,
                                        const float r_lin, const float g_lin, const float b_lin) {
    L = 0.3904054f * r_lin + 0.54994122f * g_lin + 0.00892632f * b_lin;
    M = 0.0708416f * r_lin + 0.96317176f * g_lin + 0.00135775f * b_lin;
    S = 0.0491304f * r_lin + 0.21556128f * g_lin + 0.9450824f * b_lin;
  }
  // #CAT_ColorSpace convert sRGB linear to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) -- this is good for representing adaptation but NOT apparently good for representing appearances
  
  static inline void sRGBtoLMS_CAT02(float& L, float& M, float& S,
                               const float r_s, const float g_s, const float b_s) {
    float r_lin, g_lin, b_lin;
    sRGBtoLinear(r_lin, g_lin, b_lin, r_s, g_s, b_s);
    sRGBlinToLMS_CAT02(L,M,S, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert sRGB to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02)

  static inline void XYZtoLMS_HPE(float& L, float& M, float& S,
                                  const float X, const float Y, const float Z) {
    L = 0.38971f * X + 0.68898f * Y + -0.07868f * Z;
    M = -0.22981f * X + 1.18340f * Y + 0.04641f * Z;
    S = Z;
  }
  // #CAT_ColorSpace convert XYZ to Long, Medium, Short cone-based responses, using the Hunt-Pointer-Estevez transform -- this is closer to the actual response functions of the L,M,S cones apparently

  static inline void sRGBlinToLMS_HPE(float& L, float& M, float& S,
                                      const float r_lin, const float g_lin, const float b_lin) {
    L = 0.30567503f * r_lin + 0.62274014f * g_lin + 0.04530167f * b_lin;
    M = 0.15771291f * r_lin + 0.7697197f * g_lin + 0.08807348f * b_lin;
    S = 0.0193f * r_lin + 0.1192f * g_lin + 0.9505f * b_lin;
  }
  // #CAT_ColorSpace convert sRGB linear to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) -- this is good for representing adaptation but NOT apparently good for representing appearances
  
  static inline void sRGBtoLMS_HPE(float& L, float& M, float& S,
                               const float r_s, const float g_s, const float b_s) {
    float r_lin, g_lin, b_lin;
    sRGBtoLinear(r_lin, g_lin, b_lin, r_s, g_s, b_s);
    sRGBlinToLMS_HPE(L,M,S, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert sRGB to Long, Medium, Short cone-based responses, using the Hunt-Pointer-Estevez transform -- this is closer to the actual response functions of the L,M,S cones apparently

  static inline void LMStoXYZ_CAT02(float& X, float& Y, float& Z,
                                    const float L, const float M, const float S) {
    X = 1.096124f * L + 0.4296f * Y + -0.1624f * Z;
    Y = -0.7036f * X + 1.6975f * Y + 0.0061f * Z;
    Z = 0.0030f * X + 0.0136f * Y + 0.9834 * Z;
  }
  // #CAT_ColorSpace convert Long, Medium, Short cone-based responses to XYZ, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02)

  static inline void LMStoXYZ_HPE(float& X, float& Y, float& Z,
                                    const float L, const float M, const float S) {
    X = 1.096124f * L + 0.4296f * Y + -0.1624f * Z;
    Y = -0.7036f * X + 1.6975f * Y + 0.0061f * Z;
    Z = 0.0030f * X + 0.0136f * Y + 0.9834 * Z;
  }
  // #CAT_ColorSpace convert Long, Medium, Short cone-based responses to XYZ, using the Hunt-Pointer-Estevez transform -- this is closer to the actual response functions of the L,M,S cones apparently

  static inline float LuminanceAdaptation(const float bg_lum = 200.0f) {
    const float lum5 = 5.0f * bg_lum;
    float k = 1.0f / (lum5 + 1.0f);
    float k4 = k*k*k*k;
    float k4m1 = 1.0f - k4;
    float fl = .2f * k4 * lum5 + .1f * k4m1 * k4m1 * powf(lum5, 1.0f / 3.0f);
    return fl;
  }
  // #CAT_ColorSpace Luminance adaptation function -- equals 1 at background luminance of 200 so we generally ignore it..

  static inline float ResponseCompression(const float val) {
    float pval = powf(val, 0.42f);
    float rc = 0.1f + 4.0f * pval / (27.13f + pval);
    return rc;
  }
  // #CAT_ColorSpace takes a 0-1 normalized LMS value and performs hyperbolic response compression -- val must ALREADY have the luminance adaptation applied to it using the luminance adaptation function, which is 1 at a background luminance level of 200 = 2, so you can skip that step if you assume that level of background

  static inline void LMStoOpponents(float& L_c, float& M_c, float& S_c, float& LM_c,
                                    float& LvM, float& SvLM, float& grey,
                                    const float L, const float M, const float S) {
    float L_rc = ResponseCompression(L); float M_rc = ResponseCompression(M); float S_rc = ResponseCompression(S);
    const float LmM = L_rc - M_rc;  const float MmS = M_rc - S_rc;  const float SmL = S_rc - L_rc;
    // subtract min and mult by 6 gets values roughly into 1-0 range for L,M
    L_c = 6.0f * (((L_rc + (1.0f / 11.0f) * S_rc)) - 0.109091f); 
    M_c = 6.0f * (((12.0f / 11.0f) * M_rc) - 0.109091f); 
    LvM = L_c - M_c; // red-green subtracting "criterion for unique yellow"
    LM_c = 6.0f * (((1.0f / 9.0f) * (L_rc + M_rc)) - 0.0222222f);
    S_c = 6.0f * (((2.0f / 9.0f) * S_rc) - 0.0222222f);
    SvLM = S_c - LM_c;          // blue-yellow contrast
    grey = (1.0f / 0.431787f) * (2.0f * L_rc + M_rc + .05f * S_rc - 0.305f);
    // note: last term should be: 0.725 * (1/5)^-0.2 = grey background assumption (Yb/Yw = 1/5) = 1
  }
  // #CAT_ColorSpace convert Long, Medium, Short cone-based responses to opponent components: Red - Green (LvM) and Blue - Yellow (SvLM) -- includes the separate components in these subtractions as well -- uses the CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) https://en.wikipedia.org/wiki/CIECAM02
  
  static inline void sRGBtoOpponents(float& L_c, float& M_c, float& S_c, float& LM_c,
                                     float& LvM, float& SvLM, float& grey,
                                     const float r_s, const float g_s, const float b_s) {
    float L,M,S;
    sRGBtoLMS_HPE(L,M,S, r_s, g_s, b_s);
    LMStoOpponents(L_c, M_c, S_c, LM_c, LvM, SvLM, grey, L, M, S);
  }
  // #CAT_ColorSpace convert sRGB to opponent components via LMS using the HPE cone values: Red - Green (LvM) and Blue - Yellow (SvLM) -- includes the separate components in these subtractions as well -- uses the CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) https://en.wikipedia.org/wiki/CIECAM02

  
  /////////////////////////////////////////////////////////
  //    Image-level functions for above conversions
  
  static void sRGBtoGreyFastImg(float_Matrix& grey_img, const float_Matrix& srgb_img);
  // #CAT_ColorSpaceImg convert sRGB image to greyscale monochrome image using simple fast mean of r,g,b values

  static void sRGBtoXYZImg(float_Matrix& xyz_img, const float_Matrix& srgb_img);
  // #CAT_ColorSpaceImg convert sRGB image to CIE XYZ color space -- xyz_img is a 3D matrix with 2D planes of X,Y,Z values, srgb_img is standard 3D color image with R,G,B planes

  static void XYZtosRGBImg(float_Matrix& srgb_img, const float_Matrix& xyz_img);
  // #CAT_ColorSpaceImg convert CIE XYZ image to sRGB color space -- xyz_img is a 3D matrix with 2D planes of X,Y,Z values, srgb_img is standard 3D color image with R,G,B planes

  static void sRGBtoOpponentsImg(float_Matrix& opp_img, const float_Matrix& srgb_img);
  // #CAT_ColorSpaceImg convert sRGB image to opponent components image, which is 3D matrix of 2D image slices, outer dimension = 7 (OppComp enum, N_OP_C): L_c, M_c, S_c, LM_c, LvM, SvLM, grey -- done via LMS using the HPE cone values: Red - Green (LvM) and Blue - Yellow (SvLM) -- includes the separate components in these subtractions as well -- uses the CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) https://en.wikipedia.org/wiki/CIECAM02

  static void RenderColorChecker_sRGB(float_Matrix& img, int width = 640, int height = 480);
  // #CAT_ColorSpaceImg render a standard Macbeth ColorChecker pattern -- makes a solid black border around it too

  TA_SIMPLE_BASEFUNS(ColorSpace);
// protected:
//   void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};

#endif // ColorSpace_h
