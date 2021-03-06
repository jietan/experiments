#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
//#include <gl/gl.h>
//#include <OpenGLES/gl.h>
#import <OpenGLES/ES1/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prof.h"
#include "prof_internal.h"

#include "Mathematics.h"

//#pragma warning(disable:4305; disable:4244)

// use factor to compute a glow amount
static int get_colors(float factor,
                       float text_color_ret[3],
                       float glow_color_ret[3],
                       float *glow_alpha_ret)
{
   const float GLOW_RANGE = 0.5f;
   const float GLOW_ALPHA_MAX = 0.5f;
   float glow_alpha;
   int i;
   float hot[3] = {1, 1.0, 0.9};
   float cold[3] = {0.15, 0.9, 0.15};

   float glow_cold[3] = {0.5f, 0.5f, 0};
   float glow_hot[3] = {1.0f, 1.0f, 0};

   if (factor < 0) factor = 0;
   if (factor > 1) factor = 1;

   for (i=0; i < 3; ++i)
      text_color_ret[i] = cold[i] + (hot[i] - cold[i]) * factor;

   // Figure out whether to start up the glow as well.
   glow_alpha = (factor - GLOW_RANGE) / (1 - GLOW_RANGE);
   if (glow_alpha < 0) {
      *glow_alpha_ret = 0;
      return 0;
   }

   for (i=0; i < 3; ++i)
      glow_color_ret[i] = glow_cold[i] + (glow_hot[i] - glow_cold[i]) * factor;

   *glow_alpha_ret = glow_alpha * GLOW_ALPHA_MAX;
   return 1;
}

static void draw_rectangle(float x0, float y0, float x1, float y1)
{
	static const GLbyte verts[4 * 3] = 
	{    
		 x0,  y0,  1,      x1,  y1,  1, 
		 x1,  y0,  1,      x0,  y1,  1 
	}; 
	
	glVertexPointer( 3, GL_FLOAT, 0, verts); 

	// last number is 4 or 6?
	glDrawArrays( GL_TRIANGLES, 0, 6);
/*   
   // FACE_CULL is disabled so winding doesn't matter
   glVertex2f(x0, y0);
   glVertex2f(x1, y0);
   glVertex2f(x1, y1);
   glVertex2f(x0, y1);
*/
}

typedef struct
{
   float x0,y0;
   float sx,sy;
} GraphLocation;

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} ubColor;

static void graph_func(int id, int x0, int x1, float *values, void *data)
{
   GraphLocation *loc = (GraphLocation *) data;

   int i,r,g,b;

/*
   // trim out values that are under 0.2 ms to accelerate rendering
   while (x0 < x1 && (*values < 0.0002f)) { ++x0; ++values; }
   while (x1 > x0 && (values[x1-1-x0] < 0.0002f)) --x1;
*/
   ubColor VertexColor;
   
   if (id == 0)
   {
	  VertexColor.r = 1;
	  VertexColor.g = 1;
	  VertexColor.b = 1;
	  VertexColor.a = 0.5;
	}  
     // glColor4f(1,1,1,0.5);
   else 
   {
      if (x0 == x1) return;

      id = (id >> 8) + id;
      r = id * 37;
      g = id * 59;
      b = id * 45;
	  
//#pragma warning(disable:4761)
      //glColor3ub((r & 127) + 80, (g & 127) + 80, (b & 127) + 80);
	  VertexColor.r = (r & 127) + 80;
	  VertexColor.g = (g & 127) + 80;
	  VertexColor.b = (b & 127) + 80;
	  VertexColor.a = 0;
   }

	VECTOR2 Vertices[1024];
	memset(Vertices, 0, (1024 * 2 * sizeof(float)));
	int z = 0;
		
   //glBegin(GL_LINE_STRIP);
   if (x0 == x1) 
   {
      float x,y;
      x = loc->x0 + x0 * loc->sx;
      y = loc->y0 + values[0] * loc->sy;
      Vertices[z].x = x;
	  Vertices[z++].y = loc->y0;
      Vertices[z].x = x;
	  Vertices[z++].y = y;
   }
   for (i=0; i < x1-x0; ++i) 
   {
      float x,y;
      x = loc->x0 + (i+x0) * loc->sx;
      y = loc->y0 + values[i] * loc->sy;
	  Vertices[z].x = x;
	  Vertices[z++].y = y;
      //glVertex2f(x,y);
   }
   
   glColorPointer(4, GL_UNSIGNED_BYTE, 0, &VertexColor);
   glVertexPointer(2,GL_FLOAT,0, Vertices);
   
   // is z right here?
   glDrawArrays(GL_LINE_STRIP,0, z);
  // glEnd();
}

