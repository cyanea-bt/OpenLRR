// Object.cpp : 
//

#include "../../engine/core/Maths.h"
#include "../../engine/core/Utils.h"
#include "../../engine/input/Input.h"

#include "../audio/SFX.h"
#include "../effects/DamageText.h"
#include "../effects/Effects.h"
#include "../front/Reward.h"
#include "../interface/hud/Bubbles.h"
#include "../interface/Encyclopedia.h"
#include "../interface/HelpWindow.h"
#include "../interface/InfoMessages.h"
#include "../interface/Interface.h"
#include "../interface/Pointers.h"
#include "../interface/RadarMap.h"
#include "../interface/TextMessages.h"
#include "../mission/Messages.h"
#include "../mission/NERPsFile.h"
#include "../world/Construction.h"
#include "../world/ElectricFence.h"
#include "../Debug.h"
#include "../Game.h"
#include "AITask.h"
#include "Flocks.h"
#include "Stats.h"
#include "Object.h"



/**********************************************************************************
 ******** Constants
 **********************************************************************************/

#pragma region Constants

// Logic for how often survey scan is performed, and speed at which dots extend out on radar map.
#define OBJECT_SURVEY_CYCLEDURATION		(STANDARD_FRAMERATE * 1.0f)

#pragma endregion

/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// Static variable used in LegoObject_Callback_Update.
// <LegoRR.exe @004a58b0>
real32 & LegoRR::s_objectDamageShakeTimer = *(real32*)0x004a58b0; // = 1.0f;

// <LegoRR.exe @004df790>
LegoRR::LegoObject_Globs & LegoRR::objectGlobs = *(LegoRR::LegoObject_Globs*)0x004df790;

// Static variable used in LegoObject_Callback_Update. Assigned to but never used.
// <LegoRR.exe @00557090>
LegoRR::LegoObject* (& LegoRR::s_currentUpdateObject) = *(LegoRR::LegoObject**)0x00557090;

LegoRR::LegoObject_ListSet LegoRR::objectListSet = LegoRR::LegoObject_ListSet(LegoRR::objectGlobs);

#pragma endregion

/**********************************************************************************
 ******** Class Functions
 **********************************************************************************/

#pragma region Class Functions

bool LegoRR::LegoObject_ListSet::FilterSkipUpgradeParts(const LegoObject* liveObj)
{
	return ListSet::IsAlive(liveObj) && !(liveObj->flags3 & LIVEOBJ3_UPGRADEPART);
}

LegoRR::LegoObject_ListSet::enumerable<LegoRR::LegoObject_ListSet::FilterSkipUpgradeParts> LegoRR::LegoObject_ListSet::EnumerateSkipUpgradeParts()
{
	return this->EnumerateWhere<FilterSkipUpgradeParts>();
}

#pragma endregion

/**********************************************************************************
 ******** Functions
 **********************************************************************************/

#pragma region Functions

// <LegoRR.exe @00436ee0>
void __cdecl LegoRR::LegoObject_Initialise(void)
{
	std::memset(&objectGlobs, 0, 0xc644); // Exclude end of struct holding non-glob fields. // sizeof(objectGlobs));

	Activity_RegisterName(Activity_Stand);
	Activity_RegisterName(Activity_Route);
	Activity_RegisterName(Activity_RouteRubble);
	Activity_RegisterName(Activity_RunPanic);
	Activity_RegisterName(Activity_Drill);
	Activity_RegisterName(Activity_Teleport);
	Activity_RegisterName(Activity_Walk);
	Activity_RegisterName(Activity_Reinforce);
	Activity_RegisterName(Activity_Reverse);
	Activity_RegisterName(Activity_TurnLeft);
	Activity_RegisterName(Activity_TurnRight);
	Activity_RegisterName(Activity_CantDo);
	Activity_RegisterName(Activity_Emerge);
	Activity_RegisterName(Activity_Enter);
	Activity_RegisterName(Activity_EnterRein);
	Activity_RegisterName(Activity_Collect);
	Activity_RegisterName(Activity_Gather);
	Activity_RegisterName(Activity_Carry);
	Activity_RegisterName(Activity_CarryRubble);
	Activity_RegisterName(Activity_Throw);
	Activity_RegisterName(Activity_CarryTurnLeft);
	Activity_RegisterName(Activity_CarryTurnRight);
	Activity_RegisterName(Activity_CarryStand);
	Activity_RegisterName(Activity_HitLeft);
	Activity_RegisterName(Activity_HitRight);
	Activity_RegisterName(Activity_HitFront);
	Activity_RegisterName(Activity_HitBack);
	Activity_RegisterName(Activity_HitHard);
	Activity_RegisterName(Activity_Dynamite);
	Activity_RegisterName(Activity_Deposit);
	Activity_RegisterName(Activity_Clear);
	Activity_RegisterName(Activity_Place);
	Activity_RegisterName(Activity_Repair);
	Activity_RegisterName(Activity_Slip);
	Activity_RegisterName(Activity_Rest);
	Activity_RegisterName(Activity_Eat);
	Activity_RegisterName(Activity_Stamp);
	Activity_RegisterName(Activity_ThrowMan);
	Activity_RegisterName(Activity_ThrownByRockMonster);
	Activity_RegisterName(Activity_GetUp);
	Activity_RegisterName(Activity_BuildPath);
	Activity_RegisterName(Activity_Upgrade);
	Activity_RegisterName(Activity_Explode);
	Activity_RegisterName(Activity_Unpowered);
	Activity_RegisterName(Activity_FireLaser);
	Activity_RegisterName(Activity_Freezed);
	Activity_RegisterName(Activity_FreezeStart);
	Activity_RegisterName(Activity_FreezeMelt);
	Activity_RegisterName(Activity_Recharge);
	Activity_RegisterName(Activity_WakeUp);
	Activity_RegisterName(Activity_Train);
	Activity_RegisterName(Activity_FloatOn);
	Activity_RegisterName(Activity_FloatOff);
	Activity_RegisterName(Activity_Opening);
	Activity_RegisterName(Activity_Closing);
	Activity_RegisterName(Activity_Open);
	Activity_RegisterName(Activity_Waiting1);
	Activity_RegisterName(Activity_Waiting2);
	Activity_RegisterName(Activity_Waiting3);
	Activity_RegisterName(Activity_Waiting4);
	Activity_RegisterName(Activity_Waiting5);
	Activity_RegisterName(Activity_Waiting6);
	Activity_RegisterName(Activity_Waiting7);
	Activity_RegisterName(Activity_Waiting8);
	Activity_RegisterName(Activity_Waiting9);
	Activity_RegisterName(Activity_Waiting10);
	Activity_RegisterName(Activity_Waiting11);
	Activity_RegisterName(Activity_Waiting12);
	Activity_RegisterName(Activity_Waiting13);
	Activity_RegisterName(Activity_Waiting14);
	Activity_RegisterName(Activity_Waiting15);
	Activity_RegisterName(Activity_Waiting16);
	Activity_RegisterName(Activity_Waiting17);
	Activity_RegisterName(Activity_Waiting18);
	Activity_RegisterName(Activity_Waiting19);
	Activity_RegisterName(Activity_Waiting20);
	Activity_RegisterName(Activity_Waiting21);
	Activity_RegisterName(Activity_Crumble);
	Activity_RegisterName(Activity_TeleportIn);

	AbilityType_RegisterName(LegoObject_AbilityType_Pilot);
	AbilityType_RegisterName(LegoObject_AbilityType_Sailor);
	AbilityType_RegisterName(LegoObject_AbilityType_Driver);
	AbilityType_RegisterName(LegoObject_AbilityType_Dynamite);
	AbilityType_RegisterName(LegoObject_AbilityType_Repair);
	AbilityType_RegisterName(LegoObject_AbilityType_Scanner);

	objectGlobs.toolNullIndex[LegoObject_ToolType_Drill]      = 0;
	objectGlobs.toolNullIndex[LegoObject_ToolType_Spade]      = 0;
	objectGlobs.toolNullIndex[LegoObject_ToolType_Hammer]     = 1;
	objectGlobs.toolNullIndex[LegoObject_ToolType_Spanner]    = 1;
	objectGlobs.toolNullIndex[LegoObject_ToolType_Laser]      = 4;
	objectGlobs.toolNullIndex[LegoObject_ToolType_PusherGun]  = 4;
	objectGlobs.toolNullIndex[LegoObject_ToolType_BirdScarer] = 4;
	objectGlobs.toolNullIndex[LegoObject_ToolType_FreezerGun] = 4;
	objectGlobs.toolNullIndex[LegoObject_ToolType_Barrier]    = 3;
	objectGlobs.toolNullIndex[LegoObject_ToolType_Dynamite]   = 2;
	objectGlobs.toolNullIndex[LegoObject_ToolType_CryOre]     = 4;

	ToolType_RegisterName(LegoObject_ToolType_Drill);
	ToolType_RegisterName(LegoObject_ToolType_Spade);
	ToolType_RegisterName(LegoObject_ToolType_Hammer);
	ToolType_RegisterName(LegoObject_ToolType_Spanner);
	ToolType_RegisterName(LegoObject_ToolType_Laser);
	ToolType_RegisterName(LegoObject_ToolType_PusherGun);
	ToolType_RegisterName(LegoObject_ToolType_BirdScarer);
	ToolType_RegisterName(LegoObject_ToolType_FreezerGun);

	/// NOTE: Special tools that DO NOT have their names registered.
	//ToolType_RegisterName(LegoObject_ToolType_Barrier);
	//ToolType_RegisterName(LegoObject_ToolType_Dynamite);
	//ToolType_RegisterName(LegoObject_ToolType_CryOre);

	objectListSet.Initialise();
	objectGlobs.flags = LegoObject_GlobFlags::OBJECT_GLOB_FLAG_INITIALISED;
}

// <LegoRR.exe @00437310>
void __cdecl LegoRR::LegoObject_Shutdown(void)
{
	LegoObject_RemoveAll();

	if (objectGlobs.routeBuildListScores != nullptr) {
		Gods98::Mem_Free(objectGlobs.routeBuildListScores);
	}

	/// REFACTOR: Moved below freeing `routeBuildListScores`.
	objectListSet.Shutdown();
	objectGlobs.flags = LegoObject_GlobFlags::OBJECT_GLOB_FLAG_NONE;

	// Yup, this assignment really exists after the above one (originally) next to objectListSet.Shutdown()....
	// It has no effect, so we can keep it commented out.
	//objectGlobs.flags &= ~LegoObject_GlobFlags::OBJECT_GLOB_FLAG_INITIALISED;
}

// <LegoRR.exe @00437370>
void __cdecl LegoRR::Object_Save_CopyStruct18(OUT SaveStruct_18* saveStruct18)
{
	*saveStruct18 = objectGlobs.savestruct18_c01c;
}

// <LegoRR.exe @00437390>
void __cdecl LegoRR::Object_Save_OverwriteStruct18(OPTIONAL const SaveStruct_18* saveStruct18)
{
	if (saveStruct18 != nullptr) {
		objectGlobs.savestruct18_c01c = *saveStruct18;
	}
	else {
		std::memset(&objectGlobs.savestruct18_c01c, 0, sizeof(SaveStruct_18));
	}
}

// <LegoRR.exe @004373c0>
uint32 __cdecl LegoRR::LegoObject_GetObjectsBuilt(LegoObject_Type objType, bool32 excludeToolStore)
{
	uint32 builtCount = 0;

	/// FIX APPLY: Don't enumerate through object IDs that don't exist!
	const uint32 objIDCount = Lego_GetObjectTypeIDCount(objType); // LegoObject_ID_Count;
	for (uint32 objID = 0; objID < objIDCount; objID++) {

		/// REFACTOR: Move loop inside check, since it doesn't require objLevel.
		if (!excludeToolStore || !(Stats_GetStatsFlags1(objType, (LegoObject_ID)objID) & STATS1_STOREOBJECTS)) {

			// Still enumerate through max levels for objects in-case there's any jank with non-standard object types.
			for (uint32 objLevel = 0; objLevel < OBJECT_MAXLEVELS; objLevel++) {
				builtCount += LegoObject_GetLevelObjectsBuilt(objType, (LegoObject_ID)objID, objLevel, true);
			}
		}
	}
	return builtCount;
}

// <LegoRR.exe @00437410>
void __cdecl LegoRR::Object_LoadToolTipIcons(const Gods98::Config* config)
{
	const Gods98::Config* arrayFirst = Gods98::Config_FindArray(config, Lego_ID("ToolTipIcons"));

	for (const Gods98::Config* prop = arrayFirst; prop != nullptr; prop = Gods98::Config_GetNextItem(prop)) {
		bool found = false;

		for (uint32 toolType = 0; !found && toolType < LegoObject_ToolType_Count; toolType++) {
			const char* toolName = legoGlobs.toolName[toolType];
			if (toolName != nullptr && ::_stricmp(toolName, Gods98::Config_GetItemName(prop)) == 0) {
				objectGlobs.ToolTipIcons_Tools[toolType] = Gods98::Image_LoadBMP(Gods98::Config_GetDataString(prop));
				found = true;
			}
		}

		for (uint32 abilityType = 0; !found && abilityType < LegoObject_AbilityType_Count; abilityType++) {
			const char* abilityName = objectGlobs.abilityName[abilityType];
			if (abilityName != nullptr && ::_stricmp(abilityName, Gods98::Config_GetItemName(prop)) == 0) {
				objectGlobs.ToolTipIcons_Abilities[abilityType] = Gods98::Image_LoadBMP(Gods98::Config_GetDataString(prop));
				found = true;
			}
		}

		if (!found) {
			if (::_stricmp(Gods98::Config_GetItemName(prop), "Blank") == 0) {
				objectGlobs.ToolTipIcon_Blank = Gods98::Image_LoadBMP(Gods98::Config_GetDataString(prop));
				found = true;
			}
			else if (::_stricmp(Gods98::Config_GetItemName(prop), "Ore") == 0) {
				objectGlobs.ToolTipIcon_Ore = Gods98::Image_LoadBMP(Gods98::Config_GetDataString(prop));
				found = true;
			}
			else {
				Config_WarnItemF(true, prop, "Unknown ToolTipIcon %s", Gods98::Config_GetItemName(prop));
			}
		}
	}

	if (objectGlobs.ToolTipIcon_Ore != nullptr) {
		Gods98::Image_SetupTrans(objectGlobs.ToolTipIcon_Ore, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	}
}

// <LegoRR.exe @00437560>
void __cdecl LegoRR::LegoObject_CleanupLevel(void)
{
	LegoObject_RemoveAll();
	LegoObject_CleanupObjectLevels();
	HiddenObject_RemoveAll();

	std::memset(objectGlobs.slugHoleBlocks, 0, sizeof(objectGlobs.slugHoleBlocks));
	std::memset(objectGlobs.rechargeSeamBlocks, 0, sizeof(objectGlobs.rechargeSeamBlocks));

	objectGlobs.slugHoleCount = 0;
	objectGlobs.rechargeSeamCount = 0;
	objectGlobs.dischargeBuildup = 0.0f;

	std::memset(objectGlobs.liveObjArray100_c43c, 0, sizeof(objectGlobs.liveObjArray100_c43c));

	objectGlobs.uintCount_c5cc = 0;

	LegoObject_SetNumBuildingsTeleported(0);
}

// Used for consuming and producing unpowered crystals after weapon discharge.
// <LegoRR.exe @004375c0>
void __cdecl LegoRR::LegoObject_Weapon_FUN_004375c0(LegoObject* liveObj, sint32 weaponID, real32 coef)
{
	real32 rate = coef;
	if (weaponID != -1) {
		rate *= Weapon_GetDischargeRate(weaponID);
	}
	objectGlobs.dischargeBuildup += rate;

	Point2I blockPos = { 0 }; // dummy init
	LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

	while (objectGlobs.dischargeBuildup >= 1.0f) {
		LegoObject* crystalObj = LegoObject_FUN_00438d20(&blockPos, LegoObject_PowerCrystal, (LegoObject_ID)0, 0);
		if (crystalObj == nullptr) break;

		crystalObj = LegoObject_SpawnCarryableObject(crystalObj, LegoObject_PowerCrystal, (LegoObject_ID)0, 0);
		if (crystalObj == nullptr) break;

		crystalObj->flags3 |= LIVEOBJ3_POWEROFF;
		LegoObject_SetCrystalPoweredColour(crystalObj, false);
		objectGlobs.dischargeBuildup -= 1.0;
	}
}

// <LegoRR.exe @00437690>
bool32 __cdecl LegoRR::LegoObject_DoOpeningClosing(LegoObject* liveObj, bool32 open)
{
	if (open && !(liveObj->flags4 & LIVEOBJ4_UNK_2000)) {
		liveObj->flags4 |= LIVEOBJ4_UNK_1000;
		LegoObject_SetActivity(liveObj, Activity_Opening, 0);
	}
	else if (!open && (liveObj->flags4 & LIVEOBJ4_UNK_2000)) {
		liveObj->flags4 &= ~LIVEOBJ4_UNK_2000;
		liveObj->flags4 |= LIVEOBJ4_UNK_4000;
		LegoObject_SetActivity(liveObj, Activity_Closing, 0);
	}

	return LegoObject_UpdateActivityChange(liveObj);
}

// <LegoRR.exe @00437700>
void __cdecl LegoRR::LegoObject_CleanupObjectLevels(void)
{
	std::memset(objectGlobs.objectPrevLevels, 0, sizeof(objectGlobs.objectPrevLevels));
	std::memset(objectGlobs.objectTotalLevels, 0, sizeof(objectGlobs.objectTotalLevels));
}

// <LegoRR.exe @00437720>
uint32 __cdecl LegoRR::LegoObject_GetLevelObjectsBuilt(LegoObject_Type objType, LegoObject_ID objID, uint32 objLevel, bool32 currentCount)
{
	uint32 levelsCount = objectGlobs.objectTotalLevels[objType][objID][objLevel];
	if (currentCount) {
		// Subtract objects that have been removed from the total.
		levelsCount -= objectGlobs.objectPrevLevels[objType][objID][objLevel];
	}
	return levelsCount;
}

// <LegoRR.exe @00437760>
uint32 __cdecl LegoRR::LegoObject_GetPreviousLevelObjectsBuilt(LegoObject_Type objType, LegoObject_ID objID, uint32 objLevel)
{
	return objectGlobs.objectPrevLevels[objType][objID][objLevel];
}

// <LegoRR.exe @00437790>
void __cdecl LegoRR::LegoObject_IncLevelPathsBuilt(bool32 incCurrent)
{
	if (incCurrent) {
		objectGlobs.objectTotalLevels[LegoObject_Path][0][0]++;
	}
	else {
		objectGlobs.objectPrevLevels[LegoObject_Path][0][0]++;
	}
}

// Removes all route-to references that match the specified object.
// Does nothing if routeToObj is a Boulder type.
// <LegoRR.exe @004377b0>
void __cdecl LegoRR::LegoObject_RemoveRouteToReferences(LegoObject* routeToObj)
{
	if (routeToObj->type != LegoObject_Boulder) {
		for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {
			LegoObject_Callback_RemoveRouteToReference(obj, routeToObj);
		}
		//LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_Callback_RemoveRouteToReference, routeToObj);
	}
}

// Removes the route-to reference if it matches the specified object.
// DATA: LegoObject* routeToObj
// <LegoRR.exe @004377d0>
bool32 __cdecl LegoRR::LegoObject_Callback_RemoveRouteToReference(LegoObject* liveObj, void* pRouteToObj)
{
	const LegoObject* routeToObj = (LegoObject*)pRouteToObj;
	if (liveObj->routeToObject == routeToObj) {
		LegoObject_Route_End(liveObj, false);
		liveObj->routeToObject = nullptr;
	}
	return false;
}


// <LegoRR.exe @00437800>
bool32 __cdecl LegoRR::LegoObject_Remove(LegoObject* deadObj)
{
	/// CUSTOM: Cleanup routing path lines.
	Debug_RouteVisual_Remove(deadObj);

	/// CUSTOM: Cleanup disposable stats (if it is overridden).
	StatsObject_RestoreModified(deadObj);

	/// CUSTOM: Cleanup follow reference.
	if (Lego_GetFollowUnit() == deadObj) {
		Lego_SetFollowUnit(nullptr);
	}


	// Increment number of this object type/id/levels that were removed from the level.
	objectGlobs.objectPrevLevels[deadObj->type][deadObj->id][deadObj->objLevel]++;

	// Stop currently-playing sound handles.
	if (deadObj->flags4 & LIVEOBJ4_DRILLSOUNDPLAYING) {
		SFX_Sound3D_StopSound(deadObj->drillSoundHandle);
	}
	if (deadObj->flags4 & LIVEOBJ4_ENGINESOUNDPLAYING) {
		SFX_Sound3D_StopSound(deadObj->engineSoundHandle);
	}

	// Clear all projectileObject fields referencing this object.
	for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {
		// Always returns false to enumerate the full list.
		Weapon_Callback_RemoveProjectileReference(obj, deadObj);
	}
	//LegoObject_RunThroughListsSkipUpgradeParts(Weapon_Callback_RemoveProjectileReference, deadObj);

	if (deadObj->type == LegoObject_RockMonster) {
		AITask_RemoveAttackRockMonsterReferences(deadObj);
	}

	if (deadObj->contMiniTeleportUp != nullptr) {
		Gods98::Container_Remove(deadObj->contMiniTeleportUp);
	}

	if (deadObj->carryingThisObject != nullptr) {
		LegoObject_DropCarriedObject(deadObj->carryingThisObject, false);
	}

	// This function has no side effects with deadObj, we can safely reorganize the LIVEOBJ4_DOCKOCCUPIED cleanup around this call.
	LegoObject_WaterVehicle_Unregister(deadObj);

	// Exit laser tracker mode.
	// Note that this mode does not treat the object as selected, so no extra handling seems to be needed.
	if (deadObj->flags4 & LIVEOBJ4_LASERTRACKERMODE) {
		legoGlobs.flags1 &= ~GAME1_LASERTRACKER;
	}

	// Clear flags that link routeToObjects to one another.
	/// TODO: Check if a single routeToObject can be referenced by multiple objects at once.
	///       If that's the case, then is clearing these flags causing problems?
	/// REFACTOR: First block moved from above `LegoObject_WaterVehicle_Unregister`.
	if (deadObj->flags4 & LIVEOBJ4_DOCKOCCUPIED) {
		deadObj->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
		if (deadObj->routeToObject != nullptr) {
			deadObj->routeToObject->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
		}
		/// TODO: Should routeToObject be cleared in this scenario too?
	}
	if (deadObj->flags4 & LIVEOBJ4_ENTRANCEOCCUPIED) {
		if (deadObj->routeToObject != nullptr) {
			deadObj->routeToObject->flags4 &= ~LIVEOBJ4_ENTRANCEOCCUPIED;
		}
		deadObj->routeToObject = nullptr;
	}

	LegoObject_Route_End(deadObj, false);
	LegoObject_DoBuildingsCallback_AttackByBoulder(deadObj);
	LegoObject_DropCarriedObject(deadObj, false); // false : don't put away.

	AITask_RemoveTargetObjectReferences(deadObj);

	/// FIX APPLY: Cleanup freeze object references.
	if (deadObj->freezeObject != nullptr) {
		if (deadObj->flags2 & LIVEOBJ2_FROZEN) {
			// We're not the ice cube. Perform cleanup to remove the ice cube too.
			deadObj->flags2 &= ~LIVEOBJ2_FROZEN;

			Error_InfoF("Object @ %i with ice cube @ %i removed", (uint32)objectListSet.IndexOf(deadObj), (uint32)objectListSet.IndexOf(deadObj->freezeObject));

			// Set ice cube to final stage of melting, where it won't interact with freezeObject.
			deadObj->freezeObject->health = -1.0f;
			deadObj->freezeObject->flags1 &= ~LIVEOBJ1_TELEPORTINGDOWN;
			//deadObj->freezeObject->flags3 &= ~LIVEOBJ3_REMOVING; // Mark the ice cube for melting.
			deadObj->freezeObject->flags3 |= LIVEOBJ3_REMOVING; // Mark the ice cube for melting.

			// Link the ice cube object to itself, so that it can die in peace (and not reference a destroyed object)...
			deadObj->freezeObject->freezeObject = deadObj->freezeObject;
		}
		else {
			// This is the ice cube.
			Error_InfoF("Ice cube @ %i removed", (uint32)objectListSet.IndexOf(deadObj));

			// Make sure our object is unfrozen.
			deadObj->freezeObject->flags2 &= ~LIVEOBJ2_FROZEN;
			deadObj->freezeObject->freezeObject = nullptr;
		}
		// Unlink freeze object (one-way).
		deadObj->freezeObject = nullptr;
	}

	// Clear first teleportDownObject field referencing this object.
	LegoObject_RemoveTeleportDownReference(deadObj);

	// Remove encyclopedia reference to this object.
	Encyclopedia_RemoveCurrentReference(deadObj);

	if ((StatsObject_GetStatsFlags1(deadObj) & STATS1_PROCESSCRYSTAL) ||
		(StatsObject_GetStatsFlags1(deadObj) & STATS1_PROCESSORE))
	{
		// Probably to remove static task saying: "Hey! if you need processed resources, then come on down to <buildingNameHere>!"
		AITask_LiveObject_FUN_00403940(deadObj);
	}

	// Remove object from selection.
	// Cancel first-person view if this is the objectFP.
	// And clear the camera tracker reference to this object.
	Message_RemoveObjectReference(deadObj);

	// Removes info messages referencing this object.
	Info_RemoveObjectReferences(deadObj);
	Bubble_RemoveObjectReferences(deadObj);

	// Clear routeToObject fields referencing this object.
	LegoObject_RemoveRouteToReferences(deadObj);

	Lego_RemoveRecordObject(deadObj);


	/// FIX APPLY: Remove object references created by K register/get in vehicle debug key.
	if (gamectrlGlobs.dbgGetInVehicle == deadObj) gamectrlGlobs.dbgGetInVehicle = nullptr;
	

	// Remove the model, and other object type-specific cleanup.
	switch (deadObj->type) {
	case LegoObject_Vehicle:
		Vehicle_Remove(deadObj->vehicle);
		Gods98::Mem_Free(deadObj->vehicle);
		break;

	case LegoObject_MiniFigure:
		Creature_Remove(deadObj->miniFigure);
		Gods98::Mem_Free(deadObj->miniFigure);
		break;

	case LegoObject_RockMonster:
		if (StatsObject_GetStatsFlags1(deadObj) & STATS1_FLOCKS) {
			LegoObject_FlocksDestroy(deadObj);
		}
		Creature_Remove(deadObj->rockMonster);
		Gods98::Mem_Free(deadObj->rockMonster);
		break;

	case LegoObject_Building:
		Building_Remove(deadObj->building);
		Gods98::Mem_Free(deadObj->building);
		break;

	case LegoObject_UpgradePart:
		/// REFACTOR: This case was checked after flag `LIVEOBJ3_SIMPLEOBJECT`. But these cases are mutually exclusive.
		Upgrade_Part_Remove(deadObj->upgradePart);
		Gods98::Mem_Free(deadObj->upgradePart);
		break;

	default: // Simple object types:
		if (deadObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) {
			Gods98::Container_Remove(deadObj->other);
		}
		break;
	}


	objectListSet.Remove(deadObj);

	return false;
}

// <LegoRR.exe @00437a70>
bool32 __cdecl LegoRR::LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_RunThroughListsCallback callback, void* data)
{
	for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {

		if (callback(obj, data))
			return true; // terminate run through listSet
	}
	return false;
	//return LegoObject_RunThroughLists(callback, search, true);
}

// <LegoRR.exe @00437a90>
bool32 __cdecl LegoRR::LegoObject_RunThroughLists(LegoObject_RunThroughListsCallback callback, void* data, bool32 skipUpgradeParts)
{
	for (LegoObject* obj : objectListSet.EnumerateAlive()) {
		// Secondary filter
		if (!skipUpgradeParts || !(obj->flags3 & LIVEOBJ3_UPGRADEPART)) {

			if (callback(obj, data))
				return true; // terminate run through listSet
		}
	}
	return false;
}

// <LegoRR.exe @00437b40>
void __cdecl LegoRR::LegoObject_SetCustomName(LegoObject* liveObj, OPTIONAL const char* newCustomName)
{
	if (newCustomName != nullptr && *newCustomName != '\0') {
		// Add or Set custom name.
		/// FIX APPLY: Actually allocate 12 chars instead of allocating 11 and writing to index 11!!!
		///            The only reason this worked before is because the malloc implementation allocates in units of 16 for smaller sizes.
		if (liveObj->customName == nullptr) {
			liveObj->customName = (char*)Gods98::Mem_Alloc(OBJECT_CUSTOMNAMELENGTH);
		}

		// Copy up to Length-1. strncpy null-terminates all chars beyond the string up to excluding Length-1.
		// So we'll need to null-terminate the final char ourselves.
		std::strncpy(liveObj->customName, newCustomName, OBJECT_MAXCUSTOMNAMECHARS);
		liveObj->customName[OBJECT_MAXCUSTOMNAMECHARS] = '\0'; // Null-terminate final char, not handled by strncpy.
	}
	else {
		// Remove custom name.
		if (liveObj->customName != nullptr) {
			Gods98::Mem_Free(liveObj->customName);
			liveObj->customName = nullptr;
		}
	}
}

// <LegoRR.exe @00437ba0>
void __cdecl LegoRR::HiddenObject_RemoveAll(void)
{
	for (uint32 i = 0; i < objectGlobs.hiddenObjectCount; i++) {
		HiddenObject* hiddenObj = &objectGlobs.hiddenObjects[i];
		
		if (hiddenObj->olistID != nullptr) {
			Gods98::Mem_Free(hiddenObj->olistID);
		}

		if (hiddenObj->olistDrivingID != nullptr) {
			Gods98::Mem_Free(hiddenObj->olistDrivingID);
		}
	}
	objectGlobs.hiddenObjectCount = 0;

	std::memset(objectGlobs.hiddenObjects, 0, sizeof(objectGlobs.hiddenObjects));

	NERPsRuntime_ClearHiddenObjectsFound();
}

// <LegoRR.exe @00437c00>
void __cdecl LegoRR::HiddenObject_ExposeBlock(const Point2I* blockPos)
{
	LegoObject* exposedObjs[OBJECT_MAXHIDDENOBJECTS]      = { nullptr }; // apLStack2400
	char* exposedOListIDs[OBJECT_MAXHIDDENOBJECTS]        = { nullptr }; // apcStack800
	char* exposedOListDrivingIDs[OBJECT_MAXHIDDENOBJECTS] = { nullptr }; // local_640
	uint32 exposedCount = 0; // local_968

	// Find exposed OL objects.
	for (uint32 i = 0; i < objectGlobs.hiddenObjectCount; i++) {
		HiddenObject* hiddenObj = &objectGlobs.hiddenObjects[i];
		if (hiddenObj->blockPos.x == blockPos->x && hiddenObj->blockPos.y == blockPos->y) {

			LegoObject* liveObj;
			if (hiddenObj->type == LegoObject_Building) {
				// Convert radians to range [0,8], then wrap to a valid direction [0,3].
				// This uses slices (like a pie chart) around each cardinal direction.
				// So a value of 340deg or 41deg would still be closest to 0deg, not 270deg or 90deg.
				//  (This is stored in radians, degrees are just for the example)
				const sint32 rotationTemp = (sint32)(hiddenObj->heading * (8.0f / (M_PI * 2))); // (0.1591549f * 8.0f));
				// Positive modulus of (rotation + 1). Use +1 to align pie slices for modulus operation.
				const Direction rotation = (Direction)(((((rotationTemp + 1) / 2) % DIRECTION__COUNT) + DIRECTION__COUNT) % DIRECTION__COUNT);

				uint32 shapeCount = 0; // dummy init
				const Point2I* shapePoints = Building_GetShapePoints(&legoGlobs.buildingData[hiddenObj->id], &shapeCount);
				const Point2I* finalShapePoints = SelectPlace_TransformShapePoints(&hiddenObj->blockPos, shapePoints, shapeCount, rotation);

				liveObj = Construction_SpawnBuilding(hiddenObj->id, &hiddenObj->blockPos, rotation, finalShapePoints, shapeCount, false);
			}
			else {
				liveObj = LegoObject_Create(hiddenObj->model, hiddenObj->type, hiddenObj->id);
			}

			if (liveObj != nullptr) {
				if (liveObj->type != LegoObject_Building) {
					LegoObject_SetPositionAndHeading(liveObj, hiddenObj->worldPos.x, hiddenObj->worldPos.y, hiddenObj->heading, true);
				}

				if (liveObj->type == LegoObject_PowerCrystal || liveObj->type == LegoObject_Ore ||
					liveObj->type == LegoObject_Dynamite || liveObj->type == LegoObject_Barrier)
				{
					AITask_DoCollect(liveObj, 0.0f);
				}

				liveObj->health = hiddenObj->health;

				if (liveObj->type == LegoObject_MiniFigure) {
					NERPsRuntime_IncHiddenObjectsFound(LegoObject_MiniFigure);
					Info_Send(Info_FoundMinifigure, nullptr, liveObj, nullptr);
				}

				liveObj->flags4 |= LIVEOBJ4_DONTSHOWHELPWINDOW; // Don't spam the help window when discovering hidden units.

				LegoObject_FUN_00438720(liveObj);

				if (liveObj->type == LegoObject_ElectricFence) {
					ElectricFence_CreateFence(liveObj);
				}

				exposedObjs[exposedCount]            = liveObj;
				exposedOListIDs[exposedCount]        = hiddenObj->olistID;
				exposedOListDrivingIDs[exposedCount] = hiddenObj->olistDrivingID;
				exposedCount++;

				/// TODO: Should we also nullify olistDrivingID?
				hiddenObj->olistID = nullptr;
			}

			// Replace this exposed object with the hidden object at the end of the list.
			objectGlobs.hiddenObjects[i] = objectGlobs.hiddenObjects[objectGlobs.hiddenObjectCount - 1];
			objectGlobs.hiddenObjectCount--;
			i--;
		}
	}

	// Connect exposed OL drivers.
	/// NOTE: This requires that both drivers and vehicle OL objects be defined in the same blockPos.
	for (uint32 i = 0; i < exposedCount; i++) {
		char* olistDrivingID = exposedOListDrivingIDs[i];
		if (olistDrivingID != nullptr) {

			// Find the the exposed OL object of the same olistID as olistDrivingID.
			for (uint32 j = 0; j < exposedCount; j++) {
				if (i != j && ::_stricmp(exposedOListIDs[j], olistDrivingID) == 0) {

					LegoObject* driverObj = exposedObjs[i];
					LegoObject* vehicleObj = exposedObjs[j];

					driverObj->routeToObject = vehicleObj;
					driverObj->flags2 |= LIVEOBJ2_UNK_4; // OL object flag? Related to driving? Route to object and get in?

					LegoObject_AbilityFlags trainFlags;

					const StatsFlags3 sflags3 = StatsObject_GetStatsFlags3(vehicleObj);
					if (sflags3 & STATS3_NEEDSPILOT) {
						trainFlags = ABILITY_FLAG_PILOT;
					}
					else {
						const StatsFlags1 sflags1 = StatsObject_GetStatsFlags1(vehicleObj);
						if (!(sflags1 & STATS1_CROSSWATER)) {
							trainFlags = ABILITY_FLAG_DRIVER;
						}
						else if (!(sflags1 & STATS1_CROSSLAND)) {
							trainFlags = ABILITY_FLAG_SAILOR;
						}
						else {
							trainFlags = ABILITY_FLAG_PILOT;
						}
					}
					LegoObject_TrainMiniFigure_instantunk(driverObj, trainFlags);
					LegoObject_ClearDockOccupiedFlag(driverObj, vehicleObj);
				}
			}
		}
	}

	// Cleanup exposed OL string IDs that were owned by HiddenObjects.
	for (uint32 i = 0; i < exposedCount; i++) {
		if (exposedOListIDs[i] != nullptr) {
			Gods98::Mem_Free(exposedOListIDs[i]);
		}

		if (exposedOListDrivingIDs[i] != nullptr) {
			Gods98::Mem_Free(exposedOListDrivingIDs[i]);
		}
	}
}

