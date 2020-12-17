#include "platform.hpp"
#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <future>
#include <Windows.h>

#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "tdogl/Camera.h"

#include "gui.h"
#include "heightMesh.h"
#include "skydome.h"
#include "billboard.h"
#include "model.h"

const bool DEBUG = true;
const bool GOD_MODE = false;
const bool MULTITHREAD = false; //No funciona, pendiente de corrección
const bool FULL_SCREEN = false; //Cuidado, si truena, se queda pantalla negra (solo usar al entregar)

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

const unsigned int TERRAIN_WIDTH = 2000;
const unsigned int TERRAIN_HEIGHT = 200;
const unsigned int TERRAIN_DEPTH = 2000;

const unsigned int WATER_HEIGHT = 30;
const float WATER_SPEED = 0.001;

const unsigned int BILLBOARDS_SQRT_COUNT = 15;

const float PROJECTILE_INITIAL_SPEED = 500.0f;
const float PROJECTILE_SPEED_INCREASE = 50.0f;
const float PROJECTILE_MAX_SPEED = 2500.0f;

const float TERRAIN_MOVEMENT_SPEED = 100.0f;
const float WATER_MOVEMENT_SPEED = 50.0f;

const unsigned int TARGETS_SQRT_COUNT = 5;

const float EXPLOSIONS_TIMESPAN = 3;

const float SCALE_TARGETS = 10.0f;
const float SCALE_PROJECTILE = 0.01f;



//Manejador de la ventana
vec2 SCREEN_SIZE;
GLFWwindow* gWindow = NULL;

//Punteros a nuestros shaders
Program* guiShader = NULL;
Program* meshShader = NULL;
Program* terrainShader = NULL;
Program* waterShader = NULL;
Program* billboardsShader = NULL;
Program* skydomeShader = NULL;

//Punteros a nuestros elementos
Camera* gCamera;
Model *hummer, *cup,*pickup,*salud,*ammo,*enemy1,*enemy2,*boat,*bullet, *rock, *bottle, *granade;
GUI *gui;
Texture* background, *target[2], * explosion, * win, * tree[3], * skydomeTexture[2];
Billboard* billboard;
Skydome* skydome;
HeightMesh *terrain, *water;
BoundingSphere* collisionDome;

struct Projectile {
	vec3 position;
	vec3 direction;
	mat4 orientation;
	GLfloat speed;
	GLboolean automatic;
	GLboolean isShooted;
	vec3 initialPosition;
	GLfloat timeAlive;
};

struct Target {
	vec3 position;
	GLuint health = 1;
	GLuint health2 = 2;
};

struct Bboard {
	vec3 position;
	GLuint type;
};

struct Explosion {
	vec3 position;
	GLfloat timespan;
};

GLboolean thirdPerson = false;
GLboolean paused = false;
GLboolean aim = true;
GLboolean lowl = true;
GLboolean winc = false;
GLboolean hide = false;
GLboolean hideh = false;
GLdouble gScrollY = 0.0;
GLfloat gDegreesRotated = 0.0f;
GLfloat waterMovement = 0.0f;
GLuint myHealth = 10;
GLuint myAmmo = 30;
GLuint myGranade = 5;
GLboolean leftPressed = false;
GLboolean middlePressed = false;
GLboolean rightPressed = false;
GLfloat tempView = 0.0f;
vec3 mobile, shoot;
vector<Projectile> pProjectiles;
vector<Target> pTargets;
vector<Target> pTargets2;
vector<Explosion> explosions;
vector<Bboard> billboards;


void LoadScreen() {
	//screen de inicio
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gui->Draw(gCamera->orthographicMatrix(), background->object(), 0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y);
	gui->DrawFont(gCamera->orthographicMatrix(), "Dead Trip", 450.0f, 50.0f, 0.9f, vec3(1, 1, 1));
	gui->DrawFont(gCamera->orthographicMatrix(), "Loading... Please Wait", (SCREEN_SIZE.x / 2.0f) * 0.70f, (SCREEN_SIZE.y / 2.0f) * 0.95f, 0.5f, vec3(1.0f, 1.0f, 1.0f));
	gui->DrawFont(gCamera->orthographicMatrix(), "Llega rapido a la balsa antes que los zombies te encuentren", 10.0f, 550.0f, 0.3, vec3(1, 1, 1));
	gui->DrawFont(gCamera->orthographicMatrix(), "-Dispara a todos los enemigos.", 10.0f, 565.0f, 0.3, vec3(1, 1, 1));
	gui->DrawFont(gCamera->orthographicMatrix(), "- Recoge las municiones y la vida para poder mantenerte.", 10.0f, 580.0f, 0.3, vec3(1, 1, 1));
	
	glfwSwapBuffers(gWindow);
}

