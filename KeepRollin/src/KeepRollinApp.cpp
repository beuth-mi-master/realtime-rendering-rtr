#include "KeepRolling.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

void KeepRollinApp::keyDown(KeyEvent event) {
    const int keyCode = event.getCode();
    switch (keyCode) {
        case KeyEvent::KEY_c: {
                if (debugMode) {
                    fs::path p1(getHomeDirectory()/"Desktop/5_scatter1.jpg");
                    fs::path p2(getHomeDirectory()/"Desktop/6_scatter2.jpg");
                    fs::path p3(getHomeDirectory()/"Desktop/0_normal.jpg");
                    fs::path p4(getHomeDirectory()/"Desktop/1_emissive.jpg");
                    fs::path p5(getHomeDirectory()/"Desktop/2_blur-horizontal.jpg");
                    fs::path p6(getHomeDirectory()/"Desktop/3_blur-vertical.jpg");
                    fs::path p7(getHomeDirectory()/"Desktop/4_bloom.jpg");

                    writeImage(p1, fboLightScattering1->getColorTexture()->createSource());
                    writeImage(p2, fboLightScattering2->getColorTexture()->createSource());
                    writeImage(p3, fboNormalAndEmissive->getTexture2d(GL_COLOR_ATTACHMENT0)->createSource());
                    writeImage(p4, fboNormalAndEmissive->getTexture2d(GL_COLOR_ATTACHMENT1)->createSource());
                    writeImage(p5, fboBloomPass1->getColorTexture()->createSource());
                    writeImage(p6, fboBloomPass2->getColorTexture()->createSource());
                    writeImage(p7, fboCombine->getColorTexture()->createSource());
                }
            }
            break;
        case KeyEvent::KEY_s:
            game.start(getElapsedSeconds());
            if (!debugMode) {
                acceleration.z = accelerationForce.z;
            }
            break;
        case KeyEvent::KEY_e:
            if (debugMode) {
                game.end();
            }
            break;
        case KeyEvent::KEY_r:
            if (game.hasEnded()) {
                targetX = 0.0f;
                acceleration = vec3(0);
                velocity = vec3(0);
                speed = vec3(0);
                maxSpeed = initialMaxSpeed;
                ball.setPosition(ballPosition);
                game.reset();
            }
            break;
        case KeyEvent::KEY_UP:
            if (game.isRunning() && debugMode) {
                acceleration.z = accelerationForce.z;
            }
            break;
        case KeyEvent::KEY_DOWN:
            if (game.isRunning() && debugMode) {
                 acceleration.z = -accelerationForce.z;
            }
            break;
        case KeyEvent::KEY_LEFT:
            if (game.isRunning()) {
                 targetX = moveBallX(1, targetX);
            }
            break;
        case KeyEvent::KEY_RIGHT:
            if (game.isRunning()) {
                 targetX = moveBallX(-1, targetX);
            }
            break;
        case KeyEvent::KEY_ESCAPE:
            quit();
            break;
        case KeyEvent::KEY_w:
        case KeyEvent::KEY_q:
            if (event.isMetaDown()) quit();
            break;
        default:
            cout << "key not bound: " << keyCode << endl;
            break;
    }
}

void KeepRollinApp::keyUp(KeyEvent event) {
    const int keyCode = event.getCode();
    switch (keyCode) {
        case KeyEvent::KEY_UP:
        case KeyEvent::KEY_DOWN:
            if (debugMode) {
                acceleration.z = 0;
            }
            break;
        case KeyEvent::KEY_LEFT:
        case KeyEvent::KEY_RIGHT:
            acceleration.x = 0;
            break;
        default:
            break;
    }
}

void KeepRollinApp::resize() {
    windowWidth = getWindowWidth();
    windowHeight = getWindowHeight();
    fboWidth = windowWidth;
    fboHeight = windowHeight;
    fboWidthSmall = fboWidth * downSampling;
    fboHeightSmall = fboHeight * downSampling;
    uiStyle.bounds = ci::Rectf(0, 0, windowWidth, 42.0f);
    createFBOs();
};

