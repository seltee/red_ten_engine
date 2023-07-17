CC = clang++
LD = clang++

OPENAL_LIB_PATH = "../openal"
SDL_LIB_PATH = "../SDL2-2.0.14"
SDL_TTF_LIB_PATH = "../SDL2_ttf-2.0.15"

CFLAGS = -I${OPENAL_LIB_PATH}/include -Isrc \
	     -I${SDL_LIB_PATH}/include -I${SDL_TTF_LIB_PATH}/include \
		 -Wall -c  -std=c++17 -mfpmath=sse -mavx -g

LFLAGS = -shared -Wall -g 
LIBRARIES = -L${SDL_LIB_PATH}/lib/x64/ -L${OPENAL_LIB_PATH}/libs/Win64 \
			-L${SDL_TTF_LIB_PATH}/lib/x64 \
			-lSDL2 -lSDL2main -lkernel32 -luser32 -lgdi32 -lwinspool -lSDL2_ttf.lib \
			-lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 -lopengl32 -lOpenAL32.lib

# The build target 
TARGET = rtengine
SRCDIR = src
EXMDIR = examples
OBJDIR = objects
BINDIR = bin
 
OBJ_FILES = ${OBJDIR}/rtengine.o ${OBJDIR}/view.o ${OBJDIR}/stage.o ${OBJDIR}/glew.o ${OBJDIR}/effect.o ${OBJDIR}/transformation.o \
			${OBJDIR}/layer.o ${OBJDIR}/layerActors.o ${OBJDIR}/layerEffects.o  ${OBJDIR}/layerDebug.o ${OBJDIR}/input.o ${OBJDIR}/entity.o ${OBJDIR}/pawn.o \
			${OBJDIR}/camera.o ${OBJDIR}/cameraOrto.o ${OBJDIR}/cameraPerspective.o  \
			${OBJDIR}/viewController.o ${OBJDIR}/stageController.o ${OBJDIR}/debugController.o  \
			${OBJDIR}/soundController.o ${OBJDIR}/resourceController.o \
			${OBJDIR}/inputController.o ${OBJDIR}/logController.o ${OBJDIR}/configController.o \
			${OBJDIR}/shape.o ${OBJDIR}/shapeBox.o ${OBJDIR}/shapeSphere.o ${OBJDIR}/shapeGeometry.o ${OBJDIR}/shapePlain.o ${OBJDIR}/shapeConvex.o \
			${OBJDIR}/actor.o  ${OBJDIR}/actorPawn.o ${OBJDIR}/actorGUIElement.o \
			${OBJDIR}/sound.o ${OBJDIR}/texture.o ${OBJDIR}/resourceFont.o ${OBJDIR}/resourceMesh.o \
			${OBJDIR}/component.o ${OBJDIR}/componentSoundPlayer.o \
			${OBJDIR}/componentText.o ${OBJDIR}/componentLight.o ${OBJDIR}/color.o \
			${OBJDIR}/componentMesh.o ${OBJDIR}/meshDescriptor.o ${OBJDIR}/renderer.o \
			${OBJDIR}/componentSprite.o ${OBJDIR}/componentFramedSprite.o \
			${OBJDIR}/stb_image.o ${OBJDIR}/fbx_loader.o ${OBJDIR}/stb_vorbis.o \
			${OBJDIR}/destroyable.o ${OBJDIR}/commonShaders.o ${OBJDIR}/utils.o ${OBJDIR}/hullCliping.o \
			${OBJDIR}/phongShader.o ${OBJDIR}/rawShader.o ${OBJDIR}/shader.o ${OBJDIR}/lightningShader.o \
			${OBJDIR}/withLogger.o ${OBJDIR}/withDebug.o ${OBJDIR}/withRepository.o ${OBJDIR}/withMeshMaker.o \
			${OBJDIR}/soundPlayer.o ${OBJDIR}/childProcess.o \
			${OBJDIR}/config.o ${OBJDIR}/mesh.o ${OBJDIR}/geometry.o ${OBJDIR}/dm_sans.o \
			${OBJDIR}/physicsWorld.o ${OBJDIR}/physicsBody.o ${OBJDIR}/hull.o \
			${OBJDIR}/collisionSolver.o ${OBJDIR}/collisionMesh.o \
			${OBJDIR}/meshMaker.o ${OBJDIR}/motion.o ${OBJDIR}/collisionDispatcher.o ${OBJDIR}/collisionCollector.o \
			${OBJDIR}/constraint.o ${OBJDIR}/constraint6DOF.o