void LoadShaders() {
	vector<Shader> shaders;

	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/gui.vs"), GL_VERTEX_SHADER));
	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/gui.fs"), GL_FRAGMENT_SHADER));
	guiShader = new Program(shaders);
	shaders.clear();


	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/mesh.vs"), GL_VERTEX_SHADER));
	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/mesh.fs"), GL_FRAGMENT_SHADER));
	meshShader = new Program(shaders);
	shaders.clear();

	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/height.vs"), GL_VERTEX_SHADER));
	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/terrain.fs"), GL_FRAGMENT_SHADER));
	terrainShader = new Program(shaders);
	shaders.clear();

	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/height.vs"), GL_VERTEX_SHADER));
	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/water.fs"), GL_FRAGMENT_SHADER));
	waterShader = new Program(shaders);
	shaders.clear();

	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/gui.vs"), GL_VERTEX_SHADER));
	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/gui.fs"), GL_FRAGMENT_SHADER));
	billboardsShader = new Program(shaders);
	shaders.clear();

	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/gui.vs"), GL_VERTEX_SHADER));
	shaders.push_back(Shader::shaderFromFile(ResourcePath("shaders/gui.fs"), GL_FRAGMENT_SHADER));
	skydomeShader = new Program(shaders);
	shaders.clear();

	//Send info to shaders

	meshShader->use();
	meshShader->setUniform("lightPosition", vec3(1.0f, -1.0f, 1.0f));
	meshShader->setUniform("lightColor", vec3(1.0f, 1.0f, 1.0f));
	meshShader->setUniform("ambientCoe", 0.1f);
	meshShader->stopUsing();

	terrainShader->use();
	terrainShader->setUniform("lightPosition", vec3(1.0f, -1.0f, 1.0f));
	terrainShader->setUniform("lightColor", vec3(1.0f, 1.0f, 1.0f));
	terrainShader->setUniform("ambientCoe", 0.1f);
	terrainShader->stopUsing();

	waterShader->use();
	waterShader->setUniform("lightPosition", vec3(1.0f, -1.0f, 1.0f));
	waterShader->setUniform("lightColor", vec3(1.0f, 1.0f, 1.0f));
	waterShader->setUniform("ambientCoe", 0.1f);
	waterShader->stopUsing();
}
//modelos
void LoadModels() {

	pickup = Model::ObjToModel(meshShader, ResourcePath("models/pickup"), "pickup.obj");
	enemy1 = Model::ObjToModel(meshShader, ResourcePath("models/PhychoZombie"), "Phychozomb.obj");
	enemy2 = Model::ObjToModel(meshShader, ResourcePath("models/TypicalNekro"), "Slasher.obj");
	boat = Model::ObjToModel(meshShader, ResourcePath("models/obj_boat"), "boat.obj");
	salud = Model::ObjToModel(meshShader, ResourcePath("models/salud"), "First_Aid_Kit.obj");
	ammo = Model::ObjToModel(meshShader, ResourcePath("models/ShellsBoxOBJ"), "ShotgunShells.obj");
	cup = Model::ObjToModel(meshShader, ResourcePath("models/cup"), "cup.obj");
	bullet = Model::ObjToModel(meshShader, ResourcePath("models/"), "Bullet.obj");
	bottle = Model::ObjToModel(meshShader, ResourcePath("models/oil"), "old_oil_barrel.obj");
	rock = Model::ObjToModel(meshShader, ResourcePath("models/rock"), "Rock_1.obj");

	if (DEBUG) 
	{
		hummer = new Model(*cup);
	}
	else
	{
		hummer = Model::ObjToModel(meshShader, ResourcePath("models/hummer"), "hummer.obj");
	}
	mobile = vec3(800, 175, 300);
}

//random things
int random(int min, int max) //range : [min, max)
{
	static bool first = true;
	if (first)
	{
		srand(time(NULL)); //seeding for the first time only!
		first = false;
	}
	return min + rand() % ((max + 1) - min);
}

