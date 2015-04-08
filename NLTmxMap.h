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
            DeleteNull(properties[propertyindex]);
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
            DeleteNull(objects[objectindex]);
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
            DeleteNull(groups[groupindex]);
        }
        
        for ( int layerindex = 0; layerindex < layers.size(); layerindex++ ) {
            DeleteNull(layers[layerindex]);
        }
        
        for ( int tilesetindex = 0; tilesetindex < tilesets.size(); tilesetindex++ ) {
            DeleteNull(tilesets[tilesetindex]);
        }
    }
};


NLTmxMap* NLLoadTmxMap(const char *xml );

