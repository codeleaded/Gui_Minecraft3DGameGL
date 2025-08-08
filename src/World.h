#ifndef WORLD_H
#define WORLD_H

#include "/home/codeleaded/System/Static/Container/Vector.h"
#include "/home/codeleaded/System/Static/Library/PerlinNoise.h"
#include "/home/codeleaded/System/Static/Library/Files.h"

typedef unsigned char Light;
#define LIGHT_MIN	0
#define LIGHT_MAX	15
#define LIGHT_STEP	1

#define LIGHT_SHADE_X	1.0f
#define LIGHT_SHADE_Y	0.8f
#define LIGHT_SHADE_Z	0.6f

Light Light_Step(vec3d p,vec3d n,Light l){
	if(l<=LIGHT_MIN) return LIGHT_MIN;

	Light nl = l - LIGHT_STEP;
	//Light nl = l - I32_max(l / 6,1);
	//Light nl = l - vec3d_Length(vec3d_Sub(p,n));
	if(nl<LIGHT_MIN) 	return LIGHT_MIN;
	else 				return nl;
}


typedef unsigned char Block;

#define BLOCK_ERROR	255

#define BLOCK_VOID	0
#define BLOCK_DIRT	1
#define BLOCK_STONE	2
#define BLOCK_GRAS	3
#define BLOCK_LEAF	4
#define BLOCK_LOG	5
#define BLOCK_TORCH	6

float Block_ShadeSide(int s){
	switch(s){
		case CUBE_SIDE_SOUTH:	return LIGHT_SHADE_Z;
		case CUBE_SIDE_EAST:	return LIGHT_SHADE_X;
		case CUBE_SIDE_NORTH:	return LIGHT_SHADE_Z;
		case CUBE_SIDE_WEST:	return LIGHT_SHADE_X;
		case CUBE_SIDE_TOP:		return LIGHT_SHADE_Y;
		case CUBE_SIDE_BOTTOM:	return LIGHT_SHADE_Y;
	}
	return 0.0f;
}
char Block_Solid(Block b){
	switch (b){
	case BLOCK_DIRT: 	return 1;
	case BLOCK_STONE: 	return 1;
	case BLOCK_GRAS:	return 1;
	case BLOCK_LEAF: 	return 1;
	case BLOCK_LOG: 	return 1;
	}
	return 0;
}
Pixel Block_Pixel(Block b,int s){
	switch (b){
	case BLOCK_VOID: 	return BLACK;
	case BLOCK_DIRT: 	return BROWN;
	case BLOCK_STONE: 	return GRAY;
	case BLOCK_GRAS:	return s==CUBE_SIDE_TOP ? GREEN : BROWN;
	case BLOCK_LEAF: 	return DARK_GREEN;
	case BLOCK_LOG: 	return DARK_BROWN;
	case BLOCK_TORCH: 	return LIGHT_YELLOW;
	}
	return BLACK;
}

triangle PlaneTris_Side(vec3d p,vec3d d,int Tri,Pixel c1,Pixel c2){
	triangle tris[12] = {
	// SOUTH
	{ 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c1 },
	{ 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c2 },
	// EAST                                                      
	{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c1 },
	{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f, 1.0f,	c2 },
	// NORTH                                                     
	{ 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c1 },
	{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c2 },
	// WEST                                                      
	{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c1 },
	{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c2 },
	// TOP                                                       
	{ 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c1 },
	{ 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c2 },
	// BOTTOM                                                    
	{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c1 },
	{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,	    0.0f, 0.0f, 0.0f, 1.0f,	c2 },
	};

	for(int j = 0;j<3;j++){
		tris[Tri].p[j] = vec3d_Add(p,vec3d_new(tris[Tri].p[j].x * d.x,tris[Tri].p[j].y * d.y,tris[Tri].p[j].z * d.z));
	}
	Triangle_CalcNorm(&tris[Tri]);

	float dp = Block_ShadeSide(Tri / 2);
	tris[Tri].c = Pixel_Mul(tris[Tri].c,Pixel_toRGBA(dp,dp,dp,1.0f));
	
	return tris[Tri];
}
void Cube_Set(triangle* trisout,vec3d p,vec3d d,Pixel c){
	for(int i = 0;i<12;i++){
		trisout[i] = PlaneTris_Side(p,d,i,c,c);
	}
}
void MakeCube(Vector* tris,vec3d p,vec3d d,Pixel c){
	triangle buff[12];
	Cube_Set(buff,p,d,c);

	for(int i = 0;i<12;i++){
		Vector_Push(tris,&buff[i]);
	}
}
void MakePlane(Vector* tris,vec3d p,vec3d d,int Plane,Pixel c){
	Vector_Push(tris,(triangle[]){ PlaneTris_Side(p,d,Plane * 2,c,c) });
	Vector_Push(tris,(triangle[]){ PlaneTris_Side(p,d,Plane * 2 + 1,c,c) });
}

