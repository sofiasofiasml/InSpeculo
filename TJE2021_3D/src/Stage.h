
#ifndef STAGE_H
#define STAGE_H

#include "Scene.h"
#include "framework.h"

#define MAX_CHARACTERS 1

#define MAX_ENT_INTRO 6
#define MAX_ENT_BODY 20 //comun mirror and reality
#define MAX_ENT_MIND 24 //comun mirror and reality
#define MAX_ENT_SOUL 25
#define MAX_ENT_CORRIDOR 24
#define MAX_ENT_TITLE 1

class Stage {
public:
	bool doorOpen2 = true;
	bool changeGlass = false; 
	int glassCount = 0;

	bool isAmulet = false;
	int id = 0;
	bool isRa = false;

	//GUI
	bool amuleto = false;
	bool grail = false;
	bool cruz = false;
	bool arrow = false;


	bool animation = false;
	float Timeanimation = 0.0f;
	bool firstTime = true;
	bool animation2 = true;

	//particle active mirror
	bool AnimationMirror = true;
	int NumParticle = 20;

	//stages 
	bool body = false;
	bool mind = false;
	bool soul = false;

	bool InitStage = false;
	
	virtual void createEntities() {};
	virtual void createTextures() {};
	virtual void render() {}; //empty body 
	virtual void update(double seconds_elapsed) {}; //empty body 

	void renderGui();
	void renderTorch(int i, vector<EntityMesh*> entities);
	void renderMirror(int i , vector<EntityMesh*> entities);
	void Stage::renderParticle(float timeParticle); 
};

class TitleStage : public Stage {
public:

	vector<EntityMesh*> ButtonsPlane;
	int numButton = 3; 
	EntityMesh* menu;
	Shader* shader; 
	virtual void createEntities();
	void createTextures(); 
	virtual void render();
	virtual void update(double seconds_elapsed);
	TitleStage();
};

class IntroStage : public Stage {
public:

	vector<EntityMesh*> entities;

	virtual void createTextures();
	virtual void createEntities();
	virtual void render();
	virtual void update(double seconds_elapsed);
	IntroStage();
};

class BodyStage : public Stage {
public:

	vector<EntityMesh*> entities;
	vector<EntityMesh*> entities_mirror;
	bool glass = false; 
	virtual void createTextures();
	virtual void createEntities();
	virtual void render();
	void renderWater(int i);
	virtual void update(double seconds_elapsed);
	BodyStage();
};

class MindStage : public Stage {
public:

	vector<EntityMesh*> entities;
	vector<EntityMesh*> entities_mirror;
	bool glass = false;
	int contObject = 0; 
	
	int timeSpot = 0; 
	virtual void createTextures();
	virtual void createEntities();
	virtual void render();

	virtual void update(double seconds_elapsed);
	void ChangePosLight(); 
	MindStage();
};


class SoulStage : public Stage {
public:

	vector<EntityMesh*> entities;
	vector<EntityMesh*> entities_mirror;
	bool glass = false;
	bool PuzzleCorrect = true;
	virtual void createTextures();
	virtual void createEntities();
	virtual void render();
	virtual void update(double seconds_elapsed);
	SoulStage();
};

class CorridorStage : public Stage {
public:

	vector<EntityMesh*> entities;
	vector<EntityMesh*> entities_mirror;
	bool glass = false;

	virtual void createTextures();
	virtual void createEntities();
	virtual void render();
	virtual void update(double seconds_elapsed);
	CorridorStage();
};

class EndStage : public Stage {
public:
	enum eButton {
		START,
		SAFE,
		LOAD,
		CONTROLS,
		CONFIGURATION,
		EXIT
	};
	eButton button_type;
	EntityMesh* menu;
	float time;
	virtual void createEntities();
	virtual void render();
	virtual void update(double seconds_elapsed);
	EndStage();
};



#endif 
