#include <Geode/Geode.hpp>
#include <random>

using namespace geode::prelude;

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(AutoRateLayer, LevelInfoLayer) {
    void levelDownloadFinished(GJGameLevel* p0) {
        LevelInfoLayer::levelDownloadFinished(p0);

        if (Mod::get()->getSettingValue<bool>("enable") && !p0->m_stars.value() && m_starRateBtn && m_starRateBtn->isEnabled()) {
            RateStarsLayer* rsl = RateStarsLayer::create(m_level->m_levelID, p0->isPlatformer(), false);
            int number = p0->m_starsRequested;
            if (!number) {
                int avg = p0->getAverageDifficulty();
                number = avg > 0 ? avg : AutoRateLayer::randInt();
            }

            rsl->m_starsRate = number;
            rsl->onRate(rsl->m_submitButton);
        }
    }

    // for those pesky "I didn't request a rating" guys (You know who you are) with 0 player submitted rating
    int randInt() {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> distrib(1, 10);
        return distrib(gen);
    }
};

// #include <Geode/modify/RateStarsLayer.hpp>
// class $modify(rs, RateStarsLayer) {
//     void onRate(CCObject* sender) {
//         RateStarsLayer::onRate(sender);
//         log::debug("{}", m_starsRate);
//     }
// };