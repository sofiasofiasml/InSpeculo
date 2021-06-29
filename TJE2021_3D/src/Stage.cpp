#include "Stage.h"
#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "game.h"

void Stage::renderTorch(int i, vector<EntityMesh*> entities)
{

	EntityMesh* torch;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Camera* camera = Camera::current;
	torch = entities[i - 1];
	torch->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/torch.fs");
	torch->shader->enable();
	torch->shader->setUniform("u_model", torch->model);
	torch->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	torch->shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	torch->shader->setUniform("u_time", Game::instance->time);
	////render the 
	torch->mesh->render(GL_TRIANGLES);
	torch->shader->disable();
	glDisable(GL_BLEND);
}

void Stage::renderMirror(int i, vector<EntityMesh*> entities)
{
	Scene* scene = Game::instance->CurrentScene;
	Stage* stage = Game::instance->current_stage;
	Game* game = Game::instance;

	EntityMesh* mirror;
	if (stage == game->body_stage) {
		if (!game->body_stage->animation2)
			glDisable(GL_DEPTH_TEST);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Camera* camera = Camera::current;
	mirror = entities[i - 1];

	mirror->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/mirror.fs");
	mirror->shader->enable();
	mirror->shader->setUniform("u_model", mirror->model);
	mirror->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	mirror->shader->setUniform("u_color", Vector4(1, 1, 1, 1));

	mirror->shader->setUniform("u_texture_1", mirror->texture2, 5);
	mirror->shader->setUniform("u_texture", mirror->texture, 6);
	mirror->shader->setUniform("u_alpha", mirror->alpha);


	////render the 
	mirror->mesh->render(GL_TRIANGLES);
	mirror->shader->disable();
	glDisable(GL_BLEND);
	if (!animation2)
		glEnable(GL_DEPTH_TEST);
}

void Stage::renderParticle(float timeParticle)
{
	Scene* scene = Game::instance->CurrentScene;
	Camera* camera = Camera::current;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);


	//particle->texture = Texture::Get("data/imShader/grayMirror.tga"); 
	for (int i = 0; i < scene->mirrorParticle.size(); i++)
	{
		scene->mirrorParticle[i].v_particles->mesh->vertices.resize(6);
		scene->mirrorParticle[i].v_particles->mesh->uvs.resize(6);

		Vector3& pos = Vector3(0, -0.5 * pow(timeParticle, 2), -timeParticle);
		float sizeParticle = scene->mirrorParticle[i].sizeParticle;
		Vector3 camUp = camera->getLocalVector(Vector3(0, 1, 0)) * sizeParticle;
		Vector3 camRight = camera->getLocalVector(Vector3(1, 0, 0)) * sizeParticle;
		//encarar al ojo de la camara
		scene->mirrorParticle[i].v_particles->mesh->vertices[0] = Vector3(pos + camUp + camRight);
		scene->mirrorParticle[i].v_particles->mesh->uvs[0] = Vector2(1, 0);
		scene->mirrorParticle[i].v_particles->mesh->vertices[1] = Vector3(pos + camUp - camRight);
		scene->mirrorParticle[i].v_particles->mesh->uvs[1] = Vector2(0, 0);
		scene->mirrorParticle[i].v_particles->mesh->vertices[2] = Vector3(pos - camUp - camRight);
		scene->mirrorParticle[i].v_particles->mesh->uvs[2] = Vector2(0, 1);


		scene->mirrorParticle[i].v_particles->mesh->vertices[3] = Vector3(pos - camUp - camRight);
		scene->mirrorParticle[i].v_particles->mesh->uvs[3] = Vector2(0, 1);
		scene->mirrorParticle[i].v_particles->mesh->vertices[4] = Vector3(pos - camUp + camRight);
		scene->mirrorParticle[i].v_particles->mesh->uvs[4] = Vector2(1, 1);
		scene->mirrorParticle[i].v_particles->mesh->vertices[5] = Vector3(pos + camUp + camRight);
		scene->mirrorParticle[i].v_particles->mesh->uvs[5] = Vector2(1, 0);

		scene->mirrorParticle[i].v_particles->model.rotate(scene->mirrorParticle[i].rotateParticle, Vector3(0, 0, 1));
		scene->mirrorParticle[i].v_particles->render();
		scene->mirrorParticle[i].v_particles->model.rotate(-scene->mirrorParticle[i].rotateParticle, Vector3(0, 0, 1));
	}
	//Fin de encarar
	glDisable(GL_BLEND);
	glDepthMask(true);



}

void Stage::renderGui() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Game* game = Game::instance;

	Camera* cam2D = new Camera();
	cam2D->setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	Mesh quad;
	quad.createQuad(100, 100, 100, 100, true);

	Mesh inventario;
	inventario.createQuad(100, 500, 100, 100, true);
	cam2D->enable();

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	shader->enable();

	shader->setUniform("u_model", Matrix44());
	shader->setUniform("u_viewprojection", cam2D->viewprojection_matrix);
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_texture_tiling", 1.0f);
	if (glassCount == 0)
		shader->setUniform("u_texture", Texture::Get("data/gui/GUI0.tga"), 0);
	if (this->glassCount == 1)
		shader->setUniform("u_texture", Texture::Get("data/gui/GUI1.tga"), 0);
	if (this->glassCount == 2)
		shader->setUniform("u_texture", Texture::Get("data/gui/GUI2.tga"), 0);
	if (this->glassCount == 3)
		shader->setUniform("u_texture", Texture::Get("data/gui/GUI3.tga"), 0);
	quad.render(GL_TRIANGLES);
	shader->disable();
	//inventario
	Shader* shader2 = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	shader2->enable();

	shader2->setUniform("u_model", Matrix44());
	shader2->setUniform("u_viewprojection", cam2D->viewprojection_matrix);
	shader2->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader2->setUniform("u_texture_tiling", 1.0f);
	if (this->amuleto)
		shader2->setUniform("u_texture", Texture::Get("data/gui/amuleto.tga"), 1);
	else  if (this->cruz)
		shader2->setUniform("u_texture", Texture::Get("data/gui/cruz.tga"), 2);
	else if (this->grail)
		shader2->setUniform("u_texture", Texture::Get("data/gui/grail.tga"), 3);
	else if (this->arrow)
		shader2->setUniform("u_texture", Texture::Get("data/gui/ouijaArrow.tga"), 4);
	else
		shader2->setUniform("u_texture", Texture::Get("data/gui/void.tga"), 5);

	inventario.render(GL_TRIANGLES);
	shader2->disable();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void IntroStage::createTextures()
{
	Scene* scene = Game::instance->intro_scene;
	string texture = "data/intro/Door_BaseColor.tga,data/intro/cielo.tga,data/intro/ground.tga,data/intro/intro.tga";
	string cad;
	int found = -1;
	int init = 0;
	for (int i = 0; i < MAX_ENT_INTRO; i++)
	{
		if (i == 0 || i > 2) {
			init = found + 1;
			found = texture.find(",", found + 1);
			cad = texture.substr(init, found - init);
		}

		entities[i]->texture = Texture::Get(cad.c_str());
	}
}

// 0 der puerta ,1  iz puerta ,2 arco puerta ,3 cielo ,4 suelo ,5 iglesia
void IntroStage::createEntities()
{
	Game* game = Game::instance;
	Scene* scene = Game::instance->intro_scene;
	string mesh = "data/intro/RightDoor.ase,data/intro/LeftDoor.ase,data/intro/ArcDoor.ase,data/intro/cielo.ASE,data/intro/intro.ase";
	string cad;
	this->InitStage = true;
	game->audio->Play("data/audio/intro.wav", 0.1, false);

	int found = -1;
	int init = 0;
	this->entities.resize(MAX_ENT_INTRO);
	scene->entities.resize(MAX_ENT_INTRO + 1);
	for (int i = 0; i < MAX_ENT_INTRO; i++)
	{
		entities[i] =new EntityMesh();

		if (i == 4) {
			entities[i]->mesh->createPlane(2000);
			entities[i]->tiling = 40.0f;
			entities[i]->isColision = false;

		}
		else {
			init = found + 1;
			found = mesh.find(",", found + 1);
			cad = mesh.substr(init, found - init);
			entities[i]->mesh = Mesh::Get(cad.c_str());
		}
		entities[i]->id = i;
		scene->entities[i+1] =entities[i];

		if (i == 3) {
			entities[i]->isColision = false;
		}
		if (i == 2) {
			entities[i]->isColision = false;
			scene->entities[i + 1]->model.translate(0.0f, 0.0f, -15.5f);

		}
		if (i == 1)
			scene->entities[i + 1]->model.translate(0.0f, 0.0f, -31.0f);
		if (i == 5)
		{
			scene->entities[i + 1]->model.translate(11.0f, 0.0f, -12.0f);
			entities[i]->isColision = false;
		}
	}
	createTextures();
}