// <LegoRR.exe @00437ee0>
void __cdecl LegoRR::HiddenObject_Add(ObjectModel* objModel, LegoObject_Type objType, LegoObject_ID objID, const Point2F* worldPos, real32 heading, real32 health, const char* olistID, const char* olistDrivingID)
{
	/// SANITY: Error check bounds.
	if (objectGlobs.hiddenObjectCount >= OBJECT_MAXHIDDENOBJECTS) {
		Error_Fatal(true, "Too many hidden objects.");
	}

	HiddenObject* hiddenObj = &objectGlobs.hiddenObjects[objectGlobs.hiddenObjectCount];
	objectGlobs.hiddenObjectCount++;

	std::memset(hiddenObj, 0, sizeof(HiddenObject));

	// Get blockPos
	Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), worldPos->x, worldPos->y, &hiddenObj->blockPos.x, &hiddenObj->blockPos.y);

	hiddenObj->worldPos = *worldPos;
	hiddenObj->heading = heading;
	hiddenObj->model = objModel;
	hiddenObj->type = objType;
	hiddenObj->id = objID;
	hiddenObj->health = health;

	if (olistID != nullptr) {
		hiddenObj->olistID = Gods98::Util_StrCpy(olistID);
	}

	if (olistDrivingID != nullptr) {
		hiddenObj->olistDrivingID = Gods98::Util_StrCpy(olistDrivingID);
	}
}

// <LegoRR.exe @00437f80>
bool32 __cdecl LegoRR::LegoObject_CanShootObject(LegoObject* liveObj)
{
	const StatsFlags2 sflags2 = StatsObject_GetStatsFlags2(liveObj);
	if (sflags2 & STATS2_CANBESHOTAT) {
		if (liveObj->health > StatsObject_GetPainThreshold(liveObj)) {
			return true;
		}
	}
	return false;
}

// <LegoRR.exe @00437fc0>
LegoRR::LegoObject* __cdecl LegoRR::LegoObject_Create(ObjectModel* objModel, LegoObject_Type objType, LegoObject_ID objID)
{
	LegoObject* liveObj = LegoObject_Create_internal();
	if (!liveObj) return nullptr;

	const uint32 objLevel = 0; // Keep this here if we every want to extend creating an object that isn't at level 0.

	liveObj->type = objType;
	liveObj->id = objID;
	liveObj->objLevel = objLevel; // We *would* need to assign this beforehand so that StatsObject_SetObjectLevel doesn't change the prevCount for Lv0.

	liveObj->flags3 = LIVEOBJ3_NONE;

	liveObj->health = 100.0f;
	liveObj->energy = 100.0f;

	// Disable sound temporarily to avoid anything that would play during initial activity setup(?)
	Gods98::Container_EnableSoundTriggers(false);
	const bool32 isSoundOn = SFX_IsSoundOn();
	SFX_SetSoundOn(false, false);

	switch (liveObj->type) {
	case LegoObject_Vehicle:
		{
			VehicleModel* srcVehicleModel = (VehicleModel*)objModel;

			liveObj->vehicle = (VehicleModel*)Gods98::Mem_Alloc(sizeof(VehicleModel));
			Vehicle_Clone(srcVehicleModel, liveObj->vehicle);
			Vehicle_SetOwnerObject(liveObj->vehicle, liveObj);

			liveObj->carryNullFrames = Vehicle_GetCarryNullFrames(liveObj->vehicle);

			liveObj->abilityFlags = ABILITY_FLAG_SCANNER; // Do vehicles always have a scanner by default??
			liveObj->flags3 |= (LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANSELECT|
								LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_AITASK_UNK_400000);

			// Set upgrade parts used in model.
			Vehicle_SetUpgradeLevel(liveObj->vehicle, objLevel);
			// Assign normal stats.
			StatsObject_SetObjectLevel(liveObj, objLevel);

			if (srcVehicleModel->drillNullName != nullptr) {
				liveObj->flags3 |= LIVEOBJ3_CANDIG;
			}

			if (srcVehicleModel->carryNullName != nullptr) {
				liveObj->flags3 |= LIVEOBJ3_CANCARRY;

				const StatsFlags1 sflags1 = StatsObject_GetStatsFlags1(liveObj);
				if (sflags1 & STATS1_CROSSLAND) {
					liveObj->flags3 |= LIVEOBJ3_CANPICKUP;
				}
			}
		}
		break;
	case LegoObject_MiniFigure:
		{
			CreatureModel* srcCreatureModel = (CreatureModel*)objModel;

			liveObj->miniFigure = (CreatureModel*)Gods98::Mem_Alloc(sizeof(CreatureModel));
			Creature_Clone(srcCreatureModel, liveObj->miniFigure);
			Creature_SetOwnerObject(liveObj->miniFigure, liveObj);

			liveObj->carryNullFrames = Creature_GetCarryNullFrames(liveObj->miniFigure);

			liveObj->flags3 |= (LIVEOBJ3_UNK_1|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANFIRSTPERSON|
								LIVEOBJ3_CANCARRY|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANYESSIR|
								LIVEOBJ3_CANSELECT|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_AITASK_UNK_400000);

			if (srcCreatureModel->drillNullName != nullptr) {
				liveObj->flags3 |= LIVEOBJ3_CANDIG;
			}

			liveObj->flags3 |= LIVEOBJ3_CANDYNAMITE;

			// Assign normal stats.
			StatsObject_SetObjectLevel(liveObj, objLevel);

			/// FIXME: Allow modding to change which tools a unit automatically starts with.
			LegoObject_MiniFigure_EquipTool(liveObj, LegoObject_ToolType_Drill);
		}
		break;
	case LegoObject_RockMonster:
		{
			CreatureModel* srcCreatureModel = (CreatureModel*)objModel;

			liveObj->rockMonster = (CreatureModel*)Gods98::Mem_Alloc(sizeof(CreatureModel));
			Creature_Clone(srcCreatureModel, liveObj->rockMonster);
			Creature_SetOwnerObject(liveObj->rockMonster, liveObj);

			liveObj->carryNullFrames = Creature_GetCarryNullFrames(liveObj->rockMonster);

			liveObj->flags3 |= (LIVEOBJ3_UNK_200|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_MONSTER_UNK_8000000);

			if (srcCreatureModel->drillNullName != nullptr) {
				liveObj->flags3 |= LIVEOBJ3_CANDIG;
			}

			// Assign normal stats.
			StatsObject_SetObjectLevel(liveObj, objLevel);

			const StatsFlags1 sflags1 = StatsObject_GetStatsFlags1(liveObj);
			if (!(sflags1 & STATS1_FLOCKS)) {
				liveObj->flocks = nullptr;
			}
			else {
				LegoObject_Flocks_Initialise(liveObj);
				if (!(sflags1 & STATS1_FLOCKS_DEBUG)) {
					LegoObject_Hide(liveObj, true);
					// CANSELECT flag was (definitely) present in debug builds for monsters, but it's never set in release.
					liveObj->flags3 &= ~LIVEOBJ3_CANSELECT;
				}
			}

			if (LegoObject_CanShootObject(liveObj)) {
				// Register as a shootable target(?)
				AITask_DoAttackRockMonster_Target(liveObj);
			}

			liveObj->flags3 |= LIVEOBJ3_POWEROFF;
		}
		break;
	case LegoObject_Building:
		{
			BuildingModel* srcBuildingModel = (BuildingModel*)objModel;

			liveObj->building = (BuildingModel*)Gods98::Mem_Alloc(sizeof(BuildingModel));
			Building_Clone(srcBuildingModel, liveObj->building);
			Building_SetOwnerObject(liveObj->building, liveObj);

			liveObj->carryNullFrames = Building_GetCarryNullFrames(liveObj->building);

			liveObj->flags3 |= (LIVEOBJ3_CANSELECT|LIVEOBJ3_CANDAMAGE);

			// Set upgrade parts used in model.
			Building_SetUpgradeLevel(liveObj->building, objLevel);
			// Assign normal stats.
			StatsObject_SetObjectLevel(liveObj, objLevel);

			const StatsFlags1 sflags1 = StatsObject_GetStatsFlags1(liveObj);
			// Not entirely sure, but register this new building as the primary teleporter for what it supports(??)
			if (sflags1 & STATS1_SMALLTELEPORTER) {
				legoGlobs.placeDestSmallTeleporter = liveObj;
			}
			if (sflags1 & STATS1_BIGTELEPORTER) {
				legoGlobs.placeDestBigTeleporter = liveObj;
			}
			if (sflags1 & STATS1_WATERTELEPORTER) {
				legoGlobs.placeDestWaterTeleporter = liveObj;
			}
			if (sflags1 & STATS1_SNAXULIKE) {
				// Register this location as a valid *slow* food joint for Rock Raiders to hang out and loiter at(?)
				AITask_DoGotoEat(liveObj);
			}

			// Register object as a repairable target(?)
			AITask_DoRepair(liveObj);

			liveObj->abilityFlags = ABILITY_FLAG_SCANNER; // Do buildings always have a scanner by default??

			// The power grid needs to be updated for this building's consumption/or output.
			LegoObject_RequestPowerGridUpdate();
		}
		break;
	case LegoObject_UpgradePart:
		{
			Upgrade_PartModel* srcUpgradePartModel = (Upgrade_PartModel*)objModel;

			liveObj->upgradePart = (Upgrade_PartModel*)Gods98::Mem_Alloc(sizeof(Upgrade_PartModel));
			Upgrade_Part_Clone(srcUpgradePartModel, liveObj->upgradePart);

			liveObj->flags3 |= LIVEOBJ3_UPGRADEPART; // This makes 99% of all object enumerations skip this object (see: EnumerateSkipUpgradeParts).

			// Assign special stats.
			liveObj->stats = &c_ObjectStats_Upgrade;
		}
		break;
	default:
		{
			Gods98::Container* srcContainer = (Gods98::Container*)objModel;

			liveObj->other = Gods98::Container_Clone(srcContainer);
			Gods98::Container_SetUserData(liveObj->other, liveObj);

			liveObj->flags3 |= LIVEOBJ3_SIMPLEOBJECT;

			// Assign flags depending on the object type.
			switch (liveObj->type) {
			case LegoObject_PowerCrystal:
			case LegoObject_Ore:
				liveObj->flags3 |= (LIVEOBJ3_CANSELECT|LIVEOBJ3_ALLOWCULLING_UNK);
				break;
			case LegoObject_ElectricFence:
				liveObj->flags3 |= (LIVEOBJ3_CANSELECT|LIVEOBJ3_CANDAMAGE);
				break;
			case LegoObject_Dynamite:
			case LegoObject_OohScary:
				liveObj->flags3 |= LIVEOBJ3_CANDAMAGE; // Uhhh... these can be damaged!? OR WAIT! MAYBE ITS THE COUNTDOWN NUMBERS! AHAHHHH!!!!
				break;
			}

			// Only does anything when liveObj->type == LegoObject_PowerCrystal,
			//  but leave it outside the switch/if-else check if we want to extend this.
			LegoObject_SetCrystalPoweredColour(liveObj, true);

			// Assign special or normal stats depending on the object type.
			// Note that these special stats types are all identical. They have 0x30 for the first field, and the rest is all zeroes.
			switch (liveObj->type) {
			case LegoObject_Dynamite:
				// Assign special stats.
				liveObj->stats = &c_ObjectStats_Dynamite;
				break;
			case LegoObject_Barrier:
				// Assign special stats.
				liveObj->stats = &c_ObjectStats_Barrier;
				break;
			case LegoObject_SpiderWeb:
			case LegoObject_OohScary:
			case LegoObject_ElectricFenceStud:
			case LegoObject_Pusher:
			case LegoObject_Freezer:
			case LegoObject_IceCube:
			case LegoObject_LaserShot:
				// Assign special stats.
				liveObj->stats = &c_ObjectStats_Other;
				break;
			case LegoObject_Boulder:
			case LegoObject_PowerCrystal:
			case LegoObject_Ore:
			case LegoObject_ElectricFence:
			//case LegoObject_Path: // Not a real object type
			default:
				// Assign normal stats.
				StatsObject_SetObjectLevel(liveObj, objLevel);
				break;
			}
		}
		break;
	}

	// Go through a few activities and confirm if our activity model supports them.
	// Note that activityName2 needs to be different for the UpdateAcivityChange function to succeed.
	liveObj->activityName2 = objectGlobs.activityName[Activity_Stand];

	liveObj->activityName1 = objectGlobs.activityName[Activity_TurnLeft];
	if (LegoObject_UpdateActivityChange(liveObj)) {
		liveObj->flags3 |= LIVEOBJ3_CANTURN;
	}

	liveObj->activityName1 = objectGlobs.activityName[Activity_Gather];
	if (LegoObject_UpdateActivityChange(liveObj)) {
		liveObj->flags3 |= LIVEOBJ3_CANGATHER;
	}

	liveObj->activityName1 = objectGlobs.activityName[Activity_RouteRubble];
	if (LegoObject_UpdateActivityChange(liveObj)) {
		liveObj->flags3 |= LIVEOBJ3_CANROUTERUBBLE;
	}

	// Set back to default activity. Note, this only works because the previous calls to
	//  LegoObject_UpdateActivityChange automatically assigned activityName2, even on failure.
	liveObj->activityName1 = objectGlobs.activityName[Activity_Stand];
	LegoObject_UpdateActivityChange(liveObj);

	AITask_LiveObject_Unk_UpdateAITask_AnimationWait(liveObj);

	// Restore sound functionality now that our object creation is finished.
	Gods98::Container_EnableSoundTriggers(true);
	if (isSoundOn) SFX_SetSoundOn(true, false);

	LegoObject_UpdatePowerConsumption(liveObj);

	// Track the number of objects that ever existed in the level.
	/// NOTE: We ALWAYS want to start by incrementing the initial level, since StatsObject_SetObjectLevel
	///       only either updates total+prevLevels, or neither if the level hasn't changed.
	objectGlobs.objectTotalLevels[liveObj->type][liveObj->id][liveObj->objLevel]++;

	// Respect the unselectable stats flag. This stats flag is ONLY used for initialisation HERE.
	const StatsFlags2 sflags2 = StatsObject_GetStatsFlags2(liveObj);
	if (sflags2 & STATS2_UNSELECTABLE) {
		liveObj->flags3 &= ~LIVEOBJ3_CANSELECT;
	}

	return liveObj;
}

// <LegoRR.exe @00438580>
LegoRR::LegoObject* __cdecl LegoRR::LegoObject_Create_internal(void)
{
	LegoObject* newLiveObj = objectListSet.Add();
	ListSet::MemZero(newLiveObj);

	newLiveObj->targetBlockPos.y = -1.0f;
	newLiveObj->targetBlockPos.x = -1.0f;

	return newLiveObj;
}

// <LegoRR.exe @004385d0>
void __cdecl LegoRR::LegoObject_AddList(void)
{
	// NOTE: This function is no longer called, objectListSet.Add already does so.
	objectListSet.AddList();
}

// <LegoRR.exe @00438650>
uint32 __cdecl LegoRR::LegoObject_GetNumBuildingsTeleported(void)
{
	return objectGlobs.BuildingsTeleported;
}

// <LegoRR.exe @00438660>
void __cdecl LegoRR::LegoObject_SetNumBuildingsTeleported(uint32 numTeleported)
{
	objectGlobs.BuildingsTeleported = numTeleported;
}

// <LegoRR.exe @00438670>
void __cdecl LegoRR::LegoObject_SetCrystalPoweredColour(LegoObject* liveObj, bool32 powered)
{
	if (liveObj->type == LegoObject_PowerCrystal) {
		ColourRGBF colour;
		if (powered)
		{
			colour = (liveObj->objLevel == 0 ? legoGlobs.PowerCrystalRGB : legoGlobs2.powerCrystalLv1RGB);
		}
		else
		{
			colour = legoGlobs.UnpoweredCrystalRGB;
		}

		for (uint32 groupID = 0; groupID < Gods98::Container_Mesh_GetGroupCount(liveObj->other); groupID++) {

			Gods98::Container_Mesh_SetColourAlpha(liveObj->other, groupID, colour.r, colour.g, colour.b, 1.0f);
			Gods98::Container_Mesh_SetEmissive(liveObj->other, groupID, colour.r, colour.g, colour.b);
		}
	}
	else if (liveObj->type == LegoObject_Ore && liveObj->id == LegoObject_ID_Ore) {
		/// CUSTOM: Support for custom Lv1 Ore colour
		if (powered && liveObj->objLevel != 0) {
			const ColourRGBF colour = legoGlobs2.oreLv1RGB;

			for (uint32 groupID = 0; groupID < Gods98::Container_Mesh_GetGroupCount(liveObj->other); groupID++) {

				Gods98::Container_Mesh_SetColourAlpha(liveObj->other, groupID, colour.r, colour.g, colour.b, 1.0f);
			}
		}
	}
}

// <LegoRR.exe @00438720>
void __cdecl LegoRR::LegoObject_FUN_00438720(LegoObject* liveObj)
{
	if (liveObj->type == LegoObject_Building) {
		LegoObject_UpdateActivityChange(liveObj);

		liveObj->flags3 |= LIVEOBJ3_CANCARRY; // Temporarily set flag, not sure if other cases unset it.

		const StatsFlags1 sflags1 = StatsObject_GetStatsFlags1(liveObj);
		if (sflags1 & STATS1_STOREOBJECTS) {
			AITask_LiveObject_FUN_004038d0(liveObj);
		}
		else if (sflags1 & STATS1_PROCESSORE) {
			AITask_DoDeposit_ObjectType(liveObj, LegoObject_Ore, (LegoObject_ID)0, 0);
		}
		else if (sflags1 & STATS1_PROCESSCRYSTAL) {
			AITask_DoDeposit_ObjectType(liveObj, LegoObject_PowerCrystal, (LegoObject_ID)0, 0);
		}
		else {
			liveObj->flags3 &= ~LIVEOBJ3_CANCARRY; // Unset temporary flag.
		}

		LegoObject_RequestPowerGridUpdate();
		liveObj->flags4 |= LIVEOBJ4_UNK_200000;
	}
	else if (liveObj->type == LegoObject_Vehicle) {
		Vehicle_HideWheels(liveObj->vehicle, false);
	}

	if (liveObj->type == LegoObject_MiniFigure || liveObj->type == LegoObject_Vehicle) {
		LegoObject_RemoveTeleportDownReference(liveObj);
	}

	HelpWindow_RecallDependencies(liveObj->type, liveObj->id, 0, (liveObj->flags4 & LIVEOBJ4_DONTSHOWHELPWINDOW));

	if (liveObj->type == LegoObject_Building) {
		objectGlobs.BuildingsTeleported++;
	}
	if (liveObj->type == LegoObject_MiniFigure || liveObj->type == LegoObject_Vehicle) {
		AITask_DoAttackObject(liveObj);
	}
	if (liveObj->type == LegoObject_Vehicle || liveObj->type == LegoObject_Building) {
		liveObj->flags4 |= LIVEOBJ4_UNK_40000;
	}
}

// <LegoRR.exe @00438840>
void __cdecl LegoRR::LegoObject_SetPowerOn(LegoObject* liveObj, bool32 on)
{
	if (!on) {
		liveObj->flags3 |= LIVEOBJ3_POWEROFF;
	}
	else {
		liveObj->flags3 &= ~LIVEOBJ3_POWEROFF;
	}
	/// TODO: Can we choose to skip this when the flag is unchanged?
	LegoObject_RequestPowerGridUpdate();
}

// <LegoRR.exe @00438870>
bool32 __cdecl LegoRR::LegoObject_IsActive(LegoObject* liveObj, bool32 ignoreUnpowered)
{
	if (!(liveObj->flags1 & (LIVEOBJ1_EXPANDING|LIVEOBJ1_CRUMBLING|LIVEOBJ1_TELEPORTINGDOWN|LIVEOBJ1_TELEPORTINGUP)) &&
		!(liveObj->flags1 & LIVEOBJ1_ENTERING_WALLHOLE) && !(liveObj->flags2 & LIVEOBJ2_GETTINGSERVICED) &&
		liveObj->health >= 0.0f &&
		(ignoreUnpowered || ((liveObj->flags3 & LIVEOBJ3_HASPOWER) && !(liveObj->flags3 & LIVEOBJ3_POWEROFF))))
	{
		return true;
	}
	return false;
}

// <LegoRR.exe @004388d0>
LegoRR::LegoObject* __cdecl LegoRR::LegoObject_CreateInWorld(ObjectModel* objModel, LegoObject_Type objType, LegoObject_ID objID, uint32 objLevel, real32 xPos, real32 yPos, real32 heading)
{
	LegoObject* liveObj = LegoObject_Create(objModel, objType, objID);
	if (liveObj != nullptr) {
		LegoObject_SetPositionAndHeading(liveObj, xPos, yPos, heading, true);
		if (liveObj->type == LegoObject_PowerCrystal || liveObj->type == LegoObject_Ore) {
			LegoObject_SetLevel(liveObj, objLevel);
		}
		return liveObj;
	}
	return nullptr;
}

// <LegoRR.exe @00438930>
LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FindPoweredBuildingAtBlockPos(const Point2I* blockPos)
{
	SearchObjectBlockXY_c search = { nullptr }; // dummy init

	search.resultObj = nullptr;
	search.bx = blockPos->x;
	search.by = blockPos->y;

	for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {
		if (LegoObject_Callback_FindPoweredBuildingAtBlockPos(obj, &search))
			break;
	}
	//LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_Callback_FindPoweredBuildingAtBlockPos, &search);
	return search.resultObj;
}

// DATA: SearchObjectBlockXY_c* search
// <LegoRR.exe @00438970>
bool32 __cdecl LegoRR::LegoObject_Callback_FindPoweredBuildingAtBlockPos(LegoObject* liveObj, void* pSearch)
{
	SearchObjectBlockXY_c* search = (SearchObjectBlockXY_c*)pSearch;

	if (liveObj->type == LegoObject_Building) {
		const StatsFlags2 sflags2 = StatsObject_GetStatsFlags2(liveObj);
		if (sflags2 & STATS2_GENERATEPOWER) {
			if (LegoObject_IsActive(liveObj, false)) {
				sint32 bx = 0, by = 0; // dummy inits
				LegoObject_GetBlockPos(liveObj, &bx, &by);
				if (bx == search->bx && by == search->by) {
					search->resultObj = liveObj;
					return true;
				}
			}
		}
	}
	return false;
}

// <LegoRR.exe @004389e0>
//bool32 __cdecl LegoRR::LegoObject_AddThisDrainedCrystals(LegoObject* liveObj, sint32 crystalDrainedAmount);

// <LegoRR.exe @00438a30>
//bool32 __cdecl LegoRR::LegoObject_GetBuildingUpgradeCost(LegoObject* liveObj, OPTIONAL OUT uint32* oreCost);

// <LegoRR.exe @00438ab0>
//void __cdecl LegoRR::LegoObject_UpgradeBuilding(LegoObject* liveObj);

// <LegoRR.exe @00438b70>
//bool32 __cdecl LegoRR::LegoObject_HasEnoughOreToUpgrade(LegoObject* liveObj, uint32 objLevel);

// <LegoRR.exe @00438c20>
//undefined4 __cdecl LegoRR::LegoObject_Search_FUN_00438c20(LegoObject* opt_liveObj, bool32 param_2);

// <LegoRR.exe @00438ca0>
//undefined4 __cdecl LegoRR::LegoObject_Search_FUN_00438ca0(LegoObject* liveObj, bool32 param_2);

// <LegoRR.exe @00438d20>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FUN_00438d20(const Point2I* blockPos, LegoObject_Type objType, LegoObject_ID objID, uint32 objLevel);

// <LegoRR.exe @00438da0>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FindResourceProcessingBuilding(LegoObject* liveObj, const Point2I* blockPos, LegoObject_Type objType, uint32 objLevel);

// <LegoRR.exe @00438e40>
//undefined4 __cdecl LegoRR::LegoObject_Search_FUN_00438e40(LegoObject* liveObj, undefined4 param_2);

// <LegoRR.exe @00438eb0>
//undefined4 __cdecl LegoRR::LegoObject_Search_FUN_00438eb0(LegoObject* liveObj);

// <LegoRR.exe @00438f20>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FindSnackServingBuilding(LegoObject* liveObj);

// <LegoRR.exe @00438f90>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FindBigTeleporter(const Point2F* worldPos);

// <LegoRR.exe @00438ff0>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FindSmallTeleporter(const Point2F* worldPos);

// <LegoRR.exe @00439050>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_FindWaterTeleporter(const Point2F* worldPos);

// <LegoRR.exe @004390b0>
//LegoRR::LegoObject* __cdecl LegoRR::Level_GetBuildingAtPosition(const Point2F* worldPos);

// <LegoRR.exe @00439110>
//undefined4 __cdecl LegoRR::LegoObject_Search_FUN_00439110(LegoObject* liveObj, OPTIONAL const Point2F* worldPos, LegoObject_AbilityFlags abilityFlags);

// <LegoRR.exe @00439190>
//bool32 __cdecl LegoRR::LegoObject_HasTraining(LegoObject* liveObj, LegoObject_AbilityFlags abilityFlags);

// <LegoRR.exe @00439220>
//bool32 __cdecl LegoRR::LegoObject_IsDocksBuilding_Unk(LegoObject* liveObj);

// <LegoRR.exe @00439270>
//bool32 __cdecl LegoRR::LegoObject_CallbackSearch_FUN_00439270(LegoObject* liveObj, sint32** search);

// <LegoRR.exe @004394c0>
//bool32 __cdecl LegoRR::LegoObject_CanStoredObjectTypeBeSpawned(LegoObject_Type objType);

// <LegoRR.exe @00439500>
//bool32 __cdecl LegoRR::LegoObject_Callback_CanSpawnStoredObjects(LegoObject* liveObj1, LegoObject* spawnObj);

// <LegoRR.exe @00439540>
//void __cdecl LegoRR::LegoObject_PTL_GenerateFromCryOre(const Point2I* blockPos);

// <LegoRR.exe @00439600>
//void __cdecl LegoRR::LegoObject_PTL_GenerateCrystalsAndOre(const Point2I* blockPos, uint32 objLevel);

// <LegoRR.exe @00439630>
//void __cdecl LegoRR::Level_GenerateCrystal(const Point2I* blockPos, uint32 objLevel, OPTIONAL const Point2F* worldPos, bool32 showInfoMessage);

// <LegoRR.exe @00439770>
//void __cdecl LegoRR::Level_GenerateOre(const Point2I* blockPos, uint32 objLevel, OPTIONAL const Point2F* worldPos, bool32 showInfoMessage);

// <LegoRR.exe @004398a0>
//const char* __cdecl LegoRR::LegoObject_GetLangName(LegoObject* liveObj);

// <LegoRR.exe @00439980>
//const char* __cdecl LegoRR::Object_GetTypeName(LegoObject_Type objType, LegoObject_ID objID);

// <LegoRR.exe @00439a50>
//const char* __cdecl LegoRR::Object_GetLangTheName(LegoObject_Type objType, LegoObject_ID objID);

// <LegoRR.exe @00439b20>
//const char* __cdecl LegoRR::Object_GetLangName(LegoObject_Type objType, LegoObject_ID objID);

// <LegoRR.exe @00439bf0>
//void __cdecl LegoRR::LegoObject_GetTypeAndID(LegoObject* liveObj, OUT LegoObject_Type* objType, OUT LegoObject_ID* objID);

// <LegoRR.exe @00439c10>
//void __cdecl LegoRR::Object_LoadToolNames(const Gods98::Config* config, const char* gameName);

// <LegoRR.exe @00439c50>
void __cdecl LegoRR::LegoObject_RequestPowerGridUpdate(void)
{
	if (objectGlobs.flags & LegoObject_GlobFlags::OBJECT_GLOB_FLAG_UPDATING) {
		// We're in the update loop. Recalculating power now is dangerous when only some of the
		// objects have been update while others are awaiting an update.
		// Delay recalculation until the end of the update loop.
		objectGlobs.flags |= LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERNEEDSUPDATE;
	}
	else {
		// Not in the update loop, go straight to power grid recalculation mode.
		objectGlobs.flags |= LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERUPDATING;

		// We need to reset this before PowerGrid recalculation.
		Level_ResetCrystalsDrained();
	}
}

// <LegoRR.exe @00439c80>
//bool32 __cdecl LegoRR::LegoObject_VehicleMaxCarryChecksTime_FUN_00439c80(LegoObject* liveObj);

// <LegoRR.exe @00439ce0>
//bool32 __cdecl LegoRR::LegoObject_TryCollectObject(LegoObject* liveObj, LegoObject* targetObj);

// <LegoRR.exe @00439e40>
//bool32 __cdecl LegoRR::LegoObject_FUN_00439e40(LegoObject* liveObj, LegoObject* otherObj);

// <LegoRR.exe @00439e90>
//bool32 __cdecl LegoRR::LegoObject_FUN_00439e90(LegoObject* liveObj, LegoObject* targetObj, bool32 param_3);

// <LegoRR.exe @00439f40>
//void __cdecl LegoRR::LegoObject_CompleteVehicleUpgrade(LegoObject* liveObj);

// <LegoRR.exe @00439f90>
//bool32 __cdecl LegoRR::LegoObject_SetLevel(LegoObject* liveObj, uint32 newLevel);

// <LegoRR.exe @00439fb0>
//bool32 __cdecl LegoRR::LegoObject_IsSmallTeleporter(LegoObject* liveObj);

// <LegoRR.exe @00439fd0>
//bool32 __cdecl LegoRR::LegoObject_IsBigTeleporter(LegoObject* liveObj);

// <LegoRR.exe @00439ff0>
//bool32 __cdecl LegoRR::LegoObject_IsWaterTeleporter(LegoObject* liveObj);

// <LegoRR.exe @0043a010>
//bool32 __cdecl LegoRR::LegoObject_UnkGetTerrainCrossBlock_FUN_0043a010(LegoObject* liveObj, OPTIONAL OUT Point2I* blockPos);

// <LegoRR.exe @0043a0d0>
//bool32 __cdecl LegoRR::LegoObject_UnkGetTerrainGetOutAtLandBlock_FUN_0043a0d0(LegoObject* liveObj, OUT Point2I* blockPos);

// <LegoRR.exe @0043a100>
//bool32 __cdecl LegoRR::LegoObject_CheckUnkGetInAtLand_FUN_0043a100(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @0043a130>
//void __cdecl LegoRR::LegoObject_DropCarriedObject(LegoObject* liveObj, bool32 putAway);

// <LegoRR.exe @0043a3e0>
//bool32 __cdecl LegoRR::LegoObject_TryRequestOrDumpCarried(LegoObject* liveObj, const Point2I* blockPos, const Point2F* worldPos, bool32 place, bool32 setRouteFlag8);

// <LegoRR.exe @0043a5c0>
//bool32 __cdecl LegoRR::LegoObject_TryDepositCarried(LegoObject* liveObj, LegoObject* destObj);

// <LegoRR.exe @0043a8b0>
//Gods98::Container* __cdecl LegoRR::LegoObject_GetDepositNull(LegoObject* liveObj);

// <LegoRR.exe @0043a910>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_SpawnCarryableObject(LegoObject* liveObj, LegoObject_Type objType, LegoObject_ID objID, sint32 objLevel);

