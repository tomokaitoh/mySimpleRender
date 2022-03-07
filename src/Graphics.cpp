#pragma once
#include"Graphics.hpp"
#include"Pipline.hpp"
Matrix4f Graphics::modelTrans(Model& m){
    float trans_arr[16] = {
        1,0,0,m.pos.x,
        0,1,0,m.pos.y,
        0,0,1,m.pos.z,
        0,0,0,1
    };
    return Matrix4f(trans_arr);
}

Matrix4f Graphics::viewTrans(Camera& c){
    float trans_arr[16] = {
        1.0 , 0.0 , 0.0 , -c.pos.x,
        0.0 , 1.0 , 0.0 , -c.pos.y,
        0.0 , 0.0 , 1.0 , -c.pos.z,
        0.0 , 0.0 , 0.0 , 1.0  
    };
    float view_arr[16] = {
        c.right.x , c.up.x , c.direction.x , 0.0,
        c.right.y , c.up.y , c.direction.y , 0.0,
        c.right.z , c.up.z , c.direction.z , 0.0,
        0.0     , 0.0  , 0.0         , 1.0  
    };
    return Matrix4f(view_arr)*Matrix4f(trans_arr);
}

Matrix4f Graphics::projTrans(Camera& c,int projectionMode){
    if(projectionMode==PerspectiveMode){
        float trans_arr[16]={
            float(Window.height)/tan(c.fov/2)/Window.width,0,0,0,
            0,1.f/tan(c.fov/2),0,0,
            0,0,(c.zFar+c.zNear)/(c.zFar-c.zNear),(2*c.zNear*c.zFar)/(c.zNear-c.zFar),
            0,0,1,0
        };
        return Matrix4f(trans_arr);
    }
    else if(projectionMode==OrthogonalMode){
        float trans_arr[16] = {
            2/float(Window.width),0,0,0,
            0,2/float(Window.height),0,0,
            0,0,1,0,
            0,0,0,1
        };
        return Matrix4f(trans_arr);
    }
    return Matrix4f::identityMatrix;
}
Matrix4f Graphics::viewportTrans(){
    float trans_arr[16] = {
        Window.width/2.f,0,0,0,
        0,Window.height/2.f,0,0,
        0,0,1,0,
        0,0,0,1
    };
    return Matrix4f(trans_arr);
}

//bresenham算法
void Graphics::line(vec4f &v1,vec4f& v2,Color& color){
    static int x1,x2,y1,y2;
    static int dx,dy,k,d,d2x,d2y;
    static int x_step,y_step;

    x1 = int(v1.x); 
    y1 = int(v1.y);
    x2 = int(v2.x); 
    y2 = int(v2.y);
    dx = std::abs(x2-x1); d2x = 2*dx;
    dy = std::abs(y2-y1); d2y = 2*dy;
    x_step = x1<=x2?1:-1;
    y_step = y1<=y2?1:-1;
    d = 0;

    if(dx>dy) for(k=d2y;x1!=x2;d+=k,x1+=x_step){
        if(d>dx){ 
            d-=d2x;
            Window.setPixel(x1,y1+=y_step,color);
        }
        else Window.setPixel(x1,y1,color);
    }

    else for(k=d2x;y1!=y2;d+=k,y1+=y_step){
        if(d>dy){ 
            d-=d2y;
            Window.setPixel(x1+=x_step,y1,color);
        }
        else Window.setPixel(x1,y1,color);
    }
}


void Graphics::wireFrameTriangle(Pipline& p){
    line(p.triangleNow.v1,p.triangleNow.v2);
    line(p.triangleNow.v1,p.triangleNow.v3);
    line(p.triangleNow.v2,p.triangleNow.v3);
}

void Graphics::scanLine(Pipline& p,int y,int x1,int x2){
    TriangleTmp &t = p.triangleNow;
    float z;
    if(x1>x2) std::swap(x1,x2);
    for(int i=x1;i<=x2;++i){
        calculateWeight(p,i,y);
        z = t.weight[0]*t.v1.z + t.weight[1]*t.v2.z + t.weight[2]*t.v3.z;
        if(z>=p.getZbuffer(i,y)){
            p.setZbuffer(i,y,z);
            Window.setPixel(i,y,lerpTexture(p));
        }
    }
}

