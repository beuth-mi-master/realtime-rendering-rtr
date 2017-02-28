//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "RTR/RTR.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// App template for the Real Time Rendering course, inherits from Cinder App.
class SimpleLightingApp : public App {
  public:
    // Called once when the App starts, after OpenGL has been initialized.
    void setup() override;

    // Called for every frame to be rendered, before draw().
    void update() override;

    // Called once for every frame to be rendered.
    void draw() override;
    
    // defining keydown method that is automatically called when a key is pressed
    void keyDown(KeyEvent event) override;
    
    // defining a method that will handle the pressed key event
    void handleKeyCode(KeyEvent keyCode);

    // exercise 2.1
    void ex2_1();
    
    // exercise 2.2
    void ex2_2();
    
    // exercise 2.3
    void ex2_3();
    
    // exercise 2.4
    void ex2_4();
    
    // changes the toonshading level of detail
    void changeLevelOfDetail();
    
    // update color of circles
    void updateCircleColor();
    
  private:
    // Rotation angle used for the animation.
    double angle = -110.0;

    // Tracking time between two draw() calls.
    double lastTime = getElapsedSeconds();

    rtr::ModelRef model;
    
    // this defines toon shading level default is 5
    int level = 5;
    // defines min level of detail for toon shader
    int minLevel = 1;
    // defines max level of detail for toon shader
    int maxLevel = 20;
    
    // color of circles for ex_2.3
    double color_red = 0.8;
    double color_green = 0.8;
    double color_blue = 0.8;
    
    // depth and size of circles in ex:2.3
    int circleDepth = 5;
    int maxCircleDepth = 20;
    float circleSize = 0.5;
    
};

void SimpleLightingApp::keyDown (KeyEvent event){
    SimpleLightingApp::handleKeyCode(event);
}

void SimpleLightingApp::handleKeyCode(KeyEvent event) {
    int keyCode = event.getCode();
    switch (keyCode) {
        // render ex_2.1
        case KeyEvent::KEY_1:
            SimpleLightingApp::ex2_1();
            break;
        // render ex_2.2
        case KeyEvent::KEY_2:
            SimpleLightingApp::ex2_2();
            break;
        // render ex_2.3
        case KeyEvent::KEY_3:
            SimpleLightingApp::ex2_3();
            break;
        // render ex_2.4
        case KeyEvent::KEY_4:
            SimpleLightingApp::ex2_4();
            break;
        // increase toon shading level
        case KeyEvent::KEY_UP:
            level++;
            SimpleLightingApp::changeLevelOfDetail();
            break;
        // decrease toon shading level
        case KeyEvent::KEY_DOWN:
            level--;
            SimpleLightingApp::changeLevelOfDetail();
            break;
        // increase / decrease depth
        case KeyEvent::KEY_d:
            circleDepth += (event.isAltDown()) ? -1 : 1;
            circleDepth = glm::clamp(circleDepth, 0, maxCircleDepth);
            for (auto& shape : model->shapes)
                shape->material()->uniform("circleDepth", circleDepth);
            break;
        // increase / decrease size of circles
        case KeyEvent::KEY_s:
            circleSize += (event.isAltDown()) ? -0.01 : 0.01;
            circleSize = glm::clamp(circleSize, 0.0f, 1.0f);
            for (auto& shape : model->shapes)
                shape->material()->uniform("circleSize", circleSize);
            break;
        // change red of circle's color
        case KeyEvent::KEY_r:
            color_red += (event.isAltDown()) ? -0.01 : 0.01;
            color_red = glm::clamp(color_red, 0.0, 1.0);
            updateCircleColor();
            break;
        // change green of circle's color
        case KeyEvent::KEY_g:
            color_green += (event.isAltDown()) ? -0.01 : 0.01;
            color_green = glm::clamp(color_green, 0.0, 1.0);
            updateCircleColor();
            break;
        // change blue of circle's color
        case KeyEvent::KEY_b:
            color_blue += (event.isAltDown()) ? -0.01 : 0.01;
            color_blue = glm::clamp(color_blue, 0.0, 1.0);
            updateCircleColor();
            break;
        // log key, if there is no function bound
        default:
            cout << "pressed key: " << keyCode << "\r\n";
            break;
    }
}

// updates level of detail for toon shader
void SimpleLightingApp::changeLevelOfDetail() {
    level = glm::clamp(level, minLevel, maxLevel);
    for (auto& shape : model->shapes)
        shape->material()->uniform("levels", level);
}

// updates color of circle in ex_2.3
void SimpleLightingApp::updateCircleColor() {
    vec3 color = vec3(color_red, color_green, color_blue);
    for (auto& shape : model->shapes)
        shape->material()->uniform("circleColor", color);
}

// Place all one-time setup code here.
void SimpleLightingApp::setup() {
    // Arange for the window to always be on top. This helps with live shader coding
    getWindow()->setAlwaysOnTop();
    SimpleLightingApp::ex2_1();
}

void SimpleLightingApp::ex2_1() {
    // Create a live-reloading shader program.
    auto phong = rtr::watcher.createWatchedProgram({ getAssetPath("phong.vert"), getAssetPath("phong.frag") });
    
    // set phong material
    rtr::MaterialRef material = rtr::Material::create(phong);
    material->uniform("k_diffuse", vec3(0.85,0.0,0.0));
    material->uniform("k_specular", vec3(0.6, 0.6, 0.6));
    material->uniform("shininess", float(64));
    material->uniform("lightPositionEC", vec4(0,3,3,1));
    material->uniform("lightColor", vec3(1,1,1));
    
    auto teapot = ci::geom::Teapot().subdivisions(40);
    auto s_teapot = rtr::Shape::create({teapot}, material);
    
    model = rtr::Model::create({s_teapot});
}