// <LegoRR.exe @0043aa80>
//bool32 __cdecl LegoRR::LegoObject_CanSpawnCarryableObject(LegoObject* liveObj, LegoObject_Type objType, LegoObject_ID objID);

// <LegoRR.exe @0043ab10>
//void __cdecl LegoRR::LegoObject_PutAwayCarriedObject(LegoObject* liveObj, LegoObject* carriedObj);

// <LegoRR.exe @0043abb0>
//void __cdecl LegoRR::Level_AddCrystals__unusedLegoObject(LegoObject* liveObj_unused, uint32 crystalCount);

// <LegoRR.exe @0043abd0>
//void __cdecl LegoRR::Level_AddOre__unusedLegoObject(LegoObject* liveObj_unused, uint32 oreCount);

// <LegoRR.exe @0043abf0>
//void __cdecl LegoRR::LegoObject_WaterVehicle_Unregister(LegoObject* liveObj);

// <LegoRR.exe @0043ac20>
//void __cdecl LegoRR::LegoObject_WaterVehicle_Register(LegoObject* liveObj);

// <LegoRR.exe @0043aca0>
//void __cdecl LegoRR::LegoObject_RegisterVehicle__callsForWater(LegoObject* liveObj);

// <LegoRR.exe @0043acb0>
//void __cdecl LegoRR::LegoObject_FUN_0043acb0(LegoObject* liveObj1, LegoObject* liveObj2);

// <LegoRR.exe @0043ad70>
//void __cdecl LegoRR::LegoObject_RockMonster_FUN_0043ad70(LegoObject* liveObj);

// <LegoRR.exe @0043aeb0>
void __cdecl LegoRR::LegoObject_AttackPath(LegoObject* liveObj)
{
	Point2I blockPos = { 0, 0 }; // dummy init
	LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

	// 1-in-5 chance to generate landslides near the stamped block.
	if (((uint32)Gods98::Maths_Rand() % 5) == 0) {
		Level_GenerateLandSlideNearBlock(&blockPos, 3, true);
	}

	// Stamp and destroy the path.
	if (Level_Block_IsPath(&blockPos)) {
		Effect_Spawn_SmashPath(liveObj, nullptr);
		AITask_DoClearTypeAction(&blockPos, Message_ClearRemovePathComplete);
		Level_BlockUpdateSurface(Lego_GetLevel(), blockPos.x, blockPos.y, 0);
	}

	// Cause any Mini-Figures carrying crystals within the stamp radius to drop everything.
	for (auto obj : objectListSet.EnumerateSkipUpgradeParts()) {
		LegoObject_Callback_StampMiniFigureWithCrystal(obj, liveObj);
	}
	//LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_Callback_StampMiniFigureWithCrystal, liveObj);
}

// <LegoRR.exe @0043af50>
bool32 __cdecl LegoRR::LegoObject_Callback_StampMiniFigureWithCrystal(LegoObject* targetObj, void* pStamperObj)
{
	LegoObject* stamperObj = (LegoObject*)pStamperObj;

	if (targetObj->type == LegoObject_MiniFigure && (targetObj->flags1 & LIVEOBJ1_CARRYING) &&
		targetObj->carriedObjects[0]->type == LegoObject_PowerCrystal)
	{
		Point2F stamperPos = { 0.0f, 0.0f }; // dummy init
		LegoObject_GetPosition(stamperObj, &stamperPos.x, &stamperPos.y);
		Point2F targetPos = { 0.0f, 0.0f }; // dummy init
		LegoObject_GetPosition(targetObj, &targetPos.x, &targetPos.y);

		// Is target within stamping distance?
		const real32 stampRadius = StatsObject_GetStampRadius(stamperObj);
		if (Gods98::Maths_Vector2DDistance(&targetPos, &stamperPos) < stampRadius) {
			// Drop carried crystal and end routing.
			LegoObject_DropCarriedObject(targetObj, false);
			LegoObject_Route_End(targetObj, false);
		}
	}
	return false;
}

// <LegoRR.exe @0043b010>
LegoRR::LegoObject* __cdecl LegoRR::LegoObject_TryGenerateSlug(LegoObject* originObj, LegoObject_ID objID)
{
	if (objectGlobs.slugHoleCount != 0) {
		Point2I blockPos = { 0 }; // dummy init
		if (originObj == nullptr) {
			const uint32 holeIndex = ((uint32)Gods98::Maths_Rand() % objectGlobs.slugHoleCount);
			blockPos = objectGlobs.slugHoleBlocks[holeIndex];
		}
		else {
			LegoObject_FindNearestSlugHole(originObj, &blockPos);
		}
		return LegoObject_TryGenerateSlugAtBlock(&legoGlobs.rockMonsterData[objID], LegoObject_RockMonster, objID,
												 blockPos.x, blockPos.y, 0.0f, false);
	}
	return nullptr;
}

/// CUSTOM: Generation for slug with a specific block pos already specified.
LegoRR::LegoObject* LegoRR::LegoObject_TryGenerateSlugAtBlock(ObjectModel* objModel, LegoObject_Type objType, LegoObject_ID objID, uint32 bx, uint32 by, real32 heading, bool assignHeading)
{
	if (objType != LegoObject_RockMonster)
		return nullptr;

	bool isSlugHole = false;
	for (uint32 i = 0; i < objectGlobs.slugHoleCount; i++) {
		if (objectGlobs.slugHoleBlocks[i].x == bx && objectGlobs.slugHoleBlocks[i].y == by) {
			isSlugHole = true;
			break;
		}
	}
	if (!isSlugHole)
		return nullptr;

	// If no heading supplied, generate a random one.
	if (!assignHeading) {
		heading = Gods98::Maths_RandRange(0.0f, M_PI*2.0f);
	}

	Point2F wPos = { 0.0f }; // dummy init
	Map3D_BlockToWorldPos(Lego_GetMap(), bx, by, &wPos.x, &wPos.y);
	wPos.x += Maths_Sin(heading) * 12.75f;
	wPos.y += Maths_Cos(heading) * 12.75f;

	LegoObject* slugObj = LegoObject_CreateInWorld(objModel, objType, objID, 0, wPos.x, wPos.y, heading);
	if (slugObj != nullptr) {
		slugObj->flags1 |= LIVEOBJ1_EXPANDING;
		slugObj->flags3 &= ~LIVEOBJ3_POWEROFF;

		LegoObject_SetActivity(slugObj, Activity_Emerge, 0);
		LegoObject_UpdateActivityChange(slugObj);
		AITask_DoAnimationWait(slugObj);
		Info_Send(Info_SlugEmerge, nullptr, slugObj, nullptr);
	}
	return slugObj;
}

// <LegoRR.exe @0043b160>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_TryGenerateRMonsterAtRandomBlock(void);

// <LegoRR.exe @0043b1f0>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_TryGenerateRMonster(CreatureModel* objModel, LegoObject_Type objType, LegoObject_ID objID, uint32 bx, uint32 by);

// <LegoRR.exe @0043b530>
void __cdecl LegoRR::LegoObject_UpdateAll(real32 elapsedGame)
{
	// Were we updating our power grid after the update loop last tick?
	if (objectGlobs.flags & LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERUPDATING) {
		Level_PowerGrid_UnpowerPoweredBlocks();
	}


	objectGlobs.flags |= LegoObject_GlobFlags::OBJECT_GLOB_FLAG_UPDATING;

	for (LegoObject* obj : objectListSet.EnumerateAlive()) {
		LegoObject_Callback_Update(obj, &elapsedGame);
	}
	//LegoObject_RunThroughLists(LegoObject_Callback_Update, &elapsedGame, false);
	
	objectGlobs.flags &= ~LegoObject_GlobFlags::OBJECT_GLOB_FLAG_UPDATING;
	

	// Were we updating our power grid after the update loop last tick?
	if (objectGlobs.flags & LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERUPDATING) {
		Level_PowerGrid_UpdateUnpoweredBlockSurfaces();
	}

	// Stop updating power grid if a recalculation is no longer needed.
	if (!(objectGlobs.flags & LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERNEEDSUPDATE)) {
		objectGlobs.flags &= ~LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERUPDATING;
	}
	else {
		// Otherwise, restart the request for a power grid update (turns on POWERUPDATING flag).
		LegoObject_RequestPowerGridUpdate();
		objectGlobs.flags &= ~LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERNEEDSUPDATE;
	}

	LegoObject_Flocks_Update_FUN_0044c1c0(&elapsedGame);

	objectGlobs.s_sound3DUpdateTimer -= elapsedGame;
	if (objectGlobs.s_sound3DUpdateTimer <= 0.0f) {
		objectGlobs.s_sound3DUpdateTimer = 5.0f * STANDARD_FRAMERATE; // Every 5 seconds
		Lego_UpdateAll3DSounds(true);
	}
}

// <LegoRR.exe @0043b5e0>
void __cdecl LegoRR::LegoObject_RemoveAll(void)
{
	objectGlobs.flags |= LegoObject_GlobFlags::OBJECT_GLOB_FLAG_REMOVING;

	/// TODO: WHY IS SKIPUPGRADEPARTS BEING USED??? Shouldn't we be trying to remove all objects?
	///       Or was that intended to happen after this loop, but was forgotten?
	///       It's also possible upgrade parts are just handled in a wildly different way.
	for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {
		// LegoObject_Callback_Remove is just a wrapper around LegoObject_Remove. *something something, with extra steps.*
		LegoObject_Remove(obj);
		//LegoObject_Callback_Remove(obj, nullptr);
	}
	//LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_Callback_Remove, nullptr);

	objectGlobs.flags &= ~LegoObject_GlobFlags::OBJECT_GLOB_FLAG_REMOVING;
}

// RunThroughLists wrapper for LegoObject_Remove.
// <LegoRR.exe @0043b610>
bool32 __cdecl LegoRR::LegoObject_Callback_Remove(LegoObject* liveObj, void* unused)
{
	// ALWAYS returns false, and even if it didn't we still wouldn't want its return value.
	/*return*/ LegoObject_Remove(liveObj);
	return false;
}

// <LegoRR.exe @0043b620>
//bool32 __cdecl LegoRR::LegoObject_DoPickSphereSelection(uint32 mouseX, uint32 mouseY, LegoObject** selectedObj);

// <LegoRR.exe @0043b670>
//bool32 __cdecl LegoRR::LegoObject_Callback_PickSphereSelection(LegoObject* liveObj, SearchObjectMouseXY_c* search);

// <LegoRR.exe @0043b980>
//void __cdecl LegoRR::LegoObject_DoDragSelection(Gods98::Viewport* view, const Point2F* dragStart, const Point2F* dragEnd);

// <LegoRR.exe @0043ba30>
//bool32 __cdecl LegoRR::LegoObject_CallbackDoSelection(LegoObject* liveObj, SearchViewportWindow_14* search);

// <LegoRR.exe @0043bae0>
//void __cdecl LegoRR::LegoObject_SwapPolyFP(LegoObject* liveObj, uint32 cameraNo, bool32 on);

// <LegoRR.exe @0043bb10>
//void __cdecl LegoRR::LegoObject_FP_SetRanges(LegoRR::LegoObject* liveObj, Gods98::Container* cont, real32 medPolyRange, real32 highPolyRange, bool32 onCont);

// <LegoRR.exe @0043bb90>
//bool32 __cdecl LegoRR::LegoObject_FP_Callback_SwapPolyMeshParts(LegoObject* liveObj, LiveObjectInfo* liveInfo);

// <LegoRR.exe @0043bdb0>
//bool32 __cdecl LegoRR::LegoObject_Check_LotsOfFlags1AndFlags2_FUN_0043bdb0(LegoObject* liveObj);

// <LegoRR.exe @0043bde0>
//void __cdecl LegoRR::LegoObject_FUN_0043bde0(LegoObject* liveObj);

// <LegoRR.exe @0043be80>
//void __cdecl LegoRR::LegoObject_FinishEnteringWallHole(LegoObject* liveObj);

// <LegoRR.exe @0043bf00>
void __cdecl LegoRR::LegoObject_UpdateRemoval(LegoObject* liveObj)
{
	if (liveObj->type != LegoObject_Building) {
		if (liveObj->flags2 & LIVEOBJ2_FROZEN)
			return;

		if (liveObj->flags1 & LIVEOBJ1_TELEPORTINGDOWN)
			return;
	}

	if (liveObj->health > 0.0f)
		return;


	if ((StatsObject_GetStatsFlags1(liveObj) & STATS1_ANYTELEPORTER) &&
		liveObj->teleportDownObject != nullptr &&
		(liveObj->teleportDownObject->flags1 & LIVEOBJ1_TELEPORTINGDOWN))
	{
		// Cancel teleport-in-progress.
		liveObj->teleportDownObject->flags1 &= ~LIVEOBJ1_TELEPORTINGDOWN;
		liveObj->teleportDownObject->health = -1.0f;

		LegoObject_FUN_00438720(liveObj);
		LegoObject_SetActivity(liveObj->teleportDownObject, Activity_Stand, true);
		LegoObject_UpdateActivityChange(liveObj->teleportDownObject);
	}


	if (liveObj->flags3 & LIVEOBJ3_REMOVING) {
		// Object is already marked for removing. Handle final cleanup.
		if (liveObj->driveObject != nullptr) {
			// Eject driver.
			Interface_ChangeMenu_IfPrimarySelectedVehicle_IsLiveObject(liveObj);
			liveObj->driveObject->flags2 &= ~LIVEOBJ2_DRIVING;
			liveObj->driveObject->driveObject = nullptr;
			liveObj->driveObject = nullptr;
		}

		if (liveObj->type == LegoObject_MiniFigure && (liveObj->flags4 & LIVEOBJ4_UNK_8)) {
			ObjectRecall_StoreMiniFigure(liveObj);
		}

		if (liveObj->type == LegoObject_Vehicle) {
			Vehicle_SetUpgradeLevel(liveObj->vehicle, 0);
		}

		LegoObject_FUN_0044b0a0(liveObj);

		// Handle different types of removal.
		if (liveObj->type == LegoObject_Boulder) {
			LegoObject_DestroyBoulder_AndCreateExplode(liveObj);
		}
		else if (liveObj->type == LegoObject_Building) {
			Construction_RemoveBuildingObject(liveObj);
		}
		else {
			LegoObject_Remove(liveObj);
		}
		return;
	}

	if (liveObj->type == LegoObject_PowerCrystal) {
		// No clue why this is only handled for power crystals and not other simple objects.
		liveObj->flags3 |= LIVEOBJ3_REMOVING;
		return;
	}
	
	if (liveObj->type == LegoObject_RockMonster) {
		if (StatsObject_GetStatsFlags2(liveObj) & STATS2_SPLITONZEROHEALTH) {

			if (!(liveObj->flags1 & LIVEOBJ1_CRUMBLING)) {
				LegoObject_StartCrumbling(liveObj);
			}
		}
		else if (StatsObject_GetStatsFlags2(liveObj) & STATS2_USEHOLES) {

			LegoObject_Interrupt(liveObj, false, true);

			liveObj->flags1 |= LIVEOBJ1_ENTERING_WALLHOLE;
			LegoObject_SetActivity(liveObj, Activity_Enter, false);
			LegoObject_UpdateActivityChange(liveObj);
		}
		return;
	}
	
	if (liveObj->type == LegoObject_MiniFigure || liveObj->type == LegoObject_Vehicle ||
		liveObj->type == LegoObject_Building || liveObj->type == LegoObject_ElectricFence)
	{
		// A lot of logic for the remaining object types.

		if (liveObj->type == LegoObject_Building &&
			!(liveObj->teleporter_modeFlags & 0x4) && !(liveObj->flags4 & LIVEOBJ4_UNK_8))
		{
			if (liveObj->flags2 & LIVEOBJ2_GETTINGSERVICED)
				return;

			liveObj->flags2 |= LIVEOBJ2_GETTINGSERVICED;
			Construction_CleanupBuildingFoundation(liveObj);
			LegoObject_SetActivity(liveObj, Activity_Explode, 0);
			LegoObject_UpdateActivityChange(liveObj);
			AITask_StopRepairForObject(liveObj);
		}
		else {

			if (!(liveObj->flags3 & LIVEOBJ3_CANDAMAGE))
				return;

			if (liveObj->flags1 & LIVEOBJ1_TELEPORTINGUP)
				return;

			const char* actName = liveObj->activityName1;
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);

			if (liveObj->flags4 & LIVEOBJ4_UNK_8) {
				LegoObject_FUN_0044b0a0(liveObj);
			}

			if (liveObj->type == LegoObject_Building) {
				Construction_CleanupBuildingFoundation(liveObj);
			}

			if (liveObj->flags4 & LIVEOBJ4_ENTRANCEOCCUPIED) {
				if (liveObj->routeToObject != nullptr) {
					liveObj->routeToObject->flags4 &= ~LIVEOBJ4_ENTRANCEOCCUPIED;
					liveObj->routeToObject = nullptr;
				}
			}

			// Handle driven objects.
			if (liveObj->driveObject != nullptr) {
				Point2I blockPos = { 0, 0 }; // dummy init
				LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
				if ((StatsObject_GetStatsFlags3(liveObj) & STATS3_CARRYVEHICLES) && (liveObj->flags1 & LIVEOBJ1_CARRYING) &&
					liveObj->carriedObjects[0] != nullptr &&
					!Lego_GetCrossTerrainType(liveObj->carriedObjects[0], blockPos.x, blockPos.y, blockPos.x, blockPos.y, true))
				{
					// Destroy the carried vehicle along with this object.
					liveObj->carriedObjects[0]->health = -1.0f;
				}

				if (!(StatsObject_GetStatsFlags1(liveObj) & STATS1_CANBEDRIVEN) ||
					Lego_GetCrossTerrainType(liveObj->driveObject, blockPos.x, blockPos.y, blockPos.x, blockPos.y, true))
				{
					// Eject the driver.
					Interface_ChangeMenu_IfPrimarySelectedVehicle_IsLiveObject(liveObj);

					liveObj->driveObject->flags2 &= ~LIVEOBJ2_DRIVING;

					if (liveObj->driveObject->type == LegoObject_MiniFigure) {
						Creature_SetActivity(liveObj->driveObject->miniFigure, objectGlobs.activityName[Activity_Stand], 0.0f);
					}
					liveObj->driveObject->driveObject = nullptr;
					liveObj->driveObject = nullptr;
				}
				else {
					// Otherwise kill the driver.
					liveObj->driveObject->health = -1.0f;
				}
			}

			if (liveObj->type == LegoObject_ElectricFence) {
				ElectricFence_RemoveFence(liveObj);
			}

			// Send death messages.
			{
				Point2I blockPos = { 0, 0 }; // dummy init
				LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
				switch (liveObj->type) {
				case LegoObject_Vehicle:
					Info_Send(Info_GenericDeath, nullptr, nullptr, &blockPos);
					Info_Send(Info_VehicleDeath, nullptr, nullptr, &blockPos);
					break;
				case LegoObject_MiniFigure:
					Info_Send(Info_GenericDeath, nullptr, nullptr, &blockPos);
					Info_Send(Info_LegoManDeath, nullptr, nullptr, &blockPos);
					break;
				case LegoObject_Building:
					Info_Send(Info_GenericDeath, nullptr, nullptr, &blockPos);
					Info_Send(Info_BuildingDeath, nullptr, nullptr, &blockPos);
					break;
				}
			}

			// Interrupt the object.
			if (!(liveObj->flags2 & LIVEOBJ2_THROWN)) {
				LegoObject_Interrupt(liveObj, false, true); // Normal interrupt.

				Gods98::Container_SetActivity(cont, actName);
				liveObj->activityName2 = liveObj->activityName1;
			}
			else {
				LegoObject_Interrupt(liveObj, true, true); // Thrown interrupt.
			}

			liveObj->flags1 |= LIVEOBJ1_TELEPORTINGUP;

			// Create the teleporting up effect animation.
			liveObj->contMiniTeleportUp = Gods98::Container_Clone(legoGlobs.contMiniTeleportUp);
			Gods98::Container_SetAnimationTime(liveObj->contMiniTeleportUp, 0.0f);
			Gods98::Container_Hide(liveObj->contMiniTeleportUp, false);
			Gods98::Container* refCont = LegoObject_GetActivityContainer(liveObj);
			Gods98::Container_SetPosition(liveObj->contMiniTeleportUp, refCont, 0.0f, 0.0f, 0.0f);
			Gods98::Container_SetOrientation(liveObj->contMiniTeleportUp, nullptr, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f);

			// Scale up the teleporting up effect animation for buildings.
			if (liveObj->type == LegoObject_Building) {
				real32 collRadius = StatsObject_GetCollRadius(liveObj);
				if (collRadius <= 0.0f) {
					collRadius = 4.0f; // Default radius.
				}

				// Note: Condition should always be true unless NaN.
				if (collRadius > 0.0f) {
					Gods98::Container_AddScale(liveObj->contMiniTeleportUp, Gods98::Container_Combine::Before,
											   (collRadius / 4.0f), 1.0f, (collRadius / 4.0f));
				}
			}
		}

		Message_DeselectObject(liveObj);
		Message_RemoveObjectReference(liveObj);
		return;
	}
}

// <LegoRR.exe @0043c4c0>
//bool32 __cdecl LegoRR::LegoObject_CanSupportOxygenForType(LegoObject_Type consumerType, LegoObject_ID consumerID, LegoObject_Type producerType, LegoObject_ID producerID);

// <LegoRR.exe @0043c540>
//bool32 __cdecl LegoRR::LegoObject_Callback_SumOfOxygenCoefs(LegoObject* liveObj, real32* oxygenCoef);

// DRAW MODE: Only Draw API drawing calls can be used within this function.
// <LegoRR.exe @0043c570>
void __cdecl LegoRR::LegoObject_UpdateAllRadarSurvey(real32 elapsedGame, bool32 inRadarMapView)
{
	objectGlobs.radarSurveyCycleTimer += elapsedGame;
	// Survey dots have a one second duration.
	if (objectGlobs.radarSurveyCycleTimer > OBJECT_SURVEY_CYCLEDURATION) {
		objectGlobs.radarSurveyCycleTimer = 0.0f;
	}

	for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {
		LegoObject_Callback_UpdateRadarSurvey(obj, &inRadarMapView);
	}
	//LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_Callback_UpdateRadarSurvey, &inRadarMapView);
}

// DRAW MODE: Only Draw API drawing calls can be used within this function.
// DATA: bool32* inRadarMapView
// <LegoRR.exe @0043c5b0>
bool32 __cdecl LegoRR::LegoObject_Callback_UpdateRadarSurvey(LegoObject* liveObj, void* pInRadarMapView)
{
	const bool32 inRadarMapView = *static_cast<bool32*>(pInRadarMapView);

	if (liveObj->abilityFlags & ABILITY_FLAG_SCANNER) {
		const uint32 surveyRadius = StatsObject_GetSurveyRadius(liveObj);
		if (surveyRadius <= 0)
			return false; // No distance to survey.

		if (liveObj->type == LegoObject_Building && !LegoObject_IsActive(liveObj, false))
			return false; // Building is not currently surveying.

		if (objectGlobs.radarSurveyCycleTimer == 0.0f) {
			// Start of next survey cycle. Perform survey checks now.
			// Also this means the survey dots are at their center, so we don't draw them.
			Point2I blockPos = { 0 }; // dummy init
			LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
			Level_Block_UpdateSurveyRadius_FUN_00434f40(&blockPos, surveyRadius);
		}
		else if (inRadarMapView) {
			Point2F centerPos = { 0.0f }; // dummy init
			LegoObject_GetPosition(liveObj, &centerPos.x, &centerPos.y);

			// Range from [0.0,1.0].
			const real32 range = (objectGlobs.radarSurveyCycleTimer / OBJECT_SURVEY_CYCLEDURATION);

			const real32 blockSize = Map3D_BlockSize(Lego_GetMap());
			// Dots draw starting from the center of the unit and expand out to `surveyRadius` blocks.
			const real32 radius = (blockSize * range * static_cast<real32>(surveyRadius));
			// Dots get darker the further out they're drawn.
			const real32 brightness = 1.0f - range;

			RadarMap_DrawSurveyDotCircle(Lego_GetRadarMap(), &centerPos, radius, brightness);
		}
	}
	return false;
}

// <LegoRR.exe @0043c6a0>
//bool32 __cdecl LegoRR::LegoObject_FUN_0043c6a0(LegoObject* liveObj);

// <LegoRR.exe @0043c700>
//LegoRR::Weapon_KnownType __cdecl LegoRR::LegoObject_GetEquippedBeam(LegoObject* liveObj);

// <LegoRR.exe @0043c750>
//bool32 __cdecl LegoRR::LegoObject_IsTargetActiveAndValidWeapon(LegoObject* liveObj, LegoObject* routeToObject, Weapon_KnownType knownWeapon);

// <LegoRR.exe @0043c780>
//void __cdecl LegoRR::LegoObject_Proc_FUN_0043c780(LegoObject* liveObj);

// <LegoRR.exe @0043c7f0>
//void __cdecl LegoRR::LegoObject_Proc_FUN_0043c7f0(LegoObject* liveObj);

// <LegoRR.exe @0043c830>
void __cdecl LegoRR::LegoObject_UpdatePowerConsumption(LegoObject* liveObj)
{
	if (liveObj->type == LegoObject_Building && !(liveObj->flags3 & LIVEOBJ3_POWEROFF)) {
		const sint32 crystalDrainedAmount = StatsObject_GetCrystalDrain(liveObj);

		Point2I blockPos = { 0 }; // dummy init
		LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

		const StatsFlags2 sflags2 = StatsObject_GetStatsFlags2(liveObj);

		if (!(sflags2 & (STATS2_GENERATEPOWER|STATS2_SELFPOWERED))) {

			if (!Construction_PowerGrid_DrainAdjacentBlocks(&blockPos, crystalDrainedAmount)) {
				liveObj->flags3 &= ~LIVEOBJ3_HASPOWER;
				Bubble_ShowPowerOff(liveObj);
				return;
			}
		}
		liveObj->flags3 |= LIVEOBJ3_HASPOWER;

		if (sflags2 & STATS2_GENERATEPOWER) {
			// Check if building generates power, and is not "powered-off"
			//  (which is normally not possible outside of debug mode).
			if (LegoObject_IsActive(liveObj, false)) {
				if (Level_GetCrystalCount(true) != 0) {
					Construction_PowerGrid_PowerAdjacentBlocks(&blockPos);
				}
			}
		}
	}
}

// <LegoRR.exe @0043c910>
bool32 __cdecl LegoRR::LegoObject_CheckCanSteal(LegoObject* liveObj)
{
	if (StatsObject_GetStatsFlags1(liveObj) & STATS1_CANSTEAL) {
		const uint32 capacity = StatsObject_GetCapacity(liveObj);
		if (capacity != 0 && liveObj->stolenCrystalLevels != nullptr &&
			(uint32)*liveObj->stolenCrystalLevels >= capacity)
		{
			// Already at capacity for stolen crystals.
			return false;
		}

		if (!(liveObj->flags3 & LIVEOBJ3_POWEROFF)) {
			return true;
		}
	}
	return false;
}

// <LegoRR.exe @0043c970>
void __cdecl LegoRR::LegoObject_UpdateElapsedTimes(LegoObject* liveObj, real32 elapsed)
{
	const real32 lastElapsedTime1 = liveObj->elapsedTime1;
	const real32 lastElapsedTime2 = liveObj->elapsedTime2;

	liveObj->elapsedTime1 += elapsed;
	liveObj->elapsedTime2 += elapsed;
	liveObj->eatWaitTimer += elapsed;

	switch (liveObj->type) {
	case LegoObject_MiniFigure:
		{
			const real32 upgradeTime = Lego_GetObjectUpgradeTime(liveObj->type);
			if (upgradeTime != 0.0f) {
				if (lastElapsedTime2 < upgradeTime && liveObj->elapsedTime2 >= upgradeTime) {
					Info_Send(Info_CanUpgradeMinifigure, nullptr, liveObj, nullptr);
				}
			}

			const real32 trainTime = Lego_GetTrainTime();
			if (trainTime != 0.0f) {
				if (lastElapsedTime1 < trainTime && liveObj->elapsedTime1 >= trainTime) {
					Info_Send(Info_CanTrainMinifigure, nullptr, liveObj, nullptr);
				}
			}
		}
		break;

	case LegoObject_PowerCrystal:
	case LegoObject_Ore:
	case LegoObject_Barrier:
	case LegoObject_ElectricFence:
		// Can only carry inactive electric fences (ones that aren't already setup in position).
		if (liveObj->type != LegoObject_ElectricFence || !(liveObj->flags2 & LIVEOBJ2_ACTIVEELECTRICFENCE)) {

			const real32 unknownTime = (STANDARD_FRAMERATE * 30.0f); // 30 seconds.
			if (lastElapsedTime2 < unknownTime && liveObj->elapsedTime2 >= unknownTime) {
				liveObj->elapsedTime2 = 0.0f;
				AITask_LiveObject_FUN_004025f0(liveObj);
			}
		}
		break;
	}
}