void Reload() {
	thirdPerson = false;
	paused = false;
	aim = true;
	gScrollY = 0.0;
	gDegreesRotated = 0.0f;
	waterMovement = 0.0f;
	myHealth = 10;
	myAmmo = 30;
	myGranade = 5;
	leftPressed = false;
	middlePressed = false;
	rightPressed = false;

	pProjectiles.clear();
	pTargets.clear();
	pTargets2.clear();
	explosions.clear();
	billboards.clear();

	gCamera->setPosition(vec3(TERRAIN_WIDTH / 2.0, 0, TERRAIN_DEPTH / 2.0));

	GLfloat offsetWidth = TERRAIN_WIDTH / TARGETS_SQRT_COUNT;
	GLfloat offsetDepth = TERRAIN_DEPTH / TARGETS_SQRT_COUNT;

	for (GLint x = 0; x < TARGETS_SQRT_COUNT; x++)
		for (GLint z = 0; z < TARGETS_SQRT_COUNT; z++) {
			Target temp;
			temp.position = vec3(x * offsetWidth, 0, z * offsetDepth);

			GLfloat terrainHeight = terrain->GetAverageHeight(temp.position.x, temp.position.z);
			GLfloat waterHeight = water->GetAverageHeight(temp.position.x, temp.position.z);

			temp.position.y = std::max(terrainHeight, waterHeight) + 3;
			pTargets.push_back(temp);
		}

	for (GLint x = 0; x <  2; x++)
		for (GLint z = 0; z < 2; z++) {
			Target temp;
			temp.position = vec3(x * offsetWidth+500.0f, 0, z * offsetDepth+500.0f);

			GLfloat terrainHeight = terrain->GetAverageHeight(temp.position.x, temp.position.z);
			GLfloat waterHeight = water->GetAverageHeight(temp.position.x, temp.position.z);

			temp.position.y = std::max(terrainHeight, waterHeight) + 3;
			pTargets2.push_back(temp);
		}

	offsetWidth = TERRAIN_WIDTH / BILLBOARDS_SQRT_COUNT;
	offsetDepth = TERRAIN_DEPTH / BILLBOARDS_SQRT_COUNT;

	for (GLint x = 0; x < BILLBOARDS_SQRT_COUNT; x++)
		for (GLint z = 0; z < BILLBOARDS_SQRT_COUNT; z++) {
			Bboard temp;
			temp.position = vec3(x * offsetWidth, 0, z * offsetDepth);
			temp.position.y = terrain->GetAverageHeight(temp.position.x, temp.position.z) + 20.0;
			temp.type = random(0, 2);

			GLfloat terrainHeight = terrain->GetAverageHeight(temp.position.x, temp.position.z);
			GLfloat waterHeight = water->GetAverageHeight(temp.position.x, temp.position.z);

			if (terrainHeight > waterHeight) billboards.push_back(temp);
		}


}

void Load() {

	LoadShaders();
	//delcaraciones de texturas
	gCamera = new Camera(SCREEN_SIZE.x, SCREEN_SIZE.y);
	gui = new GUI(guiShader, ResourcePath("fonts/arial.ttf"));
	background = new Texture(Bitmap::bitmapFromFile(ResourcePath("gui/background.jpg"), false));
	skydomeTexture[0] = new Texture(Bitmap::bitmapFromFile(ResourcePath("environment/skydome.jpg"), false));
	skydomeTexture[1] = new Texture(Bitmap::bitmapFromFile(ResourcePath("environment/skydomeNight.jpg"), false));
	target[1] = new Texture(Bitmap::bitmapFromFile(ResourcePath("gui/target.png"), false));
	explosion = new Texture(Bitmap::bitmapFromFile(ResourcePath("gui/explosion.png"), false));
	win = new Texture(Bitmap::bitmapFromFile(ResourcePath("gui/win.png"), false));

	LoadScreen();
	
	tree[0] = new Texture(Bitmap::bitmapFromFile(ResourcePath("environment/tree1.png")));
	tree[1] = new Texture(Bitmap::bitmapFromFile(ResourcePath("environment/tree2.png")));
	tree[2] = new Texture(Bitmap::bitmapFromFile(ResourcePath("environment/tree3.png")));

	vector<string> textures;
	textures.push_back(ResourcePath("environment/rock.jpg"));
	textures.push_back(ResourcePath("environment/soil.jpg"));
	textures.push_back(ResourcePath("environment/grass.jpg"));//textura secundaria
	textures.push_back(ResourcePath("environment/snow.jpg")); //textura dominante
	textures.push_back(ResourcePath("environment/blendMap.jpg"));

	terrain = new HeightMesh(terrainShader, ResourcePath("environment/heightMap.bmp"), textures, TERRAIN_WIDTH, TERRAIN_HEIGHT, TERRAIN_DEPTH, 50);

	textures.clear();
	textures.push_back(ResourcePath("environment/waterDiffuse.jpg"));

	water = new HeightMesh(waterShader, WATER_HEIGHT, textures, TERRAIN_WIDTH, TERRAIN_DEPTH, 50);

	collisionDome = new BoundingSphere(vec3((TERRAIN_WIDTH / 2.0), (TERRAIN_HEIGHT / 2.0), (TERRAIN_DEPTH / 2.0)), std::max(TERRAIN_WIDTH, TERRAIN_DEPTH) * 1.1);

	LoadModels();

	billboard = new Billboard(billboardsShader);
	skydome = new Skydome(skydomeShader, 100.0f);

	Reload();
}
 
static float distancia=0;

void itshealth(float px, float py, float pz) {
	vec3 position = gCamera->position();
	distancia = glm::sqrt(pow((px - gCamera ->position().x), 2)
		+ pow((py - gCamera->position().y), 2)
		+ pow((pz - gCamera->position().z), 2));
	if (distancia<=10)
	{
			while (myHealth<=9)
			{
				myHealth++;
				hideh = true;
			}
	
	}
}

