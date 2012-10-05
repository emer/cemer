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

#ifndef ta_time_h
#define ta_time_h

#include "ta_base.h"
#include "ta_TA_type_WRAPPER.h"

class TA_API taTime : public taBase {
  // #STEM_BASE ##NO_TOKENS #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Program raw time information
INHERITED(taBase)
public:
  double        usr;            // #HIDDEN user clock ticks -- time spent on this process
  double        sys;            // #HIDDEN system clock ticks -- time spent in the kernel on behalf of this process
  double        tot;            // total time ticks (all clock ticks on the CPU for all processes -- wall-clock time)

  void  operator += (const taTime& td)  { usr += td.usr; sys += td.sys; tot += td.tot; }
  void  operator -= (const taTime& td)  { usr -= td.usr; sys -= td.sys; tot -= td.tot; }
  void  operator *= (const taTime& td)  { usr *= td.usr; sys *= td.sys; tot *= td.tot; }
  void  operator /= (const taTime& td)  { usr /= td.usr; sys /= td.sys; tot /= td.tot; }
  taTime operator + (const taTime& td) const;
  taTime operator - (const taTime& td) const;
  taTime operator * (const taTime& td) const;
  taTime operator / (const taTime& td) const;

  double TicksToSecs(double ticks); // convert ticks to seconds

  double GetUsrSecs()           { return TicksToSecs(usr); }
  // get usr time as seconds and fractions thereof
  double GetSysSecs()           { return TicksToSecs(sys); }
  // get sys time as seconds and fractions thereof
  double GetTotSecs()           { return TicksToSecs(tot); }
  // get tot time as seconds and fractions thereof

  void  ZeroTime();     // zero out the times
  void  GetTime();      // get current clock timing information, for computing difference at later point in time
  String GetString(int len=15, int prec=7);
  // get string output as seconds and fractions of seconds, using given length and precision values for the time values

  TA_BASEFUNS(taTime);
private:
  void  Initialize();
  void  Destroy() { }
  void  Copy_(const taTime& cp) {usr = cp.usr; sys = cp.sys; tot = cp.tot;}
};

class TA_API TimeUsed : public taNBase {
  // #STEM_BASE #INLINE #INLINE_DUMP  ##CAT_Program computes amount of time used for a given process: start the timer at the start, then do EndTimer and it computes the amount used
INHERITED(taNBase)
public:
  taTime        start;          // #HIDDEN starting time
  taTime        end;            // #HIDDEN ending time
  taTime        used;           // #HIDDEN total time used
  double        s_used;         // #SHOW #GUI_READ_ONLY total number of seconds used
  int           n_used;         // #SHOW #GUI_READ_ONLY number of individual times the timer has been used without resetting accumulation

  virtual void  StartTimer(bool reset_used = true);
  // record the current time as the starting time, and optionally reset the time used information
  virtual void  EndTimer();     // record the current time as the ending time, and compute difference as the time used
  virtual void  ResetUsed();
  // reset time used information
  virtual String GetString();   // get string output of time used as seconds and fractions of seconds

  void  Initialize();
  void  Destroy()               { CutLinks(); }
  TA_SIMPLE_BASEFUNS(TimeUsed);
};

class TimeUsedHRd; // #IGNORE

class TA_API TimeUsedHR : public taNBase {
  // #STEM_BASE #INLINE #INLINE_DUMP  ##CAT_Program computes amount of time used (in high resolution) for a given process: start the timer at the start, then do EndTimer and it computes the amount used
INHERITED(taNBase)
public:
  double        s_used;         // #SHOW #GUI_READ_ONLY total number of seconds used
  int           n_used;         // #SHOW #GUI_READ_ONLY number of individual times the timer has been used without resetting accumulation

  virtual void  StartTimer(bool reset_used = true);
  // record the current time as the starting time, and optionally reset the time used information
  virtual void  EndTimer();     // record the current time as the ending time, and compute difference as the time used
  virtual void  ResetUsed();
  // reset time used information

  TA_BASEFUNS(TimeUsedHR);
protected:
  TimeUsedHRd*          d; // private impl data (depends on platform)
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const TimeUsedHR& cp);
};


#if !defined(__MAKETA__)
# include <QDateTime>
#else
class QDateTime; // #IGNORE
#endif

class TA_API taDateTime : public taNBase {
  // #STEM_BASE #INLINE ##CAT_Program represents date and time information -- for calendar management and date fields in data tables, etc (use int value conversion for secs since 1 Jan 1970 UTC)
INHERITED(taNBase)
public:
  int64_t       secs_1jan_1970; // internal storage unit: seconds since january 1st, 1970, UTC

#ifndef __MAKETA__
  QDateTime qDateTime() const { return QDateTime::fromTime_t((uint)secs_1jan_1970); }
  // #IGNORE
  void      fmQDateTime(const QDateTime& qdt)     { secs_1jan_1970 = qdt.toTime_t(); }
  // #IGNORE

  void operator=(const QDateTime& qdt) { fmQDateTime(qdt); }
  // #IGNORE

  QDate qdate() const { return qDateTime().date(); }
  // #IGNORE
  QTime qtime() const { return qDateTime().time(); }
  // #IGNORE

  void setDate(const QDate &date) { QDateTime qdt = qDateTime(); qdt.setDate(date); fmQDateTime(qdt); }
  // #IGNORE
  void setTime(const QTime &time) { QDateTime qdt = qDateTime(); qdt.setTime(time); fmQDateTime(qdt); }
  // #IGNORE
#endif

