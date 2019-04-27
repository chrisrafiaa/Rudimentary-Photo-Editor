/* FILE: A2_bmp_helpers.c is where you will code your answers for Assignment 2.
 * 
 * Each of the functions below can be considered a start for you. 
 *
 * You should leave all of the code as is, except for what's surrounded
 * in comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A2_bmp_headers.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

unsigned int findHeightOrWidth(unsigned int first, unsigned int second, int offset);

int bmp_open( char* bmp_filename,        unsigned int *width, 
              unsigned int *height,      unsigned int *bits_per_pixel, 
              unsigned int *padding,     unsigned int *data_size, 
              unsigned int *data_offset, unsigned char** img_data )
{

              
  // YOUR CODE FOR Q1 SHOULD REPLACE EVERYTHING FROM HERE
  //opening image
  FILE* my_image = fopen(bmp_filename, "rb");
  //checking if image exists
  if(my_image == NULL)
    return -1;
  //checking if "BM" exists
  char* checkForBM = (char*)(malloc(2*sizeof(char)));
  fread(checkForBM, 1, 2, my_image);

  int comparison = strcmp(checkForBM, "BM");

  if(comparison!=0)
    return -1;
  //checking if size exists
  unsigned int* checkForSize = (unsigned int*)(malloc(4));
  fread(checkForSize, 4, 1, my_image);
  if(*checkForSize<=0)
    return -1;

  rewind(my_image);

  unsigned long bufferSize = (unsigned long) *checkForSize;
  unsigned char* buffer = (unsigned char*) malloc(bufferSize);
  *img_data = buffer;
  fread(buffer, 1, bufferSize, my_image);
  //goes to byte 2 - data size
  fseek(my_image,2,SEEK_SET);
  fread(data_size,4, 1, my_image);
  //goes to byte 10 - offset
  fseek(my_image,10,SEEK_SET);
  fread(data_offset,4, 1, my_image);
  //goes to byte 18 - width
  fseek(my_image,18,SEEK_SET);
  fread(width,4, 1, my_image);
  //goes to byte 22 - height
  fseek(my_image,22,SEEK_SET);
  fread(height,4, 1, my_image);
  //goes to byte 28 - bpp
  fseek(my_image,28,SEEK_SET);
  fread(bits_per_pixel,4, 1, my_image);
  //finding padding
  *padding = ((*bits_per_pixel)*(*width));
  *padding = *padding/8;
  *padding = *padding%4;
  *padding = (4-*padding)%4;
  // TO HERE
  
  return 0;  
}

// We've implemented bmp_close for you. No need to modify this function
void bmp_close( unsigned char **img_data ){

  if( *img_data != NULL ){
    free( *img_data );
    *img_data = NULL;
  }
}

int bmp_mask( char* input_bmp_filename, char* output_bmp_filename, 
              unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max,
              unsigned char red, unsigned char green, unsigned char blue )
{
  unsigned int img_width;
  unsigned int img_height;
  unsigned int bits_per_pixel;
  unsigned int data_size;
  unsigned int padding;
  unsigned int data_offset;
  unsigned char* img_data    = NULL;
  
  int open_return_code = bmp_open( input_bmp_filename, &img_width, &img_height, &bits_per_pixel, &padding, &data_size, &data_offset, &img_data ); 
  
  if( open_return_code ){ printf( "bmp_open failed. Returning from bmp_mask without attempting changes.\n" ); return -1; }
 
  // YOUR CODE FOR Q2 SHOULD REPLACE EVERYTHING FROM HERE
  unsigned char* newImage = (unsigned char*)malloc(data_size);
  unsigned char* pToNewImage = newImage; //making a pointer to point to beginning of newImage
  memcpy(newImage, img_data, data_size);
  newImage+=data_offset; //goes to end of header
  if(x_min>=img_width || x_max >= img_width || y_min>=img_height||y_max>=img_height)
    return -1;

 unsigned int imageByteWidth = (img_width*bits_per_pixel/8) + padding;
  newImage+= (imageByteWidth*(y_min)); //moving to first pixel we manipulate
  //going upwards
  for(int i=y_min;i<=y_max;i++)
  {
    int x = 0;
    //skip over pixels we dont wish to edit
    for(;x<x_min;x++){
      newImage+=(bits_per_pixel/8);
    }

    //going through one pixel every iteration
    while(x<=x_max){
      *newImage = blue;
      newImage++;
      *newImage = green;
      newImage++;
      *newImage = red;
      newImage++;
      x++;
    }
    //go all the way to the end of the row we dont wish to edit
    while(x<img_width){
      newImage+=(bits_per_pixel/8);
      x++;
    }
    for(int i=0;i<padding;i++){
      newImage++;
    }
  }
  FILE* fp = fopen(output_bmp_filename, "wb");
  fwrite(pToNewImage, 1, data_size, fp);
  fclose(fp);
  fp = NULL;
  // TO HERE!
  
  bmp_close( &img_data );
  
  return 0;
}

int bmp_collage( char* bmp_input1, char* bmp_input2, char* bmp_result, int x_offset, int y_offset ){

  unsigned int img_width1;
  unsigned int img_height1;
  unsigned int bits_per_pixel1;
  unsigned int data_size1;
  unsigned int padding1;
  unsigned int data_offset1;
  unsigned char* img_data1    = NULL;
  
  int open_return_code = bmp_open( bmp_input1, &img_width1, &img_height1, &bits_per_pixel1, &padding1, &data_size1, &data_offset1, &img_data1 ); 
  
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input1 ); return -1; }
  
  unsigned int img_width2;
  unsigned int img_height2;
  unsigned int bits_per_pixel2;
  unsigned int data_size2;
  unsigned int padding2;
  unsigned int data_offset2;
  unsigned char* img_data2    = NULL;
  
  open_return_code = bmp_open( bmp_input2, &img_width2, &img_height2, &bits_per_pixel2, &padding2, &data_size2, &data_offset2, &img_data2 ); 
  
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input2 ); return -1; }
  
  // YOUR CODE FOR Q3 SHOULD REPLACE EVERYTHING FROM HERE
  if(bits_per_pixel1!=bits_per_pixel2)
    return -1;
  
  unsigned int newWidth = findHeightOrWidth(img_width1,img_width2, x_offset);
  unsigned int newHeight = findHeightOrWidth(img_height1, img_height2, y_offset);

  int padding = newWidth*bits_per_pixel2;
  padding = padding/8;
  padding = padding%4;
  padding = ((4-padding)%4);

  unsigned int newSize = (((newWidth*bits_per_pixel2)/8+padding)*newHeight) +data_offset2;
  unsigned char* newImage = (unsigned char*) malloc(newSize);
  unsigned char* pointerToNewImage = newImage; //pointer pointing at beginning of newImage
  memcpy(newImage,img_data2,data_offset2); //copying header into newImage

  unsigned char bytes[4];

  //new size
  pointerToNewImage+=2;
  bytes[0] = (newSize) & 0xFF;
  bytes[1] = (newSize>>8) & 0xFF;
  bytes[2] = (newSize>>16) & 0xFF;
  bytes[3] = (newSize>>24) & 0xFF;
  memcpy(pointerToNewImage, bytes,4);

  //new width
  pointerToNewImage+=16;
  bytes[0] = (newWidth) & 0xFF;
  bytes[1] = (newWidth>>8) & 0xFF;
  bytes[2] = (newWidth>>16) & 0xFF;
  bytes[3] = (newWidth>>24) & 0xFF;
  memcpy(pointerToNewImage, bytes,4);

  //new height
  pointerToNewImage+=4;
  bytes[0] = (newHeight) & 0xFF;
  bytes[1] = (newHeight>>8) & 0xFF;
  bytes[2] = (newHeight>>16) & 0xFF;
  bytes[3] = (newHeight>>24) & 0xFF;
  memcpy(pointerToNewImage, bytes,4);


  pointerToNewImage+=(data_offset2-22); //now at beginning of pixel data of newImage
  unsigned char* pToImg1 = img_data1;
  unsigned char* pToImg2 = img_data2;

  pToImg1+=data_offset1; //move to beginning of pixel data of Image1
  pToImg2+=data_offset2;//move to beginning of pixel data of Image2

  //x_offset and y_offset (+)
    //first image starts at 0,0
    //second image starts at x_offset, y_offset

  //x_offset(+) y_offset (-)
    //first image starts at 0, -y_offset
    //second image starts at x_offset, 0

  //x_offset (-) y_offset (+)
    //first image starts at -x_offset, 0
    //second image starts at 0, y_offset

  //x_offset and y_offset (-)
    //first image starts at -x_offset, -y_offset
    //second image starts at 0,0


  int startOfImageOneX=0;
  int startOfImageOneY=0;

  int startOfImageTwoX=0;
  int startOfImageTwoY=0;

  if(x_offset>=0)
  {
    startOfImageTwoX = x_offset;
    if(y_offset>=0)
      startOfImageTwoY = y_offset;
    else
      startOfImageOneY = -y_offset;
  }

  else
  {
    startOfImageOneX = -x_offset;
    if(y_offset>=0)
      startOfImageTwoY = y_offset;
    else
      startOfImageOneY = -y_offset;
  }

  unsigned char* secondPointer = pointerToNewImage;
  int jumped = (newWidth*(bits_per_pixel2/8))+padding;
  jumped = jumped*startOfImageOneY;
  pointerToNewImage+=jumped;

  for(int j=startOfImageOneY;j<startOfImageOneY+img_height1;j++)
  {
    int i=0; //i is pixel number
    while(i<startOfImageOneX)
    {
      pointerToNewImage+=(bits_per_pixel2/8);
      i++;
    }
    while(i<(startOfImageOneX+img_width1)&&i<newWidth){
      *pointerToNewImage = *pToImg1;
      *(pointerToNewImage+1) = *(pToImg1+1);
      *(pointerToNewImage+2) = *(pToImg1+2);
      pointerToNewImage+=(bits_per_pixel2/8);
      pToImg1+=(bits_per_pixel1/8);
      i++;
    }

    while(i<newWidth){
      pointerToNewImage+=(bits_per_pixel2/8);
      i++;
    }
    
    for(int q = 0;q<padding;q++){
      pointerToNewImage++;
    }
    for(int q=0;q<padding1;q++){
      pToImg1++;
    }
  }

  jumped = (newWidth*(bits_per_pixel2/8))+padding;
  jumped = jumped*startOfImageTwoY;
  secondPointer+=jumped;

  
  for(int j=startOfImageTwoY;j<startOfImageTwoY+img_height2;j++) //loop going through columns in newImage
  {
    int i=0;
    while(i<startOfImageTwoX)
    {
      secondPointer+=(bits_per_pixel2/8);
      i++;
    }

    while(i<(startOfImageTwoX+img_width2)&&i<newWidth){

      *(secondPointer+0) = *(pToImg2+0);
      *(secondPointer+1) = *(pToImg2+1);
      *(secondPointer+2) = *(pToImg2+2);
      secondPointer+=(bits_per_pixel2/8);
      pToImg2+=(bits_per_pixel2/8);
      i++;
    }

    if ((startOfImageTwoX+img_width2) ) {

    }

    while(i<newWidth){
      secondPointer+=(bits_per_pixel2/8);
      i++;
    }
    for(int q = 0;q<padding;q++){
      secondPointer++;
    }
    for(int q=0;q<padding2;q++){
      pToImg2++;
    }
  }
  
  FILE* fp = fopen(bmp_result, "wb");
  fwrite(newImage, 1, newSize, fp);
  fclose(fp);
  fp = NULL;
  // TO HERE!
      
  bmp_close( &img_data1 );
  bmp_close( &img_data2 );
  
  return 0;
}

//helper method to find new height and width
unsigned int findHeightOrWidth(unsigned int first, unsigned int second, int offset){
  unsigned int newNumber = 0;
  if(first>=second)
  {
    int difference = first-second;
    if(offset>=0 && difference>=offset){
      newNumber = first;
    }
    else if(offset>=0 && offset>difference){
      newNumber = first+offset-difference;
    }
    else if(offset<0){
      newNumber = first-offset;
    }
  }

  else
  {
    int difference = second-first;
    if(offset>=0){
      newNumber = second+offset;
    }
    else{
      if((-offset)<difference)
        newNumber = second;
      else
        newNumber = second+(-offset-difference);
    }
  }
  return newNumber;
}