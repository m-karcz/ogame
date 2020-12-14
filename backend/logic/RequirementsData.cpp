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
    buildings[cnst::deuterExtractor] = {};
    buildings[cnst::robots] = {};
    buildings[cnst::shipyard] = {{{2, cnst::robots}}, {{}}};
    buildings[cnst::nanites] = {{{10, cnst::robots}},
                                {{10, cnst::computer}}};
    buildings[cnst::metalStorage] = {};
    buildings[cnst::crystalStorage] = {};
    buildings[cnst::deuterStorage] = {};
    buildings[cnst::researchLab] = {};
    buildings[cnst::alianceDepot] = {};
    buildings[cnst::missileSilo] = {{{1, cnst::shipyard}}, {{}}};
    buildings[cnst::terraformer] = {{{1, cnst::nanites}},
                                    {{12, cnst::energy}}};

    researchs[cnst::espionage] = onLab<3>();
    researchs[cnst::computer] = onLab<1>();
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
    researchs[cnst::intergalactic] = onLab<10>({{8, cnst::computer},
                                                {8, cnst::hyperspace}});
    researchs[cnst::graviton] = onLab<12>();


}
