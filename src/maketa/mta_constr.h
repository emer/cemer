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


// mta_constr.h : construct typea files from data

#ifndef mta_constr_h
#define mta_constr_h

#include "maketa.h"

// new type information construction system
// this includes the code to dump the information to the _TA.cc file
// _TA.cc must include this header, but libtypea does not need to include
// the code generation code (mta_constr.cc)

// the typedefs come first, and are initialized just as before,
// same with the stubs

// the member data is now in static data structures, that are then
// read in by the _init function.
// see typea_constr.h for those functions and the reading in functions..


//////////////////////////////////
// 	Link Resolution		//
//////////////////////////////////
// (called before generating types)

extern void TypeSpace_Generate_AddUndef(TypeSpace* ths, TypeSpace* trg, TypeSpace* dst);
extern void MemberSpace_Generate_AddUndef(MemberSpace* ths, TypeSpace* trg, TypeSpace* dst);
extern void MethodSpace_Generate_AddUndef(MethodSpace* ths, TypeSpace* trg, TypeSpace* dst);

extern void TypeSpace_Generate_LinkRefs(TypeSpace* ths, TypeSpace* dst);
extern void TypeDef_Generate_LinkRefs(TypeDef* ths, TypeSpace* trg, TypeSpace* dst);

//////////////////////////////////////////////////
//   	List Sorting: Parents Before Children	//
//////////////////////////////////////////////////
// (called before generating types)

extern bool TypeSpace_Sort_Order(TypeSpace* ths);
// returns true if a type was out of order, else otherwise

//////////////////////////////////
// 	Declarations		//
//////////////////////////////////
// (_TA_type.h _TA_inst.h files)

extern void TypeSpace_Declare_Types(TypeSpace* ths, ostream& strm,
				    const String_PArray& hv);
extern void TypeDef_Declare_Types(TypeDef* ths, ostream& strm);

extern void TypeSpace_Declare_Instances(TypeSpace* ths, ostream& strm,
					const String_PArray& hv);
extern void TypeDef_Declare_Instances(TypeDef* ths, ostream& strm);


//////////////////////////////////
// 	      Includes 		//
//////////////////////////////////

extern void TypeSpace_Includes(TypeSpace* ths, ostream& strm, const String_PArray& hv,
			       bool instances=false);


//////////////////////////////////
// 	  _TA.cc File		//
//////////////////////////////////

extern void TypeSpace_Generate(TypeSpace* ths, ostream& strm, const String_PArray& hv,
			       const String_PArray& ppfiles);


//////////////////////////////////
// 	  References		//
//////////////////////////////////

extern String TypeDef_Gen_Ref(TypeDef* ths);
extern String TypeDef_Gen_Ref_To(TypeDef* ths);
extern String TypeDef_Gen_Ref_Of(TypeDef* ths);

//////////////////////////////////
// 	TypeDef Constructors	//
//////////////////////////////////
// (part 1 of _TA.cc file)

extern void TypeSpace_Generate_Types(TypeSpace* ths, ostream& strm);
extern void TypeDef_FixOpts(String_PArray& op);
extern void TypeDef_Generate_Types(TypeDef* ths, ostream& strm);


//////////////////////////////////
//   Type Instances & stubs	//
//////////////////////////////////
// (part 2 of _TA.cc file)

extern void TypeSpace_Generate_Instances(TypeSpace* ths, ostream& strm);
extern void TypeDef_Generate_Instances(TypeDef* ths, ostream& strm);

extern void MethodSpace_Generate_Stubs(MethodSpace* ths, TypeDef* ownr, ostream& strm);
extern void MethodDef_InitTempArgVars(MethodDef* md, ostream& strm, int act_argc);
extern void MethodDef_AssgnTempArgVars(TypeDef* ownr, MethodDef* md, ostream& strm, int act_argc);
extern String MethodDef_GetCSSType(TypeDef* td);
extern void MethodDef_GenArgCast(MethodDef* md, TypeDef* argt, int j, ostream& strm);
extern void MethodDef_GenArgs(MethodDef* md, ostream& strm, int act_argc);
extern void MethodDef_GenStubName(TypeDef* ownr, MethodDef* md, ostream& strm);
extern void MethodDef_GenStubCall(TypeDef* ownr, MethodDef* md, ostream& strm);
extern void MethodDef_GenFunCall(TypeDef* ownr, MethodDef* md, ostream& strm,
				 int act_argc);



//////////////////////////////////
//   	      Type Data		//
//////////////////////////////////
// (part 3 of _TA.cc file)

extern void TypeSpace_Generate_Data(TypeSpace* ths, ostream& strm);
extern void TypeDef_Generate_Data(TypeDef* ths, ostream& strm);

// generates the two type fields (either a ptr to the type or a string descr)
extern String TypeDef_Generate_TypeFields(TypeDef* ths, TypeDef* ownr_ownr);


//////////////////////////////////
//   	     Enum Data		//
//////////////////////////////////

extern void TypeDef_Generate_EnumData(TypeDef* ths, ostream& strm);
extern void TypeDef_Init_EnumData(TypeDef* ths, ostream& strm);

extern void EnumSpace_Generate_Data(EnumSpace* ths, ostream& strm);


//////////////////////////////////
// 	   Member Data		//
//////////////////////////////////

extern void TypeDef_Generate_MemberData(TypeDef* ths, ostream& strm);
extern void TypeDef_Init_MemberData(TypeDef* ths, ostream& strm);

extern bool MemberSpace_Filter_Member(MemberSpace* ths, MemberDef* md);
extern void MemberSpace_Generate_Data(MemberSpace* ths, TypeDef* ownr, ostream& strm);


//////////////////////////////////
// 	   Method Data		//
//////////////////////////////////

extern void TypeDef_Generate_MethodData(TypeDef* ths, ostream& strm);
extern void TypeDef_Init_MethodData(TypeDef* ths, ostream& strm);

extern bool MethodSpace_Filter_Method(MethodSpace* ths, MethodDef* md);

extern void MethodSpace_Generate_ArgData(MethodSpace* ths, TypeDef* ownr, ostream& strm);
extern void MethodDef_Generate_ArgData(MethodDef* ths, TypeDef* ownr, ostream& strm);

extern void MethodSpace_Generate_Data(MethodSpace* ths, TypeDef* ownr, ostream& strm);


//////////////////////////////////
// 	  Init Function		//
//////////////////////////////////
// (part 4 of _TA.cc file)

extern void TypeSpace_Generate_Init(TypeSpace* ths, ostream& strm,
				    const String_PArray& ppfiles);

extern void TypeDef_Generate_Init(TypeDef* ths, ostream& strm);
extern void SubTypeSpace_Generate_Init(TypeSpace* ths, TypeDef* ownr, ostream& strm);
extern void TypeDef_Generate_AddParents(TypeDef* ths, char* typ_ref, ostream& strm);
extern void TypeDef_Generate_AddAllParents(TypeDef* ths, char* typ_ref, ostream& strm);
extern void TypeDef_Generate_AddOtherParents(TypeDef* ths, char* typ_ref, ostream& strm);




#endif // mta_constr_h
