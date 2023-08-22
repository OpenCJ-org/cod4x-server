#ifndef __WEAPONDEF_H__
#define __WEAPONDEF_H__

#include "../q_shared.h"
#include "xmodel.h"
#include "sounds.h"
#include "material.h"
#include "../dobj.h"

typedef enum 
{
  MISSILE_GUIDANCE_NONE = 0x0,
  MISSILE_GUIDANCE_SIDEWINDER = 0x1,
  MISSILE_GUIDANCE_HELLFIRE = 0x2,
  MISSILE_GUIDANCE_JAVELIN = 0x3,
  MISSILE_GUIDANCE_COUNT = 0x4,
}guidedMissileType_t;


typedef enum 
{
  WEAPSTICKINESS_NONE = 0x0,
  WEAPSTICKINESS_ALL = 0x1,
  WEAPSTICKINESS_GROUND = 0x2,
  WEAPSTICKINESS_GROUND_WITH_YAW = 0x3,
  WEAPSTICKINESS_COUNT = 0x4,
}WeapStickinessType;

typedef enum 
{
  AMMO_COUNTER_CLIP_NONE = 0x0,
  AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
  AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
  AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
  AMMO_COUNTER_CLIP_ROCKET = 0x4,
  AMMO_COUNTER_CLIP_BELTFED = 0x5,
  AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,
  AMMO_COUNTER_CLIP_COUNT = 0x7,
}ammoCounterClipType_t;

typedef enum 
{
  WEAPPROJEXP_GRENADE = 0x0,
  WEAPPROJEXP_ROCKET = 0x1,
  WEAPPROJEXP_FLASHBANG = 0x2,
  WEAPPROJEXP_NONE = 0x3,
  WEAPPROJEXP_DUD = 0x4,
  WEAPPROJEXP_SMOKE = 0x5,
  WEAPPROJEXP_HEAVY = 0x6,
  WEAPPROJEXP_NUM = 0x7,
}weapProjExposion_t;

typedef enum 
{
  VEH_ACTIVE_RETICLE_NONE = 0x0,
  VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
  VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
  VEH_ACTIVE_RETICLE_COUNT = 0x3,
}activeReticleType_t;

typedef enum 
{
  WEAPOVERLAYRETICLE_NONE = 0x0,
  WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
  WEAPOVERLAYRETICLE_NUM = 0x2,
}weapOverlayReticle_t;

typedef enum 
{
  WEAPSTANCE_STAND = 0x0,
  WEAPSTANCE_DUCK = 0x1,
  WEAPSTANCE_PRONE = 0x2,
  WEAPSTANCE_NUM = 0x3,
}weapStance_t;

typedef enum 
{
  WEAPON_ICON_RATIO_1TO1 = 0x0,
  WEAPON_ICON_RATIO_2TO1 = 0x1,
  WEAPON_ICON_RATIO_4TO1 = 0x2,
  WEAPON_ICON_RATIO_COUNT = 0x3,
}weaponIconRatioType_t;

typedef enum 
{
  OFFHAND_CLASS_NONE = 0x0,
  OFFHAND_CLASS_FRAG_GRENADE = 0x1,
  OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
  OFFHAND_CLASS_FLASH_GRENADE = 0x3,
  OFFHAND_CLASS_COUNT = 0x4,
}OffhandClass;

typedef enum 
{
  PENETRATE_TYPE_NONE = 0x0,
  PENETRATE_TYPE_SMALL = 0x1,
  PENETRATE_TYPE_MEDIUM = 0x2,
  PENETRATE_TYPE_LARGE = 0x3,
  PENETRATE_TYPE_COUNT = 0x4,
}PenetrateType;

