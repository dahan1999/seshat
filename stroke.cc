/*Copyright 2014 Francisco Alvaro

 This file is part of SESHAT.

    SESHAT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SESHAT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SESHAT.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stroke.h"
#include <cfloat>
#include <cmath>
#include <cstring>

bool esNum(char c){
  return (c >= '0' && c <= '9') || c=='-' || c=='.';
}

Stroke::Stroke(int np) {
  NP = np;
  pseq = new Punto[NP];

  cx = cy = 0;
  rx = ry =  INT_MAX;
  rs = rt = -INT_MAX;
  for(int i=0; i<NP; i++)
    pseq[i].x = pseq[i].y = -1;
}

char * getOneLine(char * str) {
  int i = 0;
  char * pos = str;

  while (*pos != '\n') {
      i++;
      pos++;
  }
  char * line = new char[i+1];
  strncpy(line, str, i);
  line[i] = '\0';
  //fprintf(stderr, "line is '%s'\n", line);
  return line;
}


Stroke::Stroke(int np, char *& str) {
  NP = np;
  pseq = new Punto[NP];

  rx = ry =  INT_MAX;
  rs = rt = -INT_MAX;
  //fprintf(stderr,"%d\n", NP);
  int spacePos = 0;
  int toBeCopied = 0;
  for(int i=0; i<NP; i++) {
    char * line = getOneLine(str);
    str += strlen(line);
    str++;
    spacePos = 0;
    while (*(line+spacePos) != ' ')
        spacePos++;
    char posX[10], posY[10];
    strncpy(posX, line, spacePos);
    posX[spacePos] = '\0';
    toBeCopied = strlen(line) -spacePos;
    strncpy(posY, line + spacePos + 1, toBeCopied);
    posY[toBeCopied] = '\0';
    pseq[i].x = atof(posX);
    pseq[i].y = atof(posY);
    //fprintf(stderr,"%f %f\n", pseq[i].x, pseq[i].y);
    delete line;
    if( pseq[i].x < rx ) rx = pseq[i].x;
    if( pseq[i].y < ry ) ry = pseq[i].y;
    if( pseq[i].x > rs ) rs = pseq[i].x;
    if( pseq[i].y > rt ) rt = pseq[i].y;
  }
}
/*
Stroke::Stroke(int np, FILE *fd) {
  NP = np;
  pseq = new Punto[NP];

  rx = ry =  INT_MAX;
  rs = rt = -INT_MAX;
  for(int i=0; i<NP; i++) {
    fscanf(fd, "%f %f", &pseq[i].x, &pseq[i].y);
    if( pseq[i].x < rx ) rx = pseq[i].x;
    if( pseq[i].y < ry ) ry = pseq[i].y;
    if( pseq[i].x > rs ) rs = pseq[i].x;
    if( pseq[i].y > rt ) rt = pseq[i].y;
  }
}
*/
Stroke::Stroke(char *str, int inkml_id) {
  char aux[512];
  int iaux;

  id = inkml_id;
  
  vector<Punto*> data;

  //Remove broken lines
  for(int i=0; str[i]; i++)
    if( str[i] == '\n' ) {
      for(int j=i; str[j]; j++)
	str[j] = str[j+1];
    }

  for(int i=0; str[i]; i++) {

    while( str[i] && !esNum(str[i]) ) i++;
    
    if( !str[i] ) break;

    float px=0, py=0;

    for(iaux=0; str[i] && esNum(str[i]); iaux++, i++)
      aux[iaux] = str[i];
    aux[iaux] = 0;

    if( !str[i] ) break;

    px=atof(aux);

    while( str[i] && !esNum(str[i]) ) i++;
    
    if( !str[i] ) break;

    for(iaux=0; str[i] && esNum(str[i]); iaux++, i++)
      aux[iaux] = str[i];
    aux[iaux] = 0;

    py=atof(aux);

    while( str[i] && str[i] != ',' ) i++;
    i--;

    data.push_back(new Punto(px,py));
  }
  
  NP = (int)data.size();
  pseq = new Punto[NP];
  for(int i=0; i<NP; i++) {
    set(i, data[i]);
    delete data[i];
  }
}

Stroke::~Stroke() {
  delete[] pseq;
}

void Stroke::set(int idx, Punto *p) {
  pseq[idx].x = p->x;
  pseq[idx].y = p->y;

  if( pseq[idx].x < rx ) rx = pseq[idx].x;
  if( pseq[idx].y < ry ) ry = pseq[idx].y;
  if( pseq[idx].x > rs ) rs = pseq[idx].x;
  if( pseq[idx].y > rt ) rt = pseq[idx].y;
}

Punto *Stroke::get(int idx) {
  return &pseq[idx];
}

int Stroke::getNpuntos() {
  return NP;
}

int Stroke::getId() {
  return id;
}

void Stroke::print() {
  printf("STROKE - %d points\n", NP);
  for(int i=0; i<NP; i++)
    printf(" (%g,%g)", pseq[i].x, pseq[i].y);
  printf("\n");
}

float Stroke::min_dist(Stroke *st) {
  float mind = FLT_MAX;
  for(int i=0; i<NP; i++) {
    for(int j=0; j<st->getNpuntos(); j++) {
      Punto *p = st->get(j);

      float d = (pseq[i].x - p->x)*(pseq[i].x - p->x)
	+ (pseq[i].y - p->y)*(pseq[i].y - p->y);

      if( d < mind ) mind=d;
    }
  }

  return sqrt( mind );
}
