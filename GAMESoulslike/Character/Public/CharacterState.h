#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapons UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	ECS_EquippedBowWeapons UMETA(DisplayName = "Equipped Bow Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_HitReact UMETA(DisplayName = "HitReact"),
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_AimMode UMETA(DisplayName = "Aiming"),
	EAS_Parring UMETA(DisplayName = "Parring"),
	EAS_Dodge UMETA(DisplayName = "Dodging"),
	EAS_Jumping UMETA(DisplayName = "Jumping"),
	EAS_EquipUnequip UMETA(DisplayName = "EquipUnequip")
};


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_HitReact UMETA(DisplayName = "HitReact"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};