// SmartCardMarlene.cpp Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#include "SmartCardMarlene.h"
#include <QtGlobal>

// There's an open source version of this ZCBCI library, but
// I'm using the commercial Windows library from Zeitcontrol
// If you want to port this to another platform, you'll need
// to build that open source library and fix these headers
#ifndef Q_OS_WIN32
#error "You need to port this class to your platform"
#endif

#include <Windows.h>
#include "zccri.h"
#include "zcbci.h"

const unsigned long SmartCardMarlene::SAFE_CHUNK_LENGTH = 200;

SmartCardMarlene::SmartCardMarlene()
{
}


SmartCardMarlene::~SmartCardMarlene()
{
}

bool
SmartCardMarlene::Nuke()
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm;
	WORD       sw1sw2;

	parm.parmtype = ZCBCINULL;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x00,&parm,1,&sw1sw2);

	return (zcrc == 0);
}

bool
SmartCardMarlene::GetContactInfo(std::string &out)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm;
	WORD           sw1sw2;
	unsigned char  buf[255];

	parm.parmtype = ZCBCIBINSTRING;
	parm.data.valbinstring.bSize = (unsigned char)sizeof(buf);
	parm.data.valbinstring.bLength = 0;
	parm.data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x02,&parm,1,&sw1sw2);
	if (zcrc == 0) {
		out.clear();
		out.append((char*)buf,parm.data.valbinstring.bLength);
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::GetHostData(unsigned long &port_out, std::string &host_out)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	unsigned char  buf[250];

	parm[0].parmtype = ZCBCILONG;
	parm[0].data.vallong = 0;
	parm[1].parmtype = ZCBCIBINSTRING;
	parm[1].data.valbinstring.bSize = sizeof(buf);
	parm[1].data.valbinstring.bLength = 0;
	parm[1].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x04,parm,2,&sw1sw2);
	if (zcrc == 0) {
		port_out = (unsigned long)parm[0].data.vallong;
		host_out.clear();
		host_out.append((char*)buf,parm[1].data.valbinstring.bLength);
	}
	return (zcrc == 0);
}

bool SmartCardMarlene::GetVersion(unsigned long &out)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm;
	WORD           sw1sw2;
	unsigned char  buf[5];

	memset(buf,0,sizeof(buf));

	parm.parmtype = ZCBCIBINSTRINGN;
	parm.data.valbinstringn.bLength = 4;
	parm.data.valbinstringn.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x06,&parm,1,&sw1sw2);
	if (zcrc == 0) {
		unsigned long *pver = (unsigned long *)buf;
		out = *pver;
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::SayHello(const std::string &pin)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm;
	WORD       sw1sw2;
	char       pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};

	parm.parmtype = ZCBCISTRINGN;
	parm.data.valstringn.bLength = 4;
	parm.data.valstringn.pString = pinbuf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x20,&parm,1,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::GetBalance(const std::string &pin, unsigned long long &balance)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[9];

	memset(buf,0,sizeof(buf));

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIBINSTRINGN;
	parm[1].data.valbinstringn.bLength = 8;
	parm[1].data.valbinstringn.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x22,parm,2,&sw1sw2);
	if (zcrc == 0) {
		unsigned long long *pbal = (unsigned long long *)buf;
		balance = *pbal;
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::GetReceivingAddress(const std::string &pin, std::string &addr_out)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[250];

	memset(buf,0,sizeof(buf));

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIBINSTRING;
	parm[1].data.valbinstring.bSize = sizeof(buf);
	parm[1].data.valbinstring.bLength = 0;
	parm[1].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x24,parm,2,&sw1sw2);
	if (zcrc == 0) {
		addr_out.clear();
		addr_out.append((char *)buf,parm[1].data.valbinstring.bLength);
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::MakePayment(const std::string &pin, unsigned long long amount, const std::string &bcaddr)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[3];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[244];

	if (bcaddr.length() >= sizeof(buf)) { return false; }

	memset(buf,0,sizeof(buf));
	memcpy(buf,bcaddr.c_str(),bcaddr.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIBINSTRINGN;
	parm[1].data.valbinstringn.bLength = sizeof(amount);
	parm[1].data.valbinstringn.pData = (unsigned char *)&amount;
	parm[2].parmtype = ZCBCIBINSTRING;
	parm[2].data.valbinstring.bSize = sizeof(buf);
	parm[2].data.valbinstring.bLength = (unsigned char)bcaddr.length();
	parm[2].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x26,parm,3,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::GetPaymentStatus(const std::string &pin,unsigned long &status_out,std::string &txid_out)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[3];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[33];
	unsigned char  statbuf[5];

	memset(buf,0,sizeof(buf));
	memset(statbuf,0,sizeof(statbuf));

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIBINSTRINGN;
	parm[1].data.valbinstringn.bLength = 4;
	parm[1].data.valbinstringn.pData = statbuf;
	parm[2].parmtype = ZCBCIBINSTRINGN;
	parm[2].data.valbinstringn.bLength = 32;
	parm[2].data.valbinstringn.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x28,parm,3,&sw1sw2);
	if (zcrc == 0) {
		unsigned long *pstatus = (unsigned long *)parm[1].data.valbinstringn.pData;
		status_out = *pstatus;
		txid_out.clear();
		if (status_out == 0) {
			// there's only a txid, if status returns success
			txid_out.append((char*)buf,32);
		}
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::SayGoodbye(const std::string &pin)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm;
	WORD       sw1sw2;
	char       pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};

	parm.parmtype = ZCBCISTRINGN;
	parm.data.valstringn.bLength = 4;
	parm.data.valstringn.pString = pinbuf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x2a,&parm,1,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::GetTxFee(const std::string &pin, unsigned long long &txfee)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[9];

	memset(buf,0,sizeof(buf));

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIBINSTRINGN;
	parm[1].data.valbinstringn.bLength = 8;
	parm[1].data.valbinstringn.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x2c,parm,2,&sw1sw2);
	if (zcrc == 0) {
		unsigned long long *ptxfee = (unsigned long long *)buf;
		txfee = *ptxfee;
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::CheckErrorCode(const std::string &pin, unsigned long &code)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[5];

	memset(buf,0,sizeof(buf));

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIBINSTRINGN;
	parm[1].data.valbinstringn.bLength = 4;
	parm[1].data.valbinstringn.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x2e,parm,2,&sw1sw2);
	if (zcrc == 0) {
		unsigned long *pcode = (unsigned long *)buf;
		code = *pcode;
	}
	return (zcrc == 0);
}