#define CHUNK_LX	8
#define CHUNK_LY	100
#define CHUNK_LZ	8

typedef struct Chunk {
	Block b[CHUNK_LZ][CHUNK_LY][CHUNK_LX];
	Light l[CHUNK_LZ][CHUNK_LY][CHUNK_LX];
} Chunk;

Chunk Chunk_New(){
	Chunk c;
    memset(c.b,BLOCK_VOID,sizeof(c.b));
    memset(c.l,LIGHT_MIN,sizeof(c.l));
	return c;
}
int Chunk_WtoC(float a,int d){
	a /= d;
	return (int)floorf(a);
}
float Chunk_CtoW(int a,int d){
	//a -= o;
	a *= d;
	return (float)a;
}
Block Chunk_Get(Chunk* c,int x,int y,int z){
	if(x<0 || x>=CHUNK_LX) return BLOCK_ERROR;
    if(y<0 || y>=CHUNK_LY) return BLOCK_ERROR;
    if(z<0 || z>=CHUNK_LZ) return BLOCK_ERROR;
    return c->b[z][y][x];
}
void Chunk_Set(Chunk* c,int x,int y,int z,Block b){
	if(x<0 || x>=CHUNK_LX) return;
    if(y<0 || y>=CHUNK_LY) return;
    if(z<0 || z>=CHUNK_LZ) return;
    c->b[z][y][x] = b;
}
Light Chunk_Get_Light(Chunk* c,int x,int y,int z){
	if(x<0 || x>=CHUNK_LX) return LIGHT_MIN;
    if(y<0 || y>=CHUNK_LY) return LIGHT_MIN;
    if(z<0 || z>=CHUNK_LZ) return LIGHT_MIN;
    return c->l[z][y][x];
}
void Chunk_Set_Light(Chunk* c,int x,int y,int z,Light l){
	if(x<0 || x>=CHUNK_LX) return;
    if(y<0 || y>=CHUNK_LY) return;
    if(z<0 || z>=CHUNK_LZ) return;
    c->l[z][y][x] = l;
}

