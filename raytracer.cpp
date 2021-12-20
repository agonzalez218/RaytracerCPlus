// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <cstring> 

// Include GLM header
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

typedef struct ray
{
    vec3 u;
    vec3 v;
}ray;

typedef struct intersection
{
    vec3 u;
    vec3 v;
    float t;
    vec3 normal;
    vec3 Opoint;
    vec3 Wpoint;
    int objectIndex;

}intersection;

float findT(float A, float B, float C);
void RayCast(int x, int y);
vec3 trace(ray useableRay);
ray rayFromEyeThrough(int x, int y);
vec3 shade(ray ray, intersection hitpoint);
ray reflection(ray primaryRay, intersection hitpoint);
bool shadow(intersection hitpoint, int lightIndex);
intersection closestIntersection(ray primaryRay);
vec3 PhongIllumination(ray primaryRay, intersection hitpoint, int lightIndex);

typedef struct materialProp
{
    vec3 diffuseProp;
    vec3 specularProp;
    float specularDegreeProp;
}materialProp;

vec3** Pixels;
string fileName = "";
int wordCount = 0, objectCount = 0, tokenCounter = 0, lightCount = 0, n = 0, groupIndex = 0, groupCount = 0;
bool nextLine = false, firstChar = true, space = false, groupFlag = false, refractionFlag = false;
string line;
mat4 transformMatrix = mat4(1.0f), transformMatrix2 = mat4(1.0f), * matrixArray, * matrixArray2;
mat4* Obj2W, * W2Obj;

materialProp* materialsArray;
materialProp currMaterials;

vec3* diffuse, * specular, * lightPos, * lightColor, * refractionColor;
vec3 startingPoint = vec3(0, 0, 1);
vec3 ambient = vec3(0), background = vec3(0.0);
float* specularDegree, * refractionIndex;
float dist = 0.0;


