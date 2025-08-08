//#include "F:/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
//#include "F:/home/codeleaded/System/Static/Library/Random.h"
//#include "F:/home/codeleaded/System/Static/Library/RayCast.h"
//#include "F:/home/codeleaded/System/Static/Library/Thread.h"
//#include "F:/home/codeleaded/System/Static/Library/ImageFilter.h"

#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/RayCast.h"
#include "/home/codeleaded/System/Static/Library/Thread.h"
#include "/home/codeleaded/System/Static/Library/ImageFilter.h"

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


mesh* meshSelected;
mesh meshCube1;
mesh meshCube2;

mat4x4 matProj;
vec3d vCamera = { 10.0f,100.0f,10.0f,1.0f };
vec3d vVelocity = { 0.0f,0.0f,0.0f,1.0f };
vec3d vLookDir;
float fYaw;	
float fPitch;	
float fTheta;

vec3d vLength = { 0.5f,1.8f,0.5f,1.0f };
Vector Cubes;
Vec2 MouseBefore = { 0.0f,0.0f };

mat4x4 matView;

char OnGround = 0;
int Mode = 0;
int Menu = 0;

char updated;
Thread worker;
World map;


void BuildCube(vec3d p,vec3d d,Pixel c){
	Vector_Push(&Cubes,(Rect3[]){ { p,d } });
	MakeCube(&meshSelected->tris,p,d,c);
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

int compare(const void* e1,const void* e2) {
	triangle t1 = *(triangle*)e1;
	triangle t2 = *(triangle*)e2;
	float z1 = (t1.p[0].z+t1.p[1].z+t1.p[2].z)/3;
    float z2 = (t2.p[0].z+t2.p[1].z+t2.p[2].z)/3;
    return z1 == z2 ? 0 : (z1 < z2 ? 1 : -1);
}

void Triangles_Project(){
	Vector vecTrianglesToRaster = Vector_New(sizeof(triangle));

	for (int i = 0;i<meshSelected->tris.size;i++){
		triangle tri = *(triangle*)Vector_Get(&meshSelected->tris,i);
		
		vec3d vCameraRay = vec3d_Sub(tri.p[0], vCamera);

		if (vec3d_DotProduct(tri.n,vCameraRay) < 0.0f){
			tri.p[0] = Matrix_MultiplyVector(matView,tri.p[0]);
			tri.p[1] = Matrix_MultiplyVector(matView,tri.p[1]);
			tri.p[2] = Matrix_MultiplyVector(matView,tri.p[2]);

			int nClippedTriangles = 0;
			triangle clipped[2];
			nClippedTriangles = Triangle_ClipAgainstPlane(vec3d_new(0.0f,0.0f,0.1f),vec3d_new(0.0f,0.0f,1.0f),tri,&clipped[0],&clipped[1]);

			for (int n = 0; n < nClippedTriangles; n++){
				clipped[n].p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
				clipped[n].p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
				clipped[n].p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);

				clipped[n].p[0] = vec3d_Div(clipped[n].p[0], clipped[n].p[0].w);
				clipped[n].p[1] = vec3d_Div(clipped[n].p[1], clipped[n].p[1].w);
				clipped[n].p[2] = vec3d_Div(clipped[n].p[2], clipped[n].p[2].w);

				clipped[n].p[0].x *= -1.0f;
				clipped[n].p[1].x *= -1.0f;
				clipped[n].p[2].x *= -1.0f;
				clipped[n].p[0].y *= -1.0f;
				clipped[n].p[1].y *= -1.0f;
				clipped[n].p[2].y *= -1.0f;

				vec3d vOffsetView = vec3d_new( 1,1,0 );
				clipped[n].p[0] = vec3d_Add(clipped[n].p[0], vOffsetView);
				clipped[n].p[1] = vec3d_Add(clipped[n].p[1], vOffsetView);
				clipped[n].p[2] = vec3d_Add(clipped[n].p[2], vOffsetView);
				clipped[n].p[0].x *= 0.5f * (float)GetWidth();
				clipped[n].p[0].y *= 0.5f * (float)GetHeight();
				clipped[n].p[1].x *= 0.5f * (float)GetWidth();
				clipped[n].p[1].y *= 0.5f * (float)GetHeight();
				clipped[n].p[2].x *= 0.5f * (float)GetWidth();
				clipped[n].p[2].y *= 0.5f * (float)GetHeight();

				Vector_Push(&vecTrianglesToRaster,&clipped[n]);
			}			
		}
	}

	qsort(vecTrianglesToRaster.Memory,vecTrianglesToRaster.size,vecTrianglesToRaster.ELEMENT_SIZE,compare);

	for (int i = 0;i<vecTrianglesToRaster.size;i++)
	{
		triangle triToRaster = *(triangle*)Vector_Get(&vecTrianglesToRaster,i);

		triangle clipped[2];
		Vector listTriangles = Vector_New(sizeof(triangle));

		Vector_Push(&listTriangles,&triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				triangle test = *(triangle*)Vector_Get(&listTriangles,0);
				Vector_Remove(&listTriangles,0);
				nNewTriangles--;

				switch (p)
				{
				case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(vec3d_new( 0.0f, 0.0f, 0.0f ), 					vec3d_new( 0.0f, 1.0f, 0.0f ), 	test, &clipped[0], &clipped[1]); break;
				case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(vec3d_new( 0.0f, (float)GetHeight() - 1, 0.0f ), vec3d_new( 0.0f, -1.0f, 0.0f ), test, &clipped[0], &clipped[1]); break;
				case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(vec3d_new( 0.0f, 0.0f, 0.0f ), 					vec3d_new( 1.0f, 0.0f, 0.0f ), 	test, &clipped[0], &clipped[1]); break;
				case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(vec3d_new( (float)GetWidth() - 1, 0.0f, 0.0f ), 	vec3d_new( -1.0f, 0.0f, 0.0f ), test, &clipped[0], &clipped[1]); break;
				}

				for (int w = 0; w < nTrisToAdd; w++)
					Vector_Push(&listTriangles,&clipped[w]);
			}
			nNewTriangles = listTriangles.size;
		}

		for (int j = 0;j<listTriangles.size;j++){
			triangle t = *(triangle*)Vector_Get(&listTriangles,j);
			//RenderTriangle(((Vec2){ t.p[0].x, t.p[0].y }),((Vec2){ t.p[1].x, t.p[1].y }),((Vec2){ t.p[2].x, t.p[2].y }),t.c);
			//RenderTriangleWire(((Vec2){ t.p[0].x, t.p[0].y }),((Vec2){ t.p[1].x, t.p[1].y }),((Vec2){ t.p[2].x, t.p[2].y }),WHITE,1.0f);

			if(Mode==0) RenderTriangle(((Vec2){ t.p[0].x, t.p[0].y }),((Vec2){ t.p[1].x, t.p[1].y }),((Vec2){ t.p[2].x, t.p[2].y }),t.c);
			if(Mode==1) RenderTriangleWire(((Vec2){ t.p[0].x, t.p[0].y }),((Vec2){ t.p[1].x, t.p[1].y }),((Vec2){ t.p[2].x, t.p[2].y }),t.c,1.0f);
			if(Mode==2){
				RenderTriangle(((Vec2){ t.p[0].x, t.p[0].y }),((Vec2){ t.p[1].x, t.p[1].y }),((Vec2){ t.p[2].x, t.p[2].y }),t.c);
				RenderTriangleWire(((Vec2){ t.p[0].x, t.p[0].y }),((Vec2){ t.p[1].x, t.p[1].y }),((Vec2){ t.p[2].x, t.p[2].y }),WHITE,1.0f);
			}
		}

		Vector_Free(&listTriangles);
	}
	Vector_Free(&vecTrianglesToRaster);
}

