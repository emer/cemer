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

#ifndef taDateTime_h
#define taDateTime_h 1

// parent includes:
#include <taNBase>

// member includes:
#if !defined(__MAKETA__)
# include <QDateTime>
# include <QTime>
#else
class QDateTime; // #IGNORE
class QTime; // #IGNORE
#endif

// declare all other types mentioned but not required to include:


TypeDef_Of(taDateTime);

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

#endif // taDateTime_h
