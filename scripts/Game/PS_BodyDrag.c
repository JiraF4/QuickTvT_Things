class PS_BodyDragUserAction : ScriptedUserAction
{
	SCR_ChimeraCharacter m_Character;
	SCR_CharacterDamageManagerComponent m_CharacterDamageManagerComponent;
	CharacterControllerComponent m_CharacterControllerComponent;
	SCR_CompartmentAccessComponent m_CompartmentAccessComponent;
	CharacterAnimationComponent m_CharacterAnimationComponent;
	
	BaseWorld m_World;
	SCR_ChimeraCharacter m_UserCharacter;
	SCR_CharacterDamageManagerComponent m_UserCharacterDamageManagerComponent;
	CharacterControllerComponent m_UserCharacterControllerComponent;
	SCR_CompartmentAccessComponent m_UserCompartmentAccessComponent;
	CharacterAnimationComponent m_UserCharacterAnimationComponent;
	TAnimGraphCommand m_UserDragCommand;
	
	PS_DragVehicle m_DragVehicle;
	bool m_bDragging;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_Character = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		GetGame().GetCallqueue().Call(LateInit);
	}
	
	void LateInit()
	{
		m_CharacterDamageManagerComponent = SCR_CharacterDamageManagerComponent.Cast(m_Character.GetDamageManager());
		m_CharacterControllerComponent = m_Character.GetCharacterController();
		m_CompartmentAccessComponent = SCR_CompartmentAccessComponent.Cast(m_Character.FindComponent(SCR_CompartmentAccessComponent));
		m_CharacterAnimationComponent = m_Character.GetAnimationComponent();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
		CacheComponents(pUserEntity);
		StartDrag(pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	void StartDrag(IEntity pUserEntity)
	{
		if (m_bDragging)
			return;
		
		if (Replication.IsServer())
		{
	      Resource resource = Resource.Load("{6A5F85999D44ABCD}Prefabs/Drag/Drag_Vehicle.et");
			EntitySpawnParams params = new EntitySpawnParams();
			vector angles = m_Character.GetYawPitchRoll();
			angles[1] = 0;
			angles[2] = 0;
			Math3D.AnglesToMatrix(angles, params.Transform);
			params.Transform[3] = m_Character.GetOrigin();
	      m_DragVehicle = PS_DragVehicle.Cast(GetGame().SpawnEntityPrefab(resource, m_World, params));
			m_DragVehicle.InitServer(pUserEntity, m_Character);
		}
		
		m_bDragging = true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		StopDrag();
	}
	
	override void OnRejected(IEntity pUserEntity)
	{
		StopDrag();
	}
	
	//------------------------------------------------------------------------------------------------
	void StopDrag()
	{
		if (!m_bDragging)
			return;
		m_bDragging = false;
		
		if (!Replication.IsServer())
			return;
		
		m_DragVehicle.Release();
		m_DragVehicle = null;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		CacheComponents(user);
		
		if (m_UserCharacterAnimationComponent.IsWeaponADSTag())
		{
			SetCannotPerformReason(ActionMenuFailReason.DEFAULT);
			StopDrag();
			return false;
		}
		
		ECharacterStance characterStance = m_UserCharacterControllerComponent.GetStance();
		if (characterStance != ECharacterStance.CROUCH)
		{
			SetCannotPerformReason("#PS-UserActionUnavailableCrouchOnly");
			StopDrag();
			return false;
		}
		
		if (m_CharacterAnimationComponent.IsRagdollActive())
		{
			SetCannotPerformReason("#PS-UserActionUnavailableRagdoll");
			StopDrag();
			return false;
		}
		
		vector targetOrigin = m_Character.GetOrigin();
		vector userOrigin = m_UserCharacter.GetOrigin();
		vector vectorToTarget = targetOrigin - userOrigin;
		vectorToTarget[1] = 0;
		float distanceToTarget = vectorToTarget.Length();
		if (distanceToTarget < 0.65 || distanceToTarget > 1.8)
		{
			SetCannotPerformReason(ActionMenuFailReason.DEFAULT);
			StopDrag();
			return false;
		}

		return true;
	}
	
	void CacheComponents(IEntity user)
	{
		SCR_ChimeraCharacter userCharacter = SCR_ChimeraCharacter.Cast(user);
		if (m_UserCharacter != userCharacter)
		{
			m_UserCharacter = userCharacter;
			m_World = userCharacter.GetWorld();
			m_UserCharacterDamageManagerComponent = SCR_CharacterDamageManagerComponent.Cast(userCharacter.GetDamageManager());
			m_UserCharacterControllerComponent = userCharacter.GetCharacterController();
			m_UserCompartmentAccessComponent = SCR_CompartmentAccessComponent.Cast(userCharacter.FindComponent(SCR_CompartmentAccessComponent));
			m_UserCharacterAnimationComponent = userCharacter.GetAnimationComponent();
			m_UserDragCommand = m_UserCharacterAnimationComponent.BindCommand("CMD_Drag");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		CacheComponents(user);
		
		return m_CharacterControllerComponent.IsUnconscious();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript() { return false; };
}