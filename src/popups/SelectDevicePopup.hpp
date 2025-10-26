#pragma once


#include <Geode/Geode.hpp>
using namespace geode::prelude;


struct SelectDevicePopupData {
    std::string m_current;
    std::function<void(std::string)> m_callback;
};

class SelectDevicePopup : public Popup<SelectDevicePopupData> {
protected:
    const float m_width = 250.f;
    const float m_height = 120.f;

    SelectDevicePopupData m_data;

public:
    static SelectDevicePopup* create(SelectDevicePopupData data);
    bool setup(SelectDevicePopupData data) override;
    void onClose(CCObject* obj) override;
};


