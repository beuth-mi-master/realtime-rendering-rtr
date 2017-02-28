#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"

#include "RTR/RTR.h"

using namespace ci;
using namespace ci::app;
using namespace ci::geom;
using namespace ci::gl;
using namespace gl;
using namespace glm;
using namespace rtr;
using namespace std;

class MultipassApp : public App {
    
  public:
    
    void setup() override;
    void update() override;
    void draw() override;
    void resize() override;
    
    void keyDown(KeyEvent event) override;
    
    void renderSceneToFbo();
    void renderSceneToFilteredFbo();
    void renderHorizontalBlurToFbo();
    void renderVerticalBlurToFbo();
    void combineBlurWithSceneToBloom();
    void createFBOs();
    
    vec4 rotateLight(vec4 lightPosition, double elapsed, double factor, int speed);
    
  private:
    
    const Font font = Font("Arial", 16.0f);
    const Font font2 = Font("Arial", 24.0f);

    
    rtr::NodeRef scene;
    rtr::NodeRef sceneFiltered;

    rtr::MaterialRef materialGreen;
    rtr::MaterialRef materialRed;
    rtr::MaterialRef materialBloom;
    rtr::MaterialRef materialFilteredGreen;
    rtr::MaterialRef materialFilteredRed;

    CameraPersp camera;
    CameraUi cameraUi;
    
    vec3 cameraPosition = vec3(5.0f, 5.0f, 5.0f);
    vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    const float fieldOfView = 70.0f;
    const float nearestClip = 0.01f;
    const float farestClip = 100.0f;
        
    vec4 light1 = vec4(0.0, 0.5, 0.0, 1.0);
    Color light1Color = Color(1.0, 1.0, 1.0);
    vec4 light2 = vec4(0.0, 2.5, 0.0, 1.0);
    Color light2Color = Color(1, 1, 1);
    
    Color backgroundColor = Color(0.1, 0.1, 0.1);
    
    gl::FboRef fbo;
    gl::FboRef fboFiltered;

    const int previewSize = 2;
    int FBO_WIDTH = getWindowWidth();
    int FBO_HEIGHT = getWindowHeight();
    Color FBO_BG_COLOR = Color(0.0f, 0.0f, 0.0f);
    
    gl::FboRef fboBlurHorizontal;
    gl::FboRef fboBlurVertical;
    gl::FboRef fboBloom;
    int BLURSIZE = 128;
    float ATTENTUATION = 1.2f;
    float BLURRADIUS = 1.0f;
    
    GlslProgRef phongShader;
    GlslProgRef blurShader;
    GlslProgRef bloomShader;
    GlslProgRef brightnessFilterShader;
    
    double lastTime;
    double angle;
    
    float brightnessfilter = 0.2;
    
    bool shouldColorChange = true;
    
    vec2 planeSize = vec2(12);
    
    bool debug = true;
    
    params::InterfaceGlRef mParams;
    bool paramsOn = false;
    
};

void MultipassApp::keyDown(KeyEvent event) {
    const int keyCode = event.getCode();
    switch (keyCode) {
        case KeyEvent::KEY_UP:
            BLURSIZE = clamp(++BLURSIZE, 1, 512);
            this->resize();
            break;
        case KeyEvent::KEY_DOWN:
            BLURSIZE = clamp(--BLURSIZE, 1, 512);
            this->resize();
            break;
        case KeyEvent::KEY_RIGHT:
            ATTENTUATION += 0.1f;
            ATTENTUATION = clamp(ATTENTUATION, 0.0f, 10.0f);
            break;
        case KeyEvent::KEY_LEFT:
            ATTENTUATION -= 0.1f;
            ATTENTUATION = clamp(ATTENTUATION, 0.0f, 10.0f);
            break;
        case KeyEvent::KEY_k:
            BLURRADIUS -= 0.1f;
            BLURRADIUS = clamp(BLURRADIUS, 0.0f, 10.0f);
            break;
        case KeyEvent::KEY_i:
            BLURRADIUS += 0.1f;
            BLURRADIUS = clamp(BLURRADIUS, 0.0f, 10.0f);
            break;
        case KeyEvent::KEY_h:
            paramsOn = !paramsOn;
            (paramsOn) ? mParams->show() : mParams->hide();
            break;
        case KeyEvent::KEY_ESCAPE:
            quit();
            break;
        case KeyEvent::KEY_w:
            if (event.isMetaDown()) quit();
            break;
        default:
            cout << "keydown: " << keyCode << endl;
            break;
    }
}

