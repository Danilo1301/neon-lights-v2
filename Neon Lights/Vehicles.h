#pragma once

#include "pch.h"

#include "Vehicle.h"

class Vehicles {
private:
	static std::map<CVehicle*, Vehicle*> m_Vehicles;
public:
	static void TryAddVehicle(CVehicle* veh);
	static bool HasVehicle(CVehicle* veh);
	static Vehicle* AddVehicle(CVehicle* veh);
	static Vehicle* GetVehicle(CVehicle* veh);
	static void RemoveVehicle(CVehicle* veh);
	static void RemoveAllVehicles();
	static void TryAddAllVehicles();
	static std::vector<Vehicle*> GetVehicles();
};