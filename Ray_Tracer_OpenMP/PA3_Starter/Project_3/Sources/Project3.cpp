/*
Project 3 Submission for CMPSC458
Name: Jamin Valick
psu id: jmv5604
*/

#include <Project3.hpp>
#include <omp.h>
#define CLUSTER true

int frame = 0;


//  Modify this preamble based on your code.  It should contain all the functionality of your project.  
std::string preamble =
"Project 3 code: Raytracing \n"
"Sample Input:  SampleScenes/reflectionTest.ray \n"
"Also, asks whether or not you want to configure for use with clusters (does not display results while running) \n"
"If you want to run without being asked for an input\n"
"'./Project_3/Project_3  SampleScenes/reflectionTest.ray y'\n\n";
int main(int argc, char **argv)
{
	//loop used for animation
	//int maxFrames = 600;
	//while (frame <= maxFrames) {

		std::printf(preamble.c_str());
		std::string fn, ans_str;
		std::string inputFile;
		char inputYN[999];

		// This turns off the window and OpenGL functions so that you can run it on the clusters.    
			//  This will also make it run slightly faster since it doesn't need to display in every 10 rows. 
		bool cluster = false;
		if (argc < 2)// if not specified, prompt for filename and if running on cluster
		{
			std::printf("Input .ray file: ");
			//std::cin >> inputFile;
			//fn = inputFile;
			fn = "myScene3.ray";
			//std::printf("Running on cluster or no display (y/n): ");
			//std::cin >> inputYN;
			ans_str = "n";
			cluster = (ans_str.compare("y") == 0) || (ans_str.compare("Y") == 0);
		}
		else //otherwise, use the name provided
		{
			/* Open jpeg (reads into memory) */
			char* filename = argv[1];
			fn = filename;

			if (argc < 3)
			{
				char input[999];
				std::printf("Running on cluster or no display (y/n): ");
				std::cin >> input;
				ans_str = input;
				cluster = (ans_str.compare("y") == 0) || (ans_str.compare("Y") == 0);
			}
			else
			{
				char* ans = argv[2];
				ans_str = ans;
				cluster = (ans_str.compare("y") == 0) || (ans_str.compare("Y") == 0);
			}
		}


		if (cluster)
			std::printf("Configured for Clusters\n");
		else
			std::printf("Not configured for Clusters\n");
		fn = "../Project_3/Media/SampleScenes/" + fn;
		std::printf("Opening File %s\n", fn.c_str());
		myScene = new scene(fn.c_str());
		GLFWwindow* window = NULL;

		Shader screenShader("", "");
		if (!cluster)
		{
			// glfw: initialize and configure
			// ------------------------------

			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	// glfw window creation
	// --------------------
			window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project 3", NULL, NULL);
			if (window == NULL)
			{
				std::cout << "Failed to create GLFW window" << std::endl;
				glfwTerminate();
				return -1;
			}
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

			// glad: load all OpenGL function pointers
			// ---------------------------------------
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				std::cout << "Failed to initialize GLAD" << std::endl;
				return -1;
			}


			// configure global opengl state
			// -----------------------------
			glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure

			// build and compile shaders
			// -------------------------


			float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
									 // positions   // texCoords
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				1.0f, -1.0f,  1.0f, 0.0f,

				-1.0f,  1.0f,  0.0f, 1.0f,
				1.0f, -1.0f,  1.0f, 0.0f,
				1.0f,  1.0f,  1.0f, 1.0f
			};

			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

			// shader configuration
			// --------------------

			screenShader = Shader("../Project_3/Shaders/screenShader.vert", "../Project_3/Shaders/screenShader.frag");
			screenShader.use();
			screenShader.setInt("screenTexture", 0);
			glActiveTexture(GL_TEXTURE0);
			// Create initial image texture
			// Starts from the top left (0,0) to the bottom right
			// X is the column number and Y is the row number
			glGenTextures(1, &textureColorbuffer);
		}
		for (int y = 0; y < SCR_HEIGHT; y++)
			for (int x = 0; x < SCR_WIDTH; x++)
				image.push_back(glm::u8vec3(0));

		if (!cluster)
			update_image_texture();


		bool raytracing = true;

		//get camera parameters from scene
		glm::vec3 eye = myScene->getEye();
		glm::vec3 lookAt = myScene->getLookAt();
		glm::vec3 up = myScene->getUp();
		float fovy = myScene->getFovy();

		fovy = fovy * 3.1416 / 180;

		//direction variables
		glm::vec3 gaze = glm::normalize(lookAt - eye); //get the direction we are looking
		glm::vec3 left = glm::normalize(glm::cross(up, gaze)); //cross up and dir to get a vector to our left
		float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
		float height = 2 * sin(fovy / 2);			//height of viewing rectangle
		float width = aspectRatio * height;		//width of viewing rectangle
		glm::vec3 topLeft = eye + gaze + height / 2 * up + width / 2 * left; //top left of viewing rectangle
		float pixelWidth = width / (float)SCR_WIDTH;	//width of each pixel
		float pixelHeight = height / (float)SCR_HEIGHT; //height of each pixel

		glm::vec3 currentColor;

		// render loop
		// -----------
		bool running = true;
		if (!cluster)
			running = !glfwWindowShouldClose(window);

		while (running)
		{
			double start;
			double end;
			start = omp_get_wtime();

			// Start the ray tracing
			if (raytracing)
			{
				// Initialize random number generator
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float> dis(0.0f, 0.001f);
				unsigned int SAMPLES_PER_PIXEL = 8;
				
				// Go through 10 rows in parallel
				//#pragma omp parallel for schedule(dynamic)
				for (int y = 0; y < SCR_HEIGHT; y++)
				{
					// This runs the following loop in parallel -- You can comment the next line out for debugging purposes 
					#pragma omp parallel for schedule(dynamic)
					for (int x = 0; x < SCR_WIDTH; x++)
					{
						glm::vec3 currentColor(0.0f);

						// Calculate Ray Direction
						float horiz = ((float)x + 0.5) * pixelWidth;  //horizontal offset
						float vert = ((float)y + 0.5) * pixelHeight;  //vertical offset
						glm::vec3 position = topLeft - horiz * left - vert * up; //position on viewing rectangle

						glm::vec3 direction = glm::normalize(position - eye); //direction from eye to pixel

						// Get multiple samples per pixel to reduce aliasing
						//#pragma omp parallel for schedule(dynamic)
						for (int sample = 0; sample < SAMPLES_PER_PIXEL; sample++) {

							// Perturb ray
							glm::vec3 directionNew = direction;
							directionNew.x += dis(gen);
							directionNew.y += dis(gen);

							// Get color of pixel
							currentColor += myScene->rayTrace(eye, directionNew, 0);
						}

						// Get average color
						currentColor /= SAMPLES_PER_PIXEL;

						// Put the color into our image buffer.  
						image[x + y * SCR_WIDTH] = glm::u8vec3(glm::clamp(currentColor, 0.0f, 1.0f) * 255.0f);
					
					}

					// Draw and process input for every 10 completed rows
					if (!cluster && (y % 10 == 0 || y == SCR_HEIGHT - 1))
					{
						// input
						// -----
						processInput(window);
						if (glfwWindowShouldClose(window))
							break;

						//  Update texture for for drawing
						update_image_texture();


						// Draw the textrue
						glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
						glClear(GL_COLOR_BUFFER_BIT);
						// Bind our texture we are creating
						glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
						glBindVertexArray(quadVAO);
						glDrawArrays(GL_TRIANGLES, 0, 6);

						glfwSwapBuffers(window);
						glfwPollEvents();
					}
				}
				// Write the final output image
				//std::string outFileString = "../Project_3/RenderingScenes/my_" + inputFile + ".png";
				//const char* outFile = outFileString.c_str();
				const char* outFile = "../Project_3/Result_Images/final_out.png";
				stbi_write_png(outFile, SCR_WIDTH, SCR_HEIGHT, 3, &image[0], sizeof(glm::u8vec3) * SCR_WIDTH);
			}
			// Done rendering, just draw the image now
			raytracing = false;



			if (!cluster)
			{
				// input
				// -----
				processInput(window);

				// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				// Bind our texture we are creating
				glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				glfwSwapBuffers(window);
				glfwPollEvents();

				running = !glfwWindowShouldClose(window);
				running = false;  //used to move onto the next frame
			}
			else
				running = false;

			end = omp_get_wtime();
			printf("\nRuntime: %f seconds\n", end - start);

		}
		// optional: de-allocate all resources once they've outlived their purpose:
		// ------------------------------------------------------------------------
		if (!cluster)
		{
			glDeleteBuffers(1, &quadVAO);
			glDeleteBuffers(1, &quadVBO);

			glfwTerminate();
		}
		//frame += 1;
	//} //end frame loop
		return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//   The movement of the boxes is still here.  Feel free to use it or take it out
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	// Escape Key quits
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	// P Key saves the image
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		stbi_write_png("../Project_3/out.png", SCR_WIDTH, SCR_HEIGHT, 3, &image[0], sizeof(glm::u8vec3)*SCR_WIDTH);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);

}

// Update the current texture in image and sent the data to the textureColorbuffer
void update_image_texture()
{
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