void MultipassApp::setup() {
    
    BLURSIZE = 128;
    
    phongShader = rtr::watcher.createWatchedProgram({
        getAssetPath("phong.vert"),
        getAssetPath("phong.frag")
    });
    
    brightnessFilterShader = rtr::watcher.createWatchedProgram({
        getAssetPath("brightness_filter.vert"),
        getAssetPath("brightness_filter.frag")
    });
    
    blurShader = rtr::watcher.createWatchedProgram({
        getAssetPath("blur.vert"),
        getAssetPath("blur.frag")
    });
    
    bloomShader = rtr::watcher.createWatchedProgram({
        getAssetPath("bloom.vert"),
        getAssetPath("bloom.frag")
    });
    
    materialBloom = rtr::Material::create(bloomShader);

    materialGreen = rtr::Material::create(phongShader);
    materialGreen->uniform("k_specular", vec3(1.0, 1.0, 1.0));
    materialGreen->uniform("shininess", float(128));
    materialGreen->uniform("light1", light1);
    materialGreen->uniform("light1Color", light1Color);
    materialGreen->uniform("light2", light2);
    materialGreen->uniform("light2Color", light2Color);
    materialGreen->uniform("k_diffuse", vec3(0.0, 0.8, 0.0));
    materialGreen->uniform("resolution", vec2(getWindowSize()));
    materialGreen->uniform("time",  float(getElapsedSeconds()));
    materialGreen->uniform("shouldColorChange", false);
    materialGreen->uniform("color1", vec4(1.0, 0.0, 0.0, 1.0));
    materialGreen->uniform("color2", vec4(0.0, 0.0, 1.0, 1.0));
    
    materialRed = rtr::Material::create(phongShader);
    materialRed->uniform("k_specular", vec3(1.0, 1.0, 1.0));
    materialRed->uniform("shininess", float(128));
    materialRed->uniform("light1", light1);
    materialRed->uniform("light1Color", light1Color);
    materialRed->uniform("light2", light2);
    materialRed->uniform("light2Color", light2Color);
    materialRed->uniform("k_diffuse", vec3(0.8, 0.0, 0.0));
    materialRed->uniform("resolution", vec2(getWindowSize()));
    materialRed->uniform("time",  float(getElapsedSeconds()));
    materialRed->uniform("shouldColorChange", shouldColorChange);
    materialRed->uniform("color1", vec4(1.0, 0.0, 0.0, 1.0));
    materialRed->uniform("color2", vec4(0.0, 0.0, 1.0, 1.0));
    
    materialFilteredGreen = rtr::Material::create(brightnessFilterShader);
    materialFilteredGreen->uniform("k_specular", vec3(1.0, 1.0, 1.0));
    materialFilteredGreen->uniform("shininess", float(128));
    materialFilteredGreen->uniform("light1", light1);
    materialFilteredGreen->uniform("light1Color", light1Color);
    materialFilteredGreen->uniform("light2", light2);
    materialFilteredGreen->uniform("light2Color", light2Color);
    materialFilteredGreen->uniform("k_diffuse", vec3(0.0, 0.8, 0.0));
    materialFilteredGreen->uniform("resolution", vec2(getWindowSize()));
    materialFilteredGreen->uniform("time",  float(getElapsedSeconds()));
    materialFilteredGreen->uniform("shouldColorChange", false);
    materialFilteredGreen->uniform("color1", vec4(1.0, 0.0, 0.0, 1.0));
    materialFilteredGreen->uniform("color2", vec4(0.0, 0.0, 1.0, 1.0));
    materialFilteredGreen->uniform("brightnessfilter", brightnessfilter);
    
    materialFilteredRed = rtr::Material::create(brightnessFilterShader);
    materialFilteredRed->uniform("k_specular", vec3(1.0, 1.0, 1.0));
    materialFilteredRed->uniform("shininess", float(128));
    materialFilteredRed->uniform("light1", light1);
    materialFilteredRed->uniform("light1Color", light1Color);
    materialFilteredRed->uniform("light2", light2);
    materialFilteredRed->uniform("light2Color", light2Color);
    materialFilteredRed->uniform("k_diffuse", vec3(0.8, 0.0, 0.0));
    materialFilteredRed->uniform("resolution", vec2(getWindowSize()));
    materialFilteredRed->uniform("time",  float(getElapsedSeconds()));
    materialFilteredRed->uniform("shouldColorChange", shouldColorChange);
    materialFilteredRed->uniform("color1", vec4(1.0, 0.0, 0.0, 1.0));
    materialFilteredRed->uniform("color2", vec4(0.0, 0.0, 1.0, 1.0));
    materialFilteredRed->uniform("brightnessfilter", brightnessfilter);

    
    ci::geom::Sphere sphere = ci::geom::Sphere().subdivisions(32);
    sphere.radius(0.5);
    
    vec3 spherePositions[14] = {
        vec3(0, 0.5, 0),
        vec3((planeSize.x * (1.0f/2.5f)), 0.5, 0),
        vec3(-(planeSize.x * (1.0f/2.5f)), 0.5, 0),
        vec3(0, 0.5, (planeSize.x * (1.0f/2.5f))),
        vec3(0, 0.5, -(planeSize.x * (1.0f/2.5f))),
        vec3((planeSize.x * (1.0f/3.5f)), 2, 0),
        vec3(-(planeSize.x * (1.0f/3.5f)), 2, 0),
        vec3(0, 2, (planeSize.x * (1.0f/3.5f))),
        vec3(0, 2, -(planeSize.x * (1.0f/3.5f))),
        vec3((planeSize.x * (1.0f/7.0f)), 3.5, 0),
        vec3(-(planeSize.x * (1.0f/7.0f)), 3.5, 0),
        vec3(0, 3.5, (planeSize.x * (1.0f/7.0f))),
        vec3(0, 3.5, -(planeSize.x * (1.0f/7.0f))),
        vec3(0, 5, 0)
    };
    
    std::vector<NodeRef> nodes;
    
    mat4 planeTransform;
    const ci::geom::Plane plane = ci::geom::Plane().size(planeSize);
    const rtr::ShapeRef s_plane = rtr::Shape::create({plane}, materialGreen);
    rtr::ModelRef planeModel = Model::create({s_plane});
    planeTransform = translate(planeTransform, vec3(0, -0.5, 0));
    NodeRef planeNode = Node::create({planeModel}, planeTransform);
    
    nodes.push_back(planeNode);
    for (int i = 0; i < (sizeof(spherePositions)/sizeof(spherePositions[0])); i++) {
        vec3 pos = spherePositions[i];
        mat4 sphereTransform;
        const rtr::ShapeRef sphereShape = rtr::Shape::create({sphere}, materialRed);
        rtr::ModelRef sphereModel = Model::create({sphereShape});
        sphereTransform = translate(sphereTransform, pos);
        NodeRef sphereNode = Node::create({sphereModel}, sphereTransform);
        nodes.push_back(sphereNode);
    }

    scene = Node::create({}, mat4(), nodes);
    
    std::vector<NodeRef> nodes2;
    
    mat4 planeTransform2;
    const ci::geom::Plane plane2 = ci::geom::Plane().size(planeSize);
    const rtr::ShapeRef s_plane2 = rtr::Shape::create({plane}, materialFilteredGreen);
    rtr::ModelRef planeModel2 = Model::create({s_plane2});
    planeTransform2 = translate(planeTransform2, vec3(0, -0.5, 0));
    NodeRef planeNode2 = Node::create({planeModel2}, planeTransform2);
    
    nodes2.push_back(planeNode2);
    for (int i = 0; i < (sizeof(spherePositions)/sizeof(spherePositions[0])); i++) {
        vec3 pos = spherePositions[i];
        mat4 sphereTransform;
        const rtr::ShapeRef sphereShape = rtr::Shape::create({sphere}, materialFilteredRed);
        rtr::ModelRef sphereModel = Model::create({sphereShape});
        sphereTransform = translate(sphereTransform, pos);
        NodeRef sphereNode = Node::create({sphereModel}, sphereTransform);
        nodes2.push_back(sphereNode);
    }
    
    sceneFiltered = Node::create({}, mat4(), nodes2);
    
    mParams = params::InterfaceGl::create(getWindow(), "Bloom", ivec2(400, 300));
    mParams->setOptions("", "valueswidth=50");
    mParams->addParam("Attentuation", &ATTENTUATION);
    mParams->addParam("Size", &BLURSIZE);
    mParams->addParam("Radius", &BLURRADIUS);
    mParams->addParam("Brightness threshold", &brightnessfilter);
    mParams->addSeparator();
    mParams->addParam("Color change", &shouldColorChange);
    mParams->addParam("Background color", &FBO_BG_COLOR);

    mParams->hide();

    camera = CameraPersp(getWindowWidth(), getWindowHeight(), fieldOfView, nearestClip, farestClip);
    camera.lookAt(cameraPosition, cameraTarget, vec3(0, 1, 0));
    cameraUi = CameraUi(&camera, getWindow());
    
}

