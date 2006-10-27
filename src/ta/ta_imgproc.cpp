// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_imgproc.h"

#include <QImage>

///////////////////////////////////////////
//	DoG Filter

void DoGFilterSpec::Initialize() {
  filter_width = 8;
  filter_size = filter_width * 2 + 1;
  ctr_sigma = 2.0f;
  sur_sigma = 4.0f;
  circle_edge = true;
}

void DoGFilterSpec::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  filter_size = filter_width * 2 + 1;
  UpdateFilter();
}

float DoGFilterSpec::Eval(float x, float y) {
  float val = x * x + y * y;
  float dist = sqrt(val);
  float ong = 0.0f;
  float offg = 0.0f;
  if(!circle_edge || (dist <= filter_width)) { // only set values inside of filter radius
    ong = 1.0f / (4.0 * taMath_float::pi * ctr_sigma * ctr_sigma) * exp(-val / (2.0f * ctr_sigma * ctr_sigma));
    offg = 1.0f / (4.0 * taMath_float::pi * sur_sigma * sur_sigma) * exp(-val / (2.0f * sur_sigma * sur_sigma));
  }
  float net = ong - offg;
  return net;
}

void DoGFilterSpec::RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt,
				 float_Matrix& net_flt) {
  on_flt.SetGeom(2, filter_size, filter_size);
  off_flt.SetGeom(2, filter_size, filter_size);
  net_flt.SetGeom(2, filter_size, filter_size);
  int x,y;
  for(y=-filter_width; y<=filter_width; y++) {
    for(x=-filter_width; x<=filter_width; x++) {
      float val = Eval(x, y);
      if(val > 0.0f) {
	on_flt.Set(val, y+filter_width, x+filter_width);
	off_flt.Set(0.0, y+filter_width, x+filter_width);
      }
      else {
	off_flt.Set(-val, y+filter_width, x+filter_width);
	on_flt.Set(0.0, y+filter_width, x+filter_width);
      }
    }
  }

  taMath_float::vec_norm_sum(&on_flt);
  taMath_float::vec_norm_sum(&off_flt);
  for(int i=0;i<on_flt.size;i++) {
    float on_val = on_flt.FastEl_Flat(i);
    float off_val = off_flt.FastEl_Flat(i);
    if(on_val > 0.0f)
      net_flt.FastEl_Flat(i) = on_val;
    else
      net_flt.FastEl_Flat(i) = off_val;
  }
  //  cerr << "+ sum: " << pos_sum_2 << " - sum: " << neg_sum_2 << endl;
}

void DoGFilterSpec::UpdateFilter() {
  RenderFilter(on_filter, off_filter, net_filter);
}

// todo: should just be a wizard call on filter..
//   virtual void	GraphFilter(GraphLog* disp_log); // #BUTTON #NULL_OK plot the filter gaussian
//   virtual void	GridFilter(GridLog* disp_log); // #BUTTON #NULL_OK plot the filter gaussian


//////////////////////////////////////////////////////////
//  	Gabor Filters!

void GaborFilterSpec::Initialize() {
  x_size = 2;
  y_size = 2;
  ctr_x = .5f * x_size;
  ctr_y = .5f * y_size;
  angle = 0;
  phase = 0;
  freq = 1;
  length = 8;
  width = 4;
  amp = 1;
}

float GaborFilterSpec::Eval(float x, float y) {
  // normalize into normal coords, where sin goes along x axis 
  float cx = x - ctr_x;
  float cy = y - ctr_y;

  float r = sqrtf(cx*cx + cy*cy);
  float thet = atan2(cy, cx);
  float totang = thet - angle;
  float n_x = r * cos(totang);
  float n_y = r * sin(totang);

  float rval = amp * cos(phase + freq * n_y) * 
    exp(-.5f * ((n_y * n_y) / (width * width) + (n_x * n_x) / (length * length)));

  return rval;
}

void GaborFilterSpec::RenderFilter(float_Matrix& flt) {
  flt.SetGeom(2, x_size * y_size);
  for(int y=0;y<y_size;y++) {
    for(int x=0;x<x_size;x++) {
      flt.Set(Eval(x, y), x, y);
    }
  }
}

void GaborFilterSpec::UpdateFilter() {
  RenderFilter(filter);
}

float GaborFilterSpec::GetParam(GaborParam param) {
  switch(param) {
  case CTR_X: 	return ctr_x;
  case CTR_Y: 	return ctr_y;
  case ANGLE:	return angle;
  case PHASE:	return phase;
  case FREQ:	return freq;
  case LENGTH:	return length;
  case WIDTH:	return width;
  case AMP:	return amp;
  }	
  return 0.0f;
}

// todo: should be wizard fun call
// void GaborFilterSpec::GraphFilter(GraphLog* graph_log) {
// }
// void GaborFilterSpec::GridFilter(GridLog* disp_log) {
// }

void GaborFilterSpec::OutputParams(ostream& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << angle
       << ", phase: " << phase << ", freq: " << freq
       << ", length: " << length << ", width: " << width
       << ", amp: " << amp
       << endl;
}


//////////////////////////////////////////////////////////
//  	Gabor Fitter!


void GaborFitter::Initialize() {
  fit_dist = 0.0f;
}

static inline float gfs_sqdist(const float f1, const float f2) {
  float d = f1-f2;
  return d * d;
}

float GaborFitter::ParamDist(const GaborFilterSpec& oth) {
  return sqrtf(gfs_sqdist(ctr_x, oth.ctr_x) + gfs_sqdist(ctr_y, oth.ctr_y)
	       + gfs_sqdist(angle, oth.angle) + gfs_sqdist(phase, oth.phase) 
	       + gfs_sqdist(freq, oth.freq)  + gfs_sqdist(length, oth.length) 
	       + gfs_sqdist(width, oth.width) + gfs_sqdist(amp, oth.amp));
}

// float GaborFitter::SquaredDist(float_Matrix& data_vals) {
//   float rval = 0.0f;
//   int x,y;
//   int ctr = 0;
//   for(y=0;y<y_size;y++) {
//     for(x=0;x<x_size;x++, ctr++) {
//       float d = Eval(x, y) - data_vals[ctr];
//       rval += d * d;
//     }
//   }
//   return rval;
// }

// float GaborFitter::FitData_firstpass(float_Matrix& data_vals) {
//   float ctr_brd = 1.0;
//   float	ctr_inc = (x_size - 2.0f * ctr_brd) / 3.0f;
//   float ang_inc = PI / 3.0f;
//   float phs_inc = PI;
//   float	frq_min = PI / x_size;
//   float frq_max = PI;
//   float frq_inc = (frq_max - frq_min) / 2.0f;
//   float sz_min = 1.0f;
//   float sz_max = .5f;
//   float sz_inc = ((x_size * sz_max) - sz_min) / 2.0f;

