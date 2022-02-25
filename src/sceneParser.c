#include "sceneParser.h"

const char DECIMAL_POINT = '.';
const char SPACE = ' ';
const char COLON = ':';
const char COMMA = ',';
const char LEFT_BRACKET = '[';
const char RIGHT_BRACKET = ']';
const char NEGATIVE = '-';
const char PERCENT = '%';

//Constants for accessing array elements
const int X = 0;
const int Y = 1;
const int Z = 2;
const int R = 0;
const int G = 1;
const int B = 2;


void addObjectNode(Object **head, Object *newObject) {
  Object **current = head;
  while( *current != NULL )
    {
    current = &((*current)->next);
    }
  *current = newObject;
}

float clampFloat(float value, float min, float max) {
  if(value < min) {
    return min;
  } else if(value > max) {
    return max;
  }
}

Object *freeObjectData(Object *localPtr){
  if(localPtr != NULL)
     {
     if(localPtr->next != NULL)
        {
        freeObjectData( localPtr->next );
        }
        free(localPtr );
     }
  return NULL;
}

bool cmpStrByIndex(char *line, int low_index, int high_index, char *cmpStr) {
  int cmp_index =  0;
  if(low_index > high_index) {
    return false;
  }

  while(low_index < high_index) {
    if(line[low_index] != cmpStr[cmp_index]) {
      return false;
    }
    low_index += 1;
    cmp_index += 1;
  }
  return cmpStr[cmp_index] == NULL_CHAR;
}

void initObject(Object *new) {
  new->next = NULL;
  new->position[X] = 0.0, new->position[Y] = 0.0, new->position[Z] = 0.0;
  new->color[R] = 0.0, new->color[G] = 0.0, new->color[B] = 0.0;
  new->diffuse_color[R] = 0.0, new->diffuse_color[G] = 0.0, new->diffuse_color[B] = 0.0;
  new->specular_color[R] = 0.0, new->specular_color[G] = 0.0, new->specular_color[B] = 0.0;
  new->shadow = 1.0;
  new->ns = 0.0;
  new->width = 0.0;
  new->height = 0.0;
  new->point_a[X] = 0.0, new->point_a[Y] = 0.0, new->point_a[Z] = 0.0;
  new->point_b[X] = 0.0, new->point_b[Y] = 0.0, new->point_b[Z] = 0.0;
  new->point_c[X] = 0.0, new->point_c[Y] = 0.0, new->point_c[Z] = 0.0;
  new->radius = 0.0;
  new->normal[X] = 0.0, new->normal[Y] = 0.0, new->normal[Z] = 0.0;
  new->radial_a0 = 0.0;
  new->radial_a1 = 0.0;
  new->radial_a2 = 0.0;
  new->theta = 0.0;
  new->cos_theta = 0.0;
  new->angular_a0 = 0.0;
  new->direction[X] = 0.0, new->direction[Y] = 0.0, new->direction[Z] = 0.0;
}

