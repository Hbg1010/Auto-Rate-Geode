#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <random>

using namespace geode::prelude;

#define GET_SETTING_BOOL(mod, key) mod->getSettingValue<bool>(key)
#define GET_SETTING_STRING(mod, key) mod->getSettingValue<std::string>(key)

class $modify(AutoRateLayer, LevelInfoLayer) {
    struct Fields {
        bool hasChecked; // used to check if a level has been liked / evaluated already
    };

    void levelDownloadFinished(GJGameLevel* p0) {
        LevelInfoLayer::levelDownloadFinished(p0);
        AutoRate();
    }

    void updateSideButtons() {
        if (!LevelInfoLayer::shouldDownloadLevel()) AutoRate();
        LevelInfoLayer::updateSideButtons();

    }

    void AutoRate() {
        Mod* modPointer = Mod::get();
        if (GET_SETTING_BOOL(modPointer, "enable") && !m_fields->hasChecked) {
            // unrated
            if (!m_level->m_stars.value() && m_starRateBtn && m_starRateBtn->isEnabled()) {
                RateStarsLayer* rsl = RateStarsLayer::create(m_level->m_levelID, m_level->isPlatformer(), false);
                int starVal = m_level->m_starsRequested;
                if (!starVal) {
                    int avg = m_level->getAverageDifficulty();
                    starVal = avg > 0 ? avg : AutoRateLayer::randInt();
                }

                rsl->m_starsRate = starVal;
                rsl->onRate(rsl->m_submitButton);
                CC_SAFE_DELETE(rsl);
                auto spr = CCSprite::createWithSpriteFrameName("GJ_starBtn2_001.png");
                m_starRateBtn->setSprite(spr);
                m_starRateBtn->m_bEnabled = false;

            // demon rates
            } else if (m_level->m_demon.value() && GET_SETTING_BOOL(modPointer, "enableDemon") && m_demonRateBtn && m_demonRateBtn->isEnabled()) {
                RateDemonLayer* RDL = RateDemonLayer::create(m_level->m_levelID);
                std::string mode = GET_SETTING_STRING(modPointer, "DemonRatingMethod");
                RDL->m_demonRate = mode == "Automatic" 
                    ? DemonAutoVal(m_level->m_demonDifficulty)
                    : DemonOverrideVal(mode);
                RDL->onRate(RDL->m_submitButton);
                CC_SAFE_DELETE(RDL);
                auto spr = CCSprite::createWithSpriteFrameName("GJ_rateDiffBtn2_001.png");
                m_demonRateBtn->setSprite(spr);
                m_demonRateBtn->m_bEnabled = false;
            }
        }
        m_fields->hasChecked = true;
    }

    // for those pesky "I didn't request a rating" guys (You know who you are) with 0 player submitted rating
    int randInt() {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> distrib(1, 10);
        return distrib(gen);
    }

    // i hate this mapping so much dude just make it make sense 
    int DemonAutoVal (int difficulty) {
        switch (difficulty)
        {
            case 3: return 1;
            case 4: return 2;
            case 5: return 4;
            case 6: return 5;
            case 0: return 3;
            default: return 3;
        }
    }

    // custom vals. I didn't wanna make a custom settings menu for an enum sry
    int DemonOverrideVal(std::string& val) {
        if (val == "Easy") {
            return 1; 
        } else if (val == "Medium") {
            return 2;
        } else if (val == "Hard") {
            return 3;
        } else if (val == "Insane") {
            return 4;
        } else {
            return 5;
        }
    }
};