// //   float tot_comp = 3.0f * 3.0f * 4.0f * 2.0f * 2.0f * 2.0f * 2.0f;
// //   cerr << "total cycles: " << tot_comp << endl;

//   float min_d = FLT_MAX;
//   GaborFitter min_params;
//   String my_nm = name;

//   for(ctr_x = ctr_brd; ctr_x <= x_size - ctr_brd; ctr_x += ctr_inc) {
//     for(ctr_y = ctr_brd; ctr_y <= y_size - ctr_brd; ctr_y += ctr_inc) {
//       for(angle = 0.0f; angle <= PI; angle += ang_inc) {
// 	for(phase = 0.0f; phase <= 2.0f * PI; phase += phs_inc) {
// 	  for(freq = frq_min; freq <= frq_max; freq += frq_inc) {
// 	    for(length = sz_min; length <= y_size * sz_max; length += sz_inc) {
// 	      for(width = sz_min; width <= x_size * sz_max; width += sz_inc) {
// 		float dist = SquaredDist(data_vals);
// 		if(dist < min_d) {
// 		  min_d = dist;
// 		  min_params = *this;
// 		}
// 	      }
// 	    }
// 	  }
// 	}
//       }
//     }
//   }
//   *this = min_params;
//   name = my_nm;
//   return min_d;
// }

// float GaborFitter::FitData(float_Matrix& data_vals, bool use_cur_vals) {
//   float min_d = FLT_MAX;
//   if(!use_cur_vals) {
//     min_d = FitData_firstpass(data_vals);
// //     cerr << "After 1st Pass\t min_d: " << min_d << "\t";
// //     OutputParams();
//   }

//   int n_itr = 3;
//   int itr;
//   for(itr = 0; itr<n_itr; itr++) {
//     float rng_div = (2.0f * float(itr) + 1.0f);
//     float inc_div = 6.0f;

// //     float amp_rng = .4 / rng_div;
// //     float amp_inc = amp_rng / inc_div;
//     float ctr_rng = 8.0 / rng_div;
//     float ctr_inc = ctr_rng / inc_div;
//     float ang_rng = (PI / 2.0) / rng_div;
//     float ang_inc = ang_rng / inc_div;
//     float phs_rng = PI / rng_div;
//     float phs_inc = phs_rng / inc_div;
//     float sub_ctr_rng = 3.0 / rng_div;
//     float sub_ctr_inc = ctr_rng / 3.0f;
//     float frq_rng = (PI / 2.0) / rng_div;
//     float frq_inc = frq_rng / inc_div;
//     float sz_rng = 4.0 / rng_div;
//     float sz_inc = sz_rng / inc_div;
//     float sub_wd_rng = 2.0 / rng_div;
//     float sub_wd_inc = sz_rng / 4.0;

//     amp = 1.0f;
// //     float cur_amp = amp;
// //     float min_amp = amp;
// //     for(amp = cur_amp - amp_rng; amp <= cur_amp + amp_rng; amp += amp_inc) {
// //       if(amp < .75) continue;
// //       float dist = SquaredDist(data_vals);
// //       if(dist < min_d) {
// // 	min_d = dist;
// // 	min_amp = amp;
// //       }
// //     }
// //     amp = min_amp;

//     float cur_ctr_x = ctr_x;  float cur_ctr_y = ctr_y;
//     float min_ctr_x = ctr_x;  float min_ctr_y = ctr_y;
//     for(ctr_x = cur_ctr_x-ctr_rng; ctr_x <= cur_ctr_x + ctr_rng; ctr_x += ctr_inc) {
//       if((ctr_x < 1) || (ctr_x >= x_size-1)) continue;
//       for(ctr_y = cur_ctr_y-ctr_rng; ctr_y <= cur_ctr_y + ctr_rng; ctr_y += ctr_inc) {
// 	if((ctr_y < 1) || (ctr_y >= y_size-1)) continue;
// 	float dist = SquaredDist(data_vals);
// 	if(dist < min_d) {
// 	  min_d = dist;
// 	  min_ctr_x = ctr_x; min_ctr_y = ctr_y;
// 	}
//       }
//     }
//     ctr_x = min_ctr_x; ctr_y = min_ctr_y;

//     float cur_ang = angle;
//     float min_ang = angle;
//     for(angle = cur_ang - ang_rng; angle <= cur_ang + ang_rng; angle += ang_inc) {
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_ang = angle;
//       }
//     }
//     angle = min_ang;
//     if(angle < 0.0f) angle += PI;
//     if(angle > PI) angle -= PI;

//     float min_frq = freq;
//     float cur_frq = freq;
//     for(freq = cur_frq - frq_rng; freq <= cur_frq + frq_rng; freq += frq_inc) {
//       if(freq < .1) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_frq = freq;
//       }
//     }
//     freq = min_frq;

//     float cur_wid = width;
//     float min_wid = width;
//     for(width = cur_wid - sz_rng; width <= cur_wid + sz_rng; width += sz_inc) {
//       if(width < .75) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_wid = width;
//       }
//     }
//     width = min_wid;

//     float cur_phs = phase; cur_ctr_x = ctr_x;  cur_ctr_y = ctr_y; cur_wid = width;
//     float min_phs = phase; min_ctr_x = ctr_x;  min_ctr_y = ctr_y; min_wid = width;
//     for(phase = cur_phs - phs_rng; phase <= cur_phs + phs_rng; phase += phs_inc) {
//       for(ctr_x = cur_ctr_x-sub_ctr_rng; ctr_x <= cur_ctr_x + sub_ctr_rng; ctr_x += sub_ctr_inc) {
// 	if((ctr_x < 1) || (ctr_x >= x_size-1)) continue;
// 	for(ctr_y = cur_ctr_y-sub_ctr_rng; ctr_y <= cur_ctr_y + sub_ctr_rng; ctr_y += sub_ctr_inc) {
// 	  if((ctr_y < 1) || (ctr_y >= y_size-1)) continue;
// 	  for(width = cur_wid - sub_wd_rng; width <= cur_wid + sub_wd_rng; width += sub_wd_inc) {
// 	    if(width < .75) continue;
// 	    float dist = SquaredDist(data_vals);
// 	    if(dist < min_d) {
// 	      min_d = dist;
// 	      min_phs = phase; min_ctr_x = ctr_x; min_ctr_y = ctr_y; min_wid = width;
// 	    }
// 	  }
// 	}
//       }
//     }
//     phase = min_phs;
//     if(phase < 0.0f) phase += 2.0f * PI;
//     if(phase > 2.0f * PI) phase -= 2.0f * PI;
//     ctr_x = min_ctr_x; ctr_y = min_ctr_y;
//     width = min_wid;