void KeepRollinApp::createFBOs() {
    // BLOOM
    gl::Texture2d::Format colorTextureFormat = gl::Texture2d::Format()
		.internalFormat(GL_RGBA8)
		.magFilter(GL_NEAREST)
		.minFilter(GL_NEAREST)
		.wrap(GL_CLAMP_TO_EDGE);
    
    fboNormalAndEmissive = gl::Fbo::create(fboWidth, fboHeight, gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, gl::Texture2d::create(fboWidth, fboHeight, colorTextureFormat))
		.attachment(GL_COLOR_ATTACHMENT1, gl::Texture2d::create(fboWidth, fboHeight, colorTextureFormat))
	);
    
    fboCombine = gl::Fbo::create(fboWidth, fboHeight, gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, gl::Texture2d::create(fboWidth, fboHeight, colorTextureFormat))
	);
    
    fboBloomPass1 = gl::Fbo::create(fboWidthSmall, fboHeightSmall, gl::Fbo::Format().colorTexture());
    fboBloomPass2 = gl::Fbo::create(fboWidthSmall, fboHeightSmall, gl::Fbo::Format().colorTexture());
    
    // SHADOWMAPPING
    gl::Texture2d::Format shadowMapFormat;
    shadowMapFormat.setInternalFormat(GL_DEPTH_COMPONENT16);
	shadowMapFormat.setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
	shadowMapFormat.setMagFilter(GL_LINEAR);
	shadowMapFormat.setMinFilter(GL_LINEAR);
	shadowMapFormat.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    shadowMapFormat.setCompareFunc(GL_LEQUAL);
    
    vec2 shadowFboSize = vec2(fboWidth, fboHeight) * shadowMapScale;
    shadowMapLeft = gl::Texture2d::create(shadowFboSize.x, shadowFboSize.y, shadowMapFormat);
    shadowMapFront = gl::Texture2d::create(shadowFboSize.x, shadowFboSize.y, shadowMapFormat);
    shadowMapRight = gl::Texture2d::create(shadowFboSize.x, shadowFboSize.y, shadowMapFormat);

    gl::Fbo::Format fboShadowMapFormatLeft;
	fboShadowMapFormatLeft.attachment(GL_DEPTH_ATTACHMENT, shadowMapLeft);
	fboShadowMapLeft = gl::Fbo::create(shadowFboSize.x, shadowFboSize.y, fboShadowMapFormatLeft);
    gl::Fbo::Format fboShadowMapFormatFront;
	fboShadowMapFormatFront.attachment(GL_DEPTH_ATTACHMENT, shadowMapFront);
	fboShadowMapFront = gl::Fbo::create(shadowFboSize.x, shadowFboSize.y, fboShadowMapFormatFront);
    gl::Fbo::Format fboShadowMapFormatRight;
	fboShadowMapFormatRight.attachment(GL_DEPTH_ATTACHMENT, shadowMapRight);
	fboShadowMapRight = gl::Fbo::create(shadowFboSize.x, shadowFboSize.y, fboShadowMapFormatRight);
    
    // LIGHT SCATTERING
    vec2 lightScatteringSize = vec2(fboWidth, fboHeight) * lightScatteringScale;
    gl::Fbo::Format lightScatteringFormat = gl::Fbo::Format();
    fboLightScattering1 = gl::Fbo::create(lightScatteringSize.x, lightScatteringSize.y, gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, gl::Texture2d::create(lightScatteringSize.x, lightScatteringSize.y, colorTextureFormat))
	);
    fboLightScattering2 = gl::Fbo::create(lightScatteringSize.x, lightScatteringSize.y, gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, gl::Texture2d::create(lightScatteringSize.x, lightScatteringSize.y, colorTextureFormat))
	);
}

ci::gl::Texture2dRef KeepRollinApp::loadTexture(const fs::path &url) {
    gl::Texture::Format format;
    format.enableMipmapping(true);
    format.setMagFilter(GL_LINEAR);
    format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    format.setWrap(GL_REPEAT, GL_REPEAT);
    ci::gl::Texture2dRef tex = ci::gl::Texture2d::create(loadImage(getAssetPath(url)), format);
    return tex;
}

ci::gl::TextureCubeMapRef KeepRollinApp::loadCubeTexture(const fs::path &url) {
    gl::TextureCubeMap::Format format;
    format.enableMipmapping(true);
    format.setMagFilter(GL_LINEAR);
    format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    return ci::gl::TextureCubeMap::create(loadImage(getAssetPath(url)), format);
}

