//import fungsi openGL
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <gl/GLU.h>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265359

using namespace std;

//alpan=========================
float cam_angle = 0;
float cam_x = 0;
float cam_y = 0;
float cam_height = 6;
float cam_rotate_speed = 0.003;
float cam_distance = 40;

float eye_x = 0;
float eye_y = 0;
float eye_z = 0;

float cam_angleX = 0;
float cam_angleY = 0;
float mouse_old_pos_x = 0;
float mouse_old_pos_y = 0;

//jel=========================
GLboolean ambient = true;
GLboolean diffuse = true;
GLboolean specular = true;

//tip=========================
GLuint texture[10]; 

//jel=========================
GLfloat no_light[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_ambient[]  = {1.0, 1.0, 1.0, 1.0}; //lighting sesuai posisi, menyesuaikan dengan koordinat ruangan
GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 2.0, 25.0, 3.0, 1.0 };

//alpan=========================
GLfloat bed_no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat bed_ambient[] = { 0.576, 0.839, 0.759, 1.0 };
GLfloat bed_diffuse[] = { 0.5, 0.18, 0.4, 0.9 };
GLfloat bed_specular[] = { 0.56, 0.20, 0.40, 1.0 };
GLfloat bed_shiness[] = { 13 };

GLfloat bed_head_no_mat[] = { 0.0,0.0,0.0,1.0 };
GLfloat bed_head_ambient[] = { 0.276, 0.139, 0.159, 1.0 };
GLfloat bed_head_diffuse[] = { 0.29, 0.4, 0.43, 1.0 };
GLfloat bed_head_specular[] = { 0.184, 0.35, 0.35, 1.0 };
GLfloat bed_head_shiness[] = { 13 };

//jel=========================
GLfloat wall_no_mat[] = { 0.0,0.0,0.0,1.0 };
GLfloat wall_ambient[] = { 0.373, 0.120, 0.827, 1.0 };
GLfloat wall_diffuse[] = { 0.8, 0.48, 0.6, 0.9 };
GLfloat wall_specular[] = { 0.86, 0.50, 0.60, 1.0 };
GLfloat wall_shiness[] = { 13 };

//tip=========================
GLfloat floor_no_mat[] = { 0.0,0.0,0.0,1.0 };
GLfloat floor_ambient[] = { 0.176, 0.039, 0.059, 1.0 };
GLfloat floor_diffuse[] = { 0.19, 0.3, 0.33, 1.0 };
GLfloat floor_specular[] = { 0.184, 0.35, 0.35, 1.0 };
GLfloat floor_shiness[] = { 15 };

//jel=========================
void lighting(GLfloat no_mat[4], GLfloat mat_ambient[4], GLfloat mat_diffuse[4], GLfloat mat_specular[4], GLfloat shiness[1]) {
    if (ambient) {
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    } else {
        glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    }

    if (diffuse) {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    } else {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, no_mat);
    }

    if (specular) {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

    } else {
        glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    }

    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
}

//tip=========================
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};

//tip=========================
int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size; //ukuran gambar dalam bytes
    unsigned long i; //penghitung
    unsigned short int planes; //ukuran planes dalam gambar (harus 1)
    unsigned short int bpp; //ukuran bits per pixel (harus 24)
    char temp; //untuk nyimpan warna sementara (konversi bgr-rgb)

    file = fopen(filename, "rb"); //cek file
    if(file==NULL) {
        printf("Can't open file!");
    }

    fseek(file,18,SEEK_CUR); //melihat header bmp, ukuran lebar/tinggi

    i = fread(&image->sizeX,4,1,file); //melihat ukuran lebar
    i = fread(&image->sizeY,4,1,file); //melihat ukuran tinggi
    size = image->sizeX * image->sizeY * 3; //hitung ukuran (contoh: 24 bits atau 3 bytes per pixel)
    
    fread(&planes, 2, 1, file); //melihat ukuran plane
    fseek(file, 24, SEEK_CUR); //melihat ukuran bits per pixel
    image->data = (char*)malloc(size); //baca data

    i=fread(image->data, size, 1, file);

    for(i=0;i<size;i+=3) { //perulangan untuk semua warna bgr -> rgb
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }

    fclose(file);

    return 1;
}

