// util.h Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#ifndef _UTIL_H_
#define _UTIL_H_
#include <string>

// a place for calls that don't need the internal state of an object
namespace Utility {
	std::string OSSLPublicKey2ZCPublicKey(const std::string &in);

	std::string StripAllWhitespace(const std::string &in);

	std::string Base64Encode(const std::string &in);
	std::string Base64Decode(const std::string &in);

	// also known as Base16
	std::string HexDecode(const std::string &in);
	std::string HexEncode(const std::string &in);

	unsigned long ToSatoshis(double amount);
	double        FromSatoshis(unsigned long amount);

    std::string Sha256Hash(const std::string &in);
    long        CheckSum(const std::string &in);
}

#endif // _UTIL_H_
