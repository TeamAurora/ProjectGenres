#pragma once

#include <string>
#include <vector>

using namespace std;

class NLTmxMapTile {
public:
	string id;
	int width;
	int height;
	string filename;
};

class NLTmxMapTileset {
public:
	int firstGid;
	string name;
	int tileWidth;
	int tileHeight;
	vector<NLTmxMapTile*> tiles;
};

class NLTmxMapLayer {
public:
    string name;
    int width;
    int height;
    vector<int> data;
    
    ~NLTmxMapLayer() {
    }
};


struct NLTmxMapObjectProperty {
    string name;
    string value;
};


class NLTmxMapObject {
public:
    string name;
    int gid;
    int x;
    int y;
    int width;
    int height;
    vector<NLTmxMapObjectProperty*> properties;
    
public:
    
    ~NLTmxMapObject() {
        for ( int propertyindex = 0; propertyindex < properties.size(); propertyindex++ ) {
            delete properties[propertyindex];
			properties[propertyindex] = NULL;
        }
    }
};


class NLTmxMapObjectGroup {
public:
    string name;
    int width;
    int height;
    bool visible;
    
    vector<NLTmxMapObject*> objects;
    
    ~NLTmxMapObjectGroup() {
        for ( int objectindex = 0; objectindex < objects.size(); objectindex++ ) {
            delete objects[objectindex];
			objects[objectindex] = NULL;
        }
    }
};


class NLTmxMap {
public:
    
    int width;
    int height;
    int tileWidth;
    int tileHeight;
	int totalTileCount;
    
    vector<NLTmxMapTileset*> tilesets;
    vector<NLTmxMapLayer*> layers;
    vector<NLTmxMapObjectGroup*> groups;
    
    ~NLTmxMap() {
        for ( int groupindex = 0; groupindex < groups.size(); groupindex++ ) {
			delete groups[groupindex];
			groups[groupindex] = NULL;
        }
        
        for ( int layerindex = 0; layerindex < layers.size(); layerindex++ ) {
			delete layers[layerindex];
			layers[layerindex] = NULL;
        }
        
        for ( int tilesetindex = 0; tilesetindex < tilesets.size(); tilesetindex++ ) {
			delete tilesets[tilesetindex];
			tilesets[tilesetindex] = NULL;
        }
    }
};


NLTmxMap* NLLoadTmxMap(const char *xml );