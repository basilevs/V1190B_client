/*
 * V1190BClient.cpp
 *
 *  Created on: 20.10.2009
 *      Author: gulevich
 */

#include "V1190BClient.h"
#include "socketbuf.h"
#include <netinet/in.h>

#include <iostream>

using namespace std;


typedef socketbuf<char> SocketType;

enum {MAX_EVENT_LENGTH=2000};

struct V1190BClient::RawEvent {
	unsigned int count;
	int length;
	//		uint32_t words[MAX_EVENT_LENGTH];
	unsigned int words[MAX_EVENT_LENGTH];
	void clear() {length=0; count=0;}
	RawEvent() {clear();}
};


V1190BClient::V1190BClient(const string & host, int port):
	_host(host),
	_port(port),
	_started(false),
	_highSpeedCoreClock(true)
{
}

void V1190BClient::disconnect() {
	SocketType * socket = _streambuf.get();
	if (!socket) return;
	socket->disconnect();
}

V1190BClient::V1190BClient():
	_started(false),
	_highSpeedCoreClock(true)
{}


V1190BClient::operator bool() const {
	SocketType * socket = _streambuf.get();
	if (!socket) return false;
	if (!socket->isConnected()) return false;
	return true;
}

V1190BClient::~V1190BClient() {
}

void V1190BClient::setWindow(int offset, int width) {
	if (connect() != socketwrapper::OK) return ;
	ostream ostr(_streambuf.get());
	ostr << "window " << offset << " " << width << endl;
	noop();
}

void V1190BClient::setDeadTime(int time) {
	if (connect() != socketwrapper::OK)
		return;
	unsigned mode = 0;
	if (time <= 5) {
		mode = 0;
	} else if (time <= 10) {
		mode = 1;
	} else if (time <= 30) {
		mode = 2;
	} else {
		mode = 3;
	}
	ostream ostr(_streambuf.get());
	ostr << "deadTime " << mode << endl;
	noop();
}

void V1190BClient::setDllClock(unsigned mode) {
	if (connect() != socketwrapper::OK)
		return;
	assert(mode < 4);
	if (mode >=4)
		return;
	ostream ostr(_streambuf.get());
	ostr << "dllClock " << mode << endl;
	noop();
}


void V1190BClient::start() {
	if (_started)
		return;
	ostream ostr(_streambuf.get());
	ostr << "start" << endl;
	_started = true;
	noop();
}

void V1190BClient::noop() {
	if (connect() != socketwrapper::OK) return ;
	ostream ostr(_streambuf.get());
	for (unsigned i = 0; i < 1; i++) {
		ostr << "noop" << endl;
	}
}

void V1190BClient::clear() {
	if (!connect()) return ;
	ostream ostr(_streambuf.get());
	ostr << "clear"<< endl;
}

void V1190BClient::lowSpeedCoreClock() {
	_highSpeedCoreClock = false;
	sendCoreClock();
}

SocketType::Error V1190BClient::sendCoreClock() {
	if (_highSpeedCoreClock)
		return SocketType::OK;
	SocketType::Error rc = connect();
	if (rc != SocketType::OK)
		return rc;
	ostream ostr(_streambuf.get());
	ostr << "lowSpeedCoreClock" << endl;
	return SocketType::OK;
}

SocketType::Error V1190BClient::connect(const char * host, int port) {
	_streambuf.reset();
	_host = host;
	_port = port;
	return connect();
}

SocketType::Error V1190BClient::connect() {
	SocketType * socket= _streambuf.get();
	if (!socket) {
		socket = new SocketType(_host, _port);
		assert(socket);
		socket->setNoDelay(true);
		_streambuf.reset(socket);
	}
	if (!socket->isConnected()) {
		socket->setNoDelay(true);
		SocketType::Error rc=socket->connect();
		_started = false;
		if (rc != SocketType::OK) {
			cerr << "Connection error: " <<  SocketType::errorToString(rc) << endl;
			socket->disconnect();
			return rc;
		}
		return sendCoreClock();
	}
	return SocketType::OK;
}