void IntroStage::render()
{

	Camera* camera = Game::instance->camera;
	Scene* scene = Game::instance->intro_scene;

	for (int i = 0; i < scene->entities.size(); i++)
	{
		scene->entities[i]->render();
	}

}

void IntroStage::update(double seconds_elapsed)
{
	Scene* scene = Game::instance->intro_scene;
	Game* game = Game::instance;
	for (int i = 0; i < scene->entities.size(); i++)
	{
		scene->entities[i]->update(seconds_elapsed);
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_B))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Stop("data/audio/intro.wav");

		game->current_stage = game->body_stage;
		game->CurrentScene = game->BodyScene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_M))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Stop("data/audio/intro.wav");

		game->current_stage = game->mind_stage;
		game->CurrentScene = game->mind_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_X))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Stop("data/audio/intro.wav");

		game->current_stage = game->soul_stage;
		game->CurrentScene = game->soul_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_C))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Stop("data/audio/intro.wav");

		game->current_stage = game->corridor_stage;
		game->CurrentScene = game->corridor_scene;
		game->current_stage->createEntities();
	}
	if (this->animation) //animacion de las puertas
	{
		float radLeftDoor = -90 * DEG2RAD * seconds_elapsed;
		float radRightDoor = 90 * DEG2RAD * seconds_elapsed;

		if (this->firstTime) {
			Timeanimation = game->time;
			this->firstTime = false;
		}
		if (game->time - Timeanimation < 1.2f) {
			scene->entities[2]->model.rotate(radLeftDoor, Vector3(0.0f, 1.0f, 0.0f));
			scene->entities[1]->model.rotate(radRightDoor, Vector3(0.0f, 1.0f, 0.0f));
		}

	}

}

IntroStage::IntroStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = true;
}

void BodyStage::createTextures()
{
	Scene* scene = Game::instance->CurrentScene;

	string texture = "data/imShader/water.tga,data/body/ceiling.tga,data/body/wall.tga,data/body/bath.tga,data/body/door.tga,data/body/sink.tga,data/body/sink.tga,data/body/cabinet.tga,data/body/cabinet.tga,data/body/passage.tga,data/body/passagePlane.tga,data/imShader/noise.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga";
	
	string cad;
	int found = -1;
	int init = 0;

	for (int i = 0; i < MAX_ENT_BODY; i++)
	{
		if (this->entities[i]->id == 1) {
			this->entities[i]->texture2 = Texture::Get("data/imShader/cloud.tga");
		}
		if (this->entities[i]->id == 12) {
			this->entities[i]->texture2 = Texture::Get("data/imShader/gray.tga");
		}
		if (this->entities[i]->id > 16 || this->entities[i]->id < 13) {
			init = found + 1;
			found = texture.find(",", found + 1);
			cad = texture.substr(init, found - init);
			this->entities[i]->texture = Texture::Get(cad.c_str());
			this->entities_mirror[i]->texture = Texture::Get(cad.c_str());
		}
		
	}
}