//tip=========================
void loadTexture(char* file_name, int tex_id) {
    Image *image1;
    
    image1 = (Image*) malloc(sizeof(Image)); //nyiapin ruang untuk texture
    
    if(!ImageLoad(file_name,image1)) { //gambar tidak terbaca
        exit(1);
    }

    texture[tex_id] = tex_id;

    glBindTexture(GL_TEXTURE_2D,tex_id);
    glTexImage2D(GL_TEXTURE_2D,0,3,image1->sizeX,image1->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,image1->data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR); //skala linear kalo gambar > texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR); //skala linear kalo gambar < texture
}

//jel=========================
void cube(GLfloat size) {
    float x1,y1,z1;
    float x2,y2,z2;
    float x3,y3,z3;
    float x4,y4,z4;

    x1 = -size/2;
    y1 = -size/2;
    z1 = -size/2;

    x2 = size/2;
    y2 = -size/2;
    z2 = -size/2;

    x3 = size/2;
    y3 = -size/2;
    z3 = size/2;

    x4 = -size/2;
    y4 = -size/2;
    z4 = size/2;

    float x5,y5,z5;
    float x6,y6,z6;
    float x7,y7,z7;
    float x8,y8,z8;

    x5 = -size/2;
    y5 = size/2;
    z5 = -size/2;

    x6 = size/2;
    y6 = size/2;
    z6 = -size/2;

    x7 = size/2;
    y7 = size/2;
    z7 = size/2;

    x8 = -size/2;
    y8 = size/2;
    z8 = size/2;

    float n1_x = 0, n1_y = -1, n1_z = 0; //hadap bawah
    float n2_x = 0, n2_y = 1, n2_z = 0; //hadap atas
    float n3_x = 0, n3_y = 0, n3_z = -1; //hadap depan
    float n4_x = 0, n4_y = 0, n4_z = 1; //hadap belakang
    float n5_x = 1, n5_y = 0, n5_z = 0; //hadap kanan
    float n6_x = -1, n6_y = 0, n6_z = 0; //hadap kiri

    glBegin(GL_QUADS);

    //Quad 1 (dinding bawah, kasur bawah)
    glNormal3f(n1_x,n1_y,n1_z);

    glTexCoord2f(0,0);
    glVertex3f(x1,y1,z1);

    glTexCoord2f(1,0);
    glVertex3f(x2,y2,z2);

    glTexCoord2f(1,1);
    glVertex3f(x3,y3,z3);

    glTexCoord2f(0,1);
    glVertex3f(x4,y4,z4);

    //Quad 2 (atas kasur & tiang kasur)
    glNormal3f(n2_x,n2_y,n2_z);

    glTexCoord2f(0,0);
    glVertex3f(x8,y8,z8);

    glTexCoord2f(1,0);
    glVertex3f(x7,y7,z7);

    glTexCoord2f(1,1);
    glVertex3f(x6,y6,z6);

    glTexCoord2f(0,1);
    glVertex3f(x5,y5,z5);

    //Quad 3 (dinding kanan kiri, kasur kiri)
    glNormal3f(n3_x,n3_y,n3_z);

    glTexCoord2f(0,0);
    glVertex3f(x5,y5,z5);

    glTexCoord2f(1,0);
    glVertex3f(x6,y6,z6);

    glTexCoord2f(1,1);
    glVertex3f(x2,y2,z2);

    glTexCoord2f(0,1);
    glVertex3f(x1,y1,z1);

    //Quad 4 (kasur kiri)
    glNormal3f(n4_x,n4_y,n4_z);

    glTexCoord2f(0,0);
    glVertex3f(x7,y7,z7);

    glTexCoord2f(1,0);
    glVertex3f(x8,y8,z8);

    glTexCoord2f(1,1);
    glVertex3f(x4,y4,z4);

    glTexCoord2f(0,1);
    glVertex3f(x3,y3,z3);

    //Quad 5 (kasur belakang)
    glNormal3f(n5_x,n5_y,n5_z);

    glTexCoord2f(0,0);
    glVertex3f(x6,y6,z6);

    glTexCoord2f(1,0);
    glVertex3f(x7,y7,z7);

    glTexCoord2f(1,1);
    glVertex3f(x3,y3,z3);

    glTexCoord2f(0,1);
    glVertex3f(x2,y2,z2);

    //Quad 6 (dinding belakang, kasur depan, tiang kasur depan)
    glNormal3f(n6_x,n6_y,n6_z);

    glTexCoord2f(0,0);
    glVertex3f(x1,y1,z1);

    glTexCoord2f(1,0);
    glVertex3f(x4,y4,z4);

    glTexCoord2f(1,1);
    glVertex3f(x8,y8,z8);

    glTexCoord2f(0,1);
    glVertex3f(x5,y5,z5);

    glEnd();
}