// <LegoRR.exe @0043cad0>
bool32 __cdecl LegoRR::LegoObject_Callback_Update(LegoObject* liveObj, void* pElapsed)
{
	const real32 elapsed = *(real32*)pElapsed;

	bool shouldStopDrillSound = true;

	//bool32 reinforcingFinished = false; // dummy init
	//real32 dummyOutTransSpeed = 0.0f; // dummy init

	// Assigned to but never used.
	//static LegoObject* s_currentUpdateObject = nullptr;
	s_currentUpdateObject = liveObj;
	//liveObj_00 = liveObj;

	// Used for LIVEOBJ2_DAMAGESHAKING flag.
	//static real32 s_objectDamageShakeTimer = 1.0f;

	LegoObject_UpdateElapsedTimes(liveObj, elapsed);


	if (liveObj->flags4 & LIVEOBJ4_UNK_200000) {
		// Some kind of update/end/change activity flag.
		liveObj->activityName1 = nullptr;
		LegoObject_UpdateActivityChange(liveObj);
		liveObj->flags4 &= ~LIVEOBJ4_UNK_200000;
	}

	if (liveObj->flags2 & LIVEOBJ2_FROZEN) {
		// Object can't do anything else while frozen.
		if (liveObj->freezeTimer <= 0.0f) {
			LegoObject_FUN_0044c3d0(liveObj);
		}
		liveObj->freezeTimer -= elapsed;
		return false;
	}


	if (liveObj->flags4 & LIVEOBJ4_UNK_40000) {
		Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
		if (cont != nullptr) {
			const SFX_ID engineSFXID = StatsObject_GetEngineSound(liveObj);
			if (engineSFXID != SFX_NULL) {
				liveObj->engineSoundHandle = SFX_Random_PlaySound3DOnContainer(cont, engineSFXID, true, true, nullptr);
				liveObj->flags4 |= LIVEOBJ4_ENGINESOUNDPLAYING;
			}
		}
		liveObj->flags4 &= ~LIVEOBJ4_UNK_40000;
	}


	if ((legoGlobs.flags2 & GAME2_CALLTOARMS) && LegoObject_MiniFigureHasBeamEquipped2(liveObj)) {
		// Update MiniFigure with Call-to-arms behavior?
		Bubble_ShowCallToArms(liveObj);
		liveObj->flags4 |= LIVEOBJ4_CALLTOARMS;
	}
	else {
		Bubble_SetCallToArmsTimer(liveObj, 0.0f);
		liveObj->flags4 &= ~LIVEOBJ4_CALLTOARMS;
	}


	if (liveObj->type == LegoObject_Vehicle && (liveObj->flags4 & LIVEOBJ4_DOCKOCCUPIED) &&
		liveObj->routeToObject == nullptr && liveObj->driveObject == nullptr)
	{
		liveObj->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
		LegoObject_WaterVehicle_Register(liveObj);
	}


	if (liveObj->type == LegoObject_MiniFigure || liveObj->type == LegoObject_Vehicle) {
		Lego_Level* level = Lego_GetLevel();
		Point2I blockPos = { 0, 0 }; // dummy init
		LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
		if (blockValue(level, blockPos.x, blockPos.y).flags2 & BLOCK2_EMERGE_TRIGGER) {
			Level_HandleEmergeTriggers(level, &blockPos, nullptr);
		}
	}


	LegoObject_FUN_0043bde0(liveObj);


	if (liveObj->type == LegoObject_MiniFigure && (liveObj->flags2 & LIVEOBJ2_DRIVING) &&
		liveObj->driveObject != nullptr && liveObj->driveObject->type == LegoObject_Vehicle)
	{
		const real32 vehicleAnimTime = Vehicle_GetAnimationTime(liveObj->driveObject->vehicle);
		Creature_SetAnimationTime(liveObj->miniFigure, vehicleAnimTime);
	}
	else {
		// dummyOutTransSpeed is only passed to a function as a dummy output argument.
		/*dummyOutTransSpeed =*/ LegoObject_MoveAnimation(liveObj, elapsed);
	}


	if (StatsObject_GetStatsFlags1(liveObj) & STATS1_ANYTELEPORTER) {
		LegoObject_UpdateTeleporter(liveObj);
	}

	if (liveObj->type != LegoObject_Building || LegoObject_IsActive(liveObj, false)) {
		Level_ConsumeObjectOxygen(liveObj, elapsed);
	}

	if (!(objectGlobs.flags & LegoObject_GlobFlags::OBJECT_GLOB_FLAG_POWERUPDATING)) {
		liveObj->flags3 &= ~LIVEOBJ3_UNK_40000000;
	}
	else {
		LegoObject_UpdatePowerConsumption(liveObj);
	}


	if (liveObj->flags1 & LIVEOBJ1_TELEPORTINGUP) {
		if (liveObj->contMiniTeleportUp == nullptr) {
			// Teleporting, but no teleport animation, so finish instantly??
			// This seems bugged, since it doesn't unset the LIVEOBJ1_TELEPORTINGUP flag.
			// Nor does it set health to -1.0f.
			liveObj->flags3 |= LIVEOBJ3_REMOVING;
		}
		else {
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
			Gods98::Container_AddTranslation(cont, Gods98::Container_Combine::After, 0.0f, 0.0f, -elapsed);
			const real32 animOverrun = Gods98::Container_MoveAnimation(liveObj->contMiniTeleportUp, elapsed);
			if (animOverrun > 0.0f) {
				// Teleport animation finished. Done teleporting up.
				liveObj->health = -1.0f;
				liveObj->flags3 |= LIVEOBJ3_REMOVING;
				liveObj->flags1 &= ~LIVEOBJ1_TELEPORTINGUP;
				Gods98::Container_Remove(liveObj->contMiniTeleportUp);
			}
		}
		return false;
	}


	if (liveObj->flags1 & LIVEOBJ1_DRILLING) {

		if (liveObj->flags1 & LIVEOBJ1_DRILLINGSTART) {
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
			const SFX_ID drillSFXID = StatsObject_GetDrillSoundType(liveObj, false);
			liveObj->drillSoundHandle = SFX_Random_PlaySound3DOnContainer(cont, drillSFXID, true, true, nullptr);

			liveObj->flags4 |= LIVEOBJ4_DRILLSOUNDPLAYING;
			liveObj->flags1 &= ~LIVEOBJ1_DRILLINGSTART;
		}


		bool doDrillSound = false;
		const Point2I target = {
			(sint32)liveObj->targetBlockPos.x,
			(sint32)liveObj->targetBlockPos.y,
		};
		Point2F drillNullPos = { 0.0f, 0.0f }; // dummy init


		if (liveObj->routeBlocks == nullptr ||
			!LiveObject_BlockCheck_FUN_004326a0(liveObj, (uint32)target.x, (uint32)target.y,
												(liveObj->flags3 & LIVEOBJ3_UNK_2000000), true))
		{
			liveObj->flags1 &= ~LIVEOBJ1_DRILLING;
			liveObj->flags3 &= ~LIVEOBJ3_UNK_2000000;
			doDrillSound = true;
		}
		else if (LegoObject_GetDrillNullPosition(liveObj, &drillNullPos.x, &drillNullPos.y)) {

			const Point2I routeBlockPos = liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos;

			if (liveObj->type == LegoObject_MiniFigure) {
				Gods98::Container* contCam = Lego_GetCurrentCamera_Container();
				Gods98::Container* drillNull = Creature_GetDrillNull(liveObj->miniFigure);
				//util::logf_removed((char*)drillNull, contCam, 0);
			}

			//target = Point2I {
			//	(sint32)liveObj->targetBlockPos.x,
			//	(sint32)liveObj->targetBlockPos.y,
			//};
			real32 drillTime = StatsObject_GetDrillTimeType(liveObj, Lego_GetBlockTerrain(target.x, target.y));
			if ((StatsObject_GetStatsFlags1(liveObj) & STATS1_SINGLEWIDTHDIG) &&
				(liveObj->flags3 & LIVEOBJ3_UNK_2000000))
			{
				drillTime /= 2.0f;
			}

			if (!Level_Block_Damage((uint32)target.x, (uint32)target.y, drillTime, elapsed)) {

				Point2I drillNullBlockPos = { 0, 0 }; // dummy init
				if (Lego_WorldToBlockPos_NoZ(drillNullPos.x, drillNullPos.y, &drillNullBlockPos.x, &drillNullBlockPos.y) &&
					drillNullBlockPos.x == routeBlockPos.x && drillNullBlockPos.y == routeBlockPos.y)
				{
					liveObj->flags3 |= LIVEOBJ3_UNK_4000;
				}
			}
			else if (LiveObject_BlockCheck_FUN_004326a0(liveObj, (uint32)target.x, (uint32)target.y,
														(liveObj->flags3 & LIVEOBJ3_UNK_2000000), true))
			{
				//target = Point2I {
				//	(sint32)liveObj->targetBlockPos.x,
				//	(sint32)liveObj->targetBlockPos.y,
				//};
				bool wallDestroyed;
				if (!(liveObj->flags3 & LIVEOBJ3_UNK_2000000)) {
					wallDestroyed = Level_DestroyWall(Lego_GetLevel(), (uint32)routeBlockPos.x, (uint32)routeBlockPos.y, false);
				}
				else {
					wallDestroyed = Level_DestroyWallConnection(Lego_GetLevel(), (uint32)target.x, (uint32)target.y);
				}

				if (wallDestroyed) {
					AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Dig, nullptr, true);
					liveObj->flags1 &= ~LIVEOBJ1_DRILLING;

					Level_Block_SetBusy(&target, false);

					if (liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].actionByte == ROUTE_ACTION_UNK_1) {
						liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].actionByte = ROUTE_ACTION_NONE;
					}

					doDrillSound = true;
				}
			}
		}
		
		if (doDrillSound) {
			SFX_Sound3D_StopSound(liveObj->drillSoundHandle);
			liveObj->flags4 &= ~LIVEOBJ4_DRILLSOUNDPLAYING;

			/// TODO: What is happening with the drill sound here?
			///       We're stopping it then playing it again without storing the handle...
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
			const SFX_ID drillSFXID = StatsObject_GetDrillSoundType(liveObj, true);
			SFX_Random_PlaySound3DOnContainer(cont, drillSFXID, false, true, nullptr);
			//util::logf_removed(nullptr, 0, 0);
			liveObj->flags3 &= ~LIVEOBJ3_UNK_4000;
		}
	}


	if (liveObj->flags2 & LIVEOBJ2_PUSHED) {
		LegoObject_UpdatePushing(liveObj, elapsed);
	}


	if ((liveObj->flags2 & LIVEOBJ2_DAMAGESHAKING) && elapsed > 0.0f) {
		Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
		if (cont != nullptr) {
			Vector3F dir, up;
			Gods98::Container_GetOrientation(cont, nullptr, &dir, &up);
			Gods98::Maths_Vector3DScale(&up, &up, 10.0f);

			Vector3F rngVec;
			Gods98::Maths_Vector3DRandom(&rngVec);

			/// FIXME: We shouldn't be using a static variable here if this flag is used by more than one object at a time.
			Gods98::Maths_Vector3DAdd(&up, &up, &rngVec);
			Gods98::Maths_Vector3DScale(&up, &up, (s_objectDamageShakeTimer * 0.1f));
			//up.z = -1.0f;

			/// TODO: Do we not need to normalize up before passing it to SetOrientation?
			Gods98::Container_SetOrientation(cont, nullptr, dir.x, dir.y, dir.z, up.x, up.y, -1.0f);

			s_objectDamageShakeTimer -= elapsed * 0.16f;
			if (s_objectDamageShakeTimer < 0.0f) {
				s_objectDamageShakeTimer = 1.0f;
				liveObj->flags2 &= ~LIVEOBJ2_DAMAGESHAKING;
			}
		}
	}


	if (liveObj->flags1 & LIVEOBJ1_CRUMBLING) {
		LegoObject_DestroyRockMonster_FUN_0044c290(liveObj);
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_SLIPPING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_SLIPPING;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags4 & LIVEOBJ4_UNK_1000) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags4 &= ~LIVEOBJ4_UNK_1000;
			liveObj->flags4 |= LIVEOBJ4_UNK_2000;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags4 & LIVEOBJ4_UNK_4000) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags4 &= ~LIVEOBJ4_UNK_4000;
			LegoObject_SetActivity(liveObj, Activity_Stand, 0);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags4 & LIVEOBJ4_UNK_2000) {
		LegoObject_SetActivity(liveObj, Activity_Open, 0);
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_PUSHED) {
		// Can't do anything while pushed?
		goto objectupdate_end;
	}


	/// TODO: Is this flag name accurate?
	if (liveObj->flags1 & LIVEOBJ1_TURNING) {
		/// FIXME: Is turning not based on elapsed time???
		if (liveObj->animTime > 0.0f) {
			if (liveObj->flags1 & LIVEOBJ1_TURNRIGHT) {
				Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
				Gods98::Container_AddRotation(cont, Gods98::Container_Combine::Before, 0.0f, 1.0f, 0.0f, (M_PI / 2.0f));
				Gods98::Container_SetAnimationTime(cont, 0.0f);
				liveObj->flags1 &= ~LIVEOBJ1_TURNRIGHT;
			}
			else {
				real32 angle;
				if (liveObj->activityName1 == objectGlobs.activityName[Activity_TurnLeft] ||
					liveObj->activityName1 == objectGlobs.activityName[Activity_CarryTurnLeft])
				{
					angle = -(M_PI / 2.0f);
				}
				else {
					angle = (M_PI / 2.0f);
				}
				Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
				Gods98::Container_AddRotation(cont, Gods98::Container_Combine::Before, 0.0f, 1.0f, 0.0f, angle);

				liveObj->flags1 &= ~LIVEOBJ1_TURNING;
				liveObj->flags1 |= LIVEOBJ1_MOVING;

				if (liveObj->routeBlocksTotal <= 0) {
					LegoObject_SetActivity(liveObj, Activity_Stand, 0);
				}
				else if ((liveObj->flags1 & LIVEOBJ1_RUNNINGAWAY) && liveObj->type == LegoObject_MiniFigure) {
					LegoObject_SetActivity(liveObj, Activity_RunPanic, 0);
				}
				else {
					LegoObject_SetActivity(liveObj, Activity_Route, 0);
				}
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_DEPARTING) {
		LegoObject_TryDepart_FUN_004499c0(liveObj);
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_CANTDO) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_CANTDO;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_UNUSED_10000000) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_UNUSED_10000000;
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
			Gods98::Container_SetOrientation(cont, nullptr, liveObj->dirVector_2c8.x, liveObj->dirVector_2c8.y, 0.0f, 0.0f, 0.0f, -1.0f);
			LegoObject_SetActivity(liveObj, Activity_Stand, 0);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_FIRINGLASER) {
		if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
			LegoObject_CreateWeaponProjectile(liveObj, Weapon_KnownType_Laser);
		}
		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_FIRINGLASER;

			if (!(liveObj->flags2 & LIVEOBJ2_ACTIVITYCHANGING)) {
				LegoObject_CreateWeaponProjectile(liveObj, Weapon_KnownType_Laser);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags4 & LIVEOBJ4_UNK_8000) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags4 &= ~LIVEOBJ4_UNK_8000;

			if (liveObj->routeToObject != nullptr) {
				Gods98::Container* cont = LegoObject_GetDepositNull(liveObj->routeToObject);
				if (cont != nullptr) {
					Vector3F wPos, dir;
					Gods98::Container_GetPosition(cont, nullptr, &wPos);
					Gods98::Container_GetOrientation(cont, nullptr, &dir, nullptr);

					wPos.z = Map3D_GetWorldZ(Lego_GetMap(), wPos.x, wPos.y);
					Gods98::Container_SetPosition(cont, nullptr, wPos.x, wPos.y, wPos.z);
					Gods98::Container_SetOrientation(cont, nullptr, dir.x, dir.y, dir.z, 0.0f, 0.0f, -1.0f);
				}

				LegoObject_DoOpeningClosing(liveObj->routeToObject, false);
			}
			LegoObject_SetActivity(liveObj, Activity_Stand, 0);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_UNK_80000000) {
		if (liveObj->routeToObject == nullptr) {
			liveObj->flags2 &= ~LIVEOBJ2_UNK_80000000;
		}
		else {
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
			Gods98::Container* routeToCont = LegoObject_GetActivityContainer(liveObj->routeToObject);
			LegoObject* routeToObj = liveObj->routeToObject;

			Vector3F wPos, dir, up;
			Gods98::Container_GetPosition(routeToCont, nullptr, &wPos);
			Gods98::Container_GetOrientation(routeToCont, nullptr, &dir, &up);
			Gods98::Container_SetPosition(cont, nullptr, wPos.x, wPos.y, wPos.z);
			Gods98::Container_SetOrientation(cont, nullptr, dir.x, dir.y, dir.z, up.x, up.y, up.z);

			if (liveObj->animTime > 0.0f) {
				routeToObj->carriedObjects[routeToObj->numCarriedObjects] = liveObj;
				routeToObj->carriedObjects[routeToObj->numCarriedObjects]->carryingThisObject = routeToObj;
				routeToObj->carriedObjects[routeToObj->numCarriedObjects]->carryingIndex = routeToObj->numCarriedObjects;

				routeToObj->flags1 |= LIVEOBJ1_CARRYING;
				LegoObject_DoOpeningClosing(routeToObj, false);
				routeToObj->numCarriedObjects++;

				liveObj->flags2 &= ~LIVEOBJ2_UNK_80000000;
				LegoObject_SetActivity(liveObj, Activity_Stand, 0);
				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_FindLoad, nullptr, true);

				routeToObj->flags4 &= ~LIVEOBJ4_UNK_10000;
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_RECHARGING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_RECHARGING;

			if (liveObj->flags1 & LIVEOBJ1_CARRYING) {
				liveObj->carriedObjects[0]->flags3 &= ~LIVEOBJ3_POWEROFF;

				Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj->carriedObjects[0]);
				Vector3F wPos;
				Gods98::Container_GetPosition(cont, nullptr, &wPos);
				SFX_Random_PlaySound3DOnContainer(nullptr, SFX_CrystalRecharge, false, false, &wPos);
				LegoObject_SetCrystalPoweredColour(liveObj->carriedObjects[0], true);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_FIRINGPUSHER) {
		if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
			LegoObject_CreateWeaponProjectile(liveObj, Weapon_KnownType_Pusher);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_FIRINGPUSHER;

			if (!(liveObj->flags2 & LIVEOBJ2_ACTIVITYCHANGING)) {
				LegoObject_CreateWeaponProjectile(liveObj, Weapon_KnownType_Pusher);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_FIRINGFREEZER) {
		if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
			LegoObject_CreateWeaponProjectile(liveObj, Weapon_KnownType_Freezer);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_FIRINGFREEZER;

			if (!(liveObj->flags2 & LIVEOBJ2_ACTIVITYCHANGING)) {
				LegoObject_CreateWeaponProjectile(liveObj, Weapon_KnownType_Freezer);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_UNK_800) {
		if (liveObj->type == LegoObject_RockMonster && Creature_CheckThrowNull(liveObj->rockMonster)) {
			LegoObject_FUN_00447a40(liveObj);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_UNK_800;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_UNK_4000) {
		LegoObject* routeToObj = liveObj->routeToObject;
		if (routeToObj != nullptr && !(routeToObj->flags1 & LIVEOBJ1_STORING) &&
			!(routeToObj->flags3 & LIVEOBJ3_UNK_1000000) && routeToObj->numCarriedObjects != routeToObj->carryNullFrames)
		{
			liveObj->flags1 &= ~LIVEOBJ1_UNK_4000;
			liveObj->flags1 |= LIVEOBJ1_STORING;

		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_STORING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_STORING;

			AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Deposit, liveObj, true);
			LegoObject_ProccessCarriedObjects(liveObj);
		}
		else if (liveObj->routeToObject != nullptr) {
			liveObj->routeToObject->flags3 |= LIVEOBJ3_UNK_1000000;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_RESTING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_RESTING;
			liveObj->flags3 &= ~LIVEOBJ3_UNK_4000;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_ATTACKINGPATH) {
		if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
			LegoObject_AttackPath(liveObj);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_ATTACKINGPATH;

			AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_AttackPath, nullptr, true);
			if (!(liveObj->flags2 & LIVEOBJ2_ACTIVITYCHANGING)) {
				LegoObject_AttackPath(liveObj);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_BUMPDAMAGING) {
		if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
			LegoObject_BumpDamageRouteObject(liveObj);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_BUMPDAMAGING;

			if (!(liveObj->flags2 & LIVEOBJ2_ACTIVITYCHANGING)) {
				LegoObject_BumpDamageRouteObject(liveObj);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_WAKINGUP) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_WAKINGUP;
			liveObj->flags3 &= ~LIVEOBJ3_POWEROFF;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_REPAIRDRAINING) {
		if ((StatsObject_GetStatsFlags2(liveObj) & STATS2_DRAINPOWER) && liveObj->routeToObject != nullptr &&
			!(liveObj->routeToObject->flags4 & LIVEOBJ4_UNK_10))
		{
			if (LegoObject_IsActive(liveObj->routeToObject, false)) {
				Info_Send(Info_PowerDrain, nullptr, liveObj->routeToObject, nullptr);
				Lego_SetCallToArmsOn(true);
			}
			liveObj->routeToObject->flags4 |= LIVEOBJ4_UNK_10;
			LegoObject_RequestPowerGridUpdate();
		}

		if ((liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) && LegoObject_FUN_00447880(liveObj)) {
			liveObj->flags2 &= ~LIVEOBJ2_UNK_80000;
		}

		if (liveObj->animTime > 0.0f) {
			if (!(liveObj->flags2 & LIVEOBJ2_ACTIVITYCHANGING) && LegoObject_FUN_00447880(liveObj)) {
				liveObj->flags2 &= ~LIVEOBJ2_UNK_80000;

				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Repair, nullptr, true);
			}

			if (!(liveObj->flags2 & LIVEOBJ2_UNK_80000)) {
				liveObj->flags1 &= ~LIVEOBJ1_REPAIRDRAINING;

				if (liveObj->routeToObject != nullptr) {
					liveObj->routeToObject->flags4 &= ~LIVEOBJ4_UNK_10;
					LegoObject_RequestPowerGridUpdate();
					liveObj->routeToObject = nullptr;
				}
				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Repair, nullptr, true);
			}
			else {
				Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
				Gods98::Container_SetAnimationTime(cont, liveObj->animTime);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_EATING) {
		if (liveObj->animTime > 0.0f) {
			if (LegoObject_Add25EnergyAndSetHealth(liveObj)) {
				liveObj->flags1 &= ~LIVEOBJ1_EATING;

				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Eat, nullptr, true);
				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_GotoEat, nullptr, true);
			}
			else {
				Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
				Gods98::Container_SetAnimationTime(cont, liveObj->animTime);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_GETTINGTOOL) {
		liveObj->flags2 &= ~LIVEOBJ2_GETTINGTOOL;

		if (liveObj->aiTask != nullptr) {
			LegoObject_MiniFigure_EquipTool(liveObj, liveObj->aiTask->toolType);
			AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_GetTool, nullptr, true);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_TRAINING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_TRAINING;

			AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Train, nullptr, true);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_UPGRADING) {
		if (liveObj->type == LegoObject_MiniFigure) {
			if (liveObj->routeToObject == nullptr) {
				liveObj->flags2 &= ~LIVEOBJ2_UPGRADING;
			}
			else {
				const real32 animTime = Creature_GetAnimationTime(liveObj->miniFigure);
				const real32 upgradeTime = StatsObject_GetUpgradeTime(liveObj);
				const real32 funcCoef = StatsObject_GetFunctionCoef(liveObj->routeToObject);
				if (animTime >= (funcCoef * upgradeTime)) {
					AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Upgrade, nullptr, true);
					LegoObject_SetActivity(liveObj, Activity_Stand, 0);
					LegoObject_UpdateActivityChange(liveObj);
					const uint32 newObjLevel = liveObj->objLevel + 1;
					const uint32 maxObjLevels = Stats_GetLevels(liveObj->type, liveObj->id);
					if (newObjLevel < maxObjLevels) {
						StatsObject_SetObjectLevel(liveObj, newObjLevel);
						HelpWindow_RecallDependencies(liveObj->type, liveObj->id, liveObj->objLevel, false);
					}
					liveObj->routeToObject = nullptr;
					liveObj->flags2 &= ~LIVEOBJ2_UPGRADING;
				}
			}
			goto objectupdate_end;
		}

		if (liveObj->animTime > 0.0f) {

			if (liveObj->routeToObject == nullptr ||
				!(StatsObject_GetStatsFlags2(liveObj->routeToObject) & STATS2_UPGRADEBUILDING))
			{
				liveObj->flags2 &= ~LIVEOBJ2_UPGRADING;
			}
			else if (!(liveObj->routeToObject->flags2 & LIVEOBJ2_UPGRADING)) {

				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Upgrade, nullptr, true);

				const uint32 oreCost = Stats_GetUpgradeCostOre(liveObj->type, liveObj->id, liveObj->upgradingType);
				const uint32 oreCount = Level_GetOreCount(false);
				if (oreCount >= oreCost) {
					LegoObject_CompleteVehicleUpgrade(liveObj);
					Level_SubtractOreStored(false, oreCost);
				}
				else {
					const uint32 studsCost = Stats_GetUpgradeCostStuds(liveObj->type, liveObj->id, liveObj->upgradingType);
					const uint32 studsCount = Level_GetOreCount(true);
					if (studsCount >= studsCost) {
						LegoObject_CompleteVehicleUpgrade(liveObj);
						Level_SubtractOreStored(true, studsCost);
					}
				}

				liveObj->routeToObject->interactObject = nullptr;
				liveObj->routeToObject = nullptr;
				liveObj->flags2 &= ~LIVEOBJ2_UPGRADING;
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_PLACING) {
		Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
		Gods98::Container_ForceAnimationUpdate(cont);

		if ((liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) && (liveObj->flags1 & LIVEOBJ1_CARRYING) &&
			liveObj->carriedObjects[0] != nullptr)
		{
			Gods98::Container* carriedCont = LegoObject_GetActivityContainer(liveObj->carriedObjects[0]);
			Vector3F wPos;
			Gods98::Container_GetPosition(carriedCont, nullptr, &wPos);

			SFX_ID placeSFXID;
			switch (liveObj->carriedObjects[0]->type) {
			case LegoObject_PowerCrystal:
				placeSFXID = SFX_PlaceCrystal;
				break;
			case LegoObject_Ore:
				placeSFXID = SFX_PlaceOre;
				break;
			default:
				placeSFXID = SFX_Place;
				break;
			}
			SFX_Random_PlaySound3DOnContainer(nullptr, placeSFXID, false, false, &wPos);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_PLACING;

			LegoObject_DropCarriedObject(liveObj, true);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_GETTINGHIT) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_GETTINGHIT;
		}
		goto objectupdate_end;
	}


	if (liveObj->carryingThisObject != nullptr) {
		goto objectupdate_end;
	}


	if ((liveObj->flags1 & LIVEOBJ1_MOVING) && LegoObject_FUN_0043c6a0(liveObj)) {
		LegoObject_Route_End(liveObj, true);
		LegoObject_Proc_FUN_0043c7f0(liveObj);
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_MOVING) {
		LegoObject_Route_UpdateMovement(liveObj, elapsed);
		LegoObject_UpdateCarryingEnergy(liveObj, elapsed);
		LegoObject_RockMonster_FUN_0043ad70(liveObj);

		if (liveObj->flags2 & LIVEOBJ2_ATTACKINGMONSTER) {
			if (!Weapon_LegoObject_WithinWeaponRange(liveObj, liveObj->aiTask->targetObject) ||
				Weapon_LegoObject_DoCallbacksSearch_FUN_00471b90(liveObj, liveObj->aiTask->targetObject))
			{
				liveObj->flags2 &= ~LIVEOBJ2_ATTACKINGMONSTER;
			}
			else {
				LegoObject_Proc_FUN_0043c780(liveObj);
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_DRILLING) {
			const Point2I target = {
				(sint32)liveObj->targetBlockPos.x,
				(sint32)liveObj->targetBlockPos.y,
			};
			LegoObject_SetActivity(liveObj, Activity_Drill, 0);
			Level_Block_SetBusy(&target, true);
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_LIFTING) {
			LegoObject_SetActivity(liveObj, Activity_Reverse, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags2 & LIVEOBJ2_TRAINING) {
			LegoObject_SetActivity(liveObj, Activity_Train, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags2 & LIVEOBJ2_RECHARGING) {
			LegoObject_SetActivity(liveObj, Activity_Recharge, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_CLEARING) {
			// repeatCount = (liveObj->type != LegoObject_MiniFigure) - 1 & 2;
			const uint32 repeatCount = (liveObj->type == LegoObject_MiniFigure ? 2 : 0);
			LegoObject_SetActivity(liveObj, Activity_Clear, repeatCount);
			goto objectupdate_end;
		}

		if (liveObj->flags2 & LIVEOBJ2_UNK_80000000) {
			LegoObject_SetActivity(liveObj, Activity_FloatOn, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags2 & LIVEOBJ2_UNK_4) {
			if (liveObj->routeToObject != nullptr) {
				LegoObject_ClearDockOccupiedFlag(liveObj, liveObj->routeToObject);
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_REINFORCING) {
			const Point2I target = {
				(sint32)liveObj->targetBlockPos.x,
				(sint32)liveObj->targetBlockPos.y,
			};
			Level_Block_SetBusy(&target, true);
			LegoObject_SetActivity(liveObj, Activity_Reinforce, legoGlobs.ReinforceHits);
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_PLACING) {
			LegoObject_SetActivity(liveObj, Activity_Place, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_ENTERING_WALLHOLE) {
			const Point2I target = {
				(sint32)liveObj->targetBlockPos.x,
				(sint32)liveObj->targetBlockPos.y,
			};
			if (!(StatsObject_GetStatsFlags2(liveObj) & STATS2_USEHOLES) &&
				!Level_Block_IsWall((uint32)target.x, (uint32)target.y))
			{
				liveObj->flags1 &= ~LIVEOBJ1_ENTERING_WALLHOLE;
				liveObj->flags2 |= LIVEOBJ2_DEPARTING;
			}
			else if (!Level_Block_IsReinforced((uint32)target.x, (uint32)target.y)) {
				LegoObject_SetActivity(liveObj, Activity_Enter, 0);
			}
			else {
				LegoObject_SetActivity(liveObj, Activity_EnterRein, 0);
				if (!LegoObject_UpdateActivityChange(liveObj)) {
					LegoObject_SetActivity(liveObj, Activity_Enter, 0);
					LegoObject_UpdateActivityChange(liveObj);
				}

				if (StatsObject_GetStatsFlags2(liveObj) & STATS2_REMOVEREINFORCEMENT) {
					Level_Block_RemoveReinforcement(&target);
				}
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_REPAIRDRAINING) {
			LegoObject_SetActivity(liveObj, Activity_Repair, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags2 & LIVEOBJ2_ATTACKINGPATH) {
			LegoObject_SetActivity(liveObj, Activity_Stamp, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags2 & LIVEOBJ2_UPGRADING) {
			if (liveObj->type == LegoObject_MiniFigure) {
				LegoObject_SetActivity(liveObj, Activity_Train, 100);
			}
			else {
				LegoObject_SetActivity(liveObj, Activity_Upgrade, 0);
				LegoObject_SetActivity(liveObj->routeToObject, Activity_Upgrade, 0);
				LegoObject_UpdateActivityChange(liveObj->routeToObject);
				liveObj->routeToObject->flags2 |= LIVEOBJ2_UPGRADING;
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_EATING) {
			LegoObject_SetActivity(liveObj, Activity_Eat, 0);
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_GATHERINGROCK) {
			if (liveObj->routeToObject == nullptr || liveObj->routeToObject->carryingThisObject != nullptr) {
				liveObj->flags1 &= ~LIVEOBJ1_GATHERINGROCK;
			}
			else {
				LegoObject_SetActivity(liveObj, Activity_Collect, 0);
				if (liveObj->type == LegoObject_RockMonster) {
					switch (liveObj->routeToObject->type) {
					case LegoObject_Boulder:
						LegoObject_SetActivity(liveObj, Activity_Gather, 0);
						break;
					case LegoObject_PowerCrystal:
						LegoObject_SetActivity(liveObj, Activity_Eat, 0);
						break;
					}
				}

				liveObj->carriedObjects[liveObj->numCarriedObjects] = liveObj->routeToObject;
				liveObj->carriedObjects[liveObj->numCarriedObjects]->carryingThisObject = liveObj;
				liveObj->carriedObjects[liveObj->numCarriedObjects]->carryingIndex = liveObj->numCarriedObjects;
				liveObj->routeToObject = nullptr;

				liveObj->flags1 |= LIVEOBJ1_CARRYING;
				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Collect, liveObj->carriedObjects[liveObj->numCarriedObjects], true);
				AITask_DoAnimationWait(liveObj);
				liveObj->numCarriedObjects++;
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_STORING) {
			LegoObject_SetActivity(liveObj, Activity_Deposit, 0);

			LegoObject* routeToObj = liveObj->routeToObject;
			if (routeToObj != nullptr && (StatsObject_GetStatsFlags1(routeToObj) & (STATS1_PROCESSCRYSTAL|STATS1_PROCESSORE))) {

				if (!(routeToObj->flags1 & LIVEOBJ1_STORING) && !(routeToObj->flags3 & LIVEOBJ3_UNK_1000000) &&
					routeToObj->numCarriedObjects != routeToObj->carryNullFrames)
				{
					routeToObj->flags3 |= LIVEOBJ3_UNK_1000000;
				}
				else {
					liveObj->flags1 &= ~LIVEOBJ1_STORING;
					liveObj->flags1 |= LIVEOBJ1_UNK_4000;
				}
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_UNK_800) {
			LegoObject_SetActivity(liveObj, Activity_Throw, 0);
			goto objectupdate_end;
		}

		if ((liveObj->flags1 & LIVEOBJ1_TURNING) || (liveObj->flags1 & LIVEOBJ1_RESTING)) {
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_CARRYING) {
			Point2I blockPos = { 0, 0 }; // dummy init
			LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

			const uint32 rubbleLayers = Level_Block_GetRubbleLayers(&blockPos);
			if ((rubbleLayers >= (LEGO_RUBBLELAYERCOUNT - 1) ||
				 blockValue(legoGlobs.currLevel, blockPos.x, blockPos.y).terrain == Lego_SurfaceType_Lake) &&
				(liveObj->flags3 & LIVEOBJ3_CANROUTERUBBLE))
			{
				LegoObject_SetActivity(liveObj, Activity_CarryRubble, 0);
			}
			else {
				LegoObject_SetActivity(liveObj, Activity_Carry, 0);
			}
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_CANTDO) {
			LegoObject_Route_End(liveObj, false);
			LegoObject_SetActivity(liveObj, Activity_CantDo, 1);
			goto objectupdate_end;
		}

		if (liveObj->flags1 & LIVEOBJ1_MOVING) {
			Point2I blockPos = { 0, 0 }; // dummy init
			LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

			if ((liveObj->flags1 & LIVEOBJ1_RUNNINGAWAY) && liveObj->type == LegoObject_MiniFigure) {
				LegoObject_SetActivity(liveObj, Activity_RunPanic, 0);
			}
			else {
				const uint32 rubbleLayers = Level_Block_GetRubbleLayers(&blockPos);
				if ((rubbleLayers >= (LEGO_RUBBLELAYERCOUNT - 1) ||
					 blockValue(legoGlobs.currLevel, blockPos.x, blockPos.y).terrain == Lego_SurfaceType_Lake) &&
					(liveObj->flags3 & LIVEOBJ3_CANROUTERUBBLE))
				{
					LegoObject_SetActivity(liveObj, Activity_RouteRubble, 0);
				}
				else {
					LegoObject_SetActivity(liveObj, Activity_Route, 0);
				}
			}
			goto objectupdate_end;
		}

		// End of large LIVEOBJ1_MOVING block (not the block just above this).
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_TELEPORTINGDOWN) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_TELEPORTINGDOWN;

			if (liveObj->type == LegoObject_Vehicle) {
				LegoObject_SetActivity(liveObj, Activity_Route, 0);
				LegoObject_UpdateActivityChange(liveObj);
			}
			LegoObject_SetActivity(liveObj, Activity_Stand, 0);
			LegoObject_FUN_00438720(liveObj);
		}
		LegoObject_UpdateWorldStickyPosition(liveObj, elapsed);

		Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
		if (cont != nullptr) {
			// Change up orientation to (0.0f, 0.0f, -1.0f).
			Vector3F dir;
			Gods98::Container_GetOrientation(cont, nullptr, &dir, nullptr);
			Gods98::Container_SetOrientation(cont, nullptr, dir.x, dir.y, dir.z, 0.0f, 0.0f, -1.0f);
		}
		goto objectupdate_end;
	}


	{
		bool32 reinforcingFinished;
		if (LegoObject_Update_Reinforcing(liveObj, elapsed, &reinforcingFinished)) {
			if (reinforcingFinished) {
				AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Reinforce, nullptr, true);
			}
			goto objectupdate_end;
		}
	}


	if (liveObj->flags1 & LIVEOBJ1_EXPANDING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_EXPANDING;

			if (LegoObject_IsRockMonsterCanGather(liveObj)) {
				const Point2I target = {
					(sint32)liveObj->targetBlockPos.x,
					(sint32)liveObj->targetBlockPos.y,
				};
				Message_PostEvent(Message_GenerateRockMonsterComplete, liveObj, 0, nullptr);
				Level_Block_SetBusy(&target, false);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_GETTINGSERVICED) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_GETTINGSERVICED;
			liveObj->flags3 |= LIVEOBJ3_REMOVING;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_UNK_4) {
		liveObj->flags2 &= ~LIVEOBJ2_UNK_4;

		LegoObject* routeToObj = liveObj->routeToObject;

		AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_FindDriver, nullptr, true);

		liveObj->flags2 |= LIVEOBJ2_DRIVING;
		routeToObj->flags4 &= ~LIVEOBJ4_ENTRANCEOCCUPIED;
		if (routeToObj->routeToObject != nullptr) {
			routeToObj->routeToObject->flags4 &= ~LIVEOBJ4_ENTRANCEOCCUPIED;
		}
		routeToObj->flags2 &= ~LIVEOBJ2_FINDINGDRIVER;

		routeToObj->routeToObject = nullptr;
		routeToObj->driveObject = liveObj;
		liveObj->driveObject = routeToObj;
		liveObj->routeToObject = nullptr;

		Interface_ChangeMenu_IfVehicleMounted_IsLiveObject(liveObj->driveObject);
		LegoObject_UpdateWorldStickyPosition(liveObj, elapsed);

		routeToObj->activityName2 = nullptr;
		LegoObject_UpdateActivityChange(routeToObj);

		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_DRIVING) {
		// A driver can't perform any of the remaining actions.
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_ENTERING_WALLHOLE) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_ENTERING_WALLHOLE;
			liveObj->flags2 &= ~LIVEOBJ2_DEPARTING;
			LegoObject_FinishEnteringWallHole(liveObj);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_BUILDPATH) {
		if (liveObj->animTime > 0.0f) {
			Point2I blockPos = { 0, 0 }; // dummy init
			LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

			Construction_Zone* construct = Construction_Zone_FindByHandleOrAtBlock(&blockPos, nullptr);
			if (construct != nullptr && (construct->flags & CONSTRUCTION_FLAG_LAYPATH)) {
				Level_Block_SetPath(&blockPos);
				Construction_Zone_CompletePath(&blockPos);
				Info_Send(Info_PathCompleted, nullptr, nullptr, &blockPos);
				AITask_DoAttackPath(&blockPos);
			}

			liveObj->flags2 &= ~LIVEOBJ2_BUILDPATH;
			AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_BuildPath, nullptr, true);
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_CLEARING) {
		if (liveObj->type == LegoObject_Vehicle) {
			LegoObject_UpdateWorldStickyPosition(liveObj, elapsed);
		}

		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_CLEARING;

			AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Clear, nullptr, true);
			if (liveObj->type == LegoObject_Vehicle) {
				LegoObject_SetActivity(liveObj, Activity_Stand, 0);
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_CANTDO) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_CANTDO;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_GETTINGUP) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_GETTINGUP;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_SPIDERCANWANDER) {
		const uint32 rng = (uint32)Gods98::Maths_Rand();
		if ((rng % 10) > 0) {
			LegoObject_FUN_00444520(liveObj);
		}
		liveObj->flags1 &= ~LIVEOBJ1_SPIDERCANWANDER;

		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_THROWN) {
		if (!(liveObj->throwObject->flags2 & LIVEOBJ2_THROWING) || liveObj->animTime > 0.0f) {
			liveObj->flags2 &= ~LIVEOBJ2_THROWN;

			LegoObject_SetActivity(liveObj, Activity_GetUp, 0);
			LegoObject_UpdateActivityChange(liveObj);
			liveObj->flags2 |= LIVEOBJ2_GETTINGUP;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags2 & LIVEOBJ2_THROWING) {
		if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
			Point2F wPos2D = { 0.0f, 0.0f }; // dummy init
			LegoObject_GetPosition(liveObj, &wPos2D.x, &wPos2D.y);
			const real32 heading = LegoObject_GetHeading(liveObj);
			LegoObject_SetPositionAndHeading(liveObj->throwObject, wPos2D.x, wPos2D.y, heading, true);
		}

		if (liveObj->animTime > 0.0f) {
			Gods98::Container* depositNull = LegoObject_GetDepositNull(liveObj);
			Vector3F wPos;
			Gods98::Container_GetPosition(depositNull, nullptr, &wPos);
			LegoObject_SetPositionAndHeading(liveObj, wPos.x, wPos.y, 0.0f, false);
			liveObj->flags2 &= ~LIVEOBJ2_THROWING;
			LegoObject_AddDamage2(liveObj->throwObject, 10.0f, true, elapsed);
			LegoObject_SetActivity(liveObj, Activity_Stand, 0);
		}
		goto objectupdate_end;
	}


	if (LiveObject_FUN_00433b40(liveObj, elapsed, false)) {
		goto objectupdate_end;
	}


	{
		real32 dummyOutTransSpeed;
		const sint32 fpState = LegoObject_FP_UpdateMovement(liveObj, elapsed, &dummyOutTransSpeed);
		if (fpState != 0) {
			switch (fpState) {
			case 1:
				if (!(liveObj->flags1 & LIVEOBJ1_CARRYING)) {
					LegoObject_SetActivity(liveObj, Activity_Route, 0);
				}
				else {
					LegoObject_SetActivity(liveObj, Activity_Carry, 0);
				}
				break;
			case 2:
				LegoObject_SetActivity(liveObj, Activity_Walk, 0);
				break;
			case 3:
				{
					Point2F drillNullPos = { 0.0f, 0.0f }; // dummy init
					if (LegoObject_GetDrillNullPosition(liveObj, &drillNullPos.x, &drillNullPos.y)) {
						Point2I blockPos = { 0, 0 }; // dummy init
						if (Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), drillNullPos.x, drillNullPos.y, &blockPos.x, &blockPos.y) &&
							Level_Block_IsWall((uint32)blockPos.x, (uint32)blockPos.y))
						{
							const real32 drillTime = StatsObject_GetDrillTimeType(liveObj, Lego_GetBlockTerrain(blockPos.x, blockPos.y));
							if (!(liveObj->flags4 & LIVEOBJ4_UNK_20000)) {
								/// REMOVE: Return value not used.
								//LegoObject_GetActivityContainer(liveObj);
								const SFX_ID drillSFXID = StatsObject_GetDrillSoundType(liveObj, false);
								liveObj->drillSoundHandle = SFX_Random_PlaySoundNormal(drillSFXID, true);
								liveObj->flags4 |= (LIVEOBJ4_UNK_20000|LIVEOBJ4_DRILLSOUNDPLAYING);
							}

							shouldStopDrillSound = false;

							LegoObject_SetActivity(liveObj, Activity_Drill, 0);

							if (Level_Block_Damage((uint32)blockPos.x, (uint32)blockPos.y, drillTime, elapsed) &&
								LiveObject_BlockCheck_FUN_004326a0(liveObj, (uint32)blockPos.x, (uint32)blockPos.y, false, true))
							{
								Level_DestroyWall(Lego_GetLevel(), (uint32)blockPos.x, (uint32)blockPos.y, false);
							}
						}
					}
				}
				break;
			case 4:
				LegoObject_SetActivity(liveObj, Activity_Stand, 0);
				break;
			}

			if (liveObj->type == LegoObject_MiniFigure) {
				/// FIXME: Step SFX is not effected by elapsed time.
				if ((objectGlobs.s_stepCounter_c63c & 3) == 0) {
					SFX_Random_PlaySoundNormal(SFX_Step, false);
				}
				objectGlobs.s_stepCounter_c63c++;
			}
			goto objectupdate_end;
		}
	}


	if (liveObj->flags1 & LIVEOBJ1_GATHERINGROCK) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_GATHERINGROCK;

			if (liveObj->flags1 & LIVEOBJ1_CARRYING) {
				if (liveObj->type == LegoObject_RockMonster) {
					LegoObject* carriedObj = liveObj->carriedObjects[0];
					if (carriedObj->type == LegoObject_Boulder) {
						const Message_Argument msgArg = Message_Argument(LegoObject_Search_FUN_00438e40(liveObj, 0x400));
						Message_PostEvent(Message_GatherRockComplete, liveObj, msgArg, nullptr);
					}
					else {
						liveObj->carriedObjects[0] = nullptr;
						liveObj->numCarriedObjects = 0;
						carriedObj->carryingThisObject = nullptr;
						carriedObj->interactObject = nullptr;

						AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Collect, carriedObj, true);
						LegoObject_PutAwayCarriedObject(liveObj, carriedObj);

						liveObj->flags1 &= ~LIVEOBJ1_CARRYING;
					}
				}
				else {
					switch (liveObj->carriedObjects[liveObj->numCarriedObjects - 1]->type) {
					case LegoObject_PowerCrystal:
						Level_IncCrystalsPickedUp();
						break;
					case LegoObject_Ore:
						Level_IncOrePickedUp();
						Info_Send(Info_OreCollected, nullptr, liveObj, nullptr);
						break;
					}
				}
			}
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_WAITING) {
		if (liveObj->animTime > 0.0f) {
			liveObj->flags1 &= ~LIVEOBJ1_WAITING;
		}
		goto objectupdate_end;
	}


	if (liveObj->flags1 & LIVEOBJ1_TURNING) {
		goto objectupdate_end;
	}


	// Final behaviour:
	if (liveObj->flags1 & LIVEOBJ1_CARRYING) {

		LegoObject_SetActivity(liveObj, Activity_CarryStand, 0);
	}
	else if (liveObj->type == LegoObject_Building && ((liveObj->flags3 & LIVEOBJ3_POWEROFF) ||
													  !(liveObj->flags3 & LIVEOBJ3_HASPOWER)))
	{
		LegoObject_SetActivity(liveObj, Activity_Unpowered, 0);
	}
	else if (liveObj->type == LegoObject_RockMonster && (liveObj->flags3 & LIVEOBJ3_POWEROFF)) {

		LegoObject_SetActivity(liveObj, Activity_Unpowered, 0);
	}
	else if (liveObj->type == LegoObject_Vehicle && (liveObj->flags4 & LIVEOBJ4_UNK_2000)) {

		LegoObject_SetActivity(liveObj, Activity_Open, 0);
	}
	else {
		LegoObject_SetActivity(liveObj, Activity_Stand, 0);
	}
	LegoObject_UpdateWorldStickyPosition(liveObj, elapsed);
	goto objectupdate_end;



objectupdate_end:

	if (shouldStopDrillSound && (liveObj->flags4 & LIVEOBJ4_UNK_20000)) {
		/// REMOVE: Return value not used.
		//LegoObject_GetActivityContainer(liveObj);
		SFX_Sound3D_StopSound(liveObj->drillSoundHandle);
		liveObj->flags4 &= ~LIVEOBJ4_DRILLSOUNDPLAYING;

		/// TODO: What is happening with the drill sound here?
		///       We're stopping it then playing it again without storing the handle...
		const SFX_ID drillSFXID = StatsObject_GetDrillSoundType(liveObj, true);
		SFX_Random_PlaySoundNormal(drillSFXID, false);
		liveObj->flags4 &= ~LIVEOBJ4_UNK_20000;
	}

	LegoObject_UpdateActivityChange(liveObj);

	if (liveObj->driveObject != nullptr && liveObj->type != LegoObject_MiniFigure) {
		LegoObject_UpdateDriverStickyPosition(liveObj);
	}

	if (liveObj->flags1 & LIVEOBJ1_CARRYING) {
		LegoObject_UpdateCarrying(liveObj);
	}

	if (Weapon_LegoObject_IsActiveWithTracker(liveObj)) {
		Weapon_LegoObject_UpdateTracker(liveObj, elapsed);
	}

	LegoObject_UpdateSlipAndScare(liveObj, elapsed);
	LegoObject_Flocks_FUN_0044bef0(liveObj, elapsed);
	LegoObject_UpdateEnergyHealthAndContact(liveObj, elapsed);

	if (liveObj->flags2 & LIVEOBJ2_TRIGGERFRAMECALLBACK) {
		liveObj->flags2 &= ~LIVEOBJ2_TRIGGERFRAMECALLBACK;
		liveObj->flags2 |= LIVEOBJ2_ACTIVITYCHANGING;
	}

	liveObj->flags4 &= ~LIVEOBJ4_DOUBLESELECTREADY;

	LegoObject_UpdateRemoval(liveObj);

	return false;
}

// <LegoRR.exe @0043f160>
void __cdecl LegoRR::LegoObject_ProccessCarriedObjects(LegoObject* liveObj)
{
	const bool canProcess = (StatsObject_GetStatsFlags1(liveObj) & (STATS1_PROCESSCRYSTAL|STATS1_PROCESSORE));

	LegoObject* routeToObj = liveObj->routeToObject;


	for (uint32 i = 0; i < liveObj->numCarriedObjects; i++) {
		LegoObject* carriedObj = liveObj->carriedObjects[i];
		carriedObj->routeToObject = nullptr;

		if (canProcess) {
			carriedObj->carryingThisObject = nullptr;
			carriedObj->interactObject = nullptr;

			switch (carriedObj->type) {
			case LegoObject_Ore:
				if (i == 0) {
					// Produce a stud.
					Gods98::Container* depositNull = Building_GetDepositNull(liveObj->building);
					Vector3F depositNullPos;
					Gods98::Container_GetPosition(depositNull, nullptr, &depositNullPos);

					ObjectModel* studModel = legoGlobs.contOresTable[LegoObject_ID_ProcessedOre];
					LegoObject* studObj = LegoObject_CreateInWorld(studModel,
																   LegoObject_Ore, LegoObject_ID_ProcessedOre, 0,
																   depositNullPos.x, depositNullPos.y, 0.0f);
					Message_PostEvent(Message_GenerateOreComplete, studObj, Message_Argument(0), nullptr);
				}
				else if (i >= (uint32)StatsObject_GetMaxCarry(liveObj)) {
					// Overflow ore into storage.
					Level_IncOreStored(false);
				}
				LegoObject_Remove(carriedObj);
				break;

			/// NOTE: Original logic doesn't check for PowerCrystal type.
			case LegoObject_PowerCrystal:
			default:
				LegoObject_PutAwayCarriedObject(liveObj, carriedObj);
				if (liveObj->type == LegoObject_Building) {
					Building_ChangePowerLevel(liveObj->building, true);
				}
				LegoObject_RequestPowerGridUpdate();
				break;
			}
		}
		else if (routeToObj->numCarriedObjects != routeToObj->carryNullFrames) {
			/// TODO: Should this be a less-than check??

			routeToObj->carriedObjects[routeToObj->numCarriedObjects] = carriedObj;
			routeToObj->carriedObjects[routeToObj->numCarriedObjects]->carryingThisObject = routeToObj;
			routeToObj->carriedObjects[routeToObj->numCarriedObjects]->carryingIndex = routeToObj->numCarriedObjects;
			routeToObj->numCarriedObjects++;

			routeToObj->flags1 |= LIVEOBJ1_CARRYING;

			Message_PostEvent(Message_CrystalToRefineryComplete, nullptr, Message_Argument(0), nullptr);
			if (StatsObject_GetStatsFlags1(routeToObj) & STATS1_PROCESSCRYSTAL) {
				Info_Send(Info_CrystalPower, nullptr, routeToObj, nullptr);
			}
		}
		else {
			// The routeToObj can't carry more objects
			// Shift all carried objects starting at 'i' to the front of the list.
			// Note that all objects being shifted out of the list were already deposited into routeToObj.

			// Readability: j minus 'i' offset, not minus one.
			const uint32 start = i;
			for (uint32 j = start; j < liveObj->numCarriedObjects; j++) {
				liveObj->carriedObjects[j - start] = liveObj->carriedObjects[j];
			}
			liveObj->numCarriedObjects -= start;
			return; // End loop here. All items handled.
		}
	}

	liveObj->numCarriedObjects = 0;
	liveObj->routeToObject = nullptr;
	liveObj->flags1 &= ~LIVEOBJ1_CARRYING;
}

// <LegoRR.exe @0043f3c0>
void __cdecl LegoRR::LegoObject_ClearDockOccupiedFlag(LegoObject* unused_liveObj, LegoObject* liveObj)
{
	liveObj->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
	if (liveObj->routeToObject != nullptr) {
		liveObj->routeToObject->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
	}
}

// <LegoRR.exe @0043f3f0>
void __cdecl LegoRR::LegoObject_TriggerFrameCallback(Gods98::Container* cont, void* data)
{
	LegoObject* liveObj = (LegoObject*)Gods98::Container_GetUserData(cont);
	if (liveObj != nullptr) {
		liveObj->flags2 |= LIVEOBJ2_TRIGGERFRAMECALLBACK;
	}
}

// <LegoRR.exe @0043f410>
bool32 __cdecl LegoRR::LegoObject_QueueTeleport(LegoObject* unused_liveObj, LegoObject_Type objType, LegoObject_ID objID)
{
	/// CONSTANT: Max queued teleport units.
	if (legoGlobs.objTeleportQueue_COUNT < 9) {
		legoGlobs.objTeleportQueueTypes_TABLE[legoGlobs.objTeleportQueue_COUNT] = objType;
		legoGlobs.objTeleportQueueIDs_TABLE[legoGlobs.objTeleportQueue_COUNT] = objID;
		legoGlobs.objTeleportQueue_COUNT++;
		return true;
	}
	return false;
}

// <LegoRR.exe @0043f450>
void __cdecl LegoRR::LegoObject_UpdateTeleporter(LegoObject* liveObj)
{
	if (legoGlobs.objTeleportQueue_COUNT == 0 || !LegoObject_IsActive(liveObj, false) ||
		(liveObj->teleportDownObject != nullptr && (liveObj->teleportDownObject->flags1 & LIVEOBJ1_TELEPORTINGDOWN)) ||
		(liveObj->flags4 & (LIVEOBJ4_DOCKOCCUPIED|LIVEOBJ4_ENTRANCEOCCUPIED)))
	{
		// Either there's nothing to teleport, the object isn't active,
		//  it's busy teleporting, or its entrance is occupied.
		return;
	}

	bool teleporterFound = false;
	uint32 queuedIndex = 0; // dummy init

	for (uint32 i = 0; i < legoGlobs.objTeleportQueue_COUNT; i++) {
		const LegoObject_Type queuedType = legoGlobs.objTeleportQueueTypes_TABLE[i];
		const LegoObject_ID queuedID = legoGlobs.objTeleportQueueIDs_TABLE[i];

		const StatsFlags2 teleRequire = Stats_GetStatsFlags2(queuedType, queuedID);
		const StatsFlags1 teleType = StatsObject_GetStatsFlags1(liveObj);
		if (((teleRequire & STATS2_USESMALLTELEPORTER)   && (teleType & STATS1_SMALLTELEPORTER)) ||
			((teleRequire & STATS2_USEBIGTELEPORTER)     && (teleType & STATS1_BIGTELEPORTER))   ||
			((teleRequire & STATS2_USEWATERTELEPORTER)   && (teleType & STATS1_WATERTELEPORTER)) ||
			((teleRequire & STATS2_USELEGOMANTELEPORTER) && (teleType & STATS1_MANTELEPORTER)))
		{
			const sint32 crystalCost = Stats_GetCostCrystal(queuedType, queuedID, 0);
			const sint32 crystalCount = Level_GetCrystalCount(false);
			if (crystalCost <= crystalCount) {
				LegoObject_Type barracksType;
				LegoObject_ID barracksID;
				Lego_GetObjectByName("Barracks", &barracksType, &barracksID, nullptr);
				if (LegoObject_CanSupportOxygenForType(queuedType, queuedID, barracksType, barracksID)) {
					// Teleported found, break out of list.
					teleporterFound = true;
					queuedIndex = i;
					break;
				}
			}
		}
	}

	if (teleporterFound) {
		const LegoObject_Type queuedType = legoGlobs.objTeleportQueueTypes_TABLE[queuedIndex];
		const LegoObject_ID queuedID = legoGlobs.objTeleportQueueIDs_TABLE[queuedIndex];

		/// TODO: We should sanity check for success with Lego_GetObjectTypeModel.
		ObjectModel* objModel = nullptr;
		Lego_GetObjectTypeModel(queuedType, queuedID, &objModel);
		liveObj->teleportDownObject = LegoObject_CreateInWorld(objModel, queuedType, queuedID, 0, 0.0f, 0.0f, 0.0f);

		Level_SubtractCrystalsStored(Stats_GetCostCrystal(queuedType, queuedID, 0));
		LegoObject_RequestPowerGridUpdate();


		if (StatsObject_GetStatsFlags1(liveObj) & STATS1_WATERTELEPORTER) {
			liveObj->flags4 |= LIVEOBJ4_DOCKOCCUPIED;
			liveObj->teleportDownObject->flags4 |= LIVEOBJ4_DOCKOCCUPIED;
			liveObj->teleportDownObject->routeToObject = liveObj;
		}
		else if (StatsObject_GetStatsFlags1(liveObj->teleportDownObject) & STATS1_CANBEDRIVEN) {
			liveObj->flags4 |= LIVEOBJ4_ENTRANCEOCCUPIED;
			liveObj->teleportDownObject->flags4 |= LIVEOBJ4_ENTRANCEOCCUPIED;
			liveObj->teleportDownObject->routeToObject = liveObj;
		}

			
		Gods98::Container* depositNull = Building_GetDepositNull(liveObj->building);
		Vector3F wPos, dir;
		Gods98::Container_GetPosition(depositNull, nullptr, &wPos);
		Gods98::Container_GetOrientation(depositNull, nullptr, &dir, nullptr);

		if (queuedType == LegoObject_Vehicle && (Stats_GetStatsFlags2(queuedType, queuedID) & STATS2_USESMALLTELEPORTER)) {
			// Because the Teleport Pad also supports Rock Raiders,
			//  extra logic is used here to choose the position of teleported vehicles.
			// This simply gets the center of the destination block.
			Point2I blockPos = { 0, 0 }; // dummy init
			Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), wPos.x, wPos.y, &blockPos.x, &blockPos.y);
				
			Map3D_BlockToWorldPos(Lego_GetMap(), blockPos.x, blockPos.y, &wPos.x, &wPos.y);
		}

		LegoObject_SetPositionAndHeading(liveObj->teleportDownObject, wPos.x, wPos.y, 0.0f, false);
		LegoObject_SetHeadingOrDirection(liveObj->teleportDownObject, 0.0f, &dir);
		ObjectRecall_RecallMiniFigure(liveObj->teleportDownObject);

		liveObj->teleportDownObject->flags1 |= LIVEOBJ1_TELEPORTINGDOWN;

		LegoObject_SetActivity(liveObj->teleportDownObject, Activity_TeleportIn, 1);
		LegoObject_UpdateActivityChange(liveObj->teleportDownObject);

		if (liveObj->teleportDownObject->type == LegoObject_Vehicle) {
			// Wheels are built into the teleport-down animation, unlike the animations when active.
			Vehicle_HideWheels(liveObj->teleportDownObject->vehicle, true);
		}

		// Shift all queued objects up one to replace the object that was just teleported.
		/// REFACTOR: Move count decrement below shift for clarity.
		for (uint32 i = queuedIndex; i < (legoGlobs.objTeleportQueue_COUNT - 1); i++) {
			legoGlobs.objTeleportQueueTypes_TABLE[i] = legoGlobs.objTeleportQueueTypes_TABLE[i + 1];
			legoGlobs.objTeleportQueueIDs_TABLE[i] = legoGlobs.objTeleportQueueIDs_TABLE[i + 1];
		}
		legoGlobs.objTeleportQueue_COUNT--;
	}
}

