//#include "F:/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
//#include "F:/home/codeleaded/System/Static/Library/Random.h"
//#include "F:/home/codeleaded/System/Static/Library/RayCast.h"
//#include "F:/home/codeleaded/System/Static/Library/Thread.h"
//#include "F:/home/codeleaded/System/Static/Library/ImageFilter.h"

#include "/home/codeleaded/System/Static/Library/WindowEngineGL1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/RayCast.h"
#include "/home/codeleaded/System/Static/Library/Thread.h"

#include "./Math3D.h"
#include "./Hitbox3D.h"
#include "./World.h"

typedef struct Camera {
	mat4x4 matProj;
	vec3d vCamera;
	vec3d vVelocity;
	vec3d vLookDir;
	float fYaw;	
	float fPitch;
	vec3d vLength;
} Camera;

Camera Camera_New(vec3d p){
	Camera c;
	c.matProj = Matrix_MakeProjection(90.0f,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f);
	c.vCamera = p;
	c.vVelocity = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.vLookDir = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	c.fYaw = 0.0f;
	c.fPitch = 0.0f;
	c.vLength = (vec3d){ 0.0f,0.0f,0.0f,1.0f };
	return c;
}



Vector* cubeSidesSelected;
Vector cubeSides1;
Vector cubeSides2;

vec3d vCamera = { 10.0f,100.0f,10.0f,1.0f };
vec3d vLength = { 0.5f,1.8f,0.5f,1.0f };
vec3d vVelocity = { 0.0f,0.0f,0.0f,1.0f };
vec3d vLookDir;

float fYaw;	
float fPitch;	
float fTheta;

Vector Cubes;
Vec2 MouseBefore = { 0.0f,0.0f };

char OnGround = 0;
int Mode = 0;
int Menu = 0;


mat4x4 model;
mat4x4 view;
mat4x4 proj;


GLuint VBO, VAO;
GLuint shaderProgram;
GLuint texture1;

char updated;
Thread worker;
World map;

vec2d getUVsFromIndex(int tileIndex) {
    int col = tileIndex % ATLAS_COLS;
    int row = tileIndex / ATLAS_COLS;

    float u0 = (float)col * TILE_WIDTH;
    float v0 = (float)row * TILE_HEIGHT;

    return (vec2d){ u0,v0 };
}
void checkCompileErrors(GLuint shader, const char* type) {
    GLint success;
    GLchar infoLog[512];
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            printf("ERROR: Shader Compilation [%s]\n%s\n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            printf("ERROR: Program Linking\n%s\n", infoLog);
        }
    }
}
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}
unsigned int createShaderProgram() {
    char* vertexShaderSource = Files_Read("./src/vertexShader.glsl");
    if(!vertexShaderSource){
        printf("[vertexShader]: Error\n");
        window.w.Running = 0;
        return 0;
    }
    
    char* fragmentShaderSource = Files_Read("./src/fragmentShader.glsl");
    if(!fragmentShaderSource){
        printf("[fragmentShader]: Error\n");
        window.w.Running = 0;
        return 0;
    }
    
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	checkCompileErrors(fragment, "FRAGMENT");

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);
	checkCompileErrors(program, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

	if(vertexShaderSource)      free(vertexShaderSource);
    if(fragmentShaderSource)    free(fragmentShaderSource);

    return program;
}
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Setze Texturparameter (wie Wrapping und Filter)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Lade Textur (mit einer Bibliothek wie stb_image oder SOIL)
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Error: Texture\n");
    }
    stbi_image_free(data);
    return textureID;
}

int Cubes_Compare(const void* e1,const void* e2) {
	Rect3 r1 = *(Rect3*)e1;
	Rect3 r2 = *(Rect3*)e2;
	
	vec3d pos = vec3d_Add(vCamera,(vec3d){ vLength.x * 0.5f,vLength.y * 0.9f,vLength.z * 0.5f });
	vec3d d1 = vec3d_Sub(r1.p,pos);
    vec3d d2 = vec3d_Sub(r2.p,pos);
	return vec3d_Length(d1) == vec3d_Length(d2) ? 0 : (vec3d_Length(d1) < vec3d_Length(d2) ? 1 : -1);
}
void Cubes_Reload(World* map){
	Vector_Clear(&Cubes);

	vec3d f = { (int)vCamera.x,(int)vCamera.y,(int)vCamera.z };
	for(int i = -2;i<2;i++){
		for(int j = -2;j<2;j++){
			for(int k = -2;k<2;k++){
				vec3d n = { k,j,i };
				vec3d r = vec3d_Add(f,n);

				Block b = World_Get(map,r.x,r.y,r.z);

				if(b!=BLOCK_VOID && b!=BLOCK_ERROR){
					Vector_Push(&Cubes,(Rect3[]){ { r,(vec3d){ 1.0f,1.0f,1.0f } } });
				}
			}
		}
	}

	qsort(Cubes.Memory,Cubes.size,Cubes.ELEMENT_SIZE,Cubes_Compare);
}