//jel=========================
void wall() {
    lighting(wall_no_mat,wall_ambient,wall_diffuse,wall_specular,wall_shiness);
    
    //kiri
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glPushMatrix();
    glTranslatef(0, 7, -15); //posisi
    glScalef(30, 20, 0.2);  //ukuran
    cube(1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //belakang
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glPushMatrix();
    glTranslatef(15, 7, 0);
    glScalef(0.2, 20, 30);
    cube(1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //kanan
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glPushMatrix();
    glTranslatef(0, 7, 15);
    glScalef(30, 20, 0.2);
    cube(1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//tip=========================
void floor() {
    lighting(floor_no_mat,floor_ambient,floor_diffuse,floor_specular,floor_shiness);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glPushMatrix();
    glTranslatef(0, -3, 0);
    glScalef(30, 0.2, 30);
    cube(1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//alpan=========================
void bed() {
    //kasur
    lighting(bed_no_mat,bed_ambient,bed_diffuse,bed_specular,bed_shiness);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glPushMatrix();
    glTranslatef(-6.5, 0, 0); //posisi
    glScalef(18, 3, 9); //ukuran
    cube(1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //tiang kasur
    lighting(bed_head_no_mat,bed_head_ambient,bed_head_diffuse,bed_head_specular,bed_head_shiness);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glPushMatrix();
    glTranslatef(1.3, 2.5, 0);
    glScalef(2.25, 2.5, 9);
    cube(1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//tip=========================
void input_keyboard(unsigned char key, int x, int y) {
    switch(key)
    {
    case 'a':
    case 'A':
        cam_y = cam_y - 0.5;
        break;
    case 'd':
    case 'D':
        cam_y = cam_y + 0.5;
        break;
    case 'w':
    case 'W':
        cam_x = cam_x + 0.5;
        break;
    case 's':
    case 'S':
        cam_x = cam_x - 0.5;
        break;
    case 'l':
    case 'L':
        cam_angleX +=.15;
        cam_angleY +=.15;
        break;
    case 'k':
    case 'K':
        cam_angleX -=.15;
        cam_angleY -=.15;
        break;
    case '1':
        ambient=!ambient;
        break;
    case '2':
        diffuse=!diffuse;
        break;
    case '3':
        specular=!specular;
        break;
    case '0':
        exit(0);
    }
}

void initRendering() {
    glEnable(GL_DEPTH_TEST);
}

void resize(int w, int h) {
    glViewport(0,0,w,h);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,1,5,100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cam_x, cam_height, cam_y, cam_x + cos(cam_angleX), cam_height, cam_y + sin(cam_angleY), 0, 1, 0);

    wall();
    floor();
    bed();

    //lighting in scene
    //jel=========================
    glEnable(GL_LIGHT0);
    if (ambient) {
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    }
    else {
        glLightfv(GL_LIGHT0, GL_AMBIENT, no_light);
    }

    if (diffuse) {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    }
    else {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, no_light);
    }

    if (specular) {
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    }
    else {
        glLightfv(GL_LIGHT0, GL_SPECULAR, no_light);
    }

    glutSwapBuffers();
}

static void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000,800);
    glutInitWindowPosition(100,100);

    glutCreateWindow("Ruang Tidur");
    initRendering();

    glClearColor(0,0,0,1);

    //jel=========================
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //tip=========================
    glGenTextures(10,texture);
    loadTexture("Resource\\kasur.bmp", 0);
    loadTexture("Resource\\dinding.bmp", 1);
    loadTexture("Resource\\lantai.bmp", 3);

    glutDisplayFunc(display);
    glutKeyboardFunc(input_keyboard);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