typedef enum 
{
  IMPACT_TYPE_NONE = 0x0,
  IMPACT_TYPE_BULLET_SMALL = 0x1,
  IMPACT_TYPE_BULLET_LARGE = 0x2,
  IMPACT_TYPE_BULLET_AP = 0x3,
  IMPACT_TYPE_SHOTGUN = 0x4,
  IMPACT_TYPE_GRENADE_BOUNCE = 0x5,
  IMPACT_TYPE_GRENADE_EXPLODE = 0x6,
  IMPACT_TYPE_ROCKET_EXPLODE = 0x7,
  IMPACT_TYPE_PROJECTILE_DUD = 0x8,
  IMPACT_TYPE_COUNT = 0x9,
}ImpactType;

typedef enum 
{
  WEAPCLASS_RIFLE = 0x0,
  WEAPCLASS_MG = 0x1,
  WEAPCLASS_SMG = 0x2,
  WEAPCLASS_SPREAD = 0x3,
  WEAPCLASS_PISTOL = 0x4,
  WEAPCLASS_GRENADE = 0x5,
  WEAPCLASS_ROCKETLAUNCHER = 0x6,
  WEAPCLASS_TURRET = 0x7,
  WEAPCLASS_NON_PLAYER = 0x8,
  WEAPCLASS_ITEM = 0x9,
  WEAPCLASS_NUM = 0xA,
}weapClass_t;

typedef enum 
{
  WEAPINVENTORY_PRIMARY = 0x0,
  WEAPINVENTORY_OFFHAND = 0x1,
  WEAPINVENTORY_ITEM = 0x2,
  WEAPINVENTORY_ALTMODE = 0x3,
  WEAPINVENTORYCOUNT = 0x4,
}weapInventoryType_t;


typedef enum 
{
  WEAPTYPE_BULLET = 0x0,
  WEAPTYPE_GRENADE = 0x1,
  WEAPTYPE_PROJECTILE = 0x2,
  WEAPTYPE_BINOCULARS = 0x3,
  WEAPTYPE_NUM = 0x4,
}weapType_t;

typedef enum 
{
  WEAPOVERLAYINTERFACE_NONE = 0x0,
  WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
  WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
  WEAPOVERLAYINTERFACECOUNT = 0x3,
}WeapOverlayInteface_t;

typedef enum 
{
  WEAPON_FIRETYPE_FULLAUTO = 0x0,
  WEAPON_FIRETYPE_SINGLESHOT = 0x1,
  WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
  WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
  WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
  WEAPON_FIRETYPECOUNT = 0x5,
}weapFireType_t;


typedef enum 
{
  IT_BAD = 0x0,
  IT_WEAPON = 0x1,
}itemType_t;

typedef struct 
{
  itemType_t giType;
}gitem_s;


