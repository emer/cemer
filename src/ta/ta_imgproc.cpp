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
#include "ta_project.h"

#include <QMatrix>


///////////////////////////////////////////
//	taImage

void taImage::Initialize() {
}

void taImage::Copy_(const taImage& cp) {
  q_img = cp.q_img;
}

bool taImage::LoadImage(const String& fname) {
  if(fname.empty()) {
    taFiler* flr = GetLoadFiler(fname);
    name = flr->fname;
    flr->Close();
    taRefN::unRefDone(flr);
  }
  else {
    name = fname;
  }
  QString fn = (const char*)name;
  if(!q_img.load(fn)) {
    taMisc::Error("LoadImage: could not read image file:", name);
    return false;
  }
  return true;
}

float taImage::GetPixelGrey_float(int x, int y) {
  if(q_img.isNull()) {
    return -1.0f;
  }
  QRgb pix = q_img.pixel(x, y);
  float gval = qGray(pix) / 255.0f;
  return gval;
}

bool taImage::GetPixelRGB_float(int x, int y, float& r, float& g, float& b) {
  if(q_img.isNull()) {
    return false;
  }
  QRgb pix = q_img.pixel(x, y);
  r = qRed(pix) / 255.0f;
  g = qGreen(pix) / 255.0f;
  b = qBlue(pix) / 255.0f;
  return true;
}

bool taImage::ImageToGrey_float(float_Matrix& img_data) {
  if(q_img.isNull()) {
    return false;
  }
  img_data.SetGeom(2, q_img.width(), q_img.height());

  for(int y=0; y<q_img.height(); y++) {
    for(int x=0; x< q_img.width(); x++) {
      QRgb pix = q_img.pixel(x, y);
      float gval = qGray(pix) / 255.0f;
      img_data.Set(gval, x, q_img.height()-1 - y);
    }
  }
  return true;
}

bool taImage::ImageToRGB_float(float_Matrix& rgb_data) {
  if(q_img.isNull()) {
    return false;
  }
  rgb_data.SetGeom(3, q_img.width(), q_img.height(), 3); // r,g,b = 3rd dim

  for(int y=0; y<q_img.height(); y++) {
    for(int x=0; x< q_img.width(); x++) {
      QRgb pix = q_img.pixel(x, y);
      float rval = qRed(pix) / 255.0f;
      float gval = qGreen(pix) / 255.0f;
      float bval = qBlue(pix) / 255.0f;
      rgb_data.Set(rval, x, q_img.height()-1 -y, 0);
      rgb_data.Set(gval, x, q_img.height()-1 -y, 1);
      rgb_data.Set(bval, x, q_img.height()-1 -y, 2);
    }
  }
  return true;
}

bool taImage::ScaleImage(float sx, float sy, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  int wd = q_img.width();
  int ht = q_img.height();
  int nw_wd = (int)(sx * (float)wd);
  int nw_ht = (int)(sy * (float)ht);
  if(smooth)
    q_img = q_img.scaled(nw_wd, nw_ht, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  else
    q_img = q_img.scaled(nw_wd, nw_ht); // default is fast
  return true;
}

bool taImage::RotateImage(float norm_deg, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  float deg = norm_deg * 360.0f;
  QMatrix mat;
  mat.rotate(deg);
  if(smooth)
    q_img = q_img.transformed(mat, Qt::SmoothTransformation);
  else
    q_img = q_img.transformed(mat); // default is fast
  return true;
}

bool taImage::TranslateImage(float move_x, float move_y, bool smooth) {
  if(q_img.isNull()) {
    return false;
  }
  int wd = q_img.width();
  int ht = q_img.height();
  int nw_move_x= (int)(move_x * (float)wd);
  int nw_move_y = (int)(move_y * (float)ht);
  QMatrix mat;
  mat.translate(nw_move_x, nw_move_y);
  if(smooth)
    q_img = q_img.transformed(mat, Qt::SmoothTransformation);
  else
    q_img = q_img.transformed(mat); // default is fast
  return true;
}

///////////////////////////////////////////
//	DoG Filter

void DoGFilterSpec::Initialize() {
  color_chan = BLACK_WHITE;
  filter_width = 8;
  filter_size = filter_width * 2 + 1;
  on_sigma = 2.0f;
  off_sigma = 4.0f;
  circle_edge = true;
//   on_filter.SetGeom(2, filter_size, filter_size);
//   off_filter.SetGeom(2, filter_size, filter_size);
//   net_filter.SetGeom(2, filter_size, filter_size);
}

void DoGFilterSpec::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  filter_size = filter_width * 2 + 1;
  UpdateFilter();
}

