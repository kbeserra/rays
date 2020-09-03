#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <curses.h>
#include <unistd.h> 
#include <signal.h>

typedef float vec[3]; 



float x( vec v ) { return v[0]; }
float y( vec v ) { return v[1]; }
float z( vec v ) { return v[2]; }

void lin( vec r, float s, vec v, float t, vec u ) {
  r[0] = s*x(v) + t*x(u);
  r[1] = s*y(v) + t*y(u);
  r[2] = s*z(v) + t*z(u);
}

void add( vec r, vec v, vec u ) {
  lin( r, 1.0f, v, 1.0f, u);
}

void sub( vec r, vec v, vec u ) {
  lin( r, 1.0f, v, -1.0f, u);
}

void scale( vec r, float s, vec v ) {
  lin( r, s, v, 0, v);
}

float dot( vec a , vec b ) {
  return x(a)*x(b) + y(a)*y(b) + z(a)*z(b);
}

void cross( vec r, vec v, vec u ) {
  float a = y(v)*z(u) - z(v)*y(u);
  float b = z(v)*x(u) - x(v)*z(u);
  float c = x(v)*y(u) - y(v)*x(u);
  r[0] = a;
  r[1] = b;
  r[2] = c;
}

float norm( vec a ) {
  return sqrt( dot(a,a) ); 
}

void normalize( vec r, vec v ) { 
  scale( r, 1.0f / norm( v ), v );  
}

float metric( vec v, vec u ) {
  vec c;
  sub( c, v ,u );
  return norm( c );
}

void vecString( char* s, vec v ) {
  sprintf( s, "<%.4f, %.4f, %.4f>", x(v), y(v), z(v) );
}

void vecPrint( vec v, char* s ) {
  printf( "<%.4f, %.4f, %.4f>%s", x(v), y(v), z(v), s );
}

#define MAX_ITERATIONS 16
#define EPSILON 0.0000001f
float trace( vec start, vec dir, float (*f)(vec) ) {
  float acc = 0;
  float dist = 0;
  normalize( dir, dir );
  int iteration = 0;
  vec v; scale( v, 1.0f, start );
  
  do {
    lin( v, 1.0f, v, dist, dir );
    acc += dist;
    dist = f( v );  
  } while ( iteration++ < MAX_ITERATIONS && dist > EPSILON );
  

  return acc; 
}
char pallet[70] = {' ', '.','\'','`','^','\"',',',':',';','I','l','!','i','>','<','~','+','_','-','?',']','[','}','{','1',')','(','|','\\','/','t','f','j','r','x','n','u','v','c','z','X','Y','U','J','C','L','Q','0','O','Z','m','w','q','p','d','b','k','h','a','o','*','#','M','W','&','8','%','B','@','$' };

int palletN = 70;
void drawImage( char* image, int xn, int yn, float(*f)(vec), 
                vec eye, vec dir, vec up ) {
  float forward = norm( dir );
  normalize( dir, dir ); 
  vec right; cross( right, dir, up ); normalize( right, right );
  
  float max = 0;
  float* dImage = malloc( xn*yn*sizeof(float) );
  
  for( int i = 0; i < xn; i++ ) {
    float xc = -1.0f + i*( 2.0f / ( ((float)xn)-1.0f ) );
    for( int j = 0; j < yn; j++ ) {
      float yc = -1.0f + j*( 2.0f / ( ((float)yn)-1.0f ) );
      vec a; lin( a, xc, right, yc, up );
      add( a, a, eye);

      lin( a, forward, dir, 1, a );
      
      vec d; sub( d, a, eye );

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
float f( vec v ) {
  return norm(v) - 1.0f;
}

float g( vec v ) {
  vec a = { 0.0f, 0.0f, 1.0f };
  vec b = { 0.0f, 1.0f, 0.0f };
  float x = dot( v, a );
  float y = dot( v, b );
  float d = sqrt( x*x + y*y );
  if( d > EPSILON ) {
    x /= d;
    y /= d;
  } else {
    x = 1;
    y = 0;
  }
  vec p; lin( p, x, a, y, b );
  
  return metric(p,v) - 0.5f;
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

    vec eye = { radius*cos(theta), 0, radius*sin(theta) };
    vec dir; scale( dir, -1, eye ); normalize(dir, dir); scale( dir, 1.0f, dir ); 
    vec up = { 0, 1, 0 };

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
