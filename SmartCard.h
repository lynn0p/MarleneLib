// SmartCard.h Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#ifndef _SMARTCARD_H_
#define _SMARTCARD_H_

#include <string>

class SmartCardInterface
{
public:
	SmartCardInterface();
	virtual ~SmartCardInterface();

	bool Init();
	bool WaitConnect(unsigned int msecs=1000);
	bool GetApplicationID(std::string &out);
	bool Disconnect();
	bool Term();

protected:
	void * m_cri;
	void * m_bci;
	void * m_ATR;

private:
	// don't impl
	SmartCardInterface( const SmartCardInterface & src );
	SmartCardInterface & operator= ( const SmartCardInterface & src );
};

#endif // _SMARTCARD_H_