float DoGFilterSpec::FilterPoint(int x, int y, float r_val, float g_val, float b_val) {
  if(color_chan == BLACK_WHITE) {
    float grey = (r_val + g_val + b_val) / 3.0f;
    return grey * (on_filter.FastEl(x+filter_width, y+filter_width) * grey - 
		   off_filter.FastEl(x+filter_width, y+filter_width) * grey);
  }
  else if(color_chan == RED_GREEN) {
    return (on_filter.FastEl(x+filter_width, y+filter_width) * r_val - 
	    off_filter.FastEl(x+filter_width, y+filter_width) * g_val);
  }
  else if(color_chan == BLUE_YELLOW) {
    float y_val = .5f * (r_val + g_val);
    return (on_filter.FastEl(x+filter_width, y+filter_width) * b_val - 
	    off_filter.FastEl(x+filter_width, y+filter_width) * y_val);
  }
  return 0.0f;
}

void DoGFilterSpec::RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt,
				 float_Matrix& net_flt) {
  on_flt.SetGeom(2, filter_size, filter_size);
  off_flt.SetGeom(2, filter_size, filter_size);
  net_flt.SetGeom(2, filter_size, filter_size);
  int x,y;
  for(y=-filter_width; y<=filter_width; y++) {
    for(x=-filter_width; x<=filter_width; x++) {
      float sqdist = (float)(x * x + y * y);
      float dist = sqrt(sqdist);
      float ong = 0.0f;
      float offg = 0.0f;
      if(!circle_edge || (dist <= filter_width)) { // only set values inside of filter radius
	ong = GaussVal(sqdist, on_sigma);
	offg = GaussVal(sqdist, off_sigma);
      }
      on_flt.Set(ong, y+filter_width, x+filter_width);
      off_flt.Set(offg, y+filter_width, x+filter_width);
    }
  }

  taMath_float::vec_norm_sum(&on_flt); // make sure sums to 1.0
  taMath_float::vec_norm_sum(&off_flt); // make sure sums to 1.0

  for(int i=0;i<on_flt.size;i++) {
    float net = on_flt.FastEl_Flat(i) - off_flt.FastEl_Flat(i);
    net_flt.FastEl_Flat(i) = net;
  }
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
  filter.SetGeom(2, x_size, y_size);
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
  flt.SetGeom(2, x_size, y_size);
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
  region = PARAFOVEA;
  retina_size.x = 321;
  retina_size.y = 241;
  output_units = 0;
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
// 		DoG + Retinal Spacing

void DoGRetinaSpec::Initialize() {
  
}

void DoGRetinaSpec::UpdateAfterEdit() {
  dog.UpdateAfterEdit();
  spacing.UpdateAfterEdit();
}

//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		taImageProc

void taImageProc::Initialize() {
}

void taImageProc::Destroy() {
}

bool taImageProc::RenderBorder_float(float_Matrix& img_data) {
  if(img_data.dims() == 3) {	// an rgb guy
    for(int i=0;i<3;i++) {
      float_Matrix* cmp = img_data.GetFrameSlice(i);
      taBase::Ref(cmp);
      RenderBorder_float(*cmp);
      taBase::unRefDone(cmp);
    }
    return true;
  }
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));
  float tavg = 0.0f;
  float bavg = 0.0f;
  for(int x=0;x<img_size.x;x++) {
    tavg += img_data.FastEl(x, img_size.y-1);
    bavg += img_data.FastEl(x, 0);
  }
  tavg /= (float)(img_size.x);
  bavg /= (float)(img_size.x);

  float lavg = 0.0f;
  float ravg = 0.0f;
  for(int y=0;y<img_size.y;y++) {
    ravg += img_data.FastEl(img_size.x-1, y);
    lavg += img_data.FastEl(0, y);
  }
  lavg /= (float)(img_size.y);
  ravg /= (float)(img_size.y);

  float oavg = .25 * (tavg + bavg + lavg + ravg);

  for(int x=0;x<img_size.x;x++) {
    img_data.FastEl(x, img_size.y-1) = oavg;
    img_data.FastEl(x, 0) = oavg;
  }
  for(int y=1;y<img_size.y-1;y++) {
    img_data.FastEl(img_size.x-1, y) = oavg;
    img_data.FastEl(0, y) = oavg;
  }
//   cerr << "border avgs: t: " << tavg << ", b: " << bavg
//        << ", l: " << lavg << ", r: " << ravg << endl;
  return true;
}

bool taImageProc::TranslateImage_float(float_Matrix& xlated_img, float_Matrix& orig_img,
				       float move_x, float move_y) {

  FloatTwoDCoord deltas(move_x, move_y);
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  FloatTwoDCoord img_ctr = FloatTwoDCoord(img_size) / 2.0f;
  TwoDCoord img_off = TwoDCoord(deltas * img_ctr);

  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    xlated_img.SetGeom(3, img_size.x, img_size.y, 3);
    rgb_img = true;
  }
  else
    xlated_img.SetGeom(2, img_size.x, img_size.y);

  for(int ny = 0; ny < img_size.y; ny++) {
    int iy = ny - img_off.y;
    if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1; // use border if "off-screen"
    for(int nx = 0; nx < img_size.x; nx++) {
      int ix = nx - img_off.x;
      if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
      if(rgb_img) {
	for(int i=0;i<3;i++)
	  xlated_img.FastEl(nx, ny, i) = orig_img.FastEl(ix, iy, i);
      }
      else {
	xlated_img.FastEl(nx, ny) = orig_img.FastEl(ix, iy);
      }
    }
  }
  return true;
}