Prof_extern_C void Prof_draw_graph_gl(float sx, float sy, float x_spacing, float y_spacing)
{
#ifdef Prof_ENABLED
   Prof_Begin(iprof_draw_graph)

	static GLint iMatrixMode;	
	MATRIX Matrix;
	// Save matrices 
	glGetIntegerv(GL_MATRIX_MODE, &iMatrixMode);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	// Set matrix with viewport dimensions 
	for(int i=0; i< 16; i++)
	{
		Matrix.f[i]=0;
	}
	Matrix.f[0] =	f2vt(2.0f/(480.0f));
	Matrix.f[5] =	f2vt(-2.0f/(320.0f));
	Matrix.f[10] = f2vt(1.0f);
	Matrix.f[12] = f2vt(-1.0f);
	Matrix.f[13] = f2vt(1.0f);
	Matrix.f[15] = f2vt(1.0f);


	// Set matrix mode so that screen coordinates can be specified 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRotatef(90, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(Matrix.f);

//	glDisable(GL_LIGHTING);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);


	GraphLocation loc = { sx, sy, x_spacing, y_spacing * 1000 };
	Prof_graph(128, graph_func, &loc);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Restore matrix mode & matrix 
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(iMatrixMode);

   Prof_End
      
#endif
}


// float to string conversion with sprintf() was
// taking up 10-20% of the Prof_draw time, so I
// wrote a faster float-to-string converter

static char int_to_string[100][4];
static char int_to_string_decimal[100][4];
static char int_to_string_mid_decimal[100][4];
static void int_to_string_init(void)
{
   int i;
   for (i=0; i < 100; ++i) {
      sprintf(int_to_string[i], "%d", i);
      sprintf(int_to_string_decimal[i], ".%02d", i);
      sprintf(int_to_string_mid_decimal[i], "%d.%d", i/10, i % 10);
   }
}

static char *formats[5] =
{
   "%.0f",
   "%.1f",
   "%.2f",
   "%.3f",
   "%.4f",
};

static void float_to_string(char *buf, float num, int precision)
{
   int x,y;
   switch(precision) {
      case 2:
         if (num < 0 || num >= 100)
            break;
         x = num;
         y = (num - x) * 100;
         strcpy(buf, int_to_string[x]);
         strcat(buf, int_to_string_decimal[y]);
         return;
      case 3:
         if (num < 0 || num >= 10)
            break;
         num *= 10;
         x = num;
         y = (num - x) * 100;
         strcpy(buf, int_to_string_mid_decimal[x]);
         strcat(buf, int_to_string_decimal[y]+1);
         return;
      case 4:
         if (num < 0 || num >= 1)
            break;
         num *= 100;
         x = num;
         y = (num - x) * 100;
         buf[0] = '0';
         strcpy(buf+1, int_to_string_decimal[x]);
         strcat(buf, int_to_string_decimal[y]+1);
         return;
   }
   sprintf(buf, formats[precision], num);
}