void KeepRollinApp::setup() {
    
    game = Game(offsetFactorGameboard);
    game.buildWorld(vec2(0.2f, 0.6f));
    
    // SHADER
    colorShader = rtr::watcher.createWatchedProgram({
        getAssetPath("color.vert"),
        getAssetPath("color.frag")
    });
    
    allsparkShader = rtr::watcher.createWatchedProgram({
        getAssetPath("allspark.vert"),
        getAssetPath("allspark.frag")
    });
    
    downsamplingShader = rtr::watcher.createWatchedProgram({
        getAssetPath("downsampling.vert"),
        getAssetPath("downsampling.frag")
    });
    
    blurShader = rtr::watcher.createWatchedProgram({
        getAssetPath("blur.vert"),
        getAssetPath("blur.frag")
    });
    
    bloomShader = rtr::watcher.createWatchedProgram({
        getAssetPath("bloom.vert"),
        getAssetPath("bloom.frag")
    });
    
    ballShader = rtr::watcher.createWatchedProgram({
        getAssetPath("ball.vert"),
        getAssetPath("ball.frag")
    });
    
    skyboxShader = rtr::watcher.createWatchedProgram({
        getAssetPath("skybox.vert"),
        getAssetPath("skybox.frag")
    });
    
    shadowShader = gl::getStockShader(gl::ShaderDef());

    scatteringShader = rtr::watcher.createWatchedProgram({
        getAssetPath("scattering.vert"),
        getAssetPath("scattering.frag")
    });
    
    occlusionShader = rtr::watcher.createWatchedProgram({
        getAssetPath("occlusion.vert"),
        getAssetPath("occlusion.frag")
    });

    // TEXTURES
    ci::gl::Texture2dRef tex_metal = loadTexture("floor_d.tga");
    ci::gl::Texture2dRef tex_metal_n = loadTexture("floor_n.tga");
    ci::gl::Texture2dRef tex_ball = loadTexture("ball_diffuse.jpg");
    ci::gl::Texture2dRef tex_box = loadTexture("box_d.tga");
    ci::gl::Texture2dRef tex_box_n = loadTexture("box_n.tga");
    ci::gl::Texture2dRef tex_ground = loadTexture("ground_d.tga");
    ci::gl::Texture2dRef tex_ground_n = loadTexture("ground_n.tga");
    ci::gl::Texture2dRef tex_booster = loadTexture("booster_d.jpg");
    ci::gl::Texture2dRef tex_booster_n = loadTexture("booster_n.jpg");
    ci::gl::Texture2dRef tex_deprecator = loadTexture("deprecator_d.jpg");
    ci::gl::Texture2dRef tex_deprecator_n = loadTexture("deprecator_n.jpg");
    ci::gl::TextureCubeMapRef tex_sky = loadCubeTexture("skybox.jpg");
    ci::gl::Texture2dRef tex_planks = loadTexture("planks_d.tga");
    ci::gl::Texture2dRef tex_planks_n = loadTexture("planks_n.tga");
    
    // MATERIALS
    material_shadow = rtr::Material::create(shadowShader);
    
    material_occluded = rtr::Material::create(occlusionShader);
    material_scattering = rtr::Material::create(scatteringShader);
    
    material_skybox = rtr::Material::create(skyboxShader);
    material_skybox->texture("uCubeTexture", tex_sky);

    material_color_ui = rtr::Material::create(colorShader);
    material_color_ui->uniform("uColor", vec4((vec3)uiStyle.background, 1));

    material_floor = rtr::Material::create(allsparkShader);
    material_floor->uniform("uAmbient", vec3(0));
    material_floor->uniform("uLightColor", vec3(0.7f));
    material_floor->uniform("uSpecularColor", vec3(1.0f, 0.84f, 0.105f));
    material_floor->uniform("uShininess", 256);
    material_floor->texture("uTexDiffuse", tex_metal);
    material_floor->texture("uTexNormal", tex_metal_n);
    material_floor->uniform("uResolution", game.gameboardSize * 6.0f);
    material_floor->uniform("uEmissiveThreshold", emissiveThreshold);
    
    material_block = rtr::Material::create(allsparkShader);
    material_block->uniform("uAmbient", vec3(0));
    material_block->uniform("uLightColor", vec3(0.7f));
    material_block->uniform("uSpecularColor", vec3(1.0f, 0.84f, 0.105f));
    material_block->uniform("uShininess", 32);
    material_block->texture("uTexDiffuse", tex_box);
    material_block->texture("uTexNormal", tex_box_n);
    material_block->uniform("uResolution", vec2(1.0f));
    material_block->uniform("uEmissiveThreshold", emissiveThreshold);

    material_booster = rtr::Material::create(allsparkShader);
    material_booster->uniform("uAmbient", vec3(0.1f, 0.3f, 0.1f));
    material_booster->uniform("uLightColor", vec3(1));
    material_booster->uniform("uSpecularColor", vec3(1.0f, 0.84f, 0.105f));
    material_booster->uniform("uShininess", 32);
    material_booster->texture("uTexDiffuse", tex_booster);
    material_booster->texture("uTexNormal", tex_booster_n);
    material_booster->uniform("uResolution", vec2(16));
    material_booster->uniform("uEmissiveThreshold", 0.0f);

    material_deprecator = rtr::Material::create(allsparkShader);
    material_deprecator->uniform("uAmbient", vec3(0.3f, 0.1f, 0.1f));
    material_deprecator->uniform("uLightColor", vec3(1));
    material_deprecator->uniform("uSpecularColor", vec3(1.0f, 0.84f, 0.105f));
    material_deprecator->uniform("uShininess", 32);
    material_deprecator->texture("uTexDiffuse", tex_deprecator);
    material_deprecator->texture("uTexNormal", tex_deprecator_n);
    material_deprecator->uniform("uResolution", vec2(16));
    material_deprecator->uniform("uEmissiveThreshold", 0.0f);
    
    material_ground = rtr::Material::create(allsparkShader);
    material_ground->uniform("uAmbient", vec3(0));
    material_ground->uniform("uLightColor", vec3(0.9f));
    material_ground->uniform("uSpecularColor", vec3(1.0f, 0.84f, 0.105f));
    material_ground->uniform("uShininess", 256);
    material_ground->texture("uTexDiffuse", tex_ground);
    material_ground->texture("uTexNormal", tex_ground_n);
    material_ground->uniform("uResolution", vec2(32));
    material_ground->uniform("uEmissiveThreshold", emissiveThreshold);
    
    material_planks = rtr::Material::create(allsparkShader);
    material_planks->uniform("uAmbient", vec3(0));
    material_planks->uniform("uLightColor", vec3(0.9f));
    material_planks->uniform("uSpecularColor", vec3(1.0f, 0.84f, 0.105f));
    material_planks->uniform("uShininess", 256);
    material_planks->texture("uTexDiffuse", tex_planks);
    material_planks->texture("uTexNormal", tex_planks_n);
    material_planks->uniform("uResolution", vec2(1));
    material_planks->uniform("uEmissiveThreshold", emissiveThreshold);

    material_downsampling = rtr::Material::create(downsamplingShader);
    
    material_blur = rtr::Material::create(blurShader);
    
    material_bloom = rtr::Material::create(bloomShader);
    
    material_ball = rtr::Material::create(ballShader);
    material_ball->texture("uTexDiffuse", tex_ball);
    material_ball->uniform("uEmissiveThreshold", 0.1f);
    
    allSpark_materials = {material_ground, material_floor, material_block, material_booster, material_deprecator, material_planks};
    
    // SKY
    ci::geom::Cube gSky = ci::geom::Cube().size(vec3(skyBoxSize)).subdivisions(1);
    const rtr::ShapeRef sSky = rtr::Shape::create({gSky}, material_skybox);
    mSky = rtr::Model::create({sSky});


    // BALL
    ball = Ball(
        ballSize,
        ballPosition,
        ballDistanceToCamera,
        ballTargetOfCamera,
        material_ball,
        ballSubdivisions,
        material_occluded
    );
    
    // SCENE
    mat4 tPlane;
    const ci::geom::Plane gPlane = ci::geom::Plane().subdivisions(vec2(1)).size(game.gameboardSize);
    const rtr::ShapeRef sPlane = rtr::Shape::create({gPlane}, material_floor);
    sPlane->setMaterialForPass("shadow", material_shadow);
    rtr::ModelRef mPlane = rtr::Model::create({sPlane});
    tPlane = translate(tPlane, vec3(0.0f, ball.getPosition().y - ball.getSize(), game.gameboardSize.y/2 - offsetFactorGameboard));
    rtr::NodeRef nPlane = rtr::Node::create({mPlane}, tPlane);
    
    mat4 tGround;
    const ci::geom::Plane gGround = ci::geom::Plane().subdivisions(vec2(1)).size(vec2(game.gameboardSize.y));
    const rtr::ShapeRef sGround = rtr::Shape::create({gGround}, material_ground);
    sGround->setMaterialForPass("shadow", material_shadow);
    rtr::ModelRef mGround = rtr::Model::create({sGround});
    tGround = translate(tGround, vec3(0, -ball.getSize(), game.gameboardSize.y/2 - offsetFactorGameboard));
    nGround = rtr::Node::create({mGround}, tGround);
    
    mat4 tPlankLeft;
    const ci::geom::Cube gPlankLeft = ci::geom::Cube().subdivisions(1).size(vec3(planksWidth, planksHeight, game.gameboardSize.y - offsetFactorGameboard));
    const rtr::ShapeRef sPlankLeft = rtr::Shape::create({gPlankLeft}, material_planks);
    rtr::ModelRef mPlankLeft = rtr::Model::create({sPlankLeft});
    tPlankLeft = translate(tPlankLeft, vec3(game.gameboardSize.x/2.0f + planksWidth / 2.0f, ball.getPosition().y - ball.getSize(), game.gameboardSize.y/2 - offsetFactorGameboard / 2.0f));
    rtr::NodeRef nPlankLeft = rtr::Node::create({mPlankLeft}, tPlankLeft);
    
    mat4 tPlankRight;
    const ci::geom::Cube gPlankRight = ci::geom::Cube().subdivisions(1).size(vec3(planksWidth, planksHeight, game.gameboardSize.y - offsetFactorGameboard));
    const rtr::ShapeRef sPlankRight = rtr::Shape::create({gPlankRight}, material_planks);
    rtr::ModelRef mPlankRight = rtr::Model::create({sPlankRight});
    tPlankRight = translate(tPlankRight, vec3(-game.gameboardSize.x/2.0f - planksWidth / 2.0f, ball.getPosition().y - ball.getSize(), game.gameboardSize.y/2 - offsetFactorGameboard / 2.0f));
    rtr::NodeRef nPlankRight = rtr::Node::create({mPlankRight}, tPlankRight);

    scene = rtr::Node::create({}, mat4(), {nPlane, nPlankLeft, nPlankRight});
    
    
    int rows = glm::ceil(game.gameboardColumns / 2.0f);
    for (int i = 0; i <= rows; i++) {
        mat4 tPlank;
        const ci::geom::Cylinder gPlank = ci::geom::Cylinder().radius(divisionsWidth).height(game.gameboardSize.y);
        const rtr::ShapeRef sPlank = rtr::Shape::create({gPlank}, material_planks);
        rtr::ModelRef mPlank = rtr::Model::create({sPlank});
        tPlank = translate(tPlank, vec3(game.gameboardSize.x/2.0f - (game.gameboardSize.x / game.gameboardColumns) * (i + 1) - divisionsWidth / 2.0f, ball.getPosition().y - ball.getSize(), -offsetFactorGameboard));
        tPlank = rotate(tPlank, glm::radians(90.0f), vec3(1, 0, 0));
        rtr::NodeRef currentNode = rtr::Node::create({mPlank}, tPlank);
        scene->children.push_back(currentNode);
    }
    
    for (vec3 &obstacle : game.obstacles) {
        Block block = Block(
            vec3(obstacle.x, 0.05f, obstacle.z),
            obstacleSize,
            material_block,
            1,
            material_shadow,
            material_occluded
        );
        blocks.push_back(block);
    }
    
    for (vec3 &booster : game.boosters) {
        Item item = Item(
            vec3(booster.x, itemOffsetY, booster.z),
            itemSize,
            material_booster,
            24,
            10,
            material_shadow,
            material_occluded
        );
        items.push_back(item);
    }
    
    for (vec3 &detractor : game.detractors) {
        Item item = Item(
            vec3(detractor.x, itemOffsetY, detractor.z),
            itemSize,
            material_deprecator,
            24,
            -10,
            material_shadow,
            material_occluded
        );
        items.push_back(item);
    }

    
    for (Block &block : blocks) {
        scene->children.push_back(block.node);
    }
    
    for (Item &currentItem : items) {
        scene->children.push_back(currentItem.node);
    }
    
    createFBOs();

    // CAMERA
    cameraPosition = ball.getPosition() - ball.getDistanceToCamera();
    cameraTarget = ball.getCameraTarget();
    
    camera = CameraPersp(getWindowWidth(), getWindowHeight(), fieldOfView, nearestClip, farestClip);
    camera.lookAt(cameraPosition, cameraTarget, vec3(0, 1, 0));
    cameraUi = CameraUi(&camera, getWindow());
    cameraUi.setMinimumPivotDistance(0);
    
    lightCameraLeft.setPerspective(160.0f, fboShadowMapLeft->getAspectRatio(), nearestClip, farestClip);
    lightCameraFront.setPerspective(160.0f, fboShadowMapFront->getAspectRatio(), nearestClip, farestClip);
    lightCameraRight.setPerspective(160.0f, fboShadowMapRight->getAspectRatio(), nearestClip, farestClip);

    vec3 lightCameraUpVector = vec3(0, 1, 0);
    vec3 ballPos = ball.getPosition();
    lightCameraLeft.lookAt(ballPos, vec3(1, 0, 0), lightCameraUpVector);
    lightCameraFront.lookAt(ballPos, vec3(0, 0, 1), lightCameraUpVector);
    lightCameraRight.lookAt(ballPos, vec3(-1, 0, 0), lightCameraUpVector);

}