// <LegoRR.exe @0043f820>
bool32 __cdecl LegoRR::LegoObject_RemoveTeleportDownReference(LegoObject* teleportDownObj)
{
	for (auto obj : objectListSet.EnumerateSkipUpgradeParts()) {
		if (LegoObject_Callback_RemoveTeleportDownReference(obj, teleportDownObj))
			return true;
	}
	return false;
	//return LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_Callback_RemoveTeleportDownReference, teleportDownObj);
}

// <LegoRR.exe @0043f840>
bool32 __cdecl LegoRR::LegoObject_Callback_RemoveTeleportDownReference(LegoObject* liveObj, void* pTeleportDownObj)
{
	LegoObject* teleportDownObj = (LegoObject*)pTeleportDownObj;

	if (liveObj->teleportDownObject == teleportDownObj) {
		liveObj->teleportDownObject = nullptr;
		return true;
	}
	return false;
}

// <LegoRR.exe @0043f870>
void __cdecl LegoRR::LegoObject_TrainMiniFigure_instantunk(LegoObject* liveObj, LegoObject_AbilityFlags trainFlags)
{
	if (liveObj->type == LegoObject_MiniFigure) {
		liveObj->abilityFlags |= trainFlags;
		objectGlobs.NERPs_TrainFlags |= trainFlags;

		/// FIXME: This should really allow training all types at once...
		if (trainFlags & ABILITY_FLAG_PILOT) {
			Info_Send(Info_TrainPilot, nullptr, liveObj, nullptr);
		}
		else if (trainFlags & ABILITY_FLAG_SAILOR) {
			Info_Send(Info_TrainSailor, nullptr, liveObj, nullptr);
		}
		else if (trainFlags & ABILITY_FLAG_DRIVER) {
			Info_Send(Info_TrainDriver, nullptr, liveObj, nullptr);
		}
		else if (trainFlags & ABILITY_FLAG_DYNAMITE) {
			Info_Send(Info_TrainDynamite, nullptr, liveObj, nullptr);
		}
		else if (trainFlags & ABILITY_FLAG_REPAIR) {
			Info_Send(Info_TrainRepair, nullptr, liveObj, nullptr);
		}
		else if (trainFlags & ABILITY_FLAG_SCANNER) {
			Info_Send(Info_TrainScanner, nullptr, liveObj, nullptr);
		}

		liveObj->elapsedTime1 = 0.0f;
	}
}

// <LegoRR.exe @0043f960>
void __cdecl LegoRR::LegoObject_AddDamage2(LegoObject* liveObj, real32 damage, bool32 showVisual, real32 elapsed)
{
	if (LegoObject_IsActive(liveObj, true) && liveObj->carryingThisObject == nullptr) {

		if (liveObj->type == LegoObject_Building && damage != 0.0f && liveObj->health == 100.0f) {
			AITask_DoRepair_Target(liveObj, false);
		}

		if (LegoObject_IsRockMonsterCanGather(liveObj)) {
			// I have no idea what these numbers are trying to say.
			// Note that the first number is 10,010 (with a ten at the end), while the second is a flat 10,000.
			const real32 healthDecayRate = StatsObject_GetHealthDecayRate(liveObj);
			const uint32 decayVal = (uint32)(sint32)(healthDecayRate / STANDARD_FRAMERATE * elapsed * 10010.0f);
			const uint32 decayCap = (uint32)(sint32)(damage * 10000.0f);

			if (decayVal < decayCap) {
				RewardQuota_RockMonsterDamageDealt(damage);
			}
		}

		if (liveObj->type == LegoObject_MiniFigure) {
			RewardQuota_MiniFigureDamageTaken(damage);
		}

		liveObj->health -= damage;
		
		if (damage > 0.0f) {
			if (showVisual) {
				// Mark damage numbers to be shown, but use the delay flag so that numbers
				//  will not be shown until the unit hasn't taken damage for one tick.
				// (We don't want damage numbers getting spammed while walking across lava, etc.)
				liveObj->flags2 |= (LIVEOBJ2_SHOWDAMAGEDELAY|LIVEOBJ2_SHOWDAMAGENUMBERS);
				liveObj->damageNumbers += damage;
				Bubble_ShowHealthBar(liveObj);
			}

			if ((StatsObject_GetStatsFlags2(liveObj) & STATS2_DAMAGECAUSESCALLTOARMS)) {
				Lego_SetCallToArmsOn(true);
			}
		}
	}
}

// <LegoRR.exe @0043fa90>
void __cdecl LegoRR::LegoObject_UpdateEnergyHealthAndContact(LegoObject* liveObj, real32 elapsed)
{
	const Point2I DIRECTIONS[DIRECTION__COUNT] = {
		{  0, -1 },
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
	};

	if (liveObj->type != LegoObject_UpgradePart && liveObj->type != LegoObject_Boulder) {
		// Update object health decay.
		if (liveObj->health > 0.0f) {
			if (!(liveObj->flags3 & LIVEOBJ3_POWEROFF)) {
				const real32 healthDecayRate = StatsObject_GetHealthDecayRate(liveObj);
				LegoObject_AddDamage2(liveObj, (healthDecayRate / STANDARD_FRAMERATE * elapsed), false, elapsed);
			}
		}
		else {
			liveObj->health = 0.0f;
		}

		// Update object energy decay.
		if (liveObj->energy > 0.0f) {
			if (!(liveObj->flags3 & LIVEOBJ3_POWEROFF)) {
				const real32 energyDecayRate = StatsObject_GetEnergyDecayRate(liveObj);
				liveObj->energy -= (energyDecayRate / STANDARD_FRAMERATE * elapsed);
			}
		}
		else {
			liveObj->energy = 0.0f;
		}


		Point2I blockPos = { 0, 0 }; // dummy init
		LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

		// Destroy ore and crystal objects that are sitting on lava.
		if (liveObj->carryingThisObject == nullptr &&
			(liveObj->type == LegoObject_Ore || liveObj->type == LegoObject_PowerCrystal) &&
			blockValue(legoGlobs.currLevel, blockPos.x, blockPos.y).terrain == Lego_SurfaceType_Lava)
		{
			/// FIXME: Stolen ore is treated as crystals.
			Level_AddStolenCrystals(1);
			liveObj->health = -1.0f;
			liveObj->flags3 |= LIVEOBJ3_REMOVING;
		}

		// Apply surface damage to the object.
		real32 damage = 0.0f;
		if (Level_GetObjectDamageFromSurface(liveObj, blockPos.x, blockPos.y, elapsed, &damage)) {
			LegoObject_AddDamage2(liveObj, damage, true, elapsed);
		}

		// See if an electric fence can hit this object.
		if ((StatsObject_GetStatsFlags2(liveObj) & STATS2_CANBEHITBYFENCE) &&
			!(liveObj->flags1 & (LIVEOBJ1_EXPANDING|LIVEOBJ1_CRUMBLING)) &&
			!(liveObj->flags2 & LIVEOBJ2_FROZEN))
		{
			ElectricFence_TrySparkObject(liveObj);
		}

		// See if the object needs to eat food, and try to navigate to a place serving food if needed.
		/// CONSTANT: Unit tries to eat every 5 seconds.
		const real32 eatWaitTime = (STANDARD_FRAMERATE * 5.0f); // 5 seconds.
		// Scale minEnergy by health so that units with low health don't try to eat all the time.
		const real32 scaledMinEnergy = liveObj->health / 100.0f * legoGlobs.MinEnergyForEat;
		if (!(legoGlobs.flags2 & GAME2_NOAUTOEAT) && liveObj->type == LegoObject_MiniFigure &&
			liveObj->energy < scaledMinEnergy && liveObj->eatWaitTimer > eatWaitTime)
		{
			// Is this a timer for how long a unit waits before eating again?
			liveObj->eatWaitTimer = 0.0f;

			if (liveObj->driveObject == nullptr &&
				!AITask_LiveObject_IsCurrentTaskType(liveObj, AITask_Type_GotoEat))
			{
				// Find an object that serves snacks.
				LegoObject* snacksObj = LegoObject_FindSnackServingBuilding(liveObj);
				if (snacksObj != nullptr) {
					// Reuse variable blockPos.
					blockPos = Point2I { 0, 0 }; // dummy init
					Point2I snacksBlockPos = { 0, 0 }; // dummy init
					LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
					LegoObject_GetBlockPos(snacksObj, &snacksBlockPos.x, &snacksBlockPos.y);

					// Find a side of the snacks-serving object that this object can to navigate to.
					for (uint32 d = 0; d < _countof(DIRECTIONS); d++) {
						sint32* bxList = nullptr;
						sint32* byList = nullptr;
						uint32 count; // dummy output
						if (LegoObject_Route_BuildList(liveObj, blockPos.x, blockPos.y,
																snacksBlockPos.x + DIRECTIONS[d].x,
																snacksBlockPos.y + DIRECTIONS[d].y,
																&bxList, &byList, &count, nullptr, nullptr))
						{
							if (bxList != nullptr) Gods98::Mem_Free(bxList);
							if (byList != nullptr) Gods98::Mem_Free(byList);
							AITask_QueueGotoEat_Target(liveObj, snacksObj);
							break;
						}
					}
				}
			}
		}

		// Cap energy at the object's health.
		// Those poor injured Rock Raiders are starving!!!
		if (liveObj->energy >= liveObj->health) {
			liveObj->energy = liveObj->health;
		}

		// This only does something when damage is non-zero.
		LegoObject_MiniFigurePlayHurtSound(liveObj, elapsed, damage);

		if ((liveObj->flags2 & LIVEOBJ2_SHOWDAMAGENUMBERS) && !(liveObj->flags2 & LIVEOBJ2_SHOWDAMAGEDELAY)) {
			// Damage numbers should be shown, and one tick has passed without the unit taking damage.
			DamageText_ShowNumber(liveObj, (uint32)(sint32)liveObj->damageNumbers);
			liveObj->damageNumbers = 0.0f;
			liveObj->flags2 &= ~LIVEOBJ2_SHOWDAMAGENUMBERS;
		}

		// Unset delay flag, if the unit doesn't receive more damage next tick then
		//  we can show the damage numbers.
		liveObj->flags2 &= ~LIVEOBJ2_SHOWDAMAGEDELAY;
	}
}

// <LegoRR.exe @0043fe00>
bool32 __cdecl LegoRR::LegoObject_MiniFigurePlayHurtSound(LegoObject* liveObj, real32 elapsed, real32 damage)
{
	if (liveObj->type == LegoObject_MiniFigure && damage != 0.0f) {
		SFX_ID hurtSFXID;
		if (SFX_GetType("SND_Hurt", &hurtSFXID)) {
			if (liveObj->hurtSoundTimer == 0.0f) {
				// Play a new sound.
				Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
				SFX_Random_PlaySound3DOnContainer(cont, hurtSFXID, false, true, nullptr);
				/// FIXME: hurtSoundTimer adds amount in standard units, but below it adds amount in seconds...
				liveObj->hurtSoundTimer += elapsed;
				liveObj->hurtSoundDuration = SFX_Random_GetSamplePlayTime(hurtSFXID);
			}
			else {
				// Update the sound timer.
				liveObj->hurtSoundTimer += elapsed / STANDARD_FRAMERATE;

				// Reset timer after twice the hurt SFX duration has passed.
				const real32 resetTime = (liveObj->hurtSoundDuration * 2.0f);
				if (liveObj->hurtSoundTimer > resetTime) {
					liveObj->hurtSoundTimer = 0.0f;
					liveObj->hurtSoundDuration = 0.0f;
					return true;
				}
			}
		}
	}
	return true;
}