void SimpleLightingApp::ex2_2() {
    // Create a live-reloading shader program.
    auto toon = rtr::watcher.createWatchedProgram({ getAssetPath("toon.vert"), getAssetPath("toon.frag") });
    
    // set phong material
    rtr::MaterialRef material_1 = rtr::Material::create(toon);
    material_1->uniform("k_diffuse", vec3(0.85,0.0,0.0));
    material_1->uniform("k_specular", vec3(0.6, 0.6, 0.6));
    material_1->uniform("shininess", float(64));
    material_1->uniform("lightPositionEC", vec4(0,3,3,1));
    material_1->uniform("lightColor", vec3(1,1,1));
    // add level uniform to adjust level live
    material_1->uniform("levels", level);
    
    rtr::MaterialRef material_2 = rtr::Material::create(toon);
    material_2->uniform("k_diffuse", vec3(0.47,0.41,0.34));
    material_2->uniform("k_specular", vec3(0.6, 0.6, 0.6));
    material_2->uniform("shininess", float(16));
    material_2->uniform("lightPositionEC", vec4(0,3,3,1));
    material_2->uniform("lightColor", vec3(1,1,1));
    // add level uniform to adjust level live
    material_2->uniform("levels", level);
    
    auto teapot = ci::geom::Teapot().subdivisions(40);
    auto torus = ci::geom::Torus().subdivisionsAxis(40).subdivisionsHeight(40);
    
    auto s_teapot = rtr::Shape::create({teapot}, material_1);
    auto s_torus = rtr::Shape::create({torus}, material_2);
    
    model = rtr::Model::create({s_teapot, s_torus});
}

void SimpleLightingApp::ex2_3() {
    // Create a live-reloading shader program.
    auto mat = rtr::watcher.createWatchedProgram({ getAssetPath("toonTextures.vert"), getAssetPath("toonTextures.frag") });
    
    // set phong material
    rtr::MaterialRef material = rtr::Material::create(mat);
    material->uniform("k_diffuse", vec3(0.85,0.0,0.0));
    material->uniform("k_specular", vec3(0.6, 0.6, 0.6));
    material->uniform("shininess", float(64));
    material->uniform("lightPositionEC", vec4(0,3,3,1));
    material->uniform("lightColor", vec3(1,1,1));
    // add level uniform to adjust level live
    material->uniform("levels", level);
    material->uniform("circleColor", vec3(color_red, color_green, color_blue));
    material->uniform("circleDepth", circleDepth);
    material->uniform("circleSize", circleSize);

    auto cube = ci::geom::Cube().subdivisions(40);
    auto s_cube = rtr::Shape::create({cube}, material);
    
    model = rtr::Model::create({s_cube});
}

void SimpleLightingApp::ex2_4() {
    // Create a live-reloading shader program.
    auto mat = rtr::watcher.createWatchedProgram({ getAssetPath("chess.vert"), getAssetPath("chess.frag") });
    
    // set phong material
    rtr::MaterialRef material = rtr::Material::create(mat);
    material->uniform("k_diffuse", vec3(0.85,0.0,0.0));
    material->uniform("k_specular", vec3(0.6, 0.6, 0.6));
    material->uniform("shininess", float(64));
    material->uniform("lightPositionEC", vec4(0,3,3,1));
    material->uniform("lightColor", vec3(1,1,1));

    float now = getElapsedSeconds();
    
    material->uniform("depth", 5);
    material->uniform("time", now);

    
    auto cube = ci::geom::Cube().subdivisions(40);
    auto s_cube = rtr::Shape::create({cube}, material);
    
    model = rtr::Model::create({s_cube});
}

// Place all non-OpenGL once-per-frame code here.
void SimpleLightingApp::update() {
    // Calculate elapsed time since last frame.
    double now = getElapsedSeconds();
    double elapsed = now - lastTime;
    lastTime = now;

    // Animate the rotation angle.
    angle += (M_PI / 10) * elapsed;

    // Check now whether any files changed.
    rtr::watcher.checkForChanges();
}

// Place all once-per-frame drawing code here.
void SimpleLightingApp::draw() {
    // Clear background to gray.
    gl::clear(Color(0.5, 0.5, 0.5));

    // Setup a perspective projection camera.
    CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 10.0f);
    camera.lookAt(vec3(0, 3, 3), vec3(0, 0, 0));

    // Push the view-projection matrix to the bottom of the matrix stack.
    gl::setMatrices(camera);

    // Enable depth buffering.
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // Save current model-view-projection matrix by pushing a new matrix on top.
    gl::pushModelMatrix();

    // Apply the rotation around the diagonal unit axis.
    gl::rotate(angle, vec3(0, 1, 0));

    float now = getElapsedSeconds();
    for (auto& shape : model->shapes)
        shape-> material()->uniform("time", now);
    
    // draw model
    model->draw();

    // Restore the previous model-view-projection matrix.
    gl::popModelMatrix();
}

void prepareSettings(SimpleLightingApp::Settings* settings) {
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(SimpleLightingApp, RendererGl, prepareSettings)