int main(int argc, char* argv[]) {
    int i = 0, j = 0, index = 0, lightIndex = 0, k = 0;
    fileName = argv[1];
    ifstream inputScene(fileName);
    if (inputScene.is_open())
    {
        while (getline(inputScene, line))
        {
            j = 0;
            firstChar = true;
            nextLine = false;
            while (line[j] == ' ' && firstChar) {
                j++;
            }
            for (i = j; i < line.length(); i++)
            {
                if (line[i] == '#')
                {
                    nextLine = false;
                    break;
                }
                if (line[i] == ' ' && space)
                {
                    if (space)
                    {
                        wordCount++;
                        space = false;
                    }
                }
                else
                {
                    space = true;
                    nextLine = true;
                    firstChar = false;
                }
            }
            if (nextLine)
            {
                wordCount++;
            }
            firstChar = false;
        }
        inputScene.close();
    }

    string* token = new string[wordCount];
    wordCount = 0;
    inputScene.open(fileName);

    if (inputScene.is_open())
    {
        while (getline(inputScene, line))
        {
            j = 0;
            firstChar = true;
            nextLine = false;
            while (line[j] == ' ' && firstChar) {
                j++;
            }
            for (i = j; i < line.length(); i++)
            {
                if (line[i] == '#')
                {
                    nextLine = false;
                    break;
                }
                if (line[i] == ' ' && space)
                {
                    if (space)
                    {
                        wordCount++;
                        space = false;
                    }
                }
                else
                {
                    space = true;
                    nextLine = true;
                    firstChar = false;
                    token[wordCount] += line[i];
                    if (token[wordCount] == "sphere")
                    {
                        objectCount++;
                    }
                    if (token[wordCount] == "light")
                    {
                        lightCount++;
                    }
                    if (token[wordCount] == "group")
                    {
                        groupCount++;
                    }
                }
            }
            if (nextLine)
            {
                wordCount++;
            }
            firstChar = false;
        }
        inputScene.close();
    }

    for (i = 0; i < wordCount; i++)
    {
        if (token[i] == "view")
        {
            n = stoi(token[i + 1]);
            dist = stof(token[i + 2]);
        }
        if (token[i] == "background")
        {
            background = vec3(stof(token[i + 1]), stof(token[i + 2]), stof(token[i + 3]));
        }
    }

    specularDegree = (float*)malloc(sizeof(float) * objectCount);
    refractionIndex = (float*)malloc(sizeof(float) * objectCount);

    lightPos = (vec3*)malloc(sizeof(vec3) * lightCount);
    lightColor = (vec3*)malloc(sizeof(vec3) * lightCount);

    matrixArray2 = (mat4*)malloc(sizeof(mat4) * groupCount);
    materialsArray = (materialProp*)malloc(sizeof(materialProp) * groupCount);
    matrixArray = (mat4*)malloc(sizeof(mat4) * groupCount);
    Obj2W = (mat4*)malloc(sizeof(mat4) * objectCount);
    W2Obj = (mat4*)malloc(sizeof(mat4) * objectCount);

    refractionColor = (vec3*)malloc(sizeof(vec3) * objectCount);
    diffuse = (vec3*)malloc(sizeof(vec3) * objectCount);
    specular = (vec3*)malloc(sizeof(vec3) * objectCount);
    Pixels = (vec3**)malloc(sizeof(vec3) * n * n);
    for (i = 0; i < n; i++)
    {
        Pixels[i] = (vec3*)malloc(sizeof(vec3) * n);
    }


    // find center of each pixel
    // shoot a ray through each pixel
    // find closest pixel

    // lighting
    // use phong model, color = ka + kd(N * L) + ks(R * V)
    // ka is in file
    // kd is the intersected sphere, N is the hitpoint in object space, * inverse transpose of objext 2 world and normalized, L is position of the light - hitpoint in world space normalized
    // V is negative world ray's direction from center of pixel, R is reflect(L, N in world space)

    // shadow ray
    // u is the hitpoint in world space
    // v is the  (light) L - U

    // obj2w inverse of
    // w2obj 

    // t is move transformation matrix, values following move
    // have current transform matrix, when move comes set obj2w * move(t)
    // w2Obj = move(-1) * W2Obj


    for (tokenCounter = 0; tokenCounter < wordCount; tokenCounter++)
    {
        if (token[tokenCounter] == "light")
        {
            lightColor[lightIndex] = vec3(stof(token[tokenCounter + 1]), stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]));
            lightPos[lightIndex] = vec3(stof(token[tokenCounter + 4]), stof(token[tokenCounter + 5]), stof(token[tokenCounter + 6]));
            lightIndex++;
        }
        if (token[tokenCounter] == "refraction")
        {
            refractionFlag = true;
            refractionColor[index] = vec3(stof(token[tokenCounter + 1]), stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]));
            refractionIndex[index] = stof(token[tokenCounter + 4]);
        }
        if (token[tokenCounter] == "ambient")
        {
            ambient = vec3(stof(token[tokenCounter + 1]), stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]));
        }
        if (token[tokenCounter] == "group")
        {
            groupFlag = true;
            matrixArray[groupIndex] = transformMatrix;
            matrixArray2[groupIndex] = transformMatrix2;
            materialsArray[groupIndex] = currMaterials;
            groupIndex++;
        }
        if (token[tokenCounter] == "groupend")
        {
            groupFlag = false;
            groupIndex--;
            currMaterials = materialsArray[groupIndex];
            transformMatrix = matrixArray[groupIndex];
            transformMatrix2 = matrixArray2[groupIndex];
        }
        if (token[tokenCounter] == "move")
        {
            vec3 transformVector = vec3(stof(token[tokenCounter + 1]), stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]));
            transformMatrix *= translate(mat4(1.0f), transformVector);
            transformMatrix2 = (translate(mat4(1.0f), -transformVector)) * transformMatrix2;
            tokenCounter += 2;
        }
        if (token[tokenCounter] == "material")
        {
            currMaterials.diffuseProp = vec3(stof(token[tokenCounter + 1]), stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]));
            currMaterials.specularProp = vec3(stof(token[tokenCounter + 4]), stof(token[tokenCounter + 5]), stof(token[tokenCounter + 6]));
            currMaterials.specularDegreeProp = stof(token[tokenCounter + 7]);
            tokenCounter += 6;
        }
        if (token[tokenCounter] == "rotate")
        {
            vec3 rotateVector = vec3(stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]), stof(token[tokenCounter + 4]));
            float angle = radians(stof(token[tokenCounter + 1]));
            transformMatrix *= rotate(mat4(1.0f), angle, rotateVector);
            transformMatrix2 = (rotate(mat4(1.0f), -angle, rotateVector)) * transformMatrix2;
            tokenCounter += 2;
        }
        if (token[tokenCounter] == "scale")
        {
            vec3 scaleVector = vec3(stof(token[tokenCounter + 1]), stof(token[tokenCounter + 2]), stof(token[tokenCounter + 3]));
            transformMatrix *= scale(mat4(1.0f), scaleVector);
            transformMatrix2 = inverse(scale(mat4(1.0f), scaleVector)) * transformMatrix2;
            tokenCounter += 2;
        }
        if (token[tokenCounter] == "sphere")
        {
            if (!refractionFlag)
            {
                refractionColor[index] = vec3(0, 0, 0);
                refractionIndex[index] = 1.0002926f;
            }
            diffuse[index] = currMaterials.diffuseProp;
            specular[index] = currMaterials.specularProp;
            specularDegree[index] = currMaterials.specularDegreeProp;
            Obj2W[index] = transformMatrix;
            W2Obj[index] = transformMatrix2;
            index++;
            //cout << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << " ";
            //cout << sphereVec[0] << " " << sphereVec[1] << " " << sphereVec[2] << " ";
            refractionFlag = false;
        }
    }

    /*for (i = 0; i < objectCount; i++)
    {
        cout << diffuse[i][0] << " " << diffuse[i][1] << " " << diffuse[i][2] << endl;
        cout << specular[i][0] << " " << specular[i][1] << " " << specular[i][2] << endl;
        cout << specularDegree[i] << endl;
    }*/

    RayCast(n, n);

    ofstream outputImage;
    outputImage.open("out.ppm");
    outputImage << "P3" << endl;
    outputImage << n << " " << n << endl;
    outputImage << "255" << endl;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            for (k = 0; k < 3; k++)
            {
                if (Pixels[i][j][k] > 1.0f)
                {
                    Pixels[i][j][k] = 1.0f;
                }
                Pixels[i][j][k] *= 255.0f;
            }
            outputImage << to_string(Pixels[i][j][0]) << " ";
            outputImage << to_string(Pixels[i][j][1]) << " ";
            outputImage << to_string(Pixels[i][j][2]) << " ";
        }
        outputImage << endl;
    }
    outputImage.close();

    for (i = 0; i < n; i++)
    {
        free(Pixels[i]);
    }
    free(Obj2W);
    free(matrixArray);
    free(W2Obj);
    free(lightPos);
    free(lightColor);
    free(refractionColor);
    free(diffuse);
    free(specular);
    free(Pixels);

    return 0;
}