void MultipassApp::resize() {
    FBO_WIDTH = getWindowWidth();
    FBO_HEIGHT = getWindowHeight();
    createFBOs();
};

void MultipassApp::createFBOs() {
    gl::Fbo::Format format;
    fboFiltered = gl::Fbo::create(FBO_WIDTH, FBO_HEIGHT, format.colorTexture());
    fbo = gl::Fbo::create(FBO_WIDTH, FBO_HEIGHT, format.colorTexture());
    fboBlurHorizontal = gl::Fbo::create(BLURSIZE, BLURSIZE, format.colorTexture());
    fboBlurVertical = gl::Fbo::create(BLURSIZE, BLURSIZE, format.colorTexture());
    fboBloom = gl::Fbo::create(FBO_WIDTH, FBO_HEIGHT, format.colorTexture());
}

void MultipassApp::renderSceneToFilteredFbo() {
    pushMatrices();
    glViewport(0, 0, FBO_WIDTH , FBO_HEIGHT);
    fboFiltered->bindFramebuffer();
    clear(FBO_BG_COLOR);
    setMatrices(camera);
    enableDepthWrite();
    enableDepthRead();
    pushModelMatrix();
    sceneFiltered->draw();
    popModelMatrix();
    popMatrices();
    fboFiltered->unbindFramebuffer();
}