bool SmartCardMarlene::SetServerResponse(const std::string &pin, int flag, const std::string &chunk)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm[3];
	WORD       sw1sw2;
	char       pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[250];

	if (chunk.length() >= sizeof(buf)) { return false; }

	memset(buf,0,sizeof(buf));
	memcpy(buf,chunk.c_str(),chunk.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIINT;
	parm[1].data.valint = flag;
	parm[2].parmtype = ZCBCIBINSTRING;
	parm[2].data.valbinstring.bSize = sizeof(buf);
	parm[2].data.valbinstring.bLength = (unsigned char)chunk.length();
	parm[2].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x30,parm,3,&sw1sw2);
	return (zcrc == 0);
}

bool SmartCardMarlene::ParseServerResponse(const std::string &pin)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm;
	WORD       sw1sw2;
	char       pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};

	parm.parmtype = ZCBCISTRINGN;
	parm.data.valstringn.bLength = 4;
	parm.data.valstringn.pString = pinbuf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x32,&parm,1,&sw1sw2);
	return (zcrc == 0);
}

bool SmartCardMarlene::GetCardRequestLength(const std::string &pin, unsigned long &len)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm[2];
	WORD       sw1sw2;
	char       pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCILONG;
	parm[1].data.vallong = 0;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x34,parm,2,&sw1sw2);
	if (zcrc == 0) {
		len = parm[1].data.vallong;
	}
	return (zcrc == 0);
}

bool SmartCardMarlene::ReadCardRequest(const std::string &pin, unsigned long pos, std::string &out)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[3];
	WORD           sw1sw2;
	char           pinbuf[5] = {pin[0],pin[1],pin[2],pin[3],0};
	unsigned char  buf[250];

	if (pos == 0) { return false; }
	memset(buf,0,sizeof(buf));

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 4;
	parm[0].data.valstringn.pString = pinbuf;
	parm[1].parmtype = ZCBCIINT;
	parm[1].data.valint = pos;
	parm[2].parmtype = ZCBCIBINSTRING;
	parm[2].data.valbinstring.bSize = sizeof(buf);
	parm[2].data.valbinstring.bLength = 0;
	parm[2].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x36,parm,3,&sw1sw2);
	if (zcrc == 0) {
		out.clear();
		out.append((const char *)buf,parm[2].data.valbinstring.bLength);
	}
	return (zcrc == 0);
}