void itsammo(float px, float py, float pz) {
	vec3 position = gCamera->position();
	distancia = glm::sqrt(pow((px - gCamera->position().x), 2)
		+ pow((py - gCamera->position().y), 2)
		+ pow((pz - gCamera->position().z), 2));
	if (distancia <= 10){
		while (myAmmo <= 29)
		{
			myAmmo++;
			lowl = true;
			hide = true;
		}
	}
}

void wincond(float px, float py, float pz) {
	vec3 position = gCamera->position();
	distancia = glm::sqrt(pow((px - gCamera->position().x), 2)
		+ pow((py - gCamera->position().y), 2)
		+ pow((pz - gCamera->position().z), 2));
	if (distancia <= 10)
	{
			winc = true;
	}
}

void colide(float px, float py, float pz) {
	vec3 position = gCamera->position();
	distancia = glm::sqrt(pow((px - gCamera->position().x), 2)
		+ pow((py - gCamera->position().y), 2)
		+ pow((pz - gCamera->position().z), 2));
	if (distancia <= 10)
	{
		mobile.x--;
		mobile.z--;
	}
}


void Render() {
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	vec3 position = gCamera->position();
	position.y -= 100.0f / 2.0;
	mat4 skyModelMatrix =
		translate(mat4(), position) *
		rotate(mat4(), radians(gDegreesRotated), vec3(0, 1, 0));

	GLboolean sky = pTargets.size() <= (TARGETS_SQRT_COUNT / 2.0);
	skydome->Draw(lowl== false || myHealth<=3 ? skydomeTexture[1]->object() : skydomeTexture[0]->object(), gCamera->perspectiveMatrix(), skyModelMatrix);
	

	terrain->Draw(gCamera->perspectiveMatrix());

	meshShader->use();
	meshShader->setUniform("camera", gCamera->perspectiveMatrix());
	meshShader->setUniform("cameraPosition", gCamera->position());

	//enemy1->Draw(meshShader, translate(mat4(),vec3(1700,75,1000))*scale(mat4(),vec3(3,3,3)));
	if (!hide) {
		ammo->Draw(meshShader, translate(mat4(), vec3(1100, 175, 1100))* scale(mat4(), vec3(5, 5, 5)));
		itsammo(1100, 175, 1100);
	}
	if (!hideh)
	{
		salud->Draw(meshShader, translate(mat4(), vec3(850, 175, 800))* rotate(mat4(), radians(gDegreesRotated),vec3(1,0,0)) * scale(mat4(), vec3(6, 6, 6)));
		itshealth(850, 175, 800);
	}
	boat->Draw(meshShader, translate(mat4(), vec3(978, 40, 1730)) * scale(mat4(), vec3(6, 6, 6)));
	bottle->Draw(meshShader, translate(mat4(), vec3(910, 60, 1650)) * scale(mat4(), vec3(6, 6, 6)));
	wincond(970, 50, 1710);
	colide(910, 82, 1650);

	rock->Draw(meshShader, translate(mat4(), vec3(700, 160, 1100)) * scale(mat4(), vec3(6, 6, 6)));
	colide(700, 173, 1100);
	mat4 rotatio = mat4();
	rotatio = gCamera->orientation();
	pickup->Draw(meshShader, translate(glm::mat4(), mobile - glm::vec3(0,20, 0))*rotatio
		* scale(mat4(), vec3(10, 10, 10)));

	//objetivos
	for (unsigned int i = 0; i < pTargets.size(); i++) {
		enemy1->Draw(meshShader,translate(mat4(), pTargets[i].position)*scale(mat4(), vec3(pTargets[i].health * SCALE_TARGETS, pTargets[i].health * SCALE_TARGETS, pTargets[i].health * SCALE_TARGETS)));		
	}
	for (unsigned int i = 0; i < pTargets2.size(); i++) {
		enemy2->Draw(meshShader, translate(mat4(), pTargets2[i].position) * scale(mat4(), vec3(pTargets2[i].health2 * SCALE_TARGETS, pTargets2[i].health2 * SCALE_TARGETS, pTargets2[i].health2 * SCALE_TARGETS)));
	}

	//disparos
	for (unsigned int i = 0; i < pProjectiles.size(); i++) {
		mat4 rotation = mat4();

		if (pProjectiles[i].automatic) rotation = pProjectiles[i].orientation * rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
		else rotation = rotate(mat4(), -gDegreesRotated * 5.0f, vec3(1, 0, 1));

		bullet->Draw(meshShader,
			translate(mat4(), pProjectiles[i].position) * rotation *
			scale(mat4(), vec3(pProjectiles[i].speed * SCALE_PROJECTILE, pProjectiles[i].speed * SCALE_PROJECTILE, pProjectiles[i].speed * SCALE_PROJECTILE)));
	}
		
	meshShader->stopUsing();

	for (unsigned int i = 0; i < explosions.size(); i++) {
		billboard->DrawHV(explosion->object(), gCamera->perspectiveMatrix(), gCamera->position(), explosions[i].position, 5, 5);
	}
	//billboard
	for (unsigned int i = 0; i < billboards.size(); i++) {
		billboard->DrawH(tree[billboards[i].type]->object(), gCamera->perspectiveMatrix(), gCamera->position(), billboards[i].position, 50, 50);
	}

	waterShader->use();
	waterShader->setUniform("cameraPosition", gCamera->position());
	waterShader->stopUsing();

	water->Draw(gCamera->perspectiveMatrix(), waterMovement);

	//interface grafica
	//gui->DrawFont(gCamera->orthographicMatrix(), "Hummer Destroy", 210.0f, 50.0f, 0.9f, vec3(1.0f, 1.0f, 1.0f), 0.5f);
	gui->DrawFont(gCamera->orthographicMatrix(), "Targets left: " + to_string(pTargets.size()), 5, 420, 0.75, vec3(0, 0, 1), 0.75);
	gui->DrawFont(gCamera->orthographicMatrix(), "Boss left: " + to_string(pTargets2.size()), 5, 580, 0.75, vec3(0, 0, 1), 0.75);
	gui->DrawFont(gCamera->orthographicMatrix(), "Health: " + to_string(myHealth), 5, 540, 0.75, myHealth <= 5 ? vec3(1, 0, 0) : vec3(0, 1, 0), 0.75);
	gui->DrawFont(gCamera->orthographicMatrix(), "Ammo: " + to_string(myAmmo), 5, 500, 0.75, vec3(0, 1, 1), 0.75);
	gui->DrawFont(gCamera->orthographicMatrix(), "Granade: " + to_string(myGranade), 5, 460, 0.75, vec3(1, 1, 1), 0.75);
	
	if (myHealth == 0) gui->DrawFont(gCamera->orthographicMatrix(), "Game Over", (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) - 50, 1.0, vec3(1, 1, 1), 1.0);
	if (myHealth == 0) gui->DrawFont(gCamera->orthographicMatrix(), "Press \"R\" to restart.", (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) + 100, 0.7, vec3(1, 1, 1));
	if (winc) {
		gui->DrawFont(gCamera->orthographicMatrix(), "TE HAZ SALVADO", (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) - 50, 1.0, vec3(1, 1, 1), 1.0);
		gui->DrawFont(gCamera->orthographicMatrix(), "Press \"R\" to restart.", (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) + 100, 0.7, vec3(1, 1, 1));
		paused= !paused;
		
	}
	//coordenadas para colisiones 
	gui->DrawFont(gCamera->orthographicMatrix(), "X: " + to_string(gCamera->position().x), 5, 20, 0.3, vec3(1, 1, 1), 0.5);
	gui->DrawFont(gCamera->orthographicMatrix(), "Y: " + to_string(gCamera->position().y), 5, 40, 0.3, vec3(1, 1, 1), 0.5);
	gui->DrawFont(gCamera->orthographicMatrix(), "Z: " + to_string(gCamera->position().z), 5, 60, 0.3, vec3(1, 1, 1), 0.5);

	//letreros
//	if (!paused)gui->Draw(gCamera->orthographicMatrix(), target->object(), (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) - 50, 100, 100, 0.5f);
	if(!aim)gui->Draw(gCamera->orthographicMatrix(), target[1]->object(), (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) - 50, 100, 100, 0.5f);
	if (paused) gui->DrawFont(gCamera->orthographicMatrix(), "PAUSE", (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) + 50, 1.0, vec3(1, 1, 1));
	if (paused) gui->DrawFont(gCamera->orthographicMatrix(), "Press \"R\" to restart.", (SCREEN_SIZE.x / 2.0f) - 50, (SCREEN_SIZE.y / 2.0f) + 100, 0.7, vec3(1, 1, 1));
    glfwSwapBuffers(gWindow);
}

