/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H
#define MAX_AUDIO 15

#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "Scene.h"
#include "Stage.h"

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;
	
	//Scenes

	Scene* mind_scene;
	Scene* intro_scene;
	Scene* BodyScene;
	Scene* EndScene;
	Scene* corridor_scene; 
	Scene* soul_scene;
	Scene* CurrentScene;

	//Stages
	Stage* title_stage;
	Stage* intro_stage;
	Stage* mind_stage;
	Stage* body_stage;
	Stage* soul_stage;
	Stage* corridor_stage; 
	Stage* end_stage;
	Stage* current_stage;
	
	//audio
	Audio* audio;
	std::map<std::string, Audio*> sLoadedAudios; //para nuestro manager


	//Lights
	EntityLight* directional;
	EntityLight* spot;
	EntityLight* spot2;
	EntityLight* spot3;
	EntityLight* point;


	bool free_camera;
	//No entrar dos veces en la misma sala
	bool OneBody = false;
	bool OneMind = false;
	bool OneSoul = false;
	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

};


#endif 