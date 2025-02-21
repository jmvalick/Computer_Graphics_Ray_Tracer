Best way to run:
Build with CMAKE
Open in Visual Studio
Set Project_3 as startup project
Run through Visual Studio


## Summary
The program implements Ray Tracing

Contents:
	Project3
		Headers
			light.hpp
			material.hpp
			mesh.hpp
			model.hpp
			Project3.hpp
			rtObject.h
			rtObjGroup.h
			scene.h
			shader.hpp
			sphere.h
			triangle.h
		Media
			sampleScenes
			textures
		Shaders
			screenShader.frag
			screenShader.vert
		Sources
			Project3.cpp
			rtObjGroup.cpp
			rtObjGroup.cpp
			scene.cpp
			sphere.cpp
			triangle.cpp
	RenderingScenes
		list of rendering results
	my_animation.avi/mp4
	Project_3.exe

Changes from starter code:
	Project3.cpp
		lines 11,23-35,316-317:
			animation loop (currently not in use)
		lines 191-240:
			for each pixel:
			calculate the direction through the pixel
			call rayTrace
			take multiple samples per pixels
			add pixel color to frame buffer for rendering
	scene.cpp
		lines 15-162:
			rayTrace algorithm
			lighting, reflection, and refraction
		lines 249-255:
			camera movement for animation (currently not in use)
		lines 360-367:
			sphere movement for animation (currently not in use)
	sphere.cpp
		lines 18-48:
			sphere testIntersection implemented
		line 57:
			normal calculated
		lines 66-100:
			sphere texture coordinates implemented
		lines 92-96:
			texture rotation for animation (currently not in use)
	triangle.cpp:
		lines 26-65:
			triangle testIntersection implemented
		lines 86-109:
			triangle texture coordinates implemented
