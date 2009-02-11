/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of COLLADAFramework.

    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#include "COLLADAMayaStableHeaders.h"
#include "COLLADAMayaEffectImporter.h"

#include "MayaDMLambert.h"


namespace COLLADAMaya
{

    const String EffectImporter::EFFECT_NAME = "Effect";

    //------------------------------
	EffectImporter::EffectImporter ( DocumentImporter* documentImporter )
        : BaseImporter ( documentImporter )
	{}
	
    //------------------------------
	EffectImporter::~EffectImporter()
	{}

    //------------------------------
    bool EffectImporter::importEffect ( const COLLADAFW::Effect* effect )
    {
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects ();
        size_t numCommonEffects = commonEffects.getCount ();
        for ( size_t i=0; i<numCommonEffects; ++i )
        {
            COLLADAFW::EffectCommon* commonEffect = commonEffects [i];
            const COLLADAFW::EffectCommon::ShaderType& shaderType = commonEffect->getShaderType ();
            switch ( shaderType )
            {
            case COLLADAFW::EffectCommon::SHADER_BLINN:
                break;
            case COLLADAFW::EffectCommon::SHADER_CONSTANT:
                break;
            case COLLADAFW::EffectCommon::SHADER_PHONG:
                break;
            case COLLADAFW::EffectCommon::SHADER_LAMBERT:
                importLambertShader ( effect );
                break;
            case COLLADAFW::EffectCommon::SHADER_UNKNOWN:
            default:
                // Standard shader (lambert?)
                break;
            }
        }

        return true;
    }

    //------------------------------
    void EffectImporter::importLambertShader ( const COLLADAFW::Effect* effect )
    {
        // Get the material name.
        String effectName ( effect->getName () );
        if ( COLLADABU::Utils::equals ( effectName, COLLADABU::Utils::EMPTY_STRING ) )
            effectName = EFFECT_NAME;
        effectName = mEffectIdList.addId ( effectName );

        const COLLADAFW::UniqueId& effectId = effect->getUniqueId ();
        mMayaEffectNamesMap [effectId] = effectName;

        const COLLADAFW::Color& color = effect->getStandardColor ();

        FILE* file = getDocumentImporter ()->getFile ();
        MayaDM::Lambert* lambert = new MayaDM::Lambert ( file, effectName );

        appendEffect ( effectId, lambert );

        // TODO
        // Find all geometries, which use this effect and create the connections.
        MaterialImporter* materialImporter = getDocumentImporter ()->getMaterialImporter ();
//        std::vector<COLLADAFW::UniqueId>& materials = materialImporter->findEffectMaterials ( effectId );
// 
//         // Iterate over the materials and get all geometries, which use it.
//         size_t numMaterials = materials.size ();
//         for ( size_t i=0; i<numMaterials; ++i )
//         {
//             COLLADAFW::UniqueId& materialId = materials [i];
// 
//             // Get the geometries, which use this material
//             VisualSceneImporter* visualSceneImporter = getDocumentImporter ()->getVisualSceneImporter ();
//             visualSceneImporter->f
//         }

    }

    // --------------------------
    MayaDM::DependNode* EffectImporter::findMayaEffect ( const COLLADAFW::UniqueId& val ) const
    {
        UniqueIdMayaDependNodeMap::const_iterator it = mMayaEffectMap.find ( val );
        if ( it != mMayaEffectMap.end () )
        {
            return it->second;
        }
        return 0;
    }

    // --------------------------
    void EffectImporter::appendEffect ( const COLLADAFW::UniqueId& id, MayaDM::DependNode* effectNode )
    {
        mMayaEffectMap [id] = effectNode;
    }

} // namespace COLLADAMaya