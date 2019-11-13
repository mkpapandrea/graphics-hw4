// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
   // glfwPollEvents();
    //glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	GLuint Texture = loadDDS("godihopethisworks.dds");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("ramen.obj", vertices, uvs, normals);



/////////////////define these points yourself. ramdom numbers work



	//catmull rom stuff 
	std::vector<glm::vec3> yeet={ 
		glm::vec3(-10,0,25),
		glm::vec3(-10,0,20),
		glm::vec3(-10,0,15),
		glm::vec3(-10,0,10),
		glm::vec3(-10,0,5),		//5
		glm::vec3(-8,0,5),
		glm::vec3(-6,0,5),
		glm::vec3(-4,0,5),
		glm::vec3(-2,0,5),
		glm::vec3(0,0,5),		//10
		glm::vec3(2,0,6),
		glm::vec3(4,0,7),
		glm::vec3(6,0,8),
		glm::vec3(8,0,9),
		glm::vec3(10,0,10),		//15
		glm::vec3(10,0,8),
		glm::vec3(10,0,6),
		glm::vec3(10,0,4),
		glm::vec3(10,0,6),
		glm::vec3(10,-1,6),		//20
		glm::vec3(10,-2,6),
		glm::vec3(10,-3,6),
		glm::vec3(10,-4,6),
		glm::vec3(10,-5,6),
		glm::vec3(10,-6,6),		//25
	};

	int betweenyeets=10000;		//these are the amount of steps between the points defined above. the more points the more smooth it is


// catmull rom formula
std::vector<glm::vec3> path;
for(int i=0;i<25-2; i++){
	for (int j=0;j<betweenyeets; j++){
		float z= (float) i/ (float) betweenyeets;
		glm::vec3 bpoint;
		bpoint= z*z*z* ((-1.0f) * yeet[j-i] + 3.0f * yeet[j] - 3.0f * yeet[j+1] + yeet[j+2]) / 2.0f;
		bpoint+= z*z* (2.0f *yeet[j-1] - 5.0f * yeet[j] + 4.0f * yeet[j+1] - yeet[j+2]) / 2.0f;
		bpoint += z* ((-1.0f) * yeet[j-1] + yeet[j+1]) / 2.0f;
		bpoint += yeet[j];

		path.push_back(bpoint);
	}
}



int countpath=0;	//increments through path
bool pauseframe=false;		//for the p button
int framecount=0;		//slows it down

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f,100.0f);

		//view matrix editied by catmull rom
		glm::mat4 ViewMatrix = glm::lookAt( path[countpath],glm::vec3(0,0,0),glm::vec3(0,1,0));

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = projectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


/////////////buttton and increment logic
		if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
			pauseframe =!pauseframe;
		}

		if (pauseframe==true){
			countpath=countpath;
		}
		else {
			if(framecount%3==0){	// mods by seperate counter to slow it down. mod by any number. doesnt need to be 3
			if(countpath>25){
				countpath=0;
			}
			else{
				countpath++;
			}
			}
		}
		if(framecount >1000){		//just put a number greater than 500 so framecount isnt infinite
			framecount=0;
		}
		framecount++;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

