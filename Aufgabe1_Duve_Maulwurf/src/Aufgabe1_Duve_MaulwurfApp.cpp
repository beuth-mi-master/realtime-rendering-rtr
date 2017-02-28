//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/app/KeyEvent.h"

#include "RTR/RTR.h"


using namespace ci;
using namespace ci::app;
using namespace std;

// App template for the Real Time Rendering course, inherits from Cinder App.
class Aufgabe1_Duve_MaulwurfApp : public App
{
  public:
    // Called once when the App starts, after OpenGL has been initialized.
    void setup() override;

    // Called for every frame to be rendered, before draw().
    void update() override;

    // Called once for every frame to be rendered.
    void draw() override;
    
    void keyDown (KeyEvent event) override;

    void handleKeyCode (int key);

  private:
    // Rotation angle used for the animation.
    double angleX = 0.0;
    double angleY = 0.0;

    // Scale factor of scene
    vec3 scaleFactor = vec3(1,1,1);
    
    // Tracking time between two draw() calls.
    double lastTime = getElapsedSeconds();
    
    vec3 rotationX = vec3(0,0,0);
    vec3 rotationY = vec3(0,0,0);


    // Model of the duck that is displayed.
    rtr::ModelRef table;
    rtr::ModelRef duck;
    rtr::NodeRef scene;
};

void Aufgabe1_Duve_MaulwurfApp::keyDown (KeyEvent event){
    int keyCode = event.getCode();
    Aufgabe1_Duve_MaulwurfApp::handleKeyCode(keyCode);
}

void Aufgabe1_Duve_MaulwurfApp::handleKeyCode(int key) {
    vec3 oldRotationX = rotationX;
    vec3 oldRotationY = rotationY;

    switch (key) {
        case KeyEvent::KEY_LEFT:
            rotationY.y = -1;
            angleY += (M_PI / 100);
            break;
        case KeyEvent::KEY_UP:
            rotationX.x = -1;
            angleX += (M_PI / 100);
            break;
        case KeyEvent::KEY_RIGHT:
            rotationY.y = 1;
            angleY += (M_PI / 100);
            break;
        case KeyEvent::KEY_DOWN:
            rotationX.x = 1;
            angleX += (M_PI / 100);
            break;
        // plus
        case 93:
            scaleFactor += vec3(0.1, 0.1, 0.1);
            break;
        // minus
        case 47:
            scaleFactor -= vec3(0.1, 0.1, 0.1);
            break;
        case KeyEvent::KEY_ESCAPE:
            quit();
            break;
        default:
            cout << "pressed key: " << key << "\r\n";
            break;
    }
    if (oldRotationX.x != rotationX.x) {
        angleX *= -1;
    }
    if (oldRotationY.y != rotationY.y) {
        angleY *= -1;
    }
}


// Place all one-time setup code here.
void
Aufgabe1_Duve_MaulwurfApp::setup()
{
    // Arange for the window to always be on top. This helps with live shader
    // coding
    getWindow()->setAlwaysOnTop();

    //app::KeyEvent::getCode();
    
    // Create a live-reloading shader program.
    auto lambert = rtr::watcher.createWatchedProgram(
      { getAssetPath("lambert.vert"), getAssetPath("lambert.frag") });

    // Load the duck model and use the lambert shader on it.
    duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"), true, lambert);
    table = rtr::loadObjFile(getAssetPath("desk/desk.obj"), true, lambert);
    
    rtr::NodeRef tableNode = rtr::Node::create({table});
    
    mat4 duckTransform;
    duckTransform = rotate(duckTransform,toRadians(130.0f),vec3(0,1,0));
    duckTransform = translate(duckTransform, vec3(-0.5,0.55,-0.8));
    duckTransform = scale(duckTransform, vec3(0.08,0.08,0.08));

    rtr::NodeRef duckNode = rtr::Node::create({duck}, duckTransform);
    
    scene = rtr::Node::create({}, mat4(), {tableNode, duckNode});
    
    // The shader program can also be replaced after the fact.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));
    // for (auto& shape : duck->shapes)
    //     shape->replaceProgram(lambert);

    // Try this version of the duck to see the default shader for OBJ
    // models.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));
}

// Place all non-OpenGL once-per-frame code here.
void
Aufgabe1_Duve_MaulwurfApp::update()
{
    // Check now whether any files changed.
    rtr::watcher.checkForChanges();
}

// Place all once-per-frame drawing code here.
void
Aufgabe1_Duve_MaulwurfApp::draw()
{
    // Clear background to gray.
    gl::clear(Color(0.5, 0.5, 0.5));

    // Setup a perspective projection camera.
    CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 10.0f);
    camera.lookAt(vec3(-4, 1, 0), vec3(0, 0, 0));
    
    // Push the view-projection matrix to the bottom of the matrix stack.
    gl::setMatrices(camera);

    // Enable depth buffering.
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // Save current model-view-projection matrix by pushing a new matrix on top.
    gl::pushModelMatrix();

    // Apply the rotation around the diagonal unit axis.
    gl::scale(scaleFactor);
    gl::rotate(angleX, rotationX);
    gl::rotate(angleY, rotationY);

    // Draw the model.
    scene->draw();
    
    // Restore the previous model-view-projection matrix.
    gl::popModelMatrix();
}

void
prepareSettings(Aufgabe1_Duve_MaulwurfApp::Settings* settings)
{
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(Aufgabe1_Duve_MaulwurfApp, RendererGl, prepareSettings)
