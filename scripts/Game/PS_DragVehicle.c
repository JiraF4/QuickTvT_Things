//------------------------------------------------------------------------------------------------
class PS_DragVehicleClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
class PS_DragVehicle : GenericEntity
{
	RplComponent m_RplComponent;
	
	SCR_ChimeraCharacter m_DraggedCharacter;
	RplComponent m_RplComponentDragged;
	SCR_CharacterDamageManagerComponent m_DraggedCharacterDamageManagerComponent;
	CharacterControllerComponent m_DraggedCharacterControllerComponent;
	SCR_CompartmentAccessComponent m_CompartmentAccessComponent;
	CharacterAnimationComponent m_DraggedCharacterAnimationComponent;
	Animation m_DraggedCharacterAnimation;
	TNodeId m_NeckId;
	
	BaseWorld m_World;
	RplComponent m_RplComponentCharacter;
	SCR_ChimeraCharacter m_UserCharacter;
	SCR_CharacterDamageManagerComponent m_UserCharacterDamageManagerComponent;
	CharacterControllerComponent m_UserCharacterControllerComponent;
	SCR_CompartmentAccessComponent m_UserCompartmentAccessComponent;
	CharacterAnimationComponent m_UserCharacterAnimationComponent;
	TAnimGraphCommand m_UserDragCommand;
	