//////Test Function///////
void displayObjects(Object *obj) {
  printf("<<<<START OF LIST>>>>\n\n");
  while(obj != NULL) {
    if(obj->type == CAMERA) {
      printf("\nOBJECT TYPE: CAMERA, POSITION: {%f, %f, %f}\n",
        obj->position[X], obj->position[Y], obj->position[Z]);
      printf("                     WIDTH: %f\n", obj->width);
      printf("                     HEIGHT: %f\n", obj->height);
      obj = obj->next;
      continue; //Skip shared outputs below
    } else if(obj->type == SPHERE) {
        printf("\nOBJECT TYPE: SPHERE, RADIUS:    %f\n", obj->radius);

    } else if(obj->type == PLANE) {
        printf("\nOBJECT TYPE: PLANE,  NORMAL:   {%f, %f, %f}\n",
        obj->normal[X], obj->normal[Y], obj->normal[Z]);

    } else if(obj->type == TRIANGLE) {
        printf("\nOBJECT TYPE: TRIANGLE, \n");
        printf("                     POINT A: {%f, %f, %f}\n",
        obj->point_a[X], obj->point_a[Y], obj->point_a[Z]);
        printf("                     POINT B: {%f, %f, %f}\n",
        obj->point_b[X], obj->point_b[Y], obj->point_b[Z]);
        printf("                     POINT C: {%f, %f, %f}\n",
        obj->point_c[X], obj->point_c[Y], obj->point_c[Z]);
    } else if (obj->type == LIGHT) {
        printf("\nOBJECT TYPE: LIGHT,  RADIAL_A0: %f\n", obj->radial_a0);
        printf("                     RADIAL_A1: %f\n", obj->radial_a1);
        printf("                     RADIAL_A2: %f\n", obj->radial_a2);
        printf("                     COLOR: {%f, %f, %f}\n",
                                   obj->color[R], obj->color[G], obj->color[B]);
        printf("                     POSITION: {%f, %f, %f}\n",
                          obj->position[X], obj->position[Y], obj->position[Z]);
        if(obj->theta == 0.0) {
        printf("                     POINT LIGHT: TRUE\n");
      } else {
          printf("                     THETA: %f\n", obj->theta);
          printf("                     ANGULAR_A0: %f\n", obj->angular_a0);
          printf("                     DIRECTION: {%f, %f, %f}\n",
                      obj->direction[X], obj->direction[Y], obj->direction[Z]);
      }
      obj = obj->next;
      continue; //Skip shared outputs below
    } else {
        printf("\nOBJECT UNKNOWN\n");
    }

    //shared between objects that are not cameras and lights
    printf("                     DIFFUSE COLOR:    {%f, %f, %f}\n",
           obj->diffuse_color[R], obj->diffuse_color[G], obj->diffuse_color[B]);
    printf("                     SPECULAR COLOR: {%f, %f, %f}\n",
        obj->specular_color[R], obj->specular_color[G], obj->specular_color[B]);
    printf("                     POSITION: {%f, %f, %f}\n",
                          obj->position[X], obj->position[Y], obj->position[Z]);
    printf("                     NS:    %f\n", obj->ns);
    obj = obj->next;
  }
  printf("\n<<<<END OF LIST>>>>\n");
}

void failParse(int errorCode, int line_number) {
  if(errorCode == MALFORMED_PROPERTY_VALUE) {
    fprintf(stderr, "%s on line %d\n", "Error: malformed property value",
                                                                   line_number);
  } else if(errorCode == MALFORMED_PROPERTIES) {
      fprintf(stderr, "%s on line %d\n", "Error: malformed properties",
                                                                   line_number);
  } else if(errorCode == UNDEFINED_OBJECT){
      fprintf(stderr, "%s on line %d\n", "Error: undefined object type",
                                                                   line_number);
  } else if(errorCode == INVALID_PROPERTY_VALUE) {
      fprintf(stderr, "%s on line %d\n", "Error: invalid property value",
                                                                   line_number);
  } else if(errorCode == PROPERTY_TYPE_NOT_IMPLEMENTED) {
      fprintf(stderr, "%s on line %d\n", "Error: property type not implemented",
                                                                   line_number);
  } else {
      fprintf(stderr, "%s\n", "Error: opening input file failed");
  }
  //End program with non-normal result
  exit(1);
}

//Returns index of next occurence of search_char after curr_index
int getNextOccurence(char *line, int curr_index, char search_char) {
  int start = curr_index;
  while(line[curr_index] != NULL_CHAR) {
    if(line[curr_index] == search_char) {
      return curr_index;
    }
    curr_index += 1;
  }
  if(search_char == NULL_CHAR) {
    return curr_index - 1;
  }
  return start;
}

