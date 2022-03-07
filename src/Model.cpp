#pragma once
#include"Model.hpp"

//Texture类方法
void Texture::open(const char *path){
    if(pixels) delete[] pixels;
    std::cout<<"texture: "<<path<<std::endl;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    FILE *f = fopen(path,"rb");
    if(f==NULL){
        std::cerr <<"File \""<<path<<"\" not found.\n";
        return;
    }
    fread(&fileHeader,sizeof(BITMAPFILEHEADER),1,f);
    fread(&infoHeader,sizeof(BITMAPINFOHEADER),1,f);
    width  = infoHeader.biWidth;
    height = infoHeader.biHeight;
    pixels = new Color[width*height];
    fread(pixels,sizeof(Color),width*height,f);
    fclose(f);
}

Color& Texture::getPixel(int x,int y){
    if(((unsigned int)x)<width && ((unsigned int)y)<height)
        return pixels[x+y*width];
    else return Color_Black;
}
Texture::Texture(const char *path){
    pixels = NULL;
    this->open(path);
}
Texture::Texture(const Texture& t){
    width = t.width;
    height = t.height;
    pixels = new Color[width*height];
    for(int i=0,end=width*height;i<end;i++)
        pixels[i] = t.pixels[i];
}
Texture::~Texture(){
    if(pixels) delete[] pixels;
}

//Model类方法
void Model::open(const char *path){
    std::ifstream modelFile,mtlFile; //模型文件.obj，材质文件.mtl
    std::regex reg; //正则表达式格式
    std::smatch res; //正则表达式匹配结果
    std::string line; //读取文件时为一行一行地读取
    std::map<std::string,int> textureIDs; //纹理的名字对应ID

    //删除原来的模型数据
    this->isOpen = false;
    if(vertices.size()){
        vertices.resize(0);
        texCoords.resize(0);
        normals.resize(0);
        triangles.resize(0);
        textures.resize(0);
    }
    
    //打开模型文件
    modelFile.open(path,std::ios::in);
    tlogv(path);
    if (!modelFile.is_open()) {
        std::cerr <<"File \""<<path<<"\" not found.\n";
        return;
    }

    //打开材质文件
    while(getline(modelFile,line),line[0]!='m');
    mtlFile.open(line.substr(7),std::ios::in);
    if(!mtlFile.is_open()){
        std::cerr <<"File \""<<line.substr(7)<<"\" not found.\n"<< std::endl;
        return;
    }

    //获取材质对应的纹理
    textures.reserve(10);
    while(getline(mtlFile,line)){
        if(line.substr(0,6)=="newmtl")
            textureIDs[line.substr(7)] = textureIDs.size();
        else if(line.substr(0,6)=="map_Kd")
            textures.emplace_back(line.substr(7).c_str());
    }
    textures.resize(textures.size());
    mtlFile.close();

    getline(modelFile,line);
    std::cout<<"model_name: "<<line.substr(2)<<std::endl;

    //获取顶点数据
    getline(modelFile,line);
    reg = std::regex("v ([-\\.0-9]+) ([-\\.0-9]+) ([-\\.0-9]+)");
    for(vertices.reserve(10000); std::regex_match(line,res,reg); getline(modelFile,line))
        vertices.emplace_back(atof(res.str(1).c_str()),atof(res.str(2).c_str()),atof(res.str(3).c_str()),1);
    vertices.resize(vertices.size());
    std::cout<<"vertices.size(): "<<vertices.size()<<std::endl;

    //获取贴图坐标数据
    reg = std::regex("vt ([-\\.0-9]+) ([-\\.0-9]+)");
    for(texCoords.reserve(10000);std::regex_match(line,res,reg);getline(modelFile,line))
        texCoords.emplace_back(atof(res.str(1).c_str()),atof(res.str(2).c_str()));
    texCoords.resize(texCoords.size());
    std::cout<<"texCoords.size(): "<<texCoords.size()<<std::endl;

    //获取法线数据
    reg = std::regex("vn ([-\\.0-9]+) ([-\\.0-9]+) ([-\\.0-9]+)");
    for(normals.reserve(10000);std::regex_match(line,res,reg);getline(modelFile,line))
        normals.emplace_back(atof(res.str(1).c_str()),atof(res.str(2).c_str()),atof(res.str(3).c_str()),1);
    normals.resize(normals.size());
    std::cout<<"normals.size(): "<<normals.size()<<std::endl;

    //获取三角形数据
    int texID = 0;
    reg = std::regex("f ([0-9]+)/([0-9]+)/([0-9]+) ([0-9]+)/([0-9]+)/([0-9]+) ([0-9]+)/([0-9]+)/([0-9]+)");
    for(triangles.reserve(10000);getline(modelFile,line);){
        if(line[0]=='u') texID=textureIDs[line.substr(7)];
        if(!std::regex_search(line,res,reg)) continue;
        triangles.emplace_back(res,texID);
    }
    triangles.resize(triangles.size());
    std::cout<<"triangles.size(): "<<triangles.size()<<std::endl;
    modelFile.close();
    this->isOpen = true;

}
void Model::scale(float value){
    for(auto& v:vertices) v*=value; 
}
void Model::displayInfo(){
    Window.setText(185,std::string("vertices num: ")+std::to_string(vertices.size()));
    Window.setText(205,std::string("texCoords num: ")+std::to_string(texCoords.size()));
    Window.setText(225,std::string("normals num: ")+std::to_string(normals.size()));
    Window.setText(245,std::string("triangles num: ")+std::to_string(triangles.size()));
}

Model::Model(const char *path,int scaleValue){
    Model();
    this->open(path);
    this->scale(scaleValue);
}

//Triangle类构造方法
Triangle::Triangle(std::smatch &res,int texID){
    this->vert[0] = atoi(res.str(1).c_str())-1;this->tex[0] = atoi(res.str(2).c_str())-1;this->normal[0] = atoi(res.str(3).c_str())-1;
    this->vert[1] = atoi(res.str(4).c_str())-1;this->tex[1] = atoi(res.str(5).c_str())-1;this->normal[1] = atoi(res.str(6).c_str())-1;
    this->vert[2] = atoi(res.str(7).c_str())-1;this->tex[2] = atoi(res.str(8).c_str())-1;this->normal[2] = atoi(res.str(9).c_str())-1;
    this->textureID = texID;
}

void Camera::lookat(vec4f target){
    //vec4f newDirection = target-pos;
    float d = -pos.x*up.x - pos.y*up.y - pos.z*up.z; 
    float t = (up.x*target.x + up.y*target.y + up.z*target.z+d)/(up.x*up.x+up.y*up.y+up.z*up.z);

    direction = (vec4f(target.x-up.x*t,target.y-up.y*t,target.z-up.z*t,0)-pos).normalized();
    right = Math::cross(direction,up).normalized();
    direction = (target-pos).normalized();
    up = Math::cross(right,direction).normalized();
}
void Camera::moveY(float step){
    
}