void KeepRollinApp::drawFps() {
    gl::drawString("fps: " + to_string((int)round(getAverageFps())), vec2(10.0f, getWindowSize().y - debugFont.getSize() - 10.0f), Color::white(), debugFont);
}

void KeepRollinApp::renderSceneToFbo() {
    fboNormalAndEmissive->bindFramebuffer();
    drawScene();
    fboNormalAndEmissive->unbindFramebuffer();
}

void KeepRollinApp::downSamplingScene() {
    fboBloomPass1->bindFramebuffer();
    gl::clear(bgColor);
    gl::pushModelMatrix();
    vec2 size = fboBloomPass1->getSize();
    gl::setMatricesWindow(size);
    glViewport(0, 0, size.x, size.y);
    gl::disableDepthWrite();
    gl::disableDepthRead();
    material_downsampling->uniform("uBias",  vec4(0));
    material_downsampling->uniform("uScale", vec4(1));
	material_downsampling->texture("uHighresTexture", fboNormalAndEmissive->getTexture2d(GL_COLOR_ATTACHMENT1));
    material_downsampling->bind();
    gl::drawSolidRect(fboBloomPass1->getBounds());
    gl::popModelMatrix();
    fboBloomPass1->unbindFramebuffer();
}

void KeepRollinApp::blurPassVertical() {
    fboBloomPass2->bindFramebuffer();
    gl::clear(bgColor);
    gl::pushModelMatrix();
    vec2 size = fboBloomPass2->getSize();
    gl::setMatricesWindow(size);
    glViewport(0, 0, size.x, size.y);
    gl::disableDepthWrite();
    gl::disableDepthRead();
    material_blur->texture("uBlurTexture", fboBloomPass1->getColorTexture());
    material_blur->uniform("uHorizontal", false);
    material_blur->bind();
    gl::drawSolidRect(fboBloomPass2->getBounds());
    gl::popModelMatrix();
    fboBloomPass2->unbindFramebuffer();
}