//改进扫描填充算法
void Graphics::fill(Pipline& p){
    TriangleTmp& t = p.triangleNow;

    //对顶点进行排序
    vec4f* v1_s = &t.v1;
    vec4f* v2_s = &t.v2;
    vec4f* v3_s = &t.v3;
    if(v1_s->y > v2_s->y) std::swap(v1_s,v2_s);
    if(v1_s->y > v3_s->y) std::swap(v1_s,v3_s);
    if(v2_s->y > v3_s->y) std::swap(v2_s,v3_s);
    calculateEdge(p,*v1_s,*v3_s,true);
    calculateEdge(p,*v1_s,*v2_s,false);
    calculateEdge(p,*v2_s,*v3_s,false);
    for(int i=t.ymin;i<t.ymax;++i)
        scanLine(p,i,t.edges[i].first,t.edges[i].second);
    if(p.opinion.MsaaMode){
        for(int i=t.ymin;i<t.ymax;++i){
            if(t.edges[i].first>t.edges[i].second) std::swap(t.edges[i].first,t.edges[i].second);
            int start = std::min(t.edges[i].first,t.edges[i].second);
            int end = std::max(t.edges[i].first,t.edges[i].second);
            for(int j=start;j<=end;j++) 
                calculateMsaa(p,j,i);
        }
    }
}

//根据重心坐标进行纹理插值
//双线性插值法
Color Graphics::lerpTexture(Pipline& p){
    static Color c[4];
    static float u_lerp,v_lerp,u,v;

    TriangleTmp& t = p.triangleNow;

    u = (t.weight[0]*t.vt1.x +t.weight[1]*t.vt2.x +t.weight[2]*t.vt3.x)*t.tex->width;
    v = (t.weight[0]*t.vt1.y +t.weight[1]*t.vt2.y +t.weight[2]*t.vt3.y)*t.tex->height;
    u = Math::clamp<float>(u,0,t.tex->width-1);
    v = Math::clamp<float>(v,0,t.tex->height-1);

    c[0] = t.tex->getPixel(u,v);
    c[1] = t.tex->getPixel(u,std::ceil(v));
    c[2] = t.tex->getPixel(std::ceil(u),v);
    c[3] = t.tex->getPixel(std::ceil(u),std::ceil(v));

    return Math::lerp<Color>(Math::lerp<Color>(c[0],c[1],u_lerp),Math::lerp<Color>(c[2],c[3],u_lerp),v_lerp);
}

void Graphics::calculateWeight(Pipline& p,float x,float y){
    TriangleTmp &t = p.triangleNow;
    t.weight[2] = ((t.v2.x-t.v1.x)*(y-t.v1.y)-(t.v2.y-t.v1.y)*(x-t.v1.x))/p.triangleNow.s;
    t.weight[0] = ((t.v3.x-t.v2.x)*(y-t.v2.y)-(x-t.v2.x)*(t.v3.y-t.v2.y))/p.triangleNow.s;
    t.weight[1] = 1-t.weight[2]-t.weight[0];
}
void Graphics::calculateEdge(Pipline& p,vec4f &v1,vec4f& v2,bool flag){
    static int x1,x2,y1,y2;
    static int dx,dy,k,d,d2x,d2y;
    static int x_step,y_step;

    x1 = int(v1.x); y1 = int(v1.y);
    x2 = int(v2.x); y2 = int(v2.y);
    dx = std::abs(x2-x1); d2x = 2*dx;
    dy = std::abs(y2-y1); d2y = 2*dy;
    x_step = x1<=x2?1:-1;
    y_step = y1<=y2?1:-1;
    d = 0;
    if(dx>dy) for(k=d2y;x1!=x2;d+=k,x1+=x_step){
        if(d>dx){ 
            d-=d2x;
            p.triangleNow.setEdge(y1 += y_step,x1,flag);
        }
        else p.triangleNow.setEdge(y1,x1,flag);
    }
    else for(k=d2x;y1!=y2;d+=k,y1+=y_step){
        if(d>dy){ 
            d-=d2y;
            p.triangleNow.setEdge(y1,x1+=x_step,flag);
        }
        else p.triangleNow.setEdge(y1,x1,flag);
    }
    p.triangleNow.setEdge(y1,x1,flag);
    p.triangleNow.setEdge(y2,x2,flag);
}

void Graphics::calculateMsaa(Pipline& p,int x,int y){
    static float dx[9] = {0,-0.5,0.5,-0.5,0.5,-0.1,0.33,-0.33,0.1};
    static float dy[9] = {0,-0.5,0.5,0.5,-0.5,0.33,0.1,-0.1,-0.33};
    for(int i=0;i<p.opinion.MsaaMode;++i){
        calculateWeight(p,x+dx[i],y+dy[i]);
        if(p.triangleNow.weight[0]>=-0.05 && p.triangleNow.weight[1]>=-0.05 && p.triangleNow.weight[2]>=-0.05) 
            p.setMsaa(x,y,i);
    }
}