// <LegoRR.exe @0043fee0>
//bool32 __cdecl LegoRR::LegoObject_FUN_0043fee0(LegoObject* carriedObj);

// <LegoRR.exe @00440080>
//bool32 __cdecl LegoRR::LegoObject_UnkCarryingVehicle_FUN_00440080(LegoObject* liveObj);

// <LegoRR.exe @00440130>
//bool32 __cdecl LegoRR::LegoObject_TryFindLoad_FUN_00440130(LegoObject* liveObj, LegoObject* targetObj);

// <LegoRR.exe @004402b0>
//bool32 __cdecl LegoRR::LegoObject_TryDock_FUN_004402b0(LegoObject* liveObj);

// <LegoRR.exe @004403f0>
//void __cdecl LegoRR::LegoObject_TryDock_AtBlockPos_FUN_004403f0(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @00440470>
//bool32 __cdecl LegoRR::LegoObject_FUN_00440470(LegoObject* liveObj, bool32 param_2);

// <LegoRR.exe @00440690>
//bool32 __cdecl LegoRR::LegoObject_TryFindDriver_FUN_00440690(LegoObject* liveObj, LegoObject* drivableObj);

// <LegoRR.exe @00440a70>
//void __cdecl LegoRR::LegoObject_DoDynamiteExplosionRadiusCallbacks(LegoObject* liveObj, real32 damageRadius, real32 maxDamage, real32 wakeRadius);

// <LegoRR.exe @00440ac0>
//bool32 __cdecl LegoRR::LegoObject_Callback_DynamiteExplosion(LegoObject* liveObj, SearchDynamiteRadius* search);

// <LegoRR.exe @00440b80>
//void __cdecl LegoRR::LegoObject_ScareUnitsWithBigBang(OPTIONAL LegoObject* liveObj, OPTIONAL const Point2F* wPos, real32 radius);

// <LegoRR.exe @00440be0>
//bool32 __cdecl LegoRR::LegoObject_Callback_ScareUnitWithBigBang(LegoObject* liveObj, SearchDynamiteRadius* search);

// <LegoRR.exe @00440ca0>
void __cdecl LegoRR::LegoObject_SetActivity(LegoObject* liveObj, Activity_Type activityType, uint32 repeatCount)
{
	liveObj->activityName1 = objectGlobs.activityName[activityType];
	liveObj->animRepeat = repeatCount;
	liveObj->animTime = 0.0f;
}

// <LegoRR.exe @00440cd0>
//void __cdecl LegoRR::LegoObject_UpdateCarrying(LegoObject* liveObj);

// <LegoRR.exe @00440eb0>
//void __cdecl LegoRR::LegoObject_InitBoulderMesh_FUN_00440eb0(LegoObject* liveObj, Gods98::Container_Texture* contTexture);

// <LegoRR.exe @00440ef0>
bool32 __cdecl LegoRR::LegoObject_Route_BuildListToTarget(LegoObject* liveObj, uint32 bx, uint32 by, uint32 bxTarget, uint32 byTarget, OUT sint32** bxList, OUT sint32** byList, OUT uint32* count)
{
	return LegoObject_Route_BuildList(liveObj, bx, by, bxTarget, byTarget, bxList, byList, count, nullptr, nullptr);
}

// <LegoRR.exe @00440f30>
bool32 __cdecl LegoRR::LegoObject_Route_BuildListWithoutScore(LegoObject* liveObj, uint32 bx, uint32 by, uint32 bxTarget, uint32 byTarget, OUT sint32** bxList, OUT sint32** byList, OUT uint32* count, OPTIONAL LegoObject_RouteChooseBlockCallback callback, void* data)
{
	const Point2I DIRECTIONS[DIRECTION__COUNT] = {
		{  0, -1 },
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
	};

	const uint32 blockWidth = Lego_GetMap()->blockWidth;
	const uint32 blockHeight = Lego_GetMap()->blockHeight;

	const uint32 newCount = (blockHeight * blockWidth);

	// Unsigned comparisons used to skip checking < 0.
	if ((uint32)bx >= blockWidth || (uint32)by >= blockHeight) {
		return false;
	}

	if (callback == nullptr) {
		// bxTarget, byTarget are used as the target block pos if a callback is not supplied,
		//  otherwise the callback determines if the passed block is a target.
		// Unsigned comparisons used to skip checking < 0.
		if ((uint32)bxTarget >= blockWidth || (uint32)byTarget >= blockHeight) {
			return false;
		}
		// Ensure the unit is able to walk on the target block's terrain.
		if (!Lego_GetCrossTerrainType(liveObj, bxTarget, byTarget, bxTarget, byTarget, false)) {
			return false;
		}
	}

	if (objectGlobs.routeBuildListCount != newCount) {
		if (objectGlobs.routeBuildListLengths != nullptr) {
			Gods98::Mem_Free(objectGlobs.routeBuildListLengths);
			objectGlobs.routeBuildListLengths = nullptr;
		}
		/// FIX APPLY: Make sure the other allocated grid depending on the same count is freed.
		///            Otherwise it won't be resized when needed.
		if (objectGlobs.routeBuildListScores != nullptr) {
			Gods98::Mem_Free(objectGlobs.routeBuildListScores);
			objectGlobs.routeBuildListScores = nullptr;
		}

		objectGlobs.routeBuildListCount = newCount;
	}

	if (objectGlobs.routeBuildListLengths == nullptr) {
		objectGlobs.routeBuildListLengths = (uint32*)Gods98::Mem_Alloc(objectGlobs.routeBuildListCount * sizeof(uint32));
	}

	if (objectGlobs.routeBuildListLengths != nullptr) {

		for (uint32 i = 0; i < objectGlobs.routeBuildListCount; i++) {
			objectGlobs.routeBuildListLengths[i] = 0;
		}

		// A set of two lists are used here, one is used to store the current blocks being iterated through,
		//  and the other is used to build the possible blocks to move to.
		//RoutingBlock blockList[2][100];
		/// REFACTOR: Don't bother using RoutingBlock if we're just using the Point2I blockPos field...
		/// SANITY: Increase list size from 100 to something higher.
		Point2I blockList[2][250];
		uint32 countList[2];

		sint32 success = 0; // -1 = failure, 0 = continue, 1 = success.

		uint32 swapIndex = 0;
		blockList[0][0].x = bx;
		blockList[0][0].y = by;
		countList[0] = 1;
		countList[1] = 0;

		uint32 currCount = 1;
		objectGlobs.routeBuildListLengths[blockWidth * by + bx] = currCount;

		while (success == 0) {
			currCount++;

			for (uint32 i = 0; i < countList[swapIndex]; i++) {
				for (uint32 j = 0; j < _countof(DIRECTIONS); j++) {
					const Point2I blockPos = blockList[swapIndex][i];

					const sint32 bxOff = blockPos.x + DIRECTIONS[j].x;
					const sint32 byOff = blockPos.y + DIRECTIONS[j].y;

					const uint32 gridIndex = blockWidth * byOff + bxOff;

					// Unsigned comparisons used to skip checking >= 0.
					if ((uint32)bxOff < blockWidth && (uint32)byOff < blockHeight) {
						// Check CrossTerrain from our current direction-offset to one further in that direction.
						const sint32 bxOff2 = bxOff + DIRECTIONS[j].x;
						const sint32 byOff2 = byOff + DIRECTIONS[j].y;

						/// OPTIMIZE: Move CrossTerrain check to after grid check.
						if (objectGlobs.routeBuildListLengths[gridIndex] == 0 &&
							Lego_GetCrossTerrainType(liveObj, bxOff, byOff, bxOff2, byOff2, false))
						{
							const uint32 nextSwapIndex = (swapIndex == 0 ? 1 : 0); // Swap index.
							blockList[nextSwapIndex][countList[nextSwapIndex]].x = bxOff;
							blockList[nextSwapIndex][countList[nextSwapIndex]].y = byOff;
							countList[nextSwapIndex]++;
							objectGlobs.routeBuildListLengths[gridIndex] = currCount;

							if (callback == nullptr) {
								if (bxOff == bxTarget && byOff == byTarget) {
									success = 1; // Success.
								}
							}
							else {
								const Point2I callbackBlockPos = { bxOff, byOff };
								if (callback(liveObj, &callbackBlockPos, data)) {
									success = 1; // Success.
									bxTarget = bxOff; // This block has been chosen as the target.
									byTarget = byOff;
								}
							}
						}
					}
				}
			}
			countList[swapIndex] = 0;
			swapIndex = (swapIndex == 0 ? 1 : 0); // Swap index.
			if (countList[swapIndex] == 0) {
				success = -1; // Failure.
				// Or just return false here...
				return false;
			}
		}

		if (success == -1) {
			return false;
		}


		sint32* bxCurrList = (sint32*)Gods98::Mem_Alloc(currCount * sizeof(sint32));
		sint32* byCurrList = (sint32*)Gods98::Mem_Alloc(currCount * sizeof(sint32));
		if (bxCurrList != nullptr && byCurrList != nullptr) {
			bxCurrList[0] = bx;
			byCurrList[0] = by;
			bxCurrList[currCount - 1] = bxTarget;
			byCurrList[currCount - 1] = byTarget;

			// Start from target block and navigate our way back to the start.
			sint32 bxCurr = bxTarget;
			sint32 byCurr = byTarget;

			// Direction is preserved after successful matches,
			//  meaning the first attempt is to keep checking in the same direction.
			Direction direction = DIRECTION_UP; // 0

			/// SANITY: Change from not equals 1 to greater than 1.
			for (uint32 i = (currCount - 1); i > 1; i--) {

				for (uint32 j = 0; j < _countof(DIRECTIONS); j++) {
					// Note: We use direction (not j) here for the optimisation.
					const sint32 bxOff = bxCurr + DIRECTIONS[direction].x;
					const sint32 byOff = byCurr + DIRECTIONS[direction].y;

					// Unsigned comparisons used to skip checking >= 0.
					if ((uint32)bxOff < blockWidth && (uint32)byOff < blockHeight) {
						const uint32 gridIndex = blockWidth * byOff + bxOff;
						if (objectGlobs.routeBuildListLengths[gridIndex] == i) {
							bxCurrList[i - 1] = bxOff;
							byCurrList[i - 1] = byOff;
							bxCurr = bxOff;
							byCurr = byOff;
							break;
						}
					}

					// If we failed to match then go to the next direction.
					//direction = (direction + 1) % DIRECTION__COUNT;
					direction = DirectionCW(direction);
				}
			}
			*bxList = bxCurrList;
			*byList = byCurrList;
			*count = currCount;
			return true;
		}
		/// FIX APPLY: Free lists on failure.
		if (bxCurrList != nullptr) Gods98::Mem_Free(bxCurrList);
		if (byCurrList != nullptr) Gods98::Mem_Free(byCurrList);
	}
	/// FIX APPLY: We should return false on failure here, especially because we haven't assigned to the output variables.
	return false;
}

// <LegoRR.exe @004413b0>
bool32 __cdecl LegoRR::LegoObject_Route_BuildList(LegoObject* liveObj, uint32 bx, uint32 by, uint32 bxTarget, uint32 byTarget, OUT sint32** bxList, OUT sint32** byList, OUT uint32* count, OPTIONAL LegoObject_RouteChooseBlockCallback callback, void* data)
{
	const Point2I DIRECTIONS[DIRECTION__COUNT] = {
		{  0, -1 },
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
	};

	// Testing for diagonal pathfinding.
	/*const Point2I DIRECTIONS[8] = {
		{  0, -1 },
		{  1, -1 },
		{  1,  0 },
		{  1,  1 },
		{  0,  1 },
		{ -1,  1 },
		{ -1,  0 },
		{ -1, -1 },
	};*/

	const uint32 blockWidth = Lego_GetMap()->blockWidth;
	const uint32 blockHeight = Lego_GetMap()->blockHeight;

	const uint32 newCount = (blockHeight * blockWidth);

	// Unsigned comparisons used to skip checking < 0.
	if ((uint32)bx >= blockWidth || (uint32)by >= blockHeight) {
		return false;
	}

	if (callback == nullptr) {
		// bxTarget, byTarget are used as the target block pos if a callback is not supplied,
		//  otherwise the callback determines if the passed block is a target.
		// Unsigned comparisons used to skip checking < 0.
		if ((uint32)bxTarget >= blockWidth || (uint32)byTarget >= blockHeight) {
			return false;
		}
		// Ensure the unit is able to walk on the target block's terrain.
		if (!Lego_GetCrossTerrainType(liveObj, bxTarget, byTarget, bxTarget, byTarget, false)) {
			return false;
		}
	}

	if (objectGlobs.routeBuildListCount != newCount) {
		if (objectGlobs.routeBuildListScores != nullptr) {
			Gods98::Mem_Free(objectGlobs.routeBuildListScores);
			objectGlobs.routeBuildListScores = nullptr;
		}
		/// FIX APPLY: Make sure the other allocated grid depending on the same count is freed.
		///            Otherwise it won't be resized when needed.
		if (objectGlobs.routeBuildListLengths != nullptr) {
			Gods98::Mem_Free(objectGlobs.routeBuildListLengths);
			objectGlobs.routeBuildListLengths = nullptr;
		}

		objectGlobs.routeBuildListCount = newCount;
	}

	if (objectGlobs.routeBuildListScores == nullptr) {
		objectGlobs.routeBuildListScores = (real32*)Gods98::Mem_Alloc(objectGlobs.routeBuildListCount * sizeof(real32));
	}

	if (objectGlobs.routeBuildListScores != nullptr) {

		for (uint32 i = 0; i < objectGlobs.routeBuildListCount; i++) {
			objectGlobs.routeBuildListScores[i] = 0.0f;
		}

		// A set of two lists are used here, one is used to store the current blocks being iterated through,
		//  and the other is used to build the possible blocks to move to.
		//RoutingBlock blockList[2][100];
		/// REFACTOR: Don't bother using RoutingBlock if we're just using the Point2I blockPos field...
		/// SANITY: Increase list size from 100 to something higher.
		Point2I blockList[2][250];
		uint32 countList[2];

		sint32 success = 0; // -1 = failure, 0 = continue, 1 = success.

		uint32 swapIndex = 0;
		blockList[0][0].x = bx;
		blockList[0][0].y = by;
		countList[0] = 1;
		countList[1] = 0;

		bool targetSuccess = false;
		uint32 backupCount = 0; // Used if targetSuccess is true.

		uint32 currCount = 1;
		objectGlobs.routeBuildListScores[blockWidth * by + bx] = 1.0f;

		while (success == 0) {
			currCount++;

			for (uint32 i = 0; i < countList[swapIndex]; i++) {
				for (uint32 j = 0; j < _countof(DIRECTIONS); j++) {
					const Point2I blockPos = blockList[swapIndex][i];

					const real32 lastScore = objectGlobs.routeBuildListScores[blockWidth * blockPos.y + blockPos.x];

					const sint32 bxOff = blockPos.x + DIRECTIONS[j].x;
					const sint32 byOff = blockPos.y + DIRECTIONS[j].y;

					const uint32 gridIndex = blockWidth * byOff + bxOff;

					// Unsigned comparisons used to skip checking >= 0.
					if ((uint32)bxOff < blockWidth && (uint32)byOff < blockHeight) {
						// Check CrossTerrain from our current direction-offset to one further in that direction.
						const sint32 bxOff2 = bxOff + DIRECTIONS[j].x;
						const sint32 byOff2 = byOff + DIRECTIONS[j].y;

						real32 scoreModifier = 1.0f;
						/// TODO: Is this intended to be offset2 and not offset1??
						const Point2I blockOff2Pos = { bxOff2, byOff2 };
						/// TODO: Consider scaling score modifiers by path and rubble coefs.
						///       But first consider it's possible Rock Monsters are intended to walk through paths and destroy them.
						if (Level_Block_IsPath(&blockOff2Pos)) {
							scoreModifier = 0.5f; // Prioritize routing across paths.
						}
						/// TODO: Consider avoiding rubble by checking rubble layers.
						///       But first consider if this will hamper Rock Monster movement.
						//else if (Level_Block_GetRubbleLayers(&blockOff2Pos) > 0) {
						//	scoreModifier = 2.0f;
						//}

						const real32 newScore = lastScore + scoreModifier;

						/// OPTIMIZE: Move CrossTerrain check to after grid check.
						if ((objectGlobs.routeBuildListScores[gridIndex] == 0.0f ||
							 objectGlobs.routeBuildListScores[gridIndex] > newScore) &&
							Lego_GetCrossTerrainType(liveObj, bxOff, byOff, bxOff2, byOff2, false))
						{
							const uint32 nextSwapIndex = (swapIndex == 0 ? 1 : 0); // Swap index.
							blockList[nextSwapIndex][countList[nextSwapIndex]].x = bxOff;
							blockList[nextSwapIndex][countList[nextSwapIndex]].y = byOff;
							countList[nextSwapIndex]++;
							objectGlobs.routeBuildListScores[gridIndex] = newScore;

							if (callback == nullptr) {
								if (bxOff == bxTarget && byOff == byTarget) {
									// Why is the logic for success here different than if decided by a callback??
									//success = 1; // Success.
									targetSuccess = true; // Success.
									backupCount = currCount;
								}
							}
							else {
								const Point2I callbackBlockPos = { bxOff, byOff };
								if (callback(liveObj, &callbackBlockPos, data)) {
									success = 1; // Success.
									bxTarget = bxOff; // This block has been chosen as the target.
									byTarget = byOff;
								}
							}
						}
					}
				}
			}
			countList[swapIndex] = 0;
			swapIndex = (swapIndex == 0 ? 1 : 0); // Swap index.
			if (countList[swapIndex] == 0) {
				success = -1; // Failure.
				// Or just return false here...
				if (!targetSuccess) return false;
			}
		}

		if (targetSuccess) {
			success = 1; // Success.
			currCount = backupCount;
		}

		if (success == -1) {
			return false;
		}


		sint32* bxCurrList = (sint32*)Gods98::Mem_Alloc(currCount * sizeof(sint32));
		sint32* byCurrList = (sint32*)Gods98::Mem_Alloc(currCount * sizeof(sint32));
		if (bxCurrList != nullptr && byCurrList != nullptr) {
			bxCurrList[0] = bx;
			byCurrList[0] = by;
			bxCurrList[currCount - 1] = bxTarget;
			byCurrList[currCount - 1] = byTarget;

			// Start from target block and navigate our way back to the start.
			sint32 bxCurr = bxTarget;
			sint32 byCurr = byTarget;

			// Unlike BuildListsWithoutScore, this method doesn't prioritize the last direction.
			// Direction is preserved after successful matches,
			//  meaning the first attempt is to keep checking in the same direction.
			//Direction direction = DIRECTION_UP; // 0

			/// SANITY: Change from not equals 1 to greater than 1.
			for (uint32 i = (currCount - 1); i > 1; i--) {

				/// FIX APPLY: Handle routes that have a arbitrarily large score.
				real32 minScore = 0.0f;// 10000.0f; // Arbitrarily high score.
				sint32 bxMin = 0; // Block position for min score.
				sint32 byMin = 0;

				for (uint32 j = 0; j < _countof(DIRECTIONS); j++) {
					// Note: We use direction (not j) here for the optimisation.
					//const sint32 bxOff = bxCurr + DIRECTIONS[direction].x;
					//const sint32 byOff = byCurr + DIRECTIONS[direction].y;
					const sint32 bxOff = bxCurr + DIRECTIONS[j].x;
					const sint32 byOff = byCurr + DIRECTIONS[j].y;

					// Unsigned comparisons used to skip checking >= 0.
					if ((uint32)bxOff < blockWidth && (uint32)byOff < blockHeight) {
						const uint32 gridIndex = blockWidth * byOff + bxOff;

						const Point2I blockOffPos = { bxOff, byOff };

						const real32 score = objectGlobs.routeBuildListScores[gridIndex];
						// Prioritize lower scores, or equal scores that cross a path.
						/// FIX APPLY: Handle routes that have a arbitrarily large score.
						if ((score > 0.0f && (score < minScore || minScore == 0.0f)) ||
							(score == minScore && Level_Block_IsPath(&blockOffPos)))
						{
							minScore = score;
							bxMin = bxOff;
							byMin = byOff;
						}
					}

					// If we failed to match then go to the next direction.
					//direction = (direction + 1) % DIRECTION__COUNT;
					//direction = DirectionCW(direction);
				}
				bxCurrList[i - 1] = bxMin;
				byCurrList[i - 1] = byMin;
				bxCurr = bxMin;
				byCurr = byMin;
			}

			/// TODO: Consider only writing these when the first block is entered.
			*bxList = bxCurrList;
			*byList = byCurrList;
			*count = currCount;

			// Reverse comparison.
			//if ((bxCurrList[0] != bxCurrList[1] || std::abs(byCurrList[0] - byCurrList[1]) != 1) &&
			//	(byCurrList[0] != byCurrList[1] || std::abs(bxCurrList[0] - bxCurrList[1]) != 1))

			if ((bxCurrList[0] == bxCurrList[1] && std::abs(byCurrList[0] - byCurrList[1]) == 1) ||
				(byCurrList[0] == byCurrList[1] && std::abs(bxCurrList[0] - bxCurrList[1]) == 1))
			{
				// The route starts by moving exactly one block in one direction.
				//*bxList = bxCurrList;
				//*byList = byCurrList;
				//*count = currCount;
				return true;
			}
			else {
				// The route doesn't start by moving exactly one block in one direction.
				// I'm not actually sure if it's possible to hit this condition...
				// Was it intended at one point to support moving in 8 directions?

				// Use the second method to build a route.... I really don't understand this....
				// It must be some sort of existing pathfinding algorithm.
				Gods98::Mem_Free(bxCurrList);
				Gods98::Mem_Free(byCurrList);
				return LegoObject_Route_BuildListWithoutScore(liveObj, bx, by, bxTarget, byTarget, bxList, byList, count, callback, data);
			}
		}
		/// FIX APPLY: Free lists on failure.
		if (bxCurrList != nullptr) Gods98::Mem_Free(bxCurrList);
		if (byCurrList != nullptr) Gods98::Mem_Free(byCurrList);
	}
	/// FIX APPLY: We should return false on failure here, especially because we haven't assigned to the output variables.
	return false;
}

// <LegoRR.exe @004419c0>
bool32 __cdecl LegoRR::LegoObject_Route_Begin(LegoObject* liveObj, uint32 count, const sint32* bxList, const sint32* byList, OPTIONAL const Point2F* finalPosition)
{
	if (!LegoObject_Check_LotsOfFlags1AndFlags2_FUN_0043bdb0(liveObj)) {

		bool failed = false;

		if (liveObj->flags4 & LIVEOBJ4_DOCKOCCUPIED) {
			liveObj->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
			liveObj->routeToObject = nullptr;
		}

		RoutingBlock* routeBlocks = (RoutingBlock*)Gods98::Mem_Alloc(count * sizeof(RoutingBlock));
		if (routeBlocks != nullptr) {
			sint32 lastRouteCmp = -1;

			for (uint32 i = 0; i < count; i++) {
				RoutingBlock* routeBlock = &routeBlocks[i];

				routeBlock->blockPos = Point2I { bxList[i], byList[i] };

				// Only use the optional finalPosition argument at the final target block.
				if (finalPosition != nullptr && i == (count - 1)) {
					// Use the supplied position on the block.
					// Sanity init for failure. For once this is actually handled by Vanilla.
					Point2F blockCenter = { 0.5f, 0.5f };

					Map3D_FUN_0044fb30(Lego_GetMap(), finalPosition, nullptr, &blockCenter);
					routeBlock->blockOffset = blockCenter;
				}
				else if (!(liveObj->flags3 & LIVEOBJ3_CENTERBLOCKIDLE) &&
						 (StatsObject_GetStatsFlags1(liveObj) & STATS1_ROUTEAVOIDANCE))
				{
					// Choose a random position on the block.
					routeBlock->blockOffset = Point2F {
						Gods98::Maths_RandRange(0.3f, 0.7f),
						Gods98::Maths_RandRange(0.3f, 0.7f),
					};
				}
				else {
					// Use the center of the block.
					routeBlock->blockOffset = Point2F { 0.5f, 0.5f };
				}

				routeBlock->flagsByte = ROUTE_FLAG_NONE;
				routeBlock->actionByte = ROUTE_ACTION_NONE;

				if (i < (count - 1)) {
					const sint32 routeCmp = Map3D_CheckRoutingComparison_FUN_00450b60(bxList[i + 1], byList[i + 1], bxList[i], byList[i]);
					if (routeCmp == -1) {
						failed = true;
						break;
					}
					if (i > 0 && routeCmp != lastRouteCmp) {
						const sint32 crossType = Lego_GetCrossTerrainType(liveObj, bxList[i + 1], byList[i + 1], bxList[i - 1], byList[i - 1], false);
						if (crossType == 2) {
							routeBlock->flagsByte |= ROUTE_FLAG_UNK_10;
						}
					}
				}
			}
		}

		/// TODO: Should we be confirming routeBlocks is non-null??
		if (!failed) {
			// Cancel existing route.
			LegoObject_Route_End(liveObj, false);

			liveObj->flags3 &= ~LIVEOBJ3_UNK_400;
			liveObj->flags1 |= LIVEOBJ1_MOVING;
			liveObj->routeBlocks = routeBlocks;
			liveObj->routeBlocksTotal = count;
			liveObj->routeBlocksCurrent = 0;
			liveObj->routeCurveCurrDist = 0.0f;
			liveObj->routeCurveTotalDist = 0.0f;
			return true;
		}

		if (routeBlocks != nullptr) Gods98::Mem_Free(routeBlocks);
	}
	return false;
}

// <LegoRR.exe @00441c00>
void __cdecl LegoRR::LegoObject_Route_End(LegoObject* liveObj, bool32 completed)
{
	if (liveObj->routeBlocks != nullptr) {
		if (!completed) {
			// Loop from current block to total blocks (ignoring 0-current).
			for (uint32 i = liveObj->routeBlocksCurrent; i < liveObj->routeBlocksTotal; i++) {
				const RouteAction action = liveObj->routeBlocks[i].actionByte;

				if (action == ROUTE_ACTION_GATHERROCK) {
					if (liveObj->routeToObject != nullptr) {
						if (liveObj->routeToObject->type == LegoObject_Boulder) {
							LegoObject_DestroyBoulder_AndCreateExplode(liveObj->routeToObject);
						}
						else {
							liveObj->routeToObject->interactObject = nullptr;
						}
						liveObj->routeToObject = nullptr;
					}
					break; // Finish the loop early.
				}
				else if (action == ROUTE_ACTION_STORE) {
					liveObj->routeToObject = nullptr;
					break; // Finish the loop early.
				}
			}

			if (liveObj->type == LegoObject_RockMonster) {
				if ((liveObj->flags1 & LIVEOBJ1_CARRYING) && liveObj->carriedObjects[0]->type == LegoObject_Boulder) {
					LegoObject_DropCarriedObject(liveObj, false);
				}

				AITask_LiveObject_FUN_00403b30(liveObj, AITask_Type_Gather, nullptr);
				AITask_LiveObject_FUN_00403b30(liveObj, AITask_Type_Repair, nullptr);
			}

			if (liveObj->flags1 & LIVEOBJ1_DRILLING) {
				const Point2I targetBlockPos = {
					(sint32)liveObj->targetBlockPos.x,
					(sint32)liveObj->targetBlockPos.y,
				};
				Level_Block_SetBusy(&targetBlockPos, false);
			}

			AITask_Game_UnkLiveObjectHandleDynamite(liveObj);
		}
		else { // If completed.
			if (liveObj->type == LegoObject_MiniFigure) {
				Creature_SetOrientation(liveObj->miniFigure, liveObj->point_298.x, liveObj->point_298.y);
				liveObj->faceDirection.x = liveObj->point_298.x;
				liveObj->faceDirection.y = liveObj->point_298.y;
			}
		}

		Gods98::Mem_Free(liveObj->routeBlocks);
		liveObj->routeBlocks = nullptr;

		AITask_Route_End(liveObj, completed);
	}

	liveObj->routeBlocksTotal = 0;
	liveObj->routeBlocksCurrent = 0;
	liveObj->routeCurveCurrDist = 0.0f;
	liveObj->routeCurveTotalDist = 0.0f;
	liveObj->routeCurveInitialDist = 0.0f;
	
	liveObj->flags1 &= ~(LIVEOBJ1_MOVING|LIVEOBJ1_RUNNINGAWAY);
	liveObj->flags3 &= ~(LIVEOBJ3_UNK_400|LIVEOBJ3_UNK_4000);

	if (liveObj->routeToObject != nullptr) {
		liveObj->routeToObject->flags4 &= ~LIVEOBJ4_UNK_10;
	}

	if (liveObj->flags1 & LIVEOBJ1_LIFTING) {
		liveObj->flags1 &= ~LIVEOBJ1_LIFTING;
		// Invert face direction.
		Gods98::Maths_Vector3DScale(&liveObj->faceDirection, &liveObj->faceDirection, -1.0f);
	}

	LegoObject_SetActivity(liveObj, Activity_Stand, 0);
}

// <LegoRR.exe @00441df0>
//void __cdecl LegoRR::LegoObject_Interrupt(LegoObject* liveObj, bool32 actStand, bool32 dropCarried);

// <LegoRR.exe @00442160>
//void __cdecl LegoRR::LegoObject_DestroyBoulder_AndCreateExplode(LegoObject* liveObj);

// <LegoRR.exe @00442190>
//bool32 __cdecl LegoRR::LegoObject_FireBeamWeaponAtObject(LegoObject* liveObj, LegoObject* targetObj, Weapon_KnownType knownWeapon);

// <LegoRR.exe @00442390>
//void __cdecl LegoRR::LegoObject_CreateWeaponProjectile(LegoObject* liveObj, Weapon_KnownType knownWeapon);

// <LegoRR.exe @004424d0>
void __cdecl LegoRR::LegoObject_StartCrumbling(LegoObject* liveObj)
{
	LegoObject_Interrupt(liveObj, false, true);

	LegoObject_SetActivity(liveObj, Activity_Crumble, 1);
	if (LegoObject_UpdateActivityChange(liveObj)) {
		liveObj->flags1 |= LIVEOBJ1_CRUMBLING;
	}

	liveObj->health = -1.0f;
}

// <LegoRR.exe @00442520>
void __cdecl LegoRR::LegoObject_GetPosition(LegoObject* liveObj, OUT real32* xPos, OUT real32* yPos)
{
	Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
	Vector3F wPos;
	Gods98::Container_GetPosition(cont, nullptr, &wPos);
	*xPos = wPos.x;
	*yPos = wPos.y;
}

// <LegoRR.exe @00442560>
void __cdecl LegoRR::LegoObject_GetFaceDirection(LegoObject* liveObj, OUT Point2F* dir2D)
{
	Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
	Vector3F dir3D;
	Gods98::Container_GetOrientation(cont, nullptr, &dir3D, nullptr);
	*dir2D = dir3D.xy;
	Gods98::Maths_Vector2DNormalize(dir2D);
}

// <LegoRR.exe @004425c0>
void __cdecl LegoRR::LegoObject_SetHeadingOrDirection(LegoObject* liveObj, real32 theta, OPTIONAL const Vector3F* dir)
{
	Vector3F newDir;
	if (dir == nullptr) {
		const Vector3F v = { 0.0f, 1.0f, 0.0f };
		Vector3F axis = { 0.0f, 0.0f, -1.0f };
		Gods98::Maths_Vector3DRotate(&newDir, &v, &axis, theta);
	}
	else {
		newDir = *dir;
	}

	switch (liveObj->type) {
	case LegoObject_Vehicle:
		Vehicle_SetOrientation(liveObj->vehicle, newDir.x, newDir.y, newDir.z);
		break;

	case LegoObject_MiniFigure:
		Creature_SetOrientation(liveObj->miniFigure, newDir.x, newDir.y);
		break;

	case LegoObject_RockMonster:
		Creature_SetOrientation(liveObj->rockMonster, newDir.x, newDir.y);
		break;

	case LegoObject_Building:
		Building_SetOrientation(liveObj->building, newDir.x, newDir.y);
		break;

	default:
		if (liveObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) {
			Gods98::Container_SetOrientation(liveObj->other, nullptr, newDir.x, newDir.y, newDir.z,
											 0.0f, 0.0f, -1.0f);
		}
		break;
	}

	liveObj->faceDirection = newDir;
	liveObj->faceDirectionLength = 0.01f; // Arbitrarily low value. See also: LegoObject_SetPositionAndHeading

	/// FIXME: Should we really be assigning this to tempPosition without giving it a position???
	///        Consider changing to Maths_RayEndPoint? Maybe not though, since tempPosition is only used
	///         as a temporary variable that only ever stores the most recent return from Container_GetPosition.
	// negative faceDirectionLength
	Gods98::Maths_Vector3DScale(&liveObj->tempPosition, &liveObj->faceDirection, -liveObj->faceDirectionLength);
	//Gods98::Maths_RayEndPoint(&liveObj->tempPosition, &liveObj->tempPosition, &liveObj->faceDirection, -liveObj->faceDirectionLength);
}

// <LegoRR.exe @00442740>
real32 __cdecl LegoRR::LegoObject_GetHeading(LegoObject* liveObj)
{
	Point2F dir2D;
	LegoObject_GetFaceDirection(liveObj, &dir2D);

	/// REMOVE: std::sqrt(1.0f) multiplier (which is just one).
	/// REMOVE: Dividing by the modulus is pointless, since LegoObject_GetFaceDirection already normalizes the output.
	real32 theta = Maths_ACos(dir2D.y);// / Gods98::Maths_Vector2DModulus(&dir2D));
	// Original math: Maths_ACos(dir2D.y / (std::sqrt(1.0f) * Gods98::Maths_Vector2DModulus(&dir2D)));

	if (dir2D.x < 0.0f) {
		// Handle ranges between [M_PI, M_PI*2].
		theta = (M_PI * 2.0f) - theta;
	}
	return theta;
}

