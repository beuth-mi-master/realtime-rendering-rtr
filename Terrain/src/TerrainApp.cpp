#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/gl.h"
#include "RTR/RTR.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace cinder::gl;

// App template for the Real Time Rendering course, inherits from Cinder App.
class TerrainApp : public App {
  public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyDown(KeyEvent event) override;
    void keyUp(KeyEvent event) override;
    void toggleDisplacement();
    void changeDisplacementEffectScale(float change);
    void mouseDrag(MouseEvent event) override;
    void mouseDown(MouseEvent event) override;

  private:
    
    // font definition
    const Font font = Font("Arial", 16.0f);
    
    // toggle displacement
    bool isDisplacementEnabled = true;
    
    // change effect of displacement
    float displacementEffectScale = 0.5f;
    
    // Terrain
    rtr::MaterialRef terrainMaterial;
    rtr::ModelRef terrainModel;
    
    // Skybox
    rtr::MaterialRef materialSkybox;
    rtr::ModelRef skyboxModel;
    
    // Camera
    ci::CameraPersp camera;
    
    // delta timing
    double lastTime = getElapsedSeconds();
    const float bestDeltaTiming = 1000.0f / 60.0f;
    
    // movement speed min/max/current
    const float minSpeed = 0.0f;
    const float maxSpeed = 0.01f;
    float movementSpeed = 0.0025f;
    float rotationSpeed = 0.01f;
    float velocityFactor = 0.0f;
    
    // velocity of camera
    int velocity = 1;
    int rollVelocity = 0;
    
    // where is the camera
    vec3 cameraPosition = vec3(0.0f, 0.3f, 0.0f);
    
    // where is the texture
    vec3 texturePosition = cameraPosition;

    // height of textures on displaced plane
    // min, max and size of transition
    const vec3 grassHeight = vec3(0.0, 0.35, 0.00);
    const vec3 stoneHeight = vec3(0.35, 0.5, 0.05);
    const vec3 snowHeight = vec3(0.5, 1.00, 0.02);
    
    // mouse controls
    vec2 lastMousePosition;
    const vec2 mouseSensitivity = vec2(0.0020f, 0.0020f);
    
    // camera quaternations
    quat camera_quat;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
    
};

void TerrainApp::setup() {
    
    /*
     * TERRAIN
     */
    ci::gl::GlslProgRef terrainTexture = rtr::watcher.createWatchedProgram({
        getAssetPath("terrainTexture.vert"),
        getAssetPath("terrainTexture.frag")
    });
    
    terrainMaterial = rtr::Material::create(terrainTexture);
    terrainMaterial->uniform("wcLightPosition", vec4(-0.9f, 1.5f, 1.5f, 1.0));
    
    Texture2dRef texNormal = Texture2d::create(loadImage(getAssetPath("normal-map.png")));
    texNormal->setWrap(GL_REPEAT, GL_REPEAT);
    Texture2dRef texDisplacement = gl::Texture2d::create(loadImage(getAssetPath("displacement-map.png")));
    texDisplacement->setWrap(GL_REPEAT, GL_REPEAT);
    Texture2dRef texStone = Texture2d::create(loadImage(getAssetPath("stone.jpg")));
    texStone->setWrap(GL_REPEAT, GL_REPEAT);
    Texture2dRef texGrass = Texture2d::create(loadImage(getAssetPath("grass.jpg")));
    texGrass->setWrap(GL_REPEAT, GL_REPEAT);
    Texture2dRef texSnow = Texture2d::create(loadImage(getAssetPath("snow.jpg")));
    texSnow->setWrap(GL_REPEAT, GL_REPEAT);

    // normal and displacement textures
    terrainMaterial->texture("normalTexture", texNormal);
    terrainMaterial->texture("displacementTexture", texDisplacement);
    
    // stone texture
    terrainMaterial->texture("stoneTexture", texStone);
    terrainMaterial->uniform("stoneHeight", stoneHeight);

    // grass texture
    terrainMaterial->texture("grassTexture", texGrass);
    terrainMaterial->uniform("grassHeight", grassHeight);
    
    // snow texture
    terrainMaterial->texture("snowTexture", texSnow);
    terrainMaterial->uniform("snowHeight", snowHeight);

    terrainMaterial->uniform("isDisplacementEnabled", isDisplacementEnabled);
    terrainMaterial->uniform("displacementEffectScale", displacementEffectScale);
    
    ci::geom::Plane plane = ci::geom::Plane().subdivisions(vec2(1024));
    const rtr::ShapeRef s_plane = rtr::Shape::create({plane}, terrainMaterial);
    terrainModel = rtr::Model::create({s_plane});
    
    /*
     * SKYBOX
     */
    TextureCubeMapRef skyboxCubeMap = TextureCubeMap::create(loadImage(loadAsset("skybox.jpg")));
    
    ci::gl::GlslProgRef skyboxTexture = rtr::watcher.createWatchedProgram({
        getAssetPath("skybox.vert"),
        getAssetPath("skybox.frag")
    });
    
    materialSkybox = rtr::Material::create(skyboxTexture);
    materialSkybox->texture("cubeMapTex", skyboxCubeMap);
    
    ci::geom::Cube cube = ci::geom::Cube();
    cube.size(vec3(0.03f));
    const rtr::ShapeRef s_cube = rtr::Shape::create({cube}, materialSkybox);
    skyboxModel = rtr::Model::create({s_cube});
    
    /* 
     * CAMERA
     */
    camera = CameraPersp(getWindowWidth(), getWindowHeight(), 35.0f, 0.01f, 1.0f);
    camera.lookAt(cameraPosition, cameraPosition + vec3(1.0f, 0.0f, 0.8f), vec3(0.0f, 1.0f, 0.0f));
    camera_quat = camera.getOrientation();
}

