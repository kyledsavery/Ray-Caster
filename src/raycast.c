#include "sceneParser.h"

//Function Prototypes

float calcPlaneDist(Object *plane, float *ray_dir, float *ray_origin,
                                                           float *intersect_pt);
float calcSphereDist(Object *sphere, float *ray_dir, float *ray_origin,
                                                           float *intersect_pt);
float calcTriangleDist(Object *triangle, float *ray_dir, float *ray_origin,
                                                           float *intersect_pt);
int clampInt(int in_data, int low, int high);
void failRayCast(char *desc);
void generatePPMFromScene(char *input_file, char *output_file, int px_width,
                                                            int px_height);
void getPixmapFromObjects(int *colors, Object *objects, int *max_value,
                                              int px_width, int px_height);
void shade(int *px_colors, Object *curr_obj, Object *objects, float *origin,
                                                          float *closest_pt);

//Main execution
int main(int argc, char *argv[]) {

  if(argc != 5) {
    printf("\nUSAGE: raycast WIDTH HEIGHT input.scene output.ppm\n");
    exit(0);
  }

  //Check for valid width/height
  int px_width = atoi(argv[1]);
  int px_height = atoi(argv[2]);
  if(!(px_width > 0) || !(px_height > 0)) {
    printf("\nInvalid width and height parameters, must be positive.\n");
    exit(0);
  }

  generatePPMFromScene(argv[3], argv[4], px_width, px_height);

  //Return success
  return 0;
}

//Supporting functions//
float calcPlaneDist(Object *plane, float *ray_dir, float *ray_origin,
                                                          float *intersect_pt) {
  intersect_pt[X] = ray_dir[X];
  intersect_pt[Y] = ray_dir[Y];
  intersect_pt[Z] = ray_dir[Z];

  float denominator = v3_dot_product(plane->normal, ray_dir);

  //ray is parallel to plane
  if(denominator == 0.0) {
    return INFINITY;
  }

  float temp[3];
  v3_subtract(temp, ray_origin, plane->position);

  float numerator = -v3_dot_product(plane->normal, temp);
  float t_close = numerator/denominator;

  //ray does not intersect plane
  if(t_close < 0.0) {
    return INFINITY;
  }
  v3_scale(intersect_pt, t_close);
  v3_add(intersect_pt, intersect_pt, ray_origin);
  //ray intersects plane
  return t_close;
}

float calcSphereDist(Object *sphere, float *ray_dir, float *ray_origin,
                                                          float *intersect_pt) {
  float center[] = {sphere->position[X],
                    sphere->position[Y],
                    sphere->position[Z]};

  float radius = sphere->radius;
  float temp[3];
  //Ray from origin to sphere center
  v3_subtract(temp, center, ray_origin);

  //t scalar needed to make ray_direction be as close as possible to center
  float t_close = v3_dot_product(ray_dir, temp);
  float scaled_ray[] = {ray_dir[X], ray_dir[Y], ray_dir[Z]};
  v3_scale(scaled_ray, t_close);

  //Shift the new pt to be relative to the ray_origin
  scaled_ray[X] = ray_origin[X] + scaled_ray[X];
  scaled_ray[Y] = ray_origin[Y] + scaled_ray[Y];
  scaled_ray[Z] = ray_origin[Z] + scaled_ray[Z];

  //Vector from center of sphere to tip of ray_direction
  v3_subtract(temp, scaled_ray, center);

  //Distance from ray_direction to center of sphere
  float dist_to_center = v3_length(temp);

  intersect_pt[X] = ray_dir[X];
  intersect_pt[Y] = ray_dir[Y];
  intersect_pt[Z] = ray_dir[Z];

  //Determine number of intersections
  if(dist_to_center > radius) {
    //Ray does not intersect sphere
    return INFINITY;
  } else if(dist_to_center == radius) {
    v3_scale(intersect_pt, t_close);
    //ray intersects sphere once
    return t_close;
  }
  //Ray intersects sphere twice
  float t_1 = t_close - sqrt((radius*radius) - (dist_to_center*dist_to_center));
  float t_2 = t_close + sqrt((radius*radius) - (dist_to_center*dist_to_center));
  if(t_1 > 0.0){
    v3_scale(intersect_pt, t_1);
    return t_1;
  } else if(t_2 > 0.0) {
    v3_scale(intersect_pt, t_2);
    return t_2;
  }
  return INFINITY;

}