bool V1190BClient::readRawEvent(RawEvent & oEvent) {
	if (connect() != socketwrapper::OK) return false;
	SocketType * socket=(SocketType *) _streambuf.get();
	uint32_t netWord;
	oEvent.clear();
	while ( _streambuf->sgetn((char *)(&netWord), sizeof(uint32_t)) == sizeof(uint32_t) ) {
		uint32_t word=ntohl(netWord);
		if (word>>27==8) { // Global header
			if (oEvent.length!=0) {
				cerr << ("No oEvent trailer\n");
				oEvent.clear();
			}
			oEvent.count=word>>5;//Skipping GEO information
			oEvent.count&=0x1FFFFF;//Extracting 21 bit of oEvent count
		}
		assert(oEvent.length<MAX_EVENT_LENGTH);
		oEvent.words[oEvent.length]=word;
		oEvent.length++;
		if (word>>27==16) { //Global trailer
			return true;
		}
		if (oEvent.length>=MAX_EVENT_LENGTH) {
			oEvent.length=0;
			cerr << ("Event overflow") << endl;
		}
	}
	socket->disconnect();
	return false;
}


typedef V1190BClient::Event::Error::Type ErrorType;
V1190BClient::Event::Error::Error(ErrorType t, int tdc, int group):
	_tdc(tdc),
	_group(group),
	_type(t)
{
	assert(isValid());
}

bool isGroupError(ErrorType t) {
	typedef V1190BClient::Event::Error E;
	if (	t==E::EVENT_OVERFLOW ||
		t==E::EVENT_LOST ||
		t==E::FATAL_CHIP_ERROR ||
		t==E::OUTPUT_BUFFER_OVERFLOW
		) {
		return false;
	} else {
		return true;
	}
}

const char * typeToStr(ErrorType t) {
#define act(type) if (E::type==t) {return #type;}
	typedef V1190BClient::Event::Error E;
	act(HIT_LOST_READOUT);
	act(HIT_LOST_L1);
	act(HIT_ERROR);
	act(EVENT_OVERFLOW);
	act(EVENT_LOST);
	act(FATAL_CHIP_ERROR);
	act(OUTPUT_BUFFER_OVERFLOW);
	assert(false);
	return "UNKNOWN";
}

bool V1190BClient::Event::Error::isValid() const {
	if (!isGroupError(type())){
		return group()==-1;
	} else {
		if (tdc()<0)
			return false;
		if (tdc()>3)
			return false;
		if (group()>3)
			return false;
		return group() >= 0;
	}
}

