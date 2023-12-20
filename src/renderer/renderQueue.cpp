// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/renderQueue.h"
#include "actor/actor.h"

RenderQueue::RenderQueue()
{
    memset(renderElements, 0, sizeof(RenderElement) * MAX_RENDER_ELEMENTS);
    memset(mainPhaseElements, 0, sizeof(RenderElement *) * MAX_RENDER_ELEMENTS);
    memset(blendPhaseElements, 0, sizeof(RenderElement *) * MAX_RENDER_ELEMENTS);
    memset(shadowCasterElements, 0, sizeof(RenderElement *) * MAX_RENDER_ELEMENTS);
    memset(lightElements, 0, sizeof(RenderElementLight) * MAX_LIGHTS);
    memset(debugElements, 0, sizeof(RenderElementDebug) * MAX_DEBUG_ELEMENTS);
}

void RenderQueue::reset()
{
    lastElement = 0;
    lastElementMainPhase = 0;
    lastElementBlendPhase = 0;
    lastShadowCasterElement = 0;
    lastElementLight = 0;
    lastDebugElement = 0;
}

void RenderQueue::addMainPhase(Matrix4 &mModel, Shader *shader, Texture *texture, MeshStatic *mesh, ShaderParameter **parameters, int parametersAmount)
{
    if (lastElementMainPhase < MAX_RENDER_ELEMENTS && lastElement < MAX_RENDER_ELEMENTS && shader && mesh)
    {
        RenderElement *element = &renderElements[lastElement];
        mainPhaseElements[lastElementMainPhase] = element;

        element->mModel = mModel;
        element->mModelViewProjection = mViewProjection * mModel;
        element->colorMode = ComponentColorMode::Lit;
        element->shader = shader;
        element->texture = texture;
        element->mesh = mesh;
        element->opacity = 1.0f;
        element->parameters = parameters;
        element->parametersAmount = parametersAmount;

        lastElementMainPhase++;
        lastElement++;
    }
}

void RenderQueue::addBlendingPhase(Matrix4 &mModel, ComponentColorMode colorMode, Shader *shader, Texture *texture, MeshStatic *mesh, float opacity, ShaderParameter **parameters, int parametersAmount)
{
    if (lastElementMainPhase < MAX_RENDER_ELEMENTS && lastElement < MAX_RENDER_ELEMENTS && shader && mesh)
    {
        RenderElement *element = &renderElements[lastElement];
        blendPhaseElements[lastElementBlendPhase] = element;

        element->mModel = mModel;
        element->mModelViewProjection = mViewProjection * mModel;
        element->colorMode = colorMode;
        element->shader = shader;
        element->texture = texture;
        element->mesh = mesh;
        element->opacity = opacity;
        element->parameters = parameters;
        element->parametersAmount = parametersAmount;

        lastElementBlendPhase++;
        lastElement++;
    }
}

void RenderQueue::addShadowCaster(Matrix4 &mModel, MeshStatic *mesh)
{
    if (lastShadowCasterElement < MAX_RENDER_ELEMENTS && lastElement < MAX_RENDER_ELEMENTS && mesh)
    {
        RenderElement *element = &renderElements[lastElement];
        shadowCasterElements[lastShadowCasterElement] = element;

        element->mModel = mModel;
        element->mesh = mesh;

        lastShadowCasterElement++;
        lastElement++;
    }
}

void RenderQueue::addLight(LightType type, Vector3 position, Vector3 color, float affectDistance, bool bCastShadows)
{
    if (lastElementLight < MAX_LIGHTS)
    {
        RenderElementLight *element = &lightElements[lastElementLight];

        element->type = type;
        element->position = position;
        element->color = color;
        element->affectDistance = affectDistance;
        element->bCastShadows = bCastShadows;

        lastElementLight++;
    }
}

void RenderQueue::addDebugBody(PhysicsBody *body, float symScale, float lineThickness)
{
    if (body && lastDebugElement < MAX_DEBUG_ELEMENTS)
    {
        RenderElementDebug *element = &debugElements[lastDebugElement];

        element->body = body;
        element->actor = nullptr;
        element->symScale = symScale;
        element->lineThickness = lineThickness;

        lastDebugElement++;
    }
}

void RenderQueue::addDebugActor(Actor *actor, float lineThickness)
{
    if (actor && lastDebugElement < MAX_DEBUG_ELEMENTS)
    {
        RenderElementDebug *element = &debugElements[lastDebugElement];

        element->body = nullptr;
        element->actor = actor;
        element->symScale = 1.0f;
        element->lineThickness = lineThickness;

        lastDebugElement++;
    }
}