void Chunk_ClearLight(Chunk* c){
	for(int i = 0;i<CHUNK_LZ;i++){
		for(int j = 0;j<CHUNK_LX;j++){
			for(int py = CHUNK_LY - 1;py>=0;py--){
				Block b = Chunk_Get(c,j,py,i);
				
				if(b==BLOCK_TORCH)	Chunk_Set_Light(c,j,py,i,LIGHT_MAX);
				else 				Chunk_Set_Light(c,j,py,i,LIGHT_MIN);
			}
		}
	}
}
void Chunk_SunLight(Chunk* c){
	for(int i = 0;i<CHUNK_LZ;i++){
		for(int j = 0;j<CHUNK_LX;j++){
			for(int py = CHUNK_LY - 1;py>=0;py--){
				Block b = Chunk_Get(c,j,py,i);
				if(b!=BLOCK_VOID) break;
				Chunk_Set_Light(c,j,py,i,LIGHT_MAX);
			}
		}
	}
}
void Chunk_Tree(Chunk* c,int x,int y,int z){
	Chunk_Set(c,x,y,z,BLOCK_LOG);
	Chunk_Set(c,x,y+1,z,BLOCK_LOG);
	Chunk_Set(c,x,y+2,z,BLOCK_LOG);

	Chunk_Set(c,x,y+3,z,BLOCK_LEAF);

	Chunk_Set(c,x+1,y+3,z,BLOCK_LEAF);
	Chunk_Set(c,x+1,y+3,z+1,BLOCK_LEAF);
	Chunk_Set(c,x+1,y+3,z-1,BLOCK_LEAF);
	Chunk_Set(c,x-1,y+3,z,BLOCK_LEAF);
	Chunk_Set(c,x-1,y+3,z+1,BLOCK_LEAF);
	Chunk_Set(c,x-1,y+3,z-1,BLOCK_LEAF);
	Chunk_Set(c,x,y+3,z+1,BLOCK_LEAF);
	Chunk_Set(c,x,y+3,z-1,BLOCK_LEAF);

	Chunk_Set(c,x,y+4,z,BLOCK_LEAF);
	Chunk_Set(c,x+1,y+4,z,BLOCK_LEAF);
	Chunk_Set(c,x-1,y+4,z,BLOCK_LEAF);
	Chunk_Set(c,x,y+4,z+1,BLOCK_LEAF);
	Chunk_Set(c,x,y+4,z-1,BLOCK_LEAF);
}
void Chunk_Generate(Chunk* c,int x,int z){
	int s = CHUNK_LZ * CHUNK_LX;
	memset(c->b,0,sizeof(Block) * s * CHUNK_LY);

	int cx = Chunk_CtoW(x,CHUNK_LX);
	int cz = Chunk_CtoW(z,CHUNK_LZ);

	for(int i = 0;i<CHUNK_LZ;i++){
		for(int j = 0;j<CHUNK_LX;j++){
			float px = (float)(j + cx) / 20.0f;
			float py = (float)(i + cz) / 20.0f;
			
			PerlinNoise_Offset_Set(1);
			PerlinNoise_Persistance_Set(2.0);
			float hf = (PerlinNoise_2D_Get(px,py) + 1.0f) * 0.5f;

			int h = 0;
			int e = hf * ((float)CHUNK_LY * 0.5f) + 40;
			for(;h<e;h++){
				if(e-h>4) 	c->b[i][h][j] = BLOCK_STONE;
				else 		c->b[i][h][j] = BLOCK_DIRT;
			}
			if(h>=0 && h<CHUNK_LY) c->b[i][h][j] = BLOCK_GRAS;

			PerlinNoise_Offset_Set(1);
			PerlinNoise_Persistance_Set(8.0);
			hf = (PerlinNoise_2D_Get(px,py) + 1.0f) * 0.5f;
			if(hf<0.5f && Random_i64_MinMax(0,25)==0) Chunk_Tree(c,j,h+1,i);

			PerlinNoise_Offset_Set(100);
			PerlinNoise_Persistance_Set(2.0);
			px = (float)(j + cx) / 20.0f;
			py = (float)(i + cz) / 20.0f;
			for(int hcave = 4;hcave<40;hcave++){
				float d = (PerlinNoise_3D_Get(px,(float)F32_Abs(20 - hcave) / 10,py) + 1.0f) * 0.5f;
				if(d<0.3f) Chunk_Set(c,j,hcave,i,BLOCK_VOID);
			}
		}
	}

	Chunk_SunLight(c);
}
void Chunk_Mesh(Chunk* c,Vector* tris,int x,int z){
	int cx = x * CHUNK_LX;
	int cz = z * CHUNK_LZ;
	
	for(int i = 0;i<CHUNK_LZ;i++){
        for(int j = 0;j<CHUNK_LY;j++){
			for(int k = 0;k<CHUNK_LX;k++){
				Block b = Chunk_Get(c,k,j,i);

				if(b!=BLOCK_VOID){
					for(int s = 0;s<6;s++){
						vec3d p = { k,j,i,1.0f };
						vec3d n = vec3d_Add(p,Neighbour_Side(s));
						
						Block nb = Chunk_Get(c,n.x,n.y,n.z);
						if(nb==BLOCK_VOID || nb==BLOCK_ERROR){
							Pixel c = Block_Pixel(b,s);
							vec3d cp = { cx + k,j,cz + i,1.0f };
							MakePlane(tris,cp,(vec3d){ 1.0f,1.0f,1.0f },s,c);
						}
					}
				}
			}
		}
    }
}
Chunk Chunk_Load(char* path,int x,int z){
	String fp = String_Format("%s/Chunk_Z%d_X%d",path,(Number)z,(Number)x);
	CStr fullpath = String_CStr(&fp);
	String_Free(&fp);

	char* data = Files_Read(fullpath);
	Chunk c;
	if(data){
		memcpy(&c.b,data,sizeof(c.b));
		memset(&c.l,0,sizeof(c.l));
		CStr_Free(&fullpath);
	}else{
		c = Chunk_New();
		Chunk_Generate(&c,x,z);
		CStr_Free(&fullpath);
	}
	return c;
}
void Chunk_Save(Chunk* c,char* path,int x,int z){
	String fp = String_Format("%s/Chunk_Z%d_X%d",path,(Number)z,(Number)x);
	CStr fullpath = String_CStr(&fp);
	String_Free(&fp);

	//Files_Write(fullpath,&c->b,sizeof(c->b));
	CStr_Free(&fullpath);
}