float calcTriangleDist(Object *triangle, float *ray_dir,
                                      float *ray_origin, float *intersect_pt) {

   float planeDist =  calcPlaneDist(triangle, ray_dir, ray_origin, intersect_pt);
   //printf("Ray_origin: {%f, %f, %f}\n", ray_origin[X], ray_origin[Y], ray_origin[Z]);
   //printf("Intersect_pt: {%f, %f, %f}\n", intersect_pt[X], intersect_pt[Y], intersect_pt[Z]);
   if(planeDist < INFINITY){
     float V[3], AB[3], CB[3], AI[3], BC[3], AC[3], BI[3], CA[3], BA[3], CI[3];

     v3_from_points(AB, triangle->point_a, triangle->point_b);
     v3_from_points(CB, triangle->point_c, triangle->point_b);
     v3_from_points(AI, triangle->point_a, intersect_pt);
     float temp = v3_dot_product(CB, CB);
     assert(temp != 0.0);
     v3_scale(CB, v3_dot_product(CB, AB)/temp);
     v3_subtract(V, AB, CB);
     temp = v3_dot_product(V, AB);
     assert(temp != 0.0);
     float a = 1 - (v3_dot_product(V, AI)/v3_dot_product(V, AB));

     v3_from_points(BC, triangle->point_b, triangle->point_c);
     v3_from_points(AC, triangle->point_a, triangle->point_c);
     v3_from_points(BI, triangle->point_b, intersect_pt);
     temp = v3_dot_product(AC, AC);
     assert(temp != 0.0);
     v3_scale(AC, v3_dot_product(AC, BC)/temp);
     v3_subtract(V, BC, AC);
     temp = v3_dot_product(V, BC);
     assert(temp != 0.0);
     float b = 1 - (v3_dot_product(V, BI)/temp);

     v3_from_points(CA, triangle->point_c, triangle->point_a);
     v3_from_points(BA, triangle->point_b, triangle->point_a);
     v3_from_points(CI, triangle->point_c, intersect_pt);
     temp = v3_dot_product(BA, BA);
     assert(temp != 0.0);
     v3_scale(BA, v3_dot_product(BA, CA)/temp);
     v3_subtract(V, CA, BA);
     temp = v3_dot_product(V, CA);
     assert(temp != 0.0);
     float c = 1 - (v3_dot_product(V, CI)/temp);

     if(a >= 0.0 && a <= 1.0
     && b >= 0.0 && b <= 1.0
     && c >= 0.0 && c <= 1.0) {
       return planeDist;
     }
   }
   return INFINITY;
}

int clampInt(int in_data, int low, int high) {
  if(in_data > high) {
    return high;
  } else if(in_data < low) {
    return low;
  } else {
    return in_data;
  }
}

void generatePPMFromScene(char *input_file, char *output_file, int px_width,
                                                              int px_height) {
  //Parse scene file and create linked list of objects
  Object *objects = NULL;
  getObjects(&objects, input_file);

  //Create pixmap from parsed scene data
  int *colors = malloc(sizeof(int) * px_width * px_height * 3);
  int max_value = 1;
  getPixmapFromObjects(colors, objects, &max_value, px_width, px_height);

  //Write pixmap to ppm file
  writeP3(output_file, colors, px_width, px_height, max_value);

  //free object data and allocated array and return success
  free(colors);
  objects = freeObjectData(objects);
}

