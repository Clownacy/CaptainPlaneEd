/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) Flamewing 2013-2015 <flamewing.sonic@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <istream>
#include <ostream>
#include <sstream>

#include "rocket.h"
#include "bigendian_io.h"
#include "bitstream.h"
#include "lzss.h"
#include "ignore_unused_variable_warning.h"

using namespace std;

// NOTE: This has to be changed for other LZSS-based compression schemes.
struct RocketAdaptor {
	typedef unsigned char stream_t;
	typedef unsigned char descriptor_t;
	typedef littleendian<descriptor_t> descriptor_endian_t;
	// Number of bits on descriptor bitfield.
	constexpr static size_t const NumDescBits = sizeof(descriptor_t) * 8;
	// Number of bits used in descriptor bitfield to signal the end-of-file
	// marker sequence.
	constexpr static size_t const NumTermBits = 0;
	// Flag that tells the compressor that new descriptor fields is needed
	// when a new bit is needed and all bits in the previous one have been
	// used up.
	constexpr static size_t const NeedEarlyDescriptor = 0;
	// Flag that marks the descriptor bits as being in big-endian bit
	// order (that is, highest bits come out first).
	constexpr static size_t const DescriptorLittleEndianBits = 1;
	// Size of the search buffer.
	constexpr static size_t const SearchBufSize = 0x400;
	// Size of the look-ahead buffer.
	constexpr static size_t const LookAheadBufSize = 0x40;
	// Total size of the sliding window.
	constexpr static size_t const SlidingWindowSize = SearchBufSize + LookAheadBufSize;
	// Computes the cost of a symbolwise encoding, that is, the cost of encoding
	// one single symbol..
	constexpr static size_t symbolwise_weight() noexcept {
		// Symbolwise match: 1-bit descriptor, 8-bit length.
		return 1 + 8;
	}
	// Computes the cost of covering all of the "len" vertices starting from
	// "off" vertices ago, for matches with len > 1.
	// A return of "numeric_limits<size_t>::max()" means "infinite",
	// or "no edge".
	constexpr static size_t dictionary_weight(size_t dist, size_t len) noexcept {
		// Preconditions:
		// len > 1 && len <= LookAheadBufSize && dist != 0 && dist <= SearchBufSize
		// Dictionary match: 1-bit descriptor, 10-bit distance, 6-bit length.
		ignore_unused_variable_warning(dist, len);
		return 1 + 10 + 6;
	}
	// Given an edge, computes how many bits are used in the descriptor field.
	constexpr static size_t desc_bits(AdjListNode const &edge) noexcept {
		// Rocket always uses a single bit descriptor.
		ignore_unused_variable_warning(edge);
		return 1;
	}
	// Rocket finds no additional matches over normal LZSS.
	constexpr static void extra_matches(stream_t const *data,
	                          size_t basenode,
	                          size_t ubound, size_t lbound,
	                          LZSSGraph<RocketAdaptor>::MatchVector &matches) noexcept {
		ignore_unused_variable_warning(data, basenode, ubound, lbound, matches);
	}
	// Rocket needs no additional padding at the end-of-file.
	constexpr static size_t get_padding(size_t totallen, size_t padmask) noexcept {
		ignore_unused_variable_warning(totallen, padmask);
		return 0;
	}
};

typedef LZSSGraph<RocketAdaptor> RockGraph;
typedef LZSSOStream<RocketAdaptor> RockOStream;
typedef LZSSIStream<RocketAdaptor> RockIStream;


void rocket::decode_internal(istream &in, ostream &Dst) {
	in.seekg(2);						// Skip first part of header
	const int end_of_file = BigEndian::Read2(in) + 4;	// Get second half of header

	RockIStream src(in);

	unsigned char dictionary[0x400];
	int dictionary_index = 0x3C0;

	// Initialise dictionary (needed for plane map decompression)
	// TODO: Make compression actually account for this
	for (int i=0; i < 0x3C0; ++i)
	{
		dictionary[i] = 0x20;
	}

	while (in.good() && in.tellg() < end_of_file) {
		if (src.descbit()) {
			// Symbolwise match.
			unsigned char value = Read1(in);
			Write1(Dst, value);
			dictionary[dictionary_index++] = value;
			dictionary_index &= 0x3FF;
		} else {
			// Dictionary match.
			// Distance and length of match.
			unsigned short dictionary_entry = BigEndian::Read2(in);
			size_t distance = dictionary_entry&0x3FF,
			       length = dictionary_entry>>10;

			for (size_t i = 0; i <= length; i++) {
				unsigned char value = dictionary[(distance+i)&0x3FF];
				Write1(Dst, value);
				dictionary[dictionary_index++] = value;
				dictionary_index &= 0x3FF;
			}
		}
	}
}

bool rocket::decode(istream &Src, ostream &Dst, streampos Location) {
	Src.seekg(Location);
	stringstream in(ios::in | ios::out | ios::binary);
	in << Src.rdbuf();

	in.seekg(0);
	decode_internal(in, Dst);
	return true;
}

void rocket::encode_internal(ostream &Dst, unsigned char const *&Buffer,
                             streamsize const BSize) {
	// Compute optimal Rocket parsing of input file.
	RockGraph enc(Buffer, BSize, 1u);
	RockGraph::AdjList list = enc.find_optimal_parse();
	RockOStream out(Dst);

	streamoff pos = 0;
	// Go through each edge in the optimal path.
	for (RockGraph::AdjList::const_iterator it = list.begin();
	        it != list.end(); ++it) {
		AdjListNode const &edge = *it;
		size_t len = edge.get_length(), dist = edge.get_distance();
		// The weight of each edge uniquely identifies how it should be written.
		// NOTE: This needs to be changed for other LZSS schemes.
		if (len == 1) {
			// Symbolwise match.
			out.descbit(1);
			out.putbyte(Buffer[pos]);
		} else {
			// Dictionary match.
			out.descbit(0);
			dist = (0x3C0 + pos - dist) & 0x03FF;
			unsigned short high = (dist >> 8) & 0x03,
			               low  = dist & 0xFF;
			out.putbyte(high|(((len-1)&0x3F)<<2));
			out.putbyte(low);
		}
		// Go to next position.
		pos = edge.get_dest();
	}
}

bool rocket::encode(istream &Src, ostream &Dst) {
	Src.seekg(0, ios::end);
	streamsize ISize = Src.tellg();
	Src.seekg(0);
	// Pad to even size.
	streamsize BSize = ISize + ((ISize & 1) != 0 ? 1 : 0);
	auto const Buffer = new char[BSize];
	unsigned char const *ptr = reinterpret_cast<unsigned char *>(Buffer);
	Src.read(Buffer, ISize);
	// If we had to increase buffer size, we need to set the last byte in the
	// buffer manually. We will pad with a 0 byte.
	if (ISize != BSize) {
		Buffer[ISize] = 0;
	}

	Dst.seekp(4);		// Skip header for now

	encode_internal(Dst, ptr, BSize);

	// Write header
	int compressed_size = Dst.tellp();
	Dst.seekp(0);
	BigEndian::Write2(Dst, BSize);
	BigEndian::Write2(Dst, compressed_size-4);

	delete [] Buffer;
	return true;
}
