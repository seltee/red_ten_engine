CC = clang++
LD = clang++

ifeq ($(OS),Windows_NT)
SDL_LIB_PATH = "../SDL2-2.28.5"
SDL_TTF_LIB_PATH = "../SDL2_ttf-2.0.15"
else
SDL_LIB_PATH = "/opt/homebrew/Cellar/sdl2/SDL2-2.28.5/"
SDL_TTF_LIB_PATH = "/opt/homebrew/Cellar/sdl2_ttf/2.20.2/"
endif

ifeq ($(OS),Windows_NT)
CFLAGS = -Isrc -I${SDL_LIB_PATH}/include -I${SDL_TTF_LIB_PATH}/include \
		-I${SDL_TTF_LIB_PATH}/include -I${SDL_TTF_LIB_PATH}/include/SDL2 \
		-Wall -c -std=c++17 -mfpmath=sse -fdeclspec -g -O3
else
CFLAGS = -Isrc -I${SDL_LIB_PATH}/include -I${SDL_LIB_PATH}/include/SDL2 \
		-I${SDL_TTF_LIB_PATH}/include -I${SDL_TTF_LIB_PATH}/include/SDL2 \
		-Wall -c -std=c++17 -fdeclspec -g -O3
endif

ifeq ($(OS),Windows_NT)
LIBRARIES = -L${SDL_LIB_PATH}/lib/x64/ -L${SDL_TTF_LIB_PATH}/lib/x64/ \
			-lSDL2 -lSDL2main -lkernel32 -luser32 -lgdi32 -lwinspool -lSDL2_ttf.lib \
			-lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 -lopengl32
else
LIBRARIES = -L${SDL_LIB_PATH}/lib -L${SDL_TTF_LIB_PATH}/lib \
			-lSDL2 -lSDL2main -lSDL2_ttf -framework OpenGL -framework
endif

ifeq ($(OS),Windows_NT)
EXT = ".exe"
else
EXT = ""
endif

LFLAGS = -shared -Wall -g 

# The build target 
ifeq ($(OS),Windows_NT)
TARGET = librtengine.dll
COPY = xcopy /Y
MOVE = move
else
TARGET = librtengine.so
COPY = cp -f
MOVE = mv -f
endif

ifeq ($(OS),Windows_NT)
EFLAGS = -L./ -llibrtengine
else
EFLAGS = -L./ -lrtengine
endif

SRCDIR = src
EXMDIR = examples
OBJDIR = objects
BINDIR = bin
 