void KeepRollinApp::blurPassHorizontal() {
    fboBloomPass1->bindFramebuffer();
    gl::clear(bgColor);
    gl::pushModelMatrix();
    vec2 size = fboBloomPass1->getSize();
    gl::setMatricesWindow(size);
    glViewport(0, 0, size.x, size.y);
    gl::disableDepthWrite();
    gl::disableDepthRead();
    material_blur->texture("uBlurTexture", fboBloomPass2->getColorTexture());
    material_blur->uniform("uHorizontal", true);
    material_blur->bind();
    gl::drawSolidRect(fboBloomPass1->getBounds());
    gl::popModelMatrix();
    fboBloomPass1->unbindFramebuffer();
}

void KeepRollinApp::combineWithBloom() {
    fboCombine->bindFramebuffer();
    gl::clear(bgColor);
    gl::pushModelMatrix();
    gl::disableDepthWrite();
    gl::disableDepthRead();
    glViewport(0, 0, getWindowWidth(), getWindowHeight());
    gl::setMatricesWindow(toPixels(getWindowSize()));
    material_bloom->texture("uTex0", fboNormalAndEmissive->getTexture2d(GL_COLOR_ATTACHMENT0));
    material_bloom->texture("uTex1", fboBloomPass1->getColorTexture());
    material_bloom->bind();
    gl::drawSolidRect(getWindowBounds());
    gl::popModelMatrix();
    fboCombine->unbindFramebuffer();
}

