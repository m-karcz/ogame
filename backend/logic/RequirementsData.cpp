#include "RequirementsData.hpp"
#include "Constants.hpp"

namespace
{
    template<int ResLabLevel>
    Requirement onLab(std::vector<LeveledResearch> others = {})
    {
        return {{{ResLabLevel, cnst::researchLab}},
                others};
    }
}

RequirementsData::RequirementsData()
{
    buildings[cnst::metalMine] = {};
    buildings[cnst::crystalMine] = {};
    buildings[cnst::deuteriumSynthesizer] = {};
    buildings[cnst::roboticsFactory] = {};
    buildings[cnst::shipyard] = {{{2, cnst::roboticsFactory}}, {{}}};
    buildings[cnst::naniteFactory] = {{{10, cnst::roboticsFactory}},
                                {{10, cnst::computerTechnology}}};
    buildings[cnst::metalStorage] = {};
    buildings[cnst::crystalStorage] = {};
    buildings[cnst::deuteriumTank] = {};
    buildings[cnst::researchLab] = {};
    buildings[cnst::alianceDepot] = {};
    buildings[cnst::missileSilo] = {{{1, cnst::shipyard}}, {{}}};
    buildings[cnst::terraformer] = {{{1, cnst::naniteFactory}},
                                    {{12, cnst::energy}}};

    researchs[cnst::espionage] = onLab<3>();
    researchs[cnst::computerTechnology] = onLab<1>();
    researchs[cnst::weapons] = onLab<4>();
    researchs[cnst::shielding] = onLab<6>({{3, cnst::energy}});
    researchs[cnst::armour] = onLab<2>();
    researchs[cnst::energy] = onLab<1>();
    researchs[cnst::hyperspace] = onLab<7>({{5, cnst::shielding},
                                            {5, cnst::energy}});
    researchs[cnst::combustionDrive] = onLab<1>({{1, cnst::energy}});
    researchs[cnst::impulseDrive] = onLab<1>({{2, cnst::energy}});
    researchs[cnst::hyperspaceDrive] = onLab<7>({{3, cnst::hyperspace}});
    researchs[cnst::laser] = onLab<1>({{2, cnst::energy}});
    researchs[cnst::ion] = onLab<4>({{4, cnst::energy},
                                     {5, cnst::laser}});
    researchs[cnst::plasma] = onLab<5>({{8, cnst::energy},
                                        {10, cnst::laser},
                                        {5, cnst::ion}});
    researchs[cnst::intergalactic] = onLab<10>({{8, cnst::computerTechnology},
                                                {8, cnst::hyperspace}});
    researchs[cnst::graviton] = onLab<12>();


}
