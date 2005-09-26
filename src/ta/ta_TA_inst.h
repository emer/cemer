#ifndef ta_TA_insts_h
#define ta_TA_insts_h 1

// File Automatically Generated by MakeTA
// DO NOT EDIT


#include "ta_type.h"
#include "ta_def.h"
#include "ta_string.h"
#include "ta_stdef.h"
#include "ifont.h"
#include "ta_list.h"
#include "ta_filer.h"
#include "icolor.h"
#include "ta_type.h"
#include "ta_base.h"
#include "ta_matrix.h"
#include "ta_group.h"
#include "ta_dump.h"
#include "ta_defaults.h"
#include "ta_script.h"
#include "ta_qt.h"
#include "ta_qtdata_def.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qttype_def.h"
#include "ta_qttype.h"
#include "ta_qtclipdata.h"
#include "ta_qtbrowse_def.h"
#include "ta_qtviewer.h"
#include "ta_qtbrowse.h"
#include "ta_qtgroup.h"
#include "ta_seledit.h"
#include "igeometry.h"
#include "ta_TA_type.h"


extern	taBase*	TAI_taBase;
extern	taPtrList<taHashEl>*	TAI_taPtrList_taHashEl_;
extern	taPtrList<taHashBucket>*	TAI_taPtrList_taHashBucket_;
extern	taDataLinkItr*	TAI_taDataLinkItr;
extern	taBase**	TAI_TAPtr;
extern	taMisc*	TAI_taMisc;
extern	taPtrList<EnumDef>*	TAI_taPtrList_EnumDef_;
extern	EnumSpace*	TAI_EnumSpace;
extern	taPtrList<void>*	TAI_taPtrList_void_;
extern	TokenSpace*	TAI_TokenSpace;
extern	taPtrList<MemberDef>*	TAI_taPtrList_MemberDef_;
extern	MemberSpace*	TAI_MemberSpace;
extern	taPtrList<MethodDef>*	TAI_taPtrList_MethodDef_;
extern	MethodSpace*	TAI_MethodSpace;
extern	taPtrList<TypeDef>*	TAI_taPtrList_TypeDef_;
extern	TypeSpace*	TAI_TypeSpace;
extern	TypeItem*	TAI_TypeItem;
extern	EnumDef*	TAI_EnumDef;
extern	taOBase*	TAI_taOBase;
extern	taDataView*	TAI_taDataView;
extern	taNBase*	TAI_taNBase;
extern	SelectEdit*	TAI_SelectEdit;
extern	tabMisc*	TAI_tabMisc;
extern	taNBase**	TAI_TANPtr;
extern	taFBase*	TAI_taFBase;
extern	taPtrList<taBase>*	TAI_taPtrList_taBase_;
extern	taPtrList_base<taBase>*	TAI_taPtrList_base_taBase_;
extern	taList_impl*	TAI_taList_impl;
extern	taList<taBase>*	TAI_taList_taBase_;
extern	taBase_List*	TAI_taBase_List;
<<<<<<< .mine
=======
extern	int_Array*	TAI_int_Array;
extern	float_Array*	TAI_float_Array;
extern	double_Array*	TAI_double_Array;
extern	String_Array*	TAI_String_Array;
extern	SArg_Array*	TAI_SArg_Array;
extern	long_Array*	TAI_long_Array;
extern	voidptr_Array*	TAI_voidptr_Array;
>>>>>>> .r63
extern	taGroup_impl*	TAI_taGroup_impl;
extern	taList<taGroup_impl>*	TAI_taList_taGroup_impl_;
extern	TALOG*	TAI_TALOG;
extern	taSubGroup*	TAI_taSubGroup;
extern	DMemShareVar*	TAI_DMemShareVar;
extern	DMemShare*	TAI_DMemShare;
extern	taGroup<taBase>*	TAI_taGroup_taBase_;
extern	taBase_Group*	TAI_taBase_Group;
extern	taPtrList<VPUnref>*	TAI_taPtrList_VPUnref_;
extern	taPtrList<DumpPathSub>*	TAI_taPtrList_DumpPathSub_;
extern	taPtrList<DumpPathToken>*	TAI_taPtrList_DumpPathToken_;
extern	NameValue*	TAI_NameValue;
extern	TypeDefault*	TAI_TypeDefault;
extern	taPtrList<ScriptBase>*	TAI_taPtrList_ScriptBase_;
extern	Script*	TAI_Script;
extern	taGroup<Script>*	TAI_taGroup_Script_;
extern	Script_MGroup*	TAI_Script_MGroup;
extern	taiData*	TAI_taiData;
extern	taiMisc*	TAI_taiMisc;
extern	taPtrList<taiData>*	TAI_taPtrList_taiData_;
extern	taPtrList<taiMenuEl>*	TAI_taPtrList_taiMenuEl_;
extern	taPtrList<taiMenu>*	TAI_taPtrList_taiMenu_;
extern	Member_List*	TAI_Member_List;
extern	taiTypeBase*	TAI_taiTypeBase;
extern	taiViewType*	TAI_taiViewType;
extern	taiType*	TAI_taiType;
extern	taiMember*	TAI_taiMember;
extern	taiEdit*	TAI_taiEdit;
extern	taiMethod*	TAI_taiMethod;
extern	taiArgType*	TAI_taiArgType;
extern	taPtrList<taiType>*	TAI_taPtrList_taiType_;
extern	taiIntType*	TAI_taiIntType;
extern	taiEnumType*	TAI_taiEnumType;
extern	taiBoolType*	TAI_taiBoolType;
extern	taiClassType*	TAI_taiClassType;
extern	taiStringType*	TAI_taiStringType;
extern	gpiListType*	TAI_gpiListType;
extern	gpiGroupType*	TAI_gpiGroupType;
extern	gpiArray_Type*	TAI_gpiArray_Type;
extern	taiTokenPtrType*	TAI_taiTokenPtrType;
extern	taiTypePtr*	TAI_taiTypePtr;
extern	taiFilePtrType*	TAI_taiFilePtrType;
extern	taiDefaultEdit*	TAI_taiDefaultEdit;
extern	taiTokenPtrMember*	TAI_taiTokenPtrMember;
extern	taiDefaultToken*	TAI_taiDefaultToken;
extern	taiSubTokenPtrMember*	TAI_taiSubTokenPtrMember;
extern	taiTypePtrMember*	TAI_taiTypePtrMember;
extern	taiMemberDefPtrMember*	TAI_taiMemberDefPtrMember;
extern	taiFunPtrMember*	TAI_taiFunPtrMember;
extern	taiCondEditMember*	TAI_taiCondEditMember;
extern	taiTDefaultMember*	TAI_taiTDefaultMember;
extern	taiButtonMethod*	TAI_taiButtonMethod;
extern	taiMenuMethod*	TAI_taiMenuMethod;
extern	taiStreamArgType*	TAI_taiStreamArgType;
extern	taiBoolArgType*	TAI_taiBoolArgType;
extern	taiTokenPtrArgType*	TAI_taiTokenPtrArgType;
extern	taiTypePtrArgType*	TAI_taiTypePtrArgType;
extern	taiMemberPtrArgType*	TAI_taiMemberPtrArgType;
extern	taiMethodPtrArgType*	TAI_taiMethodPtrArgType;
extern	gpiDefaultEl*	TAI_gpiDefaultEl;
extern	gpiLinkGP*	TAI_gpiLinkGP;
extern	gpiLinkList*	TAI_gpiLinkList;
extern	gpiFromGpTokenPtrMember*	TAI_gpiFromGpTokenPtrMember;
extern	gpiTAPtrArgType*	TAI_gpiTAPtrArgType;
extern	gpiInObjArgType*	TAI_gpiInObjArgType;
extern	gpiFromGpArgType*	TAI_gpiFromGpArgType;
extern	gpiListEdit*	TAI_gpiListEdit;
extern	gpiGroupEdit*	TAI_gpiGroupEdit;
extern	gpiArrayEdit*	TAI_gpiArrayEdit;
extern	SArgEdit*	TAI_SArgEdit;
extern	gpiSelectEdit*	TAI_gpiSelectEdit;
extern	tabViewType*	TAI_tabViewType;
extern	tabOViewType*	TAI_tabOViewType;
extern	tabListViewType*	TAI_tabListViewType;
extern	tabGroupViewType*	TAI_tabGroupViewType;
extern	taPtrList<taiMimeItem>*	TAI_taPtrList_taiMimeItem_;
extern	WinGeometry*	TAI_WinGeometry;
extern	DataViewer*	TAI_DataViewer;
extern	taPtrList<ISelectable>*	TAI_taPtrList_ISelectable_;
extern	taList<DataViewer>*	TAI_taList_DataViewer_;
extern	DataViewer_List*	TAI_DataViewer_List;
extern	DataBrowser*	TAI_DataBrowser;
extern	taPtrList<gpiList_ElData>*	TAI_taPtrList_gpiList_ElData_;
extern	SelectEditConfig*	TAI_SelectEditConfig;
extern	iPoint*	TAI_iPoint;
extern	iVec3i*	TAI_iVec3i;
extern	iSize*	TAI_iSize;
extern	iRect*	TAI_iRect;
extern	iVec3f*	TAI_iVec3f;
extern	iBox3f*	TAI_iBox3f;


#endif // ta_TA_insts_h