void Stand(vec3d* Data){
	Data->y = 0.0f;
	OnGround = 1;
}
void Jump(vec3d* Data){
	Data->y = 0.0f;
}

void Menu_Set(int m){
	if(Menu==0 && m==1){
		AlxWindow_Mouse_SetInvisible(&window);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	if(Menu==1 && m==0){
		AlxWindow_Mouse_SetVisible(&window);
	}
	
	MouseBefore = GetMouse();
	Menu = m;
}



void* Worker_Thread(void* arg){
	while(window.Running){
		if(meshSelected==&meshCube1){
			if(World_Update(&map,&meshCube2.tris,"../data",vCamera.x,vCamera.z)){
				meshSelected = &meshCube2;
			}else if(updated){
				updated = 0;
				World_Mesh(&map,&meshCube2.tris);
				meshSelected = &meshCube2;
			}
		}else{
			if(World_Update(&map,&meshCube1.tris,"../data",vCamera.x,vCamera.z)){
				meshSelected = &meshCube1;
			}else if(updated){
				updated = 0;
				World_Mesh(&map,&meshCube1.tris);
				meshSelected = &meshCube1;
			}
		}

		Thread_Sleep_M(1);
	}
	return NULL;
}

void Setup(AlxWindow* w){
	Menu_Set(1);

	RGA_Set(Time_Nano());
	RGA_Get(6969);

	meshCube1 = (mesh){ Vector_New(sizeof(triangle)) };
	meshCube2 = (mesh){ Vector_New(sizeof(triangle)) };
	meshSelected = &meshCube1;
	Cubes = Vector_New(sizeof(Rect3));

	matProj = Matrix_MakeProjection(90.0f, (float)GetHeight() / (float)GetWidth(), 0.1f, 1000.0f);

	PerlinNoise_Permutations_Init();

	map = World_New();
	World_Generate(&map);
	World_Mesh(&map,&meshSelected->tris);

	updated = 0;
	worker = Thread_New(NULL,Worker_Thread,NULL);
	Thread_Start(&worker);
}

void Update(AlxWindow* w){
	//w->ElapsedTime = 0.05;
	
	/*if(Stroke(ALX_MOUSE_L).PRESSED){
		MouseBefore = GetMouse();
	}
	if(Stroke(ALX_MOUSE_L).DOWN){
		if(GetMouse().x!=MouseBefore.x || GetMouse().y!=MouseBefore.y){
			Vec2 d = Vec2_Sub(GetMouse(),MouseBefore);
			Vec2 a = Vec2_Mulf(Vec2_Div(d,(Vec2){ window.Width,window.Height }),2 * PI);
	
			fYaw += a.x;
			fPitch += a.y;
	
			MouseBefore = GetMouse();
		}
	}*/
	if(Menu==1){
		if(GetMouse().x!=MouseBefore.x || GetMouse().y!=MouseBefore.y){
			Vec2 d = Vec2_Sub(GetMouse(),MouseBefore);
			Vec2 a = Vec2_Mulf(Vec2_Div(d,(Vec2){ window.Width,window.Height }),2 * F64_PI);
	
			fYaw += a.x;
			fPitch += a.y;
	
			SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
			MouseBefore = GetMouse();
		}
	}
	
	if(Stroke(ALX_KEY_ESC).PRESSED)
		Menu_Set(!Menu);

	if(Stroke(ALX_KEY_UP).DOWN)
		fPitch -= 2.0f * w->ElapsedTime;

	if(Stroke(ALX_KEY_DOWN).DOWN)
		fPitch += 2.0f * w->ElapsedTime;

	if(Stroke(ALX_KEY_LEFT).DOWN)
		fYaw -= 2.0f * w->ElapsedTime;

	if(Stroke(ALX_KEY_RIGHT).DOWN)
		fYaw += 2.0f * w->ElapsedTime;

	if(Stroke(ALX_KEY_Z).PRESSED)
		Mode = Mode < 3 ? Mode+1 : 0;

	if(Stroke(ALX_KEY_R).DOWN)
		if(OnGround) 
			vVelocity.y = 5.0f;
	
	//if(Stroke(ALX_KEY_F).DOWN)
	//	vVelocity.y = -5.0f;

	//if(Stroke(ALX_KEY_R).RELEASED || Stroke(ALX_KEY_F).RELEASED)
	//	vVelocity.y = 0.0f;

	mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
	vec3d vForward = Matrix_MultiplyVector(matCameraRot,vec3d_new(0.0f,0.0f,1.0f));
	vec3d vLeft = vec3d_Perp(vForward);
	
	if(Stroke(ALX_KEY_W).DOWN){
		vVelocity.x += vForward.x * 20.0f * w->ElapsedTime;
		vVelocity.z += vForward.z * 20.0f * w->ElapsedTime;
	}
	if(Stroke(ALX_KEY_S).DOWN){
		vVelocity.x -= vForward.x * 20.0f * w->ElapsedTime;
		vVelocity.z -= vForward.z * 20.0f * w->ElapsedTime;
	}
	if(Stroke(ALX_KEY_A).DOWN){
		vVelocity.x -= vLeft.x * 20.0f * w->ElapsedTime;
		vVelocity.z -= vLeft.z * 20.0f * w->ElapsedTime;
	}
	if (Stroke(ALX_KEY_D).DOWN){
		vVelocity.x += vLeft.x * 20.0f * w->ElapsedTime;
		vVelocity.z += vLeft.z * 20.0f * w->ElapsedTime;
	}

	Vec2 v = { vVelocity.x,vVelocity.z };
	Vec2 d = Vec2_Norm(v);

	float drag = OnGround ? 12.0f : 10.0f;
	Vec2 da = Vec2_Norm(v);
	v = Vec2_Sub(v,Vec2_Mulf(d,drag * w->ElapsedTime));

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

	vVelocity = vec3d_Add(vVelocity,vec3d_Mul((vec3d){ 0.0f,-10.0f,0.0f,1.0f },w->ElapsedTime));
	vCamera = vec3d_Add(vCamera,vec3d_Mul(vVelocity,w->ElapsedTime));

	Cubes_Reload(&map);
	OnGround = 0;
	for(int i = 0;i<Cubes.size;i++){
		vec3d pos = { vLength.x * 0.5f,vLength.y * 0.9f,vLength.z * 0.5f };

		Rect3 r1 = *(Rect3*)Vector_Get(&Cubes,i);
		Rect3 r2 = (Rect3){ vec3d_Sub(vCamera,pos),vLength };
		Rect3_Static(&r2,r1,&vVelocity,(void (*[])(void*)){ NULL,NULL,NULL,NULL,(void*)Jump,(void*)Stand });
		vCamera = vec3d_Add(r2.p,pos);
	}

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
	matView = Matrix_QuickInverse(matCamera);

	if(Stroke(ALX_MOUSE_L).PRESSED){
		Vec3 c = (Vec3){ vCamera.x,vCamera.y,vCamera.z };
		RayCast_TileMap(&map,(void*)World_Void,c,(Vec3){ vLookDir.x,vLookDir.y,vLookDir.z },0.01f,4.0f,&c);
		
		World_Set(&map,c.x,c.y,c.z,BLOCK_VOID);
		updated = 1;
		//World_Edit(&map,&meshSelected->tris,(vec3d){ c.x,c.y,c.z },BLOCK_VOID);
	}
	if(Stroke(ALX_MOUSE_R).PRESSED){
		Vec3 c = (Vec3){ vCamera.x,vCamera.y,vCamera.z };
		RayCast_TileMap_N(&map,(void*)World_Void,c,(Vec3){ vLookDir.x,vLookDir.y,vLookDir.z },0.01f,4.0f,&c);
		
		vec3d pos = { vLength.x * 0.5f,vLength.y * 0.9f,vLength.z * 0.5f };
		Rect3 pr = { { c.x,c.y,c.z },{ 1.0f,1.0f,1.0f } };
		Rect3 br = { vec3d_Sub(vCamera,pos),vLength };
		if(!Rect3_Overlap(pr,br)){
			World_Set(&map,c.x,c.y,c.z,BLOCK_TORCH);
			updated = 1;
			//World_Edit(&map,&meshSelected->tris,(vec3d){ c.x,c.y,c.z },BLOCK_TORCH);
		}
	}

	Clear(LIGHT_BLUE);
	Triangles_Project();

	String str = String_Format("X: %f, Y: %f, Z: %f, Size: %d",vCamera.x,vCamera.y,vCamera.z,meshSelected->tris.size);
	RenderCStrSize(str.Memory,str.size,0,0,RED);
	String_Free(&str);
}

void Delete(AlxWindow* w){
	World_Free(&map);

	Vector_Free(&meshSelected->tris);
	Vector_Free(&Cubes);

	AlxWindow_Mouse_SetVisible(&window);
}

int main(){
    if(Create("Minecraft3D no Tex",2500,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}