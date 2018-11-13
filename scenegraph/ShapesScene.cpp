#include "ShapesScene.h"

#include "Camera.h"
#include "shapes/GLCube.h"
#include "shapes/GLSphere.h"
#include "shapes/GLCone.h"
#include "shapes/GLCylinder.h"
#include "shapes/GLTorus.h"
#include <QFileDialog>
#include "gl/GLDebug.h"

#include <sstream>

#define ERR_CHECK() checkError();

using namespace CS123::GL;
#include "gl/shaders/CS123Shader.h"
#include "gl/shaders/Shader.h"

#include "ResourceLoader.h"

#include <SupportCanvas3D.h>

ShapesScene::ShapesScene(int width, int height) :
    m_width(width),
    m_height(height)
{
    initializeSceneMaterial();
    initializeSceneLight();
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();

    //TODO: [SHAPES] Allocate any additional memory you need...
    setShape(SHAPE_SPHERE);
}

ShapesScene::~ShapesScene()
{
    // Pro-tip: If you use smart pointers properly, this destructor should be empty
}

void ShapesScene::initializeSceneMaterial() {
    // Use a shiny orange material
    m_material.clear();
    m_material.cAmbient.r = 0.2f;
    m_material.cAmbient.g = 0.1f;
    m_material.cDiffuse.r = 1.0f;
    m_material.cDiffuse.g = 0.5f;
    m_material.cSpecular.r = m_material.cSpecular.g = m_material.cSpecular.b = 1;
    m_material.shininess = 64;
}

void ShapesScene::initializeSceneLight() {
    // Use a white directional light from the upper left corner
    memset(&m_light, 0, sizeof(m_light));
    m_light.type = LightType::LIGHT_DIRECTIONAL;
    m_light.dir = m_lightDirection;
    m_light.color.r = m_light.color.g = m_light.color.b = 1;
    m_light.id = 0;
}

void ShapesScene::loadPhongShader() {

    //std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/test.vert");
    //std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/test.frag");

    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void ShapesScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void ShapesScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void ShapesScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void ShapesScene::render(SupportCanvas3D *context) {
    // Clear the screen in preparation for the next frame. (Use a gray background instead of a
    // black one for drawing wireframe or normals so they will show up against the background.)
    setClearColor();

    renderPhongPass(context);

    if (settings.drawWireframe) {
        renderWireframePass(context);
    }

    if (settings.drawNormals) {
        renderNormalsPass(context);
    }
}

void ShapesScene::renderPhongPass(SupportCanvas3D *context) {
    m_phongShader->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clearLights();
    setLights(context->getCamera()->getViewMatrix());
    setPhongSceneUniforms();
    setMatrixUniforms(m_phongShader.get(), context);
    renderGeometryAsFilledPolygons();

    m_phongShader->unbind();
}

void ShapesScene::setPhongSceneUniforms() {
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->applyMaterial(m_material);
}

void ShapesScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
    shader->setUniform("m", glm::mat4(1.0f));
}

void ShapesScene::renderGeometryAsFilledPolygons() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
}

void ShapesScene::renderWireframePass(SupportCanvas3D *context) {
    m_wireframeShader->bind();
    setMatrixUniforms(m_wireframeShader.get(), context);
    renderGeometryAsWireframe();
    m_wireframeShader->unbind();
}

void ShapesScene::renderGeometryAsWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderGeometry();
}

void ShapesScene::renderNormalsPass (SupportCanvas3D *context) {
    // Render the lines.
    m_normalsShader->bind();
    setMatrixUniforms(m_normalsShader.get(), context);
    renderGeometryAsWireframe();
    m_normalsShader->unbind();

    // Render the arrows.
    m_normalsArrowShader->bind();
    setMatrixUniforms(m_normalsArrowShader.get(), context);
    renderGeometryAsFilledPolygons();
    m_normalsArrowShader->unbind();
}

void ShapesScene::renderGeometry() {
    // TODO: [SHAPES] Render the shape. Lab 1 seems like it'll come in handy...

    //glUseProgram(m_solidProgramID);

    // TODO: Set the uniform's value to a color other than white. (Task 9)
    //glUniform3f(glGetUniformLocation(m_phongShader.get()->getID(), "color"), .3, .2, .9);

    m_shape->draw();

    //glUseProgram(0);
}

void ShapesScene::clearLights() {
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void ShapesScene::setLights(const glm::mat4 viewMatrix) {
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    m_light.dir = glm::inverse(viewMatrix) * m_lightDirection;

    clearLights();
    m_phongShader->setLight(m_light);
}

void ShapesScene::settingsChanged() {
    // TODO: [SHAPES] Fill this in if applicable.

    setShape(settings.shapeType);
}

void ShapesScene::setShape(int shapeType) {
    switch(shapeType) {
        case SHAPE_CUBE:
            m_shape = std::make_unique<GLCube>(settings.shapeParameter1,
                                               settings.shapeParameter2,
                                               settings.shapeParameter3);
            break;
        case SHAPE_SPHERE:
            m_shape = std::make_unique<GLSphere>(settings.shapeParameter1,
                                             settings.shapeParameter2,
                                             settings.shapeParameter3);
            break;
        case SHAPE_CONE:
            m_shape = std::make_unique<GLCone>(settings.shapeParameter1,
                                         settings.shapeParameter2,
                                         settings.shapeParameter3);
            break;
        case SHAPE_CYLINDER:
            m_shape = std::make_unique<GLCylinder>(settings.shapeParameter1,
                                         settings.shapeParameter2,
                                         settings.shapeParameter3);
            break;
        case SHAPE_TORUS:
            m_shape = std::make_unique<GLTorus>(settings.shapeParameter1,
                                         settings.shapeParameter2,
                                         settings.shapeParameter3);
            break;
        default:
            m_shape = std::make_unique<GLCube>(settings.shapeParameter1,
                                               settings.shapeParameter2,
                                               settings.shapeParameter3);
            break;
    }
}
