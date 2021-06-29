#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include <bass.h>
#include <cmath>

Animation* anim = NULL;
float mouse_speed = 80.0f;
FBO* fbo = NULL;

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;
	
	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective

	string audios = "data/audio/organ.wav,data/audio/interaction.wav,data/audio/end_glass.wav,data/audio/broken_glass.wav,data/audio/portal.wav,data/audio/light_sound.wav,data/audio/walk_water.wav,data/audio/walk_gravel.wav,data/audio/walk_eco.wav,data/audio/intro.wav,data/audio/bath.wav,data/audio/mind.wav,data/audio/soul.wav,data/audio/title.wav,data/audio/bath.wav";

	string cad;
	int found = -1;
	int init = 0;
	 
	for (int i = 0; i < MAX_AUDIO; i++)
	{
		init = found + 1;
		found = audios.find(",", found + 1);
		cad = audios.substr(init, found - init);
		if (!audio->Get(cad.c_str())) { //manage the audio
			audio = new Audio(cad.c_str());
			sLoadedAudios.insert(std::pair<std::string, Audio* >(cad, audio));
		}
	}
	directional = new EntityLight();

	spot = new EntityLight(spot->eLightType::SPOT, Vector3(-80, 25, 0), Vector3(0.5f, -1.0f, 0.0f), Vector3(0.95f, 0.96f, 0.72f), 0.2f); 
	spot2 = new EntityLight(spot->eLightType::SPOT, Vector3(-125, 25, 0), Vector3(0.5f, -1.0f, 0.0f), Vector3(0.95f, 0.96f, 0.72f), 0.8f);
	spot3 = new EntityLight(spot->eLightType::SPOT, Vector3(40.0f, 40.0f, 4.0f), Vector3(0.5f, -1.0f, 0.0f), Vector3(0.95f, 0.96f, 0.72f), 0);

	//Scene and stages
	intro_scene = new Scene();
	intro_scene->lights.push_back(directional);

	BodyScene = new Scene(); //body scene
	BodyScene->lights.push_back(directional);

	mind_scene = new Scene();
	mind_scene->lights.push_back(spot2);
	mind_scene->lights.push_back(spot);

	soul_scene = new Scene();
	soul_scene->lights.push_back(directional);

	corridor_scene = new Scene();
	corridor_scene->lights.push_back(directional);
	corridor_scene->lights.push_back(spot3);


	CurrentScene = intro_scene;

	title_stage = new TitleStage();
	intro_stage = new IntroStage();
	body_stage = new BodyStage();
	soul_stage = new SoulStage();
	end_stage = new EndStage();
	corridor_stage = new CorridorStage();
	mind_stage = new MindStage(); 
	current_stage = title_stage;

	current_stage->createEntities();
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
	free_camera = false;
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	current_stage->render();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
	//mouse input to rotate the cam
	
	if (free_camera) {
		
		if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
		}
		
		//async input to move the camera around
		if(Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_E))  camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
		
	}
	current_stage->update(seconds_elapsed);
	
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) free_camera = !free_camera;

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

