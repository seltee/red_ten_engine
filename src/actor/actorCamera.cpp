#include "actorCamera.h"
#include "stage/layerActors.h"

CameraOrto *ActorCamera::setupOrtoCamera()
{
    if (componentCamera)
        removeComponent(componentCamera);
    componentCamera = createComponent<ComponentCameraOrto>();
    camera = ((ComponentCameraOrto *)componentCamera)->getCamera();
    return (CameraOrto *)camera;
}

CameraPerspective *ActorCamera::setupPerspectiveCamera()
{
    if (componentCamera)
        removeComponent(componentCamera);
    componentCamera = createComponent<ComponentCameraPerspective>();
    camera = ((ComponentCameraPerspective *)componentCamera)->getCamera();
    return (CameraPerspective *)camera;
}

bool ActorCamera::setActive()
{
    if (camera && layer)
    {
        layer->setActiveCamera(camera);
    }
    return false;
}