struct WeaponDef
{
  const char *szInternalName; // 0x0
  const char *szDisplayName; // 0x4
  const char *szOverlayName; // 0x8
  XModel *gunXModel[16]; // 0xC
  XModel *handXModel; // 0x4C
  const char *szXAnims[33]; // 0x50
  const char *szModeName; // 0xD4
  uint16_t hideTags[8]; // 0xD8
  uint16_t notetrackSoundMapKeys[16]; // 0xE8
  uint16_t notetrackSoundMapValues[16]; // 0x108
  int playerAnimType; // 0x128
  weapType_t weapType; // 0x12C
  weapClass_t weapClass; // 0x130
  PenetrateType penetrateType; // 0x134
  ImpactType impactType; // 0x138
  weapInventoryType_t inventoryType; // 0x13C
  weapFireType_t fireType; // 0x140
  OffhandClass offhandClass; // 0x144
  weapStance_t stance; // 0x148
  struct FxEffectDef *viewFlashEffect; // 0x14C
  struct FxEffectDef *worldFlashEffect; // 0x150
  struct snd_alias_list_t *pickupSound; // 0x154
  struct snd_alias_list_t *pickupSoundPlayer; // 0x158
  struct snd_alias_list_t *ammoPickupSound; // 0x15C
  struct snd_alias_list_t *ammoPickupSoundPlayer; // 0x160
  struct snd_alias_list_t *projectileSound; // 0x164
  struct snd_alias_list_t *pullbackSound; // 0x168
  struct snd_alias_list_t *pullbackSoundPlayer; // 0x16C
  struct snd_alias_list_t *fireSound; // 0x170
  struct snd_alias_list_t *fireSoundPlayer; // 0x174
  struct snd_alias_list_t *fireLoopSound; // 0x178
  struct snd_alias_list_t *fireLoopSoundPlayer; // 0x17C
  struct snd_alias_list_t *fireStopSound; // 0x180
  struct snd_alias_list_t *fireStopSoundPlayer; // 0x184
  struct snd_alias_list_t *fireLastSound; // 0x188
  struct snd_alias_list_t *fireLastSoundPlayer; // 0x18C
  struct snd_alias_list_t *emptyFireSound; // 0x190
  struct snd_alias_list_t *emptyFireSoundPlayer; // 0x194
  struct snd_alias_list_t *meleeSwipeSound; // 0x198
  struct snd_alias_list_t *meleeSwipeSoundPlayer; // 0x19C
  struct snd_alias_list_t *meleeHitSound; // 0x1A0
  struct snd_alias_list_t *meleeMissSound; // 0x1A4
  struct snd_alias_list_t *rechamberSound; // 0x1A8
  struct snd_alias_list_t *rechamberSoundPlayer; // 0x1AC
  struct snd_alias_list_t *reloadSound; // 0x1B0
  struct snd_alias_list_t *reloadSoundPlayer; // 0x1B4
  struct snd_alias_list_t *reloadEmptySound; // 0x1B8
  struct snd_alias_list_t *reloadEmptySoundPlayer; // 0x1BC
  struct snd_alias_list_t *reloadStartSound; // 0x1C0
  struct snd_alias_list_t *reloadStartSoundPlayer; // 0x1C4
  struct snd_alias_list_t *reloadEndSound; // 0x1C8
  struct snd_alias_list_t *reloadEndSoundPlayer; // 0x1CC
  struct snd_alias_list_t *detonateSound; // 0x1D0
  struct snd_alias_list_t *detonateSoundPlayer; // 0x1D4
  struct snd_alias_list_t *nightVisionWearSound; // 0x1D8
  struct snd_alias_list_t *nightVisionWearSoundPlayer; // 0x1DC
  struct snd_alias_list_t *nightVisionRemoveSound; // 0x1E0
  struct snd_alias_list_t *nightVisionRemoveSoundPlayer; // 0x1E4
  struct snd_alias_list_t *altSwitchSound; // 0x1E8
  struct snd_alias_list_t *altSwitchSoundPlayer; // 0x1EC
  struct snd_alias_list_t *raiseSound; // 0x1F0
  struct snd_alias_list_t *raiseSoundPlayer; // 0x1F4
  struct snd_alias_list_t *firstRaiseSound; // 0x1F8
  struct snd_alias_list_t *firstRaiseSoundPlayer; // 0x1FC
  struct snd_alias_list_t *putawaySound; // 0x200
  struct snd_alias_list_t *putawaySoundPlayer; // 0x204
  struct snd_alias_list_t **bounceSound; // 0x208
  struct FxEffectDef *viewShellEjectEffect; // 0x20C
  struct FxEffectDef *worldShellEjectEffect; // 0x210
  struct FxEffectDef *viewLastShotEjectEffect; // 0x214
  struct FxEffectDef *worldLastShotEjectEffect; // 0x218
  struct Material *reticleCenter; // 0x21C
  struct Material *reticleSide; // 0x220
  int iReticleCenterSize; // 0x224
  int iReticleSideSize; // 0x228
  int iReticleMinOfs; // 0x22C
  activeReticleType_t activeReticleType; // 0x230
  float vStandMove[3]; // 0x234
  float vStandRot[3]; // 0x240
  float vDuckedOfs[3]; // 0x24C
  float vDuckedMove[3]; // 0x258
  float vDuckedRot[3]; // 0x264
  float vProneOfs[3]; // 0x270
  float vProneMove[3]; // 0x27C
  float vProneRot[3]; // 0x288
  float fPosMoveRate; // 0x294
  float fPosProneMoveRate; // 0x298
  float fStandMoveMinSpeed; // 0x29C
  float fDuckedMoveMinSpeed; // 0x2A0
  float fProneMoveMinSpeed; // 0x2A4
  float fPosRotRate; // 0x2A8
  float fPosProneRotRate; // 0x2AC
  float fStandRotMinSpeed; // 0x2B0
  float fDuckedRotMinSpeed; // 0x2B4
  float fProneRotMinSpeed; // 0x2B8
  XModel *worldModel[16]; // 0x2BC
  XModel *worldClipModel; // 0x2FC
  XModel *rocketModel; // 0x300
  XModel *knifeModel; // 0x304
  XModel *worldKnifeModel; // 0x308
  struct Material *hudIcon; // 0x30C
  weaponIconRatioType_t hudIconRatio; // 0x310
  struct Material *ammoCounterIcon; // 0x314
  weaponIconRatioType_t ammoCounterIconRatio; // 0x318
  ammoCounterClipType_t ammoCounterClip; // 0x31C
  int iStartAmmo; // 0x320
  const char *szAmmoName; // 0x324
  int iAmmoIndex; // 0x328
  const char *szClipName; // 0x32C - validated
  int iClipIndex; // 0x330
  int iMaxAmmo; // 0x334
  int iClipSize; // 0x338
  int shotCount; // 0x33C
  const char *szSharedAmmoCapName; // 0x340
  int iSharedAmmoCapIndex; // 0x344
  int iSharedAmmoCap; // 0x348
  int damage; // 0x34C
  int playerDamage; // 0x350
  int iMeleeDamage; // 0x354
  int iDamageType; // 0x358
  int iFireDelay; // 0x35C
  int iMeleeDelay; // 0x360
  int meleeChargeDelay; // 0x364
  int iDetonateDelay; // 0x368
  int iFireTime; // 0x36C
  int iRechamberTime; // 0x370
  int iRechamberBoltTime; // 0x374
  int iHoldFireTime; // 0x378
  int iDetonateTime; // 0x37C
  int iMeleeTime; // 0x380
  int meleeChargeTime; // 0x384
  int iReloadTime; // 0x388
  int reloadShowRocketTime; // 0x38C
  int iReloadEmptyTime; // 0x390
  int iReloadAddTime; // 0x394
  int iReloadStartTime; // 0x398
  int iReloadStartAddTime; // 0x39C
  int iReloadEndTime; // 0x3A0
  int iDropTime; // 0x3A4
  int iRaiseTime; // 0x3A8
  int iAltDropTime; // 0x3AC
  int iAltRaiseTime; // 0x3B0
  int quickDropTime; // 0x3B4
  int quickRaiseTime; // 0x3B8
  int iFirstRaiseTime; // 0x3BC
  int iEmptyRaiseTime; // 0x3C0
  int iEmptyDropTime; // 0x3C4
  int sprintInTime; // 0x3C8
  int sprintLoopTime; // 0x3CC
  int sprintOutTime; // 0x3D0
  int nightVisionWearTime; // 0x3D4
  int nightVisionWearTimeFadeOutEnd; // 0x3D8
  int nightVisionWearTimePowerUp; // 0x3DC
  int nightVisionRemoveTime; // 0x3E0
  int nightVisionRemoveTimePowerDown; // 0x3E4
  int nightVisionRemoveTimeFadeInStart; // 0x3E8
  int fuseTime; // 0x3EC
  int aiFuseTime; // 0x3F0
  int requireLockonToFire; // 0x3F4
  int noAdsWhenMagEmpty; // 0x3F8
  int avoidDropCleanup; // 0x3FC
  float autoAimRange; // 0x400
  float aimAssistRange; // 0x404
  float aimAssistRangeAds; // 0x408
  float aimPadding; // 0x40C
  float enemyCrosshairRange; // 0x410
  int crosshairColorChange; // 0x414
  float moveSpeedScale; // 0x418
  float adsMoveSpeedScale; // 0x41C
  float sprintDurationScale; // 0x420
  float fAdsZoomFov; // 0x424
  float fAdsZoomInFrac; // 0x428
  float fAdsZoomOutFrac; // 0x42C
  struct Material *overlayMaterial; // 0x430
  struct Material *overlayMaterialLowRes; // 0x434
  weapOverlayReticle_t overlayReticle; // 0x438
  WeapOverlayInteface_t overlayInterface; // 0x43C
  float overlayWidth; // 0x440
  float overlayHeight; // 0x444
  float fAdsBobFactor; // 0x448
  float fAdsViewBobMult; // 0x44C
  float fHipSpreadStandMin; // 0x450
  float fHipSpreadDuckedMin; // 0x454
  float fHipSpreadProneMin; // 0x458
  float hipSpreadStandMax; // 0x45C
  float hipSpreadDuckedMax; // 0x460
  float hipSpreadProneMax; // 0x464
  float fHipSpreadDecayRate; // 0x468
  float fHipSpreadFireAdd; // 0x46C
  float fHipSpreadTurnAdd; // 0x470
  float fHipSpreadMoveAdd; // 0x474
  float fHipSpreadDuckedDecay; // 0x478
  float fHipSpreadProneDecay; // 0x47C
  float fHipReticleSidePos; // 0x480
  int iAdsTransInTime; // 0x484
  int iAdsTransOutTime; // 0x488
  float fAdsIdleAmount; // 0x48C
  float fHipIdleAmount; // 0x490
  float adsIdleSpeed; // 0x494
  float hipIdleSpeed; // 0x498
  float fIdleCrouchFactor; // 0x49C
  float fIdleProneFactor; // 0x4A0
  float fGunMaxPitch; // 0x4A4
  float fGunMaxYaw; // 0x4A8
  float swayMaxAngle; // 0x4AC
  float swayLerpSpeed; // 0x4B0
  float swayPitchScale; // 0x4B4
  float swayYawScale; // 0x4B8
  float swayHorizScale; // 0x4BC
  float swayVertScale; // 0x4C0
  float swayShellShockScale; // 0x4C4
  float adsSwayMaxAngle; // 0x4C8
  float adsSwayLerpSpeed; // 0x4CC
  float adsSwayPitchScale; // 0x4D0
  float adsSwayYawScale; // 0x4D4
  float adsSwayHorizScale; // 0x4D8
  float adsSwayVertScale; // 0x4DC
  int bRifleBullet; // 0x4E0
  int armorPiercing; // 0x4E4
  int bBoltAction; // 0x4E8
  int aimDownSight; // 0x4EC
  int bRechamberWhileAds; // 0x4F0
  float adsViewErrorMin; // 0x4F4
  float adsViewErrorMax; // 0x4F8
  int bCookOffHold; // 0x4FC
  int bClipOnly; // 0x500
  int adsFireOnly; // 0x504
  int cancelAutoHolsterWhenEmpty; // 0x508
  int suppressAmmoReserveDisplay; // 0x50C
  int enhanced; // 0x510
  int laserSightDuringNightvision; // 0x514
  struct Material *killIcon; // 0x518
  weaponIconRatioType_t killIconRatio; // 0x51C
  int flipKillIcon; // 0x520
  struct Material *dpadIcon; // 0x524
  weaponIconRatioType_t dpadIconRatio; // 0x528
  int bNoPartialReload; // 0x52C
  int bSegmentedReload; // 0x530
  int iReloadAmmoAdd; // 0x534
  int iReloadStartAdd; // 0x538
  const char *szAltWeaponName; // 0x53C
  unsigned int altWeaponIndex; // 0x540
  int iDropAmmoMin; // 0x544
  int iDropAmmoMax; // 0x548
  int blocksProne; // 0x54C
  int silenced; // 0x550
  int iExplosionRadius; // 0x554
  int iExplosionRadiusMin; // 0x558
  int iExplosionInnerDamage; // 0x55C
  int iExplosionOuterDamage; // 0x560
  float damageConeAngle; // 0x564
  int iProjectileSpeed; // 0x568
  int iProjectileSpeedUp; // 0x58C
  int iProjectileSpeedForward; // 0x590
  int iProjectileActivateDist; // 0x594
  float projLifetime; // 0x598
  float timeToAccelerate; // 0x5A0
  float projectileCurvature; // 0x5A4
  XModel *projectileModel; // 0x5A8
  weapProjExposion_t projExplosion; // 0x5AC
  struct FxEffectDef *projExplosionEffect; // 0x5B0
  int projExplosionEffectForceNormalUp; // 0x5B4
  struct FxEffectDef *projDudEffect; // 0x5B8
  struct snd_alias_list_t *projExplosionSound; // 0x5BC
  struct snd_alias_list_t *projDudSound; // 0x5C0
  int bProjImpactExplode; // 0x5C4
  WeapStickinessType stickiness; // 0x5C8
  int hasDetonator; // 0x5CC
  int timedDetonation; // 0x5D0
  int rotate; // 0x5D4
  int holdButtonToThrow; // 0x5D8
  int freezeMovementWhenFiring; // 0x5DC
  int lowAmmoWarningThreshold; // 0x5E0
  float parallelBounce[29]; // 0x5E4
  float perpendicularBounce[29]; // 0x658
  struct FxEffectDef *projTrailEffect; // 0x6CC
  float vProjectileColor[3]; // 0x6D0
  guidedMissileType_t guidedMissileType; // 0x6DC
  float maxSteeringAccel; // 0x6E0
  int projIgnitionDelay; // 0x6E4
  struct FxEffectDef *projIgnitionEffect; // 0x6E8
  struct snd_alias_list_t *projIgnitionSound; // 0x6EC
  float fAdsAimPitch; // 0x6F0
  float fAdsCrosshairInFrac; // 0x6F4
  float fAdsCrosshairOutFrac; // 0x6F8
  int adsGunKickReducedKickBullets; // 0x6FC
  float adsGunKickReducedKickPercent; // 0x700
  float fAdsGunKickPitchMin; // 0x704
  float fAdsGunKickPitchMax; // 0x708
  float fAdsGunKickYawMin; // 0x70C
  float fAdsGunKickYawMax; // 0x710
  float fAdsGunKickAccel; // 0x714
  float fAdsGunKickSpeedMax; // 0x718
  float fAdsGunKickSpeedDecay; // 0x71C
  float fAdsGunKickStaticDecay; // 0x720
  float fAdsViewKickPitchMin; // 0x724
  float fAdsViewKickPitchMax; // 0x728
  float fAdsViewKickYawMin; // 0x72C
  float fAdsViewKickYawMax; // 0x730
  float fAdsViewKickCenterSpeed; // 0x734
  float fAdsViewScatterMin; // 0x738
  float fAdsViewScatterMax; // 0x73C
  float fAdsSpread; // 0x740
  int hipGunKickReducedKickBullets; // 0x744
  float hipGunKickReducedKickPercent; // 0x748
  float fHipGunKickPitchMin; // 0x74C
  float fHipGunKickPitchMax; // 0x750
  float fHipGunKickYawMin; // 0x754
  float fHipGunKickYawMax; // 0x758
  float fHipGunKickAccel; // 0x75C
  float fHipGunKickSpeedMax; // 0x760
  float fHipGunKickSpeedDecay; // 0x764
  float fHipGunKickStaticDecay; // 0x768
  float fHipViewKickPitchMin; // 0x76C
  float fHipViewKickPitchMax; // 0x770
  float fHipViewKickYawMin; // 0x774
  float fHipViewKickYawMax; // 0x778
  float fHipViewKickCenterSpeed; // 0x77C
  float fHipViewScatterMin; // 0x780
  float fHipViewScatterMax; // 0x784
  float fightDist; // 0x788
  float maxDist; // 0x78C
  const char *accuracyGraphName[2]; // 0x790
  float (*accuracyGraphKnots[2])[2]; // 0x798
  float (*originalAccuracyGraphKnots[2])[2]; // 0x7A8
  int accuracyGraphKnotCount[2]; // 0x7B8
  int originalAccuracyGraphKnotCount[2]; // 0x7C0
  int iPositionReloadTransTime; // 0x7C8
  float leftArc; // 0x7CC
  float rightArc; // 0x7D0
  float topArc; // 0x7D4
  float bottomArc; // 0x7D8
  float accuracy; // 0x7DC
  float aiSpread; // 0x7E0
  float playerSpread; // 0x7E4
  float minTurnSpeed[2]; // 0x7E8
  float maxTurnSpeed[2]; // 0x7F0
  float pitchConvergenceTime; // 0x7F8
  float yawConvergenceTime; // 0x7FC
  float suppressTime; // 0x800
  float maxRange; // 0x804
  float fAnimHorRotateInc; // 0x808
  float fPlayerPositionDist; // 0x80C
  const char *szUseHintString; // 0x810
  const char *dropHintString; // 0x814
  int iUseHintStringIndex; // 0x818
  int dropHintStringIndex; // 0x81C
  float horizViewJitter; // 0x820
  float vertViewJitter; // 0x824
  const char *szScript; // 0x828
  float fOOPosAnimLength[2]; // 0x82C
  int minDamage; // 0x834
  int minPlayerDamage; // 0x838
  float fMaxDamageRange; // 0x83C
  float fMinDamageRange; // 0x840
  float destabilizationRateTime; // 0x844
  float destabilizationCurvatureMax; // 0x848
  int destabilizeDistance; // 0x84C
  float locationDamageMultipliers[19]; // 0x850
  const char *fireRumble; // 0x89C
  const char *meleeImpactRumble; // 0x8A0
  float adsDofStart; // 0x8A4
  float adsDofEnd; // 0x8A8
}; // size = 0x8AC



