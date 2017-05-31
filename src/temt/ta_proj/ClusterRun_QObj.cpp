// Copyright 2017, Regents of the University of Colorado,
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

#include "ClusterRun_QObj.h"
#include <ClusterRun>
#include <taMisc>

void ClusterRun_QObj::UpdateEnabling(int panel_id) {
  master->UpdateUI();
}

void ClusterRun_QObj::SelectionChanged(const QModelIndex &index) {
  master->UpdateUI();
}

void ClusterRun_QObj::ReloadClusterTables() {
  master->Update_impl(false);
}


void ClusterRun_QObj::printError(const QString msg) {
  taMisc::Error("We encountered an error! ", msg);
}

void ClusterRun_QObj::printInfo(const QString msg) {
  taMisc::Info(msg);
}

void ClusterRun_QObj::promptCleanup() {
  if (taMisc::Choice("Clusterrun SVN repository is locked. Should we do a cleanup?", "Yes", "No") == 0) {
    master->Cleanup();
    master->Update();
  }
}

