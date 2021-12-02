/**
 * @file
 */

#include <gtest/gtest.h>
#include "io/BufferedReadWriteStream.h"
#include <random>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <SDL_timer.h>
#include <stdlib.h>
#include "core/String.h"

namespace {
const uint8_t BYTE_ADD = UCHAR_MAX;
const int32_t INT_ADD = INT_MAX;
}

namespace io {

TEST(BufferedReadWriteStreamTest, testWriteInt) {
	BufferedReadWriteStream stream;
	const int64_t previous = stream.size();
	stream.addInt(INT_ADD);
	ASSERT_EQ(previous + 4, stream.size());
}

TEST(BufferedReadWriteStreamTest, testReadByte) {
	BufferedReadWriteStream stream;
	stream.addByte(BYTE_ADD);
	const int64_t previous = stream.size();
	const uint8_t byte = stream.readByte();
	ASSERT_EQ(BYTE_ADD, byte);
	ASSERT_EQ(previous - 1, stream.size());
}

TEST(BufferedReadWriteStreamTest, testReadInt) {
	BufferedReadWriteStream stream;
	stream.addInt(INT_ADD);
	const int64_t previous = stream.size();
	int32_t dword = stream.readInt();
	ASSERT_EQ(INT_ADD, dword);
	ASSERT_EQ(previous - 4, stream.size());
}

}