Prof_extern_C void Prof_draw_gl(float sx, float sy,
                                float full_width, float height,
                                float line_spacing, int precision,
    void (*printText)(float x, float y, char *str), float (*textWidth)(char *str))
{
#ifdef Prof_ENABLED
   Prof_Begin(iprof_draw)

   int i,j,n,o;
   GLuint cull, texture;
   float backup_sy;

   float field_width = textWidth("5555.55");
   float name_width  = full_width - field_width * 3;
   float plus_width  = textWidth("+");

   int max_records;

   Prof_Report *pob;

   if (!int_to_string[0][0]) int_to_string_init();

   if (precision < 1) precision = 1;
   if (precision > 4) precision = 4;

   // disable face culling to avoid having to get winding correct
   texture = glIsEnabled(GL_TEXTURE_2D);
   cull = glIsEnabled(GL_CULL_FACE);
   if (cull == GL_TRUE) {
      glDisable(GL_CULL_FACE);
   }

   pob = Prof_create_report();

   for (i=0; i < NUM_TITLE; ++i) {
      if (pob->title[i]) {
         float header_x0 = sx;
         float header_x1 = header_x0 + full_width;

         if (i == 0)
            glColor4f(0.1f, 0.3f, 0, 0.85);
         else
            glColor4f(0.2f, 0.1f, 0.1f, 0.85);

         //glBegin(GL_QUADS);
         draw_rectangle(header_x0, sy-2, header_x1, sy-line_spacing+2);
         //glEnd();

         if (i == 0) 
            glColor4f(0.6, 0.4, 0, 0);
         else
            glColor4f(0.8f, 0.1f, 0.1f, 0);

         printText(sx+2, sy, pob->title[i]);

         sy += 1.5*line_spacing;
         height -= abs((int)line_spacing)*1.5;
      }
   }

   max_records = height / abs((int)line_spacing);

   o = 0;
   n = pob->num_record;
   if (n > max_records) n = max_records;
   if (pob->hilight >= o + n) {
      o = pob->hilight - n + 1;
   }

   backup_sy = sy;

   // Draw the background colors for the zone data.
   glDisable(GL_TEXTURE_2D);
   //glBegin(GL_QUADS);

   glColor4f(0,0,0,0.85);
   draw_rectangle(sx, sy, sx + full_width, sy - line_spacing);
   sy += line_spacing;

   for (i = 0; i < n; i++) {
      float y0, y1;

      if (i & 1) {
         glColor4f(0.1, 0.1f, 0.2, 0.85);
      } else {
         glColor4f(0.1f, 0.1f, 0.3, 0.85);
      }
      if (i+o == pob->hilight)
         glColor4f(0.3f, 0.3f, 0.1f, 0.85);

      y0 = sy;
      y1 = sy - line_spacing;

      draw_rectangle(sx, y0, sx + full_width, y1);
      sy += line_spacing;
   }
   //glEnd();

   sy = backup_sy;
   glColor4f(0.7,0.7,0.7,0);

   if (pob->header[0])
      printText(sx+8, sy, pob->header[0]);

   for (j=1; j < NUM_HEADER; ++j)
      if (pob->header[j])
         printText(sx + name_width + field_width * (j-1) + 
            field_width/2 - textWidth(pob->header[j])/2, sy, pob->header[j]);

   sy += line_spacing;

   for (i = 0; i < n; i++) {
      char buf[256], *b = buf;
      Prof_Report_Record *r = &pob->record[i+o];
      float text_color[3], glow_color[3];
      float glow_alpha;
      float x = sx + textWidth(" ") * r->indent + plus_width/2;
      if (r->prefix) {
         buf[0] = r->prefix;
         ++b;
      } else {
         x += plus_width;
      }
      if (r->number)
         sprintf(b, "%s (%d)", r->name, r->number);
      else
         sprintf(b, "%s", r->name);
      if (get_colors(r->heat, text_color, glow_color, &glow_alpha)) {
         glColor4f(glow_color[0], glow_color[1], glow_color[2], glow_alpha);
         printText(x+2, sy-1, buf);
      }
      //glColor3fv(text_color);
	  glColor4f(text_color[0], text_color[1], text_color[2], 1.0f);
      printText(x + 1, sy, buf);

      for (j=0; j < NUM_VALUES; ++j) {
         if (r->value_flag & (1 << j)) {
            int pad;
            float_to_string(buf, r->values[j], j == 2 ? 2 : precision);
            pad = field_width- plus_width - textWidth(buf);
            if (r->indent) pad += plus_width;
            printText(sx + pad + name_width + field_width * j, sy, buf);
         }
      }
              

      sy += line_spacing;
   }

   Prof_free_report(pob);

   if (cull == GL_TRUE)
      glEnable(GL_CULL_FACE);
   if (texture == GL_TRUE)
      glEnable(GL_TEXTURE_2D);

   Prof_End
#endif
}