OBJ_FILES = ${OBJDIR}/rtengine.o ${OBJDIR}/view.o ${OBJDIR}/stage.o ${OBJDIR}/glew.o ${OBJDIR}/effect.o ${OBJDIR}/transformation.o \
			${OBJDIR}/layer.o ${OBJDIR}/layerActors.o ${OBJDIR}/layerEffects.o  ${OBJDIR}/layerDebug.o ${OBJDIR}/input.o ${OBJDIR}/entity.o ${OBJDIR}/pawn.o \
			${OBJDIR}/camera.o ${OBJDIR}/cameraOrto.o ${OBJDIR}/cameraPerspective.o \
			${OBJDIR}/viewController.o ${OBJDIR}/stageController.o ${OBJDIR}/debugController.o \
			${OBJDIR}/audioController.o ${OBJDIR}/resourceController.o ${OBJDIR}/profilerController.o \
			${OBJDIR}/inputController.o ${OBJDIR}/logController.o ${OBJDIR}/configController.o \
			${OBJDIR}/shape.o ${OBJDIR}/shapeBox.o ${OBJDIR}/shapeSphere.o ${OBJDIR}/shapeGeometry.o \
			${OBJDIR}/shapePlain.o ${OBJDIR}/shapeConvex.o ${OBJDIR}/shapeCapsule.o \
			${OBJDIR}/actor.o  ${OBJDIR}/actorPawn.o ${OBJDIR}/actorGUIElement.o ${OBJDIR}/actorCamera.o \
			${OBJDIR}/resource.o ${OBJDIR}/resourceSound.o ${OBJDIR}/resourceImage.o ${OBJDIR}/resourceFont.o ${OBJDIR}/resourceMesh.o \
			${OBJDIR}/component.o ${OBJDIR}/componentSoundPlayer.o ${OBJDIR}/texture.o \
			${OBJDIR}/componentText.o ${OBJDIR}/componentLight.o ${OBJDIR}/color.o \
			${OBJDIR}/componentMesh.o ${OBJDIR}/componentAnimatedMesh.o ${OBJDIR}/meshDescriptor.o ${OBJDIR}/renderTarget.o \
			${OBJDIR}/componentSprite.o ${OBJDIR}/componentFramedSprite.o \
			${OBJDIR}/componentCameraOrto.o ${OBJDIR}/componentCameraPerspective.o \
			${OBJDIR}/stb_image.o ${OBJDIR}/stb_vorbis.o \
			${OBJDIR}/destroyable.o ${OBJDIR}/commonShaders.o ${OBJDIR}/utils.o ${OBJDIR}/hullCliping.o \
			${OBJDIR}/phongShader.o ${OBJDIR}/rawShader.o ${OBJDIR}/shader.o ${OBJDIR}/lightningShader.o \
			${OBJDIR}/withLogger.o ${OBJDIR}/withDebug.o ${OBJDIR}/withRepository.o ${OBJDIR}/withMeshMaker.o ${OBJDIR}/withAudio.o ${OBJDIR}/withProfiler.o \
			${OBJDIR}/soundPlayer.o ${OBJDIR}/childProcess.o \
			${OBJDIR}/config.o ${OBJDIR}/mesh.o ${OBJDIR}/geometry.o ${OBJDIR}/dm_sans.o \
			${OBJDIR}/physicsWorld.o ${OBJDIR}/physicsBody.o ${OBJDIR}/hull.o \
			${OBJDIR}/collisionSolver.o ${OBJDIR}/collisionMesh.o \
			${OBJDIR}/meshMaker.o ${OBJDIR}/motion.o ${OBJDIR}/collisionDispatcher.o ${OBJDIR}/collisionCollector.o \
			${OBJDIR}/constraint.o ${OBJDIR}/constraint6DOF.o \
			${OBJDIR}/audioBase.o ${OBJDIR}/audioSource.o \
			${OBJDIR}/mesh.o ${OBJDIR}/meshCompound.o ${OBJDIR}/meshStatic.o \
			${OBJDIR}/loader3d.o \
			${OBJDIR}/loaderFBX.o ${OBJDIR}/FBXNode.o ${OBJDIR}/FBXAnimationStack.o ${OBJDIR}/FBXAnimationCurveNode.o ${OBJDIR}/FBXAnimationCurve.o ${OBJDIR}/FBXAnimationLayer.o \
			${OBJDIR}/animation.o ${OBJDIR}/animator.o ${OBJDIR}/animationTarget.o


EXAMPLES = 	1-helloWorld${EXT} 2-helloActors${EXT} 3-helloPhysics${EXT} 4-helloSorting${EXT} \
			5-helloInput${EXT} 6-helloBytemap${EXT} 7-helloSound${EXT} 8-helloGUI${EXT} \
			9-helloEffects${EXT} 10-helloAnimation${EXT} 11-helloMusic${EXT} 12-hello3d${EXT} \
			13-hello3dPhysics${EXT} 14-helloMushrooms${EXT} 15-helloPlainsAndRays${EXT} \
			16-helloFPV${EXT} 17-helloProfiler${EXT} 18-helloRenderingToTexture${EXT} \
			19-hello3dAnimation${EXT}

all: engine examples

engine: $(TARGET)
${OBJDIR}/rtengine.o: ${SRCDIR}/rtengine.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/rtengine.o ${SRCDIR}/rtengine.cpp

${OBJDIR}/viewController.o: ${SRCDIR}/controller/viewController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/viewController.o ${SRCDIR}/controller/viewController.cpp

${OBJDIR}/stageController.o: ${SRCDIR}/controller/stageController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stageController.o ${SRCDIR}/controller/stageController.cpp

${OBJDIR}/resourceController.o: ${SRCDIR}/controller/resourceController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceController.o ${SRCDIR}/controller/resourceController.cpp

${OBJDIR}/profilerController.o: ${SRCDIR}/controller/profilerController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/profilerController.o ${SRCDIR}/controller/profilerController.cpp

${OBJDIR}/audioController.o: ${SRCDIR}/controller/audioController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/audioController.o ${SRCDIR}/controller/audioController.cpp

${OBJDIR}/inputController.o: ${SRCDIR}/controller/inputController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/inputController.o ${SRCDIR}/controller/inputController.cpp

${OBJDIR}/logController.o: ${SRCDIR}/controller/logController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/logController.o ${SRCDIR}/controller/logController.cpp

