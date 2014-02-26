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

#include "taiMiscCore.h"
#include <QCoreApplication>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taThreadMgr>
#include <QTimer>
#include <dumpMisc>
#include <taiTypeBase>
#include <ClusterRun>

#include <signal.h>

extern "C" {
  extern int rl_done;
}

#ifndef NO_TA_BASE
TA_API taiMiscCore* taiMC_ = NULL;

int taiMiscCore::rl_callback() {
  QCoreApplication* app = QCoreApplication::instance();
  if (taMisc::quitting) goto quit_exit;
  if (app)
    app->processEvents();
  if (taMisc::quitting) goto quit_exit;
  return 0; // ???

quit_exit:
  rl_done = 1;
  return 0;
}

taiMiscCore* taiMiscCore::New(QObject* parent) {
  taiMiscCore* rval = new taiMiscCore(parent);
  rval->Init();
  return rval;
}

void taiMiscCore::Quit(CancelOp cancel_op) {
  static bool is_quitting = false;
  if(is_quitting) return;
  // good place to save config, regardless what happens
  is_quitting = true;
  if (tabMisc::root && taMisc::interactive) {
    tabMisc::root->Save();
  }

  taMisc::quitting = (cancel_op == CO_NOT_CANCELLABLE) ?
    taMisc::QF_FORCE_QUIT : taMisc::QF_USER_QUIT;
  OnQuitting(cancel_op); // saves changes
  if (cancel_op != CO_CANCEL) {
#ifdef DMEM_COMPILE
    MPI_Finalize();
#endif
    taThreadMgr::TerminateAllThreads(); // don't leave any active threads lying around
    tabMisc::WaitProc_Cleanup();
    if (taiMC_) {
      taiMC_->Quit_impl(cancel_op);
    }
  }
  if (cancel_op == CO_CANCEL) {
    taMisc::quitting = taMisc::QF_RUNNING;
    is_quitting = false;
  }
}

void taiMiscCore::OnQuitting(CancelOp& cancel_op) {
  if (taiMC_) {
    taiMC_->OnQuitting_impl(cancel_op);
  }
  if (cancel_op == CO_CANCEL) {
    taMisc::quitting = taMisc::QF_RUNNING;
  }
}

int taiMiscCore::ProcessEvents() {
  if (taMisc::in_event_loop) {
    QCoreApplication::processEvents();
  }
  return 0;
}

int taiMiscCore::RunPending() {
  if (taMisc::in_event_loop) {
    if(QCoreApplication::hasPendingEvents()) {
      QCoreApplication::processEvents();
      return true;
    }
  }
  return false;
}

void taiMiscCore::WaitProc() {
  ClusterRun::WaitProcAutoUpdate();
  if(!taMisc::do_wait_proc && taMisc::err_cancel) { // only count if not calling back
    taMisc::err_waitproc_cnt++;
    if(taMisc::err_waitproc_cnt > taMisc::err_waitproc_thr) { // over threshold, cancel cancel
      taMisc::err_cancel = false;
      taMisc::err_waitproc_cnt = 0;
    }
  }
  if(!taMisc::do_wait_proc) return;
  taMisc::do_wait_proc = false; // reset at the START so other waitproc guys can get on the list from within the current waitproc
  tabMisc::WaitProc();
}

taiMiscCore::taiMiscCore(QObject* parent)
:inherited(parent)
{
  timer = NULL;
}

taiMiscCore::~taiMiscCore() {
  if (taiMC_ == this)
    taiMC_ = NULL;
}

void taiMiscCore::app_aboutToQuit() {
//NOTE: Qt will not process any more events at this point!
  if (timer)
    timer->stop();
  // do wait proc, to do final deferred deletes
  WaitProc();
}

void taiMiscCore::CheckConfigResult_(bool ok) {
//note: only called if !quiet, and if !ok only if confirm_success
  if (ok) {
    taMisc::Warning("No configuration errors were found.");
  }
  else {
    taMisc::ConsoleOutput("/n/n", true, false); // helps group this block together
    taMisc::Warning("Configuration errors were found:\n");
    taMisc::ConsoleOutput("taMisc::last_check_msg", true, false);
    // helps group this block together
  }
}

const String taiMiscCore::classname() {
  return String(QCoreApplication::instance()->applicationName());
}

void taiMiscCore::customEvent(QEvent* ev) {
  if ((int)ev->type() == (int)CE_QUIT)
    Quit(CO_NOT_CANCELLABLE);
  else inherited::customEvent(ev);
}

int taiMiscCore::Exec() {
  if (taMisc::in_event_loop) {
    taMisc::Error("Attempt to enter event loop a second time!");
    return 0;
  }

  // does idle processing in Qt
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  timer->start(taMisc::wait_proc_delay);
  // need a number here to prevent constant busy loop running
  // this must not have a number in it, so that it happens only
  // when other events are DONE!

  taMisc::in_event_loop = true;
  int rval = 0;
  try {
    rval = Exec_impl();
  }
  catch(...) {
    taMisc::Error("caught ... general exception in taiMiscCore::Exec() -- this is a programmer error -- please report bug and specify what you were doing just prior to this message -- thanks!");
    taMisc::in_event_loop = false;
    raise(SIGABRT);
  }
  taMisc::in_event_loop = false;
  return rval;
}

int taiMiscCore::Exec_impl() {
  return QCoreApplication::instance()->exec();
}

void taiMiscCore::Init(bool gui) {
  taMisc::gui_active = gui;

  connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()),
    this, SLOT(app_aboutToQuit()) );
}

void taiMiscCore::OnQuitting_impl(CancelOp& cancel_op) {
  // nothing in nongui
}

void taiMiscCore::PostUpdateAfter() {
#ifndef NO_TA_BASE
  dumpMisc::PostUpdateAfter();
#endif
}

void taiMiscCore::Quit_impl(CancelOp cancel_op) {
  // only for nogui
  QCoreApplication::instance()->quit();
}

void taiMiscCore::timer_timeout() {
  if (taMisc::WaitProc)
    taMisc::WaitProc();
}

#endif // NO_TA_BASE
