#include "LIWAssetManager.h"

namespace LIW {
    void LIWAssetManager::Init()
    {
        liw_objhdl_type hdlMDSphere = CreateMeshData(LIW_MESHDATA_SPHERE);
        auto& mdSphere = GetMeshData(hdlMDSphere);
        LIWMeshData::CreateSphere(mdSphere);
        liw_objhdl_type hdlSphere = CreateMesh(LIW_MESH_SPHERE);
        auto& sphere = GetMesh(hdlSphere);
        sphere.CreateMesh(mdSphere);

        liw_objhdl_type hdlMDCube = CreateMeshData(LIW_MESHDATA_CUBE);
        auto& mdCube = GetMeshData(hdlMDCube);
        LIWMeshData::CreateCube(mdCube);
        liw_objhdl_type hdlCube = CreateMesh(LIW_MESH_CUBE);
        auto& cube = GetMesh(hdlCube);
        cube.CreateMesh(mdCube);

        liw_objhdl_type hdlMDPlane = CreateMeshData(LIW_MESHDATA_PLANE);
        auto& mdPlane = GetMeshData(hdlMDPlane);
        LIWMeshData::CreatePlane(mdPlane);
        liw_objhdl_type hdlPlane = CreateMesh(LIW_MESH_PLANE);
        auto& plane = GetMesh(hdlPlane);
        plane.CreateMesh(mdPlane);
    }

    void LIWAssetManager::Cleanup()
    {
        DestroyMeshData(LIW_MESHDATA_SPHERE);
        DestroyMesh(LIW_MESH_SPHERE);

        DestroyMeshData(LIW_MESHDATA_CUBE);
        DestroyMesh(LIW_MESH_CUBE);

        DestroyMeshData(LIW_MESHDATA_PLANE);
        DestroyMesh(LIW_MESH_PLANE);

        if (m_assetMap.size() > 0) {
            printf("Warning: Not all assets have been destroyed: \n");
            for (auto itr = m_assetMap.begin(); itr != m_assetMap.end(); itr++) {
                printf(" AssetName: %s  AssetType[%d] \n", itr->first.c_str(), itr->second.m_assetType);
            }
        }
    }


}