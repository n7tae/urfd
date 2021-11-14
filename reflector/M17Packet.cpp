//
//  Copyright © 2020 Thomas A. Early, N7TAE
//
// ----------------------------------------------------------------------------
//
//    m17ref is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    m17ref is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    with this software.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <arpa/inet.h>

#include "M17Packet.h"

CM17Packet::CM17Packet(const uint8_t *buf)
{
	memcpy(m17.magic, buf, sizeof(SM17Frame));

	destination.CodeIn(m17.lich.addr_dst);
	source.CodeIn(m17.lich.addr_src);
}

const CCallsign &CM17Packet::GetDestCallsign() const
{
	return destination;
}

const CCallsign &CM17Packet::GetSourceCallsign() const
{
	return source;
}

char CM17Packet::GetDestModule() const
{
	return destination.GetModule();
}

uint16_t CM17Packet::GetFrameNumber() const
{
	return ntohs(m17.framenumber);
}

uint16_t CM17Packet::GetFrameType() const
{
	return ntohs(m17.lich.frametype);
}

const uint8_t *CM17Packet::GetPayload() const
{
	return m17.payload;
}

const uint8_t *CM17Packet::GetNonce() const
{
	return m17.lich.nonce;
}

void CM17Packet::SetPayload(const uint8_t *newpayload)
{
	memcpy(m17.payload, newpayload, 16);
}

uint16_t CM17Packet::GetStreamId() const
{
	return ntohs(m17.streamid);
}

uint16_t CM17Packet::GetCRC() const
{
	return ntohs(m17.crc);
}

void CM17Packet::SetCRC(uint16_t crc)
{
	m17.crc = htons(crc);
}

std::unique_ptr<CM17Packet> CM17Packet::Duplicate(void) const
{
	return std::unique_ptr<CM17Packet>(new CM17Packet(*this));
}

bool CM17Packet::IsLastPacket() const
{
	return ((0x8000u & ntohs(m17.framenumber)) == 0x8000u);
}
