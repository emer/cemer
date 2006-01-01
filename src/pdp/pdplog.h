// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



#ifndef pdplog_h
#define pdplog_h

#include "process.h"
#include "datatable.h"
#include "datagraph.h"
#include "ta_filer.h"

#ifdef TA_GUI
//#include "fontspec.h"
#include "igeometry.h"
#endif

#include "pdp_TA_type.h"

// forwards this file

class PDPLog; //

// externals
//class T3DataViewer; //
//class pdpDataViewer; //

#ifdef TA_GUI
//////////////////////////
//   LogView_PtrList	//
//////////////////////////

class LogView_PtrList : public taPtrList<LogView> { // link list of associated logviews
#ifndef __MAKETA__
typedef taPtrList<LogView> inherited;
#endif
public:
  TypeDef*	el_typ; // actual type set by subclass of owner
  LogView_PtrList() {el_typ = &TA_LogView;}
};
#endif
/* NOTES:
  PDPLog was designed to operate in two or more modes: logfile, and not
  When operating with a log file, the log_lines variable contains total lines,
  and the buffer is a window into the file.
  When operating without a log file, the buffer is the only data source, and
  is shifted up when it overflows.

  To reconcile these for client objects (ex. LogViews), the virt_xxx property
  accessors were created. These provide an always-consisent interface, and should
  be used in new code.

  Here is the usage of the key variables operating with and w/o a log file:
  			w/logfile		w/o logfile
  log_lines		# lines (or #so far)	0
  data_range.min	min buffed line#	0 (always)
  data_range.max	max buffed line#	#lines - 1

  virt_lines		log_lines		#lines
*/

class PDPLog : public taNBase {
  // ##EXT_log Records data from processes, displaying and saving it
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  taFiler*	log_file;	// optional file for saving
  int		log_lines;	// #READ_ONLY #NO_SAVE number of lines in the log
  LogData	log_data;	// #NO_SAVE #HIDDEN our own log data for reading fm file

  DataTable	data;		// data for the log
  int		data_bufsz;	// #DETAIL how big a data buffer size to keep
  float		data_shift;	// #DETAIL percentage to shift buffer upon overflow
  MinMaxInt	data_range;	// #READ_ONLY #NO_SAVE #SHOW range of lines in the data buffer (in log lines)
  bool		record_proc_name; // whether to record process name in log file or not

  Process_Group log_proc;	// #LINK_GROUP #BROWSE processes which use this log
  SchedProcess*	cur_proc;	// #READ_ONLY #NO_SAVE current process sending to log

  String_Array	display_labels;	// ordered list of labels to use for views and log files
#ifdef TA_GUI
  TDCoord	pos;  // position of view
  TDCoord	geom;  // size of view
  LogView_PtrList	views; // #IGNORE various views (graphs, grids, etc.) defined on this log
#endif
  int		virt_lines(); // number of actual or log lines available
//nn  MinMaxInt&	virt_range(); // min::max range of actual or log lines available; DO NOT CACHE (only correct at time of access)

  virtual void 	GetHeaders(bool keep_display_settings = true);
  // #MENU #MENU_ON_Actions Get header information (list of data to display) from processes and clear out all current data (except save display info if checked)
  virtual void	SetSaveFile(const char* nm = NULL, bool no_dlg = false);
  // #MENU #MENU_ON_LogFile #ARGC_0 open file to save log data to (overwrite existing file)
  virtual void	SetAppendFile(const char* nm = NULL, bool no_dlg = false);
  // #MENU #ARGC_0 open file to append log data to
  virtual void	LoadFile(const char* nm = NULL, bool no_dlg = false);
  // #MENU #ARGC_0 read in existing log file data
  virtual void	CloseFile();
  // #MENU #MENU_SEP_AFTER close any open files being logged to
  virtual void	BufferToFile(const char* nm = NULL, bool no_dlg = false);
  // #MENU #ARGC_0 send current buffer of data to file: if null args and already open, to it, else opens a new file sends header and closes after

  virtual void	HeadToFile();
  // send current header to currently open log file -- if not already open, quietly fails

  virtual void 	Buffer_F();	// forward by data_shift
  virtual void 	Buffer_R();	// rewind by data_shift
  virtual void 	Buffer_FF();	// forward to end
  virtual void 	Buffer_FR();	// rewind to begining
  virtual void	Buffer_SeekForView(const MinMaxInt& view_range);
    // makes sure buffer contains data for view_range

  virtual void	Clear();	// clears out the data
  virtual void	UpdateViewHeaders();           // update headers for all views

