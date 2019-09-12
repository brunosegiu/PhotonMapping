#pragma once
#ifndef __PHOTON_MAP_H
#define __PHOTON_MAP_H

#include "Photon.h"
#include "PhotonsIterator.h"
#include <vector>

using namespace std;

/**
* Basic photon map implementation without irradiance caching.
* Implementation is based on the original code of Henrik Wann Jensen.
* Photons are stored in static balanced k-d tree in very compact form
* (single array) consuming minimal memory.
* This kind of map is usefull mainly for caustics maps.
*/

class PhotonMap {
	friend class Iterator;

public:
	class Iterator : public PhotonsIterator {
	protected:
		const PhotonMap * _pMap;
		unsigned          _actPhoton;

	public:
		Iterator(const PhotonMap *pm)
			: _pMap(pm), _actPhoton(1) {}

		virtual bool hasNext() {
			return (_actPhoton <= _pMap->_storedPhotons);
		}

		virtual Photon* next() {
			if (_actPhoton > _pMap->_storedPhotons)
				return NULL;
			Photon* ret = &(_pMap->_map[_actPhoton]);
			_actPhoton++;
			return ret;
		}
	};


protected:
	Photon*  _map;           //!< compact kd-tree with photons
	unsigned _storedPhotons; //!< number of photons in kd-tree
	float    _bboxMin[3]; 	 //!< bounding box of the scene
	float    _bboxMax[3];

	class NearestPhotons; //!< declaration

protected:
	void _build(const vector<Photon>&);
	void _median_split(Photon**, int, int, int, const int);
	void _balance_segment(Photon **, Photon **, int, int, int);
	void _locate_photons(NearestPhotons * const, unsigned) const;

public:
	/**
	* Implicit constructor creating empty map, see set() method.
	*/
	PhotonMap();

	/**
	* Constructor.
	*
	* @param photons Container of photons that will be stored in map.
	*/
	PhotonMap(const vector<Photon>& photons);

	/**
	* Destructor.
	*/
	virtual ~PhotonMap();

	/**
	* Deletes old photons and builds new map from given photons.
	*
	* @param photons New photons to store in map
	*/
	virtual void set(const vector<Photon>& photons);

	/**
	* @return Number of stored photons.
	*/
	virtual unsigned numPhotons(void) const;

	/**
	* Finds and returns the closes photons from the neightbourhood
	* of a surface point.
	*
	* @param spoint Surface point.
	* @param snormal Normalized normal vector of point 'spoint',
	*                can be NULL
	* @param maxdist Maximal distance.
	* @param nphotons Maxiaml number of returned photons.
	* @param irrad Avarage irradiance of returned photons,
	*              can be NULL
	* @return The closest photons from the 'spoint' neighbourhood or null.
	*/
	virtual vector<Photon*>* getNeighbourhood(const Vector3D& spoint,
		const Vector3D* snormal,
		float          maxdist,
		unsigned       nphotons
	) const;

	/**
	* Finds and returns average irradiance of photons from the closest
	* neighbourhood of given point.
	*
	* @param spoint Surface point.
	* @param snormal Normalized normal vector of point 'spoint',
	*                can be NULL
	* @param maxdist Maximal distance.
	* @param nphotons Maxiaml number of returned photons.
	* @param irrad Avarage irradiance set by this method
	* @return true on success, false if no photon is found
	*/
	virtual bool getIrradiance(const Vector3D& spoint,
		const Vector3D* snormal,
		float          maxdist,
		unsigned int nphotons,
		RGBA&       irrad, float minimun) const;

	/**
	* @return Iterator that allows to traverse the map photon by photon.
	*/
	virtual PhotonsIterator* iterator() const {
		return new PhotonMap::Iterator(this);
	}
};


/**
* Priority queue for the nearest photons.
*/
class PhotonMap::NearestPhotons {
public:
	int            max;
	int            found;
	bool           gotHeap;
	float          pos[3];
	float         *dist2;
	const Photon **index;
	const Photon  *photons; // original array of photons
	unsigned       halfStoredPhotons;

public:
	NearestPhotons(unsigned n, const Vector3D p, float maxDist,
		const Photon * pa, unsigned hsp)
		: max(n), found(0), gotHeap(false),
		photons(pa), halfStoredPhotons(hsp) {
		pos[0] = p.x; pos[1] = p.y; pos[2] = p.z;
		dist2 = new float[n + 1];
		index = new const Photon*[n + 1];
		dist2[0] = maxDist * maxDist;
	}

	~NearestPhotons() { delete[] dist2; delete[] index; }

	Photon& operator ()(int i) const {
		return (Photon&) *(index[i + 1]);
	}

	int numPhotons() const { return found - 1; }

	Vector3D pointLocation() const { return Vector3D(pos[0], pos[1], pos[2]); }
}; // class PhotoMap::NearestPhotons

#endif // __PHOTON_MAP_H