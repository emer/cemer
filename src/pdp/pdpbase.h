/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef base_h
#define base_h

// everything includes base.h, this is the minimal include

#include "ta_group.h"

#ifdef TA_GUI
#include "xform.h"
#endif

#include "pdpdeclare.h"
#include "tdgeometry.h"
#include "pdp_TA_type.h"




// all of these functions are cached out in pdpshell.cc

class pdpMisc {
  // #NO_TOKENS miscellaneous things for pdp
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  static PDPRoot*	root;		// root of pdp class hierarchy
  static char*		defaults_str;
  // string representation of basic defaults compiled into executable
  static String_Array	proj_to_load;	// list of projects to load
  static taBase_List	post_load_opr;  // #HIDDEN objects that need to have operations performed on them after loading
  static TypeDef*	def_wizard; 	// default network wizard type to create (this should be set by the specific application Main function)
  static void (*Init_Hook)();	 	// #READ_ONLY set this in user's main to init ta, etc.
  static float		pdpZScale; // amount by which to scale y dimension (inventor z) def is 4.0
  static float		pts_per_so_unit; // #DEF_36 equivalent font points per so unit
  static float		char_pts_per_so_unit; // #DEF_72 chars*pt size / so unit

  static int	Main(int argc, char* argv[]);
  // #IGNORE the pdp main startup function: call this from user's main
//NOTE: v4 waitprocs (aka event loop handling)
  static int	WaitProc();  		// waiting process
  static int	WaitProc_LoadProj(); // waiting process
  static int	WaitProc_PostLoadOpr(); // waiting process
  static void 	SaveRecoverFile(int err = 1);
  // error handling function that saves a recover file when system crashes
  static ColorScaleSpec* GetDefaultColor();  // gets the default color scale

  static const iColor* GetObjColor(Project* prj, TypeDef* typ);
  // get default object color (for edit dialogs and project view)

  static PDPLog* GetNewLog(Project* prj, TypeDef* typ);
  // get a temporary log of given type in given project (e.g., for pulling up a graph, etc)
  static Environment* GetNewEnv(Project* prj, TypeDef* typ = NULL);
  // get a temporary environment of given type in given project (e.g., for storing data)
  static Network* GetNewNetwork(Project* prj, TypeDef* typ = NULL);
  // get a new network object
  static Network* GetDefNetwork(Project* prj);
  // get default network from project

  static BaseSpec_MGroup* FindMakeSpecGp(Project* prj, const char* nm, bool& nw_itm = nw_itm_def_arg);
  // find a given spec group and if not found, make it
  static BaseSpec* FindMakeSpec(Project* prj, const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a given spec and if not found, make it
  static BaseSpec* FindSpecName(Project* prj, const char* nm);
  // find a given spec by name
  static BaseSpec* FindSpecType(Project* prj, TypeDef* td);
  // find a given spec by type

  static Process* FindMakeProc(Project* prj, const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a given process and if not found, make it
  static Process*  FindProcName(Project* prj, const char* nm);
  // find a process of given name
  static Process*  FindProcType(Project* prj, TypeDef* td);
  // find a process of given type

  static PDPLog* FindMakeLog(Project* prj, const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a given log and if not found, make it
  static PDPLog*  FindLogName(Project* prj, const char* nm);
  // find a log of given name
  static PDPLog*  FindLogType(Project* prj, TypeDef* td);
  // find a log of given type

#ifdef TA_GUI
  static SelectEdit* FindSelectEdit(Project* prj);
  // find a select edit if it exists
  static SelectEdit* FindMakeSelectEdit(Project* prj);
  // find a select edit if it exists, otherwise make it
#endif
#ifdef DMEM_COMPILE
  static int 	DMem_SubEventLoop(); 	// #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop
  static int	DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
#endif
};

int get_unique_file_number(int st_no, const char* prefix, const char* suffix);

//////////////////////////////////////////////////
//	PosGroup/MGroup: groups with positions	//
//////////////////////////////////////////////////

/*nn
#ifdef USE_TEMPLATE_GROUPS

#define PosGroup_of(T)							      \
class T ## _Group : public taGroup<T> {				      \
public:									      \
  TDCoord	pos;		 	     		      \
  void	Initialize() 		{ };					      \
  void 	Destroy()		{ };					      \
  void	InitLinks()		{ taGroup<T>::InitLinks(); taBase::Own(pos,this); }\
  void  Copy_(const T ## _Group& cp)		{ pos = cp.pos; }		      \
  void  Copy(const T ## _Group& cp)		{ taGroup<T>::Copy(cp); Copy_(cp); } \
  TA_BASEFUNS(T ## _Group);						      \
}

#define PosMGroup_of(T)							      	\
class T ## _MGroup : public MenuGroup<T> {				      	\
public:									      	\
  TDCoord	pos;			     			\
  void	Initialize() 		{ };					      	\
  void 	Destroy()		{ };					      	\
  void	InitLinks()		{ MenuGroup<T>::InitLinks(); taBase::Own(pos,this); }\
  void  Copy_(const T ## _MGroup& cp)	{ pos = cp.pos; }		        	\
  void  Copy(const T ## _MGroup& cp)	{ MenuGroup<T>::Copy(cp); Copy_(cp); } 		\
  TA_BASEFUNS(T ## _MGroup);						      	\
}

#else // USE_TEMPLATE_GROUPS
*/
class PosGroup : public taBase_Group { // ##NO_TOKENS
public:
  TDCoord	pos;		// Position of Group

  void	Initialize() 		{ };
  void 	Destroy()		{ };
  void	InitLinks()		{ taBase_Group::InitLinks(); taBase::Own(pos,this); }
  void  Copy_(const PosGroup& cp)	{ pos = cp.pos; }
  COPY_FUNS(PosGroup, taBase_Group);
  TA_BASEFUNS(PosGroup);
};

#define PosGroup_of(T)							      \
class T ## _Group : public PosGroup {					      \
public:									      \
  void	Initialize() 		{ SetBaseType(&TA_ ## T); }		      \
  void 	Destroy()		{ };					      \
  TA_BASEFUNS(T ## _Group);						      \
}

#define PosMGroup_of(T)							      	\
class T ## _MGroup : public taGroup<T> {				      	\
public:									      	\
  TDCoord	pos;			     			\
  void	Initialize() 		{ }					      	\
  void 	Destroy()		{ }					      	\
  void	InitLinks()		{ taGroup<T>::InitLinks(); taBase::Own(pos,this); }\
  void  Copy_(const T ## _MGroup& cp)	{ pos = cp.pos; }		        	\
  void  Copy(const T ## _MGroup& cp)	{ taGroup<T>::Copy(cp); Copy_(cp); } 		\
  TA_BASEFUNS(T ## _MGroup);						      	\
}


//#endif // USE_TEMPLATE_GROUPS


#endif // base_h