void projectileCrash(int index, vec3 position, bool explosion = false)
{
	if (explosion) {
		Explosion temp;
		temp.position = position;
		temp.timespan = EXPLOSIONS_TIMESPAN;
		explosions.push_back(temp);
	}
	pProjectiles.erase(pProjectiles.begin() + index);
}

vec3 parabola(vec3 initialVelocity, float gravity, float time)
{
	vec3 outDisplacement;

	outDisplacement.x = initialVelocity.x * time;
	outDisplacement.z = initialVelocity.z * time;

	outDisplacement.y = (initialVelocity.y * time) - ((0.5*gravity) * (time * time));

	return outDisplacement;
}

void Update(float secondsElapsed) {
    const GLfloat degreesPerSecond = 1.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;

	waterMovement += WATER_SPEED;
	if (waterMovement >= 1.0) waterMovement = 0.0f;

	vec3 position = gCamera->position();
	GLfloat terrainHeight = terrain->GetAverageHeight(position.x, position.z);
	GLfloat waterHeight = water->GetAverageHeight(position.x, position.z);
    const float moveSpeed = terrainHeight > waterHeight ? TERRAIN_MOVEMENT_SPEED : WATER_MOVEMENT_SPEED;
	//controles
		if (glfwGetKey(gWindow, 'S'))
		{
			//gCamera->offsetPosition(secondsElapsed * moveSpeed * -gCamera->forward());
			mobile -= -secondsElapsed * moveSpeed * -gCamera->forward();
			shoot = mobile;
		}
		else if (glfwGetKey(gWindow, 'W')) {
			//gCamera->offsetPosition(secondsElapsed * moveSpeed * gCamera->forward());
			mobile += -secondsElapsed * moveSpeed * -gCamera->forward();
			shoot = mobile;
		}
		if (glfwGetKey(gWindow, 'A')) {
			//gCamera->offsetPosition(secondsElapsed * moveSpeed * -gCamera->right());
			mobile += -secondsElapsed * moveSpeed * gCamera->right();
			shoot = mobile;
		}
		else if (glfwGetKey(gWindow, 'D')) {
			//gCamera->offsetPosition(secondsElapsed * moveSpeed * gCamera->right());
			mobile += -secondsElapsed * moveSpeed * -gCamera->right();
			shoot = mobile;
		}
		if (glfwGetKey(gWindow, 'Z')) {
			gCamera->offsetPosition(secondsElapsed * moveSpeed * -gCamera->up());
			shoot = mobile;
			//mobile += -secondsElapsed * moveSpeed * -gCamera->up;
		}
		else if (glfwGetKey(gWindow, 'X')) {
			gCamera->offsetPosition(secondsElapsed * moveSpeed * gCamera->up());
			shoot = mobile;
			//mobile += -secondsElapsed * moveSpeed * gCamera->up;
		}

		//primera persona
	if (!GOD_MODE)
	{
		//position = gCamera->position();
		terrainHeight = terrain->GetAverageHeight(mobile.x, mobile.z);
		waterHeight = water->GetAverageHeight(mobile.x, mobile.z);
		GLfloat height = std::max(terrainHeight, waterHeight) + 25.0f;
		//gCamera->setHeight(height);
		mobile.y = height;
	}

	//position = gCamera->position();
	gCamera->setPosition(mobile);
    const float mouseSensitivity = 0.1f;
    double mouseX, mouseY;
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    gCamera->offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(gWindow, 0, 0);

    const float zoomSensitivity = -1.0f;
    float fieldOfView = gCamera->fieldOfView() + zoomSensitivity * (float)gScrollY;
    if(fieldOfView < 5.0f) fieldOfView = 5.0f;
    if(fieldOfView > 130.0f) fieldOfView = 130.0f;
    gCamera->setFieldOfView(fieldOfView);
    gScrollY = 0;

	static bool inCreation = false;
	static int projectileSpacer = 0;
	static float tempSpeed = PROJECTILE_INITIAL_SPEED;

	//disparo cargado
	if (myGranade != 0) {
		if (rightPressed)
		{
			if (!inCreation) {
				Projectile temp;
				temp.timeAlive = 0.0f;
				temp.initialPosition = shoot;
				temp.position = shoot;
				temp.direction = vec3(0, 0, -1) * inverse(mat3(gCamera->orientation()));
				temp.orientation = mat4();
				temp.speed = tempSpeed;
				temp.automatic = false;
				temp.isShooted = false;
				pProjectiles.push_back(temp);
			}

			if (pProjectiles.back().speed < PROJECTILE_MAX_SPEED) pProjectiles.back().speed += PROJECTILE_SPEED_INCREASE;

			pProjectiles.back().direction = vec3(0, 0, -1) * inverse(mat3(gCamera->orientation()));
			pProjectiles.back().initialPosition = pProjectiles.back().position = shoot + (pProjectiles.back().direction * PROJECTILE_SPEED_INCREASE);

			inCreation = true;
		}
		else
		{
			if (inCreation)
			{
				pProjectiles.back().isShooted = true;
				tempSpeed = PROJECTILE_INITIAL_SPEED;
				inCreation = false;
				myGranade--;
			}
		}
		
	}

	//disparo normal

	if (myAmmo != 0) {
		if (leftPressed)
		{
			projectileSpacer += 1;
			
			if ((projectileSpacer % 10) == 0)
			{
				Projectile tempProjectile;
				tempProjectile.timeAlive = 0.0f;
				tempProjectile.initialPosition = shoot;
				tempProjectile.position = shoot;
				tempProjectile.direction = vec3(0, 0, -1) * inverse(mat3(gCamera->orientation()));
				tempProjectile.speed = tempSpeed;
				tempProjectile.automatic = true;
				tempProjectile.isShooted = true;

				vec3 cameraPosition = shoot;
				vec3 tempPosition = tempProjectile.position + tempProjectile.direction;

				float angleH = atan2(tempPosition.x - cameraPosition.x, tempPosition.z - cameraPosition.z) * (180.0 / pi<float>());
				float angleV = atan2(tempPosition.y - cameraPosition.y, tempPosition.z - cameraPosition.z) * (180.0 / pi<float>());

				vec3 up = vec3(0, 1, 0);
				GLfloat angle = 0.0f;

				vec3 objToCam = cameraPosition - tempPosition;
				objToCam = normalize(objToCam);

				GLfloat angleCosine = dot(vec3(0, 0, -1), objToCam);
				up = cross(vec3(0, 0, -1), objToCam);

				angle = acos(angleCosine) * (180.0 / pi<float>());

				tempProjectile.orientation = rotate(mat4(), radians(angle), up);

				pProjectiles.push_back(tempProjectile);

				projectileSpacer = 0;
				myAmmo--;
				if (myAmmo==3)
				{
					lowl= false;
				}
				
				
			}
		}
	}


	for (unsigned int i = 0; i < pProjectiles.size(); i++) {
		if (!pProjectiles[i].isShooted) continue;
		
		pProjectiles[i].timeAlive += secondsElapsed;

		if (pProjectiles[i].automatic)
		{
			pProjectiles[i].position += pProjectiles[i].direction * (pProjectiles[i].speed * secondsElapsed);
		} 
		else
		{
			vec3 velocity = pProjectiles[i].direction * pProjectiles[i].speed * 0.5f;
			pProjectiles[i].position = pProjectiles[i].initialPosition + parabola(velocity, 500.0f, pProjectiles[i].timeAlive);
		}
		
		mat4 rotation = mat4();
		if (pProjectiles[i].automatic) rotation = pProjectiles[i].orientation * rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
		else rotation = rotate(mat4(), -gDegreesRotated * 5.0f, vec3(1, 0, 1));
		bullet->Update(translate(mat4(), pProjectiles[i].position)* rotation *
			scale(mat4(), vec3(pProjectiles[i].speed* SCALE_PROJECTILE, 
			pProjectiles[i].speed* SCALE_PROJECTILE, 
			pProjectiles[i].speed* SCALE_PROJECTILE)));

		if (terrain->GetAverageHeight(pProjectiles[i].position.x, pProjectiles[i].position.z) > pProjectiles[i].position.y)
		{
			projectileCrash(i, pProjectiles[i].position, true);
			break;
		}

		if (!bullet->collider.bs.Collides(collisionDome))
		{
			projectileCrash(i, pProjectiles[i].position);
			break;
		}

		for (unsigned int j = 0; j < pTargets.size(); j++) {
			enemy1->Update(translate(mat4(), pTargets[j].position)*
				scale(mat4(), vec3(pTargets[j].health* SCALE_TARGETS, pTargets[j].health* SCALE_TARGETS, pTargets[j].health* SCALE_TARGETS)));

			//colisiones
			if (bullet->Collides(enemy1)) {
				projectileCrash(i, pProjectiles[i].position, true);
				pTargets[j].health--;
				if (pTargets[j].health == 0) pTargets.erase(pTargets.begin() + j);
				break;
			}
		}
		for (unsigned int j = 0; j < 2; j++) {
			enemy2->Update(translate(mat4(), pTargets2[j].position) *
				scale(mat4(), vec3(pTargets2[j].health * SCALE_TARGETS, pTargets2[j].health2 * SCALE_TARGETS, pTargets2[j].health2 * SCALE_TARGETS)));

			//colisiones
			if (bullet->Collides(enemy2)) {
				projectileCrash(i, pProjectiles[i].position, true);
				pTargets2[j].health2--;
				if (pTargets2[j].health == 0) pTargets2.erase(pTargets2.begin() + j);
				break;
			}
		}
	}

	///aqui van movimiento
	for (unsigned int j = 0; j < pTargets.size(); j++) {
		enemy1->Update(translate(mat4(), pTargets[j].position)*
			scale(mat4(), vec3(pTargets[j].health* SCALE_TARGETS, pTargets[j].health* SCALE_TARGETS, pTargets[j].health* SCALE_TARGETS)));
		if(mobile.x < pTargets[j].position.x){
			pTargets[j].position.x--;
		}
		else if (mobile.x > pTargets[j].position.x)
		{
			pTargets[j].position.x++;
		}
		if (mobile.x < pTargets[j].position.x) {
			pTargets[j].position.x--;
		}
		else if (mobile.z > pTargets[j].position.z)
		{
			pTargets[j].position.z++;
		}
		if (mobile.z < pTargets[j].position.z) {
			pTargets[j].position.z--;
		}
		else if (mobile.y > pTargets[j].position.y)
		{
			pTargets[j].position.y++;
		}
		if (mobile.y < pTargets[j].position.y) {
			pTargets[j].position.y--;
		}


		//perder
		if (myHealth > 0) {
			static int healthSpacer = 0;
			healthSpacer += 1;
			if ((healthSpacer % 30) == 0)
			{
				if (enemy1->collider.bs.Collides(&BoundingPoint(mobile))) {
					myHealth--;
				}
				healthSpacer = 0;
			}
		}
	}

	for (unsigned int j = 0; j < 2; j++) {
		enemy2->Update(translate(mat4(), pTargets2[j].position) *
			scale(mat4(), vec3(pTargets2[j].health2 * SCALE_TARGETS, pTargets2[j].health2 * SCALE_TARGETS, pTargets2[j].health2 * SCALE_TARGETS)));
		if (mobile.x < pTargets2[j].position.x) {
			pTargets2[j].position.x--;
		}
		else if (mobile.x > pTargets2[j].position.x)
		{
			pTargets2[j].position.x++;
		}
		if (mobile.x < pTargets2[j].position.x) {
			pTargets2[j].position.x--;
		}
		else if (mobile.z > pTargets2[j].position.z)
		{
			pTargets2[j].position.z++;
		}
		if (mobile.z < pTargets2[j].position.z) {
			pTargets2[j].position.z--;
		}
		else if (mobile.y > pTargets2[j].position.y)
		{
			pTargets2[j].position.y++;
		}
		if (mobile.y < pTargets2[j].position.y) {
			pTargets2[j].position.y--;
		}


		//perder
		if (myHealth > 0) {
			static int healthSpacer = 0;
			healthSpacer += 1;
			if ((healthSpacer % 30) == 0)
			{
				if (enemy2->collider.bs.Collides(&BoundingPoint(mobile))) {
					myHealth--;
				}
				healthSpacer = 0;
			}
		}
	}


	for (unsigned int i = 0; i < explosions.size(); i++) {
		explosions[i].timespan -= secondsElapsed;
		if (explosions[i].timespan <= 0.0) explosions.erase(explosions.begin() + i);
	}
}

