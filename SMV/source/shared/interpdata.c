// $Date: 2011-03-30 12:04:57 -0400 (Wed, 30 Mar 2011) $ 
// $Revision: 8021 $
// $Author: gforney $

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "interpdata.h"
#include "MALLOC.h"

// svn revision character string
char interpdata_revision[]="$Revision: 8021 $";

/* ----------------------- setup_radiancemap ----------------------------- */

void setup_radiancemap(radiancedata *radianceinfo, int ijkbar[3], float xyzbar0[3], float xyzbar[3], float dxyz[3], unsigned char *radiance, unsigned char *opacity){
  radianceinfo->ijkbar=ijkbar;
  radianceinfo->xyzbar0=xyzbar0;
  radianceinfo->xyzbar =xyzbar;
  radianceinfo->radiance=radiance;
  radianceinfo->opacity=opacity;
  radianceinfo->dxyz=dxyz;
}

/* ----------------------- build_radiancemap ----------------------------- */

#define IJKRAD(i,j,k) (i) + nx*(j) + nxy*(k)
#define IJKRAD2(i,j,k) ((i)+1) + (nx+2)*((j)+1) + (nx+2)*(ny+2)*((k)+1)

void build_radiancemap2(radiancedata *radianceinfo){
  int i, j, k, nx, ny, nz, nxy;
  float *fradiance;
  unsigned char *radiance, *opacity;

  nx = radianceinfo->ijkbar[0];
  ny = radianceinfo->ijkbar[1];
  nz = radianceinfo->ijkbar[2];

  radiance = radianceinfo->radiance;
  opacity = radianceinfo->opacity;

  nxy = nx*ny;
  NewMemory((void **)&fradiance,nx*ny*nz*sizeof(float));

  i=0;
  for(j=0;j<ny;j++){
  for(k=0;k<nz;k++){
    fradiance[IJKRAD(i,j,k)]=1.0;
  }
  }

  for(i=1;i<nx;i++){
  for(j=0;j<ny;j++){
  for(k=0;k<nz;k++){
    int ijk,im1jk;

    ijk=IJKRAD(i,j,k);
    im1jk=ijk-1;
    fradiance[ijk]=(float)(fradiance[im1jk]*(float)(255-opacity[im1jk])/255.0);
  }
  }
  }

  for(i=0;i<nx*ny*nz;i++){
    radiance[i]=(unsigned char)(fradiance[i]*255.0);
  }
  FREEMEMORY(fradiance);
}


/* ----------------------- build_radiancemap ----------------------------- */