bool V1190BClient::convert(const RawEvent & iEvent, Event & oEvent) {
	assert(iEvent.length <= MAX_EVENT_LENGTH);
	assert(iEvent.length>=0);
	int tdcIndex=-1;
	oEvent.clear();
	oEvent.count=iEvent.count;
	unsigned tdcWordCount=0;
	bool error=false;
	typedef Event::Error Error;
	typedef Error::Type Type;
	for (int i=0; i<iEvent.length; i++) {
		uint32_t word=iEvent.words[i];
		uint32_t type=word >> 27;
		if (type==0x8) { // event header
			assert(i==0);
		} else if (type==0x11) {
			oEvent.triggerTime = word & ~(0x1f << 27); // First 27 bit. Last five bits for header.
		} else if (type==0x10){ // event trailer
			assert(i==iEvent.length-1);
			if ((word << 26) & 1) {
				oEvent.errors.push_back(Error(Error::EVENT_LOST));	
			}
			if ((word << 25) & 1) {
				oEvent.errors.push_back(Error(Error::OUTPUT_BUFFER_OVERFLOW));
			}
		} else if (type==0x1) { // TDC header
			assert(tdcIndex<0);
			if (tdcIndex>=0) error=true;
			tdcIndex= (word >> 24) & 0x3;
			assert(tdcIndex>=0);
			assert(tdcIndex<=1);
			tdcWordCount++;
		} else if (type==0x0) { // TDC measurement
			assert(tdcIndex>=0);
			if (tdcIndex<0) {
				error=true;
				continue;
			}
			assert((word >> 26) == 0 );
			Event::Measurement m;
			int channel= (word >> 19) & 0x7f; // 7 bit
			assert(channel>=0);
			assert(channel<64);
			m.channel=channel;
			if (1) {
				m.time=(word & 0x3ffff); // 18 bit
			} else {
				m.time=(word & 0x7ff); // 11 bit;
			}
			oEvent.measurements.push_back(m);
			tdcWordCount++;
		} else if (type==0x4) { //TDC Error
			int tdc=((word >> 24) & 0x3);
			cerr << "TDC " << tdc << " error: " << hex << (word & 0x7fff) << endl;
//			assert(tdcIndex == (word >> 24) & 0x3);
//			if (tdcIndex != tdc) {
//				cerr << "TDC "<<tdc<<" error was met in entry for TDC " << tdcIndex <<endl;
//			}
//			assert(tdcIndex == tdc);
//			if (tdcIndex!=tdc) error=true;
			tdcWordCount++;
			for (int i=0; i<12; ++i) {
				bool bit=(word >> i) & 1;
				if (!bit) continue;
				Type type;
				switch (i%3) {
				case 0: type=Error::HIT_LOST_READOUT; break;
				case 1: type=Error::HIT_LOST_L1; break;
				case 2: type=Error::HIT_ERROR; break;
				default: assert(false);
				}
				oEvent.errors.push_back(Error(type, tdc, i/3));
			}
			if ((word >> 12) & 1) {
				oEvent.errors.push_back(Error(Error::EVENT_OVERFLOW, tdc));
			}
			if ((word >> 13) & 1) {
				oEvent.errors.push_back(Error(Error::EVENT_LOST, tdc));
			}
			if ((word >> 14) & 1) {
				oEvent.errors.push_back(Error(Error::FATAL_CHIP_ERROR, tdc));
			}
		} else if (type==0x3) { // TDC trailer
			int tdc=((word >> 24) & 0x3);
			assert(tdcIndex == tdc);
			if (tdcIndex!=tdc) error=true;
			tdcWordCount++;
			assert(tdcWordCount==(word & 0x7f));
			tdcIndex=-1;
			tdcWordCount=0;
		} else {
			cerr << "V1190B: unknown word: " << hex << word << endl;
		}
	}
	return !error;
}


bool V1190BClient::readEvent(Event & oEvent) {
	RawEvent raw;
	start(); 
	if (!readRawEvent(raw)) return false;
	if (!convert(raw, oEvent)) return false;
	return true;
}

std::ostream & operator << (std::ostream & ostr, const V1190BClient::Event::Error & err) {
    assert(err.isValid());
	if (isGroupError(err.type())) {
		return ostr << typeToStr(err.type()) << " in tdc " << err.tdc() << ", group " <<
err.group();
	} else {
		return ostr << typeToStr(err.type()) << " tdc " << err.tdc();
	}
}

ostream & operator << (ostream & ostr, const V1190BClient::Event & event) {
	ostr << event.count << ": ";
	for (V1190BClient::Event::Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
		if (i!=event.measurements.begin()) {
			ostr << ", ";
		}
		ostr << *i;
	}
	if (event.errors.size()>0) {
		ostr << " Errors: " << event.errors;
	}
	return ostr;
}

std::ostream & operator << (std::ostream & ostr, const V1190BClient::Event::Errors & errs) {
	typedef V1190BClient::Event::Errors Errors;
	typedef V1190BClient::Event::Error Error;
	for (Errors::const_iterator i=errs.begin(); i!=errs.end(); ++i) {
		if (i!=errs.begin()) {
			ostr << ", ";
		}
		ostr << *i;
	}
	return ostr;
}