// <LegoRR.exe @004427b0>
bool32 __cdecl LegoRR::LegoObject_GetBlockPos(LegoObject* liveObj, OUT sint32* bx, OUT sint32* by)
{
	Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
	Vector3F wPos;
	Gods98::Container_GetPosition(cont, nullptr, &wPos);
	/// REFACTOR: We can just use LegoObject_GetPosition here instead.
	//Point2F wPos = { 0.0f, 0.0f }; // dummy init
	//LegoObject_GetPosition(liveObj, &wPos.x, &wPos.y);
	return Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), wPos.x, wPos.y, bx, by);
}

// <LegoRR.exe @00442800>
real32 __cdecl LegoRR::LegoObject_GetWorldZCallback(real32 xPos, real32 yPos, Map3D* map)
{
	return Map3D_GetWorldZ(map, xPos, yPos);
}

// <LegoRR.exe @00442820>
real32 __cdecl LegoRR::LegoObject_GetWorldZCallback_Lake(real32 xPos, real32 yPos, Map3D* map)
{
	real32 zModifier = 0.0;
	Point2I blockPos = { 0, 0 }; // dummy init
	if (Map3D_WorldToBlockPos(map, xPos, yPos, &blockPos.x, &blockPos.y, &zModifier)) {
		if (blockValue(legoGlobs.currLevel, blockPos.x, blockPos.y).terrain == Lego_SurfaceType_Lake) {
			zModifier *= 8.0f;
		}
		else {
			zModifier = 0.0f;
		}
	}
	return (Map3D_GetWorldZ(map, xPos, yPos) + zModifier);
}

// <LegoRR.exe @004428b0>
void __cdecl LegoRR::LegoObject_UpdateRoutingVectors(LegoObject* liveObj, real32 xPos, real32 yPos)
{
	/// REFACTOR: Call LegoObject_GetActivityContainer instead of manually getting
	///            the container for vehicles, minifigures, and rockmonsters.
	///           Using this means we can uncomment building and simple object movement.
	Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);

	Vector3F oldPos;
	Gods98::Container_GetPosition(cont, nullptr, &oldPos);
	if (xPos != oldPos.x || yPos != oldPos.y) {
		/// REFACTOR: For clarity, don't use face direction as an intermediate variable (newPos).
		const Vector3F newPos = { xPos, yPos, 0.0f };
		liveObj->tempPosition = oldPos;

		Gods98::Maths_Vector3DSubtract(&liveObj->faceDirection, &newPos, &oldPos);
		liveObj->faceDirection.z = 0.0f; // We only want a 2D direction.
		liveObj->faceDirectionLength = Gods98::Maths_Vector3DModulus(&liveObj->faceDirection);
		// Normalize face direction.
		Gods98::Maths_Vector3DScale(&liveObj->faceDirection, &liveObj->faceDirection, (1.0f / liveObj->faceDirectionLength));
	}

	switch (liveObj->type) {
	case LegoObject_Vehicle:
		if (!(liveObj->flags1 & LIVEOBJ1_LIFTING)) {
			// Use positive face direction.
			Vehicle_SetOrientation(liveObj->vehicle, liveObj->faceDirection.x, liveObj->faceDirection.y, liveObj->faceDirection.z);
		}
		else {
			// Use negative face direction.
			Vehicle_SetOrientation(liveObj->vehicle, -liveObj->faceDirection.x, -liveObj->faceDirection.y, -liveObj->faceDirection.z);
		}
		Vehicle_SetPosition(liveObj->vehicle, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
		break;

	case LegoObject_MiniFigure:
		if (!(liveObj->flags1 & LIVEOBJ1_LIFTING)) {
			// Use positive face direction.
			Creature_SetOrientation(liveObj->miniFigure, liveObj->faceDirection.x, liveObj->faceDirection.y);
		}
		else {
			// Use negative face direction.
			Creature_SetOrientation(liveObj->miniFigure, -liveObj->faceDirection.x, -liveObj->faceDirection.y);
		}
		// Use lake z callback for minifigures, since they sink a bit when in water.
		Creature_SetPosition(liveObj->miniFigure, xPos, yPos, LegoObject_GetWorldZCallback_Lake, Lego_GetMap());
		break;

	case LegoObject_RockMonster:
		Creature_SetOrientation(liveObj->rockMonster, liveObj->faceDirection.x, liveObj->faceDirection.y);
		Creature_SetPosition(liveObj->rockMonster, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
		break;

	case LegoObject_Building:
		/// OPTIONAL: Not in original function since buildings don't move themselves.
		//Building_SetOrientation(liveObj->building, liveObj->faceDirection.x, liveObj->faceDirection.y);
		//Building_SetPosition(liveObj->building, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
		break;

	default:
		/// OPTIONAL: Not in original function since simple objects don't move themselves.
		//if (liveObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) {
		//	Gods98::Container_SetOrientation(liveObj->other, nullptr,
		//									 liveObj->faceDirection.x, liveObj->faceDirection.y, liveObj->faceDirection.z,
		//									 0.0f, 0.0f, -1.0f);
		//	const real32 zPos = Map3D_GetWorldZ(Lego_GetMap(), xPos, yPos);
		//	Gods98::Container_SetPosition(liveObj->other, nullptr, xPos, yPos, zPos);
		//}
		break;
	}
}

// <LegoRR.exe @00442b60>
void __cdecl LegoRR::LegoObject_SetPositionAndHeading(LegoObject* liveObj, real32 xPos, real32 yPos, real32 theta, bool32 assignHeading)
{
	Vector3F newDir = { 0.0f, 0.0f, 0.0f }; // dummy init
	if (assignHeading) {
		const Vector3F v = { 0.0f, 1.0f, 0.0f };
		Vector3F axis = { 0.0f, 0.0f, -1.0f };
		Gods98::Maths_Vector3DRotate(&newDir, &v, &axis, theta);
	}

	switch (liveObj->type) {
	case LegoObject_Vehicle:
		if (assignHeading) {
			Vehicle_SetOrientation(liveObj->vehicle, newDir.x, newDir.y, newDir.z);
		}
		Vehicle_SetPosition(liveObj->vehicle, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
		break;

	case LegoObject_MiniFigure:
		if (assignHeading) {
			Creature_SetOrientation(liveObj->miniFigure, newDir.x, newDir.y);
		}
		// Use lake z callback for minifigures, since they sink a bit when in water.
		Creature_SetPosition(liveObj->miniFigure, xPos, yPos, LegoObject_GetWorldZCallback_Lake, Lego_GetMap());
		break;

	case LegoObject_RockMonster:
		if (assignHeading) {
			Creature_SetOrientation(liveObj->rockMonster, newDir.x, newDir.y);
		}
		Creature_SetPosition(liveObj->rockMonster, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
		break;

	case LegoObject_Building:
		if (assignHeading) {
			Building_SetOrientation(liveObj->building, newDir.x, newDir.y);
		}
		Building_SetPosition(liveObj->building, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
		break;

	default:
		if (liveObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) {
			if (assignHeading) {
				Gods98::Container_SetOrientation(liveObj->other, nullptr, newDir.x, newDir.y, newDir.z,
												 0.0f, 0.0f, -1.0f);
			}
			const real32 zPos = Map3D_GetWorldZ(Lego_GetMap(), xPos, yPos);
			Gods98::Container_SetPosition(liveObj->other, nullptr, xPos, yPos, zPos);
		}
		break;
	}

	if (assignHeading) {
		liveObj->faceDirection = newDir;
	}
	liveObj->faceDirectionLength = 0.01f; // Arbitrarily low value. See also: LegoObject_SetHeadingOrDirection

	// negative faceDirectionLength
	const Vector3F newPos = { xPos, yPos, 0.0f };
	Gods98::Maths_RayEndPoint(&liveObj->tempPosition, &newPos, &liveObj->faceDirection, -liveObj->faceDirectionLength);
	//Gods98::Maths_Vector3DScale(&liveObj->tempPosition, &liveObj->faceDirection, -liveObj->faceDirectionLength);
	//liveObj->tempPosition.x += xPos;
	//liveObj->tempPosition.y += yPos;
}

// <LegoRR.exe @00442dd0>
//sint32 __cdecl LegoRR::LegoObject_FP_UpdateMovement(LegoObject* liveObj, real32 elapsed, OUT real32* transSpeed);

// <LegoRR.exe @00443240>
void __cdecl LegoRR::LegoObject_UpdateWorldStickyPosition(LegoObject* liveObj, real32 elapsed)
{
	AITask_LiveObject_FUN_00403a70(liveObj);

	switch (liveObj->type) {
	case LegoObject_Vehicle:
		{
			Point2F wPos2D = { 0.0f, 0.0f }; // dummy init
			LegoObject_GetPosition(liveObj, &wPos2D.x, &wPos2D.y);
			Vehicle_SetPosition(liveObj->vehicle, wPos2D.x, wPos2D.y, LegoObject_GetWorldZCallback, Lego_GetMap());
		}
		break;

	case LegoObject_Building:
		{
			Point2F wPos2D = { 0.0f, 0.0f }; // dummy init
			LegoObject_GetPosition(liveObj, &wPos2D.x, &wPos2D.y);
			Building_SetPosition(liveObj->building, wPos2D.x, wPos2D.y, LegoObject_GetWorldZCallback, Lego_GetMap());
		}
		break;

	case LegoObject_RockMonster:
		{
			Point2F wPos2D = { 0.0f, 0.0f }; // dummy init
			LegoObject_GetPosition(liveObj, &wPos2D.x, &wPos2D.y);
			Creature_SetPosition(liveObj->rockMonster, wPos2D.x, wPos2D.y, LegoObject_GetWorldZCallback, Lego_GetMap());
		}
		break;

	case LegoObject_MiniFigure:
		{
			Vector3F wPos;
			Gods98::Container* cont = LegoObject_GetActivityContainer(liveObj);
			Gods98::Container_GetPosition(cont, nullptr, &wPos);
			Vector3F newPos = { wPos.x, wPos.y, 0.0f };

			Point2I blockPos = { 0, 0 }; // dummy init
			LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

			if (Level_Block_IsWall(blockPos.x, blockPos.y) && !Level_Block_IsSeamWall(blockPos.x, blockPos.y)) {
				const bool32 isGap = Level_Block_IsGap(blockPos.x, blockPos.y);
				Point2F dir = { 0.0f, 0.0f }; // dummy init
				Map3D_FUN_0044fe50(Lego_GetMap(), wPos.x, wPos.y, isGap, 0.0f, &dir.x, &dir.y);

				Gods98::Maths_Vector2DSubtract(&dir, &dir, &newPos.xy);

				const real32 timeValue = (elapsed / 2.0f);
				const real32 dist = Gods98::Maths_Vector2DModulus(&dir);
				if (dist > timeValue) {
					Gods98::Maths_Vector2DScale(&dir, &dir, (timeValue / dist));
					Gods98::Maths_Vector2DAdd(&newPos.xy, &newPos.xy, &dir);
				}
			}

			newPos.z = LegoObject_GetWorldZCallback(newPos.x, newPos.y, Lego_GetMap());
			if (wPos.x != newPos.x || wPos.y != newPos.y || wPos.z != newPos.z) {
				// Only update if position has changed. Why only do this for MiniFigures...?
				Creature_SetPosition(liveObj->miniFigure, newPos.x, newPos.y, LegoObject_GetWorldZCallback_Lake, Lego_GetMap());
			}
		}
		break;

	case LegoObject_PowerCrystal:
	case LegoObject_Ore:
	case LegoObject_Barrier:
	case LegoObject_Dynamite:
	case LegoObject_ElectricFence:
	case LegoObject_OohScary:
		// Don't update sticky position if it's already handled by the unit carrying this object.
		if (liveObj->carryingThisObject == nullptr) {
			Point2I blockPos = { 0, 0 }; // dummy init
			LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

			/// FIX APPLY: Don't tumble resources from landslide rockfalls, only dug wall rockfalls.
			/// FIX APPLY: Only tumble Crystals and Ore.
			if ((liveObj->type == LegoObject_PowerCrystal || liveObj->type == LegoObject_Ore) &&
			//if (liveObj->type != LegoObject_Barrier &&
				Level_Block_IsRockFallFX(blockPos.x, blockPos.y) &&
				!(blockValue(Lego_GetLevel(), blockPos.x, blockPos.y).flags1 & BLOCK1_LANDSLIDING) &&
				liveObj->cameraNull == nullptr && Lego_IsResourceTumbleOn())
			{
				liveObj->cameraNull = Effect_GetTumbleNull_RockFall(blockPos.x, blockPos.y);
				// Store blockPos for duration of rockfall tumbling.
				liveObj->targetBlockPos.x = (real32)(uint32)blockPos.x;
				liveObj->targetBlockPos.y = (real32)(uint32)blockPos.y;
				liveObj->flags3 &= ~LIVEOBJ3_ALLOWCULLING_UNK;
			}

			const Point2I target = {
				(sint32)liveObj->targetBlockPos.x,
				(sint32)liveObj->targetBlockPos.y,
			};

			/// FIX APPLY: Don't tumble resources from landslide rockfalls, only dug wall rockfalls.
			if (liveObj->cameraNull != nullptr && Level_Block_IsRockFallFX(target.x, target.y) &&
				!(blockValue(Lego_GetLevel(), target.x, target.y).flags1 & BLOCK1_LANDSLIDING))
			{
				// Tumble null positioning.
				Vector3F wPos;
				Gods98::Container_GetPosition(liveObj->cameraNull, nullptr, &wPos);
				real32 zPos = Map3D_GetWorldZ(Lego_GetMap(), wPos.x, wPos.y);
				/// TODO: What is this constant?
				zPos -= 1.2285f;
				if (wPos.z > zPos) wPos.z = zPos;

				/// FIX APPLY: Don't tumble a resource over an inaccessible block.
				// Unfortunately we can't prevent resources from landing on walls because it visually breaks the animation.
				Point2I newBlockPos = { 0, 0 }; // dummy init
				Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), wPos.x, wPos.y, &newBlockPos.x, &newBlockPos.y);
				const Lego_SurfaceType terrain = (Lego_SurfaceType)blockValue(Lego_GetLevel(), newBlockPos.x, newBlockPos.y).terrain;
				if (terrain != Lego_SurfaceType_Lava &&
					terrain != Lego_SurfaceType_Lake &&
					terrain != Lego_SurfaceType_Water)
					//!Level_Block_IsWall(newBlockPos.x, newBlockPos.y))
				{
					Gods98::Container_SetPosition(liveObj->other, nullptr, wPos.x, wPos.y, wPos.z);
				}

				Gods98::Container_SetOrientation(liveObj->other, liveObj->cameraNull, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
			}
			else {
				// Standard positioning.
				liveObj->cameraNull = nullptr;

				Vector3F wPos;
				Gods98::Container_GetPosition(liveObj->other, nullptr, &wPos);
				real32 zPos = Map3D_GetWorldZ(Lego_GetMap(), wPos.x, wPos.y);
				/// TODO: What is this constant?
				zPos -= 1.2285f;
				if (wPos.z < (zPos - 0.5f)) {
					wPos.z += elapsed;
					if (wPos.z > zPos) wPos.z = zPos;
					Gods98::Container_SetPosition(liveObj->other, nullptr, wPos.x, wPos.y, wPos.z);
				}
				if (wPos.z > (zPos + 0.5f)) {
					wPos.z -= elapsed;
					if (wPos.z < zPos) wPos.z = zPos;
					Gods98::Container_SetPosition(liveObj->other, nullptr, wPos.x, wPos.y, wPos.z);
				}

				Vector3F dir, up;
				Gods98::Container_GetOrientation(liveObj->other, nullptr, &dir, &up);
				if (dir.z > 0.0f) {
					dir.z -= (elapsed / 20.0f);
					if (dir.z < 0.0f) dir.z = 0.0f;
					Gods98::Container_SetOrientation(liveObj->other, nullptr, dir.x, dir.y, dir.z, 0.0f, 0.0f, -1.0f);
				}
				else if (dir.z < 0.0f) {
					dir.z += (elapsed / 20.0f);
					if (dir.z > 0.0f) dir.z = 0.0f;
					Gods98::Container_SetOrientation(liveObj->other, nullptr, dir.x, dir.y, dir.z, 0.0f, 0.0f, -1.0f);
				}
			}
		}
		break;
	}
}

// <LegoRR.exe @004437d0>
//void __cdecl LegoRR::LegoObject_UpdateDriverStickyPosition(LegoObject* drivenVehicleObj);

// <LegoRR.exe @00443930>
//bool32 __cdecl LegoRR::LegoObject_TryWaiting(LegoObject* liveObj);

// <LegoRR.exe @004439b0>
//bool32 __cdecl LegoRR::LegoObject_IsRockMonsterCanGather(LegoObject* liveObj);

// <LegoRR.exe @004439d0>
//bool32 __cdecl LegoRR::LegoObject_FUN_004439d0(LegoObject* liveObj, const Point2I* blockPos, OUT Point2I* out_blockPos, undefined4 unused);

// <LegoRR.exe @00443ab0>
//void __cdecl LegoRR::LegoObject_RockMonster_DoWakeUp(LegoObject* liveObj);

// <LegoRR.exe @00443b00>
//bool32 __cdecl LegoRR::LegoObject_CheckBlock_FUN_00443b00(LegoObject* liveObj, const Point2I* blockPos, bool32* pAllowWall);

// <LegoRR.exe @00443b70>
//void __cdecl LegoRR::LegoObject_Route_UpdateMovement(LegoObject* liveObj, real32 elapsed);

// <LegoRR.exe @00444360>
//void __cdecl LegoRR::LegoObject_TryDock_AtObject2FC(LegoObject* liveObj);

// <LegoRR.exe @004443b0>
//void __cdecl LegoRR::LegoObject_UpdateCarryingEnergy(LegoObject* liveObj, real32 elapsed);

// <LegoRR.exe @00444520>
//bool32 __cdecl LegoRR::LegoObject_FUN_00444520(LegoObject* liveObj);

// <LegoRR.exe @00444720>
void __cdecl LegoRR::LegoObject_TryRunAway(LegoObject* liveObj, const Point2F* dir)
{
	Point2I blockPos = { 0, 0 }; // dummy init
	LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

	if (!(liveObj->flags1 & LIVEOBJ1_RUNNINGAWAY) && LegoObject_IsActive(liveObj, true) &&
		!Lego_IsFPObject(liveObj) && liveObj->driveObject == nullptr &&
		!Level_Block_IsWall((uint32)blockPos.x, (uint32)blockPos.y))
	{
		LegoObject_Interrupt(liveObj, true, true);

		Point2F wPos = { 0.0f, 0.0f }; // dummy init
		LegoObject_GetPosition(liveObj, &wPos.x, &wPos.y);

		Point2F awayPos;
		Gods98::Maths_Vector2DScale(&awayPos, dir, legoGlobs.MiniFigureRunAway);
		Gods98::Maths_Vector2DAdd(&awayPos, &awayPos, &wPos);

		Point2I awayBlockPos = { 0, 0 }; // dummy init
		if (Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), awayPos.x, awayPos.y, &awayBlockPos.x, &awayBlockPos.y)) {

			Point2I tempBlock = blockPos;
			bool isLocalList = false;
			sint32* bxList = nullptr;
			sint32* byList = nullptr;
			uint32 count = 0;

			if (awayBlockPos.x == blockPos.x && awayBlockPos.y == blockPos.y) {
				isLocalList = true;
				bxList = &tempBlock.x;
				byList = &tempBlock.y;
				byList = &blockPos.y;
				count = 1;
			}
			else if (!LegoObject_Route_BuildList(liveObj,
														  (uint32)blockPos.x, (uint32)blockPos.y,
														  (uint32)awayBlockPos.x, (uint32)awayBlockPos.y,
														  &bxList, &byList, &count, nullptr, nullptr))
			{
				return;
			}

			if (LegoObject_Route_Begin(liveObj, count, bxList, byList, &awayPos)) {
				liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].flagsByte |= ROUTE_FLAG_RUNAWAY;
			}

			/// FIX APPLY: Don't free memory on the stack!!!
			///            This was causing fatal memory corruption in debug builds. (#66)
			if (!isLocalList) {
				if (bxList != nullptr) Gods98::Mem_Free(bxList);
				if (byList != nullptr) Gods98::Mem_Free(byList);
			}

			liveObj->flags1 |= LIVEOBJ1_RUNNINGAWAY;
			Bubble_ShowBubble(liveObj);
		}
	}
}

// <LegoRR.exe @004448e0>
//void __cdecl LegoRR::LegoObject_DoSlip(LegoObject* liveObj);

// <LegoRR.exe @00444940>
//bool32 __cdecl LegoRR::LegoObject_RoutingUnk_FUN_00444940(LegoObject* liveObj, bool32 useRoutingPos, bool32 flags3_8, bool32 notFlags1_10000);

// <LegoRR.exe @00445270>
//void __cdecl LegoRR::LegoObject_FaceTowardsCamera(LegoObject* liveObj, const Point2F* newFaceDir);

// <LegoRR.exe @004454a0>
//void __cdecl LegoRR::LegoObject_Route_CurveSolid_FUN_004454a0(LegoObject* liveObj);

// <LegoRR.exe @00445600>
//bool32 __cdecl LegoRR::LegoObject_Callback_CurveSolidCollRadius_FUN_00445600(LegoObject* liveObj1, void* pOtherObj);

// <LegoRR.exe @00445860>
//bool32 __cdecl LegoRR::LegoObject_Route_SolidBlockCheck_FUN_00445860(LegoObject* liveObj);

// <LegoRR.exe @004459a0>
void __cdecl LegoRR::LegoObject_UpdateSlipAndScare(LegoObject* liveObj, real32 elapsed)
{
	if (liveObj->type == LegoObject_RockMonster) {
		// Monster interactions with MiniFigures and Vehicles (and units that can scare scorpions).
		for (auto obj : objectListSet.EnumerateSkipUpgradeParts()) {
			if (LegoObject_Callback_SlipAndScare(obj, liveObj))
				break; // Unit was waken up.
		}
	}

	// LIVEOBJ3_UNK_10000 seems to be a multi-use flag. For these object types it means ticking down.
	if ((liveObj->type == LegoObject_Dynamite || liveObj->type == LegoObject_OohScary) &&
		(liveObj->flags3 & LIVEOBJ3_UNK_10000))
	{
		// Units have 3 seconds to run for their lives.
		// Any fuse longer than this won't startle them until then.
		const real32 animTime = Gods98::Container_GetAnimationTime(liveObj->other);
		const uint32 animFrames = Gods98::Container_GetAnimationFrames(liveObj->other);
		if (animTime > ((real32)animFrames - (STANDARD_FRAMERATE * 3.0f))) {

			for (auto obj : objectListSet.EnumerateSkipUpgradeParts()) {
				// Always returns false, enumerate through all objects.
				LegoObject_Callback_ScareTrainedMiniFiguresAwayFromTickingDynamite(obj, liveObj);
			}
		}
	}
}

// <LegoRR.exe @00445a30>
bool32 __cdecl LegoRR::LegoObject_Callback_ScareTrainedMiniFiguresAwayFromTickingDynamite(LegoObject* liveObj, void* pOtherObj)
{
	// Dynamite or a Sonic Blaster object.
	LegoObject* explosiveObj = (LegoObject*)pOtherObj;

	/// FIXME: Add option for non-explosives expert units to understand the dangers of dynamite.
	///        Note that this also affects Sonic Blasters, so explosives experts shouldn't even apply for that(?)
	if (liveObj->type == LegoObject_MiniFigure && (liveObj->abilityFlags & ABILITY_FLAG_DYNAMITE)) {
		Point2F thisPos = { 0.0f, 0.0f }; // dummy init
		Point2F otherPos = { 0.0f, 0.0f }; // dummy init
		LegoObject_GetPosition(liveObj, &thisPos.x, &thisPos.y);
		LegoObject_GetPosition(explosiveObj, &otherPos.x, &otherPos.y);

		Point2F dir2D;
		Gods98::Maths_Vector2DSubtract(&dir2D, &thisPos, &otherPos);
		const real32 dist = Gods98::Maths_Vector2DModulus(&dir2D);
		if (dist < legoGlobs.DynamiteDamageRadius) {
			// Normalize dir2D.
			Gods98::Maths_Vector2DScale(&dir2D, &dir2D, (1.0f / dist));
			LegoObject_TryRunAway(liveObj, &dir2D);
			return false;
		}
	}
	return false;
}

// <LegoRR.exe @00445af0>
bool32 __cdecl LegoRR::LegoObject_Callback_SlipAndScare(LegoObject* liveObj, void* pOtherObj)
{
	LegoObject* monsterObj = (LegoObject*)pOtherObj;

	/// SANITY: Don't apply actions on self. Only matters for can-scare-scorpion units,
	///          all other units must have different object types.
	if (liveObj == monsterObj)
		return false;

	Point2F thisPos = { 0.0f, 0.0f }; // dummy init
	Point2F otherPos = { 0.0f, 0.0f }; // dummy init
	LegoObject_GetPosition(monsterObj, &otherPos.x, &otherPos.y);
	LegoObject_GetPosition(liveObj, &thisPos.x, &thisPos.y);

	Point2F dir2D;
	Gods98::Maths_Vector2DSubtract(&dir2D, &thisPos, &otherPos);
	const real32 dist = Gods98::Maths_Vector2DModulus(&dir2D);
	/// FIX APPLY: Normalize dir2D now, so that multiple functions depending
	///             on it won't normalize it twice.
	Gods98::Maths_Vector2DScale(&dir2D, &dir2D, (1.0f / dist));
	const real32 alertRadius = StatsObject_GetAlertRadius(monsterObj);

	if (!(monsterObj->flags3 & LIVEOBJ3_POWEROFF)) {
		if (!(liveObj->flags2 & LIVEOBJ2_THROWN) && !(liveObj->flags2 & LIVEOBJ2_GETTINGUP) &&
			(!(legoGlobs.flags2 & GAME2_CALLTOARMS) || !(liveObj->flags4 & LIVEOBJ4_UNK_4)) &&
			liveObj->driveObject == nullptr)
		{
			if (liveObj->type == LegoObject_MiniFigure) {
				if (StatsObject_GetStatsFlags1(monsterObj) & STATS1_GRABMINIFIGURE) {
					Gods98::Container* cont = LegoObject_GetActivityContainer(monsterObj);
					Vector3F dir;
					Gods98::Container_GetOrientation(cont, nullptr, &dir, nullptr);

					Gods98::Maths_Vector2DNormalize(&dir.xy);
					// This is Maths_RayEndPoint(&newPos, &otherPos, &dir.xy, 15.0f) but 2D.
					const Point2F newPos = {
						otherPos.x + (dir.xy.x * 15.0f),
						otherPos.y + (dir.xy.y * 15.0f),
					};

					Point2I blockPos = { 0, 0 }; // dummy init
					Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), newPos.x, newPos.y, &blockPos.x, &blockPos.y);
					if (!Level_Block_IsWall((uint32)blockPos.x, (uint32)blockPos.y)) {
						if (!Level_Block_IsSolidBuilding((uint32)blockPos.x, (uint32)blockPos.y, true)) {
							const real32 newDist = Gods98::Maths_Vector2DDistance(&thisPos, &newPos);
							const real32 collRadius = StatsObject_GetCollRadius(liveObj);
							if (newDist < collRadius) {
								LegoObject_DoThrowLegoman(monsterObj, liveObj);
								// Once thrown, this object can't do anything else.
								return false;
							}
						}
					}
				}

				if (StatsObject_GetStatsFlags1(monsterObj) & STATS1_CAUSESLIP) {
					const real32 otherCollRadius = StatsObject_GetCollRadius(monsterObj);
					const real32 collRadius = StatsObject_GetCollRadius(liveObj);
					if (dist < (collRadius + otherCollRadius)) {
						LegoObject_DoSlip(liveObj);
						// Kamikaze successful. Destroy the object causing the slip.
						monsterObj->health = -1.0f;
						monsterObj->flags3 |= LIVEOBJ3_REMOVING;
					}
				}

				if ((StatsObject_GetStatsFlags1(monsterObj) & STATS1_CANSCARE) && alertRadius != 0.0f) {
					const real32 collRadius = StatsObject_GetCollRadius(liveObj);
					if (dist < (collRadius + alertRadius)) {
						/// REFACTOR: Normalize done ahead of time.
						LegoObject_TryRunAway(liveObj, &dir2D);
					}
				}
			}

			if (liveObj->type == LegoObject_MiniFigure || liveObj->type == LegoObject_Vehicle) {
				/// FIXME: Is this flag check supposed to be for monsterObj???
				if (!(liveObj->flags1 & LIVEOBJ1_GATHERINGROCK)) {
					const real32 painThreshold = StatsObject_GetPainThreshold(monsterObj);
					if (monsterObj->health > painThreshold) {
						const real32 stampRadius = StatsObject_GetStampRadius(monsterObj);
						if (dist < stampRadius) {
							LegoObject_FUN_0043acb0(monsterObj, liveObj);
						}
					}
				}

				if ((StatsObject_GetStatsFlags1(monsterObj) & STATS1_SCAREDBYPLAYER) &&
					!(monsterObj->flags1 & LIVEOBJ1_SCAREDBYPLAYER))
				{
					const real32 collRadius = StatsObject_GetCollRadius(liveObj);
					if (dist < (collRadius + alertRadius)) {
						if (LegoObject_FUN_00444520(monsterObj)) {
							monsterObj->flags1 |= LIVEOBJ1_SCAREDBYPLAYER;
						}
					}
				}
			}

			if (liveObj->type == LegoObject_MiniFigure) {
				if (StatsObject_GetStatsFlags1(monsterObj) & STATS1_FLOCKS) {
					const Point2F flocksPos = {
						monsterObj->flocks->floatx_18,
						monsterObj->flocks->floaty_20,
					};
					const real32 flocksDist = Gods98::Maths_Vector2DDistance(&thisPos, &flocksPos);
					const real32 collRadius = StatsObject_GetCollRadius(liveObj);
					if (flocksDist < (collRadius + alertRadius)) {

						const uint32 weaponID = Weapon_GetWeaponIDByName("BatAttack");
						const real32 damage = Weapon_GetDamageForObject(weaponID, liveObj);
						LegoObject_AddDamage2(liveObj, damage, true, 1.0f);

						monsterObj->flags2 |= LIVEOBJ2_FLOCKS_BATATTACKING;

						if (!(monsterObj->flocks->flags & FLOCKS_FLAG_UNK_1)) {
							LegoObject* unitList[1] = { monsterObj };
							AITask_PushFollowObject_Group(unitList, _countof(unitList), liveObj);
							monsterObj->flocks->flags |= FLOCKS_FLAG_UNK_1;
						}
					}
				}
			}

			if ((StatsObject_GetStatsFlags1(monsterObj) & STATS1_CANSCARESCORPION) && alertRadius != 0.0f) {
				// Check if this is the Scorpion object type/ID.
				LegoObject_Type objType;
				LegoObject_ID objID;
				if (Lego_GetObjectByName("Scorpion", &objType, &objID, nullptr) &&
					objType == liveObj->type && objID == liveObj->id)
				{
					const real32 collRadius = StatsObject_GetCollRadius(liveObj);
					if (dist < (collRadius + alertRadius)) {
						/// REFACTOR: Normalize done ahead of time.
						LegoObject_TryRunAway(liveObj, &dir2D);
					}
				}
			}
		}
	}
	else if (liveObj->type == LegoObject_MiniFigure || liveObj->type == LegoObject_Vehicle) {
		// See if we can wake the other unit.
		const real32 wakeRadius = StatsObject_GetWakeRadius(monsterObj);
		if (dist < wakeRadius) {
			LegoObject_RockMonster_DoWakeUp(monsterObj);
			// The other object is being waken up. It can't perform any other actions now, so exit the loop.
			return true;
		}
	}
	return false;
}

// <LegoRR.exe @00446030>
//LegoRR::LegoObject* __cdecl LegoRR::LegoObject_DoCollisionCallbacks_FUN_00446030(LegoObject* liveObj, const Point2F* param_2, real32 param_3, bool32 param_4);

// <LegoRR.exe @004460b0>
//bool32 __cdecl LegoRR::LegoObject_CallbackCollisionRadius_FUN_004460b0(LegoObject* liveObj, void* search);

// <LegoRR.exe @004463b0>
//bool32 __cdecl LegoRR::LegoObject_CallbackCollisionBox_FUN_004463b0(LegoObject* liveObj, void* search);

// <LegoRR.exe @004468d0>
//void __cdecl LegoRR::LegoObject_CalculateSpeeds(LegoObject* liveObj, real32 elapsed, OUT real32* routeSpeed, OUT real32* transSpeed);

// <LegoRR.exe @00446b80>
//bool32 __cdecl LegoRR::LegoObject_RoutingPtr_Realloc_FUN_00446b80(LegoObject* liveObj, uint32 bx, uint32 by);

// <LegoRR.exe @00446c80>
//bool32 __cdecl LegoRR::LegoObject_BlockRoute_FUN_00446c80(LegoObject* liveObj, uint32 bx, uint32 by, bool32 useWideRange, OPTIONAL OUT Direction* direction, bool32 countIs8);

// <LegoRR.exe @00447100>
//bool32 __cdecl LegoRR::LegoObject_RouteToDig_FUN_00447100(LegoObject* liveObj, uint32 bx, uint32 by, bool32 tunnelDig);