void build_radiancemap(radiancedata *radianceinfo){
  int i, j, k, nx, ny, nz, nxy;
  float *fradiance,*total_fradiance;
  unsigned char *radiance, *opacity;

  nx = radianceinfo->ijkbar[0];
  ny = radianceinfo->ijkbar[1];
  nz = radianceinfo->ijkbar[2];

  radiance = radianceinfo->radiance;
  opacity = radianceinfo->opacity;

  nxy = nx*ny;
  NewMemory((void **)&fradiance,(nx+2)*(ny+2)*(nz+2)*sizeof(float));
  NewMemory((void **)&total_fradiance,(nx+2)*(ny+2)*(nz+2)*sizeof(float));

  for(i=0;i<(nx+2)*(ny+2)*(nz+2);i++){
    total_fradiance[i]=0.0;
  }
  
  for(j=-1;j<ny+1;j++){
  for(k=-1;k<nz+1;k++){
    fradiance[IJKRAD2(-1,j,k)]=0.2;
    fradiance[IJKRAD2(nx,j,k)]=0.2;
  }
  }
  for(i=-1;i<nx+1;i++){
  for(k=-1;k<nz+1;k++){
    fradiance[IJKRAD2(i,-1,k)]=0.2;
    fradiance[IJKRAD2(i,ny,k)]=0.2;
  }
  }
  for(i=-1;i<nx+1;i++){
  for(j=-1;j<ny+1;j++){
    fradiance[IJKRAD2(i,j,nz)]=0.2;
  }
  }
  CheckMemory;

  // increasing i
  
  for(i=0;i<nx;i++){
  for(j=0;j<ny;j++){
  for(k=0;k<nz;k++){
    int ijk,fijk,fim1jk;

    ijk=IJKRAD(i,j,k);
    fijk=IJKRAD2(i,j,k);
    fim1jk=fijk-1;
    fradiance[fijk]=(float)(fradiance[fim1jk]*(float)(255-opacity[ijk])/255.0);
  }
  }
  }
  for(i=0;i<(nx+2)*(ny+2)*(nz+2);i++){
    total_fradiance[i]+=fradiance[i];
  }
  CheckMemory;

  // decreasing i
  
  for(i=nx-1;i>=0;i--){
  for(j=0;j<ny;j++){
  for(k=0;k<nz;k++){
    int ijk,fijk,fip1jk;

    ijk=IJKRAD(i,j,k);
    fijk=IJKRAD2(i,j,k);
    fip1jk=fijk+1;
    fradiance[fijk]=(float)(fradiance[fip1jk]*(float)(255-opacity[ijk])/255.0);
  }
  }
  }
  for(i=0;i<(nx+2)*(ny+2)*(nz+2);i++){
    total_fradiance[i]+=fradiance[i];
  }
  CheckMemory;

  // increasing j
  
  for(j=0;j<ny;j++){
  for(i=0;i<nx;i++){
  for(k=0;k<nz;k++){
    int ijk,fijk,fijm1k;

    ijk=IJKRAD(i,j,k);
    fijk=IJKRAD2(i,j,k);
    fijm1k=ijk-(nx+1);
    fradiance[fijk]=(float)(fradiance[fijm1k]*(float)(255-opacity[ijk])/255.0);
  }
  }
  }
  for(i=0;i<(nx+2)*(ny+2)*(nz+2);i++){
    total_fradiance[i]+=fradiance[i];
  }
  CheckMemory;

  // decreasing j
  
  for(j=ny-1;j>=0;j--){
  for(i=0;i<nx;i++){
  for(k=0;k<nz;k++){
    int ijk,fijk,fijp1k;

    ijk=IJKRAD(i,j,k);
    fijk=IJKRAD2(i,j,k);
    fijp1k=ijk+(nx+1);
    fradiance[fijk]=(float)(fradiance[fijp1k]*(float)(255-opacity[ijk])/255.0);
  }
  }
  }
  for(i=0;i<(nx+2)*(ny+2)*(nz+2);i++){
    total_fradiance[i]+=fradiance[i];
  }
  CheckMemory;
  
  // decreasing k
  
  for(k=nz-1;k>=0;k--){
  for(j=0;j<ny;j++){
  for(i=0;i<nx;i++){
    int ijk,fijk,fijkp1;

    ijk=IJKRAD(i,j,k);
    fijk=IJKRAD2(i,j,k);
    fijkp1=ijk+(nx+1)*(ny+1);
    fradiance[fijk]=(float)(fradiance[fijkp1]*(float)(255-opacity[ijk])/255.0);
  }
  }
  }
  for(i=0;i<(nx+2)*(ny+2)*(nz+2);i++){
    total_fradiance[i]+=fradiance[i];
  }

  CheckMemory;

  for(k=0;k<nz;k++){
  for(j=0;j<ny;j++){
  for(i=0;i<nx;i++){
    radiance[IJKRAD(i,j,k)]=(unsigned char)(total_fradiance[IJKRAD2(i,j,k)]*255.0);
    CheckMemory;
  }
  }
  }
  CheckMemory;
  FREEMEMORY(fradiance);
  FREEMEMORY(total_fradiance);
}

/* ----------------------- interp3d ----------------------------- */

unsigned char get_opacity(radiancedata *radianceinfo, float xyz[3]){
  int i, j, k;
  int ijk;

  i = (xyz[0]-radianceinfo->xyzbar0[0])/radianceinfo->dxyz[0];
  j = (xyz[1]-radianceinfo->xyzbar0[1])/radianceinfo->dxyz[1];
  k = (xyz[2]-radianceinfo->xyzbar0[2])/radianceinfo->dxyz[2];

  ijk = i + j*radianceinfo->ijkbar[0] + k*radianceinfo->ijkbar[0]*radianceinfo->ijkbar[1];

  return radianceinfo->opacity[ijk];
}