EXAMPLES = 	${BINDIR}/1-helloWorld.exe ${BINDIR}/2-helloActors.exe ${BINDIR}/3-helloPhysics.exe ${BINDIR}/4-helloSorting.exe \
			${BINDIR}/5-helloInput.exe ${BINDIR}/6-helloBytemap.exe ${BINDIR}/7-helloSound.exe ${BINDIR}/8-helloGUI.exe \
			${BINDIR}/9-helloEffects.exe ${BINDIR}/10-helloAnimation.exe ${BINDIR}/11-helloMusic.exe ${BINDIR}/12-hello3d.exe \
			${BINDIR}/13-hello3dPhysics.exe ${BINDIR}/14-helloMushrooms.exe ${BINDIR}/15-helloPlainsAndRays.exe

all: engine examples

engine: $(BINDIR)/$(TARGET).dll
${OBJDIR}/rtengine.o: ${SRCDIR}/rtengine.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/rtengine.o ${SRCDIR}/rtengine.cpp

${OBJDIR}/viewController.o: ${SRCDIR}/controller/viewController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/viewController.o ${SRCDIR}/controller/viewController.cpp

${OBJDIR}/stageController.o: ${SRCDIR}/controller/stageController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stageController.o ${SRCDIR}/controller/stageController.cpp

${OBJDIR}/resourceController.o: ${SRCDIR}/controller/resourceController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceController.o ${SRCDIR}/controller/resourceController.cpp

${OBJDIR}/soundController.o: ${SRCDIR}/controller/soundController.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/soundController.o ${SRCDIR}/controller/soundController.cpp

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

${OBJDIR}/texture.o: ${SRCDIR}/resource/texture.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/texture.o ${SRCDIR}/resource/texture.cpp

${OBJDIR}/sound.o: ${SRCDIR}/resource/sound.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/sound.o ${SRCDIR}/resource/sound.cpp

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
	
${OBJDIR}/fbx_loader.o: ${SRCDIR}/loaders/fbx_loader.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/fbx_loader.o ${SRCDIR}/loaders/fbx_loader.cpp

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

${OBJDIR}/component.o: ${SRCDIR}/component/component.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/component.o ${SRCDIR}/component/component.cpp

${OBJDIR}/componentSprite.o: ${SRCDIR}/component/componentSprite.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentSprite.o ${SRCDIR}/component/componentSprite.cpp

${OBJDIR}/componentFramedSprite.o: ${SRCDIR}/component/componentFramedSprite.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentFramedSprite.o ${SRCDIR}/component/componentFramedSprite.cpp

${OBJDIR}/componentSoundPlayer.o: ${SRCDIR}/component/componentSoundPlayer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentSoundPlayer.o ${SRCDIR}/component/componentSoundPlayer.cpp

${OBJDIR}/componentText.o: ${SRCDIR}/component/componentText.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentText.o ${SRCDIR}/component/componentText.cpp

${OBJDIR}/componentMesh.o: ${SRCDIR}/component/componentMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentMesh.o ${SRCDIR}/component/componentMesh.cpp

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
	
${OBJDIR}/renderer.o: ${SRCDIR}/common/renderer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/renderer.o ${SRCDIR}/common/renderer.cpp

${OBJDIR}/withLogger.o: ${SRCDIR}/connector/withLogger.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withLogger.o ${SRCDIR}/connector/withLogger.cpp