  bool isNull () const          { return secs_1jan_1970 == 0; }
  bool isValid () const         { return qDateTime().isValid(); }

  uint toTime_t() const { return (uint)secs_1jan_1970; }
  void setTime_t(uint secsSince1Jan1970UTC) { secs_1jan_1970 = (int64_t)secsSince1Jan1970UTC; }

  String toString(const String &format) const
  { return (String)qDateTime().toString(format); }
  // convert to a string representation: format is: d=day number, dd=d with leading zero, ddd=short day name, dddd=long day name, M=month number, MM=M with leading zero, MMM=short month name, MMMM=long month name, yy=2 digit year, yyyy=4 digit year, h=hour, hh=hour with leading zero, m=minute, mm=minute with leading zero, s=second, ss=leading zero, AP=AM or PM, ap=am or pm

  void fromString(const String &s, const String &format="");
  // convert to a string representation: if format is non-empty, format is: d=day number, dd=d with leading zero, ddd=short day name, dddd=long day name, M=month number, MM=M with leading zero, MMM=short month name, MMMM=long month name, yy=2 digit year, yyyy=4 digit year, h=hour, hh=hour with leading zero, m=minute, mm=minute with leading zero, s=second, ss=leading zero, AP=AM or PM, ap=am or pm

  void addSecs(int secs)        { fmQDateTime(qDateTime().addSecs(secs)); }
  void addMinutes(int mins)     { fmQDateTime(qDateTime().addSecs(mins * 60)); }
  void addHours(int hours)      { fmQDateTime(qDateTime().addSecs(hours * 3600)); }
  void addDays(int days)        { fmQDateTime(qDateTime().addDays(days)); }
  void addMonths(int months)    { fmQDateTime(qDateTime().addMonths(months)); }
  void addYears(int years)      { fmQDateTime(qDateTime().addYears(years)); }

  int second() const            { return qtime().second(); }
  int minute() const            { return qtime().minute(); }
  int hour() const              { return qtime().hour(); }
  int day() const               { return qdate().day(); }
  int month() const             { return qdate().month(); }
  int year() const              { return qdate().year(); }

  int dayOfWeek() const         { return qdate().dayOfWeek(); }
  int dayOfYear() const         { return qdate().dayOfYear(); }
  int daysInMonth() const       { return qdate().daysInMonth(); }
  int daysInYear() const        { return qdate().daysInYear(); }

  bool setDate(int year, int month, int day)
  { QDate dt; dt.setDate(year, month, day); setDate(dt); return isValid(); }
  void currentDateTime()        { fmQDateTime(QDateTime::currentDateTime()); }

  void toLocalTime()            { fmQDateTime(qDateTime().toLocalTime()); }
  void toUTC()                  { fmQDateTime(qDateTime().toUTC()); }

  int daysTo(const taDateTime &dt) const { return qDateTime().daysTo(dt.qDateTime()); }
  int secsTo(const taDateTime &dt) const { return qDateTime().secsTo(dt.qDateTime()); }

  bool operator==(const taDateTime &other) const { return qDateTime() == other.qDateTime(); }
  inline bool operator!=(const taDateTime &other) const { return !(*this == other); }
  bool operator<(const taDateTime &other) const { return qDateTime() < other.qDateTime(); }
  inline bool operator<=(const taDateTime &other) const { return !(other < *this); }
  inline bool operator>(const taDateTime &other) const { return other < *this; }
  inline bool operator>=(const taDateTime &other) const { return !(*this < other); }

  // static functions
  static String fmTimeToString(uint secsSince1Jan1970UTC, const String &format)
  { taDateTime dt; dt.setTime_t(secsSince1Jan1970UTC); return dt.toString(format); }
  // convert from an int time input to a formatted string output. format is: d=day number, dd=d with leading zero, ddd=short day name, dddd=long day name, M=month number, MM=M with leading zero, MMM=short month name, MMMM=long month name, yy=2 digit year, yyyy=4 digit year, h=hour, hh=hour with leading zero, m=minute, mm=minute with leading zero, s=second, ss=leading zero, AP=AM or PM, ap=am or pm
  static uint fmDateToTime_t(int year, int month, int day)
  { taDateTime dt; dt.setDate(year, month, day); return dt.toTime_t(); }
  // get a time_t value (secs since Jan 1, 1970) from a date
  static uint fmStringToTime_t(const String& s, const String& format = "")
  { taDateTime dt; dt.fromString(s, format); return dt.toTime_t(); }
  // get a time_t value (secs since Jan 1, 1970) from a date string: if format is empty, default conversion is used; format is: d=day number, dd=d with leading zero, ddd=short day name, dddd=long day name, M=month number, MM=M with leading zero, MMM=short month name, MMMM=long month name, yy=2 digit year, yyyy=4 digit year, h=hour, hh=hour with leading zero, m=minute, mm=minute with leading zero, s=second, ss=leading zero, AP=AM or PM, ap=am or pm
  static int daysBetween_Time_t(uint st_time, uint ed_time)
  { taDateTime sdt; sdt.setTime_t(st_time); taDateTime edt; edt.setTime_t(ed_time);
    return sdt.daysTo(edt); }
  // compute number of days between two time_t values (seconds since january 1st, 1970, UTC)

  static String longDayName(int weekday) { return QDate::longDayName(weekday); }
  static String longMonthName(int month) { return QDate::longMonthName(month); }
  static String shortDayName(int weekday) { return QDate::shortDayName(weekday); }
  static String shortMonthName(int month) { return QDate::shortMonthName(month); }

  TA_SIMPLE_BASEFUNS(taDateTime);
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
};


#endif // ta_time_h