float findT(float A, float B, float C)
{
    float D = (B * B) - 4 * (A * C);
    if (D < 0.0f)
        return NULL;

    float rootD = sqrt(D);
    float t0 = 0.5f * (B * (-1.0f) - rootD) / A;
    float t1 = 0.5f * (B * (-1.0f) + rootD) / A;

    if (t0 >= 0.0f)
    {
        return t0;
    }
    if (t1 >= 0.0f)
    {
        return t1;
    }
    return NULL;
}

void RayCast(int x, int y)
{
    int i = 0, j = 0;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            // p = u + vt
            // intersection of sphere x^2 + y^2 + z^2 - r^2 = 0
            Pixels[i][j] = trace(rayFromEyeThrough(i, j));
        }
    }
}

intersection closestIntersection(ray useableRay)
{
    intersection closestHitpoint;
    int i = 0;
    float minT = FLT_MAX;
    vec3 start = useableRay.u;
    vec3 rayT = useableRay.v;
    for (i = 0; i < objectCount; i++)
    {
        start = vec3(W2Obj[i] * vec4(useableRay.u, 1.0));
        rayT = vec3(W2Obj[i] * vec4(useableRay.v, 0.0));

        float A = dot(rayT, rayT);
        float B = 2 * dot(start, rayT);
        float C = dot(start, start) - 1.0f;

        float t = findT(A, B, C);
        if (minT > t && t > 0.0f)
        {
            minT = t;
            closestHitpoint.t = minT;
            closestHitpoint.Opoint = start + (rayT * minT);
            closestHitpoint.Wpoint = vec3((Obj2W[i]) * vec4(closestHitpoint.Opoint, 1.0));
            closestHitpoint.objectIndex = i;
            closestHitpoint.v = rayT;
            closestHitpoint.normal = normalize(mat3(transpose(W2Obj[closestHitpoint.objectIndex])) * closestHitpoint.Opoint);
        }
    }
    if (minT != FLT_MAX)
    {
        return closestHitpoint;
    }
    closestHitpoint.t = NULL;
    return closestHitpoint;
}