${OBJDIR}/withDebug.o: ${SRCDIR}/connector/withDebug.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withDebug.o ${SRCDIR}/connector/withDebug.cpp

${OBJDIR}/withRepository.o: ${SRCDIR}/connector/withRepository.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withRepository.o ${SRCDIR}/connector/withRepository.cpp
	
${OBJDIR}/withMeshMaker.o: ${SRCDIR}/connector/withMeshMaker.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/withMeshMaker.o ${SRCDIR}/connector/withMeshMaker.cpp

${OBJDIR}/soundPlayer.o: ${SRCDIR}/common/soundPlayer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/soundPlayer.o ${SRCDIR}/common/soundPlayer.cpp

${OBJDIR}/childProcess.o: ${SRCDIR}/common/childProcess.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/childProcess.o ${SRCDIR}/common/childProcess.cpp

${OBJDIR}/geometry.o: ${SRCDIR}/common/geometry.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/geometry.o ${SRCDIR}/common/geometry.cpp

${OBJDIR}/config.o: ${SRCDIR}/common/config.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/config.o ${SRCDIR}/common/config.cpp

${OBJDIR}/mesh.o: ${SRCDIR}/common/mesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/mesh.o ${SRCDIR}/common/mesh.cpp
	
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


$(BINDIR)/$(TARGET).dll: ${OBJ_FILES}
	$(LD) ${LFLAGS} ${LIBRARIES} ${OBJ_FILES} -o $(BINDIR)/$(TARGET).dll




examples: ${EXAMPLES} engine
${OBJDIR}/1-helloWorld.o: ${EXMDIR}/1-helloWorld.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/1-helloWorld.o ${EXMDIR}/1-helloWorld.cpp

${BINDIR}/1-helloWorld.exe: ${OBJDIR}/1-helloWorld.o
	$(LD) -Wl ${OBJDIR}/1-helloWorld.o -o ${BINDIR}/1-helloWorld.exe

${OBJDIR}/2-helloActors.o: ${EXMDIR}/2-helloActors.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/2-helloActors.o ${EXMDIR}/2-helloActors.cpp

${BINDIR}/2-helloActors.exe: ${OBJDIR}/2-helloActors.o
	$(LD) -Wl ${OBJDIR}/2-helloActors.o -o ${BINDIR}/2-helloActors.exe
	
${OBJDIR}/3-helloPhysics.o: ${EXMDIR}/3-helloPhysics.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/3-helloPhysics.o ${EXMDIR}/3-helloPhysics.cpp

${BINDIR}/3-helloPhysics.exe: ${OBJDIR}/3-helloPhysics.o
	$(LD) -Wl ${OBJDIR}/3-helloPhysics.o -o ${BINDIR}/3-helloPhysics.exe

${OBJDIR}/4-helloSorting.o: ${EXMDIR}/4-helloSorting.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/4-helloSorting.o ${EXMDIR}/4-helloSorting.cpp

${BINDIR}/4-helloSorting.exe: ${OBJDIR}/4-helloSorting.o
	$(LD) -Wl ${OBJDIR}/4-helloSorting.o -o ${BINDIR}/4-helloSorting.exe

${OBJDIR}/5-helloInput.o: ${EXMDIR}/5-helloInput.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/5-helloInput.o ${EXMDIR}/5-helloInput.cpp

${BINDIR}/5-helloInput.exe: ${OBJDIR}/5-helloInput.o
	$(LD) -Wl ${OBJDIR}/5-helloInput.o -o ${BINDIR}/5-helloInput.exe

${OBJDIR}/6-helloBytemap.o: ${EXMDIR}/6-helloBytemap.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/6-helloBytemap.o ${EXMDIR}/6-helloBytemap.cpp

${BINDIR}/6-helloBytemap.exe: ${OBJDIR}/6-helloBytemap.o
	$(LD) -Wl ${OBJDIR}/6-helloBytemap.o -o ${BINDIR}/6-helloBytemap.exe

