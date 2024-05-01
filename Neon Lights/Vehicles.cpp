#include "Vehicles.h"

#include "Mod.h"
#include "LightGroups.h"

std::map<CVehicle*, Vehicle*> Vehicles::m_Vehicles;

void Vehicles::TryAddVehicle(CVehicle* veh)
{
	auto lightGroups = LightGroups::GetLightGroups(veh->m_nModelIndex);

	if (lightGroups.size() == 0) return;

	if (HasVehicle(veh)) return;

	auto modelId = veh->m_nModelIndex;

	//if (!LightGroups::HasLightGroupForModel(modelId)) return;

	AddVehicle(veh);
}

bool Vehicles::HasVehicle(CVehicle* vehicle)
{
	return m_Vehicles.find(vehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::AddVehicle(CVehicle* veh)
{
	Vehicle* vehicle = new Vehicle(veh);
	m_Vehicles.insert(std::pair<CVehicle*, Vehicle*>(veh, vehicle));

	Log::file << "[Vehicles] AddVehicle " << veh << ", ID " << veh->m_nModelIndex << std::endl;

	return vehicle;
}

Vehicle* Vehicles::GetVehicle(CVehicle* veh)
{
	if (!HasVehicle(veh)) return NULL;
	return m_Vehicles[veh];
}

void Vehicles::RemoveVehicle(CVehicle* veh)
{
	Log::file << "[Vehicles] Removed vehicle info " << veh << ", ID " << veh->m_nModelIndex << std::endl;

	auto it = m_Vehicles.find(veh);
	auto vehicle = it->second;

	m_Vehicles.erase(it);

	vehicle->Destroy();
	delete vehicle;
}

void Vehicles::RemoveAllVehicles()
{
	std::vector<CVehicle*> vec;
	for (auto p : m_Vehicles) vec.push_back(p.first);
	for (auto veh : vec) RemoveVehicle(veh);
}

void Vehicles::TryAddAllVehicles()
{
	for (auto veh : CPools::ms_pVehiclePool) TryAddVehicle(veh);
}

std::vector<Vehicle*> Vehicles::GetVehicles()
{
	std::vector<Vehicle*> vehicles;
	for (auto p : m_Vehicles) vehicles.push_back(p.second);
	return vehicles;
}