${OBJDIR}/configController.o: ${SRCDIR}/controller/configController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/configController.o ${SRCDIR}/controller/configController.cpp

${OBJDIR}/debugController.o: ${SRCDIR}/controller/debugController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/debugController.o ${SRCDIR}/controller/debugController.cpp

${OBJDIR}/view.o: ${SRCDIR}/os/view.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/view.o ${SRCDIR}/os/view.cpp

${OBJDIR}/input.o: ${SRCDIR}/os/input.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/input.o ${SRCDIR}/os/input.cpp

${OBJDIR}/layer.o: ${SRCDIR}/stage/layer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/layer.o ${SRCDIR}/stage/layer.cpp

${OBJDIR}/layerActors.o: ${SRCDIR}/stage/layerActors.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/layerActors.o ${SRCDIR}/stage/layerActors.cpp

${OBJDIR}/layerEffects.o: ${SRCDIR}/stage/layerEffects.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/layerEffects.o ${SRCDIR}/stage/layerEffects.cpp

${OBJDIR}/layerDebug.o: ${SRCDIR}/stage/layerDebug.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/layerDebug.o ${SRCDIR}/stage/layerDebug.cpp

${OBJDIR}/stage.o: ${SRCDIR}/stage/stage.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stage.o ${SRCDIR}/stage/stage.cpp

${OBJDIR}/resource.o: ${SRCDIR}/resource/resource.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resource.o ${SRCDIR}/resource/resource.cpp

${OBJDIR}/resourceImage.o: ${SRCDIR}/resource/resourceImage.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceImage.o ${SRCDIR}/resource/resourceImage.cpp

${OBJDIR}/resourceSound.o: ${SRCDIR}/resource/resourceSound.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceSound.o ${SRCDIR}/resource/resourceSound.cpp

${OBJDIR}/resourceFont.o: ${SRCDIR}/resource/resourceFont.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceFont.o ${SRCDIR}/resource/resourceFont.cpp

${OBJDIR}/resourceMesh.o: ${SRCDIR}/resource/resourceMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceMesh.o ${SRCDIR}/resource/resourceMesh.cpp

${OBJDIR}/vector3.o: ${SRCDIR}/math/vector3.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/vector3.o ${SRCDIR}/math/vector3.cpp

${OBJDIR}/matrix4.o: ${SRCDIR}/math/matrix4.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/matrix4.o ${SRCDIR}/math/matrix4.cpp

${OBJDIR}/vector2.o: ${SRCDIR}/math/vector2.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/vector2.o ${SRCDIR}/math/vector2.cpp
	
${OBJDIR}/vector4.o: ${SRCDIR}/math/vector4.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/vector4.o ${SRCDIR}/math/vector4.cpp
	
${OBJDIR}/transformation.o: ${SRCDIR}/math/transformation.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/transformation.o ${SRCDIR}/math/transformation.cpp
	
${OBJDIR}/hullCliping.o: ${SRCDIR}/math/hullCliping.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/hullCliping.o ${SRCDIR}/math/hullCliping.cpp

${OBJDIR}/stb_image.o: ${SRCDIR}/loaders/stb_image.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stb_image.o ${SRCDIR}/loaders/stb_image.cpp

${OBJDIR}/stb_vorbis.o: ${SRCDIR}/loaders/stb_vorbis.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stb_vorbis.o ${SRCDIR}/loaders/stb_vorbis.cpp

${OBJDIR}/camera.o: ${SRCDIR}/camera/camera.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/camera.o ${SRCDIR}/camera/camera.cpp

${OBJDIR}/cameraOrto.o: ${SRCDIR}/camera/cameraOrto.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/cameraOrto.o ${SRCDIR}/camera/cameraOrto.cpp

${OBJDIR}/cameraPerspective.o: ${SRCDIR}/camera/cameraPerspective.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/cameraPerspective.o ${SRCDIR}/camera/cameraPerspective.cpp

${OBJDIR}/actor.o: ${SRCDIR}/actor/actor.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actor.o ${SRCDIR}/actor/actor.cpp

${OBJDIR}/actorPawn.o: ${SRCDIR}/actor/actorPawn.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actorPawn.o ${SRCDIR}/actor/actorPawn.cpp

${OBJDIR}/actorGUIElement.o: ${SRCDIR}/actor/actorGUIElement.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actorGUIElement.o ${SRCDIR}/actor/actorGUIElement.cpp