// Place all non-OpenGL once-per-frame code here.
void TerrainApp::update() {

    double now = getElapsedSeconds();
    double elapsed = now - lastTime;
    lastTime = now;
    
    float deltaTiming = glm::clamp((elapsed / bestDeltaTiming), 1.0, 4.0);
    terrainMaterial->uniform("deltaTiming", deltaTiming);
    
    /*
     * CAMERA
     */
    camera.setEyePoint(cameraPosition);
    roll = (rotationSpeed * rollVelocity);
    glm::quat key_quat = quat(vec3(pitch, yaw, roll));
    pitch = yaw = roll = 0.0f;
    
    camera_quat = normalize(camera_quat * key_quat);
    camera.setOrientation(camera_quat);
    
    if (velocity != 0) {
        velocityFactor += 0.01f;
        velocityFactor = glm::clamp(velocityFactor, 0.0f, 1.0f);
        vec3 dir = (movementSpeed * velocityFactor) * camera.getViewDirection() * (float)velocity;
        texturePosition += vec3(dir.x, dir.y, dir.z);
        cameraPosition.y += dir.y;
    }
    camera.setEyePoint(cameraPosition);
    terrainMaterial->uniform("texturePosition", texturePosition);

    rtr::watcher.checkForChanges();
}

// Place all once-per-frame drawing code here.
void TerrainApp::draw() {
    
    // Clear background to gray.
    gl::clear(Color(0.5, 0.5, 0.5));

    gl::setMatrices(camera);

    gl::disableDepthWrite();
    gl::enableFaceCulling(false);
    
    gl::pushModelMatrix();
    // set skybox to camera position
    gl::translate(cameraPosition);
    skyboxModel->draw();
    gl::popModelMatrix();
    
    gl::enableFaceCulling(true);
    gl::enableDepthWrite();
    gl::enableDepthRead();

    gl::pushModelMatrix();
    terrainModel->draw();
    gl::popModelMatrix();

    gl::disableDepthRead();
    
    // draw fps debug message
    gl::setMatricesWindow(getWindowSize());
    gl::drawString("fps: " + to_string((int)round(getAverageFps())), vec2(10.0f, getWindowSize().y - font.getSize() - 10.0f), Color::white(), font);
    
}

void prepareSettings(TerrainApp::Settings* settings) {
    settings->setHighDensityDisplayEnabled();
}

// toggles displacement
void TerrainApp::toggleDisplacement() {
    isDisplacementEnabled = !isDisplacementEnabled;
    terrainMaterial->uniform("isDisplacementEnabled", isDisplacementEnabled);
}

// changes effect of displacement
void TerrainApp::changeDisplacementEffectScale(float change) {
    displacementEffectScale += change;
    terrainMaterial->uniform("displacementEffectScale", displacementEffectScale);
}

// store last mouse position of mouse down
void TerrainApp::mouseDown(MouseEvent event) {
    lastMousePosition = event.getPos();
}

// handles mouse dragging
void TerrainApp::mouseDrag(MouseEvent event) {
    vec2 mousePos = event.getPos();
    
    const vec2 diff = lastMousePosition - mousePos;
    
    yaw = mouseSensitivity.x * diff.x;
    pitch = mouseSensitivity.y * diff.y * -1.0f;
    
    lastMousePosition = mousePos;
    cout << camera.getViewDirection() << endl;
}

// Key event handler
void TerrainApp::keyDown(KeyEvent event) {
    int keyCode = event.getCode();
    switch (keyCode) {
        case KeyEvent::KEY_b:
            // increase displacement
            if (event.isShiftDown() && !event.isAltDown()) {
                changeDisplacementEffectScale(0.01f);
            }
            // decrease displacement
            else if (!event.isShiftDown() && event.isAltDown()) {
                changeDisplacementEffectScale(-0.01f);
            }
            // toogle displacement
            else {
                toggleDisplacement();
            }
            break;
        case KeyEvent::KEY_RIGHTBRACKET: // increase movementspeed
            movementSpeed += 0.0001f;
            movementSpeed = glm::clamp(movementSpeed, minSpeed, maxSpeed);
            break;
        case KeyEvent::KEY_SLASH: // decrease movementspeed
            movementSpeed -= 0.0001f;
            movementSpeed = glm::clamp(movementSpeed, minSpeed, maxSpeed);
            break;
        case KeyEvent::KEY_DOWN: // move backwards
            velocity = -1;
            break;
        case KeyEvent::KEY_UP: // move forwards
            velocity = 1;
            break;
        case KeyEvent::KEY_LEFT: // roll left
            rollVelocity = 1;
            break;
        case KeyEvent::KEY_RIGHT: // roll right
            rollVelocity = -1;
            break;
        case KeyEvent::KEY_f: // move forwards
            velocity = (velocity == 0) ? -1 : 0;
            break;
        case KeyEvent::KEY_ESCAPE: // quit
            quit();
        case KeyEvent::KEY_w: // quit
            if (event.isMetaDown()) quit();
            break;
        default: // if there is no match - log it
            cout << "pressed key: " << keyCode << "\r\n";
            break;
    }
}

// when key is released
void TerrainApp::keyUp(KeyEvent event) {
    int keyCode = event.getCode();
    switch (keyCode) {
        case KeyEvent::KEY_DOWN:
        case KeyEvent::KEY_UP: // when keys are released
            velocity = 0;
            break;
        case KeyEvent::KEY_LEFT:
        case KeyEvent::KEY_RIGHT: // when keys are released
            rollVelocity = 0;
            break;
        default: // if there is no match - log it
            cout << "released key: " << keyCode << "\r\n";
            break;
    }
}

CINDER_APP(TerrainApp, RendererGl, prepareSettings)