// <LegoRR.exe @00447390>
//bool32 __cdecl LegoRR::LegoObject_PTL_GatherRock(LegoObject* liveObj);

// <LegoRR.exe @00447470>
//bool32 __cdecl LegoRR::LegoObject_RoutingNoCarry_FUN_00447470(LegoObject* liveObj, uint32 bx, uint32 by, LegoObject* boulderObj);

// <LegoRR.exe @004474d0>
//bool32 __cdecl LegoRR::LegoObject_PTL_AttackBuilding(LegoObject* liveObj1, LegoObject* targetObj);

// <LegoRR.exe @00447670>
//sint32 __cdecl LegoRR::LegoObject_FUN_00447670(LegoObject* liveObj, sint32 bx, sint32 by, LegoObject* liveObj2);

// <LegoRR.exe @004477b0>
void __cdecl LegoRR::LegoObject_BumpDamageRouteObject(LegoObject* liveObj)
{
	LegoObject* routeToObj = liveObj->routeToObject;
	liveObj->routeToObject = nullptr;

	if (routeToObj != nullptr) {
		Point2F routeToPos = { 0.0f, 0.0f }; // dummy init
		LegoObject_GetPosition(routeToObj, &routeToPos.x, &routeToPos.y);
		Point2F thisPos = { 0.0f, 0.0f }; // dummy init
		LegoObject_GetPosition(liveObj, &thisPos.x, &thisPos.y);

		Point2F dir;
		Gods98::Maths_Vector2DSubtract(&dir, &routeToPos, &thisPos);

		const real32 routeToCollRadius = StatsObject_GetCollRadius(routeToObj);
		const real32 collRadius = StatsObject_GetCollRadius(liveObj);
		if (Gods98::Maths_Vector2DModulus(&dir) < ((collRadius * 2.0f) + routeToCollRadius)) {
			LegoObject_Hit(routeToObj, &dir, true);
			// Deal 10 damage on collision.
			LegoObject_AddDamage2(routeToObj, 10.0f, true, 1.0f);
		}
	}
}

// <LegoRR.exe @00447880>
//sint32 __cdecl LegoRR::LegoObject_FUN_00447880(LegoObject* liveObj);

// <LegoRR.exe @004479f0>
//bool32 __cdecl LegoRR::LegoObject_Add25EnergyAndSetHealth(LegoObject* liveObj);

// <LegoRR.exe @00447a40>
//void __cdecl LegoRR::LegoObject_FUN_00447a40(LegoObject* liveObj);

// <LegoRR.exe @00447a90>
//void __cdecl LegoRR::LegoObject_FUN_00447a90(LegoObject* liveObj);

// <LegoRR.exe @00447bc0>
//void __cdecl LegoRR::LegoObject_DoBuildingsCallback_AttackByBoulder(LegoObject* liveObj);

// <LegoRR.exe @00447be0>
//bool32 __cdecl LegoRR::LegoObject_CallbackBoulderAttackBuilding_FUN_00447be0(LegoObject* liveObj, LegoObject* buildingLiveObj);

// <LegoRR.exe @00447c10>
//void __cdecl LegoRR::LegoObject_Hit(LegoObject* liveObj, OPTIONAL const Point2F* dir, bool32 reactToHit);

// <LegoRR.exe @00447dc0>
//void __cdecl LegoRR::LegoObject_TeleportDownBuilding(LegoObject* liveObj);

// <LegoRR.exe @00447df0>
//real32 __cdecl LegoRR::LegoObject_MoveAnimation(LegoObject* liveObj, real32 elapsed);

// <LegoRR.exe @00447f00>
bool32 __cdecl LegoRR::LegoObject_UpdateActivityChange(LegoObject* liveObj)
{
	bool32 success = false;

	if (liveObj->activityName1 == liveObj->activityName2)
		return false;

	const char* actName = liveObj->activityName1;
	if (actName != objectGlobs.activityName[Activity_Stand] &&
		actName != objectGlobs.activityName[Activity_Route] &&
		actName != objectGlobs.activityName[Activity_RouteRubble] &&
		actName != objectGlobs.activityName[Activity_Drill] &&
		actName != objectGlobs.activityName[Activity_Walk] &&
		actName != objectGlobs.activityName[Activity_Reverse] &&
		actName != objectGlobs.activityName[Activity_Carry] &&
		actName != objectGlobs.activityName[Activity_CarryStand])
	{
		AITask_DoAnimationWait(liveObj);
	}

	switch (liveObj->type) {
	case LegoObject_Vehicle:
		success = Vehicle_SetActivity(liveObj->vehicle, liveObj->activityName1, liveObj->animTime);
		liveObj->cameraNull = Vehicle_GetCameraNull(liveObj->vehicle, liveObj->cameraFrame);
		break;
	case LegoObject_MiniFigure:
		success = Creature_SetActivity(liveObj->miniFigure, liveObj->activityName1, liveObj->animTime);
		liveObj->cameraNull = Creature_GetCameraNull(liveObj->miniFigure, liveObj->cameraFrame);
		break;
	case LegoObject_RockMonster:
		success = Creature_SetActivity(liveObj->rockMonster, liveObj->activityName1, liveObj->animTime);
		liveObj->cameraNull = Creature_GetCameraNull(liveObj->rockMonster, liveObj->cameraFrame);
		break;
	case LegoObject_Building:
		success = Building_SetActivity(liveObj->building, liveObj->activityName1, liveObj->animTime);
		liveObj->cameraNull = Building_GetCameraNull(liveObj->building, liveObj->cameraFrame);
		break;
	}

	if ((StatsObject_GetStatsFlags1(liveObj) & STATS1_CANBEDRIVEN) &&
		liveObj->driveObject != nullptr && liveObj->driveObject->type == LegoObject_MiniFigure)
	{
		const char* actPrintName;
		if (::_stricmp(liveObj->activityName1, objectGlobs.activityName[Activity_Route]) == 0) {
			actPrintName = "Activity_";
		}
		else {
			actPrintName = liveObj->activityName1;
		}
		char actBuff[256];
		std::sprintf(actBuff, "%s%s", actPrintName, Object_GetTypeName(liveObj->type, liveObj->id));

		Creature_SetActivity(liveObj->driveObject->miniFigure, actBuff, 0.0f);
		liveObj->driveObject->cameraNull = Creature_GetCameraNull(liveObj->driveObject->miniFigure, liveObj->driveObject->cameraFrame);
		liveObj->driveObject->activityName1 = liveObj->driveObject->activityName2;
	}

	liveObj->animTime = 0.0f;
	liveObj->activityName2 = liveObj->activityName1;
	liveObj->flags2 &= ~LIVEOBJ2_ACTIVITYCHANGING;
	return success;
}

// <LegoRR.exe @00448160>
//void __cdecl LegoRR::LegoObject_SimpleObject_MoveAnimation(LegoObject* liveObj, real32 elapsed);

// <LegoRR.exe @00448a80>
//void __cdecl LegoRR::LegoObject_Debug_DropActivateDynamite(LegoObject* liveObj);

// <LegoRR.exe @00448ac0>
//bool32 __cdecl LegoRR::LegoObject_TryDynamite_FUN_00448ac0(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @00448b40>
//bool32 __cdecl LegoRR::LegoObject_PlaceCarriedBirdScarerAt(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @00448c60>
//bool32 __cdecl LegoRR::LegoObject_PlaceBirdScarer_AndTickDown(LegoObject* liveObj);

// <LegoRR.exe @00448d20>
//bool32 __cdecl LegoRR::LegoObject_TryElecFence_FUN_00448d20(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @00448f10>
//bool32 __cdecl LegoRR::LegoObject_TryBuildPath_FUN_00448f10(LegoObject* liveObj);

// <LegoRR.exe @00448f50>
//bool32 __cdecl LegoRR::LegoObject_TryUpgrade_FUN_00448f50(LegoObject* liveObj, LegoObject* targetObj, sint32 targetObjLevel);

// <LegoRR.exe @00449170>
//bool32 __cdecl LegoRR::LegoObject_TryTrain_FUN_00449170(LegoObject* liveObj, LegoObject* targetObj, bool32 set_0xE_or0xF);

// <LegoRR.exe @004492d0>
//bool32 __cdecl LegoRR::LegoObject_TryRechargeCarried(LegoObject* liveObj);

// <LegoRR.exe @00449360>
//bool32 __cdecl LegoRR::LegoObject_TryRepairDrainObject(LegoObject* liveObj, LegoObject* targetObj, bool32 setRouteFlag20, bool32 setLive4Flag400000);

// <LegoRR.exe @00449500>
//bool32 __cdecl LegoRR::LegoObject_TryReinforceBlock(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @00449570>
//bool32 __cdecl LegoRR::LegoObject_TryClear_FUN_00449570(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @004496a0>
//bool32 __cdecl LegoRR::LegoObject_MiniFigureHasBeamEquipped(LegoObject* liveObj);

// <LegoRR.exe @004496f0>
//bool32 __cdecl LegoRR::LegoObject_TryAttackRockMonster_FUN_004496f0(LegoObject* liveObj, LegoObject* targetObj);

// <LegoRR.exe @004497e0>
//bool32 __cdecl LegoRR::LegoObject_TryAttackObject_FUN_004497e0(LegoObject* liveObj, LegoObject* targetObj);

// <LegoRR.exe @004498d0>
//bool32 __cdecl LegoRR::LegoObject_TryAttackPath_FUN_004498d0(LegoObject* liveObj, const Point2I* blockPos);

// <LegoRR.exe @004499c0>
//bool32 __cdecl LegoRR::LegoObject_TryDepart_FUN_004499c0(LegoObject* liveObj);

// <LegoRR.exe @00449b40>
//bool32 __cdecl LegoRR::LegoObject_RouteToFaceBlock(LegoObject* liveObj, uint32 bx, uint32 by, real32 distFrom);

// <LegoRR.exe @00449c40>
//bool32 __cdecl LegoRR::LegoObject_Update_Reinforcing(LegoObject* liveObj, real32 elapsed, OUT bool32* finished);

// <LegoRR.exe @00449d30>
//void __cdecl LegoRR::LegoObject_GoEat_unk(LegoObject* liveObj);

// <LegoRR.exe @00449d80>
//bool32 __cdecl LegoRR::LegoObject_TryGoEat_FUN_00449d80(LegoObject* liveObj1, LegoObject* liveObj2);

// <LegoRR.exe @00449ec0>
void __cdecl LegoRR::LegoObject_HideAllCertainObjects(void)
{
	for (LegoObject* obj : objectListSet.EnumerateAlive()) {
		LegoObject_Callback_HideCertainObjects(obj, nullptr);
	}
	//LegoObject_RunThroughLists(LegoObject_Callback_HideCertainObjects, nullptr, false);
}

// <LegoRR.exe @00449ee0>
//void __cdecl LegoRR::LegoObject_FlocksCallback_HideCertainObjects(Flocks* flockData, FlocksItem* subdata, void* data);

// <LegoRR.exe @00449f90>
//void __cdecl LegoRR::LegoObject_Hide2(LegoObject* liveObj, bool32 hide2);

// <LegoRR.exe @00449fb0>
//bool32 __cdecl LegoRR::LegoObject_Callback_HideCertainObjects(LegoObject* liveObj, void* unused);

// <LegoRR.exe @0044a210>
//void __cdecl LegoRR::LegoObject_Hide(LegoObject* liveObj, bool32 hide);

// <LegoRR.exe @0044a2b0>
//bool32 __cdecl LegoRR::LegoObject_IsHidden(LegoObject* liveObj);

// <LegoRR.exe @0044a330>
//void __cdecl LegoRR::LegoObject_FP_GetPositionAndHeading(LegoObject* liveObj, sint32 cameraFrame, OUT Vector3F* worldPos, OPTIONAL OUT Vector3F* dir);

// <LegoRR.exe @0044a4c0>
//Gods98::Container* __cdecl LegoRR::LegoObject_GetActivityContainer(LegoObject* liveObj);

// <LegoRR.exe @0044a560>
//bool32 __cdecl LegoRR::LegoObject_GetDrillNullPosition(LegoObject* liveObj, OUT real32* xPos, OUT real32* yPos);

// <LegoRR.exe @0044a5d0>
//void __cdecl LegoRR::LegoObject_FP_Move(LegoObject* liveObj, sint32 forward, sint32 strafe, real32 rotate);

// <LegoRR.exe @0044a650>
//void __cdecl LegoRR::LegoObject_RegisterRechargeSeam(const Point2I* blockPos);

// <LegoRR.exe @0044a690>
//bool32 __cdecl LegoRR::LegoObject_FindNearestRechargeSeam(LegoObject* liveObj, OPTIONAL OUT Point2I* blockPos);

// <LegoRR.exe @0044a770>
//void __cdecl LegoRR::LegoObject_RegisterSlimySlugHole(const Point2I* blockPos);

// <LegoRR.exe @0044a7b0>
//bool32 __cdecl LegoRR::LegoObject_FindNearestSlugHole(LegoObject* liveObj, OPTIONAL OUT Point2I* blockPos);

// <LegoRR.exe @0044a890>
//bool32 __cdecl LegoRR::LegoObject_FindNearestWall(LegoObject* liveObj, OUT sint32* bx, OUT sint32* by, bool32 min1BlockDist, bool32 allowCorner, bool32 allowReinforced);

// <LegoRR.exe @0044aa60>
//bool32 __cdecl LegoRR::LegoObject_QsortCompareWallDistances(const void* a, const void* b);

// <LegoRR.exe @0044aa90>
//LegoRR::MeshLOD* __cdecl LegoRR::LegoObject_LoadMeshLOD(const Gods98::Config* act, const char* gameName, const char* dirname, LOD_PolyLevel polyLOD, uint32 numCameraFrames);

// <LegoRR.exe @0044abd0>
bool32 __cdecl LegoRR::LegoObject_UpdateBuildingPlacement(LegoObject_Type objType, LegoObject_ID objID, bool32 leftReleased, bool32 rightReleased,
															OPTIONAL const Point2F* mouseWorldPos, uint32 mouseBlockX, uint32 mouseBlockY,
															bool32 execute, SelectPlace* selectPlace)
{
	const Point2I mouseBlockPos = {
		static_cast<sint32>(mouseBlockX),
		static_cast<sint32>(mouseBlockY),
	};

	if (objType != LegoObject_Building)
		return true; // Default return value.

	if (!execute) {
		SelectPlace_Hide(selectPlace, true);
		return true; // Default return value.
	}

	if (!Level_Block_IsGround(mouseBlockX, mouseBlockY)) {
		if (!rightReleased) {
			Pointer_SetCurrent_IfTimerFinished(Pointer_CannotBuild);
		}
		if (!rightReleased && leftReleased) {
			Lego_SetPointerSFX(PointerSFX_NotOkay);
		}

		SelectPlace_Hide(selectPlace, true);
	}
	else {
		uint32 shapeCount = 0; // dummy init
		const Point2I* shapePoints = Building_GetShapePoints(&legoGlobs.buildingData[objID], &shapeCount);
		const uint32 waterEntrances = Stats_GetWaterEntrances(LegoObject_Building, objID, 0);
		SelectPlace_Hide(selectPlace, false);

		Point2F wPos = { 0.0f }; // dummy init
		Map3D_BlockToWorldPos(Lego_GetMap(), mouseBlockX, mouseBlockY, &wPos.x, &wPos.y);

		Point2F wOff; // Offset from where the mouse is to the center of the block.
		Gods98::Maths_Vector2DSubtract(&wOff, mouseWorldPos, &wPos);

		// Try to snap to a new default valid rotation, note that this may be replaced during the direction loop below this.
		if (wOff.x > 0.0f && wOff.x > std::abs(wOff.y)) {
			legoGlobs.placeObjDirection = DIRECTION_RIGHT;
		}
		else if (wOff.x < 0.0f && std::abs(wOff.x) > std::abs(wOff.y)) {
			legoGlobs.placeObjDirection = DIRECTION_LEFT;
		}
		else if (wOff.y > 0.0f && wOff.y > std::abs(wOff.x)) {
			// Wouldn't this actually be down? If we're following conventions for left/right...
			legoGlobs.placeObjDirection = DIRECTION_UP;
		}
		else if (wOff.y < 0.0f && std::abs(wOff.y) > std::abs(wOff.x)) {
			legoGlobs.placeObjDirection = DIRECTION_DOWN;
		}
		else {
			/// FIXME: No handling for this scenario???
			//legoGlobs.placeObjDirection = ???;
		}

		// FIVE!!???
		for (uint32 d = 0; d < 5; d++) {
			const Direction direction = DirectionWrap(legoGlobs.placeObjDirection + d);

			const Point2I* shapeBlocks = SelectPlace_CheckAndUpdate(selectPlace, &mouseBlockPos, shapePoints, shapeCount,
																	direction, Lego_GetMap(), waterEntrances);
			// I think this may return null if the shape is outside the world bounds, or doesn't meet OSHA regulations.
			if (shapeBlocks == nullptr)
				continue;

			if (!rightReleased) {
				Pointer_SetCurrent_IfTimerFinished(Pointer_CanBuild);
			}
			if (!rightReleased && leftReleased) {
				const uint32 constructHandle = Construction_Zone_StartBuilding(objID, &mouseBlockPos, direction,
																			   shapeBlocks, shapeCount);
				// Request Barriers:
				//const StatsFlags1 sflags1 = Stats_GetStatsFlags1(LegoObject_Building, objID);
				//if (!(sflags1 & STATS1_STOREOBJECTS)) {
				if (Stats_GetRequiresConstructionBarriers(LegoObject_Building, objID)) {
					// Only non-ToolStore objects require barriers.
					Construction_Zone_RequestBarriers(&mouseBlockPos, shapeBlocks, shapeCount);
				}

				// Request Crystals:
				const uint32 crystalsCost = Stats_GetCostCrystal(LegoObject_Building, objID, 0);
				Construction_Zone_RequestResource(&mouseBlockPos, LegoObject_PowerCrystal, (LegoObject_ID)0, 0, crystalsCost);

				// Request Ore / Studs:
				// Studs will only be used if a processing building is available, and if the studs cost is non-zero.
				const LegoObject* oreProcessingObj = LegoObject_FindResourceProcessingBuilding(nullptr, &mouseBlockPos, LegoObject_Ore, 0);
				const uint32 oreCost = Stats_GetCostOre(LegoObject_Building, objID, 0);
				const uint32 studsCost = Stats_GetCostRefinedOre(LegoObject_Building, objID, 0);

				LegoObject_ID oreObjID;
				uint32 oreTypeCost;
				if (oreProcessingObj != nullptr && studsCost > 0) {
					// An ore processing building exists, request studs instead of ore.
					oreObjID    = LegoObject_ID_ProcessedOre;
					oreTypeCost = studsCost;
				}
				else { // Otherwise, fallback to regular ore.
					oreObjID    = LegoObject_ID_Ore;
					oreTypeCost = oreCost;
				}
				Construction_Zone_RequestResource(&mouseBlockPos, LegoObject_Ore, oreObjID, 0, oreTypeCost);

				// Track this info to properly handle Construction_Zone_NeedsMoreOfResource.
				Construction_Zone_SetNoBuildCost(constructHandle, Cheat_IsNoBuildCosts());

				// Dummy call to check condition of having all resources (needed for buildings like toolstore).
				Construction_Zone_PlaceResource(constructHandle, nullptr);

				SelectPlace_Hide(selectPlace, true);
				Interface_BackToMain();
				Lego_SetPointerSFX(PointerSFX_Okay);
				return false; // End selectPlace mode??
			}
			else {
				// Found a valid direction, but didn't place.
				// Store this direction as the last valid direction.
				legoGlobs.placeObjDirection = direction;
				//break;
				return true;
			}
		}

		if (!rightReleased) {
			Pointer_SetCurrent_IfTimerFinished(Pointer_CannotBuild);
		}
		if (!rightReleased && leftReleased) {
			Lego_SetPointerSFX(PointerSFX_NotOkay);
		}
	}
	Text_DisplayMessage(Text_CannotPlaceBuilding, false, false);
	return true;
}

// <LegoRR.exe @0044af80>
//void __cdecl LegoRR::LegoObject_LoadObjTtsSFX(const Gods98::Config* config, const char* gameName);

// <LegoRR.exe @0044b040>
//LegoRR::SFX_ID __cdecl LegoRR::LegoObject_GetObjTtSFX(LegoObject* liveObj);

// <LegoRR.exe @0044b080>
void __cdecl LegoRR::LegoObject_SetLevelEnding(bool32 ending)
{
	if (ending) objectGlobs.flags |= LegoObject_GlobFlags::OBJECT_GLOB_FLAG_LEVELENDING;
	else        objectGlobs.flags &= ~LegoObject_GlobFlags::OBJECT_GLOB_FLAG_LEVELENDING;
}

// <LegoRR.exe @0044b0a0>
void __cdecl LegoRR::LegoObject_FUN_0044b0a0(LegoObject* liveObj)
{
	if (!(objectGlobs.flags & LegoObject_GlobFlags::OBJECT_GLOB_FLAG_LEVELENDING) &&
		(liveObj->type == LegoObject_Vehicle) && !(liveObj->flags4 & LIVEOBJ4_CRYORECOSTDROPPED))
	{
		LegoObject* routeToObj = ((liveObj->flags4 & LIVEOBJ4_DOCKOCCUPIED) ? liveObj->routeToObject : nullptr);

		LegoObject* spawnTargetObj = (routeToObj != nullptr ? routeToObj : liveObj);

		uint32 crystalCost = Stats_GetCostCrystal(LegoObject_Vehicle, liveObj->id, 0);
		LegoObject_SpawnDropCrystals_FUN_0044b110(spawnTargetObj, crystalCost, (routeToObj != nullptr));
		liveObj->flags4 |= LIVEOBJ4_CRYORECOSTDROPPED;
	}
}

// <LegoRR.exe @0044b110>
//void __cdecl LegoRR::LegoObject_SpawnDropCrystals_FUN_0044b110(LegoObject* liveObj, sint32 crystalCount, bool32 param_3);

// <LegoRR.exe @0044b250>
//void __cdecl LegoRR::LegoObject_CallsSpawnDropCrystals_FUN_0044b250(LegoObject* liveObj);

// <LegoRR.exe @0044b270>
//void __cdecl LegoRR::LegoObject_GenerateTinyRMs_FUN_0044b270(LegoObject* liveObj);

// <LegoRR.exe @0044b400>
//void __cdecl LegoRR::LegoObject_GenerateSmallSpiders(uint32 bx, uint32 by, uint32 randSeed);

// <LegoRR.exe @0044b510>
//void __cdecl LegoRR::LegoObject_DoThrowLegoman(LegoObject* liveObj, LegoObject* thrownObj);

// <LegoRR.exe @0044b610>
//bool32 __cdecl LegoRR::LegoObject_Tool_IsBeamWeapon(LegoObject_ToolType toolType);

// <LegoRR.exe @0044b630>
//void __cdecl LegoRR::LegoObject_MiniFigure_EquipTool(LegoObject* liveObj, LegoObject_ToolType toolType);

// <LegoRR.exe @0044b740>
//bool32 __cdecl LegoRR::LegoObject_HasToolEquipped(LegoObject* liveObj, LegoObject_ToolType toolType);

// <LegoRR.exe @0044b780>
//bool32 __cdecl LegoRR::LegoObject_TaskHasTool_FUN_0044b780(LegoObject* liveObj, AITask_Type taskType);

// <LegoRR.exe @0044b7c0>
//sint32 __cdecl LegoRR::LegoObject_DoGetTool(LegoObject* liveObj, LegoObject_ToolType toolType);

// <LegoRR.exe @0044b940>
//void __cdecl LegoRR::LegoObject_Flocks_Initialise(LegoObject* liveObj);

// <LegoRR.exe @0044ba60>
//void __cdecl LegoRR::LegoObject_FlocksMatrix_FUN_0044ba60(Gods98::Container* resData, IN OUT Matrix4F* matrix, real32 scalar);

// <LegoRR.exe @0044bb10>
//void __cdecl LegoRR::LegoObject_Flocks_Callback_SubdataOrientationAnim(Flocks* flocksData, FlocksItem* subdata, real32* pElapsed);

// <LegoRR.exe @0044c2f0>
bool32 __cdecl LegoRR::LegoObject_Freeze(LegoObject* liveObj, real32 freezerTime)
{
	/// CHANGE: Allow other units to be frozen, this is a pointless restriction when CANFREEZE is a stat.
	if (!(liveObj->flags2 & LIVEOBJ2_FROZEN) /*&& liveObj->type == LegoObject_RockMonster*/) {
		liveObj->flags2 |= LIVEOBJ2_FROZEN;
		liveObj->freezeTimer = freezerTime * STANDARD_FRAMERATE;

		Point2F wPos = { 0.0f }; // dummy init
		LegoObject_GetPosition(liveObj, &wPos.x, &wPos.y);
		const real32 heading = LegoObject_GetHeading(liveObj);
		
		LegoObject* iceCubeObj = LegoObject_CreateInWorld(legoGlobs.contIceCube, LegoObject_IceCube, (LegoObject_ID)0, 0, wPos.x, wPos.y, heading);
		Gods98::Container_SetActivity(iceCubeObj->other, "Start");
		Gods98::Container_SetAnimationTime(iceCubeObj->other, 0.0f);
		iceCubeObj->flags1 |= LIVEOBJ1_EXPANDING;

		liveObj->freezeObject = iceCubeObj;
		iceCubeObj->freezeObject = liveObj;

		return true;
	}
	return false;
}

// <LegoRR.exe @0044c3d0>
//void __cdecl LegoRR::LegoObject_FUN_0044c3d0(LegoObject* liveObj);

// <LegoRR.exe @0044c410>
//bool32 __cdecl LegoRR::LegoObject_Push(LegoObject* liveObj, const Point2F* pushVec2D, real32 pushDist);

// <LegoRR.exe @0044c470>
//void __cdecl LegoRR::LegoObject_UpdatePushing(LegoObject* liveObj, real32 elapsed);

// <LegoRR.exe @0044c760>
//void __cdecl LegoRR::LegoObject_TryEnterLaserTrackerMode(LegoObject* liveObj);

// <LegoRR.exe @0044c7c0>
//bool32 __cdecl LegoRR::LegoObject_Callback_ExitLaserTrackerMode(LegoObject* liveObj, void* unused);

// <LegoRR.exe @0044c7f0>
//bool32 __cdecl LegoRR::LegoObject_MiniFigureHasBeamEquipped2(LegoObject* liveObj);

// <LegoRR.exe @0044c810>
void __cdecl LegoRR::LegoObject_CameraCycleUnits(void)
{
	// Cycle-able units have now been validated since level-start (once the function exits).
	objectGlobs.flags |= LegoObject_GlobFlags::OBJECT_GLOB_FLAG_CYCLEUNITS;

	bool unitsExist = false; // Track if we've found the unit type to cycle through.

	// Start by trying to cycle through buildings.
	//bool32 noBuildings = false;
	LegoObject_TypeFlags objTypeFlags = OBJECT_TYPE_FLAG_BUILDING;
	if (LegoObject_CameraCycleFindNextUnitByFlags(objTypeFlags, &unitsExist))
		return; // New building added to cycle list and camera moved. We're done.


	// If no buildings are constructed, then cycle through minifigure unit types instead.
	if (!unitsExist) {//objectGlobs.cycleBuildingCount == 0) {
		// Change cycle unit type to minifigures.
		//noBuildings = true;
		objTypeFlags = OBJECT_TYPE_FLAG_MINIFIGURE;
		if (LegoObject_CameraCycleFindNextUnitByFlags(objTypeFlags, &unitsExist))
			return; // New minifigure added to cycle list and camera moved. We're done.
	}

	// If cycle-able units exist, then all valid units were already in the list.
	if (unitsExist) {//objectGlobs.cycleUnitCount != 0) {
		// Reset the cycle list, then jump to the first available unit of our cycle unit type (building or minifigure).

		// This behavior is used so that the cycle list is guaranteed to not repeat itself until all units
		// have been visited once. After all units have been visited, the list is started anew.
		//  (This effectively functions like a music shuffle, without the intention of shuffling.)

		// Note that the order of units may change if new valid units were created in the middle of a cycle,
		// This is because the order of the cycle is first and foremost based on where the object is in the listSet,
		// And the listSet is not intended to be ordered based on when a unit is added, as freed up slots earlier in
		// the list may be used instead.
		objectGlobs.cycleUnitCount = 0;
		objectGlobs.cycleBuildingCount = 0;

		// Find first unit to cycle to.
		if (LegoObject_CameraCycleFindNextUnitByFlags(objTypeFlags, nullptr))
			return; // First building/minifigure added to cycle list and camera moved. We're done.
	}
}

/// CUSTOM: Wrapper around LegoObject_Callback_CameraCycleFindUnitByFlags with extra limit handling.
bool LegoRR::LegoObject_CameraCycleFindNextUnitByFlags(LegoObject_TypeFlags objTypeFlags, OPTIONAL OUT bool* unitsExist)
{
	/// FIX APPLY: Handle cycleUnitCount limit here instead of in callback.
	///            This allows us to restart the cycle immediately, instead of after one use.
	if (objectGlobs.cycleUnitCount >= OBJECT_MAXCYCLEUNITS) {
		// If we've reached the maximum possible units to cycle through then restart the list.
		// We want to do this here so it's as early as possible in LegoObject_CameraCycleUnits,
		//  to allow checking for all unit types.
		objectGlobs.cycleUnitCount = 0;
		objectGlobs.cycleBuildingCount = 0;
	}

	if (unitsExist) *unitsExist = false;

	for (LegoObject* obj : objectListSet.EnumerateSkipUpgradeParts()) {
		if (LegoObject_Callback_CameraCycleFindUnitByFlags(obj, objTypeFlags, unitsExist))
			return true; // New unit added to cycle list and camera moved.
	}
	return false;
}

/// CUSTOM: Extension of LegoObject_Callback_CameraCycleFindUnit so that we can customize what objects are included in the cycle.
bool LegoRR::LegoObject_Callback_CameraCycleFindUnitByFlags(LegoObject* liveObj, LegoObject_TypeFlags objTypeFlags, OPTIONAL IN OUT bool* unitsExist)
{
	// Only check for units of a specific object type:
	if (!LegoObject_TypeFlagsHasType(objTypeFlags, liveObj->type))
		return false;

	// Don't check for units already in cycleUnits:
	for (uint32 i = 0; i < objectGlobs.cycleUnitCount; i++) {
		if (objectGlobs.cycleUnits[i] == liveObj) {
			if (unitsExist) *unitsExist = true;
			return false;
		}
	}

	if (objectGlobs.cycleUnitCount < OBJECT_MAXCYCLEUNITS) {
		if (unitsExist) *unitsExist = true;

		objectGlobs.cycleUnits[objectGlobs.cycleUnitCount++] = liveObj;
		if (liveObj->type == LegoObject_Building)
			objectGlobs.cycleBuildingCount++;

		Lego_Goto(liveObj, nullptr, false);

		// Move the topdown spotlight. Our cursor is over the panel interface, meaning it won't update the light movement.
		uint32 bx, by; // dummy outputs

		Vector3F wPos;
		if (Map3D_GetIntersections(legoGlobs.currLevel->map, legoGlobs.viewMain, Gods98::msx(), Gods98::msy(), &bx, &by, &wPos)) {
			Gods98::Container_SetPosition(legoGlobs.rootLight, nullptr, wPos.x, wPos.y, wPos.z - 250.0f);
			return true; // Object found and camera moved successfully.
		}
		/// TODO: Should we still return true on failure when an new unit is found?
	}
	/// FIX APPLY: Failure on limit hit/failed camera move. Part of handling for reaching the limit fix.
	return false;

	/*else {
		objectGlobs.cycleUnitCount = 0;
		objectGlobs.cycleBuildingCount = 0;
	}
	return true; // Is returning true here intentional?
	             // So cycle functionality breaks for one use after 256 building or minifigure units I guess...?
	*/
}

// <LegoRR.exe @0044c8b0>
bool32 __cdecl LegoRR::LegoObject_Callback_CameraCycleFindUnit(LegoObject* liveObj, OPTIONAL void* pNoBuildings)
{
	const bool32 noBuildings = (pNoBuildings != nullptr ? *(bool32*)pNoBuildings : false); // Default to false when NULL.

	LegoObject_TypeFlags objTypeFlags;
	if (noBuildings) objTypeFlags = OBJECT_TYPE_FLAG_MINIFIGURE;
	else             objTypeFlags = OBJECT_TYPE_FLAG_BUILDING;

	//if (noBuildings) {
	//	if (liveObj->type != LegoObject_MiniFigure) return false;
	//}
	//else {
	//	if (liveObj->type != LegoObject_Building) return false;
	//}

	return LegoObject_Callback_CameraCycleFindUnitByFlags(liveObj, objTypeFlags, nullptr);
}



/// CUSTOM: Starts the tickdown for dynamite or sonic blaster.
void LegoRR::LegoObject_StartTickDown(LegoObject* liveObj, bool showInfoMessage)
{
	if (liveObj->type == LegoObject_Dynamite || liveObj->type == LegoObject_OohScary) {
		//liveObj->flags1 &= ~LIVEOBJ1_PLACING;
		liveObj->flags3 |= LIVEOBJ3_UNK_10000;

		Gods98::Container_SetActivity(liveObj->other, "TickDown");
		Gods98::Container_SetAnimationTime(liveObj->other, 0.0f);

		Point2I blockPos = { 0 }; // dummy init
		LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
		Level_Block_SetBusyFloor(&blockPos, true);
		
		if (showInfoMessage) {
			Info_Send(Info_DynamitePlaced, nullptr, liveObj, nullptr);
		}
	}
}

#pragma endregion