${OBJDIR}/actorCamera.o: ${SRCDIR}/actor/actorCamera.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actorCamera.o ${SRCDIR}/actor/actorCamera.cpp

${OBJDIR}/component.o: ${SRCDIR}/component/component.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/component.o ${SRCDIR}/component/component.cpp

${OBJDIR}/componentSprite.o: ${SRCDIR}/component/componentSprite.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentSprite.o ${SRCDIR}/component/componentSprite.cpp

${OBJDIR}/componentCameraOrto.o: ${SRCDIR}/component/componentCameraOrto.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentCameraOrto.o ${SRCDIR}/component/componentCameraOrto.cpp

${OBJDIR}/componentCameraPerspective.o: ${SRCDIR}/component/componentCameraPerspective.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentCameraPerspective.o ${SRCDIR}/component/componentCameraPerspective.cpp

${OBJDIR}/componentFramedSprite.o: ${SRCDIR}/component/componentFramedSprite.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentFramedSprite.o ${SRCDIR}/component/componentFramedSprite.cpp

${OBJDIR}/componentSoundPlayer.o: ${SRCDIR}/component/componentSoundPlayer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentSoundPlayer.o ${SRCDIR}/component/componentSoundPlayer.cpp

${OBJDIR}/componentText.o: ${SRCDIR}/component/componentText.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentText.o ${SRCDIR}/component/componentText.cpp

${OBJDIR}/componentMesh.o: ${SRCDIR}/component/componentMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentMesh.o ${SRCDIR}/component/componentMesh.cpp

${OBJDIR}/componentAnimatedMesh.o: ${SRCDIR}/component/componentAnimatedMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentAnimatedMesh.o ${SRCDIR}/component/componentAnimatedMesh.cpp

${OBJDIR}/componentLight.o: ${SRCDIR}/component/componentLight.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentLight.o ${SRCDIR}/component/componentLight.cpp

${OBJDIR}/shapeGeometry.o: ${SRCDIR}/physics/shapes/shapeGeometry.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeGeometry.o ${SRCDIR}/physics/shapes/shapeGeometry.cpp

${OBJDIR}/shapeSphere.o: ${SRCDIR}/physics/shapes/shapeSphere.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeSphere.o ${SRCDIR}/physics/shapes/shapeSphere.cpp

${OBJDIR}/shapeBox.o: ${SRCDIR}/physics/shapes/shapeBox.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeBox.o ${SRCDIR}/physics/shapes/shapeBox.cpp

${OBJDIR}/shapePlain.o: ${SRCDIR}/physics/shapes/shapePlain.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapePlain.o ${SRCDIR}/physics/shapes/shapePlain.cpp

${OBJDIR}/shapeConvex.o: ${SRCDIR}/physics/shapes/shapeConvex.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeConvex.o ${SRCDIR}/physics/shapes/shapeConvex.cpp

${OBJDIR}/shapeCapsule.o: ${SRCDIR}/physics/shapes/shapeCapsule.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeCapsule.o ${SRCDIR}/physics/shapes/shapeCapsule.cpp

${OBJDIR}/shape.o: ${SRCDIR}/physics/shapes/shape.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shape.o ${SRCDIR}/physics/shapes/shape.cpp

${OBJDIR}/collisionDispatcher.o: ${SRCDIR}/physics/collisionDispatcher.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionDispatcher.o ${SRCDIR}/physics/collisionDispatcher.cpp

${OBJDIR}/collisionCollector.o: ${SRCDIR}/physics/collisionCollector.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionCollector.o ${SRCDIR}/physics/collisionCollector.cpp

${OBJDIR}/hull.o: ${SRCDIR}/physics/hull.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/hull.o ${SRCDIR}/physics/hull.cpp

${OBJDIR}/entity.o: ${SRCDIR}/common/entity.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/entity.o ${SRCDIR}/common/entity.cpp

${OBJDIR}/pawn.o: ${SRCDIR}/common/pawn.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/pawn.o ${SRCDIR}/common/pawn.cpp

${OBJDIR}/color.o: ${SRCDIR}/common/color.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/color.o ${SRCDIR}/common/color.cpp

${OBJDIR}/utils.o: ${SRCDIR}/common/utils.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/utils.o ${SRCDIR}/common/utils.cpp

${OBJDIR}/destroyable.o: ${SRCDIR}/common/destroyable.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/destroyable.o ${SRCDIR}/common/destroyable.cpp

