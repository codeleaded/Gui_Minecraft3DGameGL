#ifndef WORLD_H
#define WORLD_H

#include "/home/codeleaded/System/Static/Container/Vector.h"
#include "/home/codeleaded/System/Static/Library/PerlinNoise.h"
#include "/home/codeleaded/System/Static/Library/Files.h"


typedef struct CubeSide {
    Vec3D pos;
	unsigned char light;
    unsigned char id;
    unsigned char side;
} CubeSide;

typedef unsigned char Light;
#define LIGHT_MIN	0
#define LIGHT_MAX	15
#define LIGHT_STEP	1

#define LIGHT_SHADE_X	1.0f
#define LIGHT_SHADE_Y	0.8f
#define LIGHT_SHADE_Z	0.6f

Light Light_Step(Vec3D p,Vec3D n,Light l){
	if(l<=LIGHT_MIN) return LIGHT_MIN;

	Light nl = l - LIGHT_STEP;
	//Light nl = l - I32_max(l / 6,1);
	//Light nl = l - Vec3D_Length(Vec3D_Sub(p,n));
	if(nl<LIGHT_MIN) 	return LIGHT_MIN;
	else 				return nl;
}


typedef unsigned char Block;

#define BLOCK_ERROR	255

#define BLOCK_VOID			0
#define BLOCK_DIRT			1
#define BLOCK_GRAS			3
#define BLOCK_STONE			4
#define BLOCK_SAND			5
#define BLOCK_WATER			6
#define BLOCK_LOG			7
#define BLOCK_LEAF			8
#define BLOCK_TORCH			9


const int ATLAS_COLS = 16;
const int ATLAS_ROWS = 16;
const float TILE_WIDTH = 1.0f / (float)ATLAS_COLS;
const float TILE_HEIGHT = 1.0f / (float)ATLAS_ROWS;