//     float cur_len = length;
//     float min_len = length;
//     for(length = cur_len - sz_rng; length <= cur_len + sz_rng; length += sz_inc) {
//       if(length < .75) continue;
//       float dist = SquaredDist(data_vals);
//       if(dist < min_d) {
// 	min_d = dist;
// 	min_len = length;
//       }
//     }
//     length = min_len; 

// //     cerr << "Itr: " << itr << "\t min_d: " << min_d << "\t";
// //     OutputParams();
//   }

// //   cerr << "Final fit\t min_d: " << min_d << "\t";
// //   OutputParams();
//   fit_dist = min_d;
//   return min_d;
// }

// float GaborFitter::TestFit() {
//   ctr_x = 2 + Random::IntZeroN(x_size - 4);
//   ctr_y = 2 + Random::IntZeroN(y_size - 4);
//   angle = PI * Random::ZeroOne();
//   phase = 2.0f * PI * Random::ZeroOne();
//   float	frq_min = PI / x_size;
//   float frq_max = PI;
//   float frq_inc = (frq_max - frq_min);
//   freq = frq_min + frq_inc * Random::ZeroOne();
//   length = .75 + ((.25 * y_size) - .5) * Random::ZeroOne();
//   width = .75 + ((.25 * x_size) - .5) * Random::ZeroOne();
//   amp = 1.0;
//   cerr << "\nBefore:\t";
//   OutputParams();
//   GridFilter(NULL);
//   taivMisc::RunIVPending();
//   float_Matrix data;
//   RenderFilter(data);
//   float min_d = FitData(data, false);
//   cerr << "Min Dist: " << min_d << endl;
//   cerr << "After:\t";
//   OutputParams();
//   GridFilter(NULL);
//   return min_d;
// }

///////////////////////////////////////////////////////////
// 		Retinal Spacing


void RetinalSpacingSpec::Initialize() {
  retina_size.x = 321;
  retina_size.y = 241;
}

void RetinalSpacingSpec::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  spacing.x = MAX(spacing.x, 1);
  spacing.y = MAX(spacing.y, 1);
  UpdateSizes();
}

void RetinalSpacingSpec::UpdateSizes() {
  // 0 1 2 3 4 5 6 7 8 	retina_size = 9
  // b b .   .   . b b 	border = 2, spacing = 2: input_size = 5, output_size = 3
  //     sc        ec

  input_size = (retina_size - 2 * border);
  output_size = ((input_size - 1) / spacing) + 1;
  output_units = output_size.x * output_size.y;

  if((input_size.x - 1) % spacing.x != 0) {
    taMisc::Warning("Filter:",name,"x spacing:",String(spacing.x),
		  "is not even multiple of input size:",String(input_size.x));
  }
  if((input_size.y - 1) % spacing.y != 0) {
    taMisc::Warning("Filter:",name,"y spacing:",String(spacing.y),
		  "is not even multiple of input size:",String(input_size.y));
  }
}

///////////////////////////////////////////////////////////
// 		taImageProc

void taImageProc::Initialize() {
}


bool taImageProc::ReadImageGrey_float(float_Matrix& img_data, const String& fname) {
  QImage img((const char*)fname);
  if(img.isNull()) {
    taMisc::Error("ReadImageGrey_float: could not read image file:", fname);
    return false;
  }
  img_data.SetGeom(2, img.width(), img.height());

  for(int y=0; y<img.height(); y++) {
    for(int x=0; x< img.width(); x++) {
      QRgb pix = img.pixel(x, y);
      float gval = qGray(pix) / 255.0f;
      img_data.Set(gval, x, y);
    }
  }
  return true;
}

bool taImageProc::ReadImageRGB_float(float_Matrix& r_img, float_Matrix& g_img,
				     float_Matrix& b_img, const String& fname) {
  QImage img((const char*)fname);
  if(img.isNull()) {
    taMisc::Error("ReadImageRGB_float: could not read image file:", fname);
    return false;
  }
  r_img.SetGeom(2, img.width(), img.height());
  g_img.SetGeom(2, img.width(), img.height());
  b_img.SetGeom(2, img.width(), img.height());

  for(int y=0; y<img.height(); y++) {
    for(int x=0; x< img.width(); x++) {
      QRgb pix = img.pixel(x, y);
      float rval = qRed(pix) / 255.0f;
      float gval = qGreen(pix) / 255.0f;
      float bval = qBlue(pix) / 255.0f;
      r_img.Set(rval, x, y);
      g_img.Set(gval, x, y);
      b_img.Set(bval, x, y);
    }
  }
  return true;
}

// todo: here!