${OBJDIR}/commonShaders.o: ${SRCDIR}/common/commonShaders.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/commonShaders.o ${SRCDIR}/common/commonShaders.cpp

${OBJDIR}/meshDescriptor.o: ${SRCDIR}/common/meshDescriptor.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshDescriptor.o ${SRCDIR}/common/meshDescriptor.cpp
	
${OBJDIR}/renderTarget.o: ${SRCDIR}/common/renderTarget.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/renderTarget.o ${SRCDIR}/common/renderTarget.cpp

${OBJDIR}/withLogger.o: ${SRCDIR}/connector/withLogger.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withLogger.o ${SRCDIR}/connector/withLogger.cpp

${OBJDIR}/withDebug.o: ${SRCDIR}/connector/withDebug.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withDebug.o ${SRCDIR}/connector/withDebug.cpp

${OBJDIR}/withRepository.o: ${SRCDIR}/connector/withRepository.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withRepository.o ${SRCDIR}/connector/withRepository.cpp
	
${OBJDIR}/withMeshMaker.o: ${SRCDIR}/connector/withMeshMaker.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withMeshMaker.o ${SRCDIR}/connector/withMeshMaker.cpp

${OBJDIR}/withAudio.o: ${SRCDIR}/connector/withAudio.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withAudio.o ${SRCDIR}/connector/withAudio.cpp

${OBJDIR}/withProfiler.o: ${SRCDIR}/connector/withProfiler.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withProfiler.o ${SRCDIR}/connector/withProfiler.cpp

${OBJDIR}/soundPlayer.o: ${SRCDIR}/common/soundPlayer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/soundPlayer.o ${SRCDIR}/common/soundPlayer.cpp

${OBJDIR}/childProcess.o: ${SRCDIR}/common/childProcess.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/childProcess.o ${SRCDIR}/common/childProcess.cpp

${OBJDIR}/geometry.o: ${SRCDIR}/common/geometry.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/geometry.o ${SRCDIR}/common/geometry.cpp

${OBJDIR}/config.o: ${SRCDIR}/common/config.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/config.o ${SRCDIR}/common/config.cpp

${OBJDIR}/texture.o: ${SRCDIR}/common/texture.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/texture.o ${SRCDIR}/common/texture.cpp
	
${OBJDIR}/effect.o: ${SRCDIR}/shaders/effect.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/effect.o ${SRCDIR}/shaders/effect.cpp

${OBJDIR}/phongShader.o: ${SRCDIR}/shaders/phongShader.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/phongShader.o ${SRCDIR}/shaders/phongShader.cpp

${OBJDIR}/rawShader.o: ${SRCDIR}/shaders/rawShader.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/rawShader.o ${SRCDIR}/shaders/rawShader.cpp

${OBJDIR}/lightningShader.o: ${SRCDIR}/shaders/lightningShader.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/lightningShader.o ${SRCDIR}/shaders/lightningShader.cpp

${OBJDIR}/shader.o: ${SRCDIR}/shaders/shader.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shader.o ${SRCDIR}/shaders/shader.cpp

${OBJDIR}/glew.o: ${SRCDIR}/opengl/glew.c
	$(CC) $(CFLAGS) -o ${OBJDIR}/glew.o ${SRCDIR}/opengl/glew.c

${OBJDIR}/dm_sans.o: ${SRCDIR}/static/dm_sans.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/dm_sans.o ${SRCDIR}/static/dm_sans.cpp

${OBJDIR}/physicsWorld.o: ${SRCDIR}/physics/physicsWorld.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/physicsWorld.o ${SRCDIR}/physics/physicsWorld.cpp

${OBJDIR}/physicsBody.o: ${SRCDIR}/physics/physicsBody.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/physicsBody.o ${SRCDIR}/physics/physicsBody.cpp

${OBJDIR}/collisionSolver.o: ${SRCDIR}/physics/collisionSolver.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionSolver.o ${SRCDIR}/physics/collisionSolver.cpp

${OBJDIR}/collisionMesh.o: ${SRCDIR}/physics/collisionMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionMesh.o ${SRCDIR}/physics/collisionMesh.cpp

${OBJDIR}/motion.o: ${SRCDIR}/physics/motion.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/motion.o ${SRCDIR}/physics/motion.cpp

${OBJDIR}/constraint.o: ${SRCDIR}/physics/constraint.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/constraint.o ${SRCDIR}/physics/constraint.cpp

