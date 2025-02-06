// Copyright Epic Games, Inc. All Rights Reserved.

#include "OBM_GameplayTags.h"

namespace OBM_AbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_IsDead, "OBM.Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_Cooldown, "OBM.Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_Cost, "OBM.Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_TagsBlocked, "OBM.Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_TagsMissing, "OBM.Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_ActivationGroup, "OBM.Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");
}

namespace OBM_LocomotionModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Grounded, "OBM.LocomotionMode.Grounded");
	UE_DEFINE_GAMEPLAY_TAG(InAir, "OBM.LocomotionMode.InAir");
}

namespace OBM_GaitTags
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, "OBM.Gait.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Running, "OBM.Gait.Running");
}

namespace OBM_LocomotionActionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Mantling, "OBM.LocomotionAction.Mantling");
	UE_DEFINE_GAMEPLAY_TAG(Vaulting, "OBM.LocomotionAction.Vaulting");
	UE_DEFINE_GAMEPLAY_TAG(Ragdolling, "OBM.LocomotionAction.Ragdolling");
	UE_DEFINE_GAMEPLAY_TAG(GettingUp, "OBM.LocomotionAction.GettingUp");
}

namespace OBM_InputTags
{
	UE_DEFINE_GAMEPLAY_TAG(Move, "OBM.InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(Look_Mouse, "OBM.InputTag.LookMouse");
	UE_DEFINE_GAMEPLAY_TAG(Crouch, "OBM.InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Jump, "OBM.InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(ADS, "OBM.InputTag.ADS");
}

namespace OBM_StanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, "OBM.Stance.Standing");
	UE_DEFINE_GAMEPLAY_TAG(Crouching, "OBM.Stance.Crouching");
}

namespace OBM_OverlayModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Default, "OBM.OverlayMode.Default");
	UE_DEFINE_GAMEPLAY_TAG(Pistol, "OBM.OverlayMode.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(Rifle, "OBM.OverlayMode.Rifle");
}

namespace OBM_WeaponTags
{
	UE_DEFINE_GAMEPLAY_TAG(Fire, "OBM.Weapon.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Reload, "OBM.Weapon.Reload");

	UE_DEFINE_GAMEPLAY_TAG(MagazineAmmo, "OBM.Weapon.MagazineAmmo");
	UE_DEFINE_GAMEPLAY_TAG(MagazineSize, "OBM.Weapon.MagazineSize");
	UE_DEFINE_GAMEPLAY_TAG(SpareAmmo, "OBM.Weapon.SpareAmmo");
}

namespace OBM_CheatTags
{
	UE_DEFINE_GAMEPLAY_TAG(GodMode, "OBM.Cheat.GodMode");
	UE_DEFINE_GAMEPLAY_TAG(UnlimitedHealth, "OBM.Cheat.UnlimitedHealth");
}