typedef Vector Row;

Row Row_New(){
	Row r = Vector_New(sizeof(Chunk));
	return r;
}
Row Row_Make(int x,int z,int l){
	Row r = Row_New();
    for(int i = 0;i<l;i++){
        Chunk c = Chunk_New();
        Chunk_Generate(&c,x + i,z);
        Vector_Push(&r,&c);
    }
    return r;
}
Row Row_Load(char* path,int x,int z,int dx){
	Row r = Row_New();
	for(int i = 0;i<dx;i++){
		Chunk c = Chunk_Load(path,x + i,z);
		Vector_Push(&r,&c);
	}
	return r;
}
void Row_Save(Row* r,char* path,int x,int z){
	for(int i = 0;i<r->size;i++){
		Chunk* c = (Chunk*)Vector_Get(r,i);
		Chunk_Save(c,path,x + i,z);
	}
}
void Row_Free(Row* r){
	Vector_Free(r);
}

#define WORLD_DX	12
#define WORLD_DZ	12

typedef struct World {
	int x;
    int z;
	Vector rows; // Vector<Row>
	Vector blocksunplaced;
} World;

World World_New(){
	World w;
    w.rows = Vector_New(sizeof(Row));
	w.blocksunplaced = Vector_New(sizeof(Block));
	return w;
}

Block World_Get(World* w,float x,float y,float z){
	if(y<0.0) 		return BLOCK_ERROR;
    if(y>=CHUNK_LY) return BLOCK_VOID;

	int cx = Chunk_WtoC(x,CHUNK_LX);
    int cz = Chunk_WtoC(z,CHUNK_LZ);

	int px = cx - w->x;
    int pz = cz - w->z;
    
    if(pz<0 || pz>=w->rows.size) return BLOCK_ERROR;
    Row* r = (Row*)Vector_Get(&w->rows,pz);
    
    if(px<0 || px>=r->size) return BLOCK_ERROR;
    Chunk* c = (Chunk*)Vector_Get(r,px);
    
    int ix = (int)(x - Chunk_CtoW(cx,CHUNK_LX));
    int iz = (int)(z - Chunk_CtoW(cz,CHUNK_LZ));
    return c->b[iz][(int)y][ix];
}
void World_Set(World* w,float x,float y,float z,Block b){
	if(y<0.0 || y>=CHUNK_LY) return;
    
	int cx = Chunk_WtoC(x,CHUNK_LX);
    int cz = Chunk_WtoC(z,CHUNK_LZ);

	int px = cx - w->x;
    int pz = cz - w->z;
    
    if(pz<0 || pz>=w->rows.size) return;
    Row* r = (Row*)Vector_Get(&w->rows,pz);
    
    if(px<0 || px>=r->size) return;
    Chunk* c = (Chunk*)Vector_Get(r,px);
    
    int ix = (int)(x - Chunk_CtoW(cx,CHUNK_LX));
    int iz = (int)(z - Chunk_CtoW(cz,CHUNK_LZ));
    c->b[(int)iz][(int)y][(int)ix] = b;
}

