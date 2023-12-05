// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/entity.h"
#include "math/math.h"
#include "mesh/meshStatic.h"
#include "math/transformation.h"
#include "physics/shapes/shape.h"
#include "physics/shapes/shapeBox.h"
#include "physics/shapes/shapeSphere.h"
#include "physics/shapes/shapeGeometry.h"
#include "physics/shapes/shapeCapsule.h"
#include "common/destroyable.h"
#include <list>

class Camera;

enum class ComponentColorMode
{
    Lit = 0,
    Alpha = 1,
    Addition = 2,
};

class Component : public Destroyable
{
public:
    EXPORT Component();
    EXPORT virtual ~Component();
    EXPORT void prepare(Entity *owner);
    EXPORT virtual void process(float delta);

    EXPORT virtual void renderLightPhase(Matrix4 &vpMatrix, unsigned int shadowMapTexture, Camera *activeCamera);
    EXPORT virtual Matrix4 preparePreShadowPhase(Vector3 cameraPosition);

    EXPORT virtual void onRender(Matrix4 &vpMatrix, Transformation *tf);
    EXPORT virtual void onRenderShadow(Matrix4 &vpMatrix, Transformation *tf);
    EXPORT virtual void onCreated();
    EXPORT void prepareColorMode();

    EXPORT ShapeSphere *addShapeSphere(float radius);
    EXPORT ShapeSphere *addShapeSphere(Vector3 center, float radius);
    EXPORT ShapeBox *addShape2dBox(Vector2 size);
    EXPORT ShapeBox *addShape2dBox(Vector2 center, Vector2 size);
    EXPORT ShapeBox *addShape3dBox(Vector3 size);
    EXPORT ShapeBox *addShape3dBox(Vector3 center, Vector3 size);

    EXPORT ShapeConvex *addShapeConvex(Vector3 *verticies, int amount, std::vector<HullPolygonSimple> *polygons, float density = 0.1f);
    EXPORT ShapeConvex *addShapeConvex(Vector3 center, Vector3 *verticies, int amount, std::vector<HullPolygonSimple> *polygons, float density = 0.1f);

    EXPORT ShapeGeometry *addShapeGeometry(Geometry *geometry);
    EXPORT ShapeGeometry *addShapeGeometry(Vector3 center, Geometry *geometry);

    EXPORT ShapeCapsule *addShapeCapsule(float height, float radius);

    EXPORT virtual Matrix4 getLocalspaceMatrix();

    EXPORT virtual MeshStatic *getStaticMesh();

    EXPORT inline bool isUsingBlendingPhase() { return colorMode != ComponentColorMode::Lit; }
    EXPORT inline bool isUsingLightPhase() { return bUseLightPhase; }
    EXPORT inline bool isUsingShadowPhase() { return bUseShadowPhase; }

    EXPORT inline void setVisibility(bool state) { bIsVisible = state; }
    EXPORT inline bool isVisible() { return bIsVisible; }

    std::list<Shape *> shapes;
    Transformation transform;

    ComponentColorMode colorMode = ComponentColorMode::Lit;
protected:
    bool bUseLightPhase = false;
    bool bUseShadowPhase = false;
    bool bIsVisible = true;
    Entity *owner = nullptr;
};
