#include "algebra3.h"
const int MaxNum = 10;

class Material
{
public:
    vec3 color;
    float Ka, Kd, Ks;
    float exp, Reflect;
    void operator = (Material rhs)
    {
        color = rhs.color;
        Ka = rhs.Ka;
        Kd = rhs.Kd;
        Ks = rhs.Ks;
        exp = rhs.exp;
        Reflect = rhs.Reflect;
    }
};

class Sphere
{
public:
    vec3 O;
    float R;
    Material M;
};

class Triangle
{
public:
    vec3 P[3];
    Material M;
};

class Ray
{
public:
    vec3 S, V;
};

class Scene
{
public:
    Sphere sphere[MaxNum];
    Triangle triangle[MaxNum];
    int sIndex = 0;
    int tIndex = 0;

};

class Object
{
public:
    int mode = 0; //mode 1 for sphere; mode 2 for triangle
    int index = 0; //the object index;
    int dis = 0; //distance between object and eye position
    vec3 intersection = {0 , 0, 0};
    vec3 e; //Eye Position
    vec3 n, l, v, h;

    void Initial(vec3 eyePosition);
    void Set(vec3 start, vec3 v, vec3 lightPosition, Scene s);
};

class Screen
{
public:
    vec3 ur, ul, lr, ll;
    vec3 CenterCal(int i, int j, int w, int h);
};