//0 water 1celling 2 wall 3 bath 4 door 5 sink 6 sink1 7 cabin 8 cabin 1 9 passage 10 plano 11 mirror
void BodyStage::createEntities()
{
	Scene* scene = Game::instance->CurrentScene;
	Game* game = Game::instance;
	game->audio->Play("data/audio/bath.wav", 0.1, false);

	string mesh = "data/body/ceiling.ASE,data/body/wall.ASE,data/body/bath.ASE,data/body/door.ASE,data/body/sink.ASE,data/body/sink1.ASE,data/body/cabinet.ASE,data/body/cabinet1.ASE,data/body/passage.ASE,data/glassBody.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE";

	string cad;

	int found = -1;
	int init = 0;
	int playerNum = scene->entities.size();
	this->changeGlass = false; 
	this->doorOpen2 = true;
	this->InitStage= true;
	this->entities.resize(MAX_ENT_BODY);
	scene->entities.resize(MAX_ENT_BODY + 1);
	this->entities_mirror.resize(MAX_ENT_BODY);
	scene->entities_mirror.resize(MAX_ENT_BODY + 1);
	for (int i = 0; i < MAX_ENT_BODY; i++)
	{
		this->entities[i] = new EntityMesh();
		this->entities_mirror[i] = new EntityMesh();

		this->entities[i]->id = i + playerNum;
		this->entities_mirror[i]->id = i + playerNum;

		if (this->entities[i]->id != 1 && this->entities[i]->id != 11 &&( this->entities[i]->id < 13 || this->entities[i]->id > 16)) {
			init = found + 1;
			found = mesh.find(",", found + 1);
			cad = mesh.substr(init, found - init);
			this->entities[i]->mesh = Mesh::Get(cad.c_str());
			this->entities_mirror[i]->mesh = Mesh::Get(cad.c_str());
		}
		if (this->entities[i]->id == 1) {
			this->entities[i]->isColision = false;

			this->entities[i]->mesh->createPlane(30);
			this->entities[i]->model.translate(0, 0, 5);
			this->entities[i]->model.scale(1.3, 1, 0.9);
			this->entities_mirror[i]->mesh->createPlane(30);
			this->entities_mirror[i]->alpha = 1;
		}


		if (this->entities[i]->id == 2) //techo
		{
			this->entities[i]->model.translate(5, 0, -25);
			this->entities_mirror[i]->model.translate(5, 0, -25);
			this->entities[i]->isColision = false;

		}
		if (this->entities[i]->id == 3) //wall
		{
			this->entities[i]->isColision = false;
		}
		if (this->entities[i]->id == 4) //bath
		{
			this->entities[i]->model.translate(19, 0, 19);
			this->entities_mirror[i]->model.translate(19, 0, 19);
			this->entities[i]->isInteractive = true;

		}
		if (this->entities[i]->id == 5) //door
		{
			this->entities[i]->model.translate(2, 0, -25);
			this->entities_mirror[i]->model.translate(2, 0, -25);
		}
		if (this->entities[i]->id == 6) //sink1
		{
			this->entities[i]->model.translate(28, 0, 0);
			this->entities_mirror[i]->model.translate(28, 0, 0);
		}
		if (this->entities[i]->id == 7) //sink
		{
			this->entities[i]->model.translate(-15, 0, -15);
			this->entities_mirror[i]->model.translate(-15, 0, -15);
		}
		if (this->entities[i]->id == 8) //cabinet
		{
			this->entities[i]->model.translate(-10, 0, 19);
			this->entities[i]->model.rotate(-45 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.translate(-10, 0, 19);
			this->entities_mirror[i]->model.rotate(-45 * DEG2RAD, Vector3(0, 1, 0));


		}
		if (this->entities[i]->id == 9) //cabinet 1
		{
			this->entities[i]->model.translate(0, 0, -15);
			this->entities_mirror[i]->model.translate(0, 0, -15);
		}
		if (this->entities[i]->id < 8 && this->entities[i]->id > 0) {
			this->entities_mirror[i]->model = this->entities_mirror[i]->model.relfexion_x();
		}
		if (this->entities[i]->id > 7 && this->entities[i]->id < 12) {
			this->entities[i]->alpha = 1;
			this->entities_mirror[i]->model = this->entities_mirror[i]->model.relfexion_x();
		}
		if (this->entities[i]->id == 10) {
			this->entities_mirror[i]->alpha = 1;
			this->entities[i]->alpha = 0;
			this->entities[i]->isColision = false;
			this->entities[i]->model.translate(9, 0, -55);
			this->entities_mirror[i]->model.translate(9, 0, -55);

		}
		if (this->entities[i]->id == 11) {
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.setRotation(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			this->entities[i]->model.setRotation(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities_mirror[i]->alpha = 1;
			this->entities[i]->alpha = 0;
			this->entities[i]->model.translate(5, 80, 20);
			this->entities_mirror[i]->model.translate(5, 80, 20);
		}
		if (this->entities[i]->id == 12) { //trozo de espejo
			this->entities[i]->model.translate(-30, 0, 20);
			this->entities[i]->model.scale(0.2f, 0.2f, 0.2f);
			this->entities[i]->model.rotate(-45, Vector3(0, 1, 0));
			this->entities[i]->isInteractive = true;
		}
		if (this->entities[i]->id == 13) { //plano para antorchas
			this->entities[i]->mesh->createPlane(20);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			this->entities[i]->model.translate(-50, 19, 30);
			this->entities[i]->model.scale(0.5f, 0.5f, 0.5f);

		}
		if (this->entities[i]->id == 14) { //plano para antorchas
			this->entities[i]->mesh->createPlane(20);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			this->entities[i]->model.translate(-35, 19, 30);
			this->entities[i]->model.scale(0.5f, 0.5f, 0.5f);

		}
		if (this->entities[i]->id == 15) { //plano para antorchas
			this->entities[i]->mesh->createPlane(20);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			this->entities[i]->model.translate(-50, 0, 30);
			this->entities[i]->model.scale(0.5f, 0.5f, 0.5f);

		}
		if (this->entities[i]->id == 16) { //plano para antorchas
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			this->entities[i]->model.translate(-35, 0, 30);
			this->entities[i]->model.scale(0.5f, 0.5f, 0.5f);
		}

		if (this->entities[i]->id == 17) { //plano para antorchas
			this->entities[i]->model.translate(-2, 10, -35);
		}
		if (this->entities[i]->id == 20) { //plano para antorchas
			this->entities[i]->model.translate(-2, 10, -50);
		}

		if (this->entities[i]->id == 18) { //plano para antorchas
			this->entities[i]->model.rotate(180 * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
			this->entities[i]->model.translate(-20, 10, 35);

		}
		if (this->entities[i]->id == 19) { //plano para antorchas
			this->entities[i]->model.rotate(180 * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
			this->entities[i]->model.translate(-20, 10, 50);
		}
		
		scene->entities[i+1] =this->entities[i];
		scene->entities_mirror[i+1]=this->entities_mirror[i];

	}
	createTextures();
}

void BodyStage::renderWater(int i)
{

	EntityMesh* water;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Camera* camera = Camera::current;
	water = this->entities[i - 1];
	water->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/water.fs");
	water->shader->enable();
	water->shader->setUniform("u_model", water->model);
	water->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	water->shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	water->shader->setUniform("u_time", Game::instance->time);

	water->shader->setUniform("u_texture2", water->texture, 1);
	water->shader->setUniform("u_texture", water->texture2, 0);
	water->shader->setUniform("u_texture_tiling", 0.2f);

	////render the 
	water->mesh->render(GL_TRIANGLES);
	water->shader->disable();
	glDisable(GL_BLEND);
}

void BodyStage::render()
{

	Camera* camera = Camera::current;
	Scene* scene = Game::instance->CurrentScene;
	for (int i = 0; i < scene->entities_mirror.size(); i++)
	{
		if (i!= 0 && scene->entities_mirror[i]->id != 1 && scene->entities_mirror[i]->id != 11 && scene->entities_mirror[i]->id != 12 && scene->entities_mirror[i]->id < 13)  //2 es el suelo water, no lo renderizamos en la realidad mirror
			scene->entities_mirror[i]->render();

	}
	for (int i = 0; i < scene->entities.size(); i++)
	{
		if (scene->entities[i]->id == 1) {
			renderWater(i);
		}
		if (scene->entities[i]->id > 12 && scene->entities[i]->id < 17) {
			renderTorch(i, this->entities);
		}
		if (scene->entities[i]->id == 12) {
			renderMirror(i,this->entities);
		}

		if (scene->entities[i]->id != 1 && scene->entities[i]->id != 12 && (scene->entities[i]->id < 13 || scene->entities[i]->id>16))
			scene->entities[i]->render();
	}
	renderGui();
}

void BodyStage::update(double seconds_elapsed)
{

	Scene* scene = Game::instance->CurrentScene;
	Game* game = Game::instance;

	for (int i = 0; i < scene->entities.size(); i++)
	{
		scene->entities[i]->update(seconds_elapsed);
	}
	for (int i = 1; i < scene->entities_mirror.size(); i++)
	{
		scene->entities_mirror[i]->update(seconds_elapsed);
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_M))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->mind_stage;
		game->CurrentScene = game->mind_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_X))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->soul_stage;
		game->CurrentScene = game->soul_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_C))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->corridor_stage;
		game->CurrentScene = game->corridor_scene;
		game->current_stage->createEntities();
	}
	
	if (this->animation)
	{
		float radDoor = -90 * DEG2RAD * seconds_elapsed;

		if (this->firstTime) {
			Timeanimation = game->time;
			this->firstTime = false;
		}
		if (game->time - Timeanimation < 1.2f && game->current_stage->animation2) {

			game->CurrentScene->entities[5]->isColision = false;
			scene->entities[5]->model.rotate(radDoor, Vector3(0.0f, 1.0f, 0.0f));
			scene->entities_mirror[5]->model.rotate(radDoor, Vector3(0.0f, 1.0f, 0.0f));

		}
		if (game->time - Timeanimation < 1.2f && !game->current_stage->animation2) {

			game->CurrentScene->entities[5]->isColision = true;
			scene->entities[5]->model.rotate(-radDoor, Vector3(0.0f, 1.0f, 0.0f));
			scene->entities_mirror[5]->model.rotate(-radDoor, Vector3(0.0f, 1.0f, 0.0f));
		}

	}
	if (this->changeGlass && this->doorOpen2) {

		this->doorOpen2 = false;
		game->current_stage->animation2 = true;
		this->animation = true;
		this->firstTime = true;
	}
}

BodyStage::BodyStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = true;
}

void MindStage::createTextures()
{
	Scene* scene = Game::instance->CurrentScene;
	string texture = "data/mind/sala.tga,data/mind/suelo.tga,data/mind/suelo.tga,data/mind/trinidad.tga,data/mind/grail.tga,data/mind/cruz.tga,data/mind/cuadro1.tga,data/mind/cuadro2.tga,data/mind/amuleto.tga,data/imShader/noise.tga,data/mirror.tga,data/mind/puerta.tga,data/imShader/noise.tga,data/mind/altar.tga,data/body/passage.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga,data/body/passagePlane.tga";

	string cad;
	int found = -1;
	int init = 0;

	for (int i = 0; i < MAX_ENT_MIND; i++)
	{
		if (this->entities[i]->id == 10 || this->entities[i]->id == 13) { //para espejos
			this->entities[i]->texture2 = Texture::Get("data/imShader/gray.tga");
		}
		init = found + 1;
		found = texture.find(",", found + 1);
		cad = texture.substr(init, found - init);
		this->entities[i]->texture = Texture::Get(cad.c_str());
		this->entities_mirror[i]->texture = Texture::Get(cad.c_str());
		
	}
}