bool taImageProc::ScaleImage_float(float_Matrix& scaled_img, float_Matrix& orig_img,
				   float scale) {
  if(scale < .01f) {
    taMisc::Error("Can't scale below .01.");
    return false;
  }
  if(scale > 100.0f) {
    taMisc::Error("Can't scale above 100.");
    return false;
  }
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  TwoDCoord scaled_size; 
  scaled_size.x = 2 + (int)(scale * (img_size.x-2)); // keep border in there
  scaled_size.y = 2 + (int)(scale * (img_size.y-2));

  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    scaled_img.SetGeom(3, scaled_size.x, scaled_size.y, 3);
    rgb_img = true;
  }
  else
    scaled_img.SetGeom(2, scaled_size.x, scaled_size.y);
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
  sc_ary.SetGeom(2, n_orig_pix*2 + 1, n_orig_pix*2 + 1);
  int x, y;
  for(y=-n_orig_pix; y<= n_orig_pix; y++) {
    float ysc = extra;
    if((y >= -half_int) && (y <= half_int))
      ysc = 1.0f;
    for(x=-n_orig_pix; x<= n_orig_pix; x++) {
      float xsc = extra;
      if((x >= -half_int) && (x <= half_int))
	xsc = 1.0f;
      sc_ary.Set(ysc * xsc, x + n_orig_pix, y + n_orig_pix);
    }
  }

  taMath_float::vec_norm_sum(&sc_ary);		// make it add to 1

  for(y=0; y<scaled_size.y; y++) {
    int oyc = (int)floor(.5f + ((float)y / scale));
    for(x=0; x<scaled_size.x; x++) {
      int oxc = (int)floor(.5f + ((float)x / scale));
      float r_avg = 0.0f;
      float g_avg = 0.0f;
      float b_avg = 0.0f;
      int oxi, oyi;
      for(oyi=-n_orig_pix; oyi<=n_orig_pix; oyi++) {
	int oy = oyc + oyi;
	if(oy < 0) oy = 0; if(oy >= img_size.y) oy = img_size.y-1;
	for(oxi=-n_orig_pix;oxi<=n_orig_pix;oxi++) {
	  int ox = oxc + oxi;
	  if(ox < 0) ox = 0; if(ox >= img_size.x) ox = img_size.x-1;
	  float sc = sc_ary.FastEl(oxi + n_orig_pix, oyi + n_orig_pix);
	  if(rgb_img) {
	    r_avg += sc * orig_img.FastEl(ox, oy, 0);
	    g_avg += sc * orig_img.FastEl(ox, oy, 1);
	    b_avg += sc * orig_img.FastEl(ox, oy, 2);
	  }
	  else {
	    r_avg += sc * orig_img.FastEl(ox, oy);
	  }
	}
      }
      if(rgb_img) {
	scaled_img.FastEl(x, y, 0) = r_avg;
	scaled_img.FastEl(x, y, 1) = g_avg;
	scaled_img.FastEl(x, y, 2) = b_avg;
      }
      else {
	scaled_img.FastEl(x, y) = r_avg;
      }	
    }
  }
  return true;
}

