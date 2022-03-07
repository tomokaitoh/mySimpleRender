#pragma once
#include"Pipline.hpp"

void TriangleTmp::set(Pipline& p,Triangle& t){
    v1 = p.transVertices[t.vert[0]];
    v2 = p.transVertices[t.vert[1]];
    v3 = p.transVertices[t.vert[2]];
    vt1 = p.model->texCoords[t.tex[0]];
    vt2 = p.model->texCoords[t.tex[1]];
    vt3 = p.model->texCoords[t.tex[2]];
    tex = &p.model->textures[t.textureID];
    s = (v2.x-v1.x)*(v3.y-v1.y)-(v2.y-v1.y)*(v3.x-v1.x);
    ymin = std::min(std::min(v1.y,v2.y),v3.y);
    ymin = Math::clamp<int>(ymin,0,Window.height-1);
    ymax = std::max(std::max(v1.y,v2.y),v3.y)+1;
    ymax = Math::clamp<int>(ymax,0,Window.height-1);
}

void TriangleTmp::setEdge(int y,int x,bool flag){
    if(y>=Window.height || y<0) return ;
    if(flag) edges[y].second = x;
    else edges[y].first = x;
}

//Pipline类方法
void Pipline::applicationStage(){
    static time_t piplineStartTime = clock();
    static int range = 300;

    frameStartTime = clock();

    if(opinion.isAutoMoveCamera){
        int t = difftime(frameStartTime,piplineStartTime);
        camera->pos = vec4f(
            model->pos.x + cos((double)t/1000.0)*range,
            model->pos.y,
            model->pos.z - sin((double)t/1000.0)*range,
            0
        ); 
    }
    camera->lookat(model->pos);
}
void Pipline::geometryStage(){
    Matrix4f MVPtrans = Graphics::viewportTrans() * Graphics::projTrans(*camera,opinion.projectionMode) * Graphics::viewTrans(*camera) * Graphics::modelTrans(*model);

    for(int i=0,size=model->vertices.size();i<size;++i){
        transVertices[i] = MVPtrans * model->vertices[i];
        transVertices[i].x += Window.width/2; 
        transVertices[i].y += Window.height/10;
    }
}
void Pipline::rasterizationStage(){
    if(opinion.renderMode == WireframeMode)
        for(auto& t:model->triangles){
            triangleNow.set(*this,t);
            Graphics::wireFrameTriangle(*this);
        }
    else if(opinion.renderMode == RasterizationMode)
        for(auto& t:model->triangles){
            triangleNow.set(*this,t);
            Graphics::fill(*this);
        }
    else {
        for(auto& t:model->triangles){
            triangleNow.set(*this,t);
            Graphics::fill(*this);
        }
        for(int i=0;i<Window.height;++i){
            for(int j=0;j<Window.width;++j)
                Window.setPixel(j,i,Color_White*getMsaa(j,i));
    }
    }
}
void Pipline::pixelsStage(){
    if(opinion.MsaaMode){
        for(int i=0;i<Window.height;++i){
            for(int j=0;j<Window.width;++j)
                Window.setPixel(j,i,Window.getPixel(j,i) * getMsaa(j,i));
        }
    }
}

void Pipline::init(){
    if(zBufferSize) return; //不允许重新初始化
    triangleNow.edges = std::vector<std::pair<int,int>>(Window.height,std::make_pair<int,int>(0,0));
    zBufferSize = Window.width*Window.height;
    MsaaBufferSize = Window.width*Window.height*9;
    zBuffer = new float[zBufferSize];
    MsaaBuffer = new bool[MsaaBufferSize];
}

void Pipline::setModel(Model& m){
    this->model = &m;
    transVertices.resize(model->vertices.size());
}
void Pipline::setCamera(Camera& c){
    this->camera = &c;
}
Camera& Pipline::getCamera(){
    return *camera;
}
Model& Pipline::getModel(){
    return *model;
}
float Pipline::getZbuffer(int x,int y){
    if(((unsigned int)x)<Window.width && ((unsigned int)y)<Window.height)
        return zBuffer[x+y*Window.width];
    else return 0;
}
void Pipline::setZbuffer(int x,int y,float z){
    if(((unsigned int)x)<Window.width && ((unsigned int)y)<Window.height)
        zBuffer[x+y*Window.width] = z;
}
void Pipline::setMsaa(int x,int y,int i){
    if(((unsigned int)x)<Window.width && ((unsigned int)y)<Window.height)
        MsaaBuffer[y*Window.width*9+x*9+i] = true;
}
float Pipline::getMsaa(int x,int y){
    if(((unsigned int)x)>=Window.width || ((unsigned int)y)>=Window.height)
        return 0;
    int count = 0;
    for(bool *p = &MsaaBuffer[y*Window.width*9+x*9],*end = p+opinion.MsaaMode;p!=end;++p){
        if(*p) ++count;
    }
    return float(count)/opinion.MsaaMode;
}
void Pipline::clearBuffer(){
    for(int i=0;i<zBufferSize;++i) zBuffer[i] = 0;
    memset(MsaaBuffer,0,sizeof(bool)*MsaaBufferSize);
}
void Pipline::displayInfo(){
    time_t t = clock();
    int renderTime = difftime(t,frameStartTime);
    Window.setText(85,std::string("FPS: ")+std::to_string(int(1000.0/Math::clamp<int>(renderTime,1,1000))));
    difftime(t,frameStartTime);
}
Pipline::~Pipline(){
    if(zBuffer) delete[] zBuffer;
    if(MsaaBuffer) delete[] MsaaBuffer;
}

//RenderOpinion类方法
void RenderOpinion::displayInfo(){
    Window.setText(5,std::string("projectionMode: ")+
            (projectionMode?"Perspective":"Orthogonal"));
    Window.setText(25,std::string("renderMode: ")+
            (renderMode?"Rasterization":"Wireframe"));
    Window.setText(45,std::string("MSAA: ")+
            (MsaaMode==0?"off":MsaaMode==Msaa5x?"5x":"9x"));
}
void RenderOpinion::switchProjectionMode(){projectionMode=!projectionMode;}
void RenderOpinion::switchRenderMode(){if(renderMode==1) renderMode=0;else renderMode=1;}
void RenderOpinion::switchMsaa(){if(MsaaMode==9)MsaaMode=0;else if(MsaaMode==0)MsaaMode=5;else MsaaMode=9;}
void RenderOpinion::switchAutoMoveCamera(){isAutoMoveCamera=!isAutoMoveCamera;}
