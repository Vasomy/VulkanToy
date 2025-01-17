#pragma once
#include<vector>
#include<string>
#include<fstream>

#pragma once
#ifndef ModelLoader_H

#define ModelLoader_H

#include"../3D/GameObject/GameObject.hpp"
#include<iostream>
namespace Tools {
    struct DataBatch
    {
        void* data;
        int length;
        int size;
        void Free()
        {
            delete[]data;
        }
        ~DataBatch()
        {
        }
    };

    struct Face
    {
        int v[3];
        int n[3];
        int t[2];
    };

    class ModelLoader
    {
        enum TargetType
        {
            Unknow,
            Obj
        };

        inline static ModelLoader* instance = nullptr;
        std::vector<std::string> m_load_content;
        float* m_load_vertices;
        int32_t m_vertices_length;
        float* m_load_normal;
        int32_t m_normal_length;
        int32_t* m_load_index;
        int32_t m_index_length;
        TargetType m_target_type = Unknow;
        ModelLoader() = default;

        void Reset();

        static ModelLoader* Instance() {
            if (ModelLoader::instance == nullptr)
            {
                ModelLoader::instance = new (std::nothrow)ModelLoader;
            }
            return instance;
        }

        std::shared_ptr<JRender::JGameObject> LoadObjData(const char*);

        DataBatch GetOneLineData(const char* content);
        DataBatch GetOneLineData(std::string content) {
            return GetOneLineData(content.data());
        }

    public:

        static std::shared_ptr<JRender::JGameObject> ReadDataFromFile(const char* file_name);
        static void LoadDataToSceneDirectly();
    };
}
#endif