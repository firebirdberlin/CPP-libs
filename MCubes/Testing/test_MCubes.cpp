//		./MCubes/Testing/test_MCubes.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Philipp Gast <avatar81@itp.tu-berlin.de>
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
//
//		This program is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with this program; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.
/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux/SDL by Ti Leggett '01)
 *
 * If you've found this code useful, please let me know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 * or for port-specific comments, questions, bugreports etc.
 * email to leggett@eecs.tulane.edu
 */

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include <MCubes.hpp> //! \attention include MCubes.hpp from the standard library

////#include <lists.h>
//#include <Data.hpp>

using namespace std;

size_t const xcord = 3;
size_t const ycord = 3;
size_t const zcord = 3;

// screen width, height, and bit depth
size_t const SCREEN_WIDTH  = 640;
size_t const SCREEN_HEIGHT = 480;
size_t const SCREEN_BPP    =  16;

// Define our booleans
int const TRUE  = 1;
int const FALSE = 0;

//! This is for our SDL surface.
SDL_Surface *surface;

/** name: Quit
 *	Function to release/destroy our resources and restoring the old desktop
 *
 * \param returnCode: The the return value of the programm. Usally values other
 *				than zero indicates errors.
 * \return void
 */
void Quit( int returnCode ) {
	//! clean up the window
	SDL_Quit( );

	//! and exit appropriately
	exit( returnCode );
}

/** name: resizeWindow
 *	Function to reset our viewport after a window resize.
 *
 * \param width:
 * \param height: New values for the width/height of the window.
 * \return Integer
 */
int resizeWindow( int width, int height ) {
	// Height / width ration
	GLfloat ratio;

	// Protect against a divide by zero
	if ( height == 0 )
		height = 1;

	ratio = ( GLfloat )width / ( GLfloat )height;

	// Setup our viewport.
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

	// change to the projection matrix and set our viewing volume.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	// Set our perspective.
	gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

	// Make sure we're chaning the model view and not the projection.
	glMatrixMode( GL_MODELVIEW );

	// Reset The View
	glLoadIdentity( );

	return( TRUE );
}

/** name: handleKeyPress
 *	function to handle key press events
 * \param keysym: Pointer of to type SDL_keysym.
 * \return void
 */
void handleKeyPress( SDL_keysym *keysym ) {
	switch ( keysym->sym ) {
		case SDLK_ESCAPE:
			//! If ESC key was pressed, quit the window normaly.
			Quit( 0 );
			break;
		case SDLK_F1:
			/** F1 key was pressed
			 *	this toggles fullscreen mode
			 */
			SDL_WM_ToggleFullScreen( surface );
			break;
		default:
			break;
	}

	return;
}

/** name: initGL
 *	general OpenGL initialization function
 * \return Integer
 */
int initGL() {

	//! Enable smooth shading
	glShadeModel(GL_SMOOTH);

	//! Set the background black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//! Depth buffer setup
	glClearDepth(1.0f);

	// Enables Depth Testing //! Disabled to check right Tip detection
	// glEnable( GL_DEPTH_TEST );

	//! The Type Of Depth Test To Do
	glDepthFunc(GL_LEQUAL);

	// Really Nice Perspective Calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return(TRUE);
}

/** name: drawGLScene
 *	This is the drawing code
 * \return Integer
 */
