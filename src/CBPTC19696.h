//   Copyright (C) 2015 by Jonathan Naylor G4KLX

// ulxd -- The universal reflector
// Copyright © 2021 Thomas A. Early N7TAE
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

class CBPTC19696
{
public:
	CBPTC19696();
	~CBPTC19696();

	void decode(const unsigned char* in, unsigned char* out);

	void encode(const unsigned char* in, unsigned char* out);

private:
	bool m_rawData[196];
	bool m_deInterData[196];

	void decodeExtractBinary(const unsigned char* in);
	void decodeErrorCheck();
	void decodeDeInterleave();
	void decodeExtractData(unsigned char* data) const;

	void encodeExtractData(const unsigned char* in);
	void encodeInterleave();
	void encodeErrorCheck();
	void encodeExtractBinary(unsigned char* data);
};