void KeepRollinApp::combineWithScatter() {
    gl::clear(bgColor);
    gl::pushModelMatrix();
    gl::disableDepthWrite();
    gl::disableDepthRead();
    glViewport(0, 0, getWindowWidth(), getWindowHeight());
    gl::setMatricesWindow(toPixels(getWindowSize()));
    material_bloom->texture("uTex0", fboLightScattering2->getColorTexture());
    material_bloom->texture("uTex1", fboCombine->getColorTexture());
    material_bloom->bind();
    gl::drawSolidRect(getWindowBounds());
    gl::popModelMatrix();
}

void KeepRollinApp::doScattering() {
    fboLightScattering2->bindFramebuffer();
    gl::clear(bgColor);
    gl::pushModelMatrix();
    gl::setMatricesWindow(fboLightScattering2->getSize());
    gl::ScopedViewport viewport(vec2(0), fboLightScattering2->getSize());
    material_scattering->texture("uOccludedTexture", fboLightScattering1->getColorTexture());
    vec2 lightInScreenSpace = camera.worldToScreen(ball.getPosition(), getWindowWidth(), getWindowHeight()) / vec2(getWindowWidth(), getWindowHeight());
    material_scattering->uniform("uLightPositionOnScreen", vec2(lightInScreenSpace.x, 1 - lightInScreenSpace.y));
    material_scattering->bind();
    gl::drawSolidRect(fboLightScattering2->getBounds());
    gl::popModelMatrix();
    fboLightScattering2->unbindFramebuffer();
}

void KeepRollinApp::renderOccludedParts() {
    fboLightScattering1->bindFramebuffer();
    gl::clear(bgColor);

    gl::setMatrices(camera);
    gl::ScopedViewport viewport(vec2(0.0f), fboLightScattering1->getSize());
    
    glEnable(GL_CLIP_DISTANCE0);
    material_occluded->uniform("uPositionOfBall", ball.getPosition().z + ball.getSize());
    
    gl::disableDepthWrite();
    gl::disableDepthRead();
    gl::enableFaceCulling(false);
    
    gl::pushModelMatrix();
    vec3 ballPos = vec3(0, 0, ball.getPosition().z);
    gl::translate(ballPos);
    mSky->draw();
    gl::popModelMatrix();
    
    gl::enableFaceCulling(true);
    gl::enableDepthWrite();
    gl::enableDepthRead();
    
    material_occluded->uniform("uColor", vec4(0, 0, 0, 1));
    gl::pushModelMatrix();
    scene->draw("scatter");
    gl::popModelMatrix();
    
    material_occluded->uniform("uColor", vec4(1));
    gl::pushModelMatrix();
    ball.node->draw();
    glDisable(GL_CLIP_DISTANCE0);

    gl::popModelMatrix();

    fboLightScattering1->unbindFramebuffer();
}

void KeepRollinApp::blurPasses(int passes) {
    for (int i = 0; i < passes; i++) {
        blurPassVertical();
        blurPassHorizontal();
    }
}

void KeepRollinApp::drawScene() {
    gl::clear(bgColor);
    gl::setMatrices(camera);

    gl::disableDepthWrite();
    gl::disableDepthRead();
    gl::enableFaceCulling(false);
    
    gl::pushModelMatrix();
    vec3 ballPos = vec3(0, 0, ball.getPosition().z);
    gl::translate(ballPos);
    mSky->draw();
    gl::popModelMatrix();
    
    gl::enableFaceCulling(true);

    gl::pushModelMatrix();
    nGround->draw();
    gl::popModelMatrix();
    
    gl::enableDepthWrite();
    gl::enableDepthRead();

    gl::pushModelMatrix();
    scene->draw();
    ball.node->draw();
    gl::popModelMatrix();
}

void KeepRollinApp::drawShadowedScene() {

    gl::disableDepthWrite();
    gl::disableDepthRead();
    
    gl::pushModelMatrix();
    nGround->draw("shadow");
    gl::popModelMatrix();

    gl::enableDepthWrite();
    gl::enableDepthRead();

	gl::pushModelMatrix();
    scene->draw("shadow");
    gl::popModelMatrix();

}

bool intersects(vec2 circleCenter, float circleSize, Rectf rectangle) {
    float closestX = glm::clamp(circleCenter.x, rectangle.x1, rectangle.x2);
    float closestY = glm::clamp(circleCenter.y, rectangle.y1, rectangle.y2);
    
    float distanceX = circleCenter.x - closestX;
    float distanceY = circleCenter.y - closestY;

    float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    
    return distanceSquared < (circleSize * circleSize);
}