  virtual void	AddUpdater(SchedProcess* updt_proc);
  /* #NEW_FUN #MENU #MENU_ON_Object #MENU_SEP_BEFORE
     Tell updt_proc to update this log with new log data as the process runs */
  virtual void	RemoveUpdater(SchedProcess* updt_proc);
  // #MENU #MENU_ON_Object #FROM_GROUP_log_proc remove given updating process
  virtual void	RemoveAllUpdaters(); 		// remove this from all process logs
  virtual void	SyncLogViewUpdaters(); 		// #IGNORE
/*TODO: obs, replace
  // for processes communicating stuff to the log (and log acting on this)
  virtual void  NewData(LogData& ld, SchedProcess* sproc);
  // This is the primary call to make when sending data to the log
  virtual void	NewHead(LogData& ld, SchedProcess* sproc);
  // #IGNORE use this to specifically update the field headers without updating data
  virtual void	HeadToLogFile(LogData& ld);	// #IGNORE
  virtual void	DataToLogFile(LogData& ld);	// #IGNORE
  virtual void	HeadToBuffer(LogData& ld);	// #IGNORE
  virtual void	DataToBuffer(LogData& ld);	// #IGNORE
*/
  // for reading log files from disk to buffer
  virtual int 	LogLineType(char* lnc);		// #IGNORE determine type of given line
  virtual int  	FileScanTo(istream& strm, int log_ln); // #IGNORE scan to log_ln no
  virtual int	GetFileLength();		// #IGNORE gets length of current file
  virtual void	HeadFromLogFile(String& hdln);	// #IGNORE put log header into buffer
  virtual void	DataFromLogFile(String& dtln);	// #IGNORE put log data into buffer
  virtual void	ReadNewLogFile();		// #IGNORE a new log file was opened, read it
  virtual void	LogFileToBuffer();		// #IGNORE read current data_range into buffer
  virtual void 	InitFile();			// #IGNORE initialize log_file object

  static ostream& LogColumn(ostream& strm, String& str, int tabs);
  // #IGNORE output one column of data (string str), tabs is width (1 or 2)

  // for maintaining an internal version of the buffer format in log-data terms
  virtual DataItem* DataItemFromDataArray(DataArray_impl* da); 	   // #IGNORE
  virtual void 	LogDataFromDataTable(DataTable* dt, int st_idx=0); // #IGNORE
  virtual void	LogDataFromBuffer();	// #IGNORE get log data records from current buffer

  // functions for manually updating logs in ad-hoc manner
  virtual void	ViewAllData();		// make views display all available data

  int	Dump_Save_Value(ostream& strm, TAPtr par, int indent); // check for open log_file..
#ifdef TA_GUI
  TypeDef*	GetDefaultView()	{ return views.el_typ; }
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_PDPLog); }
  virtual LogView* NewLogView();// return correct type of logview
  virtual void	ShowInViewer(T3DataViewer* vw = NULL); // #NULL_OK #MENU #MENU_ON_Object #MENU_SEP_BEFORE #MENU_CONTEXT Show this log in a new or existing T3 Viewer
#endif
  void	UpdateAfterEdit();
  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const PDPLog& cp);
  COPY_FUNS(PDPLog, taNBase);
  TA_BASEFUNS(PDPLog);
protected:
//nn  MinMaxInt		m_virt_range; // #IGNORE latest updated value provided via virt_range() prop
  override void		DataViewAdding(taDataView* dv); // adds to views list if a logview
  override void		DataViewRemoving(taDataView* dv); //  removes fromviews list if a logview
};

BaseGroup_of(PDPLog);

// note: PDPLog_MGroup name is for compatiblity with v3.2 files
class PDPLog_MGroup : public taGroup<PDPLog> {
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  virtual PDPLog* FindMakeLog(const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_PDPLog); }
#endif
  void	Initialize() 		{SetBaseType(&TA_PDPLog);}
  void 	Destroy()		{ }
  TA_BASEFUNS(PDPLog_MGroup);
};

class TextLog : public PDPLog {
  // log with textview as a default display
public:
#ifdef TA_GUI
  void	Initialize() 		{views.el_typ = &TA_TextLogView; };
#else
  void	Initialize() {}
#endif
  void 	Destroy()		{ };
  TA_BASEFUNS(TextLog);
};

class GridLog : public TextLog {
  // log with grid as a default display
public:
#ifdef TA_GUI
  void	Initialize() 		{views.el_typ = &TA_GridLogView;};
#else
  void	Initialize() {}
#endif
  void 	Destroy()		{ };
  TA_BASEFUNS(GridLog);
};

class NetLog: public TextLog {
  // log with netlog as a default display
public:
  virtual void	SetNetwork(Network* net);
  // #MENU #MENU_ON_Object select given network as the one to update views on

#ifdef TA_GUI
  void	Initialize() 		{ views.el_typ = &TA_NetLogView;};
#else
  void	Initialize() {}
#endif
  void 	Destroy()		{ };
  TA_BASEFUNS(NetLog);
};

class GraphLog : public PDPLog {
  // log with graph as a default display
public:
#ifdef TA_GUI
  void	Initialize() 		{views.el_typ = &TA_GraphLogView;};
#else
  void	Initialize() {}
#endif
  void 	Destroy()		{ };
  TA_BASEFUNS(GraphLog);
};


#endif // pdplog.h