Light World_Get_Light(World* w,float x,float y,float z){
	if(y<0.0) 		return BLOCK_ERROR;
    if(y>=CHUNK_LY) return BLOCK_VOID;

	int cx = Chunk_WtoC(x,CHUNK_LX);
    int cz = Chunk_WtoC(z,CHUNK_LZ);

	int px = cx - w->x;
    int pz = cz - w->z;
    
    if(pz<0 || pz>=w->rows.size) return BLOCK_ERROR;
    Row* r = (Row*)Vector_Get(&w->rows,pz);
    
    if(px<0 || px>=r->size) return BLOCK_ERROR;
    Chunk* c = (Chunk*)Vector_Get(r,px);
    
    int ix = (int)(x - Chunk_CtoW(cx,CHUNK_LX));
    int iz = (int)(z - Chunk_CtoW(cz,CHUNK_LZ));
	return c->l[iz][(int)y][ix];
}
void World_Set_Light(World* w,float x,float y,float z,Light l){
	if(y<0.0 || y>=CHUNK_LY) return;
    
	int cx = Chunk_WtoC(x,CHUNK_LX);
    int cz = Chunk_WtoC(z,CHUNK_LZ);

	int px = cx - w->x;
    int pz = cz - w->z;
    
    if(pz<0 || pz>=w->rows.size) return;
    Row* r = (Row*)Vector_Get(&w->rows,pz);
    
    if(px<0 || px>=r->size) return;
    Chunk* c = (Chunk*)Vector_Get(r,px);
    
    int ix = (int)(x - Chunk_CtoW(cx,CHUNK_LX));
    int iz = (int)(z - Chunk_CtoW(cz,CHUNK_LZ));
    c->l[(int)iz][(int)y][(int)ix] = l;
}

int World_Height(World* map,float x,float z){
	int h = CHUNK_LY - 1;
	for(;h>=0;h--){
		if(World_Get(map,x,h,z)!=BLOCK_VOID){
			return h;
		}
	}
	return h;
}
char World_Void(World* map,Vec3 p){
	Block b = World_Get(map,p.x,p.y,p.z);
	return b==BLOCK_VOID || b==BLOCK_ERROR;
}

