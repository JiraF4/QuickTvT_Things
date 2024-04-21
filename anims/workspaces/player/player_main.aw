BaseSource {
 AnimSetTemplate "{CD0E6C3490BF0847}anims/workspaces/player/player_main.ast"
 AnimSetInstances {
  "{32965FEFBB738E5A}anims/workspaces/player/player_main.asi"
  "{0350283642830782}anims/workspaces/player/player_main_rifle.asi"
  "{41AABAE44257A657}anims/workspaces/player/player_main_pistol.asi"
  "{81E91C367B2A22D6}anims/workspaces/player/player_main_rpg.asi"
  "{60945F10BF15FC09}anims/workspaces/player/player_main_machinegun.asi"
  "{4FCBEA45CAEBC9C0}anims/workspaces/player/player_main_1h.asi"
  "{5243F3D5D0BEE57E}anims/workspaces/player/player_main_m72.asi"
  "{AF06D3F738A50403}anims/workspaces/player/player_main_heavy.asi"
 }
 AnimGraph "{8952C094E1315221}anims/workspaces/player/player_main.agr"
 PreviewModels {
  AnimSrcWorkspacePreviewModel "{563EF338E13AB791}" {
   Model "{790CB9C809DE64B8}Assets/Characters/Animation/AnimTestChar_US_01.xob"
  }
  AnimSrcWorkspacePreviewModel "{563EF338E13AB5EA}" {
   Model "{7297BDBDE223627F}Assets/Characters/Animation/AnimTestChar_USSR_01.xob"
  }
 }
 ChildPreviewModels {
  AnimSrcWorkspaceChildPreviewModel "{5A44BACB00F067DA}" {
   Enabled 0
   Model "{BA5449C606206A19}Assets/Weapons/Rifles/AK74/AK74_body.xob"
   Bone "RightHandProp"
   Parent "{563EF338E13AB791}"
   ChildBone "w_root"
  }
  AnimSrcWorkspaceChildPreviewModel "{5A76ED170BD46D3E}" {
   Model "{6F3FF53AF2AB04B3}Assets/Weapons/Handguns/PM/PM_body.xob"
   Bone "RightHandProp"
   Parent "{563EF338E13AB791}"
  }
 }
 AudioTesting AnimSrcWorkspaceAudioTesting "{58749017CEBF9BA9}" {
  AudioProjects {
   AnimSrcWorkspaceAudioProject "{5E234195C325AD11}" {
    ProjectFile "{67007BA2E4D81B0D}Sounds/Character/Character_Movement_Foley_Additive.acp"
   }
   AnimSrcWorkspaceAudioProject "{5E2341953E5B3F34}" {
    ProjectFile "{81D32DC765A2B68D}Sounds/Character/Character_Movement_Foley_Locomotion.acp"
   }
   AnimSrcWorkspaceAudioProject "{5E234195397F3DD1}" {
    ProjectFile "{413F56BD2A1737CD}Sounds/Character/Character_Items.acp"
   }
   AnimSrcWorkspaceAudioProject "{5E234195638720A9}" {
    ProjectFile "{4D7D189EAFF7AC12}Sounds/Character/Character_Footsteps.acp"
   }
   AnimSrcWorkspaceAudioProject "{5E64854D4DC18F6D}" {
    ProjectFile "{E59C77F94B1AE155}Sounds/Character/Character_Jump_Landing.acp"
   }
   AnimSrcWorkspaceAudioProject "{5E64854D4ABDF12D}" {
    ProjectFile "{FC1845C71FCA50CC}Sounds/Character/Character_Prone.acp"
   }
  }
 }
 EventTable "{6BEC82B356748306}anims/workspaces/player/player_EventTable.ae"
 SyncTable "{D452A6E92D5CF3F1}anims/workspaces/player/Player_SyncTable.asy"
 AttachmentTesting AnimSrcWorkspaceAttachmentTesting attachmentTesting {
  Attachments {
   AnimSrcWorkspaceAttachmentTest "{51E07513BF670C04}" {
    Enabled 0
    BindingName "Grenade"
    AnimGraph "{3555B2B751CE338E}Assets/Weapons/Grenades/workspaces/Grenade_M67.agr"
    AnimGraphNode "MasterControl"
    AnimInstance "{6D0438DAFBBBA1ED}Assets/Weapons/Grenades/workspaces/Player_M67.asi"
   }
   AnimSrcWorkspaceAttachmentTest "{51E07513BF670C49}" {
    Enabled 0
    BindingName "Vehicle"
    AnimGraph "{66618A6A119CAD93}Assets/Vehicles/Wheeled/workspaces/Vehicles_Wheeled_Graph.agr"
    AnimGraphNode "MasterControl"
    AnimInstance "{C44A74EBEB5C50DB}Assets/Vehicles/Wheeled/workspaces/Player_Wheeled_UAZ.asi"
   }
   AnimSrcWorkspaceAttachmentTest "{5496D8488740E129}" {
    Enabled 0
    BindingName "Gadget"
    AnimGraph "{883777E45A6C4DA1}Assets/Items/Medicine/Workspaces/Gauze_01.agr"
    AnimGraphNode "MasterControl"
    AnimInstance "{79BD98038DD60F88}Assets/Items/Medicine/Workspaces/Gauze_01_player.asi"
   }
   AnimSrcWorkspaceAttachmentTest "{51E07513BF670C7F}" {
    Enabled 1
    BindingName "Weapon"
    AnimGraph "{ADDF1CC42A489D84}Assets/Weapons/Rifles/workspaces/ak74.agr"
    AnimGraphNode "MasterControl"
    AnimInstance "{F97D8192D39B92FD}Assets/Weapons/Rifles/workspaces/ak74_player.asi"
   }
  }
 }
 IkTesting AnimSrcWorkspaceIkTesting "{52928355A81E5FAE}" {
  IkTargets {
   AnimSrcWorkspaceIkTargetTest "{52ACCA55AEBEECBE}" {
    Enabled 0
    BindingName "RFootIKTarget"
    Position 0.2 0.23 0
    Rotation 0 3.24 0.1
   }
   AnimSrcWorkspaceIkTargetTest "{52B6C1642C518E0E}" {
    Enabled 0
    BindingName "LFootIKTarget"
    Position -0.2 0.23 0
    Rotation 4 3.24 0.1
   }
  }
  IkPlanes {
   AnimSrcWorkspaceIkTargetPlaneTest "{55DEF0D15A324261}" {
    Enabled 0
    BindingName "FloorRight"
    Position 0 1 0
    Normal 0 1 0
   }
  }
 }
}