// get pixel coordinates (pc1, 2) with norm weights (pw1, 2) for given floating coordinate coord
void taImageProc::GetWeightedPixels_float(float coord, int size, int* pc, float* pw) {
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


bool taImageProc::RotateImage_float(float_Matrix& rotated_img, float_Matrix& orig_img,
				    float rotate) {
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1));

  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    rotated_img.SetGeom(3, img_size.x, img_size.y, 3);
    rgb_img = true;
  }
  else
    rotated_img.SetGeom(2, img_size.x, img_size.y);
  FloatTwoDCoord ctr = ((FloatTwoDCoord) img_size) / 2.0f;

  rotate *= 2.0f * taMath_float::pi; // convert to radians
  float rot_sin = sin(rotate);
  float rot_cos = cos(rotate);

  float_Matrix sc_ary;  sc_ary.SetGeom(2, 2, 2);

  int x,y;			// coords in new image
  for(y=0;y<img_size.y;y++) {
    for(x=0;x<img_size.x;x++) {
      float cx = ((float) x) - ctr.x;
      float cy = ((float) y) - ctr.y;

      float org_x = cx * rot_cos + cy * rot_sin + ctr.x;
      float org_y = cy * rot_cos - cx * rot_sin + ctr.y;

      int pcx[2];
      int pcy[2];
      float pwx[2];
      float pwy[2];

      GetWeightedPixels_float(org_x, img_size.x, pcx, pwx);
      GetWeightedPixels_float(org_y, img_size.y, pcy, pwy);

      int oxi, oyi;
      for(oyi=0;oyi<2;oyi++) {
	for(oxi=0;oxi<2;oxi++) {
	  sc_ary.FastEl(oxi, oyi) = pwx[oxi] + pwy[oyi];
	}
      }
      taMath_float::vec_norm_sum(&sc_ary);

      float r_avg = 0.0f;
      float g_avg = 0.0f;
      float b_avg = 0.0f;
      for(oyi=0;oyi<2;oyi++) {
	for(oxi=0;oxi<2;oxi++) {
	  int iy = pcy[oyi];
	  int ix = pcx[oxi];
	  if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1;
	  if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
	  if(rgb_img) {
	    r_avg += sc_ary.FastEl(oxi, oyi) * orig_img.FastEl(ix, iy, 0); 
	    g_avg += sc_ary.FastEl(oxi, oyi) * orig_img.FastEl(ix, iy, 1);
	    b_avg += sc_ary.FastEl(oxi, oyi) * orig_img.FastEl(ix, iy, 2);
	  }
	  else {
	    r_avg += sc_ary.FastEl(oxi, oyi) * orig_img.SafeEl(ix, iy);
	  }
	}
      }
      if(rgb_img) {
	rotated_img.FastEl(x,y,0) = r_avg;
	rotated_img.FastEl(x,y,1) = g_avg;
	rotated_img.FastEl(x,y,2) = b_avg;
      }
      else {
	rotated_img.FastEl(x,y) = r_avg;
      }
    }
  }
  return true;
}

bool taImageProc::CropImage_float(float_Matrix& crop_img, float_Matrix& orig_img, 
				  int crop_width, int crop_height) {
  TwoDCoord img_size(orig_img.dim(0), orig_img.dim(1)); 
  TwoDCoord crop_size(crop_width, crop_height);
  if(crop_size.x < 0) crop_size.x = img_size.x;
  if(crop_size.y < 0) crop_size.y = img_size.y;

  TwoDCoord img_ctr = img_size / 2;
  TwoDCoord crop_ctr = crop_size / 2;
  TwoDCoord img_off = img_ctr - crop_ctr; // offset for 0,0 pixel of cropped image, in orig_img
  
  bool rgb_img = false;
  if(orig_img.dims() == 3) { // rgb
    crop_img.SetGeom(3, crop_size.x, crop_size.y, 3);
    rgb_img = true;
  }
  else
    crop_img.SetGeom(2, crop_size.x, crop_size.y);

  for(int ny = 0; ny < crop_size.y; ny++) {
    int iy = img_off.y + ny;
    if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1; // use border if "off-screen"
    for(int nx = 0; nx < crop_size.x; nx++) {
      int ix = img_off.x + nx;
      if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
      if(rgb_img) {
	for(int i=0;i<3;i++)
	  crop_img.FastEl(nx, ny, i) = orig_img.FastEl(ix, iy, i);
      }
      else {
	crop_img.FastEl(nx, ny) = orig_img.FastEl(ix, iy);
      }
    }
  }
  return true;
}

bool taImageProc::TransformImage_float(float_Matrix& xformed_img, float_Matrix& orig_img,
				       float move_x, float move_y, float rotate,
				       float scale, int crop_width, int crop_height)
{
  float_Matrix* use_img = &orig_img;
  float_Matrix xlate_img;     	taBase::Ref(xlate_img);
  float_Matrix rot_img;		taBase::Ref(rot_img);
  float_Matrix sc_img;		taBase::Ref(sc_img);

  // render border after each xform to keep edges clean..
  taImageProc::RenderBorder_float(*use_img);
  if((move_x != 0.0f) || (move_y != 0.0f)) {
    taImageProc::TranslateImage_float(xlate_img, *use_img, move_x, move_y);
    use_img = &xlate_img;
    taImageProc::RenderBorder_float(*use_img);
  }
  if(rotate != 0.0f) {
    taImageProc::RotateImage_float(rot_img, *use_img, rotate);
    use_img = &rot_img;
    taImageProc::RenderBorder_float(*use_img);
  }
  if(scale != 1.0f) {
    taImageProc::ScaleImage_float(sc_img, *use_img, scale);
    use_img = &sc_img;
    taImageProc::RenderBorder_float(*use_img);
  }
  if(crop_width < 0 && crop_height < 0) {
    xformed_img = *use_img;
  }
  else {
    taImageProc::CropImage_float(xformed_img, *use_img, crop_width, crop_height);
  }
  return true;
}

