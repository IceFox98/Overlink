// Copyright Epic Games, Inc. All Rights Reserved.

#include "OvrlGameplayTags.h"

namespace OvrlCoreTags
{
	UE_DEFINE_GAMEPLAY_TAG(EnemyKilled, "Ovrl.Core.EnemyKilled");
};

namespace OvrlAbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_IsDead, "Ovrl.Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_Cooldown, "Ovrl.Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_Cost, "Ovrl.Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_TagsBlocked, "Ovrl.Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_TagsMissing, "Ovrl.Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateFail_ActivationGroup, "Ovrl.Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");
}

namespace OvrlLocomotionModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Grounded, "Ovrl.LocomotionMode.Grounded");
	UE_DEFINE_GAMEPLAY_TAG(InAir, "Ovrl.LocomotionMode.InAir");
}

namespace OvrlGaitTags
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, "Ovrl.Gait.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Running, "Ovrl.Gait.Running");
}

namespace OvrlLocomotionActionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Mantling, "Ovrl.LocomotionAction.Mantling");
	UE_DEFINE_GAMEPLAY_TAG(Vaulting, "Ovrl.LocomotionAction.Vaulting");
	UE_DEFINE_GAMEPLAY_TAG(Ragdolling, "Ovrl.LocomotionAction.Ragdolling");
	UE_DEFINE_GAMEPLAY_TAG(Sliding, "Ovrl.LocomotionAction.Sliding");
	UE_DEFINE_GAMEPLAY_TAG(WallrunningLeft, "Ovrl.LocomotionAction.WallrunningLeft");
	UE_DEFINE_GAMEPLAY_TAG(WallrunningRight, "Ovrl.LocomotionAction.WallrunningRight");
	UE_DEFINE_GAMEPLAY_TAG(GettingUp, "Ovrl.LocomotionAction.GettingUp");
}

namespace OvrlViewModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(ADS, "Ovrl.ViewMode.ADS");
	UE_DEFINE_GAMEPLAY_TAG(Relaxed, "Ovrl.ViewMode.Relaxed");
}

namespace OvrlInputTags
{
	UE_DEFINE_GAMEPLAY_TAG(Move, "Ovrl.InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(Look_Mouse, "Ovrl.InputTag.LookMouse");
	UE_DEFINE_GAMEPLAY_TAG(Crouch, "Ovrl.InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Jump, "Ovrl.InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Slide, "Ovrl.InputTag.Slide");
	UE_DEFINE_GAMEPLAY_TAG(ADS, "Ovrl.InputTag.ADS");
}

namespace OvrlStanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, "Ovrl.Stance.Standing");
	UE_DEFINE_GAMEPLAY_TAG(Crouching, "Ovrl.Stance.Crouching");
}

namespace OvrlOverlayModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Default, "Ovrl.OverlayMode.Default");
	UE_DEFINE_GAMEPLAY_TAG(Pistol, "Ovrl.OverlayMode.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(Rifle, "Ovrl.OverlayMode.Rifle");
}

namespace OvrlWeaponTags
{
	UE_DEFINE_GAMEPLAY_TAG(Fire, "Ovrl.Weapon.Fire");
	UE_DEFINE_GAMEPLAY_TAG(FireAuto, "Ovrl.Weapon.FireAuto");
	UE_DEFINE_GAMEPLAY_TAG(Reload, "Ovrl.Weapon.Reload");

	UE_DEFINE_GAMEPLAY_TAG(MagazineAmmo, "Ovrl.Weapon.MagazineAmmo");
	UE_DEFINE_GAMEPLAY_TAG(MagazineSize, "Ovrl.Weapon.MagazineSize");
	UE_DEFINE_GAMEPLAY_TAG(SpareAmmo, "Ovrl.Weapon.SpareAmmo");
}

namespace OvrlCheatTags
{
	UE_DEFINE_GAMEPLAY_TAG(GodMode, "Ovrl.Cheat.GodMode");
	UE_DEFINE_GAMEPLAY_TAG(UnlimitedHealth, "Ovrl.Cheat.UnlimitedHealth");
}

namespace OvrlUILayerTags
{
	UE_DEFINE_GAMEPLAY_TAG(LayerGame, "Ovrl.UI.Layer.Game");
	UE_DEFINE_GAMEPLAY_TAG(LayerMenu, "Ovrl.UI.Layer.Menu");
};