void World_Light_Block(World* w,vec3d p){
	Light pl = World_Get_Light(w,p.x,p.y,p.z);
	
	for(int s = 0;s<6;s++){
		vec3d n = vec3d_Add(p,Neighbour_Side(s));
		Block nb = World_Get(w,n.x,n.y,n.z);

		if(!Block_Solid(nb)){
			Light nl = World_Get_Light(w,n.x,n.y,n.z);
			Light newl = Light_Step(p,n,pl);

			if(newl>nl){
				//Block pb = World_Get(w,p.x,p.y,p.z);

				World_Set_Light(w,n.x,n.y,n.z,newl);
				World_Light_Block(w,n);
			}
		}
    }

	// for(int s = 0;s<27;s++){
	// 	vec3d n = vec3d_Add(p,Neighbour_AllSide(s));
	// 	Light nl = World_Get_Light(w,n.x,n.y,n.z);
	// 	Light newl = Light_Step(p,n,pl);

	// 	if(newl>nl){
	// 		Block pb = World_Get(w,p.x,p.y,p.z);

	// 		World_Set_Light(w,n.x,n.y,n.z,newl);
	// 		World_Light_Block(w,n);
	// 	}
    // }
}
void World_Light(World* w){
	for(int i = 0;i<w->rows.size;i++){
        Row* r = (Row*)Vector_Get(&w->rows,i);
        for(int j = 0;j<r->size;j++){
			Chunk* c = (Chunk*)Vector_Get(r,j);
			Chunk_ClearLight(c);
			Chunk_SunLight(c);
		}
    }
	
	for(int i = 0;i<w->rows.size;i++){
        Row* r = (Row*)Vector_Get(&w->rows,i);
        for(int j = 0;j<r->size;j++){
			Chunk* c = (Chunk*)Vector_Get(r,j);
			int x = w->x + j;
			int z = w->z + i;
			float cx = Chunk_CtoW(x,CHUNK_LX);
			float cz = Chunk_CtoW(z,CHUNK_LZ);

			for(int pz = 0;pz<CHUNK_LZ;pz++){
				for(int px = 0;px<CHUNK_LX;px++){
					for(int py = CHUNK_LY - 1;py>=0;py--){
						Light cl = Chunk_Get_Light(c,px,py,pz);
						if(cl==LIGHT_MAX) World_Light_Block(w,vec3d_new(cx + px,py,cz + pz));
					}
				}
			}
		}
    }
}
void World_Tree(World* map,int x,int y,int z){
	World_Set(map,x,y,z,BLOCK_LOG);
	World_Set(map,x,y+1,z,BLOCK_LOG);
	World_Set(map,x,y+2,z,BLOCK_LOG);

	World_Set(map,x,y+3,z,BLOCK_LEAF);

	World_Set(map,x+1,y+3,z,BLOCK_LEAF);
	World_Set(map,x+1,y+3,z+1,BLOCK_LEAF);
	World_Set(map,x+1,y+3,z-1,BLOCK_LEAF);
	World_Set(map,x-1,y+3,z,BLOCK_LEAF);
	World_Set(map,x-1,y+3,z+1,BLOCK_LEAF);
	World_Set(map,x-1,y+3,z-1,BLOCK_LEAF);
	World_Set(map,x,y+3,z+1,BLOCK_LEAF);
	World_Set(map,x,y+3,z-1,BLOCK_LEAF);

	World_Set(map,x,y+4,z,BLOCK_LEAF);
	World_Set(map,x+1,y+4,z,BLOCK_LEAF);
	World_Set(map,x-1,y+4,z,BLOCK_LEAF);
	World_Set(map,x,y+4,z+1,BLOCK_LEAF);
	World_Set(map,x,y+4,z-1,BLOCK_LEAF);
}
void World_Chunk_Mesh(World* w,Chunk* c,Vector* tris,int x,int z){
	float cx = Chunk_CtoW(x,CHUNK_LX);
	float cz = Chunk_CtoW(z,CHUNK_LZ);
	
	for(int i = 0;i<CHUNK_LZ;i++){
        for(int j = 0;j<CHUNK_LY;j++){
			for(int k = 0;k<CHUNK_LX;k++){
				vec3d p = { cx + k,j,cz + i,1.0f };
				Block b = World_Get(w,p.x,p.y,p.z);

				if(b!=BLOCK_VOID){
					for(int s = 0;s<6;s++){
						vec3d n = vec3d_Add(p,Neighbour_Side(s));
						
						Block nb = World_Get(w,n.x,n.y,n.z);
						Light nl = World_Get_Light(w,n.x,n.y,n.z);

						if(nb==BLOCK_VOID){
							triangle tri1 = PlaneTris_Side(p,(vec3d){ 1.0f,1.0f,1.0f,1.0f },s * 2,WHITE,WHITE);
							triangle tri2 = PlaneTris_Side(p,(vec3d){ 1.0f,1.0f,1.0f,1.0f },s * 2 + 1,WHITE,WHITE);

							Light light1 = nl;
							Light light2 = nl;

							vec3d avg1 = vec3d_Div(vec3d_Add(vec3d_Add(tri1.p[0],tri1.p[1]),tri1.p[2]),3.0f);
							vec3d avg2 = vec3d_Div(vec3d_Add(vec3d_Add(tri1.p[0],tri1.p[1]),tri1.p[2]),3.0f);
							
							vec3d r1 = vec3d_Round(avg1);
							vec3d r2 = vec3d_Round(avg2);
							Block block1 = World_Get(w,r1.x,r1.y,r1.z);
							Block block2 = World_Get(w,r2.x,r2.y,r2.z);
							//if(block1==BLOCK_VOID) 
								light1 = (light1 + World_Get_Light(w,r1.x,r1.y,r1.z)) / 2;
							//if(block2==BLOCK_VOID)
								light2 = (light2 + World_Get_Light(w,r2.x,r2.y,r2.z)) / 2;

							int count1 = 1;
							int count2 = 1;

							//int sides1[] = { CUBE_SIDE_NORTH,CUBE_SIDE_EAST,CUBE_SIDE_BOTTOM };
							//int sides2[] = { CUBE_SIDE_SOUTH,CUBE_SIDE_WEST,CUBE_SIDE_TOP };
							for(int ns = 0;ns<6;ns++){
								if(ns!=Side_Other(s)){
									vec3d nn1 = vec3d_Add(avg1,Neighbour_Side(ns));
									vec3d nn2 = vec3d_Add(avg2,Neighbour_Side(ns));

									Block nnb1 = World_Get(w,nn1.x,nn1.y,nn1.z);
									Block nnb2 = World_Get(w,nn2.x,nn2.y,nn2.z);

									Light nnl1 = World_Get_Light(w,nn1.x,nn1.y,nn1.z);
									Light nnl2 = World_Get_Light(w,nn2.x,nn2.y,nn2.z);
									
									if(nnb1==BLOCK_VOID){
										light1 += nnl1;
										count1++;
									}
									if(nnb2==BLOCK_VOID){
										light2 += nnl2;
										count2++;
									}
								}
							}

							light1 /= count1;
							light2 /= count2;

							float l1 = (float)(light1) / (float)LIGHT_MAX * Block_ShadeSide(s);
							float l2 = (float)(light2) / (float)LIGHT_MAX * Block_ShadeSide(s);
							Pixel c1 = Pixel_Mul(Block_Pixel(b,s),Pixel_toRGBA(l1,l1,l1,1.0f));
							Pixel c2 = Pixel_Mul(Block_Pixel(b,s),Pixel_toRGBA(l2,l2,l2,1.0f));
							tri1.c = c1;
							tri2.c = c2;
							
							Vector_Push(tris,&tri1);
							Vector_Push(tris,&tri2);

							//MakePlane(tris,p,(vec3d){ 1.0f,1.0f,1.0f },s,c);
						}
					}
				}
			}
		}
    }
}
void World_Mesh(World* w,Vector* tris){
	World_Light(w);
	Vector_Clear(tris);

	for(int i = 0;i<w->rows.size;i++){
        Row* r = (Row*)Vector_Get(&w->rows,i);
        for(int j = 0;j<r->size;j++){
			Chunk* c = (Chunk*)Vector_Get(r,j);
			int x = w->x + j;
			int z = w->z + i;
			World_Chunk_Mesh(w,c,tris,x,z);
		}
    }
}
char World_Update(World* map,Vector* tris,char* path,float x,float z){
	int cx = Chunk_WtoC(x,CHUNK_LX) - WORLD_DX / 2;
	int cz = Chunk_WtoC(z,CHUNK_LZ) - WORLD_DZ / 2;

	if(map->x!=cx || map->z!=cz){
		int dx = map->x - cx;
		int dz = map->z - cz;

        if(dx<0){
			for(int i = 0;i<map->rows.size;i++){
				Row* r = (Row*)Vector_Get(&map->rows,i);
                Chunk* s = (Chunk*)Vector_Get(r,0);
				Chunk_Save(s,path,map->x,map->z + i);
				Vector_Remove(r,0);
                
				Chunk c = Chunk_Load(path,map->x + WORLD_DX,map->z + i);
				Vector_Push(r,&c);
			}
		}else if(dx>0){
            for(int i = 0;i<map->rows.size;i++){
				Row* r = (Row*)Vector_Get(&map->rows,i);
				Chunk* s = (Chunk*)Vector_Get(r,r->size-1);
				Chunk_Save(s,path,map->x + WORLD_DX - 1,map->z + i);
				Vector_PopTop(r);
                
				Chunk c = Chunk_Load(path,map->x - 1,map->z + i);
				Vector_Add(r,&c,0);
			}
		}

		if(dz<0){
			for(int i = 0;i<abs(dz);i++){
				Row* rm = (Row*)Vector_Get(&map->rows,0);
				Row_Save(rm,path,map->x + i,map->z);
				Row_Free(rm);
                Vector_Remove(&map->rows,0);
                
				Row r = Row_Load(path,map->x + i,map->z + WORLD_DZ,WORLD_DX);
				Vector_Push(&map->rows,&r);
			}
		}else if(dz>0){
            for(int i = 0;i<abs(dz);i++){
				Row* rm = (Row*)Vector_Get(&map->rows,map->rows.size-1);
				Row_Save(rm,path,map->x + i,map->z + WORLD_DZ - 1);
				Row_Free(rm);
                Vector_PopTop(&map->rows);
                
				Row r = Row_Load(path,map->x + i,map->z - 1,WORLD_DX);
                Vector_Add(&map->rows,&r,0);
			}
		}

		map->x = cx;
		map->z = cz;

		World_Mesh(map,tris);
		return 1;
	}
	return 0;
}
void World_Generate(World* map){
	map->x = -WORLD_DX / 2;
    map->z = -WORLD_DZ / 2;
    for(int i = 0;i<WORLD_DZ;i++){
		Row r = Row_Make(map->x,map->z + i,WORLD_DX);
		Vector_Push(&map->rows,&r);
    }
}
void World_Edit(World* map,Vector* tris,vec3d p,Block b){
	World_Set(map,p.x,p.y,p.z,b);
	World_Mesh(map,tris);
}
void World_Free(World* w){
	for(int i = 0;i<w->rows.size;i++){
        Row* r = (Row*)Vector_Get(&w->rows,i);
        Row_Free(r);
    }
    Vector_Free(&w->rows);

	Vector_Free(&w->blocksunplaced);
}

#endif //!WORLD_H