${OBJDIR}/7-helloSound.o: ${EXMDIR}/7-helloSound.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/7-helloSound.o ${EXMDIR}/7-helloSound.cpp

${BINDIR}/7-helloSound.exe: ${OBJDIR}/7-helloSound.o
	$(LD) -Wl ${OBJDIR}/7-helloSound.o -o ${BINDIR}/7-helloSound.exe

${OBJDIR}/8-helloGUI.o: ${EXMDIR}/8-helloGUI.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/8-helloGUI.o ${EXMDIR}/8-helloGUI.cpp

${BINDIR}/8-helloGUI.exe: ${OBJDIR}/8-helloGUI.o
	$(LD) -Wl ${OBJDIR}/8-helloGUI.o -o ${BINDIR}/8-helloGUI.exe

${OBJDIR}/9-helloEffects.o: ${EXMDIR}/9-helloEffects.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/9-helloEffects.o ${EXMDIR}/9-helloEffects.cpp

${BINDIR}/9-helloEffects.exe: ${OBJDIR}/9-helloEffects.o
	$(LD) -Wl ${OBJDIR}/9-helloEffects.o -o ${BINDIR}/9-helloEffects.exe

${OBJDIR}/10-helloAnimation.o: ${EXMDIR}/10-helloAnimation.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/10-helloAnimation.o ${EXMDIR}/10-helloAnimation.cpp

${BINDIR}/10-helloAnimation.exe: ${OBJDIR}/10-helloAnimation.o
	$(LD) -Wl ${OBJDIR}/10-helloAnimation.o -o ${BINDIR}/10-helloAnimation.exe

${OBJDIR}/11-helloMusic.o: ${EXMDIR}/11-helloMusic.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/11-helloMusic.o ${EXMDIR}/11-helloMusic.cpp

${BINDIR}/11-helloMusic.exe: ${OBJDIR}/11-helloMusic.o
	$(LD) -Wl ${OBJDIR}/11-helloMusic.o -o ${BINDIR}/11-helloMusic.exe

${OBJDIR}/12-hello3d.o: ${EXMDIR}/12-hello3d.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/12-hello3d.o ${EXMDIR}/12-hello3d.cpp

${BINDIR}/12-hello3d.exe: ${OBJDIR}/12-hello3d.o
	$(LD) -Wl ${OBJDIR}/12-hello3d.o -o ${BINDIR}/12-hello3d.exe

${OBJDIR}/13-hello3dPhysics.o: ${EXMDIR}/13-hello3dPhysics.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/13-hello3dPhysics.o ${EXMDIR}/13-hello3dPhysics.cpp

${BINDIR}/13-hello3dPhysics.exe: ${OBJDIR}/13-hello3dPhysics.o
	$(LD) -Wl ${OBJDIR}/13-hello3dPhysics.o -o ${BINDIR}/13-hello3dPhysics.exe

${OBJDIR}/14-helloMushrooms.o: ${EXMDIR}/14-helloMushrooms.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/14-helloMushrooms.o ${EXMDIR}/14-helloMushrooms.cpp

${BINDIR}/14-helloMushrooms.exe: ${OBJDIR}/14-helloMushrooms.o
	$(LD) -Wl ${OBJDIR}/14-helloMushrooms.o -o ${BINDIR}/14-helloMushrooms.exe

${OBJDIR}/15-helloPlainsAndRays.o: ${EXMDIR}/15-helloPlainsAndRays.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/15-helloPlainsAndRays.o ${EXMDIR}/15-helloPlainsAndRays.cpp

${BINDIR}/15-helloPlainsAndRays.exe: ${OBJDIR}/15-helloPlainsAndRays.o
	$(LD) -Wl ${OBJDIR}/15-helloPlainsAndRays.o -o ${BINDIR}/15-helloPlainsAndRays.exe



# llvm-objcopy
clean:
	$(RM) $(TARGET)