void getPixmapFromObjects(int *colors, Object *objects, int *max_value,
                                                int px_width, int px_height) {
  //Create pixmap
  Object *camera = objects;
  while(camera != NULL && camera->type != CAMERA) {
    camera = camera->next;
  }
  if(camera == NULL) {
    failRayCast("\nError: no camera detected\n");
  }

  Object *nearest_obj, *curr_obj;
  float ray_origin[3] = {camera->position[X],
                         camera->position[Y],
                         camera->position[Z]};
  float nearest_dist, ray_direction[3], look_at_pt[3], curr_dist;
  float intersect_pt[3], closest_pt[3];
  float cam_width = camera->width, cam_height = camera->height;

  //Loop across view plane rigt to left, top to bottom
  for(int row=0; row < px_height;row+=1) {
    look_at_pt[X] = -cam_width/2;
    look_at_pt[Y] = cam_height/2 - (cam_height/(px_height-1))*row;
    look_at_pt[Z] = -1.0; //By default view plane is 1 unit from cam on -Z_axis

    for(int col=0; col < px_width*3; col+=3) {
      look_at_pt[X] = -cam_width/2 + (cam_width/(px_width-1))*(col/3);
      v3_from_points(ray_direction, ray_origin, look_at_pt);
      v3_normalize(ray_direction, ray_direction);
      int obj_colors[] = {0, 0, 0};

      //Calculate intersections and colors for each pixel
      nearest_obj = NULL;
      nearest_dist = INFINITY;
      curr_dist = INFINITY;
      curr_obj = objects;
      nearest_obj = NULL;
      while(curr_obj != NULL) {
        switch(curr_obj->type) {
          case SPHERE:
                curr_dist = calcSphereDist(curr_obj, ray_direction, ray_origin,
                                                                  intersect_pt);
                break;
          case PLANE:
                curr_dist = calcPlaneDist(curr_obj, ray_direction, ray_origin,
                                                                  intersect_pt);
                break;
          case TRIANGLE:
                curr_dist = calcTriangleDist(curr_obj, ray_direction, ray_origin,
                                                                  intersect_pt);
                break;
        }
        if(curr_dist < nearest_dist && curr_dist > 0.0) {
          nearest_dist = curr_dist;
          nearest_obj = curr_obj;
          closest_pt[X] = intersect_pt[X];
          closest_pt[Y] = intersect_pt[Y];
          closest_pt[Z] = intersect_pt[Z];
        }
        curr_obj = curr_obj->next;
      }

     //Assign the color of the object or make the pixel black if no intersection
      if(nearest_obj != NULL) {
        //printf("SHADING\n");
        shade(obj_colors, nearest_obj, objects, ray_origin, closest_pt);

        //Update max value for ppm output
        if(obj_colors[R] > *max_value) {
          *max_value = obj_colors[R];
        } else if(obj_colors[G] > *max_value) {
          *max_value = obj_colors[G];
        } else if(obj_colors[B] > *max_value) {
          *max_value = obj_colors[B];
        }

      }
      colors[(px_width*3*row)+col] = obj_colors[R];
      colors[(px_width*3*row)+col+1] = obj_colors[G];
      colors[(px_width*3*row)+col+2] = obj_colors[B];
    }
  }
}

