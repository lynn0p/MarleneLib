// util.cpp Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#include "Util.h"
#include "sha256.h"

// sorta dodgy - finds the 3rd 0x30 and then copies everything from
// there to the end. 
std::string Utility::OSSLPublicKey2ZCPublicKey(const std::string &in)
{
	std::string out;
	unsigned int i=0,j=0;
	while(i<in.length() && j<3) {
		if (in[i] == 0x30) { ++j; }
		++i;
	}
	if (i > 0 && j == 3) {
		--i;
		out = in.substr(i);
	}
	return out;
}


std::string Utility::StripAllWhitespace(const std::string &in)
{
	std::string out;
	for (unsigned int i=0; i<in.length(); ++i) {
		if (!isspace(in[i])) {
			out += in[i];
		}
	}
	return out;
}

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) 
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Utility::Base64Encode(const std::string &in)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	unsigned int in_len = in.length();
	const unsigned char *p = (const unsigned char *)in.c_str();
	while (in_len--) {
		char_array_3[i++] = *(p++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; i<4; i++) {
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i) {
		for(j = i; j < 3; j++) {
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; j < (i + 1); j++) {
			ret += base64_chars[char_array_4[j]];
		}

		while((i++ < 3)) {
			ret += '=';
		}
	}

	return ret;
}

std::string Utility::Base64Decode(const std::string &raw)
{
	std::string in = StripAllWhitespace(raw);
	size_t in_len = in.size();
	size_t i = 0;
	size_t j = 0;
	int k = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( in[k] != '=') && is_base64(in[k])) {
		char_array_4[i++] = in[k]; k++;
		if (4 == i) {
			for (i = 0; i<4; i++) {
				char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; i<3; i++) {
				ret += char_array_3[i];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j<4; j++) {
			char_array_4[j] = 0;
		}

		for (j = 0; j<4; j++) {
			char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));
		}

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; j < (i-1); j++) { ret += char_array_3[j]; }
	}

	return ret;
}

// there's no good automatic way to decode a hex string in std::C++
std::string Utility::HexDecode(const std::string &in)
{
	std::string out;
	if ((in.length() % 2) == 0) {
		char buf[3] = {0,0,0};
		for(unsigned int i=0; i<in.length(); i += 2) {
			buf[0] = in[i];
			buf[1] = in[i+1];
			out.push_back((char)strtoul(buf,0,16));
		}
	}
	return out;
}

std::string Utility::HexEncode(const std::string &in)
{
	const char *tbl = "0123456789abcdef";
	std::string out;
	unsigned char hi,lo;
	for(unsigned int i=0; i<in.length(); ++i) {
		hi = ((unsigned char)in[i] >> 4);
		lo = ((unsigned char)in[i] & 0x0f);
		out.push_back(tbl[hi]);
		out.push_back(tbl[lo]);
	}

	return out;
}

unsigned long Utility::ToSatoshis(double amount)
{
	return (unsigned long)(amount * 1e8 + (amount < 0.0 ? -.5 : .5));
}

double Utility::FromSatoshis(unsigned long amount)
{
	return ((double)amount / 1e8);
}

std::string Utility::Sha256Hash(const std::string &in)
{
	std::string out;
	unsigned char digest[32];
	sha256_context ctx;
	sha256_starts(&ctx);
	sha256_update(&ctx,(unsigned char*)in.c_str(),in.length());
	sha256_finish(&ctx,digest);
	out.append((char *)digest,sizeof(digest));
	return out;
}

long Utility::CheckSum(const std::string &in)
{
    long sum = 0;
    for(unsigned int i=0; i<in.length(); ++i) {
        sum += (unsigned char)in[i];
    }
    return sum;
}
