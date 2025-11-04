#include "Pose.hpp"

#include <map>

static const std::map<IconType, std::vector<PosePrototype>> poseData = {
    {
        IconType::Cube, {
            PosePrototype {
                PoseId::Cube_Up,
                "cube.jump",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.300392},
                    {Pose::Bone::Bone_7_9, 1.119343},
                    {Pose::Bone::Bone_6_8, 3.985747},
                    {Pose::Bone::Bone_8_10, 2.022249},
                }},
                {
                    {PoseId::Any, PlayerAction::Hold}
                }
            }
        }
    }, {
        IconType::Ship, {
            PosePrototype {
                PoseId::Ship_Up,
                "ship.up",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 0.700855},
                    {Pose::Bone::Bone_7_9, 0.844154},
                    {Pose::Bone::Bone_6_8, 2.629203},
                    {Pose::Bone::Bone_8_10, 2.188941},
                }},
                {
                    {PoseId::Any, PlayerAction::Hold}
                }
            },
            PosePrototype {
                PoseId::Ship_Down,
                "ship.down",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.439031},
                    {Pose::Bone::Bone_7_9, 5.564355},
                    {Pose::Bone::Bone_6_8, 3.908811},
                    {Pose::Bone::Bone_8_10, 3.799926},
                }},
                {
                    {PoseId::Any, PlayerAction::None}
                }
            }
    
        }
    }, {
        IconType::Ball, {
            PosePrototype {
                PoseId::Ball_Flip,
                "ball.flip",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.300392},
                    {Pose::Bone::Bone_7_9, 1.119343},
                    {Pose::Bone::Bone_6_8, 3.985747},
                    {Pose::Bone::Bone_8_10, 2.022249},
                }},
                {
                    {PoseId::Any, PlayerAction::Hold}
                }
            }
        }
    }, {
        IconType::Ufo, {
            PosePrototype {
                PoseId::Ufo_Up,
                "ufo.prepare_jump",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 0.382045},
                    {Pose::Bone::Bone_7_9, 1.035841},
                    {Pose::Bone::Bone_6_8, 2.782822},
                    {Pose::Bone::Bone_8_10, 2.105752},
                }},
                {
                    {PoseId::Any, PlayerAction::None}
                }
            },
            PosePrototype {
                PoseId::Ufo_Down,
                "ufo.jump",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.330533},
                    {Pose::Bone::Bone_7_9, 5.330534},
                    {Pose::Bone::Bone_6_8, 4.094245},
                    {Pose::Bone::Bone_8_10, 4.094245},
                }},
                {
                    {PoseId::Ufo_Up, PlayerAction::Click}
                }
            }
    
        }
    }, {
        IconType::Wave, {
            PosePrototype {
                PoseId::Wave_Up,
                "wave.up",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 0.844154},
                    {Pose::Bone::Bone_7_9, 0.952652},
                    {Pose::Bone::Bone_6_8, 2.570255},
                    {Pose::Bone::Bone_8_10, 2.297439},
                }},
                {
                    {PoseId::Any, PlayerAction::Hold}
                }
            },
            PosePrototype {
                PoseId::Wave_Down,
                "wave.down",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.373432},
                    {Pose::Bone::Bone_7_9, 5.363465},
                    {Pose::Bone::Bone_6_8, 3.985747},
                    {Pose::Bone::Bone_8_10, 3.953600},
                }},
                {
                    {PoseId::Any, PlayerAction::None}
                }
            }
        }
    }, {
        IconType::Robot, {
            PosePrototype {
                PoseId::Robot_Up,
                "robot.jump",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.300392},
                    {Pose::Bone::Bone_7_9, 1.119343},
                    {Pose::Bone::Bone_6_8, 3.985747},
                    {Pose::Bone::Bone_8_10, 2.022249},
                }},
                {
                    {PoseId::Any, PlayerAction::Hold}
                }
            }
        }
    }, {
        IconType::Spider, {
            PosePrototype {
                PoseId::Spider_Up,
                "spider.teleport_1",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 0.010000},
                    {Pose::Bone::Bone_7_9, 1.688764},
                    {Pose::Bone::Bone_6_8, 3.253622},
                    {Pose::Bone::Bone_8_10, 1.681454},
                }},
                {
                    {PoseId::Spider_Down, PlayerAction::Click}
                }
            },
            PosePrototype {
                PoseId::Spider_Down,
                "spider.teleport_2",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 6.075295},
                    {Pose::Bone::Bone_7_9, 4.712389},
                    {Pose::Bone::Bone_6_8, 3.265948},
                    {Pose::Bone::Bone_8_10, 4.838697},
                }},
                {
                    {PoseId::Spider_Up, PlayerAction::Click}
                }
            }
        }
    }, {
        IconType::Swing, {
            PosePrototype {
                PoseId::Swing_Flip,
                "swing.flip",
                Pose {{ 
                    {Pose::Bone::Bone_5_7, 5.300392},
                    {Pose::Bone::Bone_7_9, 1.119343},
                    {Pose::Bone::Bone_6_8, 3.985747},
                    {Pose::Bone::Bone_8_10, 2.022249},
                }},
                {
                    {PoseId::Any, PlayerAction::Click}
                }
            }
        }
    }, 
};


std::map<IconType, std::vector<PosePrototype>> const& getPoseData() {
    return poseData;
}