#define BLOCK_FACE_VOID		0
#define BLOCK_FACE_DIRT		(ATLAS_COLS * 15 + 2)
#define BLOCK_FACE_GRAS		(ATLAS_COLS * 15 + 3)
#define BLOCK_FACE_GRAST	(ATLAS_COLS * 15 + 0)
#define BLOCK_FACE_STONE	(ATLAS_COLS * 15 + 1)
#define BLOCK_FACE_SAND		(ATLAS_COLS * 8 + 14)
#define BLOCK_FACE_WATER	(ATLAS_COLS * 3 + 14)
#define BLOCK_FACE_LOG		(ATLAS_COLS * 14 + 4)
#define BLOCK_FACE_LOGT		(ATLAS_COLS * 14 + 5)
#define BLOCK_FACE_LEAF		(ATLAS_COLS * 12 + 5)
#define BLOCK_FACE_TORCH	(ATLAS_COLS * 8 + 8)
#define BLOCK_FACE_TORCHT	(ATLAS_COLS * 9 + 6)

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
		case BLOCK_GRAS:	return 1;
		case BLOCK_SAND: 	return 1;
		case BLOCK_WATER: 	return 1;
		case BLOCK_LOG: 	return 1;
		case BLOCK_LEAF: 	return 1;
		case BLOCK_TORCH: 	return 1;
	}
	return 0;
}
unsigned char Block_Id(Block b,Side s){
	switch (b){
		case BLOCK_DIRT: 			return BLOCK_FACE_DIRT;
		case BLOCK_GRAS:{
			if(s==CUBE_SIDE_TOP) 	return BLOCK_FACE_GRAST;
			if(s==CUBE_SIDE_BOTTOM)	return BLOCK_FACE_DIRT;
			return BLOCK_FACE_GRAS;
		}
		case BLOCK_STONE: 			return BLOCK_FACE_STONE;
		case BLOCK_SAND: 			return BLOCK_FACE_SAND;
		case BLOCK_WATER: 			return BLOCK_FACE_WATER;
		case BLOCK_LOG:{
			if(s==CUBE_SIDE_TOP || s==CUBE_SIDE_BOTTOM)	return BLOCK_FACE_LOGT;
			return BLOCK_FACE_LOG;
		}
		case BLOCK_LEAF: 			return BLOCK_FACE_LEAF;
		case BLOCK_TORCH:{
			if(s==CUBE_SIDE_TOP || s==CUBE_SIDE_BOTTOM)	return BLOCK_FACE_TORCHT;
			return BLOCK_FACE_TORCH;
		}
	}
	return BLOCK_FACE_VOID;
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
void Chunk_Mesh(Chunk* c,Vector* cubeSides,int x,int z){
	int cx = x * CHUNK_LX;
	int cz = z * CHUNK_LZ;
	
	for(int i = 0;i<CHUNK_LZ;i++){
        for(int j = 0;j<CHUNK_LY;j++){
			for(int k = 0;k<CHUNK_LX;k++){
				Block b = Chunk_Get(c,k,j,i);
				Light l = Chunk_Get_Light(c,k,j,i);

				if(b!=BLOCK_VOID){
					for(int s = 0;s<6;s++){
						Vec3D p = { k,j,i,1.0f };
						Vec3D n = Vec3D_Add(p,Neighbour_Side(s));
						
						Block nb = Chunk_Get(c,n.x,n.y,n.z);
						Light nl = Chunk_Get_Light(c,n.x,n.y,n.z);
						if(nb==BLOCK_VOID || nb==BLOCK_ERROR){
							Vec3D cp = { cx + k,j,cz + i,1.0f };
							Vector_Push(cubeSides,(CubeSide[]){ { .pos = cp,.light = nl,.id = b,.side = s } });
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


#define WORLD_DX	8
#define WORLD_DZ	8

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

void World_Light_Block(World* w,Vec3D p){
	Light pl = World_Get_Light(w,p.x,p.y,p.z);
	
	for(int s = 0;s<6;s++){
		Vec3D n = Vec3D_Add(p,Neighbour_Side(s));
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
	// 	Vec3D n = Vec3D_Add(p,Neighbour_AllSide(s));
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
						if(cl==LIGHT_MAX) World_Light_Block(w,Vec3D_new(cx + px,py,cz + pz));
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
void World_Chunk_Mesh(World* w,Chunk* c,Vector* cubeSides,int x,int z){
	float cx = Chunk_CtoW(x,CHUNK_LX);
	float cz = Chunk_CtoW(z,CHUNK_LZ);
	
	for(int i = 0;i<CHUNK_LZ;i++){
        for(int j = 0;j<CHUNK_LY;j++){
			for(int k = 0;k<CHUNK_LX;k++){
				Vec3D p = { cx + k,j,cz + i,1.0f };
				Block b = World_Get(w,p.x,p.y,p.z);

				if(b!=BLOCK_VOID){
					for(int s = 0;s<6;s++){
						Vec3D n = Vec3D_Add(p,Neighbour_Side(s));
						
						Block nb = World_Get(w,n.x,n.y,n.z);
						Light nl = World_Get_Light(w,n.x,n.y,n.z);

						if(nb==BLOCK_VOID){
							Vector_Push(cubeSides,(CubeSide[]){ { .pos = p,.light = nl,.id = b,.side = s } });
						}
					}
				}
			}
		}
    }
}
void World_Mesh(World* w,Vector* cubeSides){
	World_Light(w);
	Vector_Clear(cubeSides);

	for(int i = 0;i<w->rows.size;i++){
        Row* r = (Row*)Vector_Get(&w->rows,i);
        for(int j = 0;j<r->size;j++){
			Chunk* c = (Chunk*)Vector_Get(r,j);
			int x = w->x + j;
			int z = w->z + i;
			World_Chunk_Mesh(w,c,cubeSides,x,z);
		}
    }
}
char World_Update(World* map,Vector* cubeSides,char* path,float x,float z){
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

		World_Mesh(map,cubeSides);
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
void World_Edit(World* map,Vector* cubeSides,Vec3D p,Block b){
	World_Set(map,p.x,p.y,p.z,b);
	World_Mesh(map,cubeSides);
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