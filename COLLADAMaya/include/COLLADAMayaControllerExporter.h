/*
    Copyright (c) 2008 NetAllied Systems GmbH

	This file is part of COLLADAMaya.

    Portions of the code are:
    Copyright (c) 2005-2007 Feeling Software Inc.
    Copyright (c) 2005-2007 Sony Computer Entertainment America
    Copyright (c) 2004-2005 Alias Systems Corp.
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#ifndef __COLLADA_MAYA_CONTROLLER_EXPORTER_H___
#define __COLLADA_MAYA_CONTROLLER_EXPORTER_H___

#include "COLLADAMayaStableHeaders.h"
#include "COLLADALibraryControllers.h"
#include "COLLADAMayaDocumentExporter.h"
#include "COLLADAMayaSceneElement.h"
#include "COLLADAMayaBaseController.h"
#include "COLLADAMayaSkinController.h"

#include <vector>
#include <map>

#include <maya/MFnGeometryFilter.h>
#include <maya/MFnMesh.h>


namespace COLLADAMaya
{

#define WEIGHT_TOLERANCE 0.001f

#if MAYA_API_VERSION >= 650
#define MWeight double
#define MWeightArray MDoubleArray
#else
#define MWeight float
#define MWeightArray MFloatArray
#endif


    /**
     * Holds any controller associated with the given plug.
     */
    struct ControllerStackItem
    {
        bool isSkin;
        MObject skinControllerNode;
        MObjectArray morphControllerNodes;
        std::vector<long> nodeStates;
    };
    typedef std::vector<ControllerStackItem*> ControllerStack;

    /**
    * Represents a mesh controller item.
    */
    struct ControllerMeshItem
    {
        MObject mesh;
        bool isIntermediate;
        bool isVisible;
    };
    typedef std::vector<ControllerMeshItem> ControllerMeshStack;

    /**
     * Class to control the skins and morphs
     */
    class ControllerExporter : public COLLADA::LibraryControllers
    {

    private:
        
        /** Parameter, used for the bind poses transform. */
        static const String PARAM_TYPE_TRANSFORM;

        /** Parameter, used for the joint. */
        static const String PARAM_TYPE_JOINT;

        /** Parameter, used for the weight. */
        static const String PARAM_TYPE_WEIGHT;

    private:

        /** Pointer to the document exporter. */
        DocumentExporter* mDocumentExporter;

        // A lookup table of elements we've already processed
        ControllerList importedMorphControllers;
        ControllerList skinControllers;

        /** List of controllerIds from the already exported controllers. */
        std::vector<String> mExportedControllers;

    public:

        /**
         * Constructor.
         * @param streamWriter
         * @param documentExporter
         */
        ControllerExporter ( COLLADA::StreamWriter* streamWriter, DocumentExporter* documentExporter );
        virtual ~ControllerExporter();

        /**
         * Exports all controllers in the current scene graph.
         */
        void exportControllers();

        /**
         * @todo documentation
         * @param sceneNode
         * @param dagPath
         * @param isSkin
         * @param instantiate
         */
        void exportController ( SceneElement* sceneNode,
                                bool isSkin,
                                bool instantiate=true );

        /**
         * @todo documentation
         * @param sceneNode
         * @param node
        * @return True, if the controller was exported.
         */
        bool exportController ( SceneElement* sceneNode, const MObject& node );

//   DaeEntity* exportMorphController(FCDSceneNode* sceneNode, MObjectArray& controllerNodes, DaeEntity* target);
//   DaeEntity* exportSkinController(MObject controllerNode, MDagPath outputShape, DaeEntity* target);
//   FCDGeometry* exportMorphTarget(MPlug& vertexListPlug, MPlug& targetComponentListPlug, uint targetIndex, FCDGeometry* baseMesh);
//
//   void completeInstanceExport(FCDControllerInstance* instance, FCDController* entity);


        /**
         * Write the controller data into the collada document.
         */
        void completeControllerExport();
        void completeInstanceExport(BaseController* colladaController);


        // Returns true if 'node' is of a supported shape type and it has a controller.
        bool hasController ( const MObject& node );
        bool hasSkinController ( const MObject& node );
        bool hasMorphController ( const MObject& node );

        /**
        * Iterate upstream finding all the nodes which affect the mesh.
        * @param node The mesh node.
        * @param stack The controller stack for the affected nodes.
        * @param meshStack The controller stack for the affected meshes.
        * @return bool False, if the "inMesh" attribute is connected we don't have affected nodes.
        */
        static bool findAffectedNodes( 
            const MObject& node, 
            ControllerStack& stack, 
            ControllerMeshStack& meshStack );

        /**
        * Reset all the controller node states.
        * @param stack The stack with the controller nodes to reset the states.
        */
        static void resetControllerNodeStates( ControllerStack &stack );

        /**
        * Reset all the intermediate mesh parameters.
        * @param meshStack The stack with the meshes to reset the parameters.
        */
        static void resetMeshParameters( ControllerMeshStack &meshStack );


    private:

        /**
         * Recursive call to exports the controller of the current scene element
         * and all child elements.
         * @param sceneElement The element to export. 
         */
        void exportControllers(SceneElement* sceneElement);

        /**
        * Recursive call to exports the controller of the current scene element
        * and all child elements.
        * @param sceneElement The element to export. 
        * @return bool True, if a controller element was exported.
        */
        bool exportController(SceneElement* sceneElement);

        /**
        * Exports all the mesh affected nodes in the controller stack.
        * @param stack The controller stack.
        */
        void exportControllerStack( 
            SceneElement* sceneElement, 
            const ControllerStack& stack );

        /**
         * Exports a skin/joint cluster.
         * @param sceneElement The scene element to export.
         * @param controllerNode The node for the controller to export.
         * @param outputShape The path to the output shape.
         */
        void exportSkinController(
            SceneElement* sceneElement, 
            const MObject controllerNode, 
            MDagPath outputShape );

        /**
         * Writes all data of the current controller element into the collada file.
         * @param skinTarget
         * @param colladaSkinController 
         *          Reference to the collada skin controller with the export data.
         */
        void exportController( 
            const String skinTarget, 
            const ColladaSkinController& colladaSkinController );

        /**
         * Writes the joint source into the collada document.
         * @param colladaSkinController 
         *          Reference to the collada skin controller with the export data.
         */
        void exportElementJoints( const ColladaSkinController& colladaSkinController );

        /**
         * Writes the vertex weights element into the collada document.
         * @param colladaSkinController 
         *          Reference to the collada skin controller with the export data.
         */
        void exportElementVertexWeights( const ColladaSkinController &colladaSkinController );

        /**
         * Exports the bind shape transform of the given controller.
         * @param colladaSkinController 
         *          Reference to the collada skin controller with the export data.
         */
        void exportBindShapeTransform( const ColladaSkinController& colladaSkinController );

        /**
        * Exports the bind poses source of the given controller.
        * @param controllerId The controller id to export.
        * @param colladaSkinController 
        *          Reference to the collada skin controller with the export data.
        */
        void exportWeightSource( const String controllerId, const ColladaSkinController& colladaSkinController );

        /**
        * Exports the bind poses source of the given controller.
        * @param colladaSkinController 
        *          Reference to the collada skin controller with the export data.
         */
        void exportBindPosesSource( const ColladaSkinController& colladaSkinController );

        /**
         * Exports the joint source of the given controller.
         * @param colladaSkinController 
         *          Reference to the collada skin controller with the export data.
         */
        void exportJointSource( const ColladaSkinController& colladaSkinController );

        /**
         * Retrieve the instance information for this skinned character.
         * @param meshFn The current mesh object with the function set.
         * @param clusterFn The skinned character.
         * @param outputShape The output shape
         * @return uint The current cluster index.
         */
        uint retrieveInstanceInformation( 
            const MFnMesh& meshFn, 
            const MFnGeometryFilter& clusterFn, 
            MDagPath& outputShape );

        /**
         * Export the transform matrix for the shape.
         * @param clusterFn The cluster function set.
         * @param isJointCluster True, if it is a joint cluster.
         * @param clusterIndex The current cluster index for the skinned character.
         */
        void getBindShapeTransform(
            ColladaSkinController* skinController,
            const MFnGeometryFilter& clusterFn,
            const bool isJointCluster,
            const uint clusterIndex );

        /**
         * Gather the joints of the current controller node and writes them as 
         * the influences in the target controller object.
         * @param targetController The controller object to fill the bind poses.
         * @param controllerNode The controller node of the current object.
         * @param weightFilters The weight filters to fill.
         * @param clusterIndex The current cluster index for the skinned character.
         */
        void gatherJoints( 
            ColladaSkinController* skinController, 
            const MObject& controllerNode, 
            MObjectArray& weightFilters, 
            const uint clusterIndex );

        /**
         * Gather the bind matrices of the current controller node and writes 
         * them as the bind poses in the target controller object.
         * @param colladaSkinController The controller to hold the data.
         * @param controllerNode The controller node of the current object.
         */
        void gatherBindMatrices( 
            ColladaSkinController* colladaSkinController, 
            const MObject& controllerNode );

        /**
         * Support for joint clusters pipeline.
         * @param controllerNode The controller node.
         * @param influences The influences to fill.
         * @param weightFilters The weight filters to fill.
         * @param clusterIndex The current cluster index for the skinned character.
         */
        void getJointClusterInfluences(
            const MObject& controllerNode, 
            MDagPathArray& influences, 
            MObjectArray& weightFilters, 
            const uint clusterIndex );

        /**
        * Creates the joints.
        * @param colladaSkinController The skin controller with the joint imformation.
        */
        void createJoints( ColladaSkinController* colladaSkinController );

        /**
          * Collects the data of the vertex weights and push it into the collada 
          * skin controller object.
          * @param skinController The collada skin controller object to export.
          * @param controllerNode The controller node.
          * @param outputShape The output shape
          * @param weightFilters The list of the weight filters of the current node.
          * @param clusterIndex The current cluster index for the skinned character.
          * @param numInfluences The number of influences.
          */
         void collectVertexWeights( 
             ColladaSkinController* skinController, 
             const MObject& controllerNode, 
             const MDagPath& outputShape,
             const MObjectArray& weightFilters, 
             const uint clusterIndex, 
             uint numInfluences );

         /**
          * Collects the vertex weights of a skin cluster filter.
          * @param colladaInfluences List for the vertex influences.
          * @param controllerNode The current controller node.
          * @param outputShape The current output shape.
          * @param numInfluences The number of vertex influences.
          */
         void collectSkinClusterFilterVertexWeights( 
             std::vector<SkinControllerVertex>& colladaInfluences, 
             const MObject& controllerNode, 
             const MDagPath& outputShape, 
             uint numInfluences );

         /**
          * @todo documentation
          * @param colladaInfluences List for the vertex influences.
          * @param weightFilters The list with the weight filters.
          * @param outputShape The current output shape.
          * @param clusterIndex The current cluster index.
          */
         void collectJointClusterVertexWeights( 
             std::vector<SkinControllerVertex>& colladaInfluences, 
             const MObjectArray &weightFilters, 
             const MDagPath &outputShape, 
             const uint clusterIndex );


        // Support for Joint clusters pipeline
//  void getJointClusterInfluences(const MObject& controllerNode, MDagPathArray& influences, MObjectArray& weightFilters, uint clusterIndex);
    };

}

#endif // __COLLADA_MAYA_CONTROLLER_EXPORTER_H___