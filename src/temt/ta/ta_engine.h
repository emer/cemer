// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

// ta_engine.h -- execution engines

#ifndef TA_ENGINE_H
#define TA_ENGINE_H

#include "ta_group.h"
#include "ta_time.h"
#include "ta_datatable.h"
#include "ta_thread.h" 

#include "ta_def.h"
#include "ta_TA_type.h"

// external refs

// forwards
class taEngine;
TA_SMART_PTRS(taEngine);
class taEngine_Group;
class taEngineInst;

class TA_API taEngineInst: public taOBase { // ##NO_INSTANCE #VIRT_BASE ##NO_TOKENS extensible runtime-only structure that contains client-global data and tasks organized for efficient access by runtime Engines
INHERITED(taOBase)
public:
  taEngineRef		m_engine;
  taTask_List		tasks; // #NO_SAVE -- set tasks with setTaskCount
  DataTableRef		log_table; // log table, if set
  bool			use_log; // copied from engine
  int			max_rows; // stop logging after this many rows (-1=unlimited)
  
  inline int		taskCount() const {return tasks.size;} // #NO_SHOW
  virtual void		setTaskCount(int val);
  taTask*		task(int i) {return tasks.FastEl(i);} // #NO_SHOW
  
  virtual void		AssertLogTable(); // call at some point, before logging
  void			WriteLogRecord();
  
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS_NOCOPY(taEngineInst);
protected:
  virtual void		WriteLogRecord_impl() {}

private:
  void	Initialize();
  void	Destroy();
};

class TA_API taEngine: public taNBase {
  // ##TOKENS ##INSTANCE #VIRT_BASE ##DEF_NAME_STYLE_2 an object that manages XxxInst instances of a specific computing engine
INHERITED(taNBase)
public:
  bool			use_log; // use a log table (name=engine name) to log performance stats
  int			max_rows; // stop logging after this many rows (-1=unlimited)
  
  taEngineInst*		MakeEngineInst() const {return MakeEngineInst_impl();}
  
  TA_BASEFUNS_NOCOPY(taEngine);
protected:
  virtual taEngineInst*	MakeEngineInst_impl() const;
  virtual taEngineInst*	NewEngineInst_impl() const {return NULL;} // override to make exact type
private:
  void	Initialize();
  void	Destroy();
};


class TA_API taEngine_Group : public taGroup<taEngine> {
  // #CAT_Task a collection of engines -- there is usually a separate group for each family of engines
INHERITED(taGroup<taEngine>)
public:
  override String 	GetTypeDecoKey() const { return "Engine"; }

  TA_BASEFUNS(taEngine_Group);

private:
  void	Copy_(const taEngine_Group& cp) {}
  void	Initialize() {}
  void 	Destroy() {Reset(); };
};

#endif
