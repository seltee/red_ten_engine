// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentMesh.h"
#include "math/glm/gtc/type_ptr.hpp"
#include <math.h>

ComponentMesh::ComponentMesh() : Component()
{
    shader = nullptr;
}

void ComponentMesh::onRenderQueue(RenderQueue *renderQueue)
{
    if (mesh)
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix();

        if (!lods.empty())
        {
            float distance = fabsf((*renderQueue->getViewProjectionMatrix() * mModel * Vector4(0, 0, 0, 1.0f)).z);
            Mesh *toRender = mesh;
            for (auto &lod : lods)
            {
                if (lod.distance < distance)
                    toRender = lod.mesh;
                else
                    break;
            }
            if (bRenderShape)
            {
                if (colorMode == ColorMode::Lit)
                    renderQueue->addMainPhase(mModel, shader, nullptr, toRender->getAsStatic(), parametersList, parametersAmount);
                else
                    renderQueue->addBlendingPhase(mModel, colorMode, shader, nullptr, toRender->getAsStatic(), 1.0f, parametersList, parametersAmount);
            }

            if (bCastShadows)
                renderQueue->addShadowCaster(mModel, toRender->getAsStatic(), shader ? shader->getShadowTexture() : nullptr, uvShadowShiftSize);
        }
        else
        {
            if (bRenderShape)
            {
                if (colorMode == ColorMode::Lit)
                    renderQueue->addMainPhase(mModel, shader, nullptr, mesh->getAsStatic(), parametersList, parametersAmount);
                else
                    renderQueue->addBlendingPhase(mModel, colorMode, shader, nullptr, mesh->getAsStatic(), 1.0f, parametersList, parametersAmount);
            }

            if (bCastShadows)
                renderQueue->addShadowCaster(mModel, mesh->getAsStatic(), shader ? shader->getShadowTexture() : nullptr, uvShadowShiftSize);
        }
    }
}

void ComponentMesh::setMesh(Mesh *mesh)
{
    if (this->mesh != mesh)
    {
        this->mesh = mesh;
        if (owner)
            owner->childUpdated();
    }
}

void ComponentMesh::setShader(Shader *shader)
{
    this->shader = shader;
    if (shader && !shader->isReady())
        shader->build();
}

Matrix4 ComponentMesh::getLocalspaceMatrix()
{
    return *transform.getModelMatrix();
}

MeshStatic *ComponentMesh::getStaticMesh()
{
    return mesh->getAsStatic();
}

void ComponentMesh::lookAt(Vector3 &point, bool bUseGlobalTranformation)
{
    transform.setRotation(Vector3(0.0f, 0.0f, 0.0f));
    Matrix4 m = glm::inverse(bUseGlobalTranformation ? getWorldModelMatrix() : *transform.getModelMatrix());
    Vector4 localPoint = m * Vector4(point, 1.0f);
    Vector3 dif = glm::normalize(glm::vec3(localPoint));
    transform.setRotation(glm::quatLookAtRH(dif, Vector3(0.0f, 1.0f, 0.0f)) * Quat(Vector3(CONST_PI / 2.0f, CONST_PI, 0.0f)));
}

void ComponentMesh::addLod(Mesh *mesh, float distance)
{
    lods.push_back({mesh, distance});
}

void ComponentMesh::renderDebugVolume(Renderer *renderer, Matrix4 *mProjectionView, float thickness, Vector3 color)
{
    if (mesh && mesh->isRendarable())
    {
        Matrix4 worldTransform = getWorldModelMatrix();
        auto volume = mesh->getAsStatic()->getBoundVolumeSphere();
        float radius = volume->recalcRadius(&worldTransform);
        Vector3 absoluteCenter = worldTransform * Vector4(volume->center, 1.0f);

        for (int i = 0; i < 16; i++)
        {
            float lA = (float)i / 16.0f * CONST_PI * 2.0f;
            float lB = (float)(i + 1) / 16.0f * CONST_PI * 2.0f;

            renderer->renderDebugLine(
                (absoluteCenter + Vector3(radius * sinf(lA), radius * cosf(lA), 0.0f)),
                (absoluteCenter + Vector3(radius * sinf(lB), radius * cosf(lB), 0.0f)),
                mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));

            renderer->renderDebugLine(
                (absoluteCenter + Vector3(0.0f, radius * sinf(lA), radius * cosf(lA))),
                (absoluteCenter + Vector3(0.0f, radius * sinf(lB), radius * cosf(lB))),
                mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));

            renderer->renderDebugLine(
                (absoluteCenter + Vector3(radius * cosf(lA), 0.0f, radius * sinf(lA))),
                (absoluteCenter + Vector3(radius * cosf(lB), 0.0f, radius * sinf(lB))),
                mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));
        }
    }
}