${OBJDIR}/constraint6DOF.o: ${SRCDIR}/physics/constraint6DOF.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/constraint6DOF.o ${SRCDIR}/physics/constraint6DOF.cpp

${OBJDIR}/meshMaker.o: ${SRCDIR}/common/meshMaker.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshMaker.o ${SRCDIR}/common/meshMaker.cpp

${OBJDIR}/audioBase.o: ${SRCDIR}/audio/audioBase.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/audioBase.o ${SRCDIR}/audio/audioBase.cpp

${OBJDIR}/audioSource.o: ${SRCDIR}/audio/audioSource.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/audioSource.o ${SRCDIR}/audio/audioSource.cpp

${OBJDIR}/mesh.o: ${SRCDIR}/mesh/mesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/mesh.o ${SRCDIR}/mesh/mesh.cpp

${OBJDIR}/meshCompound.o: ${SRCDIR}/mesh/meshCompound.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshCompound.o ${SRCDIR}/mesh/meshCompound.cpp

${OBJDIR}/meshStatic.o: ${SRCDIR}/mesh/meshStatic.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshStatic.o ${SRCDIR}/mesh/meshStatic.cpp

${OBJDIR}/loader3d.o: ${SRCDIR}/loaders3d/loader3d.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/loader3d.o ${SRCDIR}/loaders3d/loader3d.cpp

${OBJDIR}/loaderFBX.o: ${SRCDIR}/loaders3d/loaderFBX.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/loaderFBX.o ${SRCDIR}/loaders3d/loaderFBX.cpp

${OBJDIR}/FBXNode.o: ${SRCDIR}/loaders3d/FBXNode.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXNode.o ${SRCDIR}/loaders3d/FBXNode.cpp

${OBJDIR}/FBXAnimationStack.o: ${SRCDIR}/loaders3d/FBXAnimationStack.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationStack.o ${SRCDIR}/loaders3d/FBXAnimationStack.cpp

${OBJDIR}/FBXAnimationCurveNode.o: ${SRCDIR}/loaders3d/FBXAnimationCurveNode.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationCurveNode.o ${SRCDIR}/loaders3d/FBXAnimationCurveNode.cpp

${OBJDIR}/FBXAnimationCurve.o: ${SRCDIR}/loaders3d/FBXAnimationCurve.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationCurve.o ${SRCDIR}/loaders3d/FBXAnimationCurve.cpp

${OBJDIR}/FBXAnimationLayer.o: ${SRCDIR}/loaders3d/FBXAnimationLayer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationLayer.o ${SRCDIR}/loaders3d/FBXAnimationLayer.cpp

${OBJDIR}/animation.o: ${SRCDIR}/animation/animation.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animation.o ${SRCDIR}/animation/animation.cpp
	
${OBJDIR}/animator.o: ${SRCDIR}/animation/animator.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animator.o ${SRCDIR}/animation/animator.cpp

${OBJDIR}/animationTarget.o: ${SRCDIR}/animation/animationTarget.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animationTarget.o ${SRCDIR}/animation/animationTarget.cpp

	

$(TARGET): ${OBJ_FILES}
	$(LD) ${LFLAGS} ${LIBRARIES} ${OBJ_FILES} -o $(TARGET)
	${COPY} ${TARGET} "${BINDIR}/${TARGET}"

examples: ${EXAMPLES} engine
${OBJDIR}/1-helloWorld.o: ${EXMDIR}/1-helloWorld.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/1-helloWorld.o ${EXMDIR}/1-helloWorld.cpp

1-helloWorld${EXT}: ${OBJDIR}/1-helloWorld.o
	$(LD) ${EFLAGS} ${OBJDIR}/1-helloWorld.o -o 1-helloWorld${EXT}
	${MOVE} 1-helloWorld${EXT} ${BINDIR}/1-helloWorld${EXT}

${OBJDIR}/2-helloActors.o: ${EXMDIR}/2-helloActors.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/2-helloActors.o ${EXMDIR}/2-helloActors.cpp

2-helloActors${EXT}: ${OBJDIR}/2-helloActors.o
	$(LD) ${EFLAGS} ${OBJDIR}/2-helloActors.o -o 2-helloActors${EXT}
	${MOVE} 2-helloActors${EXT} ${BINDIR}/2-helloActors${EXT}
	
${OBJDIR}/3-helloPhysics.o: ${EXMDIR}/3-helloPhysics.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/3-helloPhysics.o ${EXMDIR}/3-helloPhysics.cpp