bool taImageProc::DoGFilterRetina(float_Matrix& on_output, float_Matrix& off_output,
				  float_Matrix& retina_img, DoGRetinaSpec& spec,
				  bool superimpose) {
  spec.dog.UpdateFilter();		// just to be sure

  TwoDCoord img_size = spec.spacing.retina_size;

  if((retina_img.dim(0) != img_size.x) || 
     (retina_img.dim(1) != img_size.y)) {
    taMisc::Error("DoGFilterImage: retina_img is not appropriate size!");
    return false;
  }

  on_output.SetGeom(2, spec.spacing.output_size.x, spec.spacing.output_size.y);
  off_output.SetGeom(2, spec.spacing.output_size.x, spec.spacing.output_size.y);

  float_Matrix* on_out = &on_output;
  float_Matrix* off_out = &off_output;
  static float_Matrix tmp_on_out;
  static float_Matrix tmp_off_out;

  if(superimpose) {
    tmp_on_out.SetGeom(2, spec.spacing.output_size.x, spec.spacing.output_size.y);
    tmp_off_out.SetGeom(2, spec.spacing.output_size.x, spec.spacing.output_size.y);
    tmp_on_out.InitVals(0.0f);
    tmp_off_out.InitVals(0.0f);
    on_out = &tmp_on_out;
    off_out = &tmp_off_out;
  }
  else {
    on_output.InitVals(0.0f);
    off_output.InitVals(0.0f);
  }

  bool rgb_img = false;
  if(retina_img.dims() == 3) rgb_img = true;

  TwoDCoord st = spec.spacing.border;
  
  for(int yo = 0; yo < spec.spacing.output_size.y; yo++) {
    int yc = st.y + yo * spec.spacing.spacing.y; 
    for(int xo = 0; xo < spec.spacing.output_size.x; xo++) {
      int xc = st.x + xo * spec.spacing.spacing.x; 
      // now convolve with dog filter
      float cnvl = 0.0f;
      for(int yf = -spec.dog.filter_width; yf <= spec.dog.filter_width; yf++) {
	int iy = yc + yf;
	if(iy < 0) iy = 0;  if(iy >= img_size.y) iy = img_size.y-1;
	for(int xf = -spec.dog.filter_width; xf <= spec.dog.filter_width; xf++) {
	  int ix = xc + xf;
	  if(ix < 0) ix = 0;  if(ix >= img_size.x) ix = img_size.x-1;
	  if(rgb_img) {
	    cnvl += spec.dog.FilterPoint(xf, yf, retina_img.FastEl(ix, iy,0),
					 retina_img.FastEl(ix, iy, 1),
					 retina_img.FastEl(ix, iy, 2));
	  }
	  else {
	    float gval = retina_img.FastEl(ix, iy);
	    cnvl += spec.dog.FilterPoint(xf, yf, gval, gval, gval);
	  }
	}
      }
      if(cnvl > 0.0f)
	on_out->FastEl(xo, yo) = cnvl;
      else 
	off_out->FastEl(xo, yo) = -cnvl;
    }
  }
  
  int idx;
  float on_max = taMath_float::vec_max(on_out, idx);
  if(on_max > .01f)
     taMath_float::vec_norm_max(on_out);

  float off_max = taMath_float::vec_max(off_out, idx);
  if(off_max > .01f)
    taMath_float::vec_norm_max(off_out);

  if(superimpose) {			// add them back in!
    taMath_float::vec_add(&on_output, on_out);
    taMath_float::vec_add(&off_output, off_out);
  }
  return true;
}


bool taImageProc::AttentionFilter(float_Matrix& mat, float radius_pct) {
  TwoDCoord img_size(mat.dim(0), mat.dim(1));
  TwoDCoord img_ctr = img_size / 2;

  float max_radius = taMath_float::max(img_ctr.x, img_ctr.y);
  float scale_x = max_radius / (float)img_ctr.x;
  float scale_y = max_radius / (float)img_ctr.y;
  float radius = radius_pct * max_radius;
  float r_sq = radius * radius;

  for (int y = 0; y < img_size.y; y++) {
    for (int x = 0; x < img_size.x; x++) {
      float dist_x = scale_x * float(x - img_ctr.x);
      float dist_y = scale_y * float(y - img_ctr.y);
      float dist_sq = dist_x * dist_x + dist_y * dist_y;
      if (dist_sq > r_sq) {
	float mult = (float) r_sq / (float) dist_sq;
	mat.FastEl(x,y) *= mult;
      }
    }
  }
  int idx;
  float max_v = taMath_float::vec_max(&mat, idx);
  if(max_v > .01f)
    taMath_float::vec_norm_max(&mat);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 		Full Retinal Spec

void RetinaSpec::Initialize() {
  color_type = MONOCHROME;
  retina_size.x = 341; retina_size.y = 241;
}

void RetinaSpec::UpdateRetinaSize() {
  for(int i=0;i<dogs.size; i++) {
    DoGRetinaSpec* sp = dogs[i];
    sp->spacing.retina_size = retina_size;
    sp->spacing.UpdateAfterEdit();
  }
}

void RetinaSpec::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  UpdateRetinaSize();
}