/*

bool taImageProc::RenderBorder(float_Matrix& img_data) {
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));
  int tidx = (img_size.y-1) * img_size.x;
  int tidxm1 = (img_size.y-2) * img_size.x;
  int bidxp1 = img_size.x;
  float tavg = 0.0f;
  float bavg = 0.0f;
  int x;
  for(x=1;x<img_size.x-1;x++) {
    tavg += img_data[tidxm1 + x];
    bavg += img_data[bidxp1 + x];
  }
  tavg /= (float)(img_size.x-2);
  bavg /= (float)(img_size.x-2);

  float lavg = 0.0f;
  float ravg = 0.0f;
  int y;
  for(y=1;y<img_size.y-1;y++) {
    ravg += img_data[y * img_size.x + img_size.x-2];
    lavg += img_data[y * img_size.x + 1];
  }
  lavg /= (float)(img_size.y-2);
  ravg /= (float)(img_size.y-2);

  for(x=0;x<img_size.x;x++) {
    img_data[tidx + x] = tavg;
    img_data[x] = bavg;
  }
  for(y=1;y<img_size.y-1;y++) {
    img_data[y * img_size.x + img_size.x-1] = ravg;
    img_data[y * img_size.x] = lavg;
  }
//   cerr << "border avgs: t: " << tavg << ", b: " << bavg
//        << ", l: " << lavg << ", r: " << ravg << endl;

}

bool taImageProc::ScaleImageData(float_Matrix& scaled_img, TwoDCoord& scaled_size, float_Matrix& orig_img, TwoDCoord& img_size, float scale) {
  if(scale < .01f) {
    taMisc::Error("Can't scale below .01.");
    return;
  }
  if(scale > 100.0f) {
    taMisc::Error("Can't scale above 100.");
    return;
  }
  scaled_size.x = 2 + (int)(scale * (img_size.x-2)); // keep border in there
  scaled_size.y = 2 + (int)(scale * (img_size.y-2));
  scaled_img.EnforceSize(scaled_size.x * scaled_size.y);
  float half_sc;
  if(scale < 1.0f)
    half_sc = .5f / scale; // how much on each side of the pixel to include in avg
  else
    half_sc = .5f * scale;

  // scale > 1
  //   o o o    3
  //  /|X|X|\   connections
  // s s s s s  5

  int	half_int = (int)floor(half_sc);
  float extra = half_sc - (float)half_int;
  int n_orig_pix = half_int + 1; // number of pixels to get from original image for each scaled pixel
  //    int tot_org_pix = 2 * n_orig_pix + 1;
  float_Matrix sc_ary;
  int x, y;
  for(y=-n_orig_pix; y<= n_orig_pix; y++) {
    float ysc = extra;
    if((y >= -half_int) && (y <= half_int))
      ysc = 1.0f;
    for(x=-n_orig_pix; x<= n_orig_pix; x++) {
      float xsc = extra;
      if((x >= -half_int) && (x <= half_int))
	xsc = 1.0f;
      sc_ary.Add(ysc * xsc);
    }
  }
  sc_ary.NormSum(1.0f);		// make it add to 1
  for(y=1;y<scaled_size.y-1;y++) {
    int oyc = (int)floor(.5f + ((float)y / scale));
    for(x=1;x<scaled_size.x-1;x++) {
      int oxc = (int)floor(.5f + ((float)x / scale));
      float avg = 0.0f;
      int sc_ctr = 0;
      int oxi, oyi;
      for(oyi=-n_orig_pix;oyi<=n_orig_pix;oyi++) {
	int oy = oyc + oyi;
	if(oy < 0) oy = 0; if(oy >= img_size.y) oy = img_size.y-1;
	for(oxi=-n_orig_pix;oxi<=n_orig_pix;oxi++, sc_ctr++) {
	  int ox = oxc + oxi;
	  if(ox < 0) ox = 0; if(ox >= img_size.x) ox = img_size.x-1;
	  avg += sc_ary[sc_ctr] * orig_img[oy * img_size.x + ox];
	}
      }
      scaled_img[y * scaled_size.x + x] = avg;
    }
  }

  // copy over border values:
  {
    float tval = orig_img[(img_size.y-1) * img_size.x + 1];
    float bval = orig_img[1];
    float lval = orig_img[img_size.x];
    float rval = orig_img[2 * img_size.x - 1];
    float avg_val = .25 * (tval + bval + lval + rval); // use avg_val instead of sep vals
    // because sep vals leave streaks at the corners when filtered!
    int tidx = (scaled_size.y-1) * scaled_size.x;
    int x, y;
    for(x=0;x<scaled_size.x;x++) {
      scaled_img[tidx + x] = avg_val; // tval;
      scaled_img[x] = avg_val; // bval;
    }
    for(y=1;y<scaled_size.y-1;y++) {
      scaled_img[y * scaled_size.x + scaled_size.x-1] = avg_val; //rval;
      scaled_img[y * scaled_size.x] = avg_val; // lval;
    }
  }
}

// get pixel coordinates (pc1, 2) with norm weights (pw1, 2) for given floating coordinate coord
bool taImageProc::GetWeightedPixels(float coord, int size, int* pc, float* pw) {
  //   |  .|   |  get from two closest pixels..
  pc[0] = (int)floor(coord);
  float xfrac = coord - floor(coord);
  if(xfrac < .5f) {
    pw[0] = xfrac;
    pc[1] = pc[0]-1; pw[1] = 1.0f - xfrac;
  }
  else if(xfrac > .5f) {
    pw[0] = 1.0f - xfrac;
    pc[1] = pc[0]+1; pw[1] = xfrac;
  }
  else {
    pw[0] = pw[1] = .5f;
    pc[1] = pc[0];
  }
  if(pc[0] < 0) pc[0] = 0; if(pc[0] >= size) pc[0] = size-1;      
  if(pc[1] < 0) pc[1] = 0; if(pc[1] >= size) pc[1] = size-1;      
}


bool taImageProc::RotateImageData(float_Matrix& rotated_img, float_Matrix& orig_img, TwoDCoord& img_size, float rotate) {
  rotate *= 2.0f * PI;
  rotated_img.EnforceSize(orig_img.size);

  FloatTwoDCoord ctr = ((FloatTwoDCoord) img_size) / 2.0f;  // Changed to floating point

  float_Matrix sc_ary;  sc_ary.EnforceSize(4);

  int x,y;			// coords in new image
  for(y=1;y<img_size.y-1;y++) {
    for(x=1;x<img_size.x-1;x++) {
      float cx = ((float) x) - ctr.x;  // changed to float
      float cy = ((float) y) - ctr.y;  // changed to float

      float rot_sin = sin(rotate);
      float rot_cos = cos(rotate);
      float org_x = cx * rot_cos + cy * rot_sin + ctr.x;
      float org_y = cy * rot_cos - cx * rot_sin + ctr.y;

      //      float r = sqrtf(cx*cx + cy*cy);
      //float thet = atan2(cy, cx);
      //float totang = thet - rotate;
      //float org_x = (r * cos(totang)) + ctr.x;  // removed float cast
      //float org_y = (r * sin(totang)) + ctr.y;  // removed float cast

      int pcx[2];
      int pcy[2];
      float pwx[2];
      float pwy[2];

      GetWeightedPixels(org_x, img_size.x, pcx, pwx);
      GetWeightedPixels(org_y, img_size.y, pcy, pwy);

      int sc_ctr = 0;
      int oxi, oyi;
      for(oyi=0;oyi<2;oyi++) {
	for(oxi=0;oxi<2;oxi++, sc_ctr++) {
	  sc_ary[sc_ctr] = pwx[oxi] + pwy[oyi];
	}
      }
      sc_ary.NormSum(1.0f);

      float avg = 0.0f;
      sc_ctr = 0;
      for(oyi=0;oyi<2;oyi++) {
	for(oxi=0;oxi<2;oxi++, sc_ctr++) {
	  int oy = pcy[oyi];
	  int ox = pcx[oxi];
	  avg += sc_ary[sc_ctr] * orig_img[oy * img_size.x + ox];
	}
      }
      rotated_img[y * img_size.x + x] = avg;
//       rotated_img[y * img_size.x + x] = orig_img[pcy[0] * img_size.x + pcx[0]];
    }
  }

  // copy over border values:
  {
    float tval = orig_img[(img_size.y-1) * img_size.x + 1];
    float bval = orig_img[1];
    float lval = orig_img[img_size.x];
    float rval = orig_img[2 * img_size.x - 1];
    int tidx = (img_size.y-1) * img_size.x;
    int x, y;
    for(x=0;x<img_size.x;x++) {
      rotated_img[tidx + x] = tval;
      rotated_img[x] = bval;
    }
    for(y=1;y<img_size.y-1;y++) {
      rotated_img[y * img_size.x + img_size.x-1] = rval;
      rotated_img[y * img_size.x] = lval;
    }
  }
}

// bool taImageProc::MakeEventSpecs() {
//   if(event_specs.size < 2)
//     event_specs.EnforceSize(2);
//   EventSpec* imgsp = (EventSpec*)event_specs[0];
//   imgsp->name = "Image";
//   if(imgsp->patterns.size != 1) {
//     imgsp->patterns.EnforceSize(1);
//     PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
//     imgps->SetToLayName("None");
//     imgps->type = PatternSpec::INACTIVE;
//     imgps->geom.x = 800;
//     imgps->geom.y = 600;
//     imgps->n_vals = imgps->geom.x * imgps->geom.y;
//     imgps->UpdateAfterEdit();
//   }

//   EventSpec* fltsp = (EventSpec*)event_specs[1];
//   fltsp->name = "Filter";
//   if(fltsp->patterns.size < 2 * filters.size)
//     fltsp->patterns.EnforceSize(2 * filters.size);
//   int pctr = 0;
//   int i;
//   for(i=0;i<filters.size;i++) {
//     DoGFilterSpec* fs = (DoGFilterSpec*)filters[i];
//     PatternSpec* on_ps = (PatternSpec*)fltsp->patterns[pctr++];
//     on_ps->name = fs->name + "_on";
//     on_ps->geom.x = fs->output_size.x;
//     on_ps->geom.y = fs->output_size.y;
//     on_ps->n_vals = fs->output_size.x * fs->output_size.y;
//     on_ps->SetToLayName(on_ps->name);
//     on_ps->UpdateAfterEdit();

//     PatternSpec* off_ps = (PatternSpec*)fltsp->patterns[pctr++];
//     off_ps->name = fs->name + "_off";
//     off_ps->geom.x = fs->output_size.x;
//     off_ps->geom.y = fs->output_size.y;
//     off_ps->n_vals = fs->output_size.x * fs->output_size.y;
//     off_ps->SetToLayName(off_ps->name);
//     off_ps->UpdateAfterEdit();
//   }
//   fltsp->LinearLayout(EventSpec::HORIZONTAL);
//   UpdateAllEvents();
//   winbMisc::DelayedMenuUpdate(this);
// }

// bool taImageProc::PlotFilters() {
//   if(event_specs.size < 2)
//     MakeEventSpecs();
//   EventSpec* imgsp = (EventSpec*)event_specs[0];
//   PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
//   imgps->geom.x = retina_size.x;
//   imgps->geom.y = retina_size.y;
//   imgps->n_vals = imgps->geom.x * imgps->geom.y;
//   imgps->UpdateAfterEdit();
//   UpdateAllEvents();

//   if(events.size < 2)
//     events.EnforceSize(2);
//   Event* imgev = (Event*)events[0];
//   float_Matrix& imgar = ((Pattern*)imgev->patterns[0])->value;
//   imgar.InitVals(0.0f);

//   for(int i=0;i<filters.size;i++) {
//     DoGFilterSpec* fs = (DoGFilterSpec*)filters[i];
//     int sx = fs->border.x;
//     int sy = fs->border.y;
//     int ex = sx + fs->input_size.x;
//     int ey = sy + fs->input_size.y;
//     int xc, yc;
//     for(yc = sy; yc < ey; yc += fs->filter_spacing.y) {
//       for(xc = sx; xc < ex; xc += fs->filter_spacing.x) {
// 	imgar[yc * retina_size.x + xc] += 1.0f;
//       }
//     }
//   }
//   imgar.NormMax(1.0f);
//   InitAllViews();
// }

bool taImageProc::LoadImage(istream&) {
  String fnm = taivGetFile::last_fname;
  //  cerr << "loading: " << fnm << endl;
  if(event_specs.size < 2)
    MakeEventSpecs();
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  if(events.size < 2)
    events.EnforceSize(2);
  Event* imgev = (Event*)events[0];
  imgev->SetSpec(imgsp);
  TwoDCoord img_size;
  ReadImageGrey(((Pattern*)imgev->patterns[0])->value, img_size, fnm);
  imgps->geom.x = img_size.x;
  imgps->geom.y = img_size.y;
  imgps->n_vals = imgps->geom.x * imgps->geom.y;
  imgps->UpdateAfterEdit();
  imgev->name = String(img_size.x) + "x" + String(img_size.y) + ":" + fnm;
  UpdateAllEvents();
  InitAllViews();
}

bool taImageProc::DoGFilterImage(float_Matrix& flt_vals_on, float_Matrix& flt_vals_off,
				float_Matrix& img_data,
				TwoDCoord& img_size, FloatTwoDCoord& img_ctr_off,
				FloatTwoDCoord& ret_ctr_off, bool add) {
  TwoDCoord img_ctr = img_size / 2;
  TwoDCoord img_ctr_off_sc = TwoDCoord(img_ctr_off * FloatTwoDCoord(img_ctr));
  if(img_ctr_off_sc.x < -img_ctr.x) img_ctr_off_sc.x = -img_ctr.x;
  if(img_ctr_off_sc.x > img_ctr.x) img_ctr_off_sc.x = img_ctr.x;
  if(img_ctr_off_sc.y < -img_ctr.y) img_ctr_off_sc.y = -img_ctr.y;
  if(img_ctr_off_sc.y > img_ctr.y) img_ctr_off_sc.y = img_ctr.y;
  TwoDCoord img_ctr_pt = img_ctr + img_ctr_off_sc;

  TwoDCoord ret_ctr = retina_size / 2;
  TwoDCoord ret_ctr_off_sc = TwoDCoord(ret_ctr_off * FloatTwoDCoord(ret_ctr));
  if(ret_ctr_off_sc.x < -ret_ctr.x) ret_ctr_off_sc.x = -ret_ctr.x;
  if(ret_ctr_off_sc.x > ret_ctr.x) ret_ctr_off_sc.x = ret_ctr.x;
  if(ret_ctr_off_sc.y < -ret_ctr.y) ret_ctr_off_sc.y = -ret_ctr.y;
  if(ret_ctr_off_sc.y > ret_ctr.y) ret_ctr_off_sc.y = ret_ctr.y;

  // -------------
  // |           |
  // | +---+     |
  // | |*x |     |
  // | +---+     |
  // -------------

  TwoDCoord img_off = (img_ctr_pt - ret_ctr) - ret_ctr_off_sc;

  int n_outputs = output_size.x * output_size.y;
  flt_vals_on.EnforceSize(n_outputs);
  flt_vals_off.EnforceSize(n_outputs);

  float_Matrix* use_flt_on = &flt_vals_on;
  float_Matrix* use_flt_off = &flt_vals_off;

  static float_Matrix tmp_flt_on;
  static float_Matrix tmp_flt_off;

  if(add) {
    tmp_flt_on.EnforceSize(n_outputs);
    tmp_flt_off.EnforceSize(n_outputs);
    tmp_flt_on.InitVals(0.0f);
    tmp_flt_off.InitVals(0.0f);
    use_flt_on = &tmp_flt_on;
    use_flt_off = &tmp_flt_off;
  }
  else {
    flt_vals_on.InitVals(0.0f);
    flt_vals_off.InitVals(0.0f);
  }

  TwoDCoord st = border;
  TwoDCoord ed = st + input_size;
  
  int xc, yc;
  int ctr = 0;
  for(yc = st.y; yc < ed.y; yc += filter_spacing.y) {
    for(xc = st.x; xc < ed.x; xc += filter_spacing.x, ctr++) {
      float cnvl = 0.0f;
      int xf, yf;
      int fctr = 0;
      for(yf = -filter_width; yf <= filter_width; yf++) {
	int iy = img_off.y + yc + yf;
	if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1;
	for(xf = -filter_width; xf <= filter_width; xf++, fctr++) {
	  int ix = img_off.x + xc + xf;
	  if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
	  int idx = iy * img_size.x + ix;
	  cnvl += img_data[idx] * filter[fctr];
	}
      }
      if(cnvl > 0.0f)
	(*use_flt_on)[ctr] = cnvl;
      else if(cnvl < 0.0f)
	(*use_flt_off)[ctr] = -cnvl;
    }
  }
  use_flt_on->UpdateAllRange();
  if(use_flt_on->range.Range() > .01f)
    use_flt_on->NormMax(1.0f);
  use_flt_off->UpdateAllRange();
  if(use_flt_off->range.Range() > .01f)
    use_flt_off->NormMax(1.0f);
  if(add) {			// add them back in!
    SimpleMathSpec ms;  ms.opr = SimpleMathSpec::ADD;
    flt_vals_on.SimpleMathArg(tmp_flt_on, ms);
    flt_vals_off.SimpleMathArg(tmp_flt_off, ms);
  }
}

bool taImageProc::DoGFilterImageBox(float_Matrix& img_data,
				    TwoDCoord& img_size, FloatTwoDCoord& img_ctr_off,
				    FloatTwoDCoord& ret_ctr_off, float scale) {
  TwoDCoord img_ctr = img_size / 2;
  TwoDCoord img_ctr_off_sc = TwoDCoord(img_ctr_off * FloatTwoDCoord(img_ctr));
  if(img_ctr_off_sc.x < -img_ctr.x) img_ctr_off_sc.x = -img_ctr.x;
  if(img_ctr_off_sc.x > img_ctr.x) img_ctr_off_sc.x = img_ctr.x;
  if(img_ctr_off_sc.y < -img_ctr.y) img_ctr_off_sc.y = -img_ctr.y;
  if(img_ctr_off_sc.y > img_ctr.y) img_ctr_off_sc.y = img_ctr.y;
  TwoDCoord img_ctr_pt = img_ctr + img_ctr_off_sc;

  float inv_sc = 1.0 / scale;
  TwoDCoord sc_ret = inv_sc * FloatTwoDCoord(retina_size);

  TwoDCoord ret_ctr = sc_ret / 2;
  TwoDCoord ret_ctr_off_sc = TwoDCoord(ret_ctr_off * FloatTwoDCoord(ret_ctr));
  if(ret_ctr_off_sc.x < -ret_ctr.x) ret_ctr_off_sc.x = -ret_ctr.x;
  if(ret_ctr_off_sc.x > ret_ctr.x) ret_ctr_off_sc.x = ret_ctr.x;
  if(ret_ctr_off_sc.y < -ret_ctr.y) ret_ctr_off_sc.y = -ret_ctr.y;
  if(ret_ctr_off_sc.y > ret_ctr.y) ret_ctr_off_sc.y = ret_ctr.y;

  TwoDCoord img_off = (img_ctr_pt - ret_ctr) - ret_ctr_off_sc;

  TwoDCoord st = TwoDCoord(inv_sc * FloatTwoDCoord(border));
  TwoDCoord ed = st + TwoDCoord(inv_sc * FloatTwoDCoord(input_size));
  
  for(int yc = st.y; yc < ed.y; yc++) {
    int iy = img_off.y + yc;
    if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1;
    {
      int ix = img_off.x + st.x;
      if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
      int idx = iy * img_size.x + ix;
      if(img_data[idx] > .5) img_data[idx] *= .5;
      else img_data[idx] += .5;
    }
    {
      int ix = img_off.x + (ed.x -1);
      if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
      int idx = iy * img_size.x + ix;
      if(img_data[idx] > .5) img_data[idx] *= .5;
      else img_data[idx] += .5;
    }
  }
  for(int xc = st.x; xc < ed.x; xc++) {
    int ix = img_off.x + xc;
    if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
    {
      int iy = img_off.y + st.y;
      if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1;
      int idx = iy * img_size.x + ix;
      if(img_data[idx] > .5) img_data[idx] *= .5;
      else img_data[idx] += .5;
    }
    {
      int iy = img_off.y + (ed.y - 1);
      if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1;
      int idx = iy * img_size.x + ix;
      if(img_data[idx] > .5) img_data[idx] *= .5;
      else img_data[idx] += .5;
    }
  }
}

bool taImageProc::FilterImage(Event* fltev, float img_x_off, float img_y_off, float scale,
			   float rotate, float ret_x_off, float ret_y_off, bool add)
{
  if(event_specs.size < 2)
    MakeEventSpecs();
  if(events.size < 1) {
    taMisc::Error("No Image in 1st event, LoadImage first!");
    return;
  }
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  EventSpec* fltsp = (EventSpec*)event_specs[1];
  Event* imgev = (Event*)events[0];

  if(fltev == imgev) {
    if(events.leaves < 2) {
      taMisc::Error("Cannot send filter results to image, and no other event!");
      return;
    }
    taMisc::Error("Cannot send filter results to image!  Using next event");
    fltev = (Event*)events.Leaf(1);
  }

  fltev->SetSpec(fltsp);

  float_Matrix* img_data = &(((Pattern*)imgev->patterns[0])->value);
  TwoDCoord img_size = imgps->geom;

  float_Matrix rot_img;
  if(rotate != 0.0f) {
    RotateImageData(rot_img, *img_data, img_size, rotate);
    img_data = &rot_img;
  }

  TwoDCoord sc_size;
  float_Matrix sc_img;
  if(scale != 1.0f) {
    ScaleImageData(sc_img, sc_size, *img_data, img_size, scale);
    img_data = &sc_img;
    img_size = sc_size;
  }

  FloatTwoDCoord img_off(img_x_off, img_y_off);
  FloatTwoDCoord ret_off(ret_x_off, ret_y_off);

  int pctr = 0;
  int i;
  for(i=0;i<filters.size;i++) {
    DoGFilterSpec* fs = (DoGFilterSpec*)filters[i];
    Pattern* on_pat = (Pattern*)fltev->patterns[pctr++];
    Pattern* off_pat = (Pattern*)fltev->patterns[pctr++];
    fs->FilterImage(on_pat->value, off_pat->value, *img_data, img_size, img_off, ret_off, add);
  }  
  InitAllViews();
}

bool taImageProc::FilterImageBox(float img_x_off, float img_y_off, float scale,
			      float rotate, float ret_x_off, float ret_y_off)
{
  if(events.size < 1) {
    taMisc::Error("No Image in 1st event, LoadImage first!");
    return;
  }
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  Event* imgev = (Event*)events[0];

  float_Matrix* img_data = &(((Pattern*)imgev->patterns[0])->value);
  TwoDCoord img_size = imgps->geom;

  // todo: currently ignored!
//   float_Matrix rot_img;
//   if(rotate != 0.0f) {
//     RotateImageData(rot_img, *img_data, img_size, rotate);
//     img_data = &rot_img;
//   }

  FloatTwoDCoord img_off(img_x_off, img_y_off);
  FloatTwoDCoord ret_off(ret_x_off, ret_y_off);

  int i;
  for(i=0;i<filters.size;i++) {
    DoGFilterSpec* fs = (DoGFilterSpec*)filters[i];
    fs->FilterImageBox(*img_data, img_size, img_off, ret_off, scale);
  }  
  InitAllViews();
}

bool taImageProc::FoveateImage(Event* toev, float box_ll_x, float box_ll_y, float box_ur_x, float box_ur_y,
			    float scale, float rotate, float ret_x_off, float ret_y_off,
!                           float img_x_off, float img_y_off, bool add, bool attend) {
  // Two things I'd really like to do with this:
  //   1: Remove image edge artifacts DIRECTLY in the filter output, like I do with attending
  //   2: Instead of "foveate", do "look" where the image consumes the whole retina (not just fovea).

  if(filters.size <= 0) {
    taMisc::Error("No DOG filters!  aborting FoveateImage");
    return;
  }
  DoGFilterSpec* fov_filt = NULL;
  int min_flt_spc = -1;
  for(int i=0;i<filters.size;i++) {
    DoGFilterSpec* fs = (DoGFilterSpec*)filters[i];
    if((fs->filter_spacing.x < min_flt_spc) || (min_flt_spc < 0)) {
      fov_filt = fs;
      min_flt_spc = fs->filter_spacing.x;
    }
  }
  // img_offset stuff is easy: just the middle of the box:
  FloatTwoDCoord obj_ctr((float) (0.5 * (float) (box_ll_x + box_ur_x)),
                       (float) (0.5 * (float) (box_ll_y + box_ur_y)));

  // translate into center-relative coords:
  FloatTwoDCoord obj_ctr_off = 2.0f * (obj_ctr - 0.5f);

  // DJJ - now rotate center point, so that the box ends up in the right place
  float rot_angle = -2.0f * PI * rotate;
  float rot_sin = sin(rot_angle);
  float rot_cos = cos(rot_angle);
  FloatTwoDCoord fov_off(obj_ctr_off.x * rot_cos + obj_ctr_off.y * rot_sin,
                       obj_ctr_off.y * rot_cos - obj_ctr_off.x * rot_sin);

  // Go back to lower-left coordinate frame for attending, later
  FloatTwoDCoord obj_ctr_rot = fov_off / 2.0f + 0.5f;

  //  cerr << "Object Center:  " << obj_ctr.x << "," << obj_ctr.y << endl;
  //cerr << "Obj Ctr Offset: " << obj_ctr_off.x << "," << obj_ctr_off.y << endl;
  //cerr << "Fov Offset:     " << fov_off.x << "," << fov_off.y << endl;
  //cerr << "Rotation: " << rotate << " Angle: " << rot_angle <<
  //  " sin: " << rot_sin << " cos: " << rot_cos << endl;
  //cerr << "Offset: " << img_x_off << "," << img_y_off << "  Rot: " << rotate << "  Scale: " << scale << endl;

  // Now add to the training offset
  if (!attend) {
    img_x_off += fov_off.x;
    img_y_off += fov_off.y;
  }
  
  // now, scale the thing to fit in fov_filt->input_size
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  TwoDCoord img_size = imgps->geom;

  // height and width in pixels of box:
  float pix_x = (box_ur_x - box_ll_x) * img_size.x;
  float pix_y = (box_ur_y - box_ll_y) * img_size.y;

  // DJJ _ bounding box is full image if attending
  if (attend) {
    pix_x = img_size.x;
    pix_y = img_size.y;
  }

  float sc_x = (float)fov_filt->input_size.x / pix_x;
  float sc_y = (float)fov_filt->input_size.y / pix_y;

  float orig_scale = scale;
  float fov_sc = MIN(sc_x, sc_y);
  scale *= fov_sc;
  if(scale > 100.0f)
    scale = 100.0f;
  if(scale < .01f)
    scale = .01f;

//   cerr << "scale: " << scale << endl;
  FilterImage(toev, img_x_off, img_y_off, scale, rotate, ret_x_off, ret_y_off, add);

  // DJJ: Do 'attend' operation if requested
  if (attend) {

    // input arguments are all in percent-of-image coordinate system
    // Goal is to transform to center point and radius in percent-of-retina coordinates
    //   after rotation and scaling

    // Convert to lower-left origin, retinal coordinate frame
    // Radius is half the largest dimension of fov box
    // Offset values were in center-based image coordinate frame

    // Convert radius & center point to retinal coordinate range (from fractional)
    float radius_x = orig_scale * ((float) (fov_filt->input_size.x * (box_ur_x - box_ll_x))) / 2.0f;
    float radius_y = orig_scale * ((float) (fov_filt->input_size.y * (box_ur_y - box_ll_y))) / 2.0f;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float radius;
    if (radius_x > radius_y) {
      scale_y = radius_x / radius_y;
      radius = radius_x;
    }
    else {
      scale_x = radius_y / radius_x;
      radius = radius_y;
    }
    int r_sq = (int) (radius * radius);

    float center_x = (orig_scale * (obj_ctr_rot.x - 0.5) + 0.5) -
      (orig_scale * img_x_off / 2.0f) - ret_x_off;
    float center_y = (orig_scale * (obj_ctr_rot.y - 0.5) + 0.5) -
      (orig_scale * img_y_off / 2.0f) - ret_y_off;

    TwoDCoord c;
    c.x = fov_filt->border.x + ((int) ((float) fov_filt->input_size.x * center_x + 0.5));
    c.y = fov_filt->border.y + ((int) ((float) fov_filt->input_size.y * center_y + 0.5));

    //    cerr << "Ret offset: " << ret_x_off << "," << ret_y_off << endl;
    //cerr << "Box: " << box_ll_x << "," << box_ll_y << " " << box_ur_x << "," << box_ur_y <<
    //  " Orig Scale: " << orig_scale << endl;
    //cerr << " Radius: " << radius_x << "," << radius_y << " El scale: " << scale_x << "," << scale_y << endl;
    //cerr << " Center: " << center_x << "," << center_y << endl;
    //cerr << " Radius squared: " << r_sq << " center ret: " << c.x << "," << c.y << endl;


    // Process on/off patterns for each filter
    int i;
    int pctr = 0;
    for (i = 0; i < filters.size; i++) {
      DoGFilterSpec* fs = (DoGFilterSpec *) filters[i];
      FloatTwoDCoord pat_size = fs->output_size;
      float_RArray *on_vals = &(((Pattern *) toev->patterns[pctr++])->value);
      float_RArray *off_vals = &(((Pattern *) toev->patterns[pctr++])->value);

      // Process each "output" pixel in the filter
      //      cerr << "   Filter: " << i <<
      //      " Input Size: " << fs->input_size.x << "," << fs->input_size.y <<
      //      " Output Size: " << pat_size.x << "," << pat_size.y << endl;

      int pat_x, pat_y, ret_x, ret_y;
      int pixel = 0;
      for (pat_y = 0; pat_y < pat_size.y; pat_y++) {
      for (pat_x = 0; pat_x < pat_size.x; pat_x++, pixel++) {

        // Important: convert to retinal coordinate frame
        ret_x = (int) ((float) fs->border.x +
                       ((float) pat_x * ((float) fs->input_size.x / (float) pat_size.x)) + 0.5f);
        ret_y = (int) ((float) fs->border.y +
                       ((float) pat_y * ((float) fs->input_size.y / (float) pat_size.y)) + 0.5f);

        // Determine the new value of the pixel
        // If within the radius, unchanged; if outside, scale based on square of distance
        float dist_sq = (scale_x * ((float) (ret_x - c.x)) * scale_x * ((float) (ret_x - c.x))) +
          (scale_y * ((float) (ret_y - c.y)) * scale_y * ((float) (ret_y - c.y)));

        //      cerr << "Pat: " << pat_x << "," << pat_y << " Ret: " << ret_x << "," << ret_y << " Distsq: " << dist_sq << "; ";

        if (dist_sq > r_sq) {
          float mult = (float) r_sq / (float) dist_sq;
          //float mult = 0.0f;
          (*on_vals)[pixel] *= mult;
          (*off_vals)[pixel] *= mult;
        }
      }
      }

      // Adapted from DoGFilterSpec::FilterImage - scale the values
      on_vals->UpdateAllRange();
      if(on_vals->range.Range() > .01f)
      on_vals->NormMax(1.0f);
      off_vals->UpdateAllRange();
      if(off_vals->range.Range() > .01f)
      off_vals->NormMax(1.0f);
    }
    InitAllViews();
  } // if (attend)
}

bool taImageProc::FoveateImageBox(float box_ll_x, float box_ll_y, float box_ur_x, float box_ur_y,
			       float scale, float rotate, float ret_x_off, float ret_y_off,
			       float img_x_off, float img_y_off) {
  if(filters.size <= 0) {
    taMisc::Error("No DOG filters!  aborting FoveateImageBox");
    return;
  }
  DoGFilterSpec* fov_filt = NULL;
  int min_flt_spc = -1;
  for(int i=0;i<filters.size;i++) {
    DoGFilterSpec* fs = (DoGFilterSpec*)filters[i];
    if((fs->filter_spacing.x < min_flt_spc) || (min_flt_spc < 0)) {
      fov_filt = fs;
      min_flt_spc = fs->filter_spacing.x;
    }
  }
  // img_offset stuff is easy: just the middle of the box:
  float obj_ctr_x = .5 * (box_ll_x + box_ur_x);
  float obj_ctr_y = .5 * (box_ll_y + box_ur_y);
  // translate into center-relative coords:
  img_x_off += 2.0 * (obj_ctr_x - .5);
  img_y_off += 2.0 * (obj_ctr_y - .5);
  
  // now, scale the thing to fit in fov_filt->input_size
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  TwoDCoord img_size = imgps->geom;

  // height and width in pixels of box:
  float pix_x = (box_ur_x - box_ll_x) * img_size.x;
  float pix_y = (box_ur_y - box_ll_y) * img_size.y;

  float sc_x = (float)fov_filt->input_size.x / pix_x;
  float sc_y = (float)fov_filt->input_size.y / pix_y;

  float fov_sc = MIN(sc_x, sc_y);
  scale *= fov_sc;
  if(scale > 10.0f)
    scale = 10.0f;
  if(scale < .01f)
    scale = .01f;

  FilterImageBox(img_x_off, img_y_off, scale, rotate, ret_x_off, ret_y_off);
}

bool taImageProc::ScaleImage(float scale) {
  if(event_specs.size < 2)
    MakeEventSpecs();
  if(events.size < 1) {
    taMisc::Error("No Image in 1st event, LoadImage first!");
    return;
  }
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  Event* imgev = (Event*)events[0];

  float_Matrix& img_data = ((Pattern*)imgev->patterns[0])->value;

  TwoDCoord sc_size;
  float_Matrix sc_img;
  ScaleImageData(sc_img, sc_size, img_data, imgps->geom, scale);
  img_data = sc_img;
  img_data.EnforceSize(sc_img.size); // just to be sure

  imgps->geom.x = sc_size.x;
  imgps->geom.y = sc_size.y;
  imgps->n_vals = imgps->geom.x * imgps->geom.y;
  imgps->UpdateAfterEdit();
  InitAllViews();
}

bool taImageProc::RotateImage(float rotate) {
  if(event_specs.size < 2)
    MakeEventSpecs();
  if(events.size < 1) {
    taMisc::Error("No Image in 1st event, LoadImage first!");
    return;
  }
  EventSpec* imgsp = (EventSpec*)event_specs[0];
  PatternSpec* imgps = (PatternSpec*)imgsp->patterns[0];
  Event* imgev = (Event*)events[0];

  float_Matrix& img_data = ((Pattern*)imgev->patterns[0])->value;
  float_Matrix rot_img;
  RotateImageData(rot_img, img_data, imgps->geom, rotate);
  img_data = rot_img;
  InitAllViews();
}

*/

