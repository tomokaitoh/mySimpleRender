#include"header.hpp"

const int WindowWidth = 1000;
const int WindowHeight = 700;
const char* WindowTitle = "Render Window";

const char* ModelPath = ".\\resources\\hutao.obj";
const int ModelScale = 200;

Model model;
Camera camera;
Pipline pipline;
std::thread paintThread;

void init();
void paintLoop();

int main(int argc,char *args[]){
    init();
    paintThread = std::thread(paintLoop);
    Window.mainLoop();

    Sleep(2000);
    return 0;
}

void init(){
    camera = Camera(vec4f(0,0,-500,1),vec4f(0,0,1,0),vec4f(0,1,0,0));
    camera.zNear = 100;
    camera.zFar = 500;
    camera.fov = Math::PI/2;
    camera.aspect = float(Window.width)/Window.height;

    Window.init(WindowWidth,WindowHeight,WindowTitle);
    pipline.init();
    pipline.setCamera(camera);

    KeyBoard.setHotkey('Z',[]{pipline.opinion.switchProjectionMode();});
    KeyBoard.setHotkey('X',[]{pipline.opinion.switchRenderMode();});
    KeyBoard.setHotkey('C',[]{pipline.opinion.switchMsaa();});
    KeyBoard.setHotkey('V',[]{pipline.opinion.switchAutoMoveCamera();});
    KeyBoard.setHotkey('Q',[]{model.scale(2);});
    KeyBoard.setHotkey('E',[]{model.scale(0.5);});
    KeyBoard.setHotkey('W',[]{camera.pos.y+=1;});
    KeyBoard.setHotkey('S',[]{camera.pos.y-=1;});
    KeyBoard.setHotkey('A',[]{camera.pos.x-=1;});
    KeyBoard.setHotkey('D',[]{camera.pos.x+=1;});
    KeyBoard.setHotkey('R',[]{camera.zNear-=100;});
    KeyBoard.setHotkey('T',[]{camera.zFar-=100;});
    KeyBoard.setHotkey('Y',[]{camera.zNear+=100;});
    KeyBoard.setHotkey('U',[]{camera.zFar+=100;});
    KeyBoard.setHotkey('O',[]{std::string path(selectFileDialog());model.open(path.c_str());model.scale(ModelScale);});
}

void paintLoop(){
    while(!model.isOpen);
    pipline.setModel(model);

    while(true){
        Window.clear();
        pipline.clearBuffer();

        pipline.applicationStage();
        pipline.geometryStage();
        pipline.rasterizationStage();
        pipline.pixelsStage();

        pipline.opinion.displayInfo();
        pipline.getModel().displayInfo();
        pipline.displayInfo();

        Window.drawFrame();
    }
}