void RetinaSpec::DefaultFilters() {
  if(color_type == COLOR)
    dogs.EnforceSize(7);
  else
    dogs.EnforceSize(3);
  UpdateRetinaSize();
  DoGRetinaSpec* sp;
  int cnt = 0;
  sp = dogs[cnt++];
  sp->name = "high_freq_bw";
  sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
  sp->dog.filter_width = 8;
  sp->dog.on_sigma = 2;
  sp->dog.off_sigma = 4;
  sp->spacing.region = RetinalSpacingSpec::FOVEA;
  sp->spacing.border.x = 109; sp->spacing.border.y = 85;
  sp->spacing.spacing.x = 2; sp->spacing.spacing.y = 2;
  sp->UpdateAfterEdit();

  sp = dogs[cnt++];
  sp->name = "med_freq_bw";
  sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
  sp->dog.filter_width = 16;
  sp->dog.on_sigma = 4;
  sp->dog.off_sigma = 8;
  sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
  sp->spacing.border.x = 6; sp->spacing.border.y = 14;
  sp->spacing.spacing.x = 4; sp->spacing.spacing.y = 4;
  sp->UpdateAfterEdit();

  sp = dogs[cnt++];
  sp->name = "low_freq_bw";
  sp->dog.color_chan = DoGFilterSpec::BLACK_WHITE;
  sp->dog.filter_width = 32;
  sp->dog.on_sigma = 8;
  sp->dog.off_sigma = 16;
  sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
  sp->spacing.border.x = 6; sp->spacing.border.y = 16;
  sp->spacing.spacing.x = 8; sp->spacing.spacing.y = 8;
  sp->UpdateAfterEdit();

  if(color_type == COLOR) {
    sp = dogs[cnt++];
    sp->name = "med_freq_rg";
    sp->dog.color_chan = DoGFilterSpec::RED_GREEN;
    sp->dog.filter_width = 16;
    sp->dog.on_sigma = 4;
    sp->dog.off_sigma = 8;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.border.x = 6; sp->spacing.border.y = 14;
    sp->spacing.spacing.x = 4; sp->spacing.spacing.y = 4;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "med_freq_by";
    sp->dog.color_chan = DoGFilterSpec::BLUE_YELLOW;
    sp->dog.filter_width = 16;
    sp->dog.on_sigma = 4;
    sp->dog.off_sigma = 8;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.border.x = 6; sp->spacing.border.y = 14;
    sp->spacing.spacing.x = 4; sp->spacing.spacing.y = 4;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "low_freq_rg";
    sp->dog.color_chan = DoGFilterSpec::RED_GREEN;
    sp->dog.filter_width = 32;
    sp->dog.on_sigma = 8;
    sp->dog.off_sigma = 16;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.border.x = 8; sp->spacing.border.y = 16;
    sp->spacing.spacing.x = 8; sp->spacing.spacing.y = 8;
    sp->UpdateAfterEdit();

    sp = dogs[cnt++];
    sp->name = "low_freq_by";
    sp->dog.color_chan = DoGFilterSpec::BLUE_YELLOW;
    sp->dog.filter_width = 32;
    sp->dog.on_sigma = 8;
    sp->dog.off_sigma = 16;
    sp->spacing.region = RetinalSpacingSpec::PARAFOVEA;
    sp->spacing.border.x = 8; sp->spacing.border.y = 16;
    sp->spacing.spacing.x = 8; sp->spacing.spacing.y = 8;
    sp->UpdateAfterEdit();
  }
}

void RetinaSpec::ConfigDataTable(DataTable* dt, bool reset_cols) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!dt) {
    DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("InputData");
    dt = dgp->NewEl(1, &TA_DataTable); // todo: should be in InputData
    if(!name.empty())
      dt->name = name + "_InputData";
    else
      dt->name = "RetinaSpec_InputData";
  }
  if(reset_cols) dt->Reset();
  dt->StructUpdate(true);
  int idx =0;
  dt->FindMakeColName("Name", idx, DataTable::VT_STRING, 0);
  dt->FindMakeColName("LookBox", idx, DataTable::VT_FLOAT, 1, 4);
  dt->FindMakeColName("Move", idx, DataTable::VT_FLOAT, 1, 2);
  dt->FindMakeColName("Scale", idx, DataTable::VT_FLOAT, 0);
  dt->FindMakeColName("Rotate", idx, DataTable::VT_FLOAT, 0);
  DataArray_impl* col;
  if(color_type == COLOR)
    col = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 3,
			retina_size.x, retina_size.y, 3);
  else
    col = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 2,
			retina_size.x, retina_size.y);
  col->SetUserData("IMAGE", true);
  for(int i=0;i<dogs.size; i++) {
    DoGRetinaSpec* sp = dogs[i];
    dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
				sp->spacing.output_size.x, sp->spacing.output_size.y);
    dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT, 2,
				sp->spacing.output_size.x, sp->spacing.output_size.y);
  }
  dt->StructUpdate(false);
}