void MultipassApp::renderSceneToFbo() {
    pushMatrices();
    glViewport(0, 0, FBO_WIDTH , FBO_HEIGHT);
    fbo->bindFramebuffer();
    clear(FBO_BG_COLOR);
    setMatrices(camera);
    enableDepthWrite();
    enableDepthRead();
    pushModelMatrix();
    scene->draw();
    popModelMatrix();
    popMatrices();
    fbo->unbindFramebuffer();
}

void MultipassApp::renderHorizontalBlurToFbo() {
    blurShader->uniform( "sample_offset", vec2( 1.0f / fboBlurHorizontal->getWidth(), 0.0f ) );
    blurShader->uniform( "attenuation", ATTENTUATION );
    blurShader->uniform( "radius", BLURRADIUS);
    blurShader->bind();
    fboBlurHorizontal->bindFramebuffer();
    
    pushMatrices();
    clear(FBO_BG_COLOR);
    setMatricesWindow(BLURSIZE, BLURSIZE);
    glViewport(0, 0, BLURSIZE, BLURSIZE);
    Texture2dRef fboColorTexture = fboFiltered->getColorTexture();
    fboColorTexture->bind();
    gl::drawSolidRect( fboBlurHorizontal->getBounds() );
    popMatrices();
    
    fboBlurHorizontal->unbindFramebuffer();
}

void MultipassApp::renderVerticalBlurToFbo() {
    blurShader->uniform( "sample_offset", vec2( 0.0f, 1.0f / fboBlurVertical->getHeight() ) );
    blurShader->uniform( "attenuation", ATTENTUATION );
    blurShader->uniform( "radius", BLURRADIUS);
    blurShader->bind();
    fboBlurVertical->bindFramebuffer();
    
    pushMatrices();
    clear(FBO_BG_COLOR);
    setMatricesWindow(BLURSIZE, BLURSIZE);
    glViewport(0, 0, BLURSIZE, BLURSIZE);
    Texture2dRef fboColorTexture = fboBlurHorizontal->getColorTexture();
    fboColorTexture->bind();
    gl::drawSolidRect( fboBlurVertical->getBounds() );
    popMatrices();
    
    fboBlurVertical->unbindFramebuffer();
}

void MultipassApp::combineBlurWithSceneToBloom() {
    pushMatrices();
    glViewport(0, 0, FBO_WIDTH , FBO_HEIGHT);
    fboBloom->bindFramebuffer();
    clear(FBO_BG_COLOR);
    setMatricesWindow(FBO_WIDTH, FBO_HEIGHT);
    
    materialBloom->texture("uTex0", fbo->getColorTexture());
    materialBloom->texture("uTex1", fboBlurVertical->getColorTexture());
    
    materialBloom->bind();
    
    gl::drawSolidRect( fboBloom->getBounds() );
    popMatrices();
    fboBloom->unbindFramebuffer();
}

vec4 MultipassApp::rotateLight(vec4 lightPosition, double elapsed, double factor, int speed) {
    angle += (M_PI / 5) * elapsed;
    lightPosition[0] = factor * cos(angle * speed);
    lightPosition[2] = factor * sin(angle * speed);
    return lightPosition;
}

void MultipassApp::update() {
    
    double now = getElapsedSeconds();
    double elapsed = now - lastTime;
    lastTime = now;
    
    light1 = rotateLight(light1, elapsed, 4, -2);
    light2 = rotateLight(light2, elapsed, 1.5, 1);
    
    watcher.checkForChanges();
}

