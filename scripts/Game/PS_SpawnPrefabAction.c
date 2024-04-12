class PS_SpawnPrefabAction : ScriptedUserAction
{	
	[Attribute("Create prefab")]
	protected string m_sName;
	[Attribute("{CD85ADE9E0F54679}PrefabsEditable/Markers/EditableMarker.et")]
	protected string m_sPrefab;
	[Attribute("0 0 0")]
	protected vector m_vPosition;
	
	protected SignalsManagerComponent m_SignalsManagerComponent;
	protected int m_iPrefabSpawnedSignal;
	protected int m_iPrefabSpawned;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_SignalsManagerComponent = SignalsManagerComponent.Cast(pOwnerEntity.FindComponent(SignalsManagerComponent));
		m_iPrefabSpawnedSignal = m_SignalsManagerComponent.AddOrFindMPSignal("PrefabSpawned", 0.0, 0.0, 0, SignalCompressionFunc.Bool);
	}
	
	override bool GetActionNameScript(out string outName) {
		outName = m_sName;
		return true;
	};
	
	override bool CanBeShownScript(IEntity user) { 
		int prefabSpawned = m_SignalsManagerComponent.GetSignalValue(m_iPrefabSpawnedSignal);
		return prefabSpawned == 0;
	};
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_iPrefabSpawned = m_SignalsManagerComponent.GetSignalValue(m_iPrefabSpawnedSignal);
		if (m_iPrefabSpawned == 1)
			return;
      Resource resource = Resource.Load(m_sPrefab);
		EntitySpawnParams params = new EntitySpawnParams();
		pOwnerEntity.GetTransform(params.Transform);
		if (m_vPosition != "0 0 0")
			params.Transform[3] = m_vPosition;
      GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		m_iPrefabSpawned = 1;
		m_SignalsManagerComponent.SetSignalValue(m_iPrefabSpawnedSignal, 1);
	}
}