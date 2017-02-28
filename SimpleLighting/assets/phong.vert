/*
 * Simple Phong Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 * This vertex shader expects a position and normal vector per vertex,
 * and model-view, projection and normal matrices.
 *
 * It transforms the position and normal to eye coordinates and
 * passes them to the fragment shader; it also transforms the vertex 
 * position to clip coordinates for the needs of the pipeline.
 *
 */

#version 150

// transformation matrices as provided by Cinder
uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

// position and normal vectors
in vec4 ciPosition;
in vec3 ciNormal;

// normal vector mapped into eye coordinates (ec)
out vec4 vertexPositionEC;
out vec3 normalDirEC;

void main(void) {
    
    // position to clip coordinates
    gl_Position = ciModelViewProjection * ciPosition;
    
    // position to eye coordinates
    vertexPositionEC = ciModelView * ciPosition;
    
    // normal to eye coordinates
    normalDirEC = ciNormalMatrix * ciNormal;
        
}