bool RetinaSpec::FilterImageData(float_Matrix& img_data, DataTable* dt,
				 float move_x, float move_y,
				 float scale, float rotate, bool superimpose)
{
  if (!dt) return false;
  if(dogs.size == 0) return false;
  if(superimpose) {
    if(dt->rows <= 0) superimpose = false; // can't do it!
  }
  if(!superimpose) {		// might have changed!
    dt->AddBlankRow();
  }

  int idx;
  DataArray_impl* da_ret;
  if(color_type == COLOR)
    da_ret = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 3,
				 retina_size.x, retina_size.y, 3);
  else
    da_ret = dt->FindMakeColName("RetinaImage", idx, DataTable::VT_FLOAT, 2,
				 retina_size.x, retina_size.y);
  da_ret->SetUserData("IMAGE", true);
  float_Matrix* ret_img = (float_Matrix*)da_ret->GetValAsMatrix(-1);
  taBase::Ref(ret_img);

  taImageProc::TransformImage_float(*ret_img, img_data, move_x, move_y, rotate, scale,
				    retina_size.x, retina_size.y);

  for(int i=0;i<dogs.size;i++) {
    DoGRetinaSpec* sp = dogs[i];
    DataArray_impl* da_on = dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
						sp->spacing.output_size.x, sp->spacing.output_size.y);
    DataArray_impl* da_off = dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT,
						 2, sp->spacing.output_size.x, sp->spacing.output_size.y);

    float_Matrix* on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
    float_Matrix* off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
    taBase::Ref(on_mat);
    taBase::Ref(off_mat);
    taImageProc::DoGFilterRetina(*on_mat, *off_mat, *ret_img, *sp, superimpose);
    taBase::unRefDone(on_mat);
    taBase::unRefDone(off_mat);
  }
  taBase::unRefDone(ret_img);

  float_Matrix* mv_mat = (float_Matrix*)dt->FindMakeColName("Move", idx, DataTable::VT_FLOAT, 1, 2)->GetValAsMatrix(-1);
  taBase::Ref(mv_mat);
  mv_mat->FastEl(0) = move_x; mv_mat->FastEl(1) = move_y;
  taBase::unRefDone(mv_mat);
  
  dt->FindMakeColName("Scale", idx, DataTable::VT_FLOAT, 0)->SetValAsFloat(scale, -1);
  dt->FindMakeColName("Rotate", idx, DataTable::VT_FLOAT, 0)->SetValAsFloat(rotate, -1);
  
  return true;
}

bool RetinaSpec::FilterImage(taImage& img, DataTable* dt,
			     float move_x, float move_y,
			     float scale, float rotate, bool superimpose)
{
  if (!dt) return false;
  float_Matrix img_data;
  taBase::Ref(img_data);	// make sure it isn't killed by some other ops..
  if(color_type == COLOR) {
    img.ImageToRGB_float(img_data);
  }
  else {
    img.ImageToGrey_float(img_data);
  }
  bool rval = FilterImageData(img_data, dt, move_x, move_y, scale, rotate, superimpose);
  int idx;
  if(rval)
    dt->FindMakeColName("Name", idx, DataTable::VT_STRING, 0)->SetValAsString(img.name, -1);
  return rval;
}

bool RetinaSpec::FilterImageName(const String& img_fname, DataTable* dt,
				 float move_x, float move_y,
				 float scale, float rotate, bool superimpose)
{
  if (!dt) return false;
  taImage img;
  if(!img.LoadImage(img_fname)) return false;
  return FilterImage(img, dt, move_x, move_y, scale, rotate, superimpose);
}

DoGRetinaSpec* RetinaSpec::FindRetinalRegion(RetinalSpacingSpec::Region reg) {
  for(int i=0;i<dogs.size;i++) {
    DoGRetinaSpec* fs = (DoGRetinaSpec*)dogs[i];
    if(fs->spacing.region == reg)
      return fs;
  }
  return NULL;
}

bool RetinaSpec::AttendRegion(DataTable* dt, RetinalSpacingSpec::Region region) {
  DoGRetinaSpec* fov_spec = FindRetinalRegion(region);
  if(!fov_spec) return false;

  float fov_x_pct = (float)fov_spec->spacing.input_size.x / (float)retina_size.x;
  float fov_y_pct = (float)fov_spec->spacing.input_size.y / (float)retina_size.y;
  float fov_pct = taMath_float::max(fov_x_pct, fov_y_pct);

  int idx;
  for(int i=0;i<dogs.size;i++) {
    DoGRetinaSpec* sp = dogs[i];
    DataArray_impl* da_on = dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
						sp->spacing.output_size.x, sp->spacing.output_size.y);
    DataArray_impl* da_off = dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT,
						 2, sp->spacing.output_size.x, sp->spacing.output_size.y);

    float_Matrix* on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
    float_Matrix* off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
    taBase::Ref(on_mat);
    taBase::Ref(off_mat);
    taImageProc::AttentionFilter(*on_mat, fov_pct);
    taImageProc::AttentionFilter(*off_mat, fov_pct);
    taBase::unRefDone(on_mat);
    taBase::unRefDone(off_mat);
  }
  return true;
}

