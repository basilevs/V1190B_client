#ifndef V1190BCLIENT_H_
#define V1190BCLIENT_H_

#include <istream>
#include <memory>
#include <vector>

#include "socketbuf.h"

/** Reads and interprets data from V1190B server
 * Can also make some configuration changes.
 * Main methods are constructor and readEvent()
 */
class V1190BClient {
	typedef std::string string;
	string _host;
	int _port;
	std::auto_ptr<socketwrapper> _linuxsocket;
	std::auto_ptr<socketbuf> _streambuf;
	bool _started, _highSpeedCoreClock;
	void checkConnection();
public:
	/** Constructor
	 * @arg host - hostname or IP address of server
	 * @arg port - a TCP/IP port of server
	 *
	 * TCP/IP port may be forwarded to server running on VME controller from any other host. This can be done with SSH, iptables or other tools.
	 * No connection is done during construction. Use connect() for this.
	 */
	V1190BClient(const string & host, int port);
	V1190BClient();
	/** Connects to a host and port given in constructor arguments
	 * Use this before operator bool() if you want it to return truth.
	 */
	void connect();
	void connect(const char * host, int port);
	/** False if no more events can be read or no connection is established
	 * This is the case, when something bad had happens on server side or connection is interrupted
	 */
	operator bool() const;
	/** Software reset and buffer clear.
	 * A common error is to consider buffer empty after this - buffer will be instantly filled with events if trigger is not disabled
	 */
	void clear();
	void lowSpeedCoreClock();
	/** Sets dead time in nanoseconds.
	 *  Following values are accepted:
	 *   5
	 *   10
	 *   30
	 *   100
	 *  Any another value is converted to the nearest greater of listed.
	 */
	void setDeadTime(int time);
	void setDllClock(unsigned mode);
	virtual ~V1190BClient();
	struct Event {
		struct Measurement {
			int channel;
			int time;
			int gChannel() const {return channel;}
		};
		class Error {
		public:
			enum Type {
				HIT_LOST_READOUT, // tdc() and group() are set
				HIT_LOST_L1,      // tdc() and group() are set
				HIT_ERROR,        // tdc() and group() are set
				EVENT_OVERFLOW,
				EVENT_LOST,
				FATAL_CHIP_ERROR,
				OUTPUT_BUFFER_OVERFLOW
			};
			int tdc() const {return _tdc;}
			int group() const {return _group;}
			Type type() const {return _type;}
			bool isValid() const;
		private:
			friend class V1190BClient;
			Error(Type type, int tdc=-1, int group=-1);
			int _tdc;
			int _group;
			Type _type;
		};
		void clear() {measurements.clear(); errors.clear(); triggerTime = 0;}
		typedef std::vector<Measurement> Measurements;
		typedef std::vector<Error> Errors;
		unsigned int count;
		Measurements measurements;
		Errors errors;
		// Trigger time 800 ns scale
		unsigned long triggerTime;
	};
	/** Fills oEvent with the next read
	 * Tries to connect, if no connection has been established already
	 * @return false in case of problems
	 */
	bool readEvent(Event & oEvent);
	/** Sets offset of matching window start relative to trigger
	 * Counter reset is a side effect of this TDC configuration change.
	 * offset should be in [-2048, 40]
	 * width should be in [1, 4095]
	 * Each tick is approximately 25 ns
	 */
	void setWindow(int offset, int width);
	void noop();
	void start();
	void disconnect();
private:
	void sendCoreClock();
	struct RawEvent;
	bool readRawEvent(RawEvent & oEvent);
	static bool convert(const RawEvent & iEvent, Event & oEvent);
};

bool isGroupError(V1190BClient::Event::Error::Type t);
const char * typeToStr(V1190BClient::Event::Error::Type t);

#include <ostream>
#include <assert.h>

std::ostream & operator << (std::ostream & ostr, const V1190BClient::Event::Error & err);
std::ostream & operator << (std::ostream & ostr, const V1190BClient::Event::Errors & err);
inline std::ostream & operator << (std::ostream & ostr, const V1190BClient::Event::Measurement & measurement) {
	return ostr << measurement.gChannel() << ":" << measurement.time;
}
std::ostream & operator << (std::ostream & ostr, const V1190BClient::Event & event);



#endif /* V1190BCLIENT_H_ */