void MindStage::createEntities() {
	Scene* scene = Game::instance->CurrentScene;
	Game* game = Game::instance;
	game->audio->Play("data/audio/mind.wav", 0.1, false);
	string mesh = "data/mind/sala.ASE,data/mind/trinidad.ASE,data/mind/grail.ASE,data/mind/cruz.ASE,data/mind/cuadro1.ASE,data/mind/cuadro2.ASE,data/mind/amuleto.ASE,data/glassMind.ASE,data/mirror.ASE,data/mind/puerta.ASE,data/mind/altar.ASE,data/body/passage.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE";
	this->changeGlass = false;

	string cad;
	this->InitStage = true;
	this->doorOpen2 = true;
	int found = -1;
	int init = 0;
	int playerNum = scene->entities.size();
	this->entities.resize(MAX_ENT_MIND);
	scene->entities.resize(MAX_ENT_MIND + 1);
	this->entities_mirror.resize(MAX_ENT_MIND);
	scene->entities_mirror.resize(MAX_ENT_MIND + 1);
	for (int i = 0; i < MAX_ENT_MIND; i++)
	{
		this->entities[i]=new EntityMesh();
		this->entities_mirror[i]=new EntityMesh();

		this->entities[i]->id = i + playerNum;
		this->entities_mirror[i]->id = i + playerNum;

		if (this->entities[i]->id != 2 && this->entities[i]->id != 3 && this->entities[i]->id != 13 && this->entities[i]->id < 20)
		{
			init = found + 1;
			found = mesh.find(",", found + 1);
			cad = mesh.substr(init, found - init);
			this->entities[i]->mesh = Mesh::Get(cad.c_str());
			this->entities_mirror[i]->mesh = Mesh::Get(cad.c_str());

		}
		if (this->entities[i]->id == 1) //SALA
		{
			this->entities[i]->isColision = false;
			this->entities[i]->model.translate(0, 10, 0);
			this->entities_mirror[i]->model.translate(0, 10, 0);
		}
		if (this->entities[i]->id == 2) // suelo
		{
			this->entities[i]->isColision = false; 
			this->entities[i]->mesh->createPlane(100);
			this->entities_mirror[i]->mesh->createPlane(100);
			this->entities_mirror[i]->model.translate(-30, 0, 0);
		}
		if (this->entities[i]->id == 3) //techo
		{
			this->entities[i]->mesh->createPlane(100);
			this->entities[i]->model.translate(0, 35, 0);
			this->entities_mirror[i]->mesh->createPlane(100);
			this->entities_mirror[i]->model.translate(0,35,0);
		}
		if (this->entities[i]->id == 4)//cuadro trinidad
		{
			this->entities[i]->model.translate(42, 18, -30);
			this->entities_mirror[i]->model.translate(-42, 18, -30);
		}
		if (this->entities[i]->id == 5) //amuleto grail
		{
			this->entities[i]->alpha = 1;
			this->entities[i]->model.scale(0.3, 0.3, 0.3);
			this->entities[i]->model.translate(-200, 10, 0);
			this->entities_mirror[i]->model.scale(0.3, 0.3, 0.3);
			this->entities_mirror[i]->model.translate(-200, 10, 0);
		}
		if (this->entities[i]->id == 6) //amuelto cruz 
		{
			this->entities[i]->alpha = 1;
			this->entities[i]->model.translate(-60, 0, 20);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.translate(-110, 0, 190);
			this->entities_mirror[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
		}
		if (this->entities[i]->id == 7) //cuadro1 
		{
			this->entities[i]->model.translate(42, 12, 20);
			this->entities_mirror[i]->model.translate(-42, 12, 20);
		}
		if (this->entities[i]->id == 8) //cuadro2 
		{
			this->entities[i]->model.translate(42, 8, 70);
			this->entities[i]->model.scale(0.8,0.8,0.8);
			this->entities_mirror[i]->model.translate(-77, 8, 70);
			this->entities_mirror[i]->model.scale(0.8, 0.8, 0.8);
		}
		if (this->entities[i]->id == 9) //amuleto ra (bueno)  
		{
			this->entities[i]->alpha = 1;
			this->entities[i]->model.translate(-60, 0, 50);
			this->entities[i]->model.scale(0.4, 0.4, 0.4);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.translate(-110, 0, 120);
			this->entities_mirror[i]->model.scale(0.4, 0.4, 0.4);
			this->entities_mirror[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
		}
		if (this->entities[i]->id == 10) //trozo espejo
		{
			this->entities[i]->alpha = 1;
			this->entities[i]->isColision = false;
			this->entities_mirror[i]->alpha = 1;
			this->entities[i]->model.translate(0, 4, -10);
			this->entities[i]->model.scale(0.2, 0.2, 0.2);
			this->entities[i]->model.rotate(-90*DEG2RAD , Vector3(1, 0, 0));
		}
		if (this->entities[i]->id == 11) //espejo marco
		{
			this->entities[i]->model.translate(-80,15.5f, -24);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 0, 1));
			this->entities[i]->model.scale(1, 2.8, 1);
		}
		if (this->entities[i]->id == 12) //puerta
		{
			this->entities[i]->model.translate(-20, 0, -50);
			this->entities_mirror[i]->model.translate(19.5, 0, -50);
		}
		if (this->entities[i]->id == 13) //mirror espejo 
		{
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.translate(-84, 15.5f, 20);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 0, 1));
			this->entities[i]->model.scale(0.5, 1, 2);
		}
		if (this->entities[i]->id == 14) //altar
		{
			this->entities[i]->model.translate(-15, 0, 70);
			this->entities_mirror[i]->model.translate(-50, 0, 70);
			this->entities_mirror[i]->model.scale(0.5, 0.5, 0.5);
			this->entities[i]->model.scale(0.5, 0.5, 0.5);
		}
		if (this->entities[i]->id == 15) //pasillo 
		{
			this->entities[i]->isColision = false;
			this->entities[i]->model.translate(-15, 0, -85);
			this->entities[i]->model.scale(1.5, 1, 1);
		}
		if (this->entities[i]->id == 16) //antorchas prefab 
		{
			this->entities[i]->model.translate(2, 10, -104);
			this->entities[i]->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
		}
		if (this->entities[i]->id == 17) //antorchas prefab 
			this->entities[i]->model.translate(-35, 10, -108);

		if (this->entities[i]->id == 18) //antorchas prefab 
			this->entities[i]->model.translate(-35, 10, -93);

		if (this->entities[i]->id == 19) //antorchas prefab 
		{
			this->entities[i]->model.translate(2, 10, -93);
			this->entities[i]->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
		}
		if (this->entities[i]->id == 20) //plano profundidad 
		{
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.translate(-15, 120, 20);
		}
		if (this->entities[i]->id == 21) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.translate(0, 93, 30);
			
		}
		if (this->entities[i]->id == 22) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.translate(-33, 93, 30);
		}
		if (this->entities[i]->id == 23) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.translate(-33, 108, 30);
		}
		if (this->entities[i]->id == 24) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.translate(0, 104, 30);
		}
		//reflexion
		this->entities_mirror[i]->model = this->entities_mirror[i]->model.relfexion_y();
		this->entities_mirror[i]->model.translate(170,0,0);
		scene->entities[i+1]=this->entities[i];
		scene->entities_mirror[i+1]=this->entities_mirror[i];
	}

	createTextures();
}

void MindStage::render()
{
	Camera* camera = Camera::current;
	Scene* scene = Game::instance->mind_scene;
	for (int i = 1; i < scene->entities_mirror.size(); i++) 
	{
		//no renerizamos el epejo/ el marco ni el trozo de espejo en el mirror 
		if (scene->entities_mirror[i]->id != 10 && scene->entities_mirror[i]->id != 11 && scene->entities_mirror[i]->id != 13 && scene->entities_mirror[i]->id < 15)
			scene->entities_mirror[i]->render();
	}
	for (int i = 0; i < scene->entities.size(); i++)
	{
		if (scene->entities[i]->id == 10 || scene->entities[i]->id == 13)
			renderMirror(i, this->entities);

		if (scene->entities[i]->id > 20)
			renderTorch(i, this->entities);

		if (scene->entities[i]->id != 10 && scene->entities[i]->id != 13 && scene->entities[i]->id < 21)
			scene->entities[i]->render();
		
	}
	renderGui();
}

