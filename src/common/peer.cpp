/***************************************************************************
 *   Copyright (C) 2005-2015 by the Quassel Project                        *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "peer.h"

Peer::Peer(AuthHandler *authHandler, QObject *parent)
    : QObject(parent)
    , _authHandler(authHandler)
{

}


AuthHandler *Peer::authHandler() const
{
    return _authHandler;
}


// PeerPtr is used in RPC signatures for enabling receivers to send replies
// to a particular peer rather than broadcast to all connected ones.
// To enable this, the SignalProxy transparently replaces the bogus value
// received over the network with the actual address of the local Peer
// instance. Because the actual value isn't needed on the wire, it is
// serialized as null.
QDataStream &operator<<(QDataStream &out, PeerPtr ptr)
{
    Q_UNUSED(ptr);
    out << static_cast<quint64>(0);  // 64 bit for historic reasons
    return out;
}

QDataStream &operator>>(QDataStream &in, PeerPtr &ptr)
{
    ptr = nullptr;
    quint64 value;
    in >> value;
    return in;
}