int getNumericalValue(char *line, int low_index, int high_index, float *num) {
  char floatStr[200];
  for(int i=0; i < 200; i+=1) {
    floatStr[i] = '0';
  }

  int float_index = 0;
  int decimal_pt_cnt = 0;
  if(line[low_index] == NEGATIVE) {
    floatStr[float_index] = NEGATIVE;
    float_index += 1;
    low_index += 1;
  }
  while(low_index < high_index) {
    if((line[low_index] != DECIMAL_POINT && !validNumber(line[low_index])
      || (line[low_index] == DECIMAL_POINT && decimal_pt_cnt == 1))) {
      return MALFORMED_PROPERTY_VALUE;
    }
    if(line[low_index] == DECIMAL_POINT) {
      decimal_pt_cnt += 1;
    }
    floatStr[float_index] = line[low_index];
    low_index += 1;
    float_index += 1;
  }

  if(decimal_pt_cnt == 0) {
    floatStr[float_index] = DECIMAL_POINT;
  }

  *num = atof(floatStr);
  return SUCCESS;
}

void getObjects(Object **head, char *in_file) {
  FILE *in_handle = fopen(in_file, "r");
  if(in_handle == NULL) {
    failParse(FILE_OPEN_ERROR, 0); //arg 0 as the error did not occur on a line
  }

  int low_index = 0, high_index = 0, result = SUCCESS, line_cnt = 0;
  char line[200];
  while(fgets(line, 200, in_handle) != NULL) {
    line_cnt += 1;
    ///Isolate object type
    high_index = getNextOccurence(line, low_index, COMMA);

    if(line[0] != NULL_CHAR && line[0] == PERCENT) {
      continue;
    }

    Object *newObject = (Object *)malloc(sizeof(Object));
    initObject(newObject);

    if(cmpStrByIndex(line, low_index, high_index, "camera")) {
      result = parseCamera(newObject, line, high_index);

    } else if(cmpStrByIndex(line, low_index, high_index, "sphere")) {
        result = parseSphere(newObject, line, high_index);

    } else if(cmpStrByIndex(line, low_index, high_index, "plane")) {
        result = parsePlane(newObject, line, high_index);

    } else if(cmpStrByIndex(line, low_index, high_index, "triangle")) {
       result = parseTriangle(newObject, line, high_index);

    } else if(cmpStrByIndex(line, low_index, high_index, "light")) {
        result = parseLight(newObject, line, high_index);

    } else {
        result = UNDEFINED_OBJECT;
    }

    addObjectNode(head, newObject);
    if(result != SUCCESS) {
      //free allocated memory and close open file handle before failing
      *head = freeObjectData(*head);
      fclose(in_handle);
      failParse(result, line_cnt);
    }
  }
  fclose(in_handle);
}

int getPropValType(char *line, int low_index, int high_index) {
  if(cmpStrByIndex(line, low_index, high_index, "width")
    || cmpStrByIndex(line, low_index, high_index, "height")
    || cmpStrByIndex(line, low_index, high_index, "radius")
    || cmpStrByIndex(line, low_index, high_index, "theta")
    || cmpStrByIndex(line, low_index, high_index, "angular-a0")
    || cmpStrByIndex(line, low_index, high_index, "radial-a0")
    || cmpStrByIndex(line, low_index, high_index, "radial-a1")
    || cmpStrByIndex(line, low_index, high_index, "radial-a2")
    || cmpStrByIndex(line, low_index, high_index, "shadow")
    || cmpStrByIndex(line, low_index, high_index, "ns")) {
    return SCALAR;
  } else if(cmpStrByIndex(line, low_index, high_index, "position")
          || cmpStrByIndex(line, low_index, high_index, "color")
          || cmpStrByIndex(line, low_index, high_index, "normal")
          || cmpStrByIndex(line, low_index, high_index, "diffuse_color")
          || cmpStrByIndex(line, low_index, high_index, "specular_color")
          || cmpStrByIndex(line, low_index, high_index, "direction")
          || cmpStrByIndex(line, low_index, high_index, "point_a")
          || cmpStrByIndex(line, low_index, high_index, "point_b")
          || cmpStrByIndex(line, low_index, high_index, "point_c")) {
    return VECTOR;
  }
  return PROPERTY_TYPE_NOT_IMPLEMENTED;
}