void MindStage::update(double seconds_elapsed)
{
	Camera* camera = Camera::current;
	Scene* scene = Game::instance->mind_scene;
	Game* game = Game::instance; 
	for (int i = 0; i < scene->entities.size(); i++)
	{
		scene->entities[i]->update(seconds_elapsed);
	}
	
	for (int i = 1; i < scene->entities_mirror.size(); i++)
	{
		scene->entities_mirror[i]->update(seconds_elapsed);
	}
	if (this->animation) //animacion de la puerta
	{
		float x = 10 * seconds_elapsed;

		if (this->firstTime) {
			Timeanimation = game->time;
			this->firstTime = false;
		}
		if (game->time - Timeanimation < 1.2f && game->current_stage->animation2) {

			game->CurrentScene->entities[12]->isColision = false;			
			scene->entities[12]->model.translate(x, 0.0f, 0.0f);
		}
		if (game->time - Timeanimation < 1.2f && !game->current_stage->animation2) {
			game->CurrentScene->entities[12]->isColision = true;
			scene->entities[12]->model.translate(-x, 0.0f, 0.0f);
		}

	}
	if (this->changeGlass && this->doorOpen2) {
		this->doorOpen2 = false;
		game->current_stage->animation2 = true;
		this->animation = true;
		this->firstTime = true;
	}
	ChangePosLight(); //cambiar la posicion de la luz
	if (Input::wasKeyPressed(SDL_SCANCODE_B))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Play("data/audio/bath.wav", 0.5, false);
		game->current_stage = game->body_stage;
		game->CurrentScene = game->BodyScene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_X))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->soul_stage;
		game->CurrentScene = game->soul_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_C))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->corridor_stage;
		game->CurrentScene = game->corridor_scene;
		game->current_stage->createEntities();
	}
}

void MindStage::ChangePosLight()
{
	Scene* scene = Game::instance->mind_scene;
	Game* game = Game::instance; 
	 
	if (!isRa) {
		if (((int)game->time % 3) == 0 && timeSpot != (int)game->time) {
			
			this->contObject += 1; 
			if (this->contObject == 3)
				this->contObject = 0; 
			game->audio->Play("data/audio/light_sound.wav", 0.1f, true);
			scene->lights[0]->light_position = Vector3(-80, 25, contObject*25); //spot reality world
			scene->lights[1]->light_position = Vector3(-125, 25, contObject * 25); //spot reality world

			timeSpot = (int)game->time;
		}
	}
	else //cuando has encontrado el amuleto correcto 
	{
		scene->lights[0]->light_position = Vector3(-18, 25,0); //spot reality world
		scene->lights[1]->intensity = 0; //spot mirror world
		scene->entities[10]->alpha = 0; //trozo de espejo se hace visible
		scene->entities_mirror[10]->alpha = 0;
		scene->entities_mirror[10]->isColision = true;
	}

}

MindStage::MindStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = true;
}

void TitleStage::createTextures()
{
	string texture = "data/Button/play0.tga,data/Button/controls0.tga,data/Button/quit0.tga,data/Button/play1.tga,data/Button/controls1.tga,data/Button/quit1.tga,data/Button/controls2.tga";

	string cad;
	int found = -1;
	int init = 0;

	for (int i = 0; i < ButtonsPlane.size(); i++)
	{
		init = found + 1;
		found = texture.find(",", found + 1);
		cad = texture.substr(init, found - init);
		this->ButtonsPlane[i]->texture = Texture::Get(cad.c_str());

	}
}

void TitleStage::createEntities()
{
	Game* game = Game::instance;
	game->audio->Play("data/audio/title.wav", 0.5, false);

	menu = new EntityMesh();
	menu->mesh->createPlane(100);
	menu->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
	menu->texture = Texture::Get("data/inspeculo.tga");
	this->ButtonsPlane.resize(numButton * 2 +1);
	for (int i = 0; i< ButtonsPlane.size(); i++)
	{
		this->ButtonsPlane[i] = new EntityMesh; 
		this->ButtonsPlane[i]->id = i;

		if (this->ButtonsPlane[i]->id == 6)
			this->ButtonsPlane[i]->mesh->createPlane(40);
		else
			this->ButtonsPlane[i]->mesh->createPlane(9);
		this->ButtonsPlane[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
		if(this->ButtonsPlane[i]->id ==0 || this->ButtonsPlane[i]->id == 3)
			this->ButtonsPlane[i]->model.translate(-70, 0, 0);
		if (this->ButtonsPlane[i]->id == 1 || this->ButtonsPlane[i]->id == 4)
			this->ButtonsPlane[i]->model.translate(-70, 0, -20);
		if (this->ButtonsPlane[i]->id == 2 || this->ButtonsPlane[i]->id == 5)
			this->ButtonsPlane[i]->model.translate(-70,0, -40);
		if(this->ButtonsPlane[i]->id == 6)
			this->ButtonsPlane[i]->model.translate(20, 0, 0);
		
		this->ButtonsPlane[i]->model.scale(1.5, 1, 1); 
	}
	createTextures(); 
}

void TitleStage::render()
{
	Camera* camera = new Camera();
	Scene* scene = Game::instance->CurrentScene; 
	Vector2 v2_mouse = Input::mouse_position;
	Game* game = Game::instance; 
	Stage* stage = Game::instance->current_stage; 
	int var = 0; 
	bool ChangeIntro = false; 
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	this->shader->enable();
	this->shader->setUniform("u_model", menu->model);
	this->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	this->shader->setUniform("u_color", Vector4(1, 1, 1, 1));

	this->shader->setUniform("u_texture", menu->texture, 0);
	this->shader->setUniform("u_texture_tiling", 1.0f);
	menu->mesh->render(GL_TRIANGLES);
	for (int i = 0; i < numButton; i++)
	{
		var = i; 
		if ( v2_mouse.x>65 && v2_mouse.x<171)
		{
			if (i == 0 && v2_mouse.y > 271 && v2_mouse.y < 323) {
				var += 3;
				if (Input::mouse_state == 1) {// cambiar a intro (PLAY)
					ButtonsPlane.clear();
					game->current_stage = game->intro_stage;
					game->audio->Stop("data/audio/title.wav");
					game->CurrentScene = game->intro_scene;
					game->audio->Play("data/audio/intro.wav", 0.5, false);
					game->CurrentScene->CreatePlayer();
					game->current_stage->createEntities();
					ChangeIntro = true; 
					game->free_camera = false;
					
				}
			}
			if (i == 1 && v2_mouse.y > 332 && v2_mouse.y < 378) { //CONTROLS
				var += 3;
				//Cargar imagen de controles
				this->shader->setUniform("u_model", this->ButtonsPlane[6]->model);
				this->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
				this->shader->setUniform("u_color", Vector4(1, 1, 1, 1));

				this->shader->setUniform("u_texture", this->ButtonsPlane[6]->texture, 0);
				this->shader->setUniform("u_texture_tiling", 1.0f);
				this->ButtonsPlane[6]->mesh->render(GL_TRIANGLES);
			}
			if (i == 2 && v2_mouse.y > 392 && v2_mouse.y < 441) {//Exit
				var += 3;
				if (Input::mouse_state == 1) //salir del juego
					game->must_exit = true;
			}
			
		}
		if (!ChangeIntro) {
			this->shader->setUniform("u_model", this->ButtonsPlane[var]->model);
			this->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
			this->shader->setUniform("u_color", Vector4(1, 1, 1, 1));

			this->shader->setUniform("u_texture", this->ButtonsPlane[var]->texture, 0);
			this->shader->setUniform("u_texture_tiling", 1.0f);
			this->ButtonsPlane[var]->mesh->render(GL_TRIANGLES);
		}
	}
	////render the 
	this->shader->disable();
	glDisable(GL_BLEND);

}

void TitleStage::update(double seconds_elapsed)
{
	Game* game = Game::instance;

	if (Input::wasKeyPressed(SDL_SCANCODE_I)) {
		game->audio->Play("data/audio/intro.wav", 0.5, false);
		game->current_stage = game->intro_stage;
		game->current_stage->createEntities();
	}

}

TitleStage::TitleStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = true;
}

