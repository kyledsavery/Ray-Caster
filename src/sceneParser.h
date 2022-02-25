#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include "v3math.h"
#include "ppmrw.h"

typedef enum{CAMERA,
             SPHERE,
             PLANE,
             TRIANGLE,
             LIGHT} ObjectTypes;

typedef enum{VECTOR, SCALAR} PropertyTypes;

typedef enum{SUCCESS,
             UNDEFINED_OBJECT,
             MALFORMED_PROPERTIES,
             MALFORMED_PROPERTY_VALUE,
             INVALID_PROPERTY_VALUE,
             PROPERTY_TYPE_NOT_IMPLEMENTED,
             FILE_OPEN_ERROR} errorCodes;

extern const int X;
extern const int Y;
extern const int Z;
extern const int R;
extern const int G;
extern const int B;
extern const float BLACK;

typedef struct Object {
  int type;
  float position[3];
  struct Object *next;
  float color[3];
  float diffuse_color[3];
  float specular_color[3];
  float shadow; //Used to describe objects that do not cast shadows
  float ns;
  //Cameras
  float width;
  float height;
  //Spheres
  float radius;
  //Planes
  float normal[3];
  //Triangles
  float point_a[3];
  float point_b[3];
  float point_c[3];
  //Lights
  float radial_a0;
  float radial_a1;
  float radial_a2;
  float theta;
  float cos_theta;
  float angular_a0;
  float direction[3];

} Object;

//Function prototypes
void addObjectNode(Object **head, Object *newObject);

float clampFloat(float value, float min, float max);

Object *freeObjectData();

bool cmpStrByIndex(char *line, int low_index, int high_index, char *cmpStr);

void initObject(Object *new);

void displayObjects(Object *newObject);

void failParse(int errorCode, int line_number);

int getNextOccurence(char *line, int curr_index, char search_chars);

int getNumericalValue(char *line, int low_index, int high_index, float *num);

void getObjects(Object **head, char *in_file_name);

int getPropertyValue(char *property_name, float *dest,
                                                    char *line, int low_index);

int getPropValType(char *line, int low_index, int high_index);

int getSharedProperties(char *line, int low_index, Object *newDst);

bool isObjectEqual(Object *obj_1, Object *obj_2);

int parseCamera(Object *newDst, char *line, int low_index);

int parseLight(Object *newDst, char *line, int low_index);

int parsePlane(Object *newDst, char *line, int low_index);

int parseScalar(char *line, int low_index, float *dest);

int parseSphere(Object *newDst, char *line, int low_index);

int parseTriangle(Object *newDst, char *line, int low_index);

int parseVector(char *line, int low_index, float *dest);

int skipSpaces(char *line, int curr_index);

bool validNumber(char check_num);
