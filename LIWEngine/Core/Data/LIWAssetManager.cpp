#include "LIWAssetManager.h"

namespace LIW {
    void LIWAssetManager::Init()
    {
        liw_objhdl_type hdlMDSphere = CreateMeshData(LIW_MESHDATA_SPHERE_NAME);
        auto& mdSphere = GetMeshData(hdlMDSphere);
        LIWMeshData::CreateSphere(mdSphere);
        liw_objhdl_type hdlSphere = CreateMesh(LIW_MESH_SPHERE_NAME);
        auto& sphere = GetMesh(hdlSphere);
        sphere.CreateMesh(mdSphere);

        liw_objhdl_type hdlMDCube = CreateMeshData(LIW_MESHDATA_CUBE_NAME);
        auto& mdCube = GetMeshData(hdlMDCube);
        LIWMeshData::CreateCube(mdCube);
        liw_objhdl_type hdlCube = CreateMesh(LIW_MESH_CUBE_NAME);
        auto& cube = GetMesh(hdlCube);
        cube.CreateMesh(mdCube);

        liw_objhdl_type hdlMDPlane = CreateMeshData(LIW_MESHDATA_PLANE_NAME);
        auto& mdPlane = GetMeshData(hdlMDPlane);
        LIWMeshData::CreatePlane(mdPlane);
        liw_objhdl_type hdlPlane = CreateMesh(LIW_MESH_PLANE_NAME);
        auto& plane = GetMesh(hdlPlane);
        plane.CreateMesh(mdPlane);
    }

    void LIWAssetManager::Cleanup()
    {
        DestroyMeshData(LIW_MESHDATA_SPHERE_NAME);
        DestroyMesh(LIW_MESH_SPHERE_NAME);

        DestroyMeshData(LIW_MESHDATA_CUBE_NAME);
        DestroyMesh(LIW_MESH_CUBE_NAME);

        DestroyMeshData(LIW_MESHDATA_PLANE_NAME);
        DestroyMesh(LIW_MESH_PLANE_NAME);

        if (m_assetMap.size() > 0) {
            printf("Warning: Not all assets have been destroyed: \n");
            for (auto itr = m_assetMap.begin(); itr != m_assetMap.end(); itr++) {
                printf(" AssetName: %s  AssetType[%d] \n", itr->first.c_str(), itr->second.m_assetType);
            }
        }
    }


}