void SoulStage::createTextures()
{

	Scene* scene = Game::instance->soul_scene;

	string texture = "data/soul/ouija_mirror.tga,data/soul/OuijaArrow.tga,data/soul/Altar_9_mirror.tga,data/soul/Altar_C_mirror.tga,data/soul/Altar_M.tga,data/soul/Floor.tga,data/soul/wall.tga,data/soul/pilar.tga,data/soul/window.tga,data/soul/Floor.tga,data/soul/OuijaArrow.tga,data/imShader/noise.tga,data/soul/wall.tga,data/soul/door.tga,data/imShader/noise.tga,data/body/passage.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga,data/body/passagePlane.tga";

	string cad;
	int found = -1;
	int init = 0;

	for (int i = 0; i < MAX_ENT_SOUL; i++)
	{
		init = found + 1;
		found = texture.find(",", found + 1);
		cad = texture.substr(init, found - init);
		this->entities[i]->texture = Texture::Get(cad.c_str());
	
		if (this->entities_mirror[i]->id == 1 ) {
			this->entities_mirror[i]->texture = Texture::Get("data/soul/ouija.tga");
		}
		else if (this->entities_mirror[i]->id == 3) {
			this->entities_mirror[i]->texture = Texture::Get("data/soul/Altar_9.tga");
		}
		else if (this->entities_mirror[i]->id == 4) {
			this->entities_mirror[i]->texture = Texture::Get("data/soul/Altar_C.tga"); 
		}
		else
			this->entities_mirror[i]->texture = Texture::Get(cad.c_str());
		
		if (this->entities[i]->id == 12 || this->entities[i]->id == 15) {
			this->entities[i]->texture2 = Texture::Get("data/imShader/gray.tga");
		}

	}
	for (int j = 0; j < scene->mirrorParticle.size(); j++) {
		scene->mirrorParticle[j].v_particles->texture = Texture::Get("data/imShader/grayMirror.tga");
	}
}

void SoulStage::createEntities()
{
	Scene* scene = Game::instance->soul_scene;
	Game* game = Game::instance;
	string mesh = "data/soul/Ouija.ASE,data/soul/OuijaArrow.ASE,data/mind/altar.ASE,data/mind/altar.ASE,data/mind/altar.ASE,data/soul/floor.ASE,data/soul/wall.ASE,data/soul/pilar.ASE,data/soul/window.ASE,data/soul/floor.ASE,data/soul/mirror.ASE,data/soul/wallMirror.ASE,data/soul/door.ASE,data/glassSpirit.ASE,data/body/passage.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE";
	game->audio->Play("data/audio/soul.wav", 0.1, false);

	this->changeGlass = false;
	this->PuzzleCorrect = true; 
	scene->mirrorParticle.clear();
	scene->mirrorParticle.resize(NumParticle);
	string cad;
	this->InitStage = true;
	this->doorOpen2 = true;
	int found = -1;
	int init = 0;
	int playerNum = scene->entities.size();
	this->entities.resize(MAX_ENT_SOUL);
	scene->entities.resize(MAX_ENT_SOUL + 1);
	this->entities_mirror.resize(MAX_ENT_SOUL);
	scene->entities_mirror.resize(MAX_ENT_SOUL + 1);
	for (int i = 0; i < MAX_ENT_SOUL; i++)
	{
		this->entities[i]=new EntityMesh();
		this->entities_mirror[i]=new EntityMesh();

		this->entities[i]->id = i + playerNum;
		this->entities_mirror[i]->id = i + playerNum;

		if (this->entities[i]->id != 12 && this->entities[i]->id <21) {

			init = found + 1;
			found = mesh.find(",", found + 1);
			cad = mesh.substr(init, found - init);
			this->entities[i]->mesh = Mesh::Get(cad.c_str());
			this->entities_mirror[i]->mesh = Mesh::Get(cad.c_str());
		}
		if (this->entities_mirror[i]->id == 1) { //ouija
			this->entities[i]->model.rotate(180*DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(0, 25, 50);
			this->entities[i]->model.scale(0.7, 0.7, 0.7);

			this->entities_mirror[i]->model.translate(0,25, 100);
			this->entities_mirror[i]->model.scale(0.7, 0.7, 0.7);
		}

		if (this->entities_mirror[i]->id == 2) { //arrow ouija
			this->entities[i]->model.scale(0.2 ,0.2, 0.2);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1, 0, 0));
			this->entities[i]->model.translate(50, -100, 10);
			this->entities[i]->model.rotate(35 * DEG2RAD, Vector3(0, 0, 1));
		}
		if (this->entities_mirror[i]->id == 3) { //altar 9
			this->entities[i]->model.translate(-35, 0, 20);
			this->entities_mirror[i]->model.translate(-35, 0, 10);


			this->entities[i]->model.scale(0.5, 0.5, 0.5);
			this->entities_mirror[i]->model.scale(0.5, 0.5, 0.5);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.translate(-200, 0, -170);

		}
		if (this->entities_mirror[i]->id == 4) { //altar c
			this->entities[i]->model.translate(0, 0, 20);
			this->entities_mirror[i]->model.translate(0, 0, 10);
			this->entities[i]->model.scale(0.5, 0.5, 0.5);
			this->entities_mirror[i]->model.scale(0.5, 0.5, 0.5);


			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.translate(-200, 0, -170);
		}
		if (this->entities_mirror[i]->id == 5) { //altar m
			this->entities[i]->model.translate(35, 0, 20);
			this->entities_mirror[i]->model.translate(35, 0, 10);
			this->entities[i]->model.scale(0.5, 0.5, 0.5);
			this->entities_mirror[i]->model.scale(0.5, 0.5, 0.5);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
			this->entities_mirror[i]->model.translate(-200, 0, -160);
		}
		if (this->entities_mirror[i]->id == 6)  //floor
			this->entities[i]->isColision = false;

		if (this->entities_mirror[i]->id == 10) { //techo
			this->entities[i]->model.translate(0, 50, 0);
			this->entities_mirror[i]->model.translate(0, 50, 0);

		}
		if (this->entities[i]->id > 6 && this->entities[i]->id <10) {
			this->entities[i]->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(0, 0, -60);
			this->entities[i]->isColision = false;
		}
		if (this->entities_mirror[i]->id == 11) { //mirror
			this->entities[i]->model.translate(0, 0, 75);

		}
		if (this->entities_mirror[i]->id == 12) { //mirror
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1, 0, 0));
			this->entities[i]->model.translate(0, -75, 20);
			this->entities[i]->model.scale(1.2, 1.0, 0.8);
		}
		
		if (this->entities_mirror[i]->id == 13) { //pared
			this->entities[i]->model.translate(0, -10, 80);

		}
		if (this->entities_mirror[i]->id == 14) { //door
			this->entities[i]->model.translate(-64, 0, -39);

		}
		if (this->entities_mirror[i]->id == 15) { //trozo cristal
			this->entities[i]->model.translate(0, 0, 100);
			this->entities[i]->model.scale(0.2, 0.2, 0.2);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 0, 1));
			this->entities[i]->alpha = 1;

		}
		//passage
		if (this->entities[i]->id == 16) 
		{
			this->entities[i]->model.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(28, 0, -105); 
			this->entities[i]->model.scale(1.4, 1.4, 1.4); 
			this->entities[i]->isColision = false; 

		}
		//torch
		if (this->entities[i]->id == 17)
		{
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(-45, 10, 95);


		}
		if (this->entities[i]->id == 18)
		{
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(-45, 10, 125);


		}
		if (this->entities[i]->id == 19)
		{
			this->entities[i]->model.rotate(-90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(10, 10, -95);

		}
		if (this->entities[i]->id == 20)
		{
			this->entities[i]->model.rotate(-90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(10, 10, -125);

		}
		if (this->entities[i]->id == 21) //plano profundidad 
		{
			this->entities[i]->mesh->createPlane(35);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
			this->entities[i]->model.translate(-30, -150, 20);
		}



		if (this->entities[i]->id == 22) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));

			this->entities[i]->model.translate(-12, -95, 30);

		}
		if (this->entities[i]->id == 23) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));

			this->entities[i]->model.translate(-42, -95, 30);
		}
		if (this->entities[i]->id == 24) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));

			this->entities[i]->model.translate(-42, -125, 30);
		}
		if (this->entities[i]->id == 25) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));

			this->entities[i]->model.translate(-12, -125, 30);
		}
		this->entities_mirror[i]->model.translate(0, 0, 170);
		scene->entities[i+1]=this->entities[i];
		scene->entities_mirror[i+1]=this->entities_mirror[i];
	}
	for (int j = 0; j < scene->mirrorParticle.size(); j++) {
		scene->mirrorParticle[j].v_particles = new EntityMesh();
		scene->mirrorParticle[j].v_particles->model.translate(random() * 40 - 20, random() * 30 + 5, random() * 2 + 80);
		scene->mirrorParticle[j].sizeParticle = rand() % 25;
		scene->mirrorParticle[j].rotateParticle = rand() % 90;
	}
	createTextures();
}

