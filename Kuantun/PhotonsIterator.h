#pragma once
#ifndef __PHOTONS_ITERATOR_H
#define __PHOTONS_ITERATOR_H

#include "Photon.h"


/**
* Iterator that enables to traverse photon maps photon by photon.
*/

class PhotonsIterator {
public:
	virtual ~PhotonsIterator() {}

	/**
	* @return true if there is another uninspected photon,
	*         false otherwise
	*/
	virtual bool hasNext() = 0;

	/**
	* @return Actual photon or null and moves to the next photon
	*/
	virtual Photon* next() = 0;
};

#endif // __PHOTONS_ITERATOR_H

