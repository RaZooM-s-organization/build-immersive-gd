#include "../videoTools/CameraMan.hpp"

#include "../popups/SelectDevicePopup.hpp"

class SelectCameraSetting : public SettingBaseValueV3<std::string> {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SelectCameraSetting>();
        auto root = checkJson(json, "SelectCameraSetting");
        res->parseBaseProperties(key, modID, root);

        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    SettingNodeV3* createNode(float width) override;
};


class SelectCameraSettingNodeV3 : public SettingValueNodeV3<SelectCameraSetting> {
protected:
    CCLabelBMFont* m_label{};

    bool init(std::shared_ptr<SelectCameraSetting> setting, float wid) {
        if (!SettingValueNodeV3::init(setting, wid)) return false;

        auto menu = getButtonMenu();
        menu->setContentWidth(160);

        m_label = CCLabelBMFont::create("", "bigFont.fnt");
        menu->addChild(m_label);

        auto plus = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
        plus->setScale(0.75);
        auto btn = CCMenuItemSpriteExtra::create(
            plus, this, menu_selector(SelectCameraSettingNodeV3::openDevicePopup)
        );
        menu->addChild(btn);

        menu->setLayout(RowLayout::create()->setGap(5)->setAutoScale(false)->setAxisAlignment(AxisAlignment::End));
        
        updateState(nullptr);
        
        return true;
    }

    void updateState(CCNode* invoker) override {
        SettingValueNodeV3::updateState(invoker);
        m_label->setString(getValue().c_str());
        float limit = getButtonMenu()->getContentWidth() - 25;
        m_label->limitLabelWidth(limit, 0.5, 0.1);
        getButtonMenu()->updateLayout();
    }
    

    void openDevicePopup(CCObject* sender) {
        SelectDevicePopup::create({
            getValue(),
            [this](std::string v){
                setValue(v, nullptr);
            }
        })->show();
    }

public:
    static SelectCameraSettingNodeV3* create(std::shared_ptr<SelectCameraSetting> setting, float width) {
        auto ret = new SelectCameraSettingNodeV3();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

SettingNodeV3* SelectCameraSetting::createNode(float width) {
    return SelectCameraSettingNodeV3::create(
        std::static_pointer_cast<SelectCameraSetting>(shared_from_this()),
        width
    );
}

$execute {
    (void) Mod::get()->registerCustomSettingType("select-camera", &SelectCameraSetting::parse);
}