void Stand(vec3d* Data){
	Data->y = 0.0f;
	OnGround = 1;
}
void Jump(vec3d* Data){
	Data->y = 0.0f;
}


void Camera_Move(float Time){
	Vec2 v = { vVelocity.x,vVelocity.z };
	Vec2 d = Vec2_Norm(v);

	float drag = OnGround ? 12.0f : 10.0f;
	Vec2 da = Vec2_Norm(v);
	v = Vec2_Sub(v,Vec2_Mulf(d,drag * Time));

	if(F32_Sign(v.x)!=F32_Sign(da.x) || F32_Sign(v.y)!=F32_Sign(da.y)){
		v.x = 0.0f;
		v.y = 0.0f;
	}

	float maxVelo = OnGround ? 4.0f : 6.0f;
	if(Vec2_Mag(v)>maxVelo){
		v = Vec2_Mulf(d,maxVelo);
	}

	vVelocity.x = v.x;
	vVelocity.z = v.y;

	vVelocity = vec3d_Add(vVelocity,vec3d_Mul((vec3d){ 0.0f,0.0f,0.0f,1.0f },Time));
	vCamera = vec3d_Add(vCamera,vec3d_Mul(vVelocity,Time));
}
void Camera_Collision(){
	Cubes_Reload(&map);
	OnGround = 0;
	for(int i = 0;i<Cubes.size;i++){
		vec3d pos = { -vLength.x * 0.5f,vLength.y * 0.6f,-vLength.z * 0.5f };

		Rect3 r1 = *(Rect3*)Vector_Get(&Cubes,i);
		Rect3 r2 = (Rect3){ vec3d_Sub(vCamera,pos),vLength };
		Rect3_Static(&r2,r1,&vVelocity,(void (*[])(void*)){ NULL,NULL,NULL,NULL,(void*)Jump,(void*)Stand });
		vCamera = vec3d_Add(r2.p,pos);
	}
}

