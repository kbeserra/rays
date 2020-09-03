#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h> 
#include <signal.h>

#include "vec/vec.h" 

#define MAX_ITERATIONS 16
#define EPSILON 0.0000001f


float trace( vec3 start, vec3 dir, float (*f)(vec3) ) {
  float acc = 0;
  float dist = 0;
  vec3_normalize( dir, dir );
  int iteration = 0;
  vec3 v; vec3_scale( v, 1.0f, start );
  
  do {
    vec3_lin2( v, 1.0f, v, dist, dir );
    acc += dist;
    dist = f( v );  
  } while ( iteration++ < MAX_ITERATIONS && dist > EPSILON );
  

  return acc; 
}
char pallet[70] = {' ', '.','\'','`','^','\"',',',':',';','I','l','!','i','>','<','~','+','_','-','?',']','[','}','{','1',')','(','|','\\','/','t','f','j','r','x','n','u','v','c','z','X','Y','U','J','C','L','Q','0','O','Z','m','w','q','p','d','b','k','h','a','o','*','#','M','W','&','8','%','B','@','$' };

int palletN = 70;
void drawImage( char* image, int xn, int yn, float(*f)(vec3), 
                vec3 eye, vec3 dir, vec3 up ) {
  float forward = vec3_norm( dir );
  vec3_normalize( dir, dir ); 
  vec3 right; 
  vec3_crossProduct( right, dir, up ); 
  vec3_normalize( right, right );
  
  float max = 0;
  float* dImage = malloc( xn*yn*sizeof(float) );
  
  for( int i = 0; i < xn; i++ ) {
    float xc = -1.0f + i*( 2.0f / ( ((float)xn)-1.0f ) );
    for( int j = 0; j < yn; j++ ) {
      float yc = -1.0f + j*( 2.0f / ( ((float)yn)-1.0f ) );
      vec3 a; 
      vec3_lin2( a, xc, right, yc, up );
      vec3_add( a, a, eye);

      vec3_lin2( a, forward, dir, 1, a );
      
      vec3 d; 
      vec3_sub( d, a, eye );

      float t = trace( a, d, f );
      if( t < EPSILON ) {
        t = 0; 
      } else {
        t = 1.0f / t ;
      }
      dImage[i*xn + j] = t;
      max = max < t ? t : max;
    }
  }

  for( int i = 0; i < xn; i++ ) {
    for( int j = 0; j < yn; j++ ) {
      dImage[i*xn + j] /= max; 
      int index = dImage[i*xn + j]*palletN;
      index = index >= palletN ? palletN - 1 : index;
      image[i*xn + j] = pallet[index];
    }
  }

  free( dImage );

}










// f gives the inf. of the standard euclidean metric from the scene. 
// In this case, the scene is the unit sphere.
float f( vec3 v ) {
  return vec3_norm(v) - 1.0f;
}

float g( vec3 v ) {
  vec3 a = { 0.0f, 0.0f, 1.0f };
  vec3 b = { 0.0f, 1.0f, 0.0f };
  float x = vec3_innerProduct( v, a );
  float y = vec3_innerProduct( v, b );
  float d = sqrt( x*x + y*y );
  if( d > EPSILON ) {
    x /= d;
    y /= d;
  } else {
    x = 1;
    y = 0;
  }
  vec3 p; 
  vec3_lin2( p, x, a, y, b );
  
  return vec3_metric(p,v) - 0.5f;
}



#define DELAY 5000


void signal_callback(int signum) {
  printf("\nCaught signal %d\n",signum);
  printf("\nShutting down...\n");
  endwin();
  printf("bye\n");
  exit(0);
}


int main( int argn, char** argv) {
  float theta = 0;
  float dtheta = M_PI/128;
  float radius = 3;

  int xn = 64;
  int yn = 64;
  initscr();
  noecho();
  curs_set(FALSE);
  signal(SIGINT, signal_callback );
  
  getmaxyx(stdscr, yn, xn);
  
  printf( "starting with: %d %d\n", xn, yn );
  char* image = malloc( xn*yn*sizeof(char) );

  while(1) {

    vec3 eye = { radius*cos(theta), 0, radius*sin(theta) };
    vec3 dir; 
    vec3_scale( dir, -1, eye ); 
    vec3_normalize(dir, dir); 
    vec3_scale( dir, 1.0f, dir ); 
    

    vec3 up = { 0, 1, 0 };

    drawImage( image, xn, yn, g, eye, dir, up ); 
    clear();
    for( int j = 0; j < yn; j++ ) {
      for( int i = 0; i < xn; i++ ) {
        mvprintw(j, i, "%c", image[i*xn+j] );
      }
    }
    refresh();
    theta += dtheta;
    theta -= theta > 2*M_PI ? 2*M_PI : 0;
    
    usleep(DELAY); 
  }

  free( image );


  endwin();

  return 0;
}