void SoulStage::render()
{
	Scene* scene = Game::instance->soul_scene;
	Game* game = Game::instance;
	int timeAnimation = 3;
	for (int i = 1; i < scene->entities_mirror.size(); i++)
	{
		if (scene->entities_mirror[i]->id != 2 && scene->entities_mirror[i]->id != 11 && scene->entities_mirror[i]->id < 12)
			scene->entities_mirror[i]->render();
	}
	for (int i = 0; i < scene->entities.size(); i++)
	{
		if (scene->entities[i]->id == 12 || scene->entities[i]->id == 15)
			renderMirror(i, this->entities);
		if (scene->entities[i]->id > 21)
			renderTorch(i, this->entities);
		if (scene->entities[i]->id != 12 && scene->entities[i]->id != 15 && scene->entities[i]->id < 22) {
			scene->entities[i]->render();
		}
		if (scene->entities[15]->alpha == 0)
		{
			if (scene->entities[i]->id == 12)// espejo
			{
				scene->entities[i]->isColision = false;
				scene->entities[i]->alpha = 1;
				scene->entities[i]->model.translate(0, -50, 0);


			}
			if (PuzzleCorrect == true) {
				game->audio->Play("data/audio/broken_glass.wav", 0.5, true);

				scene->timeLive = game->time;
				PuzzleCorrect = false;
			}

			if (abs(game->time - scene->timeLive) > timeAnimation) {
				if (scene->entities[i]->id == 11) // marco
				{
					scene->entities[i]->isColision = false;
					scene->entities[i]->alpha = 1;
					scene->entities[i]->model.translate(0, -50, 0);

				}
				if (scene->entities[i]->id == 13)
					scene->entities[i]->isColision = false;
			}
			if (abs(game->time - scene->timeLive) > timeAnimation) {
				scene->mirrorParticle.clear();

			}
		}
		else
			scene->timeLive = 0.0f;
	}
	if (scene->entities[15]->alpha == 0 && abs(game->time - scene->timeLive) < timeAnimation) {
		renderParticle(abs(game->time - scene->timeLive) + 5);
	}

	renderGui();
}

void SoulStage::update(double seconds_elapsed)
{
	Scene* scene = Game::instance->soul_scene;
	Game* game = Game::instance;

	for (int i = 1; i < scene->entities_mirror.size(); i++)
	{
		scene->entities_mirror[i]->update(seconds_elapsed);
	}

	for (int i = 0; i < scene->entities.size(); i++)
	{
		scene->entities[i]->update(seconds_elapsed);
	}
	if (this->animation) //animacion puerta
	{
		float radDoor = -90 * DEG2RAD * seconds_elapsed;

		if (this->firstTime) {
			Timeanimation = game->time;
			this->firstTime = false;
		}
		if (game->time - Timeanimation < 1.2f && game->current_stage->animation2) {

			game->CurrentScene->entities[14]->isColision = false;
			scene->entities[14]->model.rotate(radDoor, Vector3(0.0f, 1.0f, 0.0f));

		}
		if (game->time - Timeanimation < 1.2f && !game->current_stage->animation2) {

			game->CurrentScene->entities[14]->isColision = true;
			scene->entities[14]->model.rotate(-radDoor, Vector3(0.0f, 1.0f, 0.0f));
		}
	}
	if (this->changeGlass && this->doorOpen2) {
		this->doorOpen2 = false;
		game->current_stage->animation2 = true;
		this->animation = true;
		this->firstTime = true;
		
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_B))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Play("data/audio/bath.wav", 0.5, false);
		game->current_stage = game->body_stage;
		game->CurrentScene = game->BodyScene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_M))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->mind_stage;
		game->CurrentScene = game->mind_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_C))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->corridor_stage;
		game->CurrentScene = game->corridor_scene;
		game->current_stage->createEntities();
	}
}

SoulStage::SoulStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = true;
}

void CorridorStage::createTextures()
{

	Scene* scene = Game::instance->corridor_scene;

	string texture = "data/passage/sala.tga,data/intro/intro.tga,data/passage/portal.tga,data/body/passagePlane.tga,data/body/passagePlane.tga,data/body/passagePlane.tga,data/imShader/noise.tga,data/intro/Door_BaseColor.tga,data/imShader/noise.tga,data/imShader/noise.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga,data/body/torch.tga";

	string cad;
	int found = -1;
	int init = 0;

	for (int i = 0; i < MAX_ENT_CORRIDOR; i++)
	{
		if ((i!=8 && i!=9 && i<17) ) {
			init = found + 1;
			found = texture.find(",", found + 1);
			cad = texture.substr(init, found - init);
		}
		this->entities[i]->texture = Texture::Get(cad.c_str());
		if (this->entities[i]->id == 7 || this->entities[i]->id == 11 || this->entities[i]->id == 12)
			this->entities[i]->texture2 = Texture::Get("data/imShader/gray.tga");
	}
}