ray rayFromEyeThrough(int x, int y)
{
    ray primaryRay;
    float width = 2 * dist / n;
    float height = 2 * dist / n;
    vec3 pixelCenter = vec3((-dist) + (y * width) + (width / 2), (dist - (x * height) - (height / 2)), 0.0);

    vec3 v = pixelCenter - startingPoint;

    primaryRay.v = normalize(v);
    primaryRay.u = startingPoint;

    return primaryRay;
}

vec3 trace(ray useableRay)
{
    intersection hitpoint = closestIntersection(useableRay);
    if (hitpoint.t)
    {
        return shade(useableRay, hitpoint);
    }
    return background;
}

vec3 shade(ray ray, intersection hitpoint)
{
    int i = 0;
    vec3 Color = ambient;
    for (i = 0; i < lightCount; i++)
    {
        if (!shadow(hitpoint, i))
        {
            Color += vec3(PhongIllumination(ray, hitpoint, i));
        }
    }
    if (specular[hitpoint.objectIndex] != vec3(0.0f))
    {
        Color += ((specular[hitpoint.objectIndex] * trace(reflection(ray, hitpoint))));
    }
    return Color;
}

ray reflection(ray primaryRay, intersection hitpoint)
{
    hitpoint.Wpoint = hitpoint.Wpoint + 0.0002f * hitpoint.normal;
    vec3 v = vec3(reflect(primaryRay.v, hitpoint.normal));
    ray reflectRay;
    reflectRay.v = normalize(v);
    reflectRay.u = hitpoint.Wpoint;
    return reflectRay;
}



bool shadow(intersection hitpoint, int lightIndex)
{
    // shadow ray
    // u is the hitpoint in world space
    // v is the  (light) L - U
    hitpoint.Wpoint = hitpoint.Wpoint + 0.0002f * hitpoint.normal;
    vec3 v = vec3(lightPos[lightIndex] - hitpoint.Wpoint);
    ray shadowRay;
    shadowRay.v = normalize(v);
    shadowRay.u = hitpoint.Wpoint;
    intersection closestHitpoint = closestIntersection(shadowRay);
    if (closestHitpoint.t != NULL)
    {
        return true;
    }
    else
        return false;
}

vec3 PhongIllumination(ray primaryRay, intersection hitpoint, int lightIndex)
{
    // diffuse
    vec3 norm = normalize(mat3(transpose(W2Obj[hitpoint.objectIndex])) * hitpoint.Opoint);
    vec3 lightDir = normalize(lightPos[lightIndex] - (hitpoint.Wpoint));

    float diff = dot(norm, lightDir);
    if (diff < 0.0f)
    {
        diff = 0.0f;
        return vec3(0.0);
    }
    vec3 diffuseObj = (diff * diffuse[hitpoint.objectIndex]);

    // specular
    vec3 viewDir = normalize(-primaryRay.v);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));
    float reflectAngle = dot(reflectDir, viewDir);
    if (reflectAngle < 0.0f || diff < 0.0f)
    {
        reflectAngle = 0.0;
    }

    float spec = pow(reflectAngle, specularDegree[hitpoint.objectIndex]);
    if (specularDegree[hitpoint.objectIndex] == 0.0f || specular[hitpoint.objectIndex] == vec3(0.0))
    {
        spec = 0.0f;
        return diffuseObj * lightColor[lightIndex];
    }
    vec3 specularObj = spec * specular[hitpoint.objectIndex];

    vec3 result = (diffuseObj + specularObj) * lightColor[lightIndex];

    return result;
}