#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/CameraUi.h"
#include "RTR/RTR.h"
#include "Game.hpp"
#include "Ball.hpp"
#include "Block.hpp"
#include "Item.hpp"

class KeepRollinApp : public ci::app::App {
    
    public:
        void setup() override;
        void update() override;
        void draw() override;
        void resize() override;
        void keyDown(ci::app::KeyEvent event) override;
        void keyUp(ci::app::KeyEvent event) override;

        ci::gl::Texture2dRef loadTexture(const ci::fs::path &url);
        ci::gl::TextureCubeMapRef loadCubeTexture(const ci::fs::path &url);
    
        void createFBOs();
    
        void drawScene();
        void drawShadowedScene();
    
        void blurPasses(int passes);
        void blurPassVertical();
        void blurPassHorizontal();
    
        void renderSceneToFbo();
        void downSamplingScene();
        void renderDepthFbo(ci::gl::FboRef shadowMap, ci::CameraPersp camera);
        void combineWithBloom();
        void renderOccludedParts();
        void doScattering();
        void combineWithScatter();
    
        ci::vec2 detectCollision();
        void detectItemCollision();
        float moveBallX(int direction, float lastTarget);
        bool wasFinishReached();
    
        void drawFps();
        void drawUI();
    
        void drawIntroscreen();
        void drawEndscreen();

        static const bool isHighRes = false;
    
    private:
        const bool debugMode = false;
        ci::Font debugFont = ci::Font("Arial", 16.0f);
    
        struct ui {
            ci::Font font;
            ci::Color color;
            ci::Color background;
            ci::Rectf bounds;
        };
    
        struct ui uiStyle = {
            ci::Font("Arial", 32.0f),
            ci::Color::white(),
            ci::Color::black(),
            ci::Rectf(0, 0, getWindowWidth(), 42.0f)
        };

        const ci::Color bgColor = ci::Color(0, 0, 0);
    
        // Scene
        rtr::NodeRef scene;
        cinder::gl::Texture2dRef tex_world;
    
        // Shader
        ci::gl::GlslProgRef colorShader;
        ci::gl::GlslProgRef ballShader;
        ci::gl::GlslProgRef allsparkShader;
        ci::gl::GlslProgRef Shader;
        ci::gl::GlslProgRef downsamplingShader;
        ci::gl::GlslProgRef blurShader;
        ci::gl::GlslProgRef bloomShader;
        ci::gl::GlslProgRef skyboxShader;
        ci::gl::GlslProgRef scatteringShader;
        ci::gl::GlslProgRef occlusionShader;
        
        // Materials
        rtr::MaterialRef material_skybox;
        rtr::MaterialRef material_color_ui;
        rtr::MaterialRef material_color_red;
        rtr::MaterialRef material_color_green;
        rtr::MaterialRef material_floor;
        rtr::MaterialRef material_ground;
        rtr::MaterialRef material_color_white;
        rtr::MaterialRef material_downsampling;
        rtr::MaterialRef material_blur;
        rtr::MaterialRef material_bloom;
        rtr::MaterialRef material_ball;
        rtr::MaterialRef material_block;
        rtr::MaterialRef material_booster;
        rtr::MaterialRef material_deprecator;
        rtr::MaterialRef material_planks;
        rtr::MaterialRef material_occluded;
        rtr::MaterialRef material_scattering;
        
        std::vector<rtr::MaterialRef> allSpark_materials;

        // Game-Objects
        Ball ball;
        float ballSize = 0.05f;
        ci::vec3 ballPosition = ci::vec3(0);
        ci::vec3 ballDistanceToCamera = ci::vec3(0, -0.3f, 0.6f);
        ci::vec3 ballTargetOfCamera = ci::vec3(0, 0, 1);
        int ballSubdivisions = 32;
        ci::quat ballRotation;
    
        float divisionsWidth = 0.005f;
        float divisionsHeight = 0.01f;
        float planksWidth = 0.005f;
        float planksHeight = 0.01f;
    
        ci::vec2 planeSize = ci::vec2(1.0f, 10.0f);
        ci::vec3 obstacleSize = ci::vec3(0.2f);
        float itemSize = 0.09f;
        float itemOffsetY = -0.025f;

        rtr::ModelRef mSky;
        float skyRotationSpeed = 25.0f;
        float skyBoxSize = 10.0f;
    
        rtr::NodeRef nGround;
    
        std::vector<Block> blocks;
        std::vector<Item> items;

        // Camera
        ci::CameraPersp camera;
        ci::CameraUi cameraUi;
        ci::vec3 cameraPosition;
        ci::vec3 cameraTarget;
        const float fieldOfView = 60.0f;
        const float nearestClip = 0.01f;
        const float farestClip = 10.0f;
    
        // Game
        Game game;
        float offsetFactorGameboard = 3.0f;
        Item lastHitItem;
    
        ci::gl::FboRef fboCombine;
    
        // Bloom
        ci::gl::FboRef fboNormalAndEmissive;
        ci::gl::FboRef fboBloomPass1;
        ci::gl::FboRef fboBloomPass2;
    
        // Shadow Map
        float shadowMapScale = 2.5f;
        ci::gl::GlslProgRef shadowShader;
        rtr::MaterialRef material_shadow;
        ci::gl::FboRef fboShadowMapLeft;
        ci::gl::Texture2dRef shadowMapLeft;
        ci::gl::FboRef fboShadowMapFront;
        ci::gl::Texture2dRef shadowMapFront;
        ci::gl::FboRef fboShadowMapRight;
        ci::gl::Texture2dRef shadowMapRight;
        ci::CameraPersp lightCameraLeft;
        ci::CameraPersp lightCameraFront;
        ci::CameraPersp lightCameraRight;
    
        int windowWidth = 600;
        int windowHeight = 600;
    
        int fboWidth = windowWidth;
        int fboHeight = windowHeight;
        float downSampling = 0.25;
        int fboWidthSmall = fboWidth * downSampling;
        int fboHeightSmall = fboHeight * downSampling;
    
        const int numberOfBlurPasses = 4;
    
        const float emissiveThreshold = 0.8f;
        const float emissiveThresholdBall = 0.5f;
    
        // Light Scattering
        float lightScatteringScale = 0.5f;
        ci::gl::FboRef fboLightScattering1;
        ci::gl::FboRef fboLightScattering2;
    
        // Physics
        ci::vec3 accelerationForce = ci::vec3(1.0f);
        ci::vec3 acceleration = ci::vec3(0);
    
        ci::vec3 velocity = ci::vec3(0);
        ci::vec3 velocityDamp = ci::vec3(0.9);

        ci::vec3 speed = ci::vec3(0);
        ci::vec3 speedDamp = ci::vec3(0.9);
        ci::vec3 speedFactor = ci::vec3(0.001f);
        ci::vec3 minSpeed = ci::vec3(-30);
        ci::vec3 initialMaxSpeed = ci::vec3(30);
        ci::vec3 maxSpeed = ci::vec3(30);
    
        float targetX = 0.0f;
};
