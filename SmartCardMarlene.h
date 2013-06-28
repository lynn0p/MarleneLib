// SmartCardMarlene.h Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#ifndef _SMARTCARDMARLENE_H_
#define _SMARTCARDMARLENE_H_

#include "smartcard.h"

class SmartCardMarlene : public SmartCardInterface
{
public:
	SmartCardMarlene();
	~SmartCardMarlene();

	bool Nuke();
    bool GetContactInfo(std::string &out);
    bool GetHostData(unsigned long &port_out, std::string &host_out);
	bool GetVersion(unsigned long &out);

	bool SayHello(const std::string &pin);
	bool GetBalance(const std::string &pin, unsigned long long &balance);
	bool GetReceivingAddress(const std::string &pin, std::string &addr_out);
	bool MakePayment(const std::string &pin, unsigned long long amount, const std::string &bcaddr);
	bool GetPaymentStatus(const std::string &pin,unsigned long &status_out,std::string &txid_out);
	bool SayGoodbye(const std::string &pin);
	bool GetTxFee(const std::string &pin, unsigned long long &txfee);
	bool CheckErrorCode(const std::string &pin, unsigned long &code);

	bool SetServerResponse(const std::string &pin, int flag, const std::string &chunk);
	bool ParseServerResponse(const std::string &pin);
	bool GetCardRequestLength(const std::string &pin, unsigned long &len);
	bool ReadCardRequest(const std::string &pin, unsigned long pos, std::string &out);

	bool SetNewPIN(const std::string &pass, const std::string &newpin);
	bool SetNewPassphrase(const std::string &pass, const std::string &newpass);
	bool SetPublicKeyData(const std::string &pass, int flag, const std::string &chunk);
	bool SetContactInfo(const std::string &pass, const std::string &info);
	bool SetHostData(const std::string &pass, unsigned long port, const std::string &host);
	bool SetWalletPassphrase(const std::string &pass, const std::string &wpass);

	// anytime you need to poke data to the card, and you
	// don't want to go trolling though the source,
	// just make sure all the chunks are at this length or shorter
	// and you won't run into problems
	static const unsigned long SAFE_CHUNK_LENGTH;
private:
};

#endif // _SMARTCARDMARLENE_H_
