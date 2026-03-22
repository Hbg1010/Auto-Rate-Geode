#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(AutoRateLayer, LevelInfoLayer) {

    struct Fields {
        bool m_hasChecked; // used to check if a level has been liked / evaluated already
    };

    void levelDownloadFinished(GJGameLevel* level) {
        LevelInfoLayer::levelDownloadFinished(level);
        autoRate();
    }

    void updateSideButtons() {
        if (!LevelInfoLayer::shouldDownloadLevel()) autoRate();
        LevelInfoLayer::updateSideButtons();
    }

    void rateStars() {
        if (m_level->m_stars > 0 || !m_starRateBtn || !m_starRateBtn->isEnabled()) return;

        int starVal = m_level->m_starsRequested;
        if (starVal == 0) {
            int avg = m_level->getAverageDifficulty();
            starVal = avg > 0 ? avg : utils::random::generate(1, 10);
        }

        GameLevelManager::get()->rateStars(m_level->m_levelID, starVal);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_starBtn2_001.png");
        m_starRateBtn->setSprite(spr);
        m_starRateBtn->setEnabled(false);
    }

    void rateDemon() {
        if (m_level->m_demon == 0 || !Mod::get()->getSettingValue<bool>("enableDemon") || !m_demonRateBtn || !m_demonRateBtn->isEnabled()) return;

        auto mode = Mod::get()->getSettingValue<std::string>("DemonRatingMethod");

        auto demonRate = mode == "Automatic" 
            ? demonAutoValue(m_level->m_demonDifficulty)
            : demonOverrideValue(mode);

        GameLevelManager::get()->rateDemon(m_level->m_levelID, demonRate, false);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_rateDiffBtn2_001.png");
        m_demonRateBtn->setSprite(spr);
        m_demonRateBtn->setEnabled(false);
    }

    void autoRate() {
        auto fields = m_fields.self();
        if (!Mod::get()->getSettingValue<bool>("enable") || fields->m_hasChecked) return;

        rateStars();
        rateDemon();
        
        fields->m_hasChecked = true;
    }

    // i hate this mapping so much dude just make it make sense 
    int demonAutoValue(int difficulty) {
        switch (difficulty) {
            case 3: return 1;
            case 4: return 2;
            case 5: return 4;
            case 6: return 5;
            case 0: return 3;
            default: return 3;
        }
    }

    // custom vals. I didn't wanna make a custom settings menu for an enum sry
    int demonOverrideValue(std::string_view value) {
        static const StringMap<int> overrides = {
            {"Easy", 1},
            {"Medium", 2},
            {"Hard", 3},
            {"Insane", 4},
            {"Extreme", 5},
            {"Automatic", 5}
        };
        auto iter = overrides.find(value);
        if (iter == overrides.end()) return 5;

        return iter->second;
    }
};