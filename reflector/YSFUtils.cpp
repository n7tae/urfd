//  Copyright © 2019 Jean-Luc Deltombe (LX3JL). All rights reserved.
//  Copyright (C) 2016,2017 by Jonathan Naylor G4KLX
//  Copyright (C) 2018 by Andy Uribe CA6JAU
//  Copyright (C) 2018 by Manuel Sanchez EA7EE

// urfd -- The universal reflector
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

#include <string.h>
#include "YSFDefines.h"
#include "YSFUtils.h"
#include "Golay24128.h"

void CYsfUtils::DecodeVD2Vchs(uint8_t *data, uint8_t **ambe)
{
	int frame = 0;

	data += YSF_SYNC_LENGTH_BYTES + YSF_FICH_LENGTH_BYTES;

	unsigned int offset = 40U; // DCH(0)

	// We have a total of 5 VCH sections, iterate through each
	for (unsigned int j = 0U; j < 5U; j++, offset += 144U)
	{

		unsigned char vch[13U];
		unsigned int dat_a = 0U;
		unsigned int dat_b = 0U;
		unsigned int dat_c = 0U;

		// Deinterleave
		for (unsigned int i = 0U; i < 104U; i++)
		{
			unsigned int n = INTERLEAVE_TABLE_26_4[i];
			bool s = READ_BIT(data, offset + n);
			WRITE_BIT(vch, i, s);
		}

		// "Un-whiten" (descramble)
		for (unsigned int i = 0U; i < 13U; i++)
			vch[i] ^= WHITENING_DATA[i];

		for (unsigned int i = 0U; i < 12U; i++)
		{
			dat_a <<= 1U;
			if (READ_BIT(vch, 3U*i + 1U))
				dat_a |= 0x01U;
		}

		for (unsigned int i = 0U; i < 12U; i++)
		{
			dat_b <<= 1U;
			if (READ_BIT(vch, 3U*(i + 12U) + 1U))
				dat_b |= 0x01U;
		}

		for (unsigned int i = 0U; i < 3U; i++)
		{
			dat_c <<= 1U;
			if (READ_BIT(vch, 3U*(i + 24U) + 1U))
				dat_c |= 0x01U;
		}

		for (unsigned int i = 0U; i < 22U; i++)
		{
			dat_c <<= 1U;
			if (READ_BIT(vch, i + 81U))
				dat_c |= 0x01U;
		}

		// convert to ambe2plus
		unsigned char v_dmr[9U];

		unsigned int a = CGolay24128::encode24128(dat_a);
		unsigned int p = PRNG_TABLE[dat_a] >> 1;
		unsigned int b = CGolay24128::encode23127(dat_b) >> 1;
		b ^= p;

		unsigned int MASK = 0x800000U;
		for (unsigned int i = 0U; i < 24U; i++, MASK >>= 1)
		{
			unsigned int aPos = DMR_A_TABLE[i];
			WRITE_BIT(v_dmr, aPos, a & MASK);
		}

		MASK = 0x400000U;
		for (unsigned int i = 0U; i < 23U; i++, MASK >>= 1)
		{
			unsigned int bPos = DMR_B_TABLE[i];
			WRITE_BIT(v_dmr, bPos, b & MASK);
		}

		MASK = 0x1000000U;
		for (unsigned int i = 0U; i < 25U; i++, MASK >>= 1)
		{
			unsigned int cPos = DMR_C_TABLE[i];
			WRITE_BIT(v_dmr, cPos, dat_c & MASK);
		}

		memcpy(ambe[frame++], v_dmr, 9);
	}
}

void CYsfUtils::EncodeVD2Vch(uint8_t *ambe, uint8_t *data)
{
	// convert from ambe2plus
	unsigned int a = 0U;
	unsigned int MASK = 0x800000U;
	for (unsigned int i = 0U; i < 24U; i++, MASK >>= 1)
	{
		unsigned int aPos = DMR_A_TABLE[i];

		if (READ_BIT(ambe, aPos))
			a |= MASK;
	}

	unsigned int b = 0U;
	MASK = 0x400000U;
	for (unsigned int i = 0U; i < 23U; i++, MASK >>= 1)
	{
		unsigned int bPos = DMR_B_TABLE[i];

		if (READ_BIT(ambe, bPos))
			b |= MASK;
	}

	unsigned int dat_c = 0U;
	MASK = 0x1000000U;
	for (unsigned int i = 0U; i < 25U; i++, MASK >>= 1)
	{
		unsigned int cPos = DMR_C_TABLE[i];

		if (READ_BIT(ambe, cPos))
			dat_c |= MASK;
	}

	// and to vch
	unsigned char vch[13U];
	unsigned char ysfFrame[13U];
	memset(vch, 0U, 13U);
	memset(ysfFrame, 0, 13U);

	unsigned int dat_a = a >> 12;

	// The PRNG
	b ^= (PRNG_TABLE[dat_a] >> 1);

	unsigned int dat_b = b >> 11;

	for (unsigned int i = 0U; i < 12U; i++)
	{
		bool s = (dat_a << (20U + i)) & 0x80000000U;
		WRITE_BIT(vch, 3*i + 0U, s);
		WRITE_BIT(vch, 3*i + 1U, s);
		WRITE_BIT(vch, 3*i + 2U, s);
	}

	for (unsigned int i = 0U; i < 12U; i++)
	{
		bool s = (dat_b << (20U + i)) & 0x80000000U;
		WRITE_BIT(vch, 3*(i + 12U) + 0U, s);
		WRITE_BIT(vch, 3*(i + 12U) + 1U, s);
		WRITE_BIT(vch, 3*(i + 12U) + 2U, s);
	}

	for (unsigned int i = 0U; i < 3U; i++)
	{
		bool s = (dat_c << (7U + i)) & 0x80000000U;
		WRITE_BIT(vch, 3*(i + 24U) + 0U, s);
		WRITE_BIT(vch, 3*(i + 24U) + 1U, s);
		WRITE_BIT(vch, 3*(i + 24U) + 2U, s);
	}

	for (unsigned int i = 0U; i < 22U; i++)
	{
		bool s = (dat_c << (10U + i)) & 0x80000000U;
		WRITE_BIT(vch, i + 81U, s);
	}

	WRITE_BIT(vch, 103U, 0U);

	// Scramble
	for (unsigned int i = 0U; i < 13U; i++)
		vch[i] ^= WHITENING_DATA[i];

	// Interleave
	for (unsigned int i = 0U; i < 104U; i++)
	{
		unsigned int n = INTERLEAVE_TABLE_26_4[i];
		bool s = READ_BIT(vch, i);
		WRITE_BIT(ysfFrame, n, s);
	}

	memcpy(data, ysfFrame, 13U);
}
