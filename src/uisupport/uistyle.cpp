/***************************************************************************
 *   Copyright (C) 2005-08 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QApplication>

#include "uistyle.h"
#include "uistylesettings.h"

UiStyle::UiStyle(const QString &settingsKey) : _settingsKey(settingsKey) {
  // Default format
  _defaultPlainFormat.setForeground(QBrush("#000000"));
  _defaultPlainFormat.setFont(QFont("Monospace", QApplication::font().pointSize()));
  _defaultPlainFormat.font().setFixedPitch(true);
  _defaultPlainFormat.font().setStyleHint(QFont::TypeWriter);
  setFormat(None, _defaultPlainFormat, Settings::Default);
  
  // Load saved custom formats
  UiStyleSettings s(_settingsKey);
  foreach(FormatType type, s.availableFormats()) {
    _customFormats[type] = s.customFormat(type);
  }

  // Initialize color codes according to mIRC "standard"
  QStringList colors;
  //colors << "white" << "black" << "navy" << "green" << "red" << "maroon" << "purple" << "orange";
  //colors << "yellow" << "lime" << "teal" << "aqua" << "royalblue" << "fuchsia" << "grey" << "silver";
  colors << "#ffffff" << "#000000" << "#000080" << "#008000" << "#ff0000" << "#800000" << "#800080" << "#ffa500";
  colors << "#ffff00" << "#00ff00" << "#008080" << "#00ffff" << "#4169E1" << "#ff00ff" << "#808080" << "#c0c0c0";

  // Now initialize the mapping between FormatCodes and FormatTypes...
  _formatCodes["%O"] = None;
  _formatCodes["%B"] = Bold;
  _formatCodes["%S"] = Italic;
  _formatCodes["%U"] = Underline;
  _formatCodes["%R"] = Reverse;

  _formatCodes["%D0"] = PlainMsg;
  _formatCodes["%Dn"] = NoticeMsg;
  _formatCodes["%Ds"] = ServerMsg;
  _formatCodes["%De"] = ErrorMsg;
  _formatCodes["%Dj"] = JoinMsg;
  _formatCodes["%Dp"] = PartMsg;
  _formatCodes["%Dq"] = QuitMsg;
  _formatCodes["%Dk"] = KickMsg;
  _formatCodes["%Dr"] = RenameMsg;
  _formatCodes["%Dm"] = ModeMsg;
  _formatCodes["%Da"] = ActionMsg;

  _formatCodes["%DT"] = Timestamp;
  _formatCodes["%DS"] = Sender;
  _formatCodes["%DN"] = Nick;
  _formatCodes["%DH"] = Hostmask;
  _formatCodes["%DC"] = ChannelName;
  _formatCodes["%DM"] = ModeFlags;
  _formatCodes["%DU"] = Url;

  // Set color formats
  for(int i = 0; i < 16; i++) {
    QString idx = QString("%1").arg(i, (int)2, (int)10, (QChar)'0');
    _formatCodes[QString("%Dcf%1").arg(idx)] = (FormatType)(FgCol00 + i);
    _formatCodes[QString("%Dcb%1").arg(idx)] = (FormatType)(BgCol00 + i);
    QTextCharFormat fgf, bgf;
    fgf.setForeground(QBrush(QColor(colors[i]))); setFormat((FormatType)(FgCol00 + i), fgf, Settings::Default);
    bgf.setBackground(QBrush(QColor(colors[i]))); setFormat((FormatType)(BgCol00 + i), bgf, Settings::Default);
  }

  // Set a few more standard formats
  QTextCharFormat bold; bold.setFontWeight(QFont::Bold);
  setFormat(Bold, bold, Settings::Default);

  QTextCharFormat italic; italic.setFontItalic(true);
  setFormat(Italic, italic, Settings::Default);

  QTextCharFormat underline; underline.setFontUnderline(true);
  setFormat(Underline, underline, Settings::Default);

  // All other formats should be defined in derived classes.
}

UiStyle::~ UiStyle() {
  
}

void UiStyle::setFormat(FormatType ftype, QTextCharFormat fmt, Settings::Mode mode) {
  if(mode == Settings::Default) {
    _defaultFormats[ftype] = fmt;
  } else {
    UiStyleSettings s(_settingsKey);
    if(fmt != _defaultFormats[ftype]) {
      _customFormats[ftype] = fmt;
      s.setCustomFormat(ftype, fmt);
    } else {
      _customFormats[ftype] = QTextFormat().toCharFormat();
      s.removeCustomFormat(ftype);
    }
  }
}

QTextCharFormat UiStyle::format(FormatType ftype, Settings::Mode mode) const {
  if(mode == Settings::Custom && _customFormats.contains(ftype)) return _customFormats.value(ftype);
  else return _defaultFormats.value(ftype, QTextCharFormat());
}

// NOTE: This function is intimately tied to the values in FormatType. Don't change this
//       until you _really_ know what you do!
QTextCharFormat UiStyle::mergedFormat(quint32 ftype) {
  if(_cachedFormats.contains(ftype)) return _cachedFormats[ftype];
  if(ftype == Invalid) return QTextCharFormat();
  // Now we construct the merged format, starting with the default
  QTextCharFormat fmt = format(None);
  // First: general message format
  fmt.merge(format((FormatType)(ftype & 0x0f)));
  // now more specific ones
  for(quint32 mask = 0x0010; mask <= 0x2000; mask <<= 1) {
    if(ftype & mask) fmt.merge(format((FormatType)mask));
  }
  // color codes!
  if(ftype & 0x00400000) fmt.merge(format((FormatType)(ftype & 0x0f400000))); // foreground
  if(ftype & 0x00800000) fmt.merge(format((FormatType)(ftype & 0xf0800000))); // background
  // URL
  if(ftype & Url) fmt.merge(format(Url));
  return fmt;
}

UiStyle::FormatType UiStyle::formatType(const QString & code) const {
  if(_formatCodes.contains(code)) return _formatCodes.value(code);
  return Invalid;
}

QString UiStyle::formatCode(FormatType ftype) const {
  return _formatCodes.key(ftype);
}

// This method expects a well-formatted string, there is no error checking!
// Since we create those ourselves, we should be pretty safe that nobody does something crappy here.
UiStyle::StyledString UiStyle::styleString(const QString &s_) {
  QString s = s_;
  StyledString result;
  result.formats.append(qMakePair(0, (quint32)None));
  quint32 curfmt = (quint32)None;
  int pos = 0; int length = 0;
  for(;;) {
    pos = s.indexOf('%', pos);
    if(pos < 0) break;
    if(s[pos+1] == '%') { // escaped %, we just remove one and continue
      s.remove(pos, 1);
      pos++;
      continue;
    }
    if(s[pos+1] == 'D' && s[pos+2] == 'c') { // color code
      if(s[pos+3] == '-') {  // color off
        curfmt &= 0x003fffff;
        length = 4;
      } else {
        int color = 10 * s[pos+4].digitValue() + s[pos+5].digitValue();
        //TODO: use 99 as transparent color (re mirc color "standard")
        color &= 0x0f;
        if(pos+3 == 'f')
          curfmt |= (color << 24) | 0x00400000;
        else
          curfmt |= (color << 28) | 0x00800000;
        length = 6;
      }
    } else if(s[pos+1] == 'O') { // reset formatting
      curfmt &= 0x0000000f; // we keep message type-specific formatting
      length = 2;
    } else if(s[pos+1] == 'R') { // reverse
      // TODO: implement reverse formatting

      length = 2;
    } else { // all others are toggles
      QString code = QString("%") + s[pos+1];
      if(s[pos+1] == 'D') code += s[pos+2];
      FormatType ftype = formatType(code);
      if(ftype == Invalid) {
        qWarning(qPrintable(QString("Invalid format code in string: %1").arg(s)));
        continue;
      }
      curfmt ^= ftype;
      length = code.length();
    }
    s.remove(pos, length);
    if(pos == result.formats.last().first)
      result.formats.last().second = curfmt;
    else
      result.formats.append(qMakePair(pos, curfmt));
  }
  result.text = s;
  return result;
}
