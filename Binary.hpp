#pragma once

#ifndef __BINARY_STREAM__
#define __BINARY_STREAM__

#include <vector>  // buffers
#include <sstream> // strings, byteStr()
#include <iomanip>

class Binary {
public:
	Binary() noexcept {}
	Binary(const std::vector<unsigned char>& _buffer) noexcept : buffer(_buffer) {}

	void setBuffer(std::vector<unsigned char>& _buffer) noexcept {
		buffer = _buffer;
	}
	const std::vector<unsigned char>& getBuffer() const noexcept {
		return buffer;
	}
	void clear() noexcept {
		buffer.clear();
		readOffset = 0;
		writeOffset = 0;
	}

	std::string byteStr(bool LE = true) const noexcept {
		std::stringstream byteStr;
		byteStr << std::hex << std::setfill('0');

		if (LE == true) {
			for (unsigned long long i = 0; i < buffer.size(); ++i)
				byteStr << std::setw(2) << (unsigned short)buffer[i] << " ";
		}
		else {
			unsigned long long size = buffer.size();
			for (unsigned long long i = 0; i < size; ++i)
				byteStr << std::setw(2) << (unsigned short)buffer[size - i - 1] << " ";
		}

		return byteStr.str();
	}

	/************************** Writing ***************************/

	template <class T> inline void writeBytes(const T& val, bool LE = true) {
		unsigned int size = sizeof(T);

		if (LE == true) {
			for (unsigned int i = 0, mask = 0; i < size; ++i, mask += 8)
				buffer.push_back(val >> mask);
		}
		else {
			unsigned const char* array = reinterpret_cast<unsigned const char*>(&val);
			for (unsigned int i = 0; i < size; ++i)
				buffer.push_back(array[size - i - 1]);
		}
		writeOffset += size;
	}

	void writeBytes(const unsigned char* data, size_t length) {
		for (size_t i = 0; i < length; i++)
			writeBytes<unsigned char>(data[i]);
	}

	void writeBytes(const std::vector<unsigned char>& data) {
		writeBytes(data.data(), data.size());
	}

	void writeBool(bool val) noexcept {
		writeBytes<bool>(val);
	}
	void writeStr(const std::string& str) noexcept {
		writeUInt8(str.size());
		for (const unsigned char& s : str) writeInt8(s);
	}
	void writeInt8(char val) noexcept {
		writeBytes<char>(val);
	}
	void writeUInt8(unsigned char val) noexcept {
		writeBytes<unsigned char>(val);
	}

	void writeInt16_LE(short val) noexcept {
		writeBytes<short>(val);
	}
	void writeInt16_BE(short val) noexcept {
		writeBytes<short>(val, false);
	}
	void writeUInt16_LE(unsigned short val) noexcept {
		writeBytes<unsigned short>(val);
	}
	void writeUInt16_BE(unsigned short val) noexcept {
		writeBytes<unsigned short>(val, false);
	}

	void writeInt32_LE(int val) noexcept {
		writeBytes<int>(val);
	}
	void writeInt32_BE(int val) noexcept {
		writeBytes<int>(val, false);
	}
	void writeUInt32_LE(unsigned int val) noexcept {
		writeBytes<unsigned int>(val);
	}
	void writeUInt32_BE(unsigned int val) noexcept {
		writeBytes<unsigned int>(val, false);
	}

	void writeInt64_LE(long long val) noexcept {
		writeBytes<long long>(val);
	}
	void writeInt64_BE(long long val) noexcept {
		writeBytes<long long>(val, false);
	}
	void writeUInt64_LE(unsigned long long val) noexcept {
		writeBytes<unsigned long long>(val);
	}
	void writeUInt64_BE(unsigned long long val) noexcept {
		writeBytes<unsigned long long>(val, false);
	}

	void writeFloat_LE(float val) noexcept {
		union { float fnum; unsigned inum; } u;
		u.fnum = val;
		writeUInt32_LE(u.inum);
	}
	void writeFloat_BE(float val) noexcept {
		union { float fnum; unsigned inum; } u;
		u.fnum = val;
		writeUInt32_BE(u.inum);
	}
	void writeDouble_LE(double val) noexcept {
		union { double fnum; unsigned long long inum; } u;
		u.fnum = val;
		writeUInt64_LE(u.inum);
	}
	void writeDouble_BE(double val) noexcept {
		union { double fnum; unsigned long long inum; } u;
		u.fnum = val;
		writeUInt64_BE(u.inum);
	}

	/************************* READING *************************/

	void setReadOffset(unsigned long long newOffset) noexcept {
		readOffset = newOffset;
	}
	unsigned long long getReadOffset() const noexcept {
		return readOffset;
	}
	template <class T> inline T readBytes(bool LE = true) {
		T result = 0;
		unsigned int size = sizeof(T);

		// Do not overflow
		if (readOffset + size > buffer.size())
			return result;

		char* dst = (char*)& result;
		char* src = (char*)& buffer[readOffset];

		if (LE == true) {
			for (unsigned int i = 0; i < size; ++i)
				dst[i] = src[i];
		}
		else {
			for (unsigned int i = 0; i < size; ++i)
				dst[i] = src[size - i - 1];
		}
		readOffset += size;
		return result;
	}

	bool readBool() noexcept {
		return readBytes<bool>();
	}
	std::string readStr(unsigned long long len, bool lengthBased = true) noexcept {
		if (lengthBased) {
			readUInt8();
		}

		if (readOffset + len > buffer.size())
			throw new std::exception("Buffer out of range (provided length greater than buffer size)");

		std::string result(buffer.begin() + readOffset, buffer.begin() + readOffset + len);
		readOffset += len;
		return result;
	}
	std::string readStr() noexcept {
		auto length = readUInt8();
		return readStr(length, false);
	}
	char readInt8() noexcept {
		return readBytes<char>();
	}
	unsigned char readUInt8() noexcept {
		return readBytes<unsigned char>();
	}

	short readInt16_LE() noexcept {
		return readBytes<short>();
	}
	short readInt16_BE() noexcept {
		return readBytes<short>(false);
	}
	unsigned short readUInt16_LE() noexcept {
		return readBytes<unsigned short>();
	}
	unsigned short readUInt16_BE() noexcept {
		return readBytes<unsigned short>(false);
	}

	int readInt32_LE() noexcept {
		return readBytes<int>();
	}
	int readInt32_BE() noexcept {
		return readBytes<int>(false);
	}
	unsigned int readUInt32_LE() noexcept {
		return readBytes<unsigned int>();
	}
	unsigned int readUInt32_BE() noexcept {
		return readBytes<unsigned int>(false);
	}

	long long readInt64_LE() noexcept {
		return readBytes<long long>();
	}
	long long readInt64_BE() noexcept {
		return readBytes<long long>(false);
	}
	unsigned long long readUInt64_LE() noexcept {
		return readBytes<unsigned long long>();
	}
	unsigned long long readUInt64_BE() noexcept {
		return readBytes<unsigned long long>(false);
	}

	float readFloat_LE() noexcept {
		return readBytes<float>();
	}
	float readFloat_BE() noexcept {
		return readBytes<float>(false);
	}
	double readDouble_LE() noexcept {
		return readBytes<double>();
	}
	double readDouble_BE() noexcept {
		return readBytes<double>(false);
	}

	~Binary() {
		clear();
	}

	private:
		std::vector<unsigned char> buffer;
		unsigned long long readOffset = 0;
		unsigned long long writeOffset = 0;
};

#endif