int getPropertyValue(char *property_name, float *dest,
                                                    char *line, int low_index){
  //Move to start of first property
  low_index = skipSpaces(line, low_index+1);

  int high_index = getNextOccurence(line, low_index, COLON);
  while(!cmpStrByIndex(line, low_index, high_index, property_name)) {
    low_index = high_index;
    high_index = getNextOccurence(line, high_index, COMMA);

    //Move through if inside of vector
    for(int test=low_index; test < high_index; test+=1) {
      if(line[test] == LEFT_BRACKET) {
        low_index = getNextOccurence(line, low_index, RIGHT_BRACKET);
        if(line[low_index] != RIGHT_BRACKET) {
          return MALFORMED_PROPERTY_VALUE;
        }
        low_index = getNextOccurence(line, low_index, COMMA);
        break;
      } else if(test == high_index - 1) {
        low_index = high_index;
      }
    }

    low_index = skipSpaces(line, low_index+1);
    high_index = getNextOccurence(line, low_index, COLON);

    //Looped to end of line and did not find property match, return error
    if(low_index == high_index){
      return MALFORMED_PROPERTIES;
    }
  }

  //Determine the type of the expected value to parse correctly
  int propType = getPropValType(line, low_index, high_index);
  low_index = skipSpaces(line, high_index+1);

  switch(propType){
    case(SCALAR):
      return parseScalar(line, low_index, dest);

    case(VECTOR):
      return parseVector(line, low_index, dest);

    default:
      return propType;
  }
}

int getSharedProperties(char *line, int low_index, Object *newDst) {
  int result = getPropertyValue("diffuse_color", newDst->diffuse_color, line, low_index);
  if(result != SUCCESS) {
    newDst->diffuse_color[R] = 0.0;
    newDst->diffuse_color[G] = 0.0;
    newDst->diffuse_color[B] = 0.0;
  }

  result = getPropertyValue("specular_color", newDst->specular_color, line, low_index);
  if(result != SUCCESS) {
    newDst->specular_color[R] = 0.0;
    newDst->specular_color[G] = 0.0;
    newDst->specular_color[B] = 0.0;
  }

  result = getPropertyValue("ns", &newDst->ns, line, low_index);
  if(result != SUCCESS) newDst->ns = 20.0;

  result = getPropertyValue("shadow", &newDst->shadow, line, low_index);
  if(result != SUCCESS) newDst->shadow = 1.0;

  return SUCCESS;
}