bool
SmartCardMarlene::SetNewPIN(const std::string &passhash, const std::string &newpin)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm[2];
	WORD       sw1sw2;
	char buf1[33];
	char buf2[5];

	if (passhash.length() >= sizeof(buf1)) { return false; }
	if (newpin.length() >= sizeof(buf2)) { return false; } 

	memset(buf1,0,sizeof(buf1));
	memcpy(buf1,passhash.c_str(),passhash.length());
	memset(buf2,0,sizeof(buf2));
	memcpy(buf2,newpin.c_str(),newpin.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 32;
	parm[0].data.valstringn.pString = buf1;
	parm[1].parmtype = ZCBCISTRINGN;
	parm[1].data.valstringn.bLength = 4;
	parm[1].data.valstringn.pString = buf2;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x40,parm,2,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::SetNewPassphrase(const std::string &passhash, const std::string &newpass)
{
	ZCCRIRET   zcrc = 0;
	ZCBCICARD *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM  parm[2];
	WORD       sw1sw2;
	char buf1[33];
	char buf2[33];

	if (passhash.length() >= sizeof(buf1)) { return false; }
	if (newpass.length() >= sizeof(buf2)) { return false; } 

	memset(buf1,0,sizeof(buf1));
	memcpy(buf1,passhash.c_str(),passhash.length());
	memset(buf2,0,sizeof(buf2));
	memcpy(buf2,newpass.c_str(),newpass.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 32;
	parm[0].data.valstringn.pString = buf1;
	parm[1].parmtype = ZCBCISTRINGN;
	parm[1].data.valstringn.bLength = 32;
	parm[1].data.valstringn.pString = buf2;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x42,parm,2,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::SetPublicKeyData(const std::string &passhash, int flag, const std::string &chunk)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[3];
	WORD           sw1sw2;
	char           passbuf[33];
	unsigned char  buf[220];

	if (passhash.length() >= sizeof(passbuf)) { return false; }
	if (chunk.length() >= (sizeof(buf)-1))        { return false; }

	memset(passbuf,0,sizeof(passbuf));
	memcpy(passbuf,passhash.c_str(),passhash.length());
	memset(buf,0,sizeof(buf));
	memcpy(buf,chunk.c_str(),chunk.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 32;
	parm[0].data.valstringn.pString = passbuf;
	parm[1].parmtype = ZCBCIINT;
	parm[1].data.valint = flag;
	parm[2].parmtype = ZCBCIBINSTRING;
	parm[2].data.valbinstring.bSize = (unsigned char)sizeof(buf);
	parm[2].data.valbinstring.bLength = (unsigned char)chunk.length();
	parm[2].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x44,parm,3,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::SetContactInfo(const std::string &passhash, const std::string &info)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	char           passbuf[33];
	unsigned char  buf[220];

	if (passhash.length() >= sizeof(passbuf)) { return false; }
	if (info.length() >= (sizeof(buf)-1)) { return false; }

	memset(passbuf,0,sizeof(passbuf));
	memcpy(passbuf,passhash.c_str(),passhash.length());
	memset(buf,0,sizeof(buf));
	memcpy(buf,info.c_str(),info.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 32;
	parm[0].data.valstringn.pString = passbuf;
	parm[1].parmtype = ZCBCIBINSTRING;
	parm[1].data.valbinstring.bSize = (unsigned char)sizeof(buf);
	parm[1].data.valbinstring.bLength = (unsigned char)info.length();
	parm[1].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x46,parm,2,&sw1sw2);
	return (zcrc == 0);
}

bool
SmartCardMarlene::SetHostData(const std::string &passhash, unsigned long port, const std::string &host)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[3];
	WORD           sw1sw2;
	char           passbuf[33];
	unsigned char  buf[210];

	if (passhash.length() >= sizeof(passbuf)) { return false; }
	if (host.length() >= (sizeof(buf)-1)) { return false; }

	memset(passbuf,0,sizeof(passbuf));
	memcpy(passbuf,passhash.c_str(),passhash.length());
	memset(buf,0,sizeof(buf));
	memcpy(buf,host.c_str(),host.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 32;
	parm[0].data.valstringn.pString = passbuf;
	parm[1].parmtype = ZCBCILONG;
	parm[1].data.vallong = port;
	parm[2].parmtype = ZCBCIBINSTRING;
	parm[2].data.valbinstring.bSize = (unsigned char)sizeof(buf);
	parm[2].data.valbinstring.bLength = (unsigned char)host.length();
	parm[2].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x48,parm,3,&sw1sw2);
	return (zcrc == 0);
}

bool SmartCardMarlene::SetWalletPassphrase(const std::string &passhash, const std::string &wpass)
{
	ZCCRIRET       zcrc = 0;
	ZCBCICARD     *phBCI = (ZCBCICARD*) m_bci;
	ZCBCIPARM      parm[2];
	WORD           sw1sw2;
	char           passbuf[33];
	unsigned char  buf[220];

	if (passhash.length() >= sizeof(passbuf)) { return false; }
	if (wpass.length() >= (sizeof(buf)-1)) { return false; }

	memset(passbuf,0,sizeof(passbuf));
	memcpy(passbuf,passhash.c_str(),passhash.length());
	memset(buf,0,sizeof(buf));
	memcpy(buf,wpass.c_str(),wpass.length());

	parm[0].parmtype = ZCBCISTRINGN;
	parm[0].data.valstringn.bLength = 32;
	parm[0].data.valstringn.pString = passbuf;
	parm[1].parmtype = ZCBCIBINSTRING;
	parm[1].data.valbinstring.bSize = sizeof(buf);
    parm[1].data.valbinstring.bLength = (BYTE)wpass.length();
	parm[1].data.valbinstring.pData = buf;
	zcrc = ZCBciTransaction(*phBCI,0x01,0x4a,parm,2,&sw1sw2);
	return (zcrc == 0);
}
