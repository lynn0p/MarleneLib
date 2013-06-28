// SmartCard.cpp Copyright 2013 Owen Lynn <owen.lynn@gmail.com>
// Released under the GNU Public License V3

#include "SmartCard.h"

#include <Windows.h>
#include <zccri.h>
#include <zcbci.h>

SmartCardInterface::SmartCardInterface()
{
	m_cri  = new ZCCRIREADER;
	m_bci  = new ZCBCICARD;
	m_ATR = new ZCCRIATR;
}

SmartCardInterface::~SmartCardInterface()
{
	delete m_cri;
	delete m_bci;
	delete m_ATR;
}

bool
SmartCardInterface::Init()
{
	ZCCRIRET    zcrc = 0;
	ZCCRIREADER * phCRI = (ZCCRIREADER*) m_cri;

    zcrc = ZCCriOpenReader(phCRI, 0);
    if (zcrc) {
		goto end;
	}

end:
	return (zcrc == 0);
}

bool
SmartCardInterface::WaitConnect(unsigned int msecs)
{
	ZCCRIRET    zcrc = 0;
	ZCCRIREADER * phCRI = (ZCCRIREADER*) m_cri;
	ZCBCICARD   * phBCI = (ZCBCICARD*) m_bci;
	ZCCRIATR    * pATR = (ZCCRIATR*) m_ATR;

	zcrc = ZCCriWaitCard(*phCRI, msecs);
	if (zcrc) {
		goto end;
	}
	zcrc = ZCCriConnect(*phCRI, pATR);
	if (zcrc) {
		zcrc = ZCCriReconnect(*phCRI, pATR);
		if (zcrc) {
			goto end;
		}
	}
	zcrc = ZCBciAttach(*phCRI, phBCI);
	if (zcrc) {
		goto end;
	}

end:
	return (zcrc == 0);
}

bool
SmartCardInterface::GetApplicationID(std::string &out)
{
	ZCBCICARD   * phBCI = (ZCBCICARD*) m_bci;
	ZCCRIRET  zcrc = 0;
	WORD      sw1sw2 = 0;
	char buf[255];

	memset(buf,0,sizeof(buf));
	out.clear();
	zcrc = ZCBciGetApplicationID(*phBCI,&sw1sw2,buf,sizeof(buf));
	if (zcrc == 0) { out.append(buf); }
	return (zcrc == 0);
}

bool
SmartCardInterface::Disconnect()
{
	ZCCRIRET    zcrc = 0;
	ZCCRIREADER * phCRI = (ZCCRIREADER*) m_cri;
	ZCBCICARD   * phBCI = (ZCBCICARD*) m_bci;

	zcrc = ZCBciDetach(*phBCI);
	if (zcrc) {
		goto end;
	}
	zcrc = ZCCriDisconnect(*phCRI);
	if (zcrc) {
		goto end;
	}

end:
	return (zcrc == 0);
}

bool
SmartCardInterface::Term()
{
	ZCCRIRET    zcrc = 0;
	ZCCRIREADER * phCRI = (ZCCRIREADER*) m_cri;

	zcrc = ZCCriCloseReader(*phCRI);
	return (zcrc == 0);
}

