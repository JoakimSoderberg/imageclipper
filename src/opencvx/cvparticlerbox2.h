/** @file
 * Rotated rectangle state particle filter +
 * 2nd order AR dynamics model ( in fact, next = current + speed + noise )
 *
 * Use this file as a template of definitions of states and 
 * state transition model for particle filter
 *
 * Currently cvparticle.h supports only linear combination of states transition
 * model only. you may create another cvParticleTransition to support more complex
 * non-linear state transition model. Most of other functions still should be 
 * available modifications
 *
 * The MIT License
 * 
 * Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CV_PARTICLE_ROTRECT2_H
#define CV_PARTICLE_ROTRECT2_H

#include "cvparticle.h"
#include "cvdrawrectangle.h"
#include "cvcropimageroi.h"
#include "cvrect32f.h"
#include <float.h>
using namespace std;

/********************** Definition of a particle *****************************/

int num_states = 10;

// Definition of meanings of 10 states.
// This kinds of structures is not necessary to be defined, 
// but I recommend to define them because it makes clear meanings of states
typedef struct CvParticleState {
    double x;        // center coord of a rectangle
    double y;        // center coord of a rectangle
    double width;    // width of a rectangle
    double height;   // height of a rectangle
    double angle;    // rotation around center. degree
    double xp;       // previous center coord of a rectangle
    double yp;       // previous center coord of a rectangle
    double widthp;   // previous width of a rectangle
    double heightp;  // previous height of a rectangle
    double anglep;   // previous rotation around center. degree
} CvParticleState;

// Definition of dynamics model
// new_particle = cvMatMul( dynamics, particle ) + noise
// curr_x =: curr_x + dx + noise = curr_x + (curr_x - prev_x) + noise
// prev_x =: curr_x
double dynamics[] = {
    2, 0, 0, 0, 0, -1, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 2, 0, 0, 0, 0, -1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
};

/********************** Function Prototypes *********************************/

// Functions for CvParticleState structure ( constructor, getter, setter )
inline CvParticleState cvParticleState( double x, double y, double width, double height, double angle = 0,
                                        double xp = 0, double yp = 0, double widthp = 0, double heightp = 0, double anglep =0 );
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id );
void cvParticleStateSet( const CvParticle* p, int p_id, CvParticleState &state );

// Particle Filter configuration
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std );
void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize );

// Utility Functions
void cvParticleStateDraw( const CvParticle* p, IplImage* frame, CvScalar color, int pid = -1 );
void cvParticleStatePrint( const CvParticleState& state );

/****************** Functions for CvParticleState structure ******************/

/**
 * Constructor
 */
inline CvParticleState cvParticleState( double x, double y, double width, double height, double angle,
                                        double xp, double yp, double widthp, double heightp, double anglep )
{
    CvParticleState state = { x, y, width, height, angle, 
                              xp, yp, widthp, heightp, anglep };
    return state;
}

/**
 * Get a state (particle)
 *
 * @param p         particle filter struct
 * @param p_id      particle id
 */
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id )
{
    CvParticleState s;
    s.x       = cvmGet( p->particles, 0, p_id );
    s.y       = cvmGet( p->particles, 1, p_id );
    s.width   = cvmGet( p->particles, 2, p_id );
    s.height  = cvmGet( p->particles, 3, p_id );
    s.angle   = cvmGet( p->particles, 4, p_id );
    s.xp      = cvmGet( p->particles, 5, p_id );
    s.yp      = cvmGet( p->particles, 6, p_id );
    s.widthp  = cvmGet( p->particles, 7, p_id );
    s.heightp = cvmGet( p->particles, 8, p_id );
    s.anglep  = cvmGet( p->particles, 9, p_id );
    return s;
}

/**
 * Set a state (particle)
 *
 * @param p         particle filter struct
 * @param p_id      particle id
 * @param state     A CvParticleState structure
 * @return void
 */
