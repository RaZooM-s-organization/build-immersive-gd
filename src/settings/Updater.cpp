#include "Settings.hpp"

$on_mod(Loaded) {
    Settings::get().pull();

    listenForAllSettingChangesV3([](std::shared_ptr<SettingV3> idk){
        Settings::get().pull();
    });
}