void Menu_Set(int m){
	if(Menu==0 && m==1){
		AlxWindow_Mouse_SetInvisible(&window.w);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	if(Menu==1 && m==0){
		AlxWindow_Mouse_SetVisible(&window.w);
	}
	
	MouseBefore = GetMouse();
	Menu = m;
}

void GL_Init(){
	model = Matrix_MakeTranslation(0.0f,0.0f,0.0f);
    view = Matrix_MakeIdentity();
    proj = Matrix_MakeProjection(90.0f,(float)GetHeight() / (float)GetWidth(),0.1f,200.0f);
    
    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        // Vorderseite (Front)
         0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f, 0.0f,   
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 1.0f,    
         0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 1.0f,   
        -0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f, 0.0f,  
        -0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f, 1.0f, 

        // Westseite (Left)
        -0.5f, -0.5f,  0.5f,     -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f,     -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,     
        -0.5f,  0.5f,  0.5f,     -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   
        -0.5f,  0.5f,  0.5f,     -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,  
        -0.5f, -0.5f, -0.5f,     -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,    
        -0.5f,  0.5f, -0.5f,     -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,    
    
        // Rückseite (Back)
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,   
         0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 0.0f,   
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,   
        -0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,   
         0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,   
        -0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,  
    
        // Ostseite (Right) - korrigierte Texturkoordinaten
         0.5f, -0.5f, -0.5f,      1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   
         0.5f, -0.5f,  0.5f,      1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   
         0.5f,  0.5f,  0.5f,      1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   
         0.5f, -0.5f, -0.5f,      1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   
         0.5f,  0.5f,  0.5f,      1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   
         0.5f,  0.5f, -0.5f,      1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
           
       // Oben (Top) - korrigierte Texturkoordinaten 
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,   1.0f, 0.0f,  
       -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   
        0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   
        0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,   1.0f, 1.0f,  
       -0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,   0.0f, 0.0f,    
       -0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 

       // Südseite (Bottom)
       -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,   0.0f, 0.0f,   
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,   1.0f, 0.0f,   
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,   1.0f, 1.0f,   
       -0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,   0.0f, 0.0f,   
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,   1.0f, 1.0f,   
       -0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,   0.0f, 1.0f, 
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Positions-Attribut binden
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normale-Attribut binden
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texturkoordinaten binden
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    stbi_set_flip_vertically_on_load(1);
    int width,height,n;
    unsigned char* data = stbi_load("./assets/Old_Atlas.png", &width, &height, &n, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, (n==3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        stbi_image_free(data);
    }

    glUseProgram(shaderProgram);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);         // Rückseiten cullen (Standard)
    glFrontFace(GL_CCW);         // Dreiecke mit Gegenuhrzeigersinn gelten als Vorderseite

    glUniform1i(glGetUniformLocation(shaderProgram,"texture1"),0);
    glUniform2f(glGetUniformLocation(shaderProgram,"uvScale"),TILE_WIDTH,TILE_HEIGHT);
}

void* Worker_Thread(void* arg){
	while(window.w.Running){
		if(cubeSidesSelected==&cubeSides1){
			if(World_Update(&map,&cubeSides2,"./data",vCamera.x,vCamera.z)){
				cubeSidesSelected = &cubeSides2;
			}else if(updated){
				updated = 0;
				World_Mesh(&map,&cubeSides2);
				cubeSidesSelected = &cubeSides2;
			}
		}else{
			if(World_Update(&map,&cubeSides1,"./data",vCamera.x,vCamera.z)){
				cubeSidesSelected = &cubeSides1;
			}else if(updated){
				updated = 0;
				World_Mesh(&map,&cubeSides1);
				cubeSidesSelected = &cubeSides1;
			}
		}

		Thread_Sleep_M(1);
	}
	return NULL;
}

void Setup(AglWindow* w){
	Menu_Set(1);

	RGA_Set(Time_Nano());
	RGA_Get(6969);

	cubeSides1 = Vector_New(sizeof(CubeSide));
	cubeSides2 = Vector_New(sizeof(CubeSide));
	cubeSidesSelected = &cubeSides1;

	Cubes = Vector_New(sizeof(Rect3));

	PerlinNoise_Permutations_Init();

	map = World_New();
	World_Generate(&map);
	World_Mesh(&map,cubeSidesSelected);

	GL_Init();

	updated = 0;
	worker = Thread_New(NULL,Worker_Thread,NULL);
	Thread_Start(&worker);
}
void Update(AglWindow* w){
	if(Menu==1){
		if(GetMouse().x!=MouseBefore.x || GetMouse().y!=MouseBefore.y){
			Vec2 d = Vec2_Sub(GetMouse(),MouseBefore);
			Vec2 a = Vec2_Mulf(Vec2_Div(d,(Vec2){ window.w.Width,window.w.Height }),2 * F64_PI);
	
			fYaw -= a.x;
			fPitch += a.y;
	
			SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
			MouseBefore = GetMouse();
		}
	}
	
	if(Stroke(ALX_KEY_ESC).PRESSED)
		Menu_Set(!Menu);

	if(Stroke(ALX_KEY_UP).DOWN)
		fPitch -= 2.0f * w->w.ElapsedTime;

	if(Stroke(ALX_KEY_DOWN).DOWN)
		fPitch += 2.0f * w->w.ElapsedTime;

	if(Stroke(ALX_KEY_LEFT).DOWN)
		fYaw -= 2.0f * w->w.ElapsedTime;

	if(Stroke(ALX_KEY_RIGHT).DOWN)
		fYaw += 2.0f * w->w.ElapsedTime;

	if(Stroke(ALX_KEY_Z).PRESSED)
		Mode = Mode < 2 ? Mode+1 : 0;

	if(Stroke(ALX_KEY_R).DOWN)
		//if(OnGround) 
			vVelocity.y = 5.0f;
	
	if(Stroke(ALX_KEY_F).DOWN)
		vVelocity.y = -5.0f;

	if(Stroke(ALX_KEY_R).RELEASED || Stroke(ALX_KEY_F).RELEASED)
		vVelocity.y = 0.0f;

	mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
	vec3d vForward = Matrix_MultiplyVector(matCameraRot,vec3d_new(0.0f,0.0f,1.0f));
	vec3d vLeft = vec3d_Perp(vForward);
	
	if(Stroke(ALX_KEY_W).DOWN){
		vVelocity.x += vForward.x * 20.0f * w->w.ElapsedTime;
		vVelocity.z += vForward.z * 20.0f * w->w.ElapsedTime;
	}
	if(Stroke(ALX_KEY_S).DOWN){
		vVelocity.x -= vForward.x * 20.0f * w->w.ElapsedTime;
		vVelocity.z -= vForward.z * 20.0f * w->w.ElapsedTime;
	}
	if(Stroke(ALX_KEY_A).DOWN){
		vVelocity.x += vLeft.x * 20.0f * w->w.ElapsedTime;
		vVelocity.z += vLeft.z * 20.0f * w->w.ElapsedTime;
	}
	if (Stroke(ALX_KEY_D).DOWN){
		vVelocity.x -= vLeft.x * 20.0f * w->w.ElapsedTime;
		vVelocity.z -= vLeft.z * 20.0f * w->w.ElapsedTime;
	}

	Camera_Move((float)w->w.ElapsedTime);
	Camera_Collision();
	
	float Border = F32_PI * 0.5f - 0.001f;
	if(fPitch<-Border) fPitch = -Border;
	if(fPitch>Border) fPitch = Border;

	vec3d vUp = vec3d_new( 0.0f,1.0f,0.0f );
	vec3d vTarget = vec3d_new( 0.0f,0.0f,1.0f );
	mat4x4 matCameraRotX = Matrix_MakeRotationX(fPitch);
	vLookDir = Matrix_MultiplyVector(matCameraRotX,vTarget);
	vLookDir = Matrix_MultiplyVector(matCameraRot,vLookDir);
	
	vTarget = vec3d_Add(vCamera, vLookDir);
	mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);
	view = Matrix_QuickInverse(matCamera);

	if(Stroke(ALX_MOUSE_L).PRESSED){
		Vec3 c = (Vec3){ vCamera.x,vCamera.y,vCamera.z };
		RayCast_TileMap(&map,(void*)World_Void,c,(Vec3){ vLookDir.x,vLookDir.y,vLookDir.z },0.01f,4.0f,&c);
		
		World_Set(&map,c.x,c.y,c.z,BLOCK_VOID);
		updated = 1;
		//World_Edit(&map,&meshSelected->tris,(vec3d){ c.x,c.y,c.z },BLOCK_VOID);
	}
	if(Stroke(ALX_MOUSE_R).PRESSED){
		Vec3 c = (Vec3){ vCamera.x,vCamera.y + vLength.y * 0.6f,vCamera.z };
		RayCast_TileMap_N(&map,(void*)World_Void,c,(Vec3){ vLookDir.x,vLookDir.y,vLookDir.z },0.01f,4.0f,&c);
		
		vec3d pos = { -vLength.x * 0.5f,vLength.y * 0.6f,-vLength.z * 0.5f };
		Rect3 pr = { { c.x,c.y,c.z },{ 1.0f,1.0f,1.0f } };
		Rect3 br = { vec3d_Sub(vCamera,pos),vLength };
		if(!Rect3_Overlap(pr,br)){
			World_Set(&map,c.x,c.y,c.z,BLOCK_WATER);
			updated = 1;
			//World_Edit(&map,&meshSelected->tris,(vec3d){ c.x,c.y,c.z },BLOCK_TORCH);
		}
	}
	
	if(Mode==1){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

	glViewport(0, 0, GetWidth(), GetHeight());

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,(const GLfloat*)&model);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,(const GLfloat*)&view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,(const GLfloat*)&proj);

    glBindVertexArray(VAO);

    for (unsigned int i = 0; i < cubeSidesSelected->size; i++){
        CubeSide* cs = (CubeSide*)Vector_Get(cubeSidesSelected,i);
        
		//printf("CS: %f %f %f, %d / %d\n",cs->pos.x,cs->pos.y,cs->pos.z,cs->id,cs->side);

        mat4x4 mat = Matrix_MultiplyMatrix(
            model,
            Matrix_MakeTranslation(cs->pos.x,cs->pos.y,cs->pos.z)
            // Matrix_MultiplyMatrix(
            //     Matrix_MakeTranslation(cubePositions[i].x,cubePositions[i].y,cubePositions[i].z),
            //     Matrix_MultiplyMatrix(
            //         Matrix_MakeRotationX(angle),
            //         Matrix_MakeRotationY(angle * 0.66f)
            //     )
            // )
        );

        vec2d atlaspos = getUVsFromIndex(Block_Id(cs->id,cs->side));
		//printf("AP: %f %f\n",atlaspos.u,atlaspos.v);
		
        glUniform2f(glGetUniformLocation(shaderProgram,"shade"),Block_ShadeSide(cs->side),(float)cs->light / (float)LIGHT_MAX);
        glUniform2f(glGetUniformLocation(shaderProgram,"uvOffset"),atlaspos.u,atlaspos.v);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,(const GLfloat*)&mat);
        
        int offset = cs->side * 6;
        glDrawArrays(GL_TRIANGLES, offset, 6);
    }
}
void Delete(AglWindow* w){
	World_Free(&map);

	Vector_Free(&cubeSides1);
	Vector_Free(&cubeSides2);
	Vector_Free(&Cubes);

	glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteProgram(shaderProgram);

	AlxWindow_Mouse_SetVisible(&window.w);
}

int main(){
    if(Create("Minecraft3D Tex GL",2500,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}