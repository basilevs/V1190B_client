/*
 * HistogramManager.h
 *
 *  Created on: 15.11.2010
 *      Author: gulevich
 */

#ifndef HISTOGRAMMANAGER_H_
#define HISTOGRAMMANAGER_H_

#include <map>
#include <sstream>
#include <TH1.h>

class HistogramManager:public std::map<int, TH1*> {
	std::auto_ptr<TH1> _prototype;
public:
	HistogramManager(std::auto_ptr<TH1> prototype):
		_prototype(prototype) {}
	~HistogramManager() {
		for (iterator i=begin(); i!=end(); i++) {
			delete i->second;
		}
	}
	Int_t Write(Int_t option = 0, Int_t bufsize = 0) const {
		Int_t rv = 0;
		for (const_iterator i=begin(); i!=end(); i++) {
			rv += i->second->Write(i->second->GetName(), option, bufsize);
		}
		return rv;
	}
	TH1 * operator[](int i) {
		iterator it=find(i);
		if (it!=end()) {
			return it->second;
		} else {
			assert(_prototype.get());
			TString name=_prototype->GetName();
			std::ostringstream numberstr;
			numberstr << _prototype->GetName() << i;
			TH1 * rv=static_cast<TH1*>(_prototype->Clone(numberstr.str().c_str()));
			assert(strcmp(rv->GetName(), numberstr.str().c_str())==0);
			insert(value_type(i, rv));
			return rv;
		}
	}
};


#endif /* HISTOGRAMMANAGER_H_ */
