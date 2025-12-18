#include "shadow.hpp"

#include <osgShadow/ShadowedScene>

#include <components/misc/stringops.hpp>
#include <components/settings/settings.hpp>

namespace SceneUtil
{
    using namespace osgShadow;

    void ShadowManager::setupShadowSettings()
    {
        mEnableShadows = Settings::Manager::getBool("enable shadows", "Shadows");

        if (!mEnableShadows)
        {
            mShadowTechnique->disableShadows();
            return;
        }
        
        mShadowTechnique->enableShadows();

        mShadowSettings->setLightNum(0);
        mShadowSettings->setReceivesShadowTraversalMask(~0u);

        int numberOfShadowMapsPerLight = Settings::Manager::getInt("number of shadow maps", "Shadows");
        numberOfShadowMapsPerLight = std::max(1, std::min(numberOfShadowMapsPerLight, 8));

        mShadowSettings->setNumShadowMapsPerLight(numberOfShadowMapsPerLight);
        mShadowSettings->setBaseShadowTextureUnit(8 - numberOfShadowMapsPerLight);

        // Store all three shadow distances
        mOutdoorShadowDistance = Settings::Manager::getFloat("maximum shadow map distance", "Shadows");

        mIndoorShadowDistance = Settings::Manager::getFloat("indoor maximum shadow map distance", "Shadows");
        if (mIndoorShadowDistance <= 0.0f)
            mIndoorShadowDistance = mOutdoorShadowDistance;

        mQuasiExShadowDistance = Settings::Manager::getFloat("quasiex maximum shadow map distance", "Shadows");
        
        if (mQuasiExShadowDistance == 0.0f)
            mQuasiExShadowDistance = mOutdoorShadowDistance;

        // Apply outdoor distance initially (we start in outdoor mode)
        if (mOutdoorShadowDistance > 0)
        {
            const float shadowFadeStart = std::min(std::max(0.f, Settings::Manager::getFloat("shadow fade start", "Shadows")), 1.f);
            mShadowSettings->setMaximumShadowMapDistance(mOutdoorShadowDistance);
            mShadowTechnique->setShadowFadeStart(mOutdoorShadowDistance * shadowFadeStart);
            mShadowTechnique->setMaximumShadowMapDistance(mOutdoorShadowDistance);
        }

        mShadowSettings->setMinimumShadowMapNearFarRatio(Settings::Manager::getFloat("minimum lispsm near far ratio", "Shadows"));

        std::string computeSceneBounds = Settings::Manager::getString("compute scene bounds", "Shadows");
        if (Misc::StringUtils::lowerCase(computeSceneBounds) == "primitives")
            mShadowSettings->setComputeNearFarModeOverride(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
        else if (Misc::StringUtils::lowerCase(computeSceneBounds) == "bounds")
            mShadowSettings->setComputeNearFarModeOverride(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);

        int mapres = Settings::Manager::getInt("shadow map resolution", "Shadows");
        mShadowSettings->setTextureSize(osg::Vec2s(mapres, mapres));

        mShadowTechnique->setSplitPointUniformLogarithmicRatio(Settings::Manager::getFloat("split point uniform logarithmic ratio", "Shadows"));
        mShadowTechnique->setSplitPointDeltaBias(Settings::Manager::getFloat("split point bias", "Shadows"));

        mShadowTechnique->setPolygonOffset(Settings::Manager::getFloat("polygon offset factor", "Shadows"), Settings::Manager::getFloat("polygon offset units", "Shadows"));

        if (Settings::Manager::getBool("use front face culling", "Shadows"))
            mShadowTechnique->enableFrontFaceCulling();
        else
            mShadowTechnique->disableFrontFaceCulling();

        if (Settings::Manager::getBool("allow shadow map overlap", "Shadows"))
            mShadowSettings->setMultipleShadowMapHint(osgShadow::ShadowSettings::CASCADED);
        else
            mShadowSettings->setMultipleShadowMapHint(osgShadow::ShadowSettings::PARALLEL_SPLIT);

        if (Settings::Manager::getBool("enable debug hud", "Shadows"))
            mShadowTechnique->enableDebugHUD();
        else
            mShadowTechnique->disableDebugHUD();
    }

    void ShadowManager::disableShadowsForStateSet(osg::ref_ptr<osg::StateSet> stateset)
    {
        if (!Settings::Manager::getBool("enable shadows", "Shadows"))
            return;

        int numberOfShadowMapsPerLight = Settings::Manager::getInt("number of shadow maps", "Shadows");
        numberOfShadowMapsPerLight = std::max(1, std::min(numberOfShadowMapsPerLight, 8));

        int baseShadowTextureUnit = 8 - numberOfShadowMapsPerLight;
        
        osg::ref_ptr<osg::Image> fakeShadowMapImage = new osg::Image();
        fakeShadowMapImage->allocateImage(1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
        *(float*)fakeShadowMapImage->data() = std::numeric_limits<float>::infinity();
        osg::ref_ptr<osg::Texture> fakeShadowMapTexture = new osg::Texture2D(fakeShadowMapImage);
        fakeShadowMapTexture->setShadowComparison(true);
        fakeShadowMapTexture->setShadowCompareFunc(osg::Texture::ShadowCompareFunc::ALWAYS);
        for (int i = baseShadowTextureUnit; i < baseShadowTextureUnit + numberOfShadowMapsPerLight; ++i)
        {
            stateset->setTextureAttributeAndModes(i, fakeShadowMapTexture, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
            stateset->addUniform(new osg::Uniform(("shadowTexture" + std::to_string(i - baseShadowTextureUnit)).c_str(), i));
            stateset->addUniform(new osg::Uniform(("shadowTextureUnit" + std::to_string(i - baseShadowTextureUnit)).c_str(), i));
        }
    }

    ShadowManager::ShadowManager(osg::ref_ptr<osg::Group> sceneRoot, osg::ref_ptr<osg::Group> rootNode, unsigned int outdoorShadowCastingMask, unsigned int indoorShadowCastingMask, Shader::ShaderManager &shaderManager) : mShadowedScene(new osgShadow::ShadowedScene),
        mShadowTechnique(new MWShadowTechnique),
        mOutdoorShadowCastingMask(outdoorShadowCastingMask),
        mIndoorShadowCastingMask(indoorShadowCastingMask)
    {
        mShadowedScene->setShadowTechnique(mShadowTechnique);

        mShadowedScene->addChild(sceneRoot);
        rootNode->addChild(mShadowedScene);
        mShadowedScene->setNodeMask(sceneRoot->getNodeMask());

        mShadowSettings = mShadowedScene->getShadowSettings();
        setupShadowSettings();

        mShadowTechnique->setupCastingShader(shaderManager);

        enableOutdoorMode();
    }

    Shader::ShaderManager::DefineMap ShadowManager::getShadowDefines()
    {
        if (!mEnableShadows)
            return getShadowsDisabledDefines();

        Shader::ShaderManager::DefineMap definesWithShadows;

        definesWithShadows["shadows_enabled"] = "1";

        for (unsigned int i = 0; i < mShadowSettings->getNumShadowMapsPerLight(); ++i)
            definesWithShadows["shadow_texture_unit_list"] += std::to_string(i) + ",";
        // remove extra comma
        definesWithShadows["shadow_texture_unit_list"] = definesWithShadows["shadow_texture_unit_list"].substr(0, definesWithShadows["shadow_texture_unit_list"].length() - 1);

        definesWithShadows["shadowMapsOverlap"] = Settings::Manager::getBool("allow shadow map overlap", "Shadows") ? "1" : "0";

        definesWithShadows["useShadowDebugOverlay"] = Settings::Manager::getBool("enable debug overlay", "Shadows") ? "1" : "0";

        // switch this to reading settings if it's ever exposed to the user
        definesWithShadows["perspectiveShadowMaps"] = mShadowSettings->getShadowMapProjectionHint() == ShadowSettings::PERSPECTIVE_SHADOW_MAP ? "1" : "0";

        definesWithShadows["disableNormalOffsetShadows"] = Settings::Manager::getFloat("normal offset distance", "Shadows") == 0.0 ? "1" : "0";

        definesWithShadows["shadowNormalOffset"] = std::to_string(Settings::Manager::getFloat("normal offset distance", "Shadows"));

        definesWithShadows["limitShadowMapDistance"] = Settings::Manager::getFloat("maximum shadow map distance", "Shadows") > 0 ? "1" : "0";

        return definesWithShadows;
    }

    Shader::ShaderManager::DefineMap ShadowManager::getShadowsDisabledDefines()
    {
        Shader::ShaderManager::DefineMap definesWithoutShadows;

        definesWithoutShadows["shadows_enabled"] = "0";

        definesWithoutShadows["shadow_texture_unit_list"] = "";

        definesWithoutShadows["shadowMapsOverlap"] = "0";

        definesWithoutShadows["useShadowDebugOverlay"] = "0";

        definesWithoutShadows["perspectiveShadowMaps"] = "0";

        definesWithoutShadows["disableNormalOffsetShadows"] = "0";

        definesWithoutShadows["shadowNormalOffset"] = "0.0";

        definesWithoutShadows["limitShadowMapDistance"] = "0";

        return definesWithoutShadows;
    }

    void ShadowManager::enableIndoorMode()
    {
        if (Settings::Manager::getBool("enable indoor shadows", "Shadows"))
        {
            mShadowSettings->setCastsShadowTraversalMask(mIndoorShadowCastingMask);

            // Apply indoor shadow distance
            if (mIndoorShadowDistance > 0)
            {
                const float shadowFadeStart = std::min(std::max(0.f, Settings::Manager::getFloat("shadow fade start", "Shadows")), 1.f);

                // Set in mShadowSettings so it's used during cull traversal
                mShadowSettings->setMaximumShadowMapDistance(mIndoorShadowDistance);

                // Also update the shader uniforms
                mShadowTechnique->setShadowFadeStart(mIndoorShadowDistance * shadowFadeStart);
                mShadowTechnique->setMaximumShadowMapDistance(mIndoorShadowDistance);
            }
        }
        else
            mShadowTechnique->disableShadows(true);
    }

    void ShadowManager::enableOutdoorMode()
    {
        if (mEnableShadows)
            mShadowTechnique->enableShadows();

        mShadowSettings->setCastsShadowTraversalMask(mOutdoorShadowCastingMask);

        // ADDED: Apply outdoor shadow distance
        if (mOutdoorShadowDistance > 0)
        {
            const float shadowFadeStart = std::min(std::max(0.f, Settings::Manager::getFloat("shadow fade start", "Shadows")), 1.f);

            // set in mShadowSettings so it's used during cull traversal
            mShadowSettings->setMaximumShadowMapDistance(mOutdoorShadowDistance);

            // Also update the shader uniforms
            mShadowTechnique->setShadowFadeStart(mOutdoorShadowDistance * shadowFadeStart);
            mShadowTechnique->setMaximumShadowMapDistance(mOutdoorShadowDistance);
        }
    }

    void ShadowManager::enableQuasiExMode()
    {
        if (mEnableShadows)
            mShadowTechnique->enableShadows();

        // QuasiEx uses outdoor shadow quality (casting mask) but with custom distance
        mShadowSettings->setCastsShadowTraversalMask(mOutdoorShadowCastingMask);

        // Apply QuasiEx shadow distance
        if (mQuasiExShadowDistance > 0)
        {
            const float shadowFadeStart = std::min(std::max(0.f, Settings::Manager::getFloat("shadow fade start", "Shadows")), 1.f);
            mShadowSettings->setMaximumShadowMapDistance(mQuasiExShadowDistance);
            mShadowTechnique->setShadowFadeStart(mQuasiExShadowDistance * shadowFadeStart);
            mShadowTechnique->setMaximumShadowMapDistance(mQuasiExShadowDistance);
        }
        else if (mQuasiExShadowDistance < 0)
        {
            // Negative value (like -1): infinite distance - set to a very large value
            const float shadowFadeStart = std::min(std::max(0.f, Settings::Manager::getFloat("shadow fade start", "Shadows")), 1.f);
            const float infiniteDistance = 1000000.0f;  // Effectively infinite
            mShadowSettings->setMaximumShadowMapDistance(infiniteDistance);
            mShadowTechnique->setShadowFadeStart(infiniteDistance * shadowFadeStart);
            mShadowTechnique->setMaximumShadowMapDistance(infiniteDistance);
        }
    }
}