void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
    gScrollY += deltaY;
}

void OnError(int errorCode, const char* msg) {
    throw runtime_error(msg);
}

void OnButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		rightPressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		rightPressed = false;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		leftPressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		leftPressed = false;
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		tempView = gCamera->fieldOfView();
		if (!thirdPerson) gCamera->setFieldOfView(5.0);
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		gCamera->setFieldOfView(tempView);
	}
}

//como aparecer cosas presinando botones
void OnKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		thirdPerson = !thirdPerson;
		gCamera->setCameraType(thirdPerson);
		gCamera->setRatio(100.0f);
		
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		paused = !paused;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		 Reload();
		 winc = false;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		aim = !aim;
	}


}

void AppMain() {
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw runtime_error("glfwInit failed");
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor* monitor = NULL;

	if (FULL_SCREEN)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		SCREEN_SIZE.x = mode->width;
		SCREEN_SIZE.y = mode->height;
	}
	else
	{
		SCREEN_SIZE.x = WINDOW_WIDTH;
		SCREEN_SIZE.y = WINDOW_HEIGHT;
	}

	gWindow = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "Dead Trip", monitor, NULL);

    if(!gWindow)
        throw runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);
    glfwSetScrollCallback(gWindow, OnScroll);
	glfwSetMouseButtonCallback(gWindow, OnButton);
	glfwSetKeyCallback(gWindow, OnKey);
    glfwMakeContextCurrent(gWindow);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        throw runtime_error("glewInit failed");
    
    while(glGetError() != GL_NO_ERROR) {}

    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;

    if(!GLEW_VERSION_3_2)
        throw runtime_error("OpenGL 3.2 API is not available.");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (MULTITHREAD)
	{
		std::thread loadThread(Load);
		if (loadThread.joinable()) loadThread.join();

		//std::future<void> result(std::async(Load));
		//result.wait();
	}
	else
	{
		Load();
	}

    double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(gWindow)){
        glfwPollEvents();
        
		if (!paused)
		{
			double thisTime = glfwGetTime();
			Update((float)(thisTime - lastTime));
			lastTime = thisTime;
		}
     
        Render();

        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            cerr << "OpenGL Error " << error << endl;

        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    glfwTerminate();
}

int main(int argc, char *argv[]) {
    try {
        AppMain();
    } catch (const exception& e){
        cerr << "ERROR: " << e.what() << endl;
		
		MessageBox(
			NULL,
			(LPCSTR)e.what(),
			(LPCSTR)"Error",
			MB_ICONWARNING | MB_OK | MB_DEFBUTTON1
		);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