// DJ: Two things I'd really like to do with this:
//   1: Remove image edge artifacts DIRECTLY in the filter output, like I do with attending
// RO: not sure what this means?

bool RetinaSpec::LookAtImageData(float_Matrix& img_data, DataTable* dt,
				  RetinalSpacingSpec::Region region,
				  float box_ll_x, float box_ll_y,
				  float box_ur_x, float box_ur_y,
				  float move_x, float move_y,
				  float scale, float rotate, 
				  bool superimpose, bool attend) {
  if(dogs.size == 0) return false;

  // find the fovea filter: one with smallest input_size
  DoGRetinaSpec* fov_spec = FindRetinalRegion(region);
  if(!fov_spec) return false;

  // translation: find the middle of the box
  FloatTwoDCoord obj_ctr((float) (0.5 * (float) (box_ll_x + box_ur_x)),
                       (float) (0.5 * (float) (box_ll_y + box_ur_y)));
  // convert into center-relative coords:
  FloatTwoDCoord obj_ctr_off = 2.0f * (obj_ctr - 0.5f);

  move_x -= obj_ctr_off.x;
  move_y -= obj_ctr_off.y;
  
  // now, scale the thing to fit in fov_spec->input_size
  TwoDCoord img_size(img_data.dim(0), img_data.dim(1));

  // height and width in pixels of box:
  float pix_x = (box_ur_x - box_ll_x) * img_size.x;
  float pix_y = (box_ur_y - box_ll_y) * img_size.y;

  // scale to fit within input size of filter or retina
  float sc_x = (float)fov_spec->spacing.input_size.x / pix_x;
  float sc_y = (float)fov_spec->spacing.input_size.y / pix_y;

  float fov_sc = MIN(sc_x, sc_y);
  scale *= fov_sc;
  if(scale > 100.0f)
    scale = 100.0f;
  if(scale < .01f)
    scale = .01f;

  bool rval = FilterImageData(img_data, dt, move_x, move_y, scale, rotate, superimpose);
  if(rval) {
    if(attend)
      AttendRegion(dt, region);
    int idx;
    float_Matrix* box_mat = (float_Matrix*)dt->FindMakeColName("LookBox", idx, DataTable::VT_FLOAT, 1, 4)->GetValAsMatrix(-1);
    taBase::Ref(box_mat);
    box_mat->FastEl(0) = box_ll_x; box_mat->FastEl(1) = box_ll_y;
    box_mat->FastEl(2) = box_ur_x; box_mat->FastEl(1) = box_ur_y;
    taBase::unRefDone(box_mat);
  }
  return rval;
}

bool RetinaSpec::LookAtImage(taImage& img, DataTable* dt,
			      RetinalSpacingSpec::Region region,
			      float box_ll_x, float box_ll_y,
			      float box_ur_x, float box_ur_y,
			      float move_x, float move_y,
			      float scale, float rotate, 
			      bool superimpose, bool attend) {
  float_Matrix img_data;
  taBase::Ref(img_data);	// make sure it isn't killed by some other ops..
  if(color_type == COLOR) {
    img.ImageToRGB_float(img_data);
  }
  else {
    img.ImageToGrey_float(img_data);
  }
  bool rval = LookAtImageData(img_data, dt, region, box_ll_x, box_ll_y, box_ur_x, box_ur_y,
			  move_x, move_y, scale, rotate, superimpose, attend);
  if(rval) {
    int idx;
    dt->FindMakeColName("Name", idx, DataTable::VT_STRING, 0)->SetValAsString(img.name, -1);
  }
  return rval;
}

bool RetinaSpec::LookAtImageName(const String& img_fname, DataTable* dt,
				  RetinalSpacingSpec::Region region,
				  float box_ll_x, float box_ll_y,
				  float box_ur_x, float box_ur_y,
				  float move_x, float move_y,
				  float scale, float rotate, 
				  bool superimpose, bool attend) {
  taImage img;
  if(!img.LoadImage(img_fname)) return false;
  return LookAtImage(img, dt, region, box_ll_x, box_ll_y, box_ur_x, box_ur_y,
		      move_x, move_y, scale, rotate, superimpose, attend);
}

///////////////////////////////////////////////////////////
// 		program stuff

void ImageProcCall::Initialize() {
  min_type = &TA_taImageProc;
  object_type = &TA_taImageProc;
}

