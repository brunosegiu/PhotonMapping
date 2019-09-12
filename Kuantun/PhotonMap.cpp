#include "PhotonMap.h"

#define swap(ph,a,b) { Photon *ph2=ph[a]; ph[a]=ph[b]; ph[b]=ph2; }

void PhotonMap::_build(const vector<Photon>& photons)
{
	_storedPhotons = photons.size();

#ifdef WIN32
	_map = (Photon*)malloc((_storedPhotons + 1) * sizeof(Photon));
#else
	_map = new Photon[_storedPhotons + 1];
#endif

	if (!_map) {
		_storedPhotons = 0;
		fprintf(stderr, "PhotonMap::_build(): Out of memory initializing photon map\n");
		return;
	}

	_bboxMin[0] = _bboxMin[1] = _bboxMin[2] = 1e8f;
	_bboxMax[0] = _bboxMax[1] = _bboxMax[2] = -1e8f;

	for (int i = 0; i < unsigned (photons.size()); i++) {
		_map[i] = photons[i];
		for (register unsigned j = 0; j < 3; j++) {
			if (photons[i]._position[j] < _bboxMin[j])
				_bboxMin[j] = photons[i]._position[j];
			if (photons[i]._position[j] > _bboxMax[j])
				_bboxMax[j] = photons[i]._position[j];
		}
	}

	/*
	* Creates a left balanced kd-tree from the flat photon array.
	* This function should be called before the photon map
	* is used for rendering.
	*/

	if (_storedPhotons <= 1) return;

	// allocate two temporary arrays for the balancing procedure
	Photon **pa1 = (Photon**)malloc(sizeof(Photon*)*(_storedPhotons + 1));
	Photon **pa2 = (Photon**)malloc(sizeof(Photon*)*(_storedPhotons + 1));

	for (register unsigned i = 0; i <= _storedPhotons; i++) pa2[i] = &_map[i];

	_balance_segment(pa1, pa2, 1, 1, _storedPhotons);
	free(pa2);

	// reorganize balanced kd-tree (make a heap)
	unsigned d, j = 1, foo = 1;
	Photon foo_photon = _map[j];

	for (register unsigned i = 1; i <= _storedPhotons; i++) {
		d = pa1[j] - _map;
		pa1[j] = NULL;
		if (d != foo)
			_map[j] = _map[d];
		else {
			_map[j] = foo_photon;
			if (i < _storedPhotons) {
				for (; foo <= _storedPhotons; foo++)
					if (pa1[foo] != NULL) break;
				foo_photon = _map[foo];
				j = foo;
			}
			continue;
		}
		j = d;
	}
	free(pa1);
}

// median_split splits the photon array into two separate
// pieces around the median with all photons below the
// the median in the lower half and all photons above
// than the median in the upper half. The comparison
// criteria is the axis (indicated by the axis parameter)
// (inspired by routine in "Algorithms in C++" by Sedgewick)
void PhotonMap::_median_split(Photon **p, int start,
	int end, int median, int axis)
{
	int left = start;
	int right = end;

	while (right > left) {
		float v = p[right]->_position[axis];
		int i = left - 1;
		int j = right;
		for (;;) {
			while (p[++i]->_position[axis] < v);
			while (p[--j]->_position[axis] > v && j>left);
			if (i >= j) break;
			swap(p, i, j);
		}

		swap(p, i, right);
		if (i >= median) right = i - 1;
		if (i <= median) left = i + 1;
	}
}


// See "Realistic image synthesis using Photon Mapping" chapter 6
// for an explanation of this function
void PhotonMap::_balance_segment(Photon **pbal, Photon **porg,
	int index, int start, int end)
{
	//--------------------
	// compute new median
	//--------------------

	int median = 1;
	while ((4 * median) <= (end - start + 1))
		median += median;

	if ((3 * median) <= (end - start + 1)) {
		median += median;
		median += start - 1;
	}
	else
		median = end - median + 1;

	//--------------------------
	// find axis to split along
	//--------------------------

	int axis = 2;
	if ((_bboxMax[0] - _bboxMin[0]) > (_bboxMax[1] - _bboxMin[1]) &&
		(_bboxMax[0] - _bboxMin[0]) > (_bboxMax[2] - _bboxMin[2]))
		axis = 0;
	else if ((_bboxMax[1] - _bboxMin[1]) > (_bboxMax[2] - _bboxMin[2]))
		axis = 1;

	//------------------------------------------
	// partition photon block around the median
	//------------------------------------------

	_median_split(porg, start, end, median, axis);

	pbal[index] = porg[median];
	pbal[index]->_plane = axis;

	//----------------------------------------------
	// recursively balance the left and right block
	//----------------------------------------------

	if (median > start) {
		// balance left segment
		if (start < median - 1) {
			float tmp = _bboxMax[axis];
			_bboxMax[axis] = pbal[index]->_position[axis];
			_balance_segment(pbal, porg, 2 * index, start, median - 1);
			_bboxMax[axis] = tmp;
		}
		else {
			pbal[2 * index] = porg[start];
		}
	}

	if (median < end) {
		// balance right segment
		if (median + 1 < end) {
			float tmp = _bboxMin[axis];
			_bboxMin[axis] = pbal[index]->_position[axis];
			_balance_segment(pbal, porg, 2 * index + 1, median + 1, end);
			_bboxMin[axis] = tmp;
		}
		else {
			pbal[2 * index + 1] = porg[end];
		}
	}
}

void PhotonMap::_locate_photons(NearestPhotons * const np,
	unsigned               index) const
{
	const Photon* p = &(np->photons[index]);
	float         dist1;
	unsigned      i2;

	if (index < np->halfStoredPhotons) { // inner node
		dist1 = np->pos[p->_plane] - p->_position[p->_plane];
		i2 = 2 * index;

		if (dist1 > 0.) { // search right plane first
			_locate_photons(np, i2 + 1);
			if (dist1*dist1 < np->dist2[0]) _locate_photons(np, i2);
		}
		else { // search left plane first
			_locate_photons(np, i2);
			if (dist1*dist1 < np->dist2[0]) _locate_photons(np, i2 + 1);
		}
	}

	// compute squared distance between current photon an np.pos
	dist1 = p->_position[0] - np->pos[0];
	float dist2 = dist1*dist1;
	dist1 = p->_position[1] - np->pos[1];
	dist2 += dist1*dist1;
	dist1 = p->_position[2] - np->pos[2];
	dist2 += dist1*dist1;

	if (dist2 < np->dist2[0]) {
		// we found a photon, insert it in the candidate list

		if (np->found < np->max) {
			// heap is not full; use array
			np->found++;
			np->dist2[np->found] = dist2;
			np->index[np->found] = p;
		}
		else {
			int j, parent;

			if (!np->gotHeap) { // Do we need to build the heap?
								// Build heap
				float dst2;
				const Photon *phot;
				int half_found = np->found >> 1;
				for (int k = half_found; k >= 1; k--) {
					parent = k;
					phot = np->index[k];
					dst2 = np->dist2[k];
					while (parent <= half_found) {
						j = parent + parent;
						if (j<np->found && np->dist2[j]<np->dist2[j + 1]) j++;
						if (dst2 >= np->dist2[j]) break;
						np->dist2[parent] = np->dist2[j];
						np->index[parent] = np->index[j];
						parent = j;
					}
					np->dist2[parent] = dst2;
					np->index[parent] = phot;
				}
				np->gotHeap = true;
			}

			// insert new photon into max heap
			// delete largest element, insert new and reorder the heap

			parent = 1;
			j = 2;
			while (j <= np->found) {
				if (j < np->found && np->dist2[j] < np->dist2[j + 1]) j++;
				if (dist2 > np->dist2[j]) break;
				np->dist2[parent] = np->dist2[j];
				np->index[parent] = np->index[j];
				parent = j;
				j += j;
			}
			np->index[parent] = p;
			np->dist2[parent] = dist2;
			np->dist2[0] = np->dist2[1];
		}
	}
}







PhotonMap::PhotonMap()
	: _map(NULL), _storedPhotons(0)
{
	_bboxMin[0] = _bboxMin[1] = _bboxMin[2] = 1e8f;
	_bboxMax[0] = _bboxMax[1] = _bboxMax[2] = -1e8f;
}

PhotonMap::PhotonMap(const vector<Photon>& photons)
	: _map(NULL), _storedPhotons(0)
{
	_build(photons);
}

PhotonMap::~PhotonMap()
{
	if (_map) delete[] _map;
}

void PhotonMap::set(const vector<Photon>& photons)
{
	if (_map) delete[] _map;
	_build(photons);
}

unsigned PhotonMap::numPhotons() const
{
	return _storedPhotons;
}

vector<Photon*>* PhotonMap::getNeighbourhood(const Vector3D& spoint,
	const Vector3D* snormal,
	float          maxdist,
	unsigned       nphotons) const
{
	
	if (_storedPhotons == 0) return NULL;
	NearestPhotons* np = new NearestPhotons(nphotons, spoint, maxdist,
		_map, _storedPhotons / 2 - 1);
	_locate_photons(np, 1); // locate the nearest photons

	vector<Photon*> * ret = new vector<Photon*>();
	for (int i = 0; i < np->numPhotons(); i++) {
		if (snormal) {
			if (snormal->GetDot((*np)(i).getDirection()) > 0.) {
				ret->push_back(&(*np)(i));
			}
		}
		else {
			ret->push_back(&(*np)(i));
		}
	}
	delete np;
	return ret;
}

bool PhotonMap::getIrradiance(const Vector3D& spoint,const Vector3D* snormal,float maxdist,unsigned int nphotons,RGBA& irrad, float minimun) const{
	float area = (M_PI * maxdist * maxdist);
	irrad=RGBA(0.0, 0.0, 0.0);
	if (_storedPhotons == 0) return false;
	NearestPhotons* np = new NearestPhotons(nphotons, spoint, maxdist,_map, _storedPhotons / 2 - 1);
	_locate_photons(np, 1); // locate the nearest photons

	if (np->found < minimun) {
		delete np;
		return false;
	}
	for (int i = 0; i < np->numPhotons(); i++) {
		if (snormal) {
			if (snormal->GetDot((*np)(i).getDirection()) > 0.) {
				//float photonDist = spoint.GetDistance((*np)(i).getLocation());
				irrad = irrad + (*np)(i)._power;// / (photonDist * photonDist);
			}
		}
		else {
			//float photonDist = spoint.GetDistance((*np)(i).getLocation());
			irrad = irrad + (*np)(i)._power;// / (photonDist * photonDist);
		}
	}
	irrad = irrad / area;
	delete np;
	return true;
}