3-helloPhysics${EXT}: ${OBJDIR}/3-helloPhysics.o
	$(LD) ${EFLAGS} ${OBJDIR}/3-helloPhysics.o -o 3-helloPhysics${EXT}
	${MOVE} 3-helloPhysics${EXT} ${BINDIR}/3-helloPhysics${EXT}

${OBJDIR}/4-helloSorting.o: ${EXMDIR}/4-helloSorting.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/4-helloSorting.o ${EXMDIR}/4-helloSorting.cpp

4-helloSorting${EXT}: ${OBJDIR}/4-helloSorting.o
	$(LD) ${EFLAGS} ${OBJDIR}/4-helloSorting.o -o 4-helloSorting${EXT}
	${MOVE} 4-helloSorting${EXT} ${BINDIR}/4-helloSorting${EXT}

${OBJDIR}/5-helloInput.o: ${EXMDIR}/5-helloInput.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/5-helloInput.o ${EXMDIR}/5-helloInput.cpp

5-helloInput${EXT}: ${OBJDIR}/5-helloInput.o
	$(LD) ${EFLAGS} ${OBJDIR}/5-helloInput.o -o 5-helloInput${EXT}
	${MOVE} 5-helloInput${EXT} ${BINDIR}/5-helloInput${EXT}

${OBJDIR}/6-helloBytemap.o: ${EXMDIR}/6-helloBytemap.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/6-helloBytemap.o ${EXMDIR}/6-helloBytemap.cpp

6-helloBytemap${EXT}: ${OBJDIR}/6-helloBytemap.o
	$(LD) ${EFLAGS} ${OBJDIR}/6-helloBytemap.o -o 6-helloBytemap${EXT}
	${MOVE} 6-helloBytemap${EXT} ${BINDIR}/6-helloBytemap${EXT}

${OBJDIR}/7-helloSound.o: ${EXMDIR}/7-helloSound.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/7-helloSound.o ${EXMDIR}/7-helloSound.cpp

7-helloSound${EXT}: ${OBJDIR}/7-helloSound.o
	$(LD) ${EFLAGS} ${OBJDIR}/7-helloSound.o -o 7-helloSound${EXT}
	${MOVE} 7-helloSound${EXT} ${BINDIR}/7-helloSound${EXT}

${OBJDIR}/8-helloGUI.o: ${EXMDIR}/8-helloGUI.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/8-helloGUI.o ${EXMDIR}/8-helloGUI.cpp

8-helloGUI${EXT}: ${OBJDIR}/8-helloGUI.o
	$(LD) ${EFLAGS} ${OBJDIR}/8-helloGUI.o -o 8-helloGUI${EXT}
	${MOVE} 8-helloGUI${EXT} ${BINDIR}/8-helloGUI${EXT}

${OBJDIR}/9-helloEffects.o: ${EXMDIR}/9-helloEffects.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/9-helloEffects.o ${EXMDIR}/9-helloEffects.cpp

9-helloEffects${EXT}: ${OBJDIR}/9-helloEffects.o
	$(LD) ${EFLAGS} ${OBJDIR}/9-helloEffects.o -o 9-helloEffects${EXT}
	${MOVE} 9-helloEffects${EXT} ${BINDIR}/9-helloEffects${EXT}

${OBJDIR}/10-helloAnimation.o: ${EXMDIR}/10-helloAnimation.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/10-helloAnimation.o ${EXMDIR}/10-helloAnimation.cpp

10-helloAnimation${EXT}: ${OBJDIR}/10-helloAnimation.o
	$(LD) ${EFLAGS} ${OBJDIR}/10-helloAnimation.o -o 10-helloAnimation${EXT}
	${MOVE} 10-helloAnimation${EXT} ${BINDIR}/10-helloAnimation${EXT}

${OBJDIR}/11-helloMusic.o: ${EXMDIR}/11-helloMusic.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/11-helloMusic.o ${EXMDIR}/11-helloMusic.cpp

11-helloMusic${EXT}: ${OBJDIR}/11-helloMusic.o
	$(LD) ${EFLAGS} ${OBJDIR}/11-helloMusic.o -o 11-helloMusic${EXT}
	${MOVE} 11-helloMusic${EXT} ${BINDIR}/11-helloMusic${EXT}

${OBJDIR}/12-hello3d.o: ${EXMDIR}/12-hello3d.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/12-hello3d.o ${EXMDIR}/12-hello3d.cpp