	void InitServer(IEntity character, IEntity draggedCharacter)
	{
		// Await Rpl component
		GetGame().GetCallqueue().Call(InitServerLate, character, draggedCharacter);
	}
	void InitServerLate(IEntity character, IEntity draggedCharacter)
	{
		// Give authority to client
		RplComponent rplComponent = RplComponent.Cast(this.FindComponent(RplComponent));
		PlayerManager playerManager = GetGame().GetPlayerManager();
		int playerId = playerManager.GetPlayerIdFromControlledEntity(character);
		SCR_PlayerController playerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerId));
		rplComponent.Give(playerController.GetRplIdentity());
		
		// Move another player to temp vehicle
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(draggedCharacter.FindComponent(SCR_CompartmentAccessComponent));
		compartmentAccess.MoveInVehicleAny(this);
		compartmentAccess.GetOnCompartmentLeft().Insert(DeleteVehicle);
		
		// Start drag on client
		RplComponent rplComponentCharacterDragged = RplComponent.Cast(draggedCharacter.FindComponent(RplComponent));
		RplComponent rplComponentCharacter = RplComponent.Cast(character.FindComponent(RplComponent));
		Rpc(RPC_InitClient, rplComponentCharacter.Id(), rplComponentCharacterDragged.Id());
		RPC_InitClient(rplComponentCharacter.Id(), rplComponentCharacterDragged.Id());
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RPC_InitClient(RplId characterId, RplId draggedCharacterId)
	{
		// Get Characters from RplId
		m_RplComponent = RplComponent.Cast(this.FindComponent(RplComponent));
		m_RplComponentDragged = RplComponent.Cast(Replication.FindItem(draggedCharacterId));
		m_RplComponentCharacter = RplComponent.Cast(Replication.FindItem(characterId));
		m_DraggedCharacter = SCR_ChimeraCharacter.Cast(m_RplComponentDragged.GetEntity());
		m_UserCharacter = SCR_ChimeraCharacter.Cast(m_RplComponentCharacter.GetEntity());
		
		// Get components from characters
		CacheComponents();
		
		SetEventMask(EntityEvent.POSTFRAME); // Arm IK
		if (m_RplComponent.IsOwner())
			SetEventMask(EntityEvent.FRAME); // Drag
	}
	
	void CacheComponents()
	{
		m_DraggedCharacterDamageManagerComponent = SCR_CharacterDamageManagerComponent.Cast(m_DraggedCharacter.GetDamageManager());
		m_DraggedCharacterControllerComponent = m_DraggedCharacter.GetCharacterController();
		m_CompartmentAccessComponent = SCR_CompartmentAccessComponent.Cast(m_DraggedCharacter.FindComponent(SCR_CompartmentAccessComponent));
		m_DraggedCharacterAnimationComponent = m_DraggedCharacter.GetAnimationComponent();
		m_DraggedCharacterAnimation = m_DraggedCharacter.GetAnimation();
		m_NeckId = m_DraggedCharacterAnimation.GetBoneIndex("Neck1");
		
		m_World = m_UserCharacter.GetWorld();
		m_UserCharacterDamageManagerComponent = SCR_CharacterDamageManagerComponent.Cast(m_UserCharacter.GetDamageManager());
		m_UserCharacterControllerComponent = m_UserCharacter.GetCharacterController();
		m_UserCompartmentAccessComponent = SCR_CompartmentAccessComponent.Cast(m_UserCharacter.FindComponent(SCR_CompartmentAccessComponent));
		m_UserCharacterAnimationComponent = m_UserCharacter.GetAnimationComponent();
		m_UserDragCommand = m_UserCharacterAnimationComponent.BindCommand("CMD_Drag");
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		MoveCharacter(m_DraggedCharacter, m_UserCharacter, timeSlice);
	}
	
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		UpdateArmIK();
	}
	
	//------------------------------------------------------------------------------------------------
	void MoveCharacter(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		IEntity camera = GetGame().GetCameraManager().CurrentCamera();
		
		vector userRotation = m_UserCharacter.GetYawPitchRoll();
		vector userOrigin = m_UserCharacter.GetOrigin();
		vector userForward = userRotation.AnglesToVector();
		vector dragOrigin = userOrigin + userForward * 0.3;
		vector targetRotation = GetYawPitchRoll();
		vector targetOrigin = GetOrigin();
		vector vectorToTarget = targetOrigin - dragOrigin;
		vector anglesToTarget = vectorToTarget.Normalized().VectorToAngles();
		
		// Rotate user to target
		vector cameraRotation = camera.GetYawPitchRoll();
		anglesToTarget[0] = Math.MapAngle(anglesToTarget[0]);
		float anglesDiff = anglesToTarget[0] - cameraRotation[0];
		if (anglesDiff >  180) anglesDiff = anglesDiff - 360;
		if (anglesDiff < -180) anglesDiff = 360 + anglesDiff;
		if (Math.AbsFloat(anglesDiff) > 84 && !m_UserCharacterControllerComponent.IsFreeLookEnabled())
		{
			float rotationValue = GetGame().GetInputManager().GetActionValue("CharacterAimHorizontal");
			rotationValue += anglesDiff / 20;
			GetGame().GetInputManager().SetActionValue("CharacterAimHorizontal", rotationValue);
		}
		
		// Rotate from user
		float targetRotationDiff = targetRotation[0] - anglesToTarget[0];
		if (targetRotationDiff >  180) targetRotationDiff = targetRotationDiff - 360;
		if (targetRotationDiff < -180) targetRotationDiff = 360 + targetRotationDiff;
		if (Math.AbsFloat(targetRotationDiff) > 16)
		{
			if (targetRotationDiff > 0)
				targetRotation[0] = targetRotation[0] - (targetRotationDiff - 16);
			if (targetRotationDiff < 0)
				targetRotation[0] = targetRotation[0] - (targetRotationDiff + 16);
			this.SetYawPitchRoll(targetRotation);
		}
		
		// Drag to user
		if (vectorToTarget.Length() > 0.6)
		{
			vector dragVector = -(vectorToTarget - vectorToTarget.Normalized() * 0.6);
			
			targetOrigin += dragVector;
			
			this.SetOrigin(targetOrigin);
		}
		
		// Get ground position
		TraceParam param = new TraceParam();
		param.Start = GetOrigin();
		param.Start[1] = param.Start[1] + 0.4;
		param.End = param.Start + "0 -1 0";
		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		array<IEntity> exclude = {m_DraggedCharacter, m_UserCharacter};
		param.ExcludeArray = exclude;
		param.LayerMask = EPhysicsLayerPresets.Projectile;
		BaseWorld world = m_DraggedCharacter.GetWorld();
		float traceDistance = world.TraceMove(param, null);
		
		// Lerp to ground
		vector groundMat[4];
		float groundQuat[4];
		Math3D.AnglesToMatrix(anglesToTarget, groundMat);
		Math3D.MatrixToQuat(groundMat, groundQuat);
		vector targetMatLerp[4];
		float targetQuatLerp[4];
		GetTransform(targetMatLerp);
		Math3D.MatrixToQuat(targetMatLerp, targetQuatLerp);
		Math3D.QuatLerp(targetQuatLerp, targetQuatLerp, groundQuat, timeSlice * 8);
		Math3D.QuatToMatrix(targetQuatLerp, targetMatLerp);
		targetMatLerp[3] = GetOrigin() + "0 0.4 0" + "0 -1 0" * traceDistance;
		SetTransform(targetMatLerp);
		
		/* --------------------- Ground normal ---------------------
			                    Look bad actually
		// Trace ground
		float traceDistance;
		vector normal;
		vector offsets[] = {
			"0.1 0 0",
			"-0.1 0 0",
			"0 0 0.1",
			"0 0 -0.1"
		};
		array<IEntity> exclude = {m_DraggedCharacter, m_UserCharacter};
		for (int i = 0; i < 4; i++)
		{
			TraceParam param = new TraceParam();
			param.Start = this.GetOrigin() + offsets[i];
			param.Start[1] = param.Start[1] + 0.4;
			param.End = param.Start + "0 -1 0";
			param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
			param.ExcludeArray = exclude;
			param.LayerMask = EPhysicsLayerPresets.Projectile;
			BaseWorld world = m_DraggedCharacter.GetWorld();
			traceDistance += world.TraceMove(param, null);
			normal += param.TraceNorm;
		}
		traceDistance *= 0.25;
		normal.Normalize();
		
		// Lerp to ground
		vector groundMat[3];
		vector direction = targetRotation.AnglesToVector();
		Math3D.DirectionAndUpMatrix(direction, normal, groundMat);
		float groundQuat[4];
		Math3D.MatrixToQuat(groundMat, groundQuat);
		vector targetMatLerp[4];
		GetTransform(targetMatLerp);
		float targetQuatLerp[4];
		Math3D.MatrixToQuat(targetMatLerp, targetQuatLerp);
		Math3D.QuatLerp(targetQuatLerp, targetQuatLerp, groundQuat, timeSlice * 8);
		Math3D.QuatToMatrix(targetQuatLerp, targetMatLerp);
		targetMatLerp[3] = GetOrigin() + "0 0.4 0" + "0 -1 0" * traceDistance;
		SetTransform(targetMatLerp);
		   --------------------- Ground normal --------------------- */
		
		// Slow movement
		float characterForward = GetGame().GetInputManager().GetActionValue("CharacterForward");
		float characterRight = GetGame().GetInputManager().GetActionValue("CharacterRight");
		float moveMultiplier = 1.0 - (vectorToTarget.Length() / 1.3);
		if (moveMultiplier < 0) moveMultiplier = 0;
		GetGame().GetInputManager().SetActionValue("CharacterForward", characterForward * moveMultiplier);
		GetGame().GetInputManager().SetActionValue("CharacterRight", characterRight * moveMultiplier);
	}
	
	void UpdateArmIK()
	{
		m_UserCharacterAnimationComponent.CallCommand(m_UserDragCommand, 1, 1);
		
		vector matNeck[4];
		m_DraggedCharacterAnimation.GetBoneMatrix(m_NeckId, matNeck);
		vector targetMat[4];
		m_DraggedCharacter.GetTransform(targetMat);
		float targetQuat[4];
		Math3D.MatrixToQuat(targetMat, targetQuat);
		vector neckOrigin = SCR_Math3D.QuatMultiply(targetQuat, matNeck[3]);
		
		vector targetOrigin = m_DraggedCharacter.GetOrigin() + neckOrigin;
		vector userOrigin = m_UserCharacter.GetOrigin();
		vector vectorToTarget = targetOrigin - userOrigin;
		
		vector userMat[4];
		m_UserCharacter.GetTransform(userMat);
		float userQuat[4];
		Math3D.MatrixToQuat(userMat, userQuat);
		Math3D.QuatInverse(userQuat, userQuat);
		vector vectorToTargetLocal = SCR_Math3D.QuatMultiply(userQuat, vectorToTarget);
		CharacterAnimationComponent userCharacterAnimationComponent = CharacterAnimationComponent.Cast(m_UserCharacter.GetAnimationComponent());
		userCharacterAnimationComponent.SetIKTarget("DragTarget", "", vectorToTargetLocal, "0 0 0");
	}
	
	void Release()
	{
		GetGame().GetCallqueue().Remove(ReleaseLoop);
		GetGame().GetCallqueue().CallLater(ReleaseLoop, 0, true);
	}
	void ReleaseLoop()
	{
		if (!m_CompartmentAccessComponent || !m_CompartmentAccessComponent.IsInCompartment())
			return;
			
		// Exit vehicle
		vector mat[4];
		Math3D.MatrixIdentity4(mat);
		mat[3][1] = 0.5;
		m_CompartmentAccessComponent.MoveOutVehicle(0, mat);
		GetGame().GetCallqueue().Remove(ReleaseLoop);
	}
	
	void DeleteVehicle(IEntity targetEntity, BaseCompartmentManagerComponent manager, int mgrID, int slotID, bool move)
	{
		m_CompartmentAccessComponent.GetOnCompartmentLeft().Remove(DeleteVehicle);
		SCR_EntityHelper.DeleteEntityAndChildren(this);
	}
}