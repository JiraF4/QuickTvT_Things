modded class SCR_MeleeWeaponProperties
{
	[Attribute("10", UIWidgets.Slider, "Size of damage dealt by the weapon", "0.0 100.0 1.0", category: "Global")]
	private float m_fResilienceDamage;
	
	float GetResilienceDamage()
	{
		return m_fResilienceDamage;
	}	
}

modded class SCR_MeleeComponent
{
	override protected void ProcessMeleeAttack()
	{
		RplComponent rplComponent = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (rplComponent && rplComponent.IsProxy())
			return; // Don't call on clients
		
		if (!CheckCollisionsSimple(m_MeleeHitData))
			return;
		
		if (m_MeleeHitData.m_Weapon)
			HandleMeleeSound();
		
#ifdef SCR_MELEE_DEBUG
		m_aDbgSamplePositionsShapes.Clear();
		Debug_DrawSphereAtPos(m_MeleeHitData.m_vHitPosition, m_aDbgSamplePositionsShapes, 0xff00ff00, 0.03, ShapeFlags.NOZBUFFER);
#endif
		
		vector hitPosDirNorm[3];
		hitPosDirNorm[0] = m_MeleeHitData.m_vHitPosition;
		hitPosDirNorm[1] = m_MeleeHitData.m_vHitDirection;
		hitPosDirNorm[2] = m_MeleeHitData.m_vHitNormal;
		
		if (m_OnMeleePerformed)
			m_OnMeleePerformed.Invoke(GetOwner());
		
		// check if the entity is destructible entity
		SCR_DestructibleEntity destructibleEntity = SCR_DestructibleEntity.Cast(m_MeleeHitData.m_Entity);
		if (destructibleEntity)
		{
			destructibleEntity.HandleDamage(EDamageType.MELEE, m_MeleeHitData.m_fDamage, hitPosDirNorm);
			return;
		}
		
		// check if the entity has the damage manager component
		HitZone hitZone;
		SCR_DamageManagerComponent damageManager = SearchHierarchyForDamageManager(m_MeleeHitData.m_Entity, hitZone);
		SCR_CharacterDamageManagerComponent characterDamageManager = SCR_CharacterDamageManagerComponent.Cast(damageManager);
		if (characterDamageManager && m_MeleeHitData.m_fDamageResilience > 0)
		{
			HitZone hitZoneResilience = characterDamageManager.GetResilienceHitZone();
			if (hitZoneResilience)
				hitZoneResilience.HandleDamage(m_MeleeHitData.m_fDamageResilience, EDamageType.MELEE, GetOwner());
		}
		
		if (!hitZone && damageManager)
			hitZone = damageManager.GetDefaultHitZone();
		
		if (hitZone)
			hitZone.HandleDamage(m_MeleeHitData.m_fDamage, EDamageType.MELEE, GetOwner());
	}
	
	override protected void CollectMeleeWeaponProperties()
	{
		BaseWeaponManagerComponent wpnManager = BaseWeaponManagerComponent.Cast(GetOwner().FindComponent(BaseWeaponManagerComponent));
		if (!wpnManager)
			return;
		
		WeaponSlotComponent currentSlot = WeaponSlotComponent.Cast(wpnManager.GetCurrent());
		if (!currentSlot)
			return;
		
		//! get current weapon and store it into SCR_MeleeHitDataClass instance
		m_MeleeHitData.m_Weapon = currentSlot.GetWeaponEntity();

		IEntity weaponEntity = currentSlot.GetWeaponEntity();
		if (!weaponEntity)
			return;
		
		SCR_MeleeWeaponProperties meleeWeaponProperties = SCR_MeleeWeaponProperties.Cast(weaponEntity.FindComponent(SCR_MeleeWeaponProperties));
		if (!meleeWeaponProperties)
			return;
		
		m_MeleeHitData.m_fDamage = meleeWeaponProperties.GetWeaponDamage();
		m_fMWPWeaponRange = meleeWeaponProperties.GetWeaponRange();
		m_MeleeHitData.m_fDamageResilience = meleeWeaponProperties.GetResilienceDamage();
	}
}

modded class SCR_MeleeHitDataClass
{
	float m_fDamageResilience = 0;
}