float KeepRollinApp::moveBallX(int direction, float lastTarget) {
    float rowWidth = game.gameboardSize.x / game.gameboardColumns;
    float halfWidth = (game.gameboardSize.x - rowWidth)/ 2.0f;
    float newXPosition = lastTarget + (rowWidth * direction);
    newXPosition = glm::clamp(newXPosition, -halfWidth, halfWidth);
    
    vec2 halfObstacleSize = vec2(obstacleSize.x / 2.0f, obstacleSize.z / 2.0f);
    
    for (vec3 currentObstacle : game.obstacles) {
    
        float left = currentObstacle.x - halfObstacleSize.x;
        float right = currentObstacle.x + halfObstacleSize.x;
        float top = currentObstacle.z - halfObstacleSize.y;
        float bottom = currentObstacle.z + halfObstacleSize.y;
        Rectf rectPosition = Rectf(left, top, right, bottom);
        bool intersection = intersects(vec2(newXPosition, ball.getPosition().z), ball.getSize(), rectPosition);
        
        if (intersection) {
            return lastTarget;
        }

    }
    
    return newXPosition;
}

vec2 KeepRollinApp::detectCollision() {
    vec2 circleCenter = vec2(ball.getPosition().x, ball.getPosition().z);
    float circleSize = ball.getSize() + 0.01f;
    
    vec2 halfObstacleSize = vec2(obstacleSize.x / 2.0f, obstacleSize.z / 2.0f);
    
    for (vec3 currentObstacle : game.obstacles) {
    
        float left = currentObstacle.x - halfObstacleSize.x;
        float right = currentObstacle.x + halfObstacleSize.x;
        float top = currentObstacle.z - halfObstacleSize.y;
        float bottom = currentObstacle.z + halfObstacleSize.y;
        Rectf rectPosition = Rectf(left, top, right, bottom);
        
        if (intersects(circleCenter, circleSize, rectPosition)) {
            vec2 p = circleCenter + vec2(circleSize);
            if (circleCenter.y <= rectPosition.y1) {
                return vec2(0, p.y - rectPosition.y1);
            }
            if (circleCenter.y >= rectPosition.y2) {
                return vec2(0, glm::abs(rectPosition.y2 - p.y) - circleSize * 2.0f);
            }
        }
    }
    return vec2(0);
}

void KeepRollinApp::detectItemCollision() {
    vec2 circleCenter = vec2(ball.getPosition().x, ball.getPosition().z);
    float circleSize = ball.getSize() + 0.01f;
    
    for (Item item : items) {
        
        vec3 p = item.position;
        
        float left = p.x - itemSize;
        float right = p.x + itemSize;
        float top = p.z - 0.05f;
        float bottom = p.z + 0.05f;
        Rectf rectPosition = Rectf(left, top, right, bottom);
        
        if (intersects(circleCenter, circleSize, rectPosition)) {
            if (lastHitItem.position != item.position) {
                maxSpeed += item.boost;
                maxSpeed = glm::clamp(maxSpeed, 10.0f, 60.0f);
            }
            lastHitItem = item;
            break;
        }
    }
}

bool KeepRollinApp::wasFinishReached() {
    return ball.getPosition().z >= game.getGameboardLength();
}

void KeepRollinApp::update() {
    
    if (debugMode) {
        rtr::watcher.checkForChanges();
    }

    if (game.isRunning()) {
        game.updateTime(getElapsedSeconds());
    }
    
    detectItemCollision();
    
    velocity.x = (acceleration.x == 0) ? velocity.x * velocityDamp.x : velocity.x + acceleration.x;
    velocity.y = (acceleration.y == 0) ? velocity.y * velocityDamp.y : velocity.y + acceleration.y;
    velocity.z = (acceleration.z == 0) ? velocity.z * velocityDamp.z : velocity.z + acceleration.z;
    velocity = glm::clamp(velocity, vec3(-50), vec3(50));
    
    velocity.x = (velocity.x > -0.1f && velocity.x < 0.1f) ? 0 : velocity.x;
    velocity.y = (velocity.y > -0.1f && velocity.y < 0.1f) ? 0 : velocity.y;
    velocity.z = (velocity.z > -0.1f && velocity.z < 0.1f) ? 0 : velocity.z;
    
    speed.x += velocity.x;
    speed.x = (acceleration.x == 0) ? speed.x * speedDamp.x : speed.x;
    
    speed.y += velocity.y;
    speed.y = (acceleration.y == 0) ? speed.y * speedDamp.y : speed.y;
    
    speed.z += velocity.z;
    speed.z = (acceleration.z == 0) ? speed.z * speedDamp.z : speed.z;
    
    speed = glm::clamp(speed, minSpeed, maxSpeed) * speedFactor;
    
    vec3 oldBallPosition = ball.getPosition();
    
    float zPos = (oldBallPosition + speed).z;
    
    float deltaX = oldBallPosition.x - ((oldBallPosition.x - targetX) * 0.15f);
    
    vec3 newPosition = vec3(deltaX, 0, zPos);
    ball.setPosition(newPosition);
    
    vec2 collides = detectCollision();
    
    if (collides != vec2(0)) {
        velocity = (velocity * -1.0f) / 2.0f;
        newPosition = ball.getPosition() - vec3(collides.x, 0, collides.y);
        ball.setPosition(newPosition);
    }
    
    ballRotation = quat(vec3(newPosition.z, 0, 0) * maxSpeed);
    quat skyRotation = quat(vec3(-newPosition.z, 0, 0) / skyBoxSize);

    material_ball->uniform("uRotation", mat4(ballRotation));
    material_skybox->uniform("uRotation", mat4(skyRotation));
    
    vec3 p = ball.getPosition();
    lightCameraLeft.setEyePoint(p);
    lightCameraFront.setEyePoint(p);
    lightCameraRight.setEyePoint(p);
    
    camera.setEyePoint(ball.getPosition() - ball.getDistanceToCamera());
    
    vec3 lightPos = camera.getEyePoint();
    vec3 ballPos = ball.getPosition();
    
    for (rtr::MaterialRef currentMaterial : allSpark_materials) {
        currentMaterial->uniform("uWcLightPosition", ballPos);
        currentMaterial->uniform("uWcCameraPosition", lightPos);
    }
    
    if (wasFinishReached()) {
        acceleration = vec3(0);
        game.end();
    }
}

void KeepRollinApp::renderDepthFbo(ci::gl::FboRef shadowMap, CameraPersp camera) {
	gl::enable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 2.0f);
    shadowMap->bindFramebuffer();
    gl::clear(Color::black());
    gl::ScopedViewport viewport(vec2(0.0f), shadowMap->getSize());
	gl::enableDepthWrite();
    gl::enableDepthRead();
	gl::color(Color::white());
	gl::setMatrices(camera);
	drawShadowedScene();
    gl::disable(GL_POLYGON_OFFSET_FILL);
    shadowMap->unbindFramebuffer();
}

void KeepRollinApp::draw() {
    
    gl::clear(bgColor);

    renderDepthFbo(fboShadowMapLeft, lightCameraLeft);
    renderDepthFbo(fboShadowMapFront, lightCameraFront);
    renderDepthFbo(fboShadowMapRight, lightCameraRight);

    vec3 shadowLightPosition = vec3(gl::getModelView() * vec4(ball.getPosition(), 1.0f)) ;
    
    mat4 shadowMatrixLeft = lightCameraLeft.getProjectionMatrix() * lightCameraLeft.getViewMatrix();
    mat4 shadowMatrixFront = lightCameraFront.getProjectionMatrix() * lightCameraFront.getViewMatrix();
    mat4 shadowMatrixRight = lightCameraRight.getProjectionMatrix() * lightCameraRight.getViewMatrix();

    for (rtr::MaterialRef currentMaterial : allSpark_materials) {
        currentMaterial->uniform("uShadowLightPosition", shadowLightPosition);
        currentMaterial->texture("uShadowTextureLeft", fboShadowMapLeft->getDepthTexture());
        currentMaterial->uniform("uShadowMatrixLeft", shadowMatrixLeft);
        currentMaterial->texture("uShadowTextureFront", fboShadowMapFront->getDepthTexture());
        currentMaterial->uniform("uShadowMatrixFront", shadowMatrixFront);
        currentMaterial->texture("uShadowTextureRight", fboShadowMapRight->getDepthTexture());
        currentMaterial->uniform("uShadowMatrixRight", shadowMatrixRight);
    }

    renderSceneToFbo();
    
    renderOccludedParts();
    doScattering();
    
    downSamplingScene();
    blurPasses(numberOfBlurPasses);
    combineWithBloom();
    combineWithScatter();

    // OVERLAY
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::setMatricesWindow(getWindowSize());
    
    if (debugMode) {
        drawFps();
    }
    drawUI();
    if (game.isAtStart()) {
        drawIntroscreen();
    } else if (game.hasEnded()) {
        drawEndscreen();
    }
    
}

void KeepRollinApp::drawUI() {
    material_color_ui->bind();
    gl::drawSolidRect(uiStyle.bounds);
    vec2 pos = vec2(uiStyle.bounds.getWidth() - uiStyle.bounds.getWidth()/2, uiStyle.bounds.getHeight() - uiStyle.font.getSize());
    gl::drawStringCentered(game.getTime(), pos, uiStyle.color, uiStyle.font);
}

void KeepRollinApp::drawIntroscreen() {
    vec2 pos = vec2(getWindowSize().x / 2, getWindowSize().y / 2 - uiStyle.font.getSize() / 2);
    gl::drawStringCentered("(S)tart the game!", pos, uiStyle.color, uiStyle.font);
}

void KeepRollinApp::drawEndscreen() {
    vec2 pos = vec2(getWindowSize().x / 2, getWindowSize().y / 2 - uiStyle.font.getSize() / 2);
    gl::drawStringCentered("(R)eset the game!", pos, uiStyle.color, uiStyle.font);
}

void prepareSettings(KeepRollinApp::Settings* settings) {
    settings->setHighDensityDisplayEnabled(KeepRollinApp::isHighRes);
    settings->setQuitOnLastWindowCloseEnabled();
    settings->setResizable();
    settings->setTitle("Game: Keep Rollin'");
    settings->setWindowSize(600, 600);
}

CINDER_APP(KeepRollinApp, RendererGl, prepareSettings)