bool isObjectEqual(Object *obj_1, Object *obj_2) {
  if((obj_1 == NULL || obj_2 == NULL) || (obj_1->type != obj_2->type)) {
    return false;
  }

  //Test equality for every field
  bool widthEq = obj_1->width == obj_2->width;

  bool heightEq = obj_1->height == obj_2->height;

  bool posEq = obj_1->position[X] == obj_2->position[X]
            && obj_1->position[Y] == obj_2->position[Y]
            && obj_1->position[Z] == obj_2->position[Z];

  bool diffColorEq = obj_1->diffuse_color[R] == obj_2->diffuse_color[R]
                  && obj_1->diffuse_color[G] == obj_2->diffuse_color[G]
                  && obj_1->diffuse_color[B] == obj_2->diffuse_color[B];

  bool specColorEq = obj_1->specular_color[R] == obj_2->specular_color[R]
                  && obj_1->specular_color[G] == obj_2->specular_color[G]
                  && obj_1->specular_color[B] == obj_2->specular_color[B];

  bool shadowEq = obj_1->shadow == obj_2->shadow;

  bool nsEq = obj_1->ns == obj_2->ns;

  bool radiusEq = obj_1->radius == obj_2->radius;

  bool normEq = obj_1->normal[X] == obj_2->normal[X]
               && obj_1->normal[Y] == obj_2->normal[Y]
               && obj_1->normal[Z] == obj_2->normal[Z];

  bool point_aEq = obj_1->point_a[X] == obj_2->point_a[X]
               && obj_1->point_a[Y] == obj_2->point_a[Y]
               && obj_1->point_a[Z] == obj_2->point_a[Z];

  bool point_bEq = obj_1->point_b[X] == obj_2->point_b[X]
               && obj_1->point_b[Y] == obj_2->point_b[Y]
               && obj_1->point_b[Z] == obj_2->point_b[Z];

  bool point_cEq = obj_1->point_c[X] == obj_2->point_c[X]
               && obj_1->point_c[Y] == obj_2->point_c[Y]
               && obj_1->point_c[Z] == obj_2->point_c[Z];

  bool radialsEq = obj_1->radial_a0 == obj_2->radial_a0
                && obj_1->radial_a1 == obj_2->radial_a1
                && obj_1->radial_a2 == obj_2->radial_a2;

  bool colorEq = obj_1->color[R] == obj_2->color[R]
              && obj_1->color[G] == obj_2->color[G]
              && obj_1->color[B] == obj_2->color[B];

  bool thetaEq = obj_1->theta == obj_2->theta;

  bool angularEq = obj_1->angular_a0 == obj_2->angular_a0;

  bool directionEq = obj_1->direction[X] == obj_2->direction[X]
                  && obj_1->direction[Y] == obj_2->direction[Y]
                  && obj_1->direction[Z] == obj_2->direction[Z];

  switch(obj_1->type) {
    case(CAMERA):
      return posEq && widthEq && heightEq;
    case(SPHERE):
      return posEq && diffColorEq && specColorEq && shadowEq && nsEq && radiusEq;
    case(PLANE):
        return posEq && diffColorEq && specColorEq && shadowEq && nsEq && normEq;
    case(TRIANGLE):
        return diffColorEq && specColorEq && point_aEq && shadowEq && point_bEq
               && point_cEq && nsEq && normEq;
    case(LIGHT):
      return posEq && radialsEq && colorEq && thetaEq && angularEq
          && directionEq;
    default:
      return false;
  }
}

int parseCamera(Object *newDst, char *line, int low_index) {
  newDst->type = CAMERA;

  int result;

  //Get camera width
  result = getPropertyValue("width", &(newDst->width), line, low_index);
  if(result != SUCCESS) return result;
  if(newDst->width <= 0.0) return INVALID_PROPERTY_VALUE;

  //Get camera height
  result = getPropertyValue("height", &newDst->height, line, low_index);
  if(result != SUCCESS) return result;
  if(newDst->height <= 0.0) return INVALID_PROPERTY_VALUE;

  //Get camera position
  result = getPropertyValue("position", newDst->position, line, low_index);
  if(result != SUCCESS) {
    //Default to (0, 0, 0) for camera if position not found
    newDst->position[X] = 0.0;
    newDst->position[Y] = 0.0;
    newDst->position[Z] = 0.0;
  }
  return SUCCESS;
}