int drawGLScene() {
	// These are to calculate our fps
	static GLint T0     = 0;
	static GLint Frames = 0;

	// Clear The Screen And The Depth Buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Move Left 1.5 Units And Into The Screen 6.0
	glLoadIdentity();
	glTranslatef( -1.5f, -1.5f, -6.0f );

	//! Here are the values of the 'concentrations' stored.
	double values1[zcord*ycord*xcord];
	double values2[zcord*ycord*xcord];

	for(size_t i = 0; i < zcord; i++) {
		for(size_t j = 0; j < ycord; j++) {
			for(size_t k = 0; k < xcord; k++) {
				if(i < 1)
					values1[k + j*xcord + i*xcord*ycord] = 1;
				else
					values1[k + j*xcord + i*xcord*ycord] = 0;

				if(k < 1)
					values2[k + j*xcord + i*xcord*ycord] = 1;
				else
					values2[k + j*xcord + i*xcord*ycord] = 0;
			}
		}
	}
	MCubes::Grid gitter(Point(1, 1, 1, 1), xcord, ycord, zcord);

	size_t const nr_triangles = gitter.TriangleList.size();

	//! RUN Marching Cubes Algorithm
	gitter.RunAlgorithm(values1, 0.5);

	glBegin(GL_POINTS);		// Drawing the Points
	for(size_t i = 0; i < zcord; i++) {
		for(size_t j = 0; j < ycord; j++) {
			for(size_t k = 0; k < xcord; k++) {
				glVertex3f(k, j, i);
			}
		}
	}
	glEnd();				// Done Drawing Points

	glBegin(GL_TRIANGLES);	// Drawing Using Triangles
	list<MCubes::Triangle>::iterator it;
	float const col_inc = 1.0/float(nr_triangles);
	float col = col_inc;
	for(it = gitter.TriangleList.begin(); it != gitter.TriangleList.end(); ++it) {
		glColor4f(0., col, 0., 0.5);//kein Plan warum die unterschiedlichen Farben der 2 ebenen nicht funktionieren
		//glColor3f(0.9, 0., 0.);
		glVertex3f(it->v1.x, it->v1.y, it->v1.z);
		glVertex3f(it->v2.x, it->v2.y, it->v2.z);
		glVertex3f(it->v3.x, it->v3.y, it->v3.z);
		col += col_inc;
		glColor3f(1.,1.,1.);
	}
	glEnd( );

	//gitter.set_Values(values2);
	//gitter.MarchingCubes(0.5);
	gitter.RunAlgorithm(values2, 0.5);

	col = col_inc;
	glBegin(GL_TRIANGLES);	// Drawing Using Triangles
	for(it = gitter.TriangleList.begin(); it != gitter.TriangleList.end(); ++it) {
		glColor4f(0., 0., col, 0.5);
		glVertex3f(it->v1.x, it->v1.y, it->v1.z);
		glVertex3f(it->v2.x, it->v2.y, it->v2.z);
		glVertex3f(it->v3.x, it->v3.y, it->v3.z);
		col += col_inc;
	}
	glEnd();
	//gitter.find_filament(values, 0.5, 0.5, filament);
	////cout << "anzahl Segmente "<<filament.size()<<endl;
	////for (uint k=0;k<filament.size();k++){
	////cout <<"Segment "<< k << " = " << filament[k].vP0 << " " << filament[k].vP1 << endl;
	////}
	//glBegin(GL_LINES );		// Drawing Filament Using Lines
	//glColor3f(1.,1.,1.);	//! \attention filament is not shown in some areas when using glEnable(GL_DEPTH_TEST);
	//for(uint hhh = 0; hhh < filament.size(); hhh++)
	//{
	//glVertex3f( filament[hhh].vP0.x,filament[hhh].vP0.y,filament[hhh].vP0.z ); // Top
	//glVertex3f( filament[hhh].vP1.x,filament[hhh].vP1.y,filament[hhh].vP1.z ); // Top
	//}
	//glEnd();

	// Draw it to the screen
	SDL_GL_SwapBuffers();

	// Gather our frames per second
	Frames++;
	{
		GLint t = SDL_GetTicks();
		if(t - T0 >= 5000) {
			GLfloat seconds = (t - T0) / 1000.0;
			GLfloat fps = Frames / seconds;
			printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
			T0 = t;
			Frames = 0;
		}
	}

	return(TRUE);
}

/** name: main
 * \param argc: Number of commandline arguments.
 * \param argv: Pointer to the c-strings which contains the commandline arguments.
 * \return Integer.
 */
int main(int argc, char **argv) {
//	filament();

	//! Flags to pass to SDL_SetVideoMode
	int videoFlags;
	//! main loop variable
	int done = FALSE;
	//! used to collect events
	SDL_Event event;
	//! this holds some info about our display
	const SDL_VideoInfo *videoInfo;
	//! whether or not the window is active
	int isActive = TRUE;

	//! initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n",
		        SDL_GetError());
		Quit( 1 );
	}

	//! Fetch the video info
	videoInfo = SDL_GetVideoInfo();

	if(!videoInfo) {
		fprintf(stderr, "Video query failed: %s\n",
		        SDL_GetError());
		Quit(1);
	}

	//! the flags to pass to SDL_SetVideoMode
	videoFlags  = SDL_OPENGL;          //!< Enable OpenGL in SDL
	videoFlags |= SDL_GL_DOUBLEBUFFER; //!< Enable double buffering
	videoFlags |= SDL_HWPALETTE;       //!< Store the palette in hardware
	videoFlags |= SDL_RESIZABLE;       //!< Enable window resizing

	//! This checks to see if surfaces can be stored in memory
	if(videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	//! This checks if hardware blits can be done
	if ( videoInfo->blit_hw )
		videoFlags |= SDL_HWACCEL;

	//! Sets up OpenGL double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//! get a SDL surface
	surface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
	                           videoFlags);

	//! Verify there is a surface
	if(!surface) {
		fprintf(stderr,  "Video mode set failed: %s\n", SDL_GetError( ));
		Quit(1);
	}


	//! initialize OpenGL
	initGL();

	//! resize the initial window
	resizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

	//! wait for events
	while(!done) {
		//! handle the events in the queue

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_ACTIVEEVENT:
					/* Something's happend with our focus
					 * If we lost focus or we are iconified, we
					 * shouldn't draw the screen
					 */
					if ( event.active.gain == 0 )
						isActive = FALSE;
					else
						isActive = TRUE;
					break;
				case SDL_VIDEORESIZE:
					// handle resize event
					surface = SDL_SetVideoMode(event.resize.w,
					                           event.resize.h,
					                           16, videoFlags);
					if (!surface) {
						fprintf(stderr, "Could not get a surface after resize: %s\n", SDL_GetError());
						Quit( 1 );
					}
					resizeWindow( event.resize.w, event.resize.h );
					break;
				case SDL_KEYDOWN:
					// handle key presses
					handleKeyPress(&event.key.keysym);
					break;
				case SDL_QUIT:
					// handle quit requests
					done = TRUE;
					break;
				default:
					break;
			}
		}

		// draw the scene
		if(isActive)
			drawGLScene();
	}

	//! clean ourselves up and exit
	Quit( 0 );

	//! Should never get here
	return( 0 );
}