12-hello3d${EXT}: ${OBJDIR}/12-hello3d.o
	$(LD) ${EFLAGS} ${OBJDIR}/12-hello3d.o -o 12-hello3d${EXT}
	${MOVE} 12-hello3d${EXT} ${BINDIR}/12-hello3d${EXT}

${OBJDIR}/13-hello3dPhysics.o: ${EXMDIR}/13-hello3dPhysics.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/13-hello3dPhysics.o ${EXMDIR}/13-hello3dPhysics.cpp

13-hello3dPhysics${EXT}: ${OBJDIR}/13-hello3dPhysics.o
	$(LD) ${EFLAGS} ${OBJDIR}/13-hello3dPhysics.o -o 13-hello3dPhysics${EXT}
	${MOVE} 13-hello3dPhysics${EXT} ${BINDIR}/13-hello3dPhysics${EXT}

${OBJDIR}/14-helloMushrooms.o: ${EXMDIR}/14-helloMushrooms.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/14-helloMushrooms.o ${EXMDIR}/14-helloMushrooms.cpp

14-helloMushrooms${EXT}: ${OBJDIR}/14-helloMushrooms.o
	$(LD) ${EFLAGS} ${OBJDIR}/14-helloMushrooms.o -o 14-helloMushrooms${EXT}
	${MOVE} 14-helloMushrooms${EXT} ${BINDIR}/14-helloMushrooms${EXT}

${OBJDIR}/15-helloPlainsAndRays.o: ${EXMDIR}/15-helloPlainsAndRays.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/15-helloPlainsAndRays.o ${EXMDIR}/15-helloPlainsAndRays.cpp

15-helloPlainsAndRays${EXT}: ${OBJDIR}/15-helloPlainsAndRays.o
	$(LD) ${EFLAGS} ${OBJDIR}/15-helloPlainsAndRays.o -o 15-helloPlainsAndRays${EXT}
	${MOVE} 15-helloPlainsAndRays${EXT} ${BINDIR}/15-helloPlainsAndRays${EXT}

${OBJDIR}/16-helloFPV.o: ${EXMDIR}/16-helloFPV.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/16-helloFPV.o ${EXMDIR}/16-helloFPV.cpp

16-helloFPV${EXT}: ${OBJDIR}/16-helloFPV.o
	$(LD) ${EFLAGS} ${OBJDIR}/16-helloFPV.o -o 16-helloFPV${EXT}
	${MOVE} 16-helloFPV${EXT} ${BINDIR}/16-helloFPV${EXT}

${OBJDIR}/17-helloProfiler.o: ${EXMDIR}/17-helloProfiler.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/17-helloProfiler.o ${EXMDIR}/17-helloProfiler.cpp

17-helloProfiler${EXT}: ${OBJDIR}/17-helloProfiler.o
	$(LD) ${EFLAGS} ${OBJDIR}/17-helloProfiler.o -o 17-helloProfiler${EXT}
	${MOVE} 17-helloProfiler${EXT} ${BINDIR}/17-helloProfiler${EXT}

${OBJDIR}/18-helloRenderingToTexture.o: ${EXMDIR}/18-helloRenderingToTexture.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/18-helloRenderingToTexture.o ${EXMDIR}/18-helloRenderingToTexture.cpp

18-helloRenderingToTexture${EXT}: ${OBJDIR}/18-helloRenderingToTexture.o
	$(LD) ${EFLAGS} ${OBJDIR}/18-helloRenderingToTexture.o -o 18-helloRenderingToTexture${EXT}
	${MOVE} 18-helloRenderingToTexture${EXT} ${BINDIR}/18-helloRenderingToTexture${EXT}

${OBJDIR}/19-hello3dAnimation.o: ${EXMDIR}/19-hello3dAnimation.cpp ${EXMDIR}/helpers.h
	$(CC) $(CFLAGS) -o ${OBJDIR}/19-hello3dAnimation.o ${EXMDIR}/19-hello3dAnimation.cpp

19-hello3dAnimation${EXT}: ${OBJDIR}/19-hello3dAnimation.o
	$(LD) ${EFLAGS} ${OBJDIR}/19-hello3dAnimation.o -o 19-hello3dAnimation${EXT}
	${MOVE} 19-hello3dAnimation${EXT} ${BINDIR}/19-hello3dAnimation${EXT}


# llvm-objcopy
clean:
	$(RM) $(TARGET)