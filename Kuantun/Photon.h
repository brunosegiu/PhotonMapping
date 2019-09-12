#pragma once
#ifndef __PHOTON_H
#define __PHOTON_H

#include <iostream>
#include "Vector3D.h"
#include "RGBA.h"

/**
* Photon storable in photon map.
*/

class Photon {
	friend class PhotonMap; // doe to efficiency ;-)

public:
	static const double COS_THETA[256];
	static const double SIN_THETA[256];
	static const double COS_PHI[256];
	static const double SIN_PHI[256];

	float         _position[3];
	RGBA       _power;
	unsigned char _phi;   //!< azimuth angle with respect to Z coord !!!
	unsigned char _theta; //!< elevation angle with respect to Z coord !!!
	short         _plane;

public:
	/**
	* Implicit constructor sets all values to zero
	*/
	Photon();

	/**
	* Constructor.
	*
	* @param pos Photon's location
	* @param dir Photon's normalized directon
	* @param pow Phoron's power (color)
	*/
	Photon(Vector3D& pos, Vector3D& dir, RGBA& pow);

	/**
	* Cloning constructor
	*
	* @param src Source photon
	*/
	Photon(const Photon& src);

	Photon& operator =(const Photon&);

	virtual ~Photon() {}

	/**
	* @return Photon's direction in world coordinates
	*/
	Vector3D getDirection() const {
		return Vector3D(SIN_THETA[_theta] * COS_PHI[_phi],
			SIN_THETA[_theta] * SIN_PHI[_phi],
			COS_THETA[_theta]
		);
	}

	/**
	* @return Photon's location in the world coordinates
	*/
	Vector3D getLocation() const {
		return Vector3D(_position[0],
			_position[1],
			_position[2]);
	}

	/**
	* @return Photon's power
	*/
	RGBA getPower() const { return _power; }

	/**
	* Sets nre location.
	*
	* @param loc New location
	*/
	void setLocation(const Vector3D&  loc) {
		_position[0] = loc.x;
		_position[1] = loc.y;
		_position[2] = loc.z;
	}

	/**
	* @param out ostream
	* @param photon photon to print out
	* @return reference to changed output stream
	*/
	friend std::ostream& operator <<(std::ostream& out, const Photon& photon) {
		out << "Location:  " << photon.getLocation().x << " " << photon.getLocation().y << " " << photon.getLocation().z << std::endl;
		out << "Direction: " << photon.getDirection().x << " " << photon.getDirection().y << " " << photon.getDirection().z << " " << std::endl;
		out << "Power:     " << photon._power.r << " " << photon._power.g << " " << photon._power.b << std::endl;
		out << "Plane:     " << photon._plane << std::endl;
		return out;
	}

};

#endif // __PHOTON_H