int parseLight(Object *newDst, char *line, int low_index) {
  newDst->type = LIGHT;

  int result;

  result = getPropertyValue("position", newDst->position, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("color", newDst->color, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("radial-a0", &newDst->radial_a0, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("radial-a1", &newDst->radial_a1, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("radial-a2", &newDst->radial_a2, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("theta", &newDst->theta, line, low_index);
  if(result != SUCCESS) {
    newDst->theta = 0.0;
    result = SUCCESS;
  }

  //The light is not a point light
  if(!newDst->theta == 0.0) {
    newDst->cos_theta = (cos(newDst->theta) * 180) / 3.14159; //radians->degrees
    result = getPropertyValue("direction", newDst->direction, line, low_index);
    if(result != SUCCESS) return result;
    result = getPropertyValue("angular-a0", &newDst->angular_a0, line, low_index);
    if(result != SUCCESS) return result;
  }
  return result;
}

int parsePlane(Object *newDst, char *line, int low_index) {
  newDst->type = PLANE;

  int result = getSharedProperties(line, low_index, newDst);
  if(result != SUCCESS) return result;

  result = getPropertyValue("position", newDst->position, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("normal", newDst->normal, line, low_index);

  return result;
}

int parseScalar(char *line, int low_index, float *dest) {
  int high_index = getNextOccurence(line, low_index, COMMA);
  if(high_index == low_index) {
    high_index = getNextOccurence(line, low_index, NULL_CHAR) - 1;
  }

  return getNumericalValue(line, low_index, high_index, dest);
}

int parseSphere(Object *newDst, char *line, int low_index) {
  newDst->type = SPHERE;

  int result = getSharedProperties(line, low_index, newDst);
  if(result != SUCCESS) return result;

  result = getPropertyValue("position", newDst->position, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("radius", &(newDst->radius), line, low_index);

  return result;
}

int parseTriangle(Object *newDst, char *line, int low_index) {
  newDst->type = TRIANGLE;

  int result = getSharedProperties(line, low_index, newDst);
  if(result != SUCCESS) return result;

  result = getPropertyValue("point_a", newDst->point_a, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("point_b", newDst->point_b, line, low_index);
  if(result != SUCCESS) return result;

  result = getPropertyValue("point_c", newDst->point_c, line, low_index);
  if(result != SUCCESS) return result;

  float AtoB[3];
  v3_from_points(AtoB, newDst->point_a, newDst->point_b);
  float AtoC[3];
  v3_from_points(AtoC, newDst->point_a, newDst->point_c);
  v3_cross_product(newDst->normal, AtoC, AtoB);
  v3_normalize(newDst->normal, newDst->normal);
  newDst->position[X] = newDst->point_a[X];
  newDst->position[Y] = newDst->point_a[Y];
  newDst->position[Z] = newDst->point_a[Z];

  return SUCCESS;
}

int parseVector(char *line, int low_index, float *dst) {
  if(line[low_index] != LEFT_BRACKET) {
    return MALFORMED_PROPERTY_VALUE;
  }
  int high_index = low_index;
  while(line[high_index] != RIGHT_BRACKET) {
    if(line[high_index] == NULL_CHAR) {
      return MALFORMED_PROPERTY_VALUE;
    }
    high_index += 1;
  }

  //Move inside of vector from '['
  low_index += 1;
  int result, curr_index, comma_index = low_index;
  float next_value;

  //Get first two entries
  for(curr_index=0; curr_index < 2; curr_index+=1) {
    comma_index = getNextOccurence(line, comma_index, COMMA);
    result = getNumericalValue(line, low_index, comma_index, &next_value);
    if(result != SUCCESS) {
      return result;
    }
    dst[curr_index] = next_value;
    low_index = skipSpaces(line, comma_index+1);
    comma_index = low_index;
  }

  //Get final entry next to RIGHT_BRACKET
  result = getNumericalValue(line, low_index, high_index, &next_value);
  if(result != SUCCESS) {
    return result;
  }
  dst[curr_index] = next_value;
  return SUCCESS;
}

int skipSpaces(char *line, int curr_index) {
  int start = curr_index;
  while(line[curr_index] == SPACE) {
    curr_index += 1;
    if(line[curr_index] == NULL_CHAR) {
      return start;
    }
  }
  return curr_index;
}

bool validNumber(char check_num) {
  return (check_num == '0' || check_num == '1' || check_num == '2'
       || check_num == '3' || check_num == '4' || check_num == '5'
       || check_num == '6' || check_num == '7' || check_num == '8'
       || check_num == '9');
}
