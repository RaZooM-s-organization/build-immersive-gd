#include "Pose.hpp"

#include <map>

static const std::map<IconType, std::vector<PosePrototype>> poseData = {
    {
        IconType::Cube, {
            PosePrototype {
                PoseId::Cube_ForearmsUp,
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
    
        }
    }, {
        IconType::Ball, {
    
        }
    }, {
        IconType::Ufo, {
    
        }
    }, {
        IconType::Wave, {
    
        }
    }, {
        IconType::Robot, {
    
        }
    }, {
        IconType::Spider, {
    
        }
    }, {
        IconType::Swing, {
    
        }
    }, 
};


std::map<IconType, std::vector<PosePrototype>> const& getPoseData() {
    return poseData;
}