void CorridorStage::createEntities()
{
	Scene* scene = Game::instance->corridor_scene;
	Game* game = Game::instance;
	game->audio->Play("data/audio/organ.wav", 0.1, true);

	game->spot3->intensity = 0;
	string mesh = "data/passage/sala.ASE,data/passage/salaIntro.ASE,data/passage/portal.ASE,data/passage/glassBody.ASE,data/intro/RightDoor.ase,data/intro/LeftDoor.ase,data/intro/ArcDoor.ase,data/passage/glassSpirit.ASE,data/passage/glassMind.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE,data/body/torch.ASE";
	this->changeGlass = false;

	string cad;
	this->InitStage = true;
	this->doorOpen2 = true;
	int found = -1;
	int init = 0;
	int playerNum = scene->entities.size();
	this->entities.resize(MAX_ENT_CORRIDOR);
	scene->entities.resize(MAX_ENT_CORRIDOR + 1); 

	//1 sala, 2 salaIntro, 3 portal, 4 planoprofundidad, 5 planoprofundidad, 6 planoprofundidad, 7 espejo roto portal 
	for (int i = 0; i < MAX_ENT_CORRIDOR; i++)
	{
		this->entities[i] = new EntityMesh();
		this->entities[i]->id = i + playerNum;

		if (this->entities[i]->id < 4 || this->entities[i]->id > 6 && this->entities[i]->id < 19) { //id:  1 2 3 7 8 9 ... 18
			init = found + 1;
			found = mesh.find(",", found + 1);
			cad = mesh.substr(init, found - init);
			this->entities[i]->mesh = Mesh::Get(cad.c_str());
		}

		if (this->entities[i]->id == 3) {
			this->entities[i]->model.translate(65, 0, 5);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
		}
		if (this->entities[i]->id == 4) {
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.translate(200, 20, 0);
			this->entities[i]->model.scale(1, 2, 2);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 0, 1));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
		}
		if (this->entities[i]->id == 5) {
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.translate(60, 7, -100);
			this->entities[i]->model.scale(1, 2, 1);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1, 0, 0));
		}
		if (this->entities[i]->id == 6) {
			this->entities[i]->mesh->createPlane(20);
			this->entities[i]->model.translate(60, 20, 100);
			this->entities[i]->model.scale(2, 2, 1);

			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1, 0, 0));
		}
		if (this->entities[i]->id == 7) { //glass body

			this->entities[i]->model.scale(0.7, 0.7, 0.7);
			this->entities[i]->model.translate(85.0f, -5.0f, 8.0f);
			this->entities[i]->alpha = 1;

		}
		if (this->entities[i]->id == 8)//door
		{
			this->entities[i]->model.translate(-18.0f, 0.0f, 16.0f);
			this->entities[i]->isColision = false;
		}

		if (this->entities[i]->id == 9) //door
		{
			this->entities[i]->model.translate(-18.0f, 0.0f, -15.5f);
			this->entities[i]->isColision = false;
		}

		if (this->entities[i]->id == 10) //door arco
		{
			this->entities[i]->isColision = false;
			this->entities[i]->model.translate(-18.0f, 0.0f, 0.0f);
		}

		if (this->entities[i]->id == 11) //glass spirit
		{ 
			this->entities[i]->model.scale(0.7, 0.7, 0.7);
			this->entities[i]->model.translate(85.0f, -5.0f, 8.0f); 
			this->entities[i]->alpha = 1;
		}

		if (this->entities[i]->id == 12) //glass mind		
		{
			this->entities[i]->model.scale(0.7, 0.7, 0.7);
			this->entities[i]->model.translate(85.0f, -5.0f, 8.0f);
			this->entities[i]->alpha = 1;
		}
		//prefab antrocha
		if (this->entities[i]->id == 13) //prefab antrocha	body		
			this->entities[i]->model.translate(45,10, -75);
		if (this->entities[i]->id == 14){ //prefab antrocha	body	
			this->entities[i]->model.rotate(180*DEG2RAD, Vector3(0,1,0));
			this->entities[i]->model.translate(-75, 10, 75);
		 }
		if (this->entities[i]->id == 15) {//prefab antrocha mind	
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(-15, 10, -160);
		}
		if (this->entities[i]->id == 16) { //prefab antrocha
			this->entities[i]->model.rotate(270 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(-25, 10, 160);
		}
		if (this->entities[i]->id == 17) //prefab antrocha		
			this->entities[i]->model.translate(40, 10, 80);
		if (this->entities[i]->id == 18) { //prefab antrocha		
			this->entities[i]->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
			this->entities[i]->model.translate(-80, 10, -80);
		}
		//Fire
		if (this->entities[i]->id == 19) //antorchas fuego MIND
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));

			this->entities[i]->model.translate(23, 160, 31);

		}
		if (this->entities[i]->id == 20) //antorchas fuego MIND
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));

			this->entities[i]->model.translate(-13, 160, 31);

		}
		//BODY antorchas fuego
		if (this->entities[i]->id == 21) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));

			this->entities[i]->model.translate(47,75 , 31); 
		}
		if (this->entities[i]->id == 22) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));

			this->entities[i]->model.translate(73, 75, 31);

		}
		//soul
		if (this->entities[i]->id == 23) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));

			this->entities[i]->model.translate(78, -80, 31);

		}
		if (this->entities[i]->id == 24) //antorchas fuego
		{
			this->entities[i]->mesh->createPlane(10);
			this->entities[i]->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));

			this->entities[i]->model.translate(42, -80, 31);

		}
		scene->entities[i+1] = this->entities[i];
	}
	createTextures();
}

void CorridorStage::render()
{
	Scene* scene = Game::instance->corridor_scene;
	for (int i = 0; i < scene->entities.size(); i++)
	{
		if (scene->entities[i]->id == 7 || scene->entities[i]->id == 11 || scene->entities[i]->id == 12)
			renderMirror(i, this->entities);
		if (scene->entities[i]->id != 7 && scene->entities[i]->id != 11 && scene->entities[i]->id != 12 && scene->entities[i]->id < 19)
			scene->entities[i]->render();
		if (scene->entities[i]->id > 18)
			renderTorch(i, this->entities);

	}
	renderGui();
}

void CorridorStage::update(double seconds_elapsed)
{
	Game* game = Game::instance;

	Scene* scene = Game::instance->corridor_scene;

	for (int i = 0; i < scene->entities.size(); i++)
	{
		scene->entities[i]->update(seconds_elapsed);
	}
	if (this->body) { //body id 7
		this->entities[6]->alpha = 0;
	}
	if (this->mind) { //mind id 11 
		this->entities[10]->alpha = 0;
	}
	if (this->soul) { //soul id 12
		this->entities[11]->alpha = 0;
	}
	if (this->glassCount == 3) {
		game->spot3->intensity = 0.8;

	}
	if (Input::wasKeyPressed(SDL_SCANCODE_B))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->audio->Play("data/audio/bath.wav", 0.5, false);
		game->current_stage = game->body_stage;
		game->CurrentScene = game->BodyScene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_M))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->mind_stage;
		game->CurrentScene = game->mind_scene;
		game->current_stage->createEntities();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_X))
	{
		this->firstTime = true;
		game->CurrentScene->entities.clear();
		game->current_stage = game->soul_stage;
		game->CurrentScene = game->soul_scene;
		game->current_stage->createEntities();
	}
	
}

CorridorStage::CorridorStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = true;
}

void EndStage::createEntities()
{
	Game* game = Game::instance;

	menu = new EntityMesh();
	menu->mesh->createPlane(100);
	menu->model.rotate(90 * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
	menu->texture = Texture::Get("data/end/end.tga");

	time = game->time;
}

void EndStage::render()
{
	Camera* camera = new Camera();
	Game* game = Game::instance;
	menu->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	menu->shader->enable();
	menu->shader->setUniform("u_model", menu->model);
	menu->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	menu->shader->setUniform("u_color", Vector4(1, 1, 1, 1));

	menu->shader->setUniform("u_texture", menu->texture, 0);
	menu->shader->setUniform("u_texture_tiling", 1.0f);

	////render the 
	menu->mesh->render(GL_TRIANGLES);
	menu->shader->disable();
	

}

void EndStage::update(double seconds_elapsed)
{
	Game* game = Game::instance;

	if (Input::wasKeyPressed(SDL_SCANCODE_M)) {
		game->current_stage = game->title_stage;
		game->current_stage->createEntities();
	}
	if (time + 0.5 < game->time) {
		menu->texture = Texture::Get("data/end/end1.tga");
	}
	if (time + 0.6 < game->time) {
		menu->texture = Texture::Get("data/end/end2.tga");
	}
	if (time + 0.7 < game->time) {
		menu->texture = Texture::Get("data/end/end3.tga");
	}
	if (time + 0.8 < game->time) {
		menu->texture = Texture::Get("data/end/end4.tga");
		
	}
	if (time + 0.9 < game->time) {
		menu->texture = Texture::Get("data/end/end5.tga");
	}
	if (time + 1.0 < game->time) {
		menu->texture = Texture::Get("data/end/end6.tga");
	}
	if (time + 1.1 < game->time) {
		menu->texture = Texture::Get("data/end/credits.tga"); //creditos
	} 
	if (time + 4.0 < game->time) {

		game->title_stage = new TitleStage();
		game->intro_stage = new IntroStage();
		game->body_stage = new BodyStage();
		game->soul_stage = new SoulStage();
		game->end_stage = new EndStage();
		game->corridor_stage = new CorridorStage();
		game->mind_stage = new MindStage();
		game->free_camera = true;
		game->current_stage = game->title_stage;
		game->current_stage->createEntities();
	}
}

EndStage::EndStage()
{
	this->doorOpen2 = true;
	this->changeGlass = false;
	this->glassCount = 0;

	this->isAmulet = false;
	this->id = 0;
	this->isRa = false;

	//GUI
	this->amuleto = false;
	this->grail = false;
	this->cruz = false;
	this->arrow = false;


	this->animation = false;
	this->Timeanimation = 0.0f;
	this->firstTime = true;
	this->animation2 = true;

	//particle active mirror
	this->AnimationMirror = true;
	this->NumParticle = 20;

	//stages 
	this->body = false;
	this->mind = false;
	this->soul = false;

	this->InitStage = false;
}