void cvParticleStateSet( const CvParticle* p, int p_id, CvParticleState &state )
{
    cvmSet( p->particles, 0, p_id, state.x );
    cvmSet( p->particles, 1, p_id, state.y );
    cvmSet( p->particles, 2, p_id, state.width );
    cvmSet( p->particles, 3, p_id, state.height );
    cvmSet( p->particles, 4, p_id, state.angle );
    cvmSet( p->particles, 5, p_id, state.xp );
    cvmSet( p->particles, 6, p_id, state.yp );
    cvmSet( p->particles, 7, p_id, state.widthp );
    cvmSet( p->particles, 8, p_id, state.heightp );
    cvmSet( p->particles, 9, p_id, state.anglep );
}

/*************************** Particle Filter Configuration *********************************/

/**
 * Configuration of Particle filter
 */
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std )
{
    // config dynamics model
    CvMat dynamicsmat = cvMat( p->num_states, p->num_states, CV_64FC1, dynamics );

    // config random noise standard deviation
    CvRNG rng = cvRNG( time( NULL ) );
    double stdarr[] = {
        std.x,
        std.y,
        std.width,
        std.height,
        std.angle,
        0,
        0,
        0,
        0,
        0
    };
    CvMat stdmat = cvMat( p->num_states, 1, CV_64FC1, stdarr );

    // config minimum and maximum values of states
    // lowerbound, upperbound, circular flag (useful for degree)
    // lowerbound == upperbound to express no bounding
    double boundarr[] = {
        0, imsize.width - 1, false,
        0, imsize.height - 1, false,
        1, imsize.width, false,
        1, imsize.height, false,
        0, 360, true,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    CvMat boundmat = cvMat( p->num_states, 3, CV_64FC1, boundarr );
    cvParticleSetDynamics( p, &dynamicsmat );
    cvParticleSetNoise( p, rng, &stdmat );
    cvParticleSetBound( p, &boundmat );
}

/**
 * @todo
 * CvParticle does not support this type of bounding currently
 * Call after transition
 */
void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize )
{
    for( int np = 0; np < p->num_particles; np++ ) 
    {
        double x      = cvmGet( p->particles, 0, np );
        double y      = cvmGet( p->particles, 1, np );
        double width  = cvmGet( p->particles, 2, np );
        double height = cvmGet( p->particles, 3, np );
        width = min( width, imsize.width - x ); // another state x is used
        height = min( height, imsize.height - y ); // another state y is used
        cvmSet( p->particles, 2, np, width );
        cvmSet( p->particles, 3, np, height );
    }
}

/***************************** Utility Functions ****************************************/

/**
 * Draw a particle or particles by rectangle on given image
 *
 * @param particle
 * @param img        image to be drawn
 * @param color      color of rectangle
 * @param [pid = -1] particle id. If -1, all particles are drawn
 */
void cvParticleStateDraw( const CvParticle* p, IplImage* img, CvScalar color, int pid )
{
    if( pid == -1 ) { // draw all particles
        int i;
        for( i = 0; i < p->num_particles; i++ ) {
            cvParticleStateDraw( p, img, color, i );
        }
    } else {
        CvRect32f rect32f;
        CvBox32f box32f;
        CvParticleState s = cvParticleStateGet( p, pid );
        box32f = cvBox32f( s.x, s.y, s.width, s.height, s.angle );
        rect32f = cvRect32fFromBox32f( box32f );
        cvDrawRectangle( img, rect32f, cvPoint2D32f(0,0), color );
    }
}

void cvParticleStatePrint( const CvParticleState& state )
{
    printf( "x :%f ", state.x );
    printf( "y :%f ", state.y );
    printf( "width :%f ", state.width );
    printf( "height :%f ", state.height );
    printf( "angle :%f\n", state.angle );
    printf( "xp:%f ", state.xp );
    printf( "yp:%f ", state.yp );
    printf( "widthp:%f ", state.widthp );
    printf( "heightp:%f ", state.heightp );
    printf( "anglep:%f\n", state.anglep );
    fflush( stdout );
}

#endif

