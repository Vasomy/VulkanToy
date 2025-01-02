#include"ModelLoader.hpp"
#include"glm/glm.hpp"
#include<iostream>
#include"../Debug/LogInfo.hpp"

using namespace Tools;
std::shared_ptr<JRender::JGameObject> ModelLoader::ReadDataFromFile(const char* file_name)
{
    auto* ins = Instance();
    int length = strlen(file_name);
    int last = length;
    std::string sufx;
    while (last >= 0 && file_name[last] != '.')
    {
        if (file_name[last])
            sufx.insert(sufx.begin(), file_name[last]);
        last--;
    }

    if (sufx == "obj")
    {
        ins->m_target_type = Obj;


    }
    else if (sufx == "txt")
    {

    }

    return ins->LoadObjData(file_name);
}

void ModelLoader::LoadDataToSceneDirectly()
{
}

std::shared_ptr<JRender::JGameObject> ModelLoader::LoadObjData(const char* file_name)
{
    std::ifstream ifs;
    ifs.open(file_name);
    std::string next_line;

    std::vector<glm::vec3>vertices;
    std::vector<glm::vec3>normals;
    std::vector<glm::vec2>texcoords;
    std::vector<Face>faces;

    struct VertexIndex
    {
        int v_index[3];//vert index
        int t_index[3];//texcoord index
    };
    std::vector<VertexIndex> indices;

    while (std::getline(ifs, next_line))
    {
        auto ftc = next_line.front(); // front char
        std::string pre_tag;
        int pre_i = 0;
        while (pre_i < next_line.size() && next_line[pre_i] != ' ')
        {
            pre_tag.push_back(next_line[pre_i]);
            pre_i++;
        }
        if (pre_tag == "#")continue;
        if (pre_tag == "o")
        {
            // do describe
        }
        else if (pre_tag == "v")
        {
            auto data_batch = GetOneLineData(next_line);
            glm::vec3 vertex;
            for (int i = 0; i < data_batch.length; ++i)
            {
                vertex[i] = *((float*)data_batch.data + i);
            }
            vertices.push_back(vertex);
            //data_batch.Free();
        }
        else if (pre_tag == "vn")
        {
            auto data_batch = GetOneLineData(next_line);
            glm::vec3 normal;

            for (int i = 0; i < data_batch.length; ++i)
            {
                normal[i] = *((float*)data_batch.data + i);
            }
            normals.push_back(normal);
            data_batch.Free();

        }
        else if (pre_tag == "vt")
        {
            auto data_batch = GetOneLineData(next_line);
            glm::vec2 texcoord;
            for (int i = 0; i < data_batch.length; ++i)
            {
        
                texcoord[i] = *((float*)data_batch.data + i);
            }
            texcoords.push_back(texcoord);
            data_batch.Free();

        }
        else if (pre_tag == "f")
        {
            auto data_batch = GetOneLineData(next_line);
            VertexIndex index;
            int gap = data_batch.length / 3;
            for (int i = 0; i < data_batch.length; i+= gap)
            {
                
                index.v_index[i/gap] = *((Index*)data_batch.data + i);
                //std::cout<< *((int*)data_batch.data + i)<<' ';
                index.t_index[i/gap] = *((Index*)data_batch.data + i + 1);
            }
            //std::cout << '\n';
            indices.push_back(index);
           
            data_batch.Free();
        }
    }
  
    std::vector<Vertex> vertices_t;
    std::vector<Index> indices_t;


    bool haveTexcoord = false;
    std::cout << "Vertex Nums: " << vertices.size() << '\n';
    std::cout << "Face Nums:" << indices.size() << '\n';
    if (vertices.size() == indices.size())
    {
        haveTexcoord = true;
    }
    else
    {
        std::cout << "not Have Texcoord!\n";
    }

    for (int i = 0; i < indices.size(); ++i)
    {
        auto& idx = indices[i];
        Vertex vert;
        for (int j = 0; j < 3; ++j)
        {
            vert.position = vertices[idx.v_index[j]-1];
            
            if(haveTexcoord)
                vert.texcoord = texcoords[idx.t_index[j]-1];
            vertices_t.push_back(vert);
        }
        indices_t.push_back(i * 3);
        indices_t.push_back(i * 3 + 1);
        indices_t.push_back(i * 3 + 2); 
    }

    std::shared_ptr<JRender::JGameObject> ret;
    ret.reset(
        new JRender::JGameObject("Marry")
    );
    std::shared_ptr<JRender::RawMesh> mesh;
    mesh.reset(
        new JRender::RawMesh(vertices_t.size(), indices_t.size(), vertices_t.data(), indices_t.data())
    );
    mesh->position = { 0,-2,0 };
    ret->meshes.push_back(mesh);

    Debug::CoutToFile("TEST.txt", indices_t);

    return ret;
    /*auto m_mesh_storge = RawMeshStorge::Instance();

    for (int i = 0; i < faces.size(); ++i)
    {

        auto& face = faces[i];
        Render::RawMeshTriangle* new_tri = new Render::RawMeshTriangle(
            {
                vertices[face.v[0]],
                vertices[face.v[1]],
                vertices[face.v[2]],
                vertices[face.n[0]],
                vertices[face.n[1]],
                vertices[face.n[2]]
            },
            glm::vec3(0)
        );
        new_tri->BindMaterial(0);
        m_mesh_storge->Insert(new_tri);
    }*/
}

DataBatch ModelLoader::GetOneLineData(const char* content)
{
    int length = 0;
    int size = 4;
    int index = 0;
    enum datatype
    {
        Float,
        Int
    };
    datatype type = Int;
    int strLength = strlen(content);
    std::string num;
    std::vector<std::string>nums;
    while (index < strLength)
    {

        while (index < strLength && content[index] && (content[index] < '0' || content[index]>'9'))
        {
            index++;

        }

        while (index < strLength && content[index] != '/' && content[index] != ' ')
        {

            if (content[index] == '.')
            {
                type = Float;
            }
            num.push_back(content[index]);

            index++;
        }

        if (index == strLength || index < strLength && (content[index] == ' ' || content[index] == '/' || content[index] == '\0' || index == strLength))
        {
            index++;
            nums.push_back(num);
            length++;
            
            num.clear();
        }
    }

    num.clear();
    num.shrink_to_fit();
    DataBatch ret_data;
 
    if (type == Float)
    {
        float* ret = new float[length];

        for (int i = 0; i < length; ++i)
        {

            float number = std::stof(nums[i]);
            ret[i] = number;
        }

        ret_data.data = ret;
        ret_data.length = length;
        ret_data.size = sizeof(float);

    }
    else if (type == Int)
    {
        int* ret = new int[length];

        for (int i = 0; i < length; ++i)
        {
            int number = std::stoi(nums[i]);
            ret[i] = number;
        }
        ret_data.data = ret;
        ret_data.length = length;
        ret_data.size = sizeof(float);

    }


    return std::move(ret_data);

}

