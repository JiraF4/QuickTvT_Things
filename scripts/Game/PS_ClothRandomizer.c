[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class PS_ClothRandomizerComponentClass : ScriptComponentClass
{
}

class PS_ClothRandomizerComponent : ScriptComponent
{
	[Attribute("")]
	ref array<ResourceName> m_aJacket;
	[Attribute("")]
	ref array<ResourceName> m_aPants;
	[Attribute("")]
	ref array<ResourceName> m_aFootwear;
	[Attribute("")]
	ref array<ResourceName> m_aHeadgear;
	[Attribute("0")]
	bool m_bFullSetMode;
	
	static ref RandomGenerator m_RandomGenerator = new RandomGenerator();
	
	SCR_ChimeraCharacter m_ChimeraCharacter;
	SCR_CharacterInventoryStorageComponent m_CharacterInventoryStorageComponent;
	SCR_InventoryStorageManagerComponent m_InventoryStorageManagerComponent;
		
	override void OnPostInit(IEntity owner)
	{
		if (!Replication.IsServer())
			return;
		
		GetGame().GetCallqueue().CallLater(LateInit, 5000, false, GameEntity.Cast(owner));
	}
	
	void LateInit(IEntity owner)
	{
		m_ChimeraCharacter = SCR_ChimeraCharacter.Cast(owner);
		m_CharacterInventoryStorageComponent = SCR_CharacterInventoryStorageComponent.Cast(m_ChimeraCharacter.FindComponent(SCR_CharacterInventoryStorageComponent));
		m_InventoryStorageManagerComponent = SCR_InventoryStorageManagerComponent.Cast(m_ChimeraCharacter.FindComponent(SCR_InventoryStorageManagerComponent));
		
		if (!m_bFullSetMode)
		{
			if (m_aJacket && m_aJacket.Count() > 0)
			{
				ResourceName jacket = m_aJacket.Get(m_RandomGenerator.RandIntInclusive(0, m_aJacket.Count() - 1));
				SwapCloth(LoadoutJacketArea, jacket);
			}
			if (m_aPants && m_aPants.Count() > 0)
			{
				ResourceName pants = m_aPants.Get(m_RandomGenerator.RandIntInclusive(0, m_aPants.Count() - 1));
				SwapCloth(LoadoutPantsArea, pants);
			}
			if (m_aFootwear && m_aFootwear.Count() > 0)
			{
				ResourceName footwear = m_aFootwear.Get(m_RandomGenerator.RandIntInclusive(0, m_aFootwear.Count() - 1));
				SwapCloth(LoadoutBootsArea, footwear, false);
			}
			if (m_aHeadgear && m_aHeadgear.Count() > 0)
			{
				ResourceName headgear = m_aHeadgear.Get(m_RandomGenerator.RandIntInclusive(0, m_aHeadgear.Count() - 1));
				SwapCloth(LoadoutHeadCoverArea, headgear, false);
			}
		} else {
			int setNum = m_RandomGenerator.RandIntInclusive(0, m_aJacket.Count() - 1);
			SwapCloth(LoadoutJacketArea, m_aJacket.Get(setNum));
			SwapCloth(LoadoutPantsArea, m_aPants.Get(setNum));
			SwapCloth(LoadoutBootsArea, m_aFootwear.Get(setNum));
			SwapCloth(LoadoutHeadCoverArea, m_aHeadgear.Get(setNum));
		}
	}
	
	void SwapCloth(typename pAreaType, ResourceName clothResource, bool moveItems = true)
	{
		LoadoutSlotInfo slot = m_CharacterInventoryStorageComponent.GetSlotFromArea(pAreaType);
		IEntity cloth = null;
		
		if (clothResource != "")
		{
	      Resource resource = Resource.Load(clothResource);
			EntitySpawnParams params = new EntitySpawnParams();
			m_ChimeraCharacter.GetTransform(params.Transform);
	      cloth = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
			
			if (moveItems)
			{
				IEntity oldCloth = slot.GetAttachedEntity();
				if (cloth && oldCloth)
				{
					SCR_UniversalInventoryStorageComponent inventory = SCR_UniversalInventoryStorageComponent.Cast(cloth.FindComponent(SCR_UniversalInventoryStorageComponent));
					SCR_UniversalInventoryStorageComponent oldInventory = SCR_UniversalInventoryStorageComponent.Cast(oldCloth.FindComponent(SCR_UniversalInventoryStorageComponent));
					
					if (inventory && oldInventory)
					{
						array<IEntity> outItems = {};
						oldInventory.GetAll(outItems);
						
						foreach (IEntity item : outItems)
						{
							m_InventoryStorageManagerComponent.TryMoveItemToStorage(item, inventory);
						}	
					}
				}
			}
		}
		
		slot.AttachEntity(cloth);
	}
	
}
