#include "Settings.hpp"

$on_mod(Loaded) {
    ModSettings::get().pull();

    listenForAllSettingChangesV3([](std::shared_ptr<SettingV3> idk){
        ModSettings::get().pull();
    });
}
