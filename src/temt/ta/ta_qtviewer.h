// Copyright, 1995-2007, Regents of the University of Colorado,
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

// ta_qtviewer.h -- gui classes for main window viewing

#ifndef TA_QTVIEWER_H
#define TA_QTVIEWER_H

// #include "ta_viewer.h"
// #include "ta_qtdata.h"
// #include "ta_datatable.h"


// #ifndef __MAKETA__
// # include <QAction>
// # include <QDockWidget>
// # include <QFrame>
// # include <QMainWindow>
// # include <QObject>
// # include <QPalette>
// # include <QIcon>
// # include <QList>
// # include <QTabBar>
// # include <QTreeWidget>
// # include <QTreeWidgetItem>
// # include <QToolBar>
// # include <QWebView>
// # include <QTime>

// # include "icliptoolwidget.h"
// # include "itreewidget.h"
// #endif


// // (new) forwards this file:
// class DataLink_QObj;
// class taiDataLink;
// class tabDataLink;
// class tabListDataLink;
// class tabGroupDataLink;
// class ISelectable;
// class ISelectable_PtrList;
// class ISelectableHost;
// class iBrowseViewer;
// class iBaseClipWidgetAction;
// class iMainWindowViewer;
// class iTreeView;
// class iTreeViewItem;
// class iTreeSearch;
// class taiListDataNode;
// class taiTreeDataNode;
// class tabTreeDataNode;
// class tabParTreeDataNode;
// class tabDefChildTreeDataNode;
// class tabListTreeDataNode;
// class tabGroupTreeDataNode;
// class iDataPanel;
// class iDataPanelFrame;
// class iViewPanelSet;
// class iListDataPanel;
// class iSearchDialog;
// class taiEditDataHost;

// // externals
// class taGroup_impl;
// class taProject;


// class QProgressBar;             // #IGNORE

//class TA_API DataLink_QObj: public QObject {

//class TA_API taiDataLink: public taDataLink { // interface for viewing system

//class TA_API tabDataLink: public taiDataLink { // DataLink for taBase objects

//class TA_API tabODataLink: public tabDataLink { // DataLink for taOBase objects

//class TA_API tabListDataLink: public tabODataLink {

//class TA_API tabGroupDataLink: public tabListDataLink {

//class TA_API ISelectable: public virtual IDataLinkProxy { //

//class TA_API IObjectSelectable: public ISelectable { // specialized for taBase object items

//class TA_API ISelectable_PtrList: public taPtrList<ISelectable> { // for selection lists

//class TA_API DynMethod_PtrList: public taPtrList<DynMethodDesc> { // #IGNORE

//class SelectableHostHelper;

//class TA_API ISelectableHost { // interface on the controlling widget hosting ISelectable items

//class TA_API SelectableHostHelper: public QObject {

//class TA_API iFrameViewer: public QWidget, public IDataViewWidget {

//class TA_API iBrowseViewer : public iFrameViewer { // base of viewer window used for 

//class TA_API iTabViewer : public iFrameViewer { // viewer window used for tabbed window

//class TA_API iDockViewer: public QDockWidget, public IDataViewWidget {

//class TA_API iToolBoxDockViewer: public iDockViewer {

//class TA_API iToolBar_List: public taPtrList<iToolBar> {

//class TA_API iApplicationToolBar: public iToolBar {

//class TA_API iBaseClipWidgetAction: public iClipWidgetAction {

//class TA_API iMainWindowViewer: public QMainWindow, public IDataViewWidget {

//class TA_API iTabBarBase : public QTabBar {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base tab bar with ctrl+f/b navigation

//class TA_API iTabBar: public iTabBarBase { //  encapsulates the TabBar for iTabView

//class TA_API iDataPanel_PtrList: public taPtrList<iDataPanel> { // ##NO_INSTANCE 

//class TA_API iTabView: public QWidget {

//class TA_API iTabView_PtrList: public taPtrList<iTabView> {

//class TA_API iDataPanel: public QFrame, public IDataLinkClient {

//class TA_API iDataPanelFrame: public iDataPanel {

//class TA_API iViewPanelFrame: public iDataPanel, public virtual IDataHost {

//class TA_API iDataPanelSetBase: public iDataPanel { //  common subclass for 

//class TA_API iDataPanelSet: public iDataPanelSetBase { //  contains 0 or more sub-data-panels, and a small control bar for selecting panels

//class TA_API iViewPanelSet: public iDataPanelSetBase { //  contains 0 or more sub-view-panels, and btm hor tab for selecting panels

//class TA_API iListDataPanel: public iDataPanelFrame {

//class TA_API iTextDataPanel: public iDataPanelFrame {

//class TA_API iDocDataPanel: public iDataPanelFrame {

//class TA_API iTreeView: public iTreeWidget, public ISelectableHost {

//class TA_API iTreeViewItem: public iTreeWidgetItem,

//class TA_API taiListDataNode: public iTreeViewItem {

//class TA_API taiTreeDataNode: public iTreeViewItem {

//class TA_API tabTreeDataNode: public taiTreeDataNode {

//class TA_API tabParTreeDataNode: public tabTreeDataNode {

//class TA_API tabDefChildRef: protected IDataLinkClient { // ##NO_INSTANCE ##NO_TOKENS 

//class TA_API tabDefChildTreeDataNode: public tabParTreeDataNode {

//class TA_API tabListTreeDataNode: public tabParTreeDataNode {

//class TA_API tabGroupTreeDataNode: public tabListTreeDataNode {

//class TA_API iTreeSearch: public QWidget {

//class TA_API iSearchDialog: public QDialog, public virtual IDataLinkClient {

//class TA_API iHelpBrowser: public QMainWindow {

#endif // TA_QTVIEWER_H