void shade(int *px_colors, Object *curr_obj, Object *objects, float *origin,
                                                          float *closest_pt) {
  //Calulate normal Vector
  float normal[3] = {0.0, 0.0, 0.0};
  if(curr_obj->type == SPHERE) {
    normal[X] = closest_pt[X]-curr_obj->position[X];
    normal[Y] = closest_pt[Y]-curr_obj->position[Y];
    normal[Z] = closest_pt[Z]-curr_obj->position[Z];
  } else if(curr_obj->type == PLANE || curr_obj->type == TRIANGLE) {
      normal[X] = curr_obj->normal[X];
      normal[Y] = curr_obj->normal[Y];
      normal[Z] = curr_obj->normal[Z];
  }

  Object *curr_light = objects;
  float Ia[3] = {0.0, 0.0, 0.0};
  float lightSum[] = {0.0, 0.0, 0.0};
  float objectIntersect[3]; //Closest object pt if in shadow

  //loop over all curr_light in the scene
  while(curr_light != NULL) {
    if(curr_light->type == LIGHT) {
      float light_vector[3];
      v3_from_points(light_vector, closest_pt, curr_light->position);
      float DL = v3_length(light_vector);
      v3_normalize(light_vector, light_vector);

      //Check if the current object is in the shadow of another object
      bool inShadow = false;
      if(curr_obj->shadow == 1.0) {
        Object *blocking_obj = NULL;
        blocking_obj = objects;
        float closest_blocking_pt = INFINITY;
        Object *closest_object = NULL;
        float obj_collision_dist;
        while(blocking_obj != NULL) {
          if(!isObjectEqual(blocking_obj, curr_obj)) {
            if(blocking_obj->shadow == 0.0) {
              blocking_obj = blocking_obj->next;
              continue;
            }
            switch(blocking_obj->type) {
              case(SPHERE):
                obj_collision_dist = calcSphereDist(blocking_obj, light_vector,
                                                     closest_pt, objectIntersect);
                break;
              case(PLANE):
                obj_collision_dist = calcPlaneDist(blocking_obj, light_vector,
                                                     closest_pt, objectIntersect);
                break;
              case(TRIANGLE):
                obj_collision_dist = calcTriangleDist(blocking_obj, light_vector,
                                                     closest_pt, objectIntersect);
                break;
              default:
                obj_collision_dist = INFINITY;
            }
            if(obj_collision_dist < closest_blocking_pt
                    && obj_collision_dist > 0.0) {
              closest_blocking_pt = obj_collision_dist;
              closest_object = blocking_obj;
              if(obj_collision_dist < DL) {
                inShadow = true;
              }
            }
          }
          blocking_obj = blocking_obj->next;
        }
      }

      if(!inShadow) {
        //printf("NOT IN SHADOW\n");
        float V[3];
        v3_from_points(V, origin, closest_pt);
        v3_normalize(V, V);
        float ref[3];
        v3_reflect(ref, light_vector, normal);
        v3_normalize(ref, ref);
        float diffuse_dot = v3_dot_product(light_vector, normal);
        if(diffuse_dot < 0.0) diffuse_dot = 0.0;
        float diffuse[] = {curr_obj->diffuse_color[R] * curr_light->color[R],
                           curr_obj->diffuse_color[G] * curr_light->color[G],
                           curr_obj->diffuse_color[B] * curr_light->color[B]};
        v3_scale(diffuse, diffuse_dot);
        float specular_dot = v3_dot_product(ref, V);
        if(specular_dot > 0.0 && diffuse_dot > 0.0) {
          specular_dot = powf(specular_dot, curr_obj->ns);
        } else {
          specular_dot = 0.0;
        }
        float specular[] = {curr_obj->specular_color[R] * curr_light->color[R],
                            curr_obj->specular_color[G] * curr_light->color[G],
                            curr_obj->specular_color[B] * curr_light->color[B]};
        v3_scale(specular, specular_dot);
        float rad_atn = 1/((curr_light->radial_a0) +
                           (curr_light->radial_a1*DL) +
                           (curr_light->radial_a2*(DL*DL)));

        float ang_atn = 1.0;
        if(curr_light->theta != 0.0) {
          float V_obj[] = {-light_vector[X],-light_vector[Y],-light_vector[Z]};
          float V_light[3];
          v3_from_points(V_light, curr_light->position, curr_light->direction);
          v3_normalize(V_light, V_light);
          float Vobj_dot_Vlight = v3_dot_product(V_obj, V_light);
          if(Vobj_dot_Vlight < curr_light->cos_theta) {
            ang_atn = 0.0;
          } else {
            ang_atn = powf(Vobj_dot_Vlight, curr_light->angular_a0);
          }
        }
        float full_lighting[3] = {(diffuse[R] + specular[R])*rad_atn*ang_atn,
                                  (diffuse[G] + specular[G])*rad_atn*ang_atn,
                                  (diffuse[B] + specular[B])*rad_atn*ang_atn};

        v3_add(lightSum, lightSum, full_lighting);
      }
    }
    curr_light = curr_light->next;
  }
  v3_add(lightSum, lightSum, Ia);
  px_colors[R] = clampInt((int)(lightSum[R]*255), 0, 255);
  px_colors[G] = clampInt((int)(lightSum[G]*255), 0, 255);
  px_colors[B] = clampInt((int)(lightSum[B]*255), 0, 255);
}


void failRayCast(char *desc) {
  fprintf(stderr, "%s", desc);
  exit(1); //End program and return non-normal result
}