void MultipassApp::draw() {
    clear(FBO_BG_COLOR);

    setMatrices(camera);

    enableDepthWrite();
    enableDepthRead();

    materialRed->uniform("light1", camera.getViewMatrix() * light1);
    materialRed->uniform("light2", camera.getViewMatrix() * light2);
    materialGreen->uniform("light1", camera.getViewMatrix() * light1);
    materialGreen->uniform("light2", camera.getViewMatrix() * light2);
    materialFilteredRed->uniform("light1", camera.getViewMatrix() * light1);
    materialFilteredRed->uniform("light2", camera.getViewMatrix() * light2);
    materialFilteredGreen->uniform("light1", camera.getViewMatrix() * light1);
    materialFilteredGreen->uniform("light2", camera.getViewMatrix() * light2);
    
    materialRed->uniform("shouldColorChange", shouldColorChange);
    materialFilteredRed->uniform("shouldColorChange", shouldColorChange);
    
    materialFilteredGreen->uniform("brightnessfilter", brightnessfilter);
    materialFilteredRed->uniform("brightnessfilter", brightnessfilter);

    materialGreen->uniform("time",  float(getElapsedSeconds()));
    materialRed->uniform("time",  float(getElapsedSeconds()));
    materialFilteredGreen->uniform("time",  float(getElapsedSeconds()));
    materialFilteredRed->uniform("time",  float(getElapsedSeconds()));
    
    renderSceneToFbo();
    renderSceneToFilteredFbo();

    renderHorizontalBlurToFbo();
    renderVerticalBlurToFbo();
    combineBlurWithSceneToBloom();
    
    if (debug) {
        ShaderDef color = gl::ShaderDef().color();
        GlslProgRef shader = gl::getStockShader(color);
        
        /*
         * draw light positions
         */
        pushModelMatrix();
        shader->bind();
        gl::color(Color(1, 1, 1));
        gl::translate(light1.x, light1.y, light1.z);
        gl::drawSphere(vec3{ 0, 0, 0 }, 0.2f);
        popModelMatrix();
        pushModelMatrix();
        shader->bind();
        gl::color(Color(1, 1, 1));
        gl::translate(light2.x, light2.y, light2.z);
        gl::drawSphere(vec3{ 0, 0, 0 }, 0.1f);
        popModelMatrix();

    }
    
    glViewport(0, 0, getWindowWidth() * previewSize, getWindowHeight() * previewSize);
    
    double w = FBO_WIDTH;
    double h = FBO_HEIGHT;
    
    gl::setMatricesWindow(toPixels(getWindowSize()));
    gl::drawString("Bloom", vec2(getWindowWidth()/2, getWindowHeight()/2) + font.getSize()/2, Color::white(), font2);
    gl::drawString("Original", vec2(0, 0 + font2.getSize()/2), Color::white(), font2);
    gl::drawString("Brightness Filter", vec2(w, 0 + font2.getSize()/2), Color::white(), font2);
    gl::drawString("Blur horizontal", vec2(0, getWindowHeight()*2 - font2.getSize()), Color::white(), font2);
    gl::drawString("Blur vertical + horizontal", vec2(w, getWindowHeight()*2 - font2.getSize()), Color::white(), font2);

    gl::draw(fboBloom->getColorTexture(), Rectf(getWindowWidth()/2, getWindowHeight()/2, w + getWindowWidth()/2, h + getWindowHeight()/2));
    gl::draw(fbo->getColorTexture(), Rectf(0, 0, w , h));
    gl::draw(fboBlurHorizontal->getColorTexture(), Rectf(0, 0 + h, w , h + h));
    gl::draw(fboBlurVertical->getColorTexture(), Rectf(0 + w, 0 + h, w + w, h + h));
    gl::draw(fboFiltered->getColorTexture(), Rectf(0 + w , 0, w + w, h));
    
    gl::disableDepthRead();
    gl::setMatricesWindow(getWindowSize());
    
    mParams->draw();
    
    gl::drawString("fps: " + to_string((int)round(getAverageFps())), vec2(10.0f, getWindowSize().y - font.getSize() - 10.0f), Color::white(), font);
    
}

void prepareSettings(MultipassApp::Settings* settings) {
    settings->setHighDensityDisplayEnabled(true);
    settings->setWindowSize(1200, 800);
    settings->setFrameRate(60.0f);
}

CINDER_APP(MultipassApp, RendererGl, prepareSettings)