#pragma pack(push, 4)
typedef struct weaponInfo_s
{
  DObj_t *viewModelDObj;
  XModel *handModel;
  XModel *gogglesModel;
  XModel *rocketModel;
  XModel *knifeModel;
  char weapModelIdx;
  char pad[3];
  int partBits[4];
  int iPrevAnim;
  struct XAnimTree_s *tree;
  int registered;
  gitem_s *item;
  const char *translatedDisplayName;
  const char *translatedModename;
  const char *translatedAIOverlayDescription;
}weaponInfo_t;
#pragma pack(pop)



extern struct WeaponDef *bg_weaponDefs[];
extern weaponInfo_t cg_weaponsArray[];
extern struct Material* weaponHudIcons[];


#ifdef __cplusplus
extern "C"{
#endif

struct WeaponDef* BG_GetWeaponDef(int index);
int BG_PlayerWeaponCountPrimaryTypes(playerState_t *ps);
int BG_GetNumWeapons();
void ChangeViewmodelDObj(int localClientNum, int weaponNum, char weaponModel, XModel *newHands, XModel *newGoggles, XModel *newRocket, XModel *newKnife, byte updateClientInfo);
void __cdecl Load_WeaponDef(bool atStreamStart);

#ifdef __